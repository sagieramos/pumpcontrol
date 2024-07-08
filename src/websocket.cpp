#include "main.h"
#include <ArduinoJson.h>

AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

bool authenticate(size_t index, const char *token,
                  ClientSession *authenticatedClients, uint32_t wsClientId) {
  // Convert index to integer
  int idx = index;

  // Find client session by index
  ClientSession *session = findClientSessionByIndex(authenticatedClients, idx);

  bool stillActive = getCurrentTimeMs() - session->lastActive < SESSION_TIMEOUT;

  // Check if session exists and token matches
  if (session != NULL && strcmp(session->token, token) == 0 && stillActive) {
    // Update client ID in the authenticatedClients array
    authenticatedClients[idx].clientId = wsClientId;
    return true; // Authentication successful
  }

  return false; // Authentication failed
}

bool deauthenticate(size_t index, const char *token,
                    ClientSession *authenticatedClients, uint32_t wsClientId) {
  // Convert index to integer
  int idx = index;

  // Find client session by index
  ClientSession *session = findClientSessionByIndex(authenticatedClients, idx);

  bool stillActive = getCurrentTimeMs() - session->lastActive < SESSION_TIMEOUT;

  // Check if session exists and token matches
  if (session != NULL && strcmp(session->token, token) == 0 && stillActive) {
    // Update client ID in the authenticatedClients array
    authenticatedClients[idx].clientId = 0;
    return true; // Deauthentication successful
  }

  return false; // Deauthentication failed
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
  case WS_EVT_CONNECT:
    DEBUG_SERIAL_PRINTLN("Websocket client connection received");
    if (len > 0) {
      DynamicJsonDocument doc(1024);
      ArduinoJson::DeserializationError error = deserializeJson(
          doc, data, len); // Specify the namespace for DeserializationError
      if (!error) {
        size_t index = atoi(doc["_idx"]);
        const char *token = doc["_tkn"];
        if (authenticate(index, token, authenticatedClients, client->id())) {
          DEBUG_SERIAL_PRINTLN("Authentication successful");
          // Proceed with handling authenticated client data
        } else {
          DEBUG_SERIAL_PRINTLN("Authentication failed");
          client->close();
        }
      } else {
        DEBUG_SERIAL_PRINTLN("Invalid JSON data");
        client->close();
      }
    }
    break;
  case WS_EVT_DISCONNECT:
    DEBUG_SERIAL_PRINTLN("Websocket client connection closed");
    break;
  case WS_EVT_ERROR:
    DEBUG_SERIAL_PRINTLN("Websocket error");
    break;
  case WS_EVT_PONG:
    DEBUG_SERIAL_PRINTLN("Websocket pong");
    break;
  case WS_EVT_DATA:
    DEBUG_SERIAL_PRINTLN("Websocket data received");
    break;
  default:
    break;
  }
}

void setupWebsocket() {
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.addHandler(&events);
}

void sendWsToAllAuthenticatedClients(const char *message) {
  for (size_t i = 0; i < MAX_CLIENTS; i++) {
    if (authenticatedClients[i].clientId != 0) {
      ws.text(authenticatedClients[i].clientId, message);
    }
  }
}
