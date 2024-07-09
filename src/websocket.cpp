#include "main.h"
#include <ArduinoJson.h>
#include <unordered_map>

AsyncWebSocket ws("/ws");
// AsyncEventSource events("/events");

std::unordered_map<size_t, size_t> clientIdToIndexMap;

bool authenticate(size_t index, const char *token, ClientSession *authClients,
                  uint32_t wsClientId) {
  int idx = static_cast<int>(index);

  ClientSession *session = findClientSessionByIndex(authClients, idx);

  bool stillActive = getCurrentTimeMs() - session->lastActive < SESSION_TIMEOUT;

  // Check if session exists and token matches
  if (session != NULL && strcmp(session->token, token) == 0 && stillActive) {
    // Update client ID in the authenticatedClients array
    authClients[idx].clientId = wsClientId;
    clientIdToIndexMap.insert({wsClientId, idx});
    return true;
  }

  return false;
}

bool deauthenticate(ClientSession *authClients, AsyncWebSocketClient *client) {
  size_t wsClientId = client->id();

  if (!authClients || !clientIdToIndexMap.count(wsClientId) || wsClientId == 0)
    return false;
  if (clientIdToIndexMap.empty())
    return false;

  auto it = clientIdToIndexMap.find(wsClientId);
  if (it != clientIdToIndexMap.end()) {
    size_t index = it->second;
    authClients[index].clientId = 0;
    clientIdToIndexMap.erase(it);
    client->close();
    return true;
  }
  return false;
}

bool handlePong(ClientSession *authClients, AsyncWebSocketClient *client) {
  size_t wsClientId = client->id();
  if (!authClients || !clientIdToIndexMap.count(wsClientId) || wsClientId == 0)
    return false;
  if (clientIdToIndexMap.empty())
    return false;

  auto it = clientIdToIndexMap.find(wsClientId);
  if (it != clientIdToIndexMap.end()) {
    size_t index = it->second;
    authClients[index].lastActive = getCurrentTimeMs();
    client->ping();
    return true;
  }
  return false;
}

void handleRecieveData(ClientSession *authClients, AsyncWebSocketClient *client,
                       uint8_t *data, size_t len) {
  size_t wsClientId = client->id();
  if (!authClients || !clientIdToIndexMap.count(wsClientId) || wsClientId == 0)
    return;
  if (clientIdToIndexMap.empty())
    return;

  auto it = clientIdToIndexMap.find(wsClientId);
  if (it != clientIdToIndexMap.end()) {
    size_t index = it->second;
    authClients[index].lastActive = getCurrentTimeMs();

    controlData &machineDataRef = getControlData();

    // Deserialize the received data
    if (deserializeMachineData(reinterpret_cast<const char *>(data),
                               machineDataRef)) {
      DEBUG_SERIAL_PRINTLN("Data deserialized");
      DEBUG_SERIAL_PRINTF("Mode: %d\n", static_cast<int>(machineDataRef.mode));
      DEBUG_SERIAL_PRINTF("Running: %u\n", machineDataRef.timer.running);
      DEBUG_SERIAL_PRINTF("Resting: %u\n", machineDataRef.timer.resting);
    } else {
      DEBUG_SERIAL_PRINTLN("Data deserialization failed");
    }
  }
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
  case WS_EVT_CONNECT:
    DEBUG_SERIAL_PRINTF("Websocket client connection received: %u\n",
                        client->id());
    if (len > 0) {
      JsonDocument doc;
      ArduinoJson::DeserializationError error = deserializeJson(
          doc, data, len); // Specify the namespace for DeserializationError
      if (!error) {
        size_t index = atoi(doc[INDEX_ATTR]);
        const char *token = doc[TOKEN_ATTR];
        if (authenticate(index, token, authClients, client->id())) {
          const controlData &machineData = getControlData();
          String machineConfig =
              serializeMachineData(machineData.mode, machineData.timer.running,
                                   machineData.timer.resting);
          // Send machine data to authenticated client
          client->text(machineConfig);

          int clientsOnline = static_cast<int>(ws.count());

          DEBUG_SERIAL_PRINTF("Clients online: %d\n", clientsOnline);

          JsonDocument online;
          online["online"] = clientsOnline;
          String clientCount;
          serializeJson(online, clientCount);

          // Send number of available clients to all authenticated clients
          ws.textAll(clientCount);

          DEBUG_SERIAL_PRINTLN("Authentication successful");
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
    if (deauthenticate(authClients, client)) {
      DEBUG_SERIAL_PRINTF("Websocket client connection closed: %u\n",
                          client->id());
    }

    DEBUG_SERIAL_PRINTF("Clients online: %d\n", ws.count());
    break;
  case WS_EVT_ERROR:
    DEBUG_SERIAL_PRINTLN("Websocket error");
    break;
  case WS_EVT_PONG:
    if (handlePong(authClients, client))
      DEBUG_SERIAL_PRINTF("Websocket pong: %u\n", client->id());
    break;
  case WS_EVT_DATA:
    DEBUG_SERIAL_PRINTLN("Websocket data received");
    handleRecieveData(authClients, client, data, len);
    break;
  default:
    break;
  }
}

void setupWebsocket() {
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  // server.addHandler(&events);
}
