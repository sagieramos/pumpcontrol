#include "main.h"

uint32_t getCurrentTimeMs() {
  TickType_t currentTick = xTaskGetTickCount();
  uint32_t currentTimeMs = currentTick * portTICK_PERIOD_MS;
  return currentTimeMs;
}

// POWER_OFF, POWER_ON, AUTO

String serializeMachineData(MachineMode mode, unsigned int running,
                            unsigned int resting, const char *objectName) {
  // Use DynamicJsonDocument with appropriate size
  JsonDocument doc;

  // Create nested object correctly
  JsonObject innerObject = doc.createNestedObject(objectName);
  innerObject["mode"] = static_cast<int>(mode);
  innerObject["running"] = running;
  innerObject["resting"] = resting;

  String output;
  serializeJson(doc, output); // Serialize the entire document
  return output;
}

bool deserializeMachineData(const char *jsonString, controlData &machineData,
                            const char *objectName) {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    DEBUG_SERIAL_PRINTF("Error deserializing JSON: %s\n", error.f_str());
    return false;
  }

  JsonObject obj = doc[objectName];
  if (!obj.isNull()) {
    // Update mode if the key exists
    if (obj.containsKey("mode")) {
      machineData.mode = static_cast<MachineMode>(obj["mode"].as<int>());
    }
    // Update running if the key exists
    if (obj.containsKey("running")) {
      machineData.timer.running = obj["running"];
    }
    // Update resting if the key exists
    if (obj.containsKey("resting")) {
      machineData.timer.resting = obj["resting"];
    }

    ws.textAll(jsonString);
    DEBUG_SERIAL_PRINTLN("Machine data updated");
    DEBUG_SERIAL_PRINTF("%s\n", jsonString);
    return true;
  }

  return false;
}