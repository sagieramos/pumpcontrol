import { Num, Str, Strnum, Numlist, Auth } from './str_num_msg.js';
import { ControlData, TimeRange, MachineMode } from './pump_control_data.js';
import { TYPE_IDS } from './type_ids.js';
import { Buffer } from 'buffer';

const serializeAndSendData = (data, typeIdentifier, messageType) => {
    const dataBuffer = messageType.encode(data);
    const buffer = Buffer.alloc(1 + dataBuffer.length);

    buffer[0] = typeIdentifier;
    dataBuffer.copy(buffer, 1);

    return buffer;
};

// Example usage
const messageType = {
    encode: (data) => {
        // Dummy encode function, replace with your actual encoding logic
        const str = JSON.stringify(data);
        return Buffer.from(str);
    }
};

const testData = { mode: 'Power On' };
const typeIdentifier = 1;
const buffer = serializeAndSendData(testData, typeIdentifier, messageType);

console.log(buffer);


const TYPE_IDENTIFIER_NUM = 0x01;

/* const TYPE_IDS = {
    VOID_TYPE_ID: 0x00,
    SINGLE_CONFIG_TYPE_ID: 0x01,
    STR_TYPE_ID: 0x02,
    STR_NUM_TYPE_ID: 0x03,
    CONTROL_DATA_TYPE_ID: 0x04,
    PUMP_TIME_RANGE_TYPE_ID: 0x05,
    AUTH_TYPE_ID: 0x06
}; */

function serializeData(data, typeIdentifier, proto) {
    const dataBuffer = proto.encode(data);
    const buffer = new ArrayBuffer(1 + dataBuffer.length);
    const uint8View = new Uint8Array(buffer);
    uint8View[0] = typeIdentifier;
    uint8View.set(new Uint8Array(dataBuffer), 1);

    return uint8View;
};

export function deserializeData(buffer, expectedTypeIdentifier, proto) {
    const typeIdentifier = buffer[0];
    if (typeIdentifier !== expectedTypeIdentifier) {
        throw new Error(`Unexpected type identifier: ${typeIdentifier}`);
    }
    return proto.decode(buffer.slice(1));
}

// Example with Num
const num = { key: 102, value: 101009 };
const serializedNum = serializeData(num, TYPE_IDS.SINGLE_CONFIG_TYPE_ID, Num);
console.log('Serialized Num:', serializedNum);

try {
    const deserializedNum = deserializeData(serializedNum, TYPE_IDS.SINGLE_CONFIG_TYPE_ID, Num);
    console.log('Deserialized Num:', deserializedNum);
} catch (error) {
    console.error('Failed to deserialize Num:', error);
}

// Example with Numlist
const numlist = { value: [1, 2, 3, 4, 5] };
const serializedNumlist = serializeData(numlist, TYPE_IDS.SINGLE_CONFIG_TYPE_ID, Numlist);
console.log('Serialized Numlist:', serializedNumlist);

try {
    const deserializedNumlist = deserializeData(serializedNumlist, TYPE_IDS.SINGLE_CONFIG_TYPE_ID, Numlist);
    console.log('Deserialized Numlist:', deserializedNumlist);
} catch (error) {
    console.error('Failed to deserialize Numlist:', error);
}

// Example with Auth
const auth = { id: "imuwahen", pass: "password" };
const serializedAuth = serializeData(auth, TYPE_IDS.AUTH_TYPE_ID, Auth);
// Convert the buffer to a hex string
const hexStrin = serializedAuth.toString('hex').toUpperCase();
console.log('Serialized Auth (Hex):', hexStrin);
console.log('Serialized Auth:', serializedAuth);

try {
    const deserializedAuth = deserializeData(serializedAuth, TYPE_IDS.AUTH_TYPE_ID, Auth);
    console.log('Deserialized Auth:', deserializedAuth);
} catch (error) {
    console.error('Failed to deserialize Auth:', error);
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


// Example with Strnum
const strnum = { key: 1, str: "Print the buffer contents and sizes at", num: 42.42 };
const serializedStrnum = serializeData(strnum, TYPE_IDS.STR_NUM_TYPE_ID, Strnum);
console.log('Serialized Strnum:', serializedStrnum);

try {
    const deserializedStrnum = deserializeData(serializedStrnum, TYPE_IDS.STR_NUM_TYPE_ID, Strnum);
    console.log('Deserialized Strnum:', deserializedStrnum);
} catch (error) {
    console.error('Failed to deserialize Strnum:', error);
}

// Example with ControlData
const controlData = {
    mode: MachineMode.POWER_ON,
    is_running: true,
    time_range: {
        running: 220000,
        resting: 140000
    }
};


// Example with TimeRange
const timeRange = { running: 5000, resting: 10000 };

// Encode the timeRange object
const encodedTimeRange = TimeRange.encode(timeRange);

console.log('Encoded TimeRange:', encodedTimeRange);

// Decode the encoded data back into an object
const decodedTimeRange = TimeRange.decode(encodedTimeRange);

console.log('Decoded TimeRange:', decodedTimeRange);

let hexString = "01 08 02 1A 09 08 80 BE 92 01 10 80 9F 49".replace(/ /g, '');
let binaryBuffer = Buffer.from(hexString, 'hex');
let decodedControlData = deserializeData(binaryBuffer, TYPE_IDENTIFIER_NUM, ControlData);
console.log('Decoded ControlData:', decodedControlData);

let hexString1 = "04 08 02 1A 09 08 80 BE 92 01 10 80 9F 49".replace(/ /g, '');
let binaryBuffer1 = Buffer.from(hexString1, 'hex');
let decodedControlData1 = deserializeData(binaryBuffer1, TYPE_IDS.CONTROL_DATA_TYPE_ID, ControlData);
console.log('Decoded ControlData:', decodedControlData1);

//let hexStringNum = "01 08 04 15 00 00 80 3F".replace(/ /g, '');
let hexStringNum6 = "01 15 33 33 53 C0 ".replace(/ /g, '');
let binaryBufferNum6 = Buffer.from(hexStringNum6, 'hex');
let decodedControlDataNum6 = deserializeData(binaryBufferNum6, TYPE_IDS.SINGLE_CONFIG_TYPE_ID, Num);
console.log('Decoded num:', decodedControlDataNum6);
