import { ControlData, TimeRange, MachineMode } from './pump_control_data.js';

const TYPE_IDENTIFIER_PUMP_CONTROL_DATA = 0x01;

function serializeControlData(controlData, typeIdentifier) {
    const dataBuffer = ControlData.encode(controlData);
    const buffer = Buffer.alloc(1 + dataBuffer.length);
    buffer[0] = typeIdentifier;
    dataBuffer.copy(buffer, 1);
    return buffer;
}

function deserializeControlData(buffer, expectedTypeIdentifier) {
    const typeIdentifier = buffer[0];
    if (typeIdentifier !== expectedTypeIdentifier) {
        throw new Error(`Unexpected type identifier: ${typeIdentifier}`);
    }
    const controlData = ControlData.decode(buffer.slice(1));
    return controlData;
}

const controlData = {
    mode: MachineMode.AUTO,
    is_running: true,
    time_range: {
        running: 5000,
        resting: 10000
    }
};

const serializedBuffer = serializeControlData(controlData, TYPE_IDENTIFIER_PUMP_CONTROL_DATA);
console.log('Serialized Buffer:', serializedBuffer);

try {
    const deserializedData = deserializeControlData(serializedBuffer, TYPE_IDENTIFIER_PUMP_CONTROL_DATA);
    console.log('Deserialized Control Data:', deserializedData);
} catch (error) {
    console.error('Failed to deserialize control data:', error);
}

let hexString = "01 08 02 1A 09 08 80 BE 92 01 10 80 9F 49".replace(/ /g, '');
let binaryBuffer = Buffer.from(hexString, 'hex');
let decodedControlData = deserializeControlData(binaryBuffer, TYPE_IDENTIFIER_PUMP_CONTROL_DATA);
console.log('Decoded ControlData:', decodedControlData);
