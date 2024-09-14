#include "pump_control.h"
#include <transcode_msg1.h>

static Msg1 pzem_data = Msg1_init_zero;

/*
  msg1.f0: total_energy_consumed_in_operation;
  msg1.f1: days_in_operation;
  msg1.f2: last_run_duration;
  msg1.f3: last_run_energy;
*/

static MessageData msg1(&pzem_data, PZEM1_TYPE_ID);

static void update_eeprom_data() {
  EEPROM.begin(EEPROM_SIZE_CTL);

  EEPROM.put(TOTAL_ENERGY_CONSUMED_ADDRESS, pzem_data.f0);
  EEPROM.put(DAYS_IN_OPERATION_ADDRESS, pzem_data.f1);
  EEPROM.commit();

  EEPROM.end();
}

void update_pzem_data(unsigned long durationMS) {
  float energy = pzem.energy();
  pzem_data.f0 += energy; // Total energy consumed

  float runtime = durationMS / 60000;
  
  pzem_data.f1 += runtime;        // Accumulated operation time in minutes
  pzem_data.f2 = runtime;         // Last run duration in minutes
  pzem_data.f3 = energy * 1000;   // Last run energy

  update_eeprom_data();

  enqueueMessage(msg1);

  reset_energy();
}

void init_pzem_data() {
  EEPROM.begin(EEPROM_SIZE_CTL);

  float stored_energy = 0.0f;
  EEPROM.get(TOTAL_ENERGY_CONSUMED_ADDRESS, stored_energy);
  if (stored_energy >= 0.0f) {
    pzem_data.f0 = stored_energy;
  } else {
    pzem_data.f0 = 0.0f;
  }

  float stored_days = 0.0f;
  EEPROM.get(DAYS_IN_OPERATION_ADDRESS, stored_days);
  if (stored_days >= 0.0f) {
    pzem_data.f1 = stored_days;
  } else {
    pzem_data.f1 = 0.0f;
  }

  EEPROM.end();

  pzem_data.f2 = 0.0f;
  pzem_data.f3 = 0.0f;
}

void send_pzem_data() {
  enqueueMessage(msg1);
}