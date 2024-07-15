/* #include <domsg.h> */
#include "main.h"
#include "network.h"
#include "test_transcode.h"
#include <unordered_map>

AsyncWebSocket ws("/ws");

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
  if (len > MAX_BUFFER_SIZE) {
    DEBUG_SERIAL_PRINTLN("Data too large");
    return;
  }

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

    /*     DoMessage doMessage;

        if (deserializeDoMessage(data, doMessage, len)) {
          // Send data to machine
          DEBUG_SERIAL_PRINTLN("Data received from client (DoMessage proto):");
          DEBUG_SERIAL_PRINTF("Instruction: %s\n", doMessage.inst);
          DEBUG_SERIAL_PRINTF("Value: %d\n", doMessage.value);
          machineDataRef = getControlData();
        } else
          DEBUG_SERIAL_PRINTLN("Failed to deserialize data");

        // Deserialize the received data
      } */
  }
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    size_t client_id = client->id();
    DEBUG_SERIAL_PRINTF("Websocket client connection received: %u\n",
                        client_id);

    test_transcode_doId(client_id);

    send_ctr_data(client_id);

    DEBUG_SERIAL_PRINTF("Clients online: %d\n", ws.count());

  } else if (type == WS_EVT_DISCONNECT) {
    if (deauthenticate(authClients, client)) {
      DEBUG_SERIAL_PRINTF("Websocket client connection closed: %u\n",
                          client->id());
    }

    DEBUG_SERIAL_PRINTF("Clients online: %d\n", ws.count());
  } else if (type == WS_EVT_ERROR) {
    DEBUG_SERIAL_PRINTLN("Websocket error");
  } else if (type == WS_EVT_PONG) {
    if (handlePong(authClients, client))
      DEBUG_SERIAL_PRINTF("Websocket pong: %u\n", client->id());
  } else if (WS_EVT_DATA) {
    DEBUG_SERIAL_PRINTLN("Websocket data received");
    handleRecieveData(authClients, client, data, len);
  } else {
    DEBUG_SERIAL_PRINTLN("Websocket event not handled");
  }
}