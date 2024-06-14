#include "main.h"
#include <map>

void generateSessionToken(char *token, size_t length) {
  for (size_t i = 0; i < length - 1; i++) {
    token[i] = 'A' + rand() % 26;
  }
  token[length - 1] = '\0';
}

void initializeClientSession(ClientSession &session) {
  generateSessionToken(session.token, TOKEN_LENGTH);
  session.startTime = millis();
  session.lastActive = session.startTime;
}

void initializeClientSessions(ClientSession *authenticatedClients) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    initializeClientSession(authenticatedClients[i]);
  }
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

ClientSession *getSessionFromRequest(AsyncWebServerRequest *request,
                                     ClientSession *authenticatedClients) {
  if (request->hasHeader("Cookie")) {
    String cookie = request->header("Cookie");
    int start = cookie.indexOf("_imuwahen=");
    if (start != -1) {
      start += String("_imuwahen=").length();
      int end = cookie.indexOf(';', start);
      if (end == -1)
        end = cookie.length();
      String token = cookie.substring(start, end);
      return findClientSession(authenticatedClients, token.c_str());
    }
  }
  return NULL;
}

void removeClientSession(ClientSession *authenticatedClients,
                         ClientSession &session) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (strcmp(authenticatedClients[i].token, session.token) == 0) {
      initializeClientSession(authenticatedClients[i]);
      break;
    }
  }
}

bool updateClientSession(ClientSession *session) {
  if (session != nullptr) {
    session->lastActive = millis();
    return true;
  }
  return false;
}

bool updateClientSessionByToken(ClientSession *authenticatedClients,
                                const char *token) {
  ClientSession *session = findClientSession(authenticatedClients, token);
  return updateClientSession(session);
}

bool isClientSessionActive(ClientSession *session) {
  if (session == NULL) {
    return false;
  }
  return millis() - session->lastActive < SESSION_TIMEOUT;
}

bool isClientSessionActive(ClientSession *authenticatedClients,
                           const char *token) {
  ClientSession *session = findClientSession(authenticatedClients, token);
  return session != NULL && isClientSessionActive(session);
}

AuthStatus authenticateClientSession(ClientSession *authenticatedClients,
                                     const char *token) {
  ClientSession *session = findClientSession(authenticatedClients, token);
  if (isClientSessionActive(session)) {
    return alreadyActive;
  }
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (authenticatedClients[i].lastActive > SESSION_TIMEOUT ||
        authenticatedClients[i].token[0] == '\0') {
      strcpy(authenticatedClients[i].token, token);
      updateClientSession(&authenticatedClients[i]);
      return authenticated;
    }
  }
  return sessionIsFull;
}

AuthStatus authenticateClientSession(AsyncWebServerRequest *request,
                                     ClientSession *authenticatedClients) {
  ClientSession *session = getSessionFromRequest(request, authenticatedClients);

  if (session) {
    if (isClientSessionActive(session)) {
      return alreadyActive;
    }
  } else {
    return noTokenProvided;
  }

  const char *token = session->token;
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (authenticatedClients[i].lastActive > SESSION_TIMEOUT ||
        authenticatedClients[i].token[0] == '\0') {
      strcpy(authenticatedClients[i].token, token);
      updateClientSession(&authenticatedClients[i]);
      return authenticated;
    }
  }
  return sessionIsFull;
}

AuthStatus checkAuthorization(AsyncWebServerRequest *request,
                              ClientSession *authenticatedClients) {
  ClientSession *session = getSessionFromRequest(request, authenticatedClients);
  if (session && isClientSessionActive(session)) {
    return alreadyActive;
  }
  return unauthorized;
}

void deauthenticateClientSession(ClientSession *authenticatedClients,
                                 const char *token) {
  ClientSession *session = findClientSession(authenticatedClients, token);
  if (session != NULL) {
    removeClientSession(authenticatedClients, *session);
  }
}
