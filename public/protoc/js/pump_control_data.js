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

  function encode(obj, arr = new Uint8Array(encodingLength(obj)), offset = 0) {
    let view = new DataView(arr.buffer);
    let oldOffset = offset;
    if (defined(obj.running)) {
      arr[offset++] = 8;
      offset += encodings.varint.encode(obj.running, arr, offset);
    }
    if (defined(obj.resting)) {
      arr[offset++] = 16;
      offset += encodings.varint.encode(obj.resting, arr, offset);
    }
    encode.bytes = offset - oldOffset;
    return arr;
  }

  function decode(arr, offset = 0, end = arr.length) {
    let view = new DataView(arr.buffer);
    let oldOffset = offset;
    let obj = {
      running: 0,
      resting: 0
    };
    while (offset < end) {
      let prefix = varint.decode(arr, offset);
      offset += varint.decode.bytes;
      let tag = prefix >> 3;
      switch (tag) {
        case 1:
          obj.running = encodings.varint.decode(arr, offset);
          offset += encodings.varint.decode.bytes;
          break;
        case 2:
          obj.resting = encodings.varint.decode(arr, offset);
          offset += encodings.varint.decode.bytes;
          break;
        default:
          offset = skip(prefix & 7, arr, offset);
      }
    }
    decode.bytes = offset - oldOffset;
    return obj;
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

  function encode(obj, arr = new Uint8Array(encodingLength(obj)), offset = 0) {
    let view = new DataView(arr.buffer);
    let oldOffset = offset;
    if (defined(obj.mode)) {
      arr[offset++] = 8;
      offset += encodings.enum.encode(obj.mode, arr, offset);
    }
    if (defined(obj.is_running)) {
      arr[offset++] = 16;
      offset += encodings.bool.encode(obj.is_running, arr, offset);
    }
    if (defined(obj.time_range)) {
      arr[offset++] = 26;
      varint.encode(TimeRange.encodingLength(obj.time_range), arr, offset);
      offset += varint.encode.bytes;
      TimeRange.encode(obj.time_range, arr, offset);
      offset += TimeRange.encode.bytes;
    }
    encode.bytes = offset - oldOffset;
    return arr;
  }

  function decode(arr, offset = 0, end = arr.length) {
    let view = new DataView(arr.buffer);
    let oldOffset = offset;
    let obj = {
      mode: 0,
      is_running: false,
      time_range: null
    };
    while (offset < end) {
      let prefix = varint.decode(arr, offset);
      offset += varint.decode.bytes;
      let tag = prefix >> 3;
      switch (tag) {
        case 1:
          obj.mode = encodings.enum.decode(arr, offset);
          offset += encodings.enum.decode.bytes;
          break;
        case 2:
          obj.is_running = encodings.bool.decode(arr, offset);
          offset += encodings.bool.decode.bytes;
          break;
        case 3:
          let len = varint.decode(arr, offset);
          offset += varint.decode.bytes;
          obj.time_range = TimeRange.decode(arr, offset, offset + len);
          offset += TimeRange.decode.bytes;
          break;
        default:
          offset = skip(prefix & 7, arr, offset);
      }
    }
    decode.bytes = offset - oldOffset;
    return obj;
  }
}

function defined(val) {
  return val !== null && val !== undefined && (typeof val !== 'number' || !isNaN(val));
}
