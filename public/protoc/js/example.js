import { Num, Numlist, Str, Strlist, Strnum, Strnumlist } from './str_num_msg.js';
import { ControlData, TimeRange, MachineMode } from './pump_control_data.js';
import { TYPE_IDS } from './type_ids.js';

const TYPE_IDENTIFIER_NUM = 0x01;

/* const TYPE_IDS = {
    VOID_TYPE_ID: 0x00,
    SINGLE_CONFIG_TYPE_ID: 0x01,
    STR_TYPE_ID: 0x02,
    STR_NUM_TYPE_ID: 0x03,
    CONTROL_DATA_TYPE_ID: 0x04,
    PUMP_TIME_RANGE_TYPE_ID: 0x05
}; */

function serializeData(data, typeIdentifier, proto) {
    const dataBuffer = proto.encode(data);
    const buffer = Buffer.alloc(1 + dataBuffer.length);
    buffer[0] = typeIdentifier;
    dataBuffer.copy(buffer, 1);
    return buffer;
}

function deserializeData(buffer, expectedTypeIdentifier, proto) {
    const typeIdentifier = buffer[0];
    if (typeIdentifier !== expectedTypeIdentifier) {
        throw new Error(`Unexpected type identifier: ${typeIdentifier}`);
    }
    return proto.decode(buffer.slice(1));
}

// Example with Num
const num = { key: 1, value: 42.42 };
const serializedNum = serializeData(num, TYPE_IDS.SINGLE_CONFIG_TYPE_ID, Num);
console.log('Serialized Num:', serializedNum);

try {
    const deserializedNum = deserializeData(serializedNum, TYPE_IDS.SINGLE_CONFIG_TYPE_ID, Num);
    console.log('Deserialized Num:', deserializedNum);
} catch (error) {
    console.error('Failed to deserialize Num:', error);
}

// Example with Numlist
const numlist = { numbers: [{ key: 1, value: 42.42 }, { key: 2, value: 24.24 }] };
const serializedNumlist = serializeData(numlist, TYPE_IDENTIFIER_NUM, Numlist);
console.log('Serialized Numlist:', serializedNumlist);

try {
    const deserializedNumlist = deserializeData(serializedNumlist, TYPE_IDENTIFIER_NUM, Numlist);
    console.log('Deserialized Numlist:', deserializedNumlist);
} catch (error) {
    console.error('Failed to deserialize Numlist:', error);
}

// Example with Str
const str = { key: 1, value: "hello" };
const serializedStr = serializeData(str, TYPE_IDS.STR_TYPE_ID, Str);
console.log('Serialized Str:', serializedStr);

try {
    const deserializedStr = deserializeData(serializedStr, TYPE_IDS.STR_TYPE_ID, Str);
    console.log('Deserialized Str:', deserializedStr);
} catch (error) {
    console.error('Failed to deserialize Str:', error);
}

// Example with Strlist
const strlist = { Str: ["hello", "world"] };
const serializedStrlist = serializeData(strlist, TYPE_IDS.STR_TYPE_ID, Strlist);
console.log('Serialized Strlist:', serializedStrlist);

try {
    const deserializedStrlist = deserializeData(serializedStrlist, TYPE_IDS.STR_TYPE_ID, Strlist);
    console.log('Deserialized Strlist:', deserializedStrlist);
} catch (error) {
    console.error('Failed to deserialize Strlist:', error);
}

// Example with Strnum
const strnum = { key: 1, str: "hello", num: 42.42 };
const serializedStrnum = serializeData(strnum, TYPE_IDS.STR_NUM_TYPE_ID, Strnum);
console.log('Serialized Strnum:', serializedStrnum);

try {
    const deserializedStrnum = deserializeData(serializedStrnum, TYPE_IDS.STR_NUM_TYPE_ID, Strnum);
    console.log('Deserialized Strnum:', deserializedStrnum);
} catch (error) {
    console.error('Failed to deserialize Strnum:', error);
}

// Example with Strnumlist
const strnumlist = { strnum: [{ key: 1, str: "hello", num: 42.42 }, { key: 2, str: "world", num: 24.24 }] };
const serializedStrnumlist = serializeData(strnumlist, TYPE_IDENTIFIER_NUM, Strnumlist);
console.log('Serialized Strnumlist:', serializedStrnumlist);

try {
    const deserializedStrnumlist = deserializeData(serializedStrnumlist, TYPE_IDENTIFIER_NUM, Strnumlist);
    console.log('Deserialized Strnumlist:', deserializedStrnumlist);
} catch (error) {
    console.error('Failed to deserialize Strnumlist:', error);
}

// Example with ControlData
const controlData = {
    mode: MachineMode.AUTO,
    is_running: true,
    time_range: {
        running: 830000,
        resting: 970000
    }
};

const serializedControlData = serializeData(controlData, TYPE_IDS.CONTROL_DATA_TYPE_ID, ControlData);
console.log('Serialized ControlData:', serializedControlData);

try {
    const deserializedControlData = deserializeData(serializedControlData, TYPE_IDS.CONTROL_DATA_TYPE_ID, ControlData);
    console.log('Deserialized ControlData:', deserializedControlData);
}
catch (error) {
    console.error('Failed to deserialize ControlData:', error);
}

// Example with TimeRange
const timeRange = { running: 5000, resting: 10000 };
const serializedTimeRange = serializeData(timeRange, TYPE_IDS.PUMP_TIME_RANGE_TYPE_ID, TimeRange);
console.log('Serialized TimeRange:', serializedTimeRange);

try {
    const deserializedTimeRange = deserializeData(serializedTimeRange, TYPE_IDS.PUMP_TIME_RANGE_TYPE_ID, TimeRange);
    console.log('Deserialized TimeRange:', deserializedTimeRange);
}
catch (error) {
    console.error('Failed to deserialize TimeRange:', error);
}

let hexString = "01 08 02 1A 09 08 80 BE 92 01 10 80 9F 49".replace(/ /g, '');
let binaryBuffer = Buffer.from(hexString, 'hex');
let decodedControlData = deserializeData(binaryBuffer, TYPE_IDENTIFIER_NUM, ControlData);
console.log('Decoded ControlData:', decodedControlData);

let hexString1 = "04 08 02 1A 09 08 80 BE 92 01 10 80 9F 49".replace(/ /g, '');
let binaryBuffer1 = Buffer.from(hexString1, 'hex');
let decodedControlData1 = deserializeData(binaryBuffer1, TYPE_IDS.CONTROL_DATA_TYPE_ID, ControlData);
console.log('Decoded ControlData:', decodedControlData1);
