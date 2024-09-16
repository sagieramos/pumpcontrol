#ifndef SESSION_H
#define SESSION_H

#include <ESPAsyncWebServer.h>
#include <cstdint>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

constexpr uint8_t MAX_CLIENTS =
    8; // Maximum number of clients that can be AUTHENTICATED
constexpr size_t TOKEN_LENGTH = 9; // 8 characters + null terminator
constexpr size_t SESSION_TIMEOUT =
    (59 * 60 * 1000) + (59 * 1000); // 59 minutes and 59 seconds
constexpr const char *TOKEN_ATTR = "_imuwahen";
constexpr const char *INDEX_ATTR = "_idx";

struct PinGen {
  unsigned long gen = 0;
  uint8_t pin[4];

  PinGen();
  void regenerate();
};

extern PinGen pinGen;

struct ClientSession {
  char token[TOKEN_LENGTH]; // Session token
  unsigned long startTime;  // Start time of the session
  unsigned long lastActive; // Last ACTIVE time of the session
  unsigned int index;       // Index of the session in the session array
  uint32_t clientId; // Client ID of the session in the WebSocket client array

  ClientSession() {
    memset(token, 0, TOKEN_LENGTH);
    startTime = 0;
    lastActive = 0;
    index = 0;
    clientId = 0;
  }

  ClientSession(const ClientSession &session) {
    strncpy(token, session.token, TOKEN_LENGTH);
    startTime = session.startTime;
    lastActive = session.lastActive;
    index = session.index;
    clientId = session.clientId;
  }

  ClientSession &operator=(const ClientSession &session) {
    if (this == &session) {
      return *this;
    }
    strncpy(token, session.token, TOKEN_LENGTH);
    startTime = session.startTime;
    lastActive = session.lastActive;
    index = session.index;
    clientId = session.clientId;
    return *this;
  }

  bool operator==(const ClientSession &session) const {
    return strcmp(token, session.token) == 0 &&
           startTime == session.startTime && lastActive == session.lastActive &&
           index == session.index && clientId == session.clientId;
  }

  bool operator!=(const ClientSession &session) const {
    return !(*this == session);
  }

  // Method to check if the session is null or uninitialized
  bool isNull() const {
    return token[0] == '\0' && startTime == 0 && lastActive == 0 &&
           index == 0 && clientId == 0;
  }

  // Boolean conversion operator to check if the session is null
  explicit operator bool() const { return !isNull(); }
};

enum AuthStatus {
  NOT_ACTIVE,
  ACTIVE,
  AUTHENTICATED,
  SESSION_IS_FULL,
  NO_TOKEN_PROVIDED,
  UNAUTHORIZED,
  DEAUTHENTICATED
};

extern ClientSession authClients[MAX_CLIENTS];

enum authAction { LOGIN, LOGOUT, CHECK };

AuthStatus authSession(ClientSession *authClients,
                       AsyncWebServerRequest *request, ClientSession &session,
                       authAction action);
AuthStatus authSession(ClientSession *authClients,
                       AsyncWebServerRequest *request, authAction action);

#endif // SESSION_H