// Copyright (c) Facebook, Inc. and its affiliates
// SPDX-License-Identifier: MIT OR Apache-2.0

#pragma once

#include <algorithm>
#include <cassert>

#include "serde.hpp"

namespace serde {

template <class S>
class BinarySerializer {
  protected:
    std::vector<uint8_t> bytes_;

  public:
    BinarySerializer() {}

    void serialize_str(const std::string &value);

    void serialize_bool(bool value);
    void serialize_unit();
    void serialize_char(char32_t value);
    void serialize_f32(float value);
    void serialize_f64(double value);

    void serialize_u8(uint8_t value);
    void serialize_u16(uint16_t value);
    void serialize_u32(uint32_t value);
    void serialize_u64(uint64_t value);
    void serialize_u128(const uint128_t &value);

    void serialize_i8(int8_t value);
    void serialize_i16(int16_t value);
    void serialize_i32(int32_t value);
    void serialize_i64(int64_t value);
    void serialize_i128(const int128_t &value);
    void serialize_option_tag(bool value);

    size_t get_buffer_offset();

    std::vector<uint8_t> bytes() && { return std::move(bytes_); }
};

template <class D>
class BinaryDeserializer {
    size_t pos_;

  protected:
    std::vector<uint8_t> bytes_;
    uint8_t read_byte();

  public:
    BinaryDeserializer(std::vector<uint8_t> bytes) {
        bytes_ = std::move(bytes);
        pos_ = 0;
    }

    std::string deserialize_str();

    bool deserialize_bool();
    void deserialize_unit();
    char32_t deserialize_char();
    float deserialize_f32();
    double deserialize_f64();

    uint8_t deserialize_u8();
    uint16_t deserialize_u16();
    uint32_t deserialize_u32();
    uint64_t deserialize_u64();
    uint128_t deserialize_u128();

    int8_t deserialize_i8();
    int16_t deserialize_i16();
    int32_t deserialize_i32();
    int64_t deserialize_i64();
    int128_t deserialize_i128();

    bool deserialize_option_tag();

