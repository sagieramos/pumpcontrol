#include "main.h"

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  dnsServer.start(53, "Imuwahen.com", WiFi.localIP());

  // Create the blink task
  xTaskCreatePinnedToCore(taskBlink,    // Function pointer to the task function
                          "Blink Task", // Name of the task (for debugging)
                          2048,         // Stack size (in words, not bytes)
                          NULL,         // Task parameters (not used here)
                          0, // Priority of the task (1 is highest priority)
                          &blinkTaskHandle, // Task handle (not used here)
                          1                 // Core to run the task on (0 or 1)
  );

  xTaskCreate(taskRun,    // Function name of the task
              "Task Run", // Name of the task (e.g. for debugging)
              2048,       // Stack size (bytes)
              NULL,       // Parameter to pass
              1,          // Task priority
              NULL        // Task handle
  );
}

void loop() {
  // Nothing to do here as main tasks are handled by FreeRTOS tasks
  vTaskDelay(portMAX_DELAY);
}
