// Code generated by protoc-gen-ts_proto. DO NOT EDIT.
// versions:
//   protoc-gen-ts_proto  v1.181.1
//   protoc               v3.21.12
// source: protoc/pump_control_data.proto

/* eslint-disable */
import * as _m0 from "protobufjs/minimal";

export const protobufPackage = "pump";

export enum MachineMode {
  POWER_OFF = 0,
  POWER_ON = 1,
  AUTO = 2,
  UNRECOGNIZED = -1,
}

export function machineModeFromJSON(object: any): MachineMode {
  switch (object) {
    case 0:
    case "POWER_OFF":
      return MachineMode.POWER_OFF;
    case 1:
    case "POWER_ON":
      return MachineMode.POWER_ON;
    case 2:
    case "AUTO":
      return MachineMode.AUTO;
    case -1:
    case "UNRECOGNIZED":
    default:
      return MachineMode.UNRECOGNIZED;
  }
}

export function machineModeToJSON(object: MachineMode): string {
  switch (object) {
    case MachineMode.POWER_OFF:
      return "POWER_OFF";
    case MachineMode.POWER_ON:
      return "POWER_ON";
    case MachineMode.AUTO:
      return "AUTO";
    case MachineMode.UNRECOGNIZED:
    default:
      return "UNRECOGNIZED";
  }
}

export interface TimeRange {
  running: number;
  resting: number;
}

export interface ControlData {
  mode: MachineMode;
  isRunning: boolean;
  timeRange: TimeRange | undefined;
}

function createBaseTimeRange(): TimeRange {
  return { running: 0, resting: 0 };
}

export const TimeRange = {
  encode(message: TimeRange, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.running !== 0) {
      writer.uint32(8).uint32(message.running);
    }
    if (message.resting !== 0) {
      writer.uint32(16).uint32(message.resting);
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): TimeRange {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBaseTimeRange();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.running = reader.uint32();
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.resting = reader.uint32();
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): TimeRange {
    return {
      running: isSet(object.running) ? globalThis.Number(object.running) : 0,
      resting: isSet(object.resting) ? globalThis.Number(object.resting) : 0,
    };
  },

  toJSON(message: TimeRange): unknown {
    const obj: any = {};
    if (message.running !== 0) {
      obj.running = Math.round(message.running);
    }
    if (message.resting !== 0) {
      obj.resting = Math.round(message.resting);
    }
    return obj;
  },

  create<I extends Exact<DeepPartial<TimeRange>, I>>(base?: I): TimeRange {
    return TimeRange.fromPartial(base ?? ({} as any));
  },
  fromPartial<I extends Exact<DeepPartial<TimeRange>, I>>(object: I): TimeRange {
    const message = createBaseTimeRange();
    message.running = object.running ?? 0;
    message.resting = object.resting ?? 0;
    return message;
  },
};

function createBaseControlData(): ControlData {
  return { mode: 0, isRunning: false, timeRange: undefined };
}

export const ControlData = {
  encode(message: ControlData, writer: _m0.Writer = _m0.Writer.create()): _m0.Writer {
    if (message.mode !== 0) {
      writer.uint32(8).int32(message.mode);
    }
    if (message.isRunning !== false) {
      writer.uint32(16).bool(message.isRunning);
    }
    if (message.timeRange !== undefined) {
      TimeRange.encode(message.timeRange, writer.uint32(26).fork()).ldelim();
    }
    return writer;
  },

  decode(input: _m0.Reader | Uint8Array, length?: number): ControlData {
    const reader = input instanceof _m0.Reader ? input : _m0.Reader.create(input);
    let end = length === undefined ? reader.len : reader.pos + length;
    const message = createBaseControlData();
    while (reader.pos < end) {
      const tag = reader.uint32();
      switch (tag >>> 3) {
        case 1:
          if (tag !== 8) {
            break;
          }

          message.mode = reader.int32() as any;
          continue;
        case 2:
          if (tag !== 16) {
            break;
          }

          message.isRunning = reader.bool();
          continue;
        case 3:
          if (tag !== 26) {
            break;
          }

          message.timeRange = TimeRange.decode(reader, reader.uint32());
          continue;
      }
      if ((tag & 7) === 4 || tag === 0) {
        break;
      }
      reader.skipType(tag & 7);
    }
    return message;
  },

  fromJSON(object: any): ControlData {
    return {
      mode: isSet(object.mode) ? machineModeFromJSON(object.mode) : 0,
      isRunning: isSet(object.isRunning) ? globalThis.Boolean(object.isRunning) : false,
      timeRange: isSet(object.timeRange) ? TimeRange.fromJSON(object.timeRange) : undefined,
    };
  },

  toJSON(message: ControlData): unknown {
    const obj: any = {};
    if (message.mode !== 0) {
      obj.mode = machineModeToJSON(message.mode);
    }
    if (message.isRunning !== false) {
      obj.isRunning = message.isRunning;
    }
    if (message.timeRange !== undefined) {
      obj.timeRange = TimeRange.toJSON(message.timeRange);
    }
    return obj;
  },

  create<I extends Exact<DeepPartial<ControlData>, I>>(base?: I): ControlData {
    return ControlData.fromPartial(base ?? ({} as any));
  },
  fromPartial<I extends Exact<DeepPartial<ControlData>, I>>(object: I): ControlData {
    const message = createBaseControlData();
    message.mode = object.mode ?? 0;
    message.isRunning = object.isRunning ?? false;
    message.timeRange = (object.timeRange !== undefined && object.timeRange !== null)
      ? TimeRange.fromPartial(object.timeRange)
      : undefined;
    return message;
  },
};

type Builtin = Date | Function | Uint8Array | string | number | boolean | undefined;

export type DeepPartial<T> = T extends Builtin ? T
  : T extends globalThis.Array<infer U> ? globalThis.Array<DeepPartial<U>>
  : T extends ReadonlyArray<infer U> ? ReadonlyArray<DeepPartial<U>>
  : T extends {} ? { [K in keyof T]?: DeepPartial<T[K]> }
  : Partial<T>;

type KeysOfUnion<T> = T extends T ? keyof T : never;
export type Exact<P, I extends P> = P extends Builtin ? P
  : P & { [K in keyof P]: Exact<P[K], I[K]> } & { [K in Exclude<keyof I, KeysOfUnion<P>>]: never };

function isSet(value: any): boolean {
  return value !== null && value !== undefined;
}
