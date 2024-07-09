#include "main.h"
#include <map>

std::map<std::string, size_t> tokenToIndexMap;

const char CHARSET[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

void generateSessionToken(char *token, size_t length) {
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

bool extractCookieAttribute(const char *cookie, const char *attribute,
                            char *value, size_t value_size) {
  // Create a temporary buffer to hold the attribute with the "="
  char attribute_with_equal[strlen(attribute) +
                            2]; // +1 for '=' and +1 for null terminator
  snprintf(attribute_with_equal, sizeof(attribute_with_equal),
           "%s=", attribute);

  const char *attribute_start = strstr(cookie, attribute_with_equal);
  if (!attribute_start) {
    DEBUG_SERIAL_PRINTF("Attribute not found\n");
    return false;
  }

  attribute_start += strlen(attribute_with_equal);
  const char *attribute_end = strchr(attribute_start, ';');
  if (!attribute_end) {
    DEBUG_SERIAL_PRINTF("Attribute end not found\n");
    attribute_end = cookie + strlen(cookie);
  }

  size_t attribute_length = attribute_end - attribute_start;
  if (attribute_length >= value_size) {
    DEBUG_SERIAL_PRINTF("Attribute length is too long: %d\n", attribute_length);
    return false;
  }

  strncpy(value, attribute_start, attribute_length);
  value[attribute_length] = '\0';

  DEBUG_SERIAL_PRINTF("Attribute name: %s, value: %s\n", attribute, value);
  return true;
}

ClientSession *getSessionFromRequest(const String &cookieHeader,
                                     ClientSession *authClients) {
  if (cookieHeader.length() == 0) {
    return NULL;
  }
  const char *cookie = cookieHeader.c_str();

  DEBUG_SERIAL_PRINTF("Cookies: %s\n", cookie);

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

AuthStatus createSession(ClientSession *authClients, ClientSession &session) {
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

AuthStatus checkAuth(ClientSession &session) {
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
  if (!request->hasHeader("Cookie")) {
    return UNAUTHORIZED;
  }

  String cookieHeader = request->header("Cookie");

  ClientSession *getSession = getSessionFromRequest(cookieHeader, authClients);
  unsigned long now = getCurrentTimeMs();

  if (getSession && now - getSession->lastActive < SESSION_TIMEOUT &&
      action == LOGIN)
    return UNAUTHORIZED;

  if (!getSession && action != LOGIN)
    return UNAUTHORIZED;

  switch (action) {
  case CHECK:
    return checkAuth(*getSession);
  case LOGIN:
    return createSession(authClients, session);
  case LOGOUT:
    return removeSession(authClients, *getSession);
  default:
    return UNAUTHORIZED;
    break;
  }
}

AuthStatus authSession(ClientSession *authClients,
                       AsyncWebServerRequest *request, authAction action) {
  if (!request->hasHeader("Cookie"))
    return UNAUTHORIZED;

  String cookieHeader = request->header("Cookie");

  ClientSession *getSession = getSessionFromRequest(cookieHeader, authClients);

  if (getSession &&
      getCurrentTimeMs() - getSession->lastActive < SESSION_TIMEOUT &&
      action == LOGIN)
    return UNAUTHORIZED;

  if (!getSession && action != LOGIN)
    return UNAUTHORIZED;

  switch (action) {
  case CHECK:
    return checkAuth(*getSession);
  case LOGIN:
    return createSession(authClients, *getSession);
  case LOGOUT:
    return removeSession(authClients, *getSession);
  default:
    return UNAUTHORIZED;
    break;
  }
}
