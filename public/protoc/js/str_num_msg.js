import * as encodings from 'protocol-buffers-encodings';

const { varint, skip } = encodings;

export const Num = {
  buffer: false, // set to false as Buffer is not used
  encodingLength: null,
  encode: null,
  decode: null
};

export const Numlist = {
  buffer: false,
  encodingLength: null,
  encode: null,
  decode: null
};

export const Str = {
  buffer: false,
  encodingLength: null,
  encode: null,
  decode: null
};

export const Strnum = {
  buffer: false,
  encodingLength: null,
  encode: null,
  decode: null
};

export const Auth = {
  buffer: false,
  encodingLength: null,
  encode: null,
  decode: null
};

defineNum();
defineNumlist();
defineStr();
defineStrnum();
defineAuth();

function defineNum() {
  Num.encodingLength = encodingLength;
  Num.encode = encode;
  Num.decode = decode;

  function encodingLength(obj) {
    let length = 0;
    if (defined(obj.key)) {
      const len = encodings.varint.encodingLength(obj.key);
      length += 1 + len;
    }
    if (defined(obj.value)) {
      const len = encodings.float.encodingLength(obj.value);
      length += 1 + len;
    }
    return length;
  }

  function encode(obj, buf, offset = 0) {
    if (!buf) buf = new Uint8Array(encodingLength(obj));
    const oldOffset = offset;
    if (defined(obj.key)) {
      buf[offset++] = 8;
      encodings.varint.encode(obj.key, buf, offset);
      offset += encodings.varint.encode.bytes;
    }
    if (defined(obj.value)) {
      buf[offset++] = 21;
      encodings.float.encode(obj.value, buf, offset);
      offset += encodings.float.encode.bytes;
    }
    encode.bytes = offset - oldOffset;
    return buf;
  }

  function decode(buf, offset = 0, end = buf.length) {
    if (!(end <= buf.length && offset <= buf.length)) throw new Error("Decoded message is not valid");
    const oldOffset = offset;
    const obj = {
      key: 0,
      value: 0
    };
    while (true) {
      if (end <= offset) {
        decode.bytes = offset - oldOffset;
        return obj;
      }
      const prefix = varint.decode(buf, offset);
      offset += varint.decode.bytes;
      const tag = prefix >> 3;
      switch (tag) {
        case 1:
          obj.key = encodings.varint.decode(buf, offset);
          offset += encodings.varint.decode.bytes;
          break;
        case 2:
          obj.value = encodings.float.decode(buf, offset);
          offset += encodings.float.decode.bytes;
          break;
        default:
          offset = skip(prefix & 7, buf, offset);
      }
    }
  }
}

function defineNumlist() {
  Numlist.encodingLength = encodingLength;
  Numlist.encode = encode;
  Numlist.decode = decode;

  function encodingLength(obj) {
    let length = 0;
    if (defined(obj.value)) {
      for (let i = 0; i < obj.value.length; i++) {
        if (!defined(obj.value[i])) continue;
        const len = encodings.float.encodingLength(obj.value[i]);
        length += 1 + len;
      }
    }
    return length;
  }

  function encode(obj, buf, offset = 0) {
    if (!buf) buf = new Uint8Array(encodingLength(obj));
    const oldOffset = offset;
    if (defined(obj.value)) {
      for (let i = 0; i < obj.value.length; i++) {
        if (!defined(obj.value[i])) continue;
        buf[offset++] = 13;
        encodings.float.encode(obj.value[i], buf, offset);
        offset += encodings.float.encode.bytes;
      }
    }
    encode.bytes = offset - oldOffset;
    return buf;
  }

  function decode(buf, offset = 0, end = buf.length) {
    if (!(end <= buf.length && offset <= buf.length)) throw new Error("Decoded message is not valid");
    const oldOffset = offset;
    const obj = {
      value: []
    };
    while (true) {
      if (end <= offset) {
        decode.bytes = offset - oldOffset;
        return obj;
      }
      const prefix = varint.decode(buf, offset);
      offset += varint.decode.bytes;
      const tag = prefix >> 3;
      switch (tag) {
        case 1:
          obj.value.push(encodings.float.decode(buf, offset));
          offset += encodings.float.decode.bytes;
          break;
        default:
          offset = skip(prefix & 7, buf, offset);
      }
    }
  }
}