    size_t get_buffer_offset();
};

template <class S>
void BinarySerializer<S>::serialize_str(const std::string &value) {
    static_cast<S*>(this)->serialize_len(value.size());
    for (auto c : value) {
        bytes_.push_back(c);
    }
}

template <class S>
void BinarySerializer<S>::serialize_unit() {}

template <class S>
void BinarySerializer<S>::serialize_f32(float) { throw "not implemented"; }

template <class S>
void BinarySerializer<S>::serialize_f64(double) { throw "not implemented"; }

template <class S>
void BinarySerializer<S>::serialize_char(char32_t) { throw "not implemented"; }

template <class S>
void BinarySerializer<S>::serialize_bool(bool value) {
    bytes_.push_back((uint8_t)value);
}

template <class S>
void BinarySerializer<S>::serialize_u8(uint8_t value) {
    bytes_.push_back(value);
}

template <class S>
void BinarySerializer<S>::serialize_u16(uint16_t value) {
    bytes_.push_back((uint8_t)value);
    bytes_.push_back((uint8_t)(value >> 8));
}

template <class S>
void BinarySerializer<S>::serialize_u32(uint32_t value) {
    bytes_.push_back((uint8_t)value);
    bytes_.push_back((uint8_t)(value >> 8));
    bytes_.push_back((uint8_t)(value >> 16));
    bytes_.push_back((uint8_t)(value >> 24));
}

template <class S>
void BinarySerializer<S>::serialize_u64(uint64_t value) {
    bytes_.push_back((uint8_t)value);
    bytes_.push_back((uint8_t)(value >> 8));
    bytes_.push_back((uint8_t)(value >> 16));
    bytes_.push_back((uint8_t)(value >> 24));
    bytes_.push_back((uint8_t)(value >> 32));
    bytes_.push_back((uint8_t)(value >> 40));
    bytes_.push_back((uint8_t)(value >> 48));
    bytes_.push_back((uint8_t)(value >> 56));
}

template <class S>
void BinarySerializer<S>::serialize_u128(const uint128_t &value) {
    serialize_u64(value.low);
    serialize_u64(value.high);
}

template <class S>
void BinarySerializer<S>::serialize_i8(int8_t value) {
    serialize_u8((uint8_t)value);
}

template <class S>
void BinarySerializer<S>::serialize_i16(int16_t value) {
    serialize_u16((uint16_t)value);
}

template <class S>
void BinarySerializer<S>::serialize_i32(int32_t value) {
    serialize_u32((uint32_t)value);
}

template <class S>
void BinarySerializer<S>::serialize_i64(int64_t value) {
    serialize_u64((uint64_t)value);
}

template <class S>
void BinarySerializer<S>::serialize_i128(const int128_t &value) {
    serialize_u64(value.low);
    serialize_i64(value.high);
}

template <class S>
void BinarySerializer<S>::serialize_option_tag(bool value) {
    serialize_bool(value);
}

template <class S>
size_t BinarySerializer<S>::get_buffer_offset() { return bytes_.size(); }

template <class D>
uint8_t BinaryDeserializer<D>::read_byte() { return bytes_.at(pos_++); }

template <class D>
std::string BinaryDeserializer<D>::deserialize_str() {
    auto len = static_cast<D*>(this)->deserialize_len();
    std::string result;
    result.reserve(len);
    for (size_t i = 0; i < len; i++) {
        result.push_back(read_byte());
    }
    return result;
}

template <class D>
void BinaryDeserializer<D>::deserialize_unit() {}

template <class D>
float BinaryDeserializer<D>::deserialize_f32() { throw "not implemented"; }

template <class D>
double BinaryDeserializer<D>::deserialize_f64() { throw "not implemented"; }

template <class D>
char32_t BinaryDeserializer<D>::deserialize_char() { throw "not implemented"; }

template <class D>
bool BinaryDeserializer<D>::deserialize_bool() { return (bool)read_byte(); }

template <class D>
uint8_t BinaryDeserializer<D>::deserialize_u8() { return read_byte(); }

template <class D>
uint16_t BinaryDeserializer<D>::deserialize_u16() {
    uint16_t val = 0;
    val |= (uint16_t)read_byte();
    val |= (uint16_t)read_byte() << 8;
    return val;
}

template <class D>
uint32_t BinaryDeserializer<D>::deserialize_u32() {
    uint32_t val = 0;
    val |= (uint32_t)read_byte();
    val |= (uint32_t)read_byte() << 8;
    val |= (uint32_t)read_byte() << 16;
    val |= (uint32_t)read_byte() << 24;
    return val;
}

template <class D>
uint64_t BinaryDeserializer<D>::deserialize_u64() {
    uint64_t val = 0;
    val |= (uint64_t)read_byte();
    val |= (uint64_t)read_byte() << 8;
    val |= (uint64_t)read_byte() << 16;
    val |= (uint64_t)read_byte() << 24;
    val |= (uint64_t)read_byte() << 32;
    val |= (uint64_t)read_byte() << 40;
    val |= (uint64_t)read_byte() << 48;
    val |= (uint64_t)read_byte() << 56;
    return val;
}

template <class D>
uint128_t BinaryDeserializer<D>::deserialize_u128() {
    uint128_t result;
    result.low = deserialize_u64();
    result.high = deserialize_u64();
    return result;
}

template <class D>
int8_t BinaryDeserializer<D>::deserialize_i8() {
    return (int8_t)deserialize_u8();
}

template <class D>
int16_t BinaryDeserializer<D>::deserialize_i16() {
    return (int16_t)deserialize_u16();
}

template <class D>
int32_t BinaryDeserializer<D>::deserialize_i32() {
    return (int32_t)deserialize_u32();
}

template <class D>
int64_t BinaryDeserializer<D>::deserialize_i64() {
    return (int64_t)deserialize_u64();
}

template <class D>
int128_t BinaryDeserializer<D>::deserialize_i128() {
    int128_t result;
    result.low = deserialize_u64();
    result.high = deserialize_i64();
    return result;
}

template <class D>
bool BinaryDeserializer<D>::deserialize_option_tag() {
    return deserialize_bool();
}

template <class D>
size_t BinaryDeserializer<D>::get_buffer_offset() { return pos_; }

} // end of namespace serde
