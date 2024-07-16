// @generated by protoc-gen-es v1.10.0 with parameter "target=ts"
// @generated from file domsgid.proto (syntax proto3)
/* eslint-disable */
// @ts-nocheck

import type { BinaryReadOptions, FieldList, JsonReadOptions, JsonValue, PartialMessage, PlainMessage } from "@bufbuild/protobuf";
import { Message, proto3, Timestamp } from "@bufbuild/protobuf";

/**
 * @generated from message DoId
 */
export class DoId extends Message<DoId> {
  /**
   * @generated from field: uint32 id = 1;
   */
  id = 0;

  /**
   * @generated from field: float value = 2;
   */
  value = 0;

  constructor(data?: PartialMessage<DoId>) {
    super();
    proto3.util.initPartial(data, this);
  }

  static readonly runtime: typeof proto3 = proto3;
  static readonly typeName = "DoId";
  static readonly fields: FieldList = proto3.util.newFieldList(() => [
    { no: 1, name: "id", kind: "scalar", T: 13 /* ScalarType.UINT32 */ },
    { no: 2, name: "value", kind: "scalar", T: 2 /* ScalarType.FLOAT */ },
  ]);

  static fromBinary(bytes: Uint8Array, options?: Partial<BinaryReadOptions>): DoId {
    return new DoId().fromBinary(bytes, options);
  }

  static fromJson(jsonValue: JsonValue, options?: Partial<JsonReadOptions>): DoId {
    return new DoId().fromJson(jsonValue, options);
  }

  static fromJsonString(jsonString: string, options?: Partial<JsonReadOptions>): DoId {
    return new DoId().fromJsonString(jsonString, options);
  }

  static equals(a: DoId | PlainMessage<DoId> | undefined, b: DoId | PlainMessage<DoId> | undefined): boolean {
    return proto3.util.equals(DoId, a, b);
  }
}

/**
 * @generated from message DoIdList
 */
export class DoIdList extends Message<DoIdList> {
  /**
   * @generated from field: repeated DoId doid = 1;
   */
  doid: DoId[] = [];

  constructor(data?: PartialMessage<DoIdList>) {
    super();
    proto3.util.initPartial(data, this);
  }

  static readonly runtime: typeof proto3 = proto3;
  static readonly typeName = "DoIdList";
  static readonly fields: FieldList = proto3.util.newFieldList(() => [
    { no: 1, name: "doid", kind: "message", T: DoId, repeated: true },
  ]);

  static fromBinary(bytes: Uint8Array, options?: Partial<BinaryReadOptions>): DoIdList {
    return new DoIdList().fromBinary(bytes, options);
  }

  static fromJson(jsonValue: JsonValue, options?: Partial<JsonReadOptions>): DoIdList {
    return new DoIdList().fromJson(jsonValue, options);
  }

  static fromJsonString(jsonString: string, options?: Partial<JsonReadOptions>): DoIdList {
    return new DoIdList().fromJsonString(jsonString, options);
  }

  static equals(a: DoIdList | PlainMessage<DoIdList> | undefined, b: DoIdList | PlainMessage<DoIdList> | undefined): boolean {
    return proto3.util.equals(DoIdList, a, b);
  }
}

