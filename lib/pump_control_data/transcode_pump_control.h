#ifndef TRANS_PUMP_CONTROL_H
#define TRANS_PUMP_CONTROL_H

#include "./protoc/pump_control_data.pb.h"

bool serialize_control_data(uint8_t *buffer, size_t *buffer_size,
                            const pump_ControlData *control_data,
                            uint8_t type_id);
bool deserialize_control_data(const uint8_t *buffer, size_t buffer_size,
                              pump_ControlData *control_data);
#endif // TRANS_PUMP_CONTROL_H