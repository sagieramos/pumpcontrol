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

ClientSession *findClientSession(ClientSession *sessions,
                                 const char *token) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (strcmp(sessions[i].token, token) == 0) {
      return &sessions[i];
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

ClientSession *getSessionFromRequest(AsyncWebServerRequest *request,
                                     ClientSession *sessions) {
  DEBUG_SERIAL_PRINTLN(".................getSession2");

  if (!request->hasHeader("Cookie")) {
    return NULL;
  }

  DEBUG_SERIAL_PRINTLN(".................getSession3");
  String cookieHeader = request->header("Cookie");
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
      if (strcmp(sessions[index].token, token) == 0) {
        return &sessions[index];
      }
    }
  }

  return NULL;
}

AuthStatus removeSession(ClientSession *sessions,
                         ClientSession &session) {
  if (session.token[0] == '\0') {
    return NO_TOKEN_PROVIDED;
  }
  unsigned long now = millis();
  if (now - session.lastActive < SESSION_TIMEOUT) {
    size_t index = session.index;
    sessions[index].token[0] = '\0';
    sessions[index].startTime = 0;
    sessions[index].lastActive = 0;
    return AUTH_DEAUTHENTICATED;
  }

  return AUTH_UNAUTHORIZED;
}

AuthStatus createSession(ClientSession *sessions,
                         ClientSession &session) {
  unsigned long now = millis();

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (sessions[i].lastActive == 0 ||
        (now - sessions[i].lastActive > SESSION_TIMEOUT)) {

      char token[TOKEN_LENGTH];
      generateSessionToken(token, TOKEN_LENGTH);

      sessions[i].startTime = now;
      sessions[i].lastActive = now;
      sessions[i].index = i;
      strncpy(sessions[i].token, token, TOKEN_LENGTH);

      session = sessions[i];
      return AUTH_AUTHENTICATED;
    }
  }

  return AUTH_SESSION_FULL;
}

AuthStatus checkAuth(ClientSession &session) {
  if (&session == NULL) {
    return AUTH_NOT_ACTIVE;
  }
  if (session.token[0] == '\0') {
    return NO_TOKEN_PROVIDED;
  }
  unsigned long now = millis();
  if (now - session.lastActive < SESSION_TIMEOUT) {
    session.lastActive = now;
    return AUTH_ACTIVE;
  }
  if (now - session.startTime >= SESSION_TIMEOUT) {
    return AUTH_NOT_ACTIVE;
  }
  return AUTH_UNAUTHORIZED;
}

AuthStatus authSession(ClientSession *sessions,
                       AsyncWebServerRequest *request, authAction action,
                       ClientSession &session) {
  ClientSession *getSession =
      getSessionFromRequest(request, sessions);
  unsigned long now = millis();

  if (getSession && now - getSession->lastActive < SESSION_TIMEOUT &&
      action == LOGIN_ACTION) {
    return AUTH_UNAUTHORIZED;
  } else if (!getSession && action != LOGIN_ACTION) {
    return AUTH_UNAUTHORIZED;
  }

  if (action == CHECK_ACTION) {
    return checkAuth(*getSession);
  } else if (action == LOGIN_ACTION) {
    return createSession(sessions, session);
  } else if (action == LOGOUT_ACTION) {
    return removeSession(sessions, *getSession);
  }

  return AUTH_UNAUTHORIZED;
}

AuthStatus authSession(ClientSession *sessions,
                       AsyncWebServerRequest *request, authAction action) {
  ClientSession *getSession =
      getSessionFromRequest(request, sessions);
  unsigned long now = millis();

  if (getSession && now - getSession->lastActive < SESSION_TIMEOUT &&
      action == LOGIN_ACTION) {
    return AUTH_UNAUTHORIZED;
  } else if (!getSession && action != LOGIN_ACTION) {
    return AUTH_UNAUTHORIZED;
  }

  switch (action) {
  case CHECK_ACTION:
    return checkAuth(*getSession);
  case LOGIN_ACTION:
    return createSession(sessions, *getSession);
  case LOGOUT_ACTION:
    return removeSession(sessions, *getSession);
  default:
    return AUTH_UNAUTHORIZED;
  }
}
