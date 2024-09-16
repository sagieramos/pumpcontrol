#include "session.h"
#include "dev_or_prod.h"
#include "main.h"

constexpr const char *PIN = "1234";

PinGen::PinGen() : gen(0) { regenerate(); }

void PinGen::regenerate() {
  srand(time(NULL));
  gen = (rand() % (9999999999UL - 1000000000UL + 1)) + 1000000000UL;

  char pin_str[11];

  sprintf(pin_str, "%lu", gen);

  for (int i = 0; i < 4; i++) {
    int k = pin_str[i] - pin_str[7 - i];
    pin[i] = static_cast<uint8_t>(abs(k));
  }

  LOG_F("Generated pin: %s\n", pin_str);
  LOG_F("pin: %d%d%d%d\n", pin[0], pin[1], pin[2], pin[3]);
}

PinGen pinGen;

const char CHARSET[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

static void generateSessionToken(char *token, size_t length) {
  size_t charset_size = sizeof(CHARSET) - 1;

  static int seeded = 0;
  if (!seeded) {
    srand(time(NULL));
    seeded = 1;
  }

  for (size_t i = 0; i < length - 1; i++) {
    token[i] = CHARSET[rand() % charset_size];
  }
  token[length - 1] = '\0';
}

ClientSession *findClientSession(ClientSession *authClients,
                                 const char *token) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (strcmp(authClients[i].token, token) == 0) {
      return &authClients[i];
    }
  }
  return NULL;
}

ClientSession *findClientSessionByIndex(ClientSession *authClients,
                                        size_t index) {
  if (index >= MAX_CLIENTS) {
    return NULL;
  }
  return &authClients[index];
}

static bool extractCookieAttribute(const char *cookie, const char *attribute,
                                   char *value, size_t value_size) {
  // Create a temporary buffer to hold the attribute with the "="
  char attribute_with_equal[strlen(attribute) +
                            2]; // +1 for '=' and +1 for null terminator
  snprintf(attribute_with_equal, sizeof(attribute_with_equal),
           "%s=", attribute);

  const char *attribute_start = strstr(cookie, attribute_with_equal);
  if (!attribute_start) {
    LOG_F("Attribute not found\n");
    return false;
  }

  attribute_start += strlen(attribute_with_equal);
  const char *attribute_end = strchr(attribute_start, ';');
  if (!attribute_end) {
    LOG_F("Attribute end not found\n");
    attribute_end = cookie + strlen(cookie);
  }

  size_t attribute_length = attribute_end - attribute_start;
  if (attribute_length >= value_size) {
    LOG_F("Attribute length is too long: %d\n", attribute_length);
    return false;
  }

  strncpy(value, attribute_start, attribute_length);
  value[attribute_length] = '\0';

  LOG_F("Attribute name: %s, value: %s\n", attribute, value);
  return true;
}

static ClientSession *getSessionFromRequest(const String &cookieHeader,
                                            ClientSession *authClients) {
  if (cookieHeader.length() == 0) {
    return NULL;
  }
  const char *cookie = cookieHeader.c_str();

  LOG_F("Cookies: %s\n", cookie);

  char token[TOKEN_LENGTH];
  char index_str[11]; // 11 characters to fit any integer, including null
                      // terminator.
  size_t index;

  if (extractCookieAttribute(cookie, TOKEN_ATTR, token, TOKEN_LENGTH) &&
      extractCookieAttribute(cookie, INDEX_ATTR, index_str,
                             sizeof(index_str))) {
    index = atoi(index_str);

    if (index >= 0 && index < MAX_CLIENTS) {
      if (strcmp(authClients[index].token, token) == 0) {
        return &authClients[index];
      }
    }
  }

  return NULL;
}

AuthStatus removeSession(ClientSession *authClients,
                         const ClientSession &session) {
  if (session.token[0] == '\0') {
    return NO_TOKEN_PROVIDED;
  }
  unsigned long now = getCurrentTimeMs();
  if (now - session.lastActive < SESSION_TIMEOUT) {
    size_t index = session.index;
    authClients[index].token[0] = '\0';
    authClients[index].startTime = 0;
    authClients[index].lastActive = 0;
    return DEAUTHENTICATED;
  }

  return UNAUTHORIZED;
}

static AuthStatus createSession(ClientSession *authClients,
                                ClientSession &session) {
  unsigned long now = getCurrentTimeMs();

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (authClients[i].lastActive == 0 ||
        (now - authClients[i].lastActive > SESSION_TIMEOUT)) {

      char token[TOKEN_LENGTH];
      generateSessionToken(token, TOKEN_LENGTH);

      authClients[i].startTime = now;
      authClients[i].lastActive = now;
      authClients[i].index = i;
      strncpy(authClients[i].token, token, TOKEN_LENGTH);

      session = authClients[i];
      return AUTHENTICATED;
    }
  }

  return SESSION_IS_FULL;
}

static AuthStatus checkAuth(ClientSession &session) {
  if (&session == NULL) {
    return NOT_ACTIVE;
  }
  if (session.token[0] == '\0') {
    return NO_TOKEN_PROVIDED;
  }
  unsigned long now = getCurrentTimeMs();
  if (now - session.lastActive < SESSION_TIMEOUT) {
    session.lastActive = now;
    return ACTIVE;
  }
  if (now - session.startTime >= SESSION_TIMEOUT) {
    return NOT_ACTIVE;
  }
  return UNAUTHORIZED;
}

AuthStatus authSession(ClientSession *authClients,
                       AsyncWebServerRequest *request, ClientSession &session,
                       authAction action) {
  if (action == LOGIN) {
    if (!request->hasParam("pin", true)) {
      return UNAUTHORIZED;
    }
    char pinStr[5];
    snprintf(pinStr, sizeof(pinStr), "%d%d%d%d", pinGen.pin[0], pinGen.pin[1],
             pinGen.pin[2], pinGen.pin[3]);
    return strcmp(request->getParam("pin", true)->value().c_str(), pinStr) == 0
               ? createSession(authClients, session)
               : UNAUTHORIZED;
  } else if (action == LOGOUT) {
    return removeSession(authClients, session);
  } else if (action == CHECK) {
    return checkAuth(session);
  }
  return UNAUTHORIZED;
}

AuthStatus authSession(ClientSession *authClients,
                       AsyncWebServerRequest *request, authAction action) {
  String cookieHeader;
  ClientSession *getSession = nullptr;

  // For actions other than LOGIN, check for cookies and get session
  if (action != LOGIN) {
    if (!request->hasHeader("Cookie"))
      return UNAUTHORIZED;

    cookieHeader = request->header("Cookie");
    getSession = getSessionFromRequest(cookieHeader, authClients);

    if (!getSession ||
        getCurrentTimeMs() - getSession->lastActive >= SESSION_TIMEOUT)
      return UNAUTHORIZED;
  }

  switch (action) {
  case CHECK:
    return checkAuth(*getSession);

  case LOGIN: {
    ClientSession newSession;
    AuthStatus status = createSession(authClients, newSession);
    if (status == AUTHENTICATED) {
      *getSession = newSession;
    }
    return status;
  }

  case LOGOUT:
    return removeSession(authClients, *getSession);

  default:
    return UNAUTHORIZED;
  }
}
