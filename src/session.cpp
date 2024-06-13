#include "main.h"
#include <map>
#define SESSION_TIMEOUT 600000

struct ClientSession {
  char token[TOKEN_LENGTH]; // Session token
  unsigned long startTime;  // Start time of the session
  unsigned long lastActive; // Last active time of the session
};

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
    int start = cookie.indexOf("session_token=");
    if (start != -1) {
      start += String("session_token=").length();
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

void updateClientSession(ClientSession &session) {
  session.lastActive = millis();
}

void updateClientSession(ClientSession *authenticatedClients,
                         const char *token) {
  ClientSession *session = findClientSession(authenticatedClients, token);
  if (session != NULL) {
    updateClientSession(*session);
  }
}

bool isClientSessionActive(ClientSession &session) {
  return millis() - session.lastActive < SESSION_TIMEOUT;
}

bool isClientSessionActive(ClientSession *authenticatedClients,
                           const char *token) {
  ClientSession *session = findClientSession(authenticatedClients, token);
  return session != NULL && isClientSessionActive(*session);
}

void authenticateClientSession(ClientSession *authenticatedClients,
                               const char *token) {
  ClientSession *session = findClientSession(authenticatedClients, token);
  if (session != NULL) {
    updateClientSession(*session);
  } else {
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (authenticatedClients[i].lastActive == 0) {
        strcpy(authenticatedClients[i].token, token);
        updateClientSession(authenticatedClients[i]);
        break;
      }
    }
  }
}

void deauthenticateClientSession(ClientSession *authenticatedClients,
                                 const char *token) {
  ClientSession *session = findClientSession(authenticatedClients, token);
  if (session != NULL) {
    removeClientSession(authenticatedClients, *session);
  }
}
