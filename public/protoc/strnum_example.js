import { Num, Numlist, Str, Strlist, Strnum, Strnumlist } from './str_num_msg.js';

const TYPE_IDENTIFIER_NUM = 0x01;

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

const num = { key: 1, value: 42.42 };
const serializedNum = serializeData(num, TYPE_IDENTIFIER_NUM, Num);
console.log('Serialized Num:', serializedNum);

try {
    const deserializedNum = deserializeData(serializedNum, TYPE_IDENTIFIER_NUM, Num);
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
const serializedStr = serializeData(str, TYPE_IDENTIFIER_NUM, Str);
console.log('Serialized Str:', serializedStr);

try {
    const deserializedStr = deserializeData(serializedStr, TYPE_IDENTIFIER_NUM, Str);
    console.log('Deserialized Str:', deserializedStr);
} catch (error) {
    console.error('Failed to deserialize Str:', error);
}

// Example with Strlist
const strlist = { Str: ["hello", "world"] };
const serializedStrlist = serializeData(strlist, TYPE_IDENTIFIER_NUM, Strlist);
console.log('Serialized Strlist:', serializedStrlist);

try {
    const deserializedStrlist = deserializeData(serializedStrlist, TYPE_IDENTIFIER_NUM, Strlist);
    console.log('Deserialized Strlist:', deserializedStrlist);
} catch (error) {
    console.error('Failed to deserialize Strlist:', error);
}

// Example with Strnum
const strnum = { key: 1, str: "hello", num: 42.42 };
const serializedStrnum = serializeData(strnum, TYPE_IDENTIFIER_NUM, Strnum);
console.log('Serialized Strnum:', serializedStrnum);

try {
    const deserializedStrnum = deserializeData(serializedStrnum, TYPE_IDENTIFIER_NUM, Strnum);
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