function defineStr() {
  Str.encodingLength = encodingLength;
  Str.encode = encode;
  Str.decode = decode;

  function encodingLength(obj) {
    let length = 0;
    if (defined(obj.key)) {
      const len = encodings.varint.encodingLength(obj.key);
      length += 1 + len;
    }
    if (defined(obj.value)) {
      const len = encodings.string.encodingLength(obj.value);
      length += 1 + len;
    }
    return length;
  }

  function encode(obj, buf, offset = 0) {
    if (!buf) buf = new Uint8Array(encodingLength(obj));
    const oldOffset = offset;
    if (defined(obj.key)) {
      buf[offset++] = 8;
      encodings.varint.encode(obj.key, buf, offset);
      offset += encodings.varint.encode.bytes;
    }
    if (defined(obj.value)) {
      buf[offset++] = 18;
      encodings.string.encode(obj.value, buf, offset);
      offset += encodings.string.encode.bytes;
    }
    encode.bytes = offset - oldOffset;
    return buf;
  }

  function decode(buf, offset = 0, end = buf.length) {
    if (!(end <= buf.length && offset <= buf.length)) throw new Error("Decoded message is not valid");
    const oldOffset = offset;
    const obj = {
      key: 0,
      value: ""
    };
    while (true) {
      if (end <= offset) {
        decode.bytes = offset - oldOffset;
        return obj;
      }
      const prefix = varint.decode(buf, offset);
      offset += varint.decode.bytes;
      const tag = prefix >> 3;
      switch (tag) {
        case 1:
          obj.key = encodings.varint.decode(buf, offset);
          offset += encodings.varint.decode.bytes;
          break;
        case 2:
          obj.value = encodings.string.decode(buf, offset);
          offset += encodings.string.decode.bytes;
          break;
        default:
          offset = skip(prefix & 7, buf, offset);
      }
    }
  }
}

function defineStrnum() {
  Strnum.encodingLength = encodingLength;
  Strnum.encode = encode;
  Strnum.decode = decode;

  function encodingLength(obj) {
    let length = 0;
    if (defined(obj.key)) {
      const len = encodings.varint.encodingLength(obj.key);
      length += 1 + len;
    }
    if (defined(obj.str)) {
      const len = encodings.string.encodingLength(obj.str);
      length += 1 + len;
    }
    if (defined(obj.num)) {
      const len = encodings.float.encodingLength(obj.num);
      length += 1 + len;
    }
    return length;
  }

  function encode(obj, buf, offset = 0) {
    if (!buf) buf = new Uint8Array(encodingLength(obj));
    const oldOffset = offset;
    if (defined(obj.key)) {
      buf[offset++] = 8;
      encodings.varint.encode(obj.key, buf, offset);
      offset += encodings.varint.encode.bytes;
    }
    if (defined(obj.str)) {
      buf[offset++] = 18;
      encodings.string.encode(obj.str, buf, offset);
      offset += encodings.string.encode.bytes;
    }
    if (defined(obj.num)) {
      buf[offset++] = 29;
      encodings.float.encode(obj.num, buf, offset);
      offset += encodings.float.encode.bytes;
    }
    encode.bytes = offset - oldOffset;
    return buf;
  }

  function decode(buf, offset = 0, end = buf.length) {
    if (!(end <= buf.length && offset <= buf.length)) throw new Error("Decoded message is not valid");
    const oldOffset = offset;
    const obj = {
      key: 0,
      str: "",
      num: 0
    };
    while (true) {
      if (end <= offset) {
        decode.bytes = offset - oldOffset;
        return obj;
      }
      const prefix = varint.decode(buf, offset);
      offset += varint.decode.bytes;
      const tag = prefix >> 3;
      switch (tag) {
        case 1:
          obj.key = encodings.varint.decode(buf, offset);
          offset += encodings.varint.decode.bytes;
          break;
        case 2:
          obj.str = encodings.string.decode(buf, offset);
          offset += encodings.string.decode.bytes;
          break;
        case 3:
          obj.num = encodings.float.decode(buf, offset);
          offset += encodings.float.decode.bytes;
          break;
        default:
          offset = skip(prefix & 7, buf, offset);
      }
    }
  }
}

function defineAuth() {
  Auth.encodingLength = encodingLength;
  Auth.encode = encode;
  Auth.decode = decode;

  function encodingLength(obj) {
    let length = 0;
    if (defined(obj.id)) {
      const len = encodings.string.encodingLength(obj.id);
      length += 1 + len;
    }
    if (defined(obj.pass)) {
      const len = encodings.string.encodingLength(obj.pass);
      length += 1 + len;
    }
    return length;
  }

  function encode(obj, buf, offset = 0) {
    if (!buf) buf = new Uint8Array(encodingLength(obj));
    const oldOffset = offset;
    if (defined(obj.id)) {
      buf[offset++] = 10;
      encodings.string.encode(obj.id, buf, offset);
      offset += encodings.string.encode.bytes;
    }
    if (defined(obj.pass)) {
      buf[offset++] = 18;
      encodings.string.encode(obj.pass, buf, offset);
      offset += encodings.string.encode.bytes;
    }
    encode.bytes = offset - oldOffset;
    return buf;
  }

  function decode(buf, offset = 0, end = buf.length) {
    if (!(end <= buf.length && offset <= buf.length)) throw new Error("Decoded message is not valid");
    const oldOffset = offset;
    const obj = {
      id: "",
      pass: ""
    };
    while (true) {
      if (end <= offset) {
        decode.bytes = offset - oldOffset;
        return obj;
      }
      const prefix = varint.decode(buf, offset);
      offset += varint.decode.bytes;
      const tag = prefix >> 3;
      switch (tag) {
        case 1:
          obj.id = encodings.string.decode(buf, offset);
          offset += encodings.string.decode.bytes;
          break;
        case 2:
          obj.pass = encodings.string.decode(buf, offset);
          offset += encodings.string.decode.bytes;
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
