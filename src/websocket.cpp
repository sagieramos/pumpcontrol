/* #include <domsg.h> */
#include "main.h"
#include <domsgid.h>
#include <unordered_map>

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
    DEBUG_SERIAL_PRINTF("Websocket client connection received: %u\n",
                        client->id());
    // Get initial data from client

    const size_t NUM_MSGS = 10;
    const controlData &machineData = getControlData();

    DoId doId[NUM_MSGS] = {{1, static_cast<float>(machineData.mode)},
                           {2, static_cast<float>(machineData.timer.resting)},
                           {3, static_cast<float>(machineData.timer.running)},
                           {4, 232.13f},
                           {5, 6765.32f},
                           {6, 0.2f},
                           {7, 7.0f},
                           {8, 0.3f},
                           {9, 9.0f},
                           {10, 10.0f}};
    DEBUG_SERIAL_PRINTLN("Data to be serialized:");
    for (size_t i = 0; i < NUM_MSGS; i++) {
      DEBUG_SERIAL_PRINTF("DoId %d: %f\n", doId[i].id, doId[i].value);
    }
    DEBUG_SERIAL_PRINTLN();

    DEBUG_SERIAL_PRINTLN("Serializing data...");
    uint8_t buffer[256];
    int bytes_written = serialize_do_id(buffer, sizeof(buffer), doId, NUM_MSGS);
    if (bytes_written > 0) {
      DEBUG_SERIAL_PRINTLN("Data serialized successfully");
      for (int i = 0; i < bytes_written; i++) {
        DEBUG_SERIAL_PRINT(buffer[i]);
      }
      DEBUG_SERIAL_PRINTLN();
      DEBUG_SERIAL_PRINTLN();
      client->text(buffer, bytes_written);
    } else {
      DEBUG_SERIAL_PRINTLN("Failed to serialize data");
    }

    DEBUG_SERIAL_PRINTLN();

    DEBUG_SERIAL_PRINTLN("Deserializing data...");
    uint8_t buffer2[1024];
    size_t bytes_read = 0;
    DoId doId2[10];
    size_t num_of_objs =
        deserialize_do_id(buffer, sizeof(buffer), bytes_read, doId2, 10);
    if (num_of_objs > 0) {
      DEBUG_SERIAL_PRINTLN("Data deserialized successfully");
      for (size_t i = 0; i < num_of_objs; i++) {
        DEBUG_SERIAL_PRINTF("DoId %d: %f\n", doId2[i].id, doId2[i].value);
      }
    } else {
      DEBUG_SERIAL_PRINTLN("Failed to deserialize data");
    }

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

// create virtual V1 to V10 use to recieve websocket data from client
