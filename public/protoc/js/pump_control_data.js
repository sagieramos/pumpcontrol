// Example conversion

// Original CommonJS code
// const encodings = require('protocol-buffers-encodings');
// const varint = encodings.varint;
// const skip = encodings.skip;
// module.exports = {
//   MachineMode,
//   TimeRange,
//   ControlData
// };

// Converted ES module code
import encodings from 'protocol-buffers-encodings';
const varint = encodings.varint;
const skip = encodings.skip;

export const MachineMode = {
  POWER_OFF: 0,
  POWER_ON: 1,
  AUTO: 2
};

export const TimeRange = {
  buffer: true,
  encodingLength: null,
  encode: null,
  decode: null
};

export const ControlData = {
  buffer: true,
  encodingLength: null,
  encode: null,
  decode: null
};

// Define the encoding and decoding functions here
defineTimeRange();
defineControlData();

function defineTimeRange() {
  TimeRange.encodingLength = encodingLength;
  TimeRange.encode = encode;
  TimeRange.decode = decode;

  function encodingLength(obj) {
    let length = 0;
    if (defined(obj.running)) {
      let len = encodings.varint.encodingLength(obj.running);
      length += 1 + len;
    }
    if (defined(obj.resting)) {
      let len = encodings.varint.encodingLength(obj.resting);
      length += 1 + len;
    }
    return length;
  }

  function encode(obj, buf, offset) {
    if (!offset) offset = 0;
    if (!buf) buf = Buffer.allocUnsafe(encodingLength(obj));
    let oldOffset = offset;
    if (defined(obj.running)) {
      buf[offset++] = 8;
      encodings.varint.encode(obj.running, buf, offset);
      offset += encodings.varint.encode.bytes;
    }
    if (defined(obj.resting)) {
      buf[offset++] = 16;
      encodings.varint.encode(obj.resting, buf, offset);
      offset += encodings.varint.encode.bytes;
    }
    encode.bytes = offset - oldOffset;
    return buf;
  }

  function decode(buf, offset, end) {
    if (!offset) offset = 0;
    if (!end) end = buf.length;
    if (!(end <= buf.length && offset <= buf.length)) throw new Error("Decoded message is not valid");
    let oldOffset = offset;
    let obj = {
      running: 0,
      resting: 0
    };
    while (true) {
      if (end <= offset) {
        decode.bytes = offset - oldOffset;
        return obj;
      }
      let prefix = varint.decode(buf, offset);
      offset += varint.decode.bytes;
      let tag = prefix >> 3;
      switch (tag) {
        case 1:
          obj.running = encodings.varint.decode(buf, offset);
          offset += encodings.varint.decode.bytes;
          break;
        case 2:
          obj.resting = encodings.varint.decode(buf, offset);
          offset += encodings.varint.decode.bytes;
          break;
        default:
          offset = skip(prefix & 7, buf, offset);
      }
    }
  }
}

function defineControlData() {
  ControlData.encodingLength = encodingLength;
  ControlData.encode = encode;
  ControlData.decode = decode;

  function encodingLength(obj) {
    let length = 0;
    if (defined(obj.mode)) {
      let len = encodings.enum.encodingLength(obj.mode);
      length += 1 + len;
    }
    if (defined(obj.is_running)) {
      let len = encodings.bool.encodingLength(obj.is_running);
      length += 1 + len;
    }
    if (defined(obj.time_range)) {
      let len = TimeRange.encodingLength(obj.time_range);
      length += varint.encodingLength(len);
      length += 1 + len;
    }
    return length;
  }

  function encode(obj, buf, offset) {
    if (!offset) offset = 0;
    if (!buf) buf = Buffer.allocUnsafe(encodingLength(obj));
    let oldOffset = offset;
    if (defined(obj.mode)) {
      buf[offset++] = 8;
      encodings.enum.encode(obj.mode, buf, offset);
      offset += encodings.enum.encode.bytes;
    }
    if (defined(obj.is_running)) {
      buf[offset++] = 16;
      encodings.bool.encode(obj.is_running, buf, offset);
      offset += encodings.bool.encode.bytes;
    }
    if (defined(obj.time_range)) {
      buf[offset++] = 26;
      varint.encode(TimeRange.encodingLength(obj.time_range), buf, offset);
      offset += varint.encode.bytes;
      TimeRange.encode(obj.time_range, buf, offset);
      offset += TimeRange.encode.bytes;
    }
    encode.bytes = offset - oldOffset;
    return buf;
  }

  function decode(buf, offset, end) {
    if (!offset) offset = 0;
    if (!end) end = buf.length;
    if (!(end <= buf.length && offset <= buf.length)) throw new Error("Decoded message is not valid");
    let oldOffset = offset;
    let obj = {
      mode: 0,
      is_running: false,
      time_range: null
    };
    while (true) {
      if (end <= offset) {
        decode.bytes = offset - oldOffset;
        return obj;
      }
      let prefix = varint.decode(buf, offset);
      offset += varint.decode.bytes;
      let tag = prefix >> 3;
      switch (tag) {
        case 1:
          obj.mode = encodings.enum.decode(buf, offset);
          offset += encodings.enum.decode.bytes;
          break;
        case 2:
          obj.is_running = encodings.bool.decode(buf, offset);
          offset += encodings.bool.decode.bytes;
          break;
        case 3:
          let len = varint.decode(buf, offset);
          offset += varint.decode.bytes;
          obj.time_range = TimeRange.decode(buf, offset, offset + len);
          offset += TimeRange.decode.bytes;
          break;
        default:
          offset = skip(prefix & 7, buf, offset);
      }
    }
  }
}

function defined(val) {
  return val !== null && val !== undefined && (typeof val !== 'number' || !isNaN(val));
}

