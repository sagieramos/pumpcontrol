#include "control.h"
#include "dev_or_prod.h"
#include "network.h"
#include "test_transcode.h"
#include <domsgid.h>

uint8_t buffer[1024];

void test_transcode_doId(const size_t client_id) {
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
  for (size_t i = 0; i < NUM_MSGS; i++) {
    DEBUG_SERIAL_PRINTLN("Serializing data...");
    size_t bytes_written;
    if (serialize_DoId(&doId[i], buffer, sizeof(buffer), &bytes_written)) {
      DEBUG_SERIAL_PRINTLN("Data serialized successfully");
      for (int i = 0; i < bytes_written; i++) {
        DEBUG_SERIAL_PRINTF("%02X ", buffer[i]);
      }
      DEBUG_SERIAL_PRINTLN();
      ws.binary(client_id, buffer, bytes_written);
    } else {
      DEBUG_SERIAL_PRINTLN("Failed to serialize data");
    }

    DEBUG_SERIAL_PRINTLN();

    DEBUG_SERIAL_PRINTLN("Deserializing data...");
    DoId doIdDeserialized = DoId_init_default;

    if (deserialize_DoId(buffer, bytes_written, &doIdDeserialized)) {
      DEBUG_SERIAL_PRINTLN("Data deserialized successfully");
      DEBUG_SERIAL_PRINTF("DoId %d: %f\n", doIdDeserialized.id,
                          doIdDeserialized.value);
    } else {
      DEBUG_SERIAL_PRINTLN("Failed to deserialize data");
    }
    memset(buffer, 0, sizeof(buffer));
    DEBUG_SERIAL_PRINTLN("............................................");
  }
}