#include "main.h"

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

ClientSession *findClientSession(ClientSession *authenticatedClients,
                                 const char *token) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (strcmp(authenticatedClients[i].token, token) == 0) {
      return &authenticatedClients[i];
    }
  }
  return NULL;
}

bool extractCookieAttribute(const char *cookie, const char *attribute,
                            char *value, size_t value_size) {
  const char *attribute_start = strstr(cookie, attribute);
  if (!attribute_start) {
    return false;
  }

  attribute_start += strlen(attribute);
  const char *attribute_end = strchr(attribute_start, ';');
  if (!attribute_end) {
    attribute_end = cookie + strlen(cookie);
  }

  size_t attribute_length = attribute_end - attribute_start;
  if (attribute_length >= value_size) {
    return false;
  }

  strncpy(value, attribute_start, attribute_length);
  value[attribute_length] = '\0';

  return true;
}

ClientSession *getSessionFromRequest(const String &cookieHeader,
                                     ClientSession *authenticatedClients) {
  if (cookieHeader.length() == 0) {
    return NULL;
  }
  const char *cookie = cookieHeader.c_str();

  DEBUG_SERIAL_PRINTF("Cookies: %s\n", cookie);

  char token[TOKEN_LENGTH];
  char index_str[11]; // 11 characters to fit any integer, including null
                      // terminator.
  size_t index;

  if (extractCookieAttribute(cookie, "_imuwahen=", token, TOKEN_LENGTH) &&
      extractCookieAttribute(cookie, "_index=", index_str, sizeof(index_str))) {
    index = atoi(index_str);

    if (index >= 0 && index < MAX_CLIENTS) {
      if (strcmp(authenticatedClients[index].token, token) == 0) {
        return &authenticatedClients[index];
      }
    }
  }

  return NULL;
}

AuthStatus removeSession(ClientSession *authenticatedClients,
                         const ClientSession &session) {
  if (session.token[0] == '\0') {
    return noTokenProvided;
  }
  unsigned long now = getCurrentTimeMs();
  if (now - session.lastActive < SESSION_TIMEOUT) {
    size_t index = session.index;
    authenticatedClients[index].token[0] = '\0';
    authenticatedClients[index].startTime = 0;
    authenticatedClients[index].lastActive = 0;
    return deauthenticated;
  }

  return unauthorized;
}

AuthStatus createSession(ClientSession *authenticatedClients,
                         ClientSession &session) {
  unsigned long now = getCurrentTimeMs();

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (authenticatedClients[i].lastActive == 0 ||
        (now - authenticatedClients[i].lastActive > SESSION_TIMEOUT)) {

      char token[TOKEN_LENGTH];
      generateSessionToken(token, TOKEN_LENGTH);

      authenticatedClients[i].startTime = now;
      authenticatedClients[i].lastActive = now;
      authenticatedClients[i].index = i;
      strncpy(authenticatedClients[i].token, token, TOKEN_LENGTH);

      session = authenticatedClients[i];
      return authenticated;
    }
  }

  return sessionIsFull;
}

AuthStatus checkAuth(ClientSession &session) {
  if (&session == NULL) {
    return notActive;
  }
  if (session.token[0] == '\0') {
    return noTokenProvided;
  }
  unsigned long now = getCurrentTimeMs();
  if (now - session.lastActive < SESSION_TIMEOUT) {
    session.lastActive = now;
    return active;
  }
  if (now - session.startTime >= SESSION_TIMEOUT) {
    return notActive;
  }
  return unauthorized;
}

AuthStatus authSession(ClientSession *authenticatedClients,
                       AsyncWebServerRequest *request, ClientSession &session,
                       authAction action) {
  if (!request->hasHeader("Cookie")) {
    return unauthorized;
  }

  String cookieHeader = request->header("Cookie");

  ClientSession *getSession =
      getSessionFromRequest(cookieHeader, authenticatedClients);
  unsigned long now = getCurrentTimeMs();

  if (getSession && now - getSession->lastActive < SESSION_TIMEOUT &&
      action == LOGIN) {
    return unauthorized;
  } else if (!getSession && action != LOGIN) {
    return unauthorized;
  }

  if (action == CHECK) {
    return checkAuth(*getSession);
  } else if (action == LOGIN) {
    return createSession(authenticatedClients, session);
  } else if (action == LOGOUT) {
    return removeSession(authenticatedClients, *getSession);
  }

  return unauthorized;
}

AuthStatus authSession(ClientSession *authenticatedClients,
                       AsyncWebServerRequest *request, authAction action) {
  if (!request->hasHeader("Cookie")) {
    return unauthorized;
  }

  String cookieHeader = request->header("Cookie");

  ClientSession *getSession =
      getSessionFromRequest(cookieHeader, authenticatedClients);
  unsigned long now = getCurrentTimeMs();

  if (getSession && now - getSession->lastActive < SESSION_TIMEOUT &&
      action == LOGIN) {
    return unauthorized;
  } else if (!getSession && action != LOGIN) {
    return unauthorized;
  }

  switch (action) {
  case CHECK:
    return checkAuth(*getSession);
  case LOGIN:
    return createSession(authenticatedClients, *getSession);
  case LOGOUT:
    return removeSession(authenticatedClients, *getSession);
  default:
    return unauthorized;
  }
}
