#pragma once
#include <stddef.h>
#include <stdint.h>

namespace SyncLink {
constexpr size_t FRAME_BYTES = 12;
constexpr size_t SYMBOLS_PER_FRAME = FRAME_BYTES * 8 * 2;
constexpr uint16_t TONE_ZERO_HZ = 1200;
constexpr uint16_t TONE_ONE_HZ = 2200;
constexpr uint16_t SYMBOL_MS = 20;
constexpr uint8_t VERSION = 1;
struct Packet {
  uint8_t src, dst, seq, type, command;
  uint16_t value;
};
enum class DecodeError : uint8_t { OK, BAD_SYNC, BAD_VERSION, BAD_CRC, BAD_PAIR, BAD_SYMBOL };
uint16_t crc16(const uint8_t* data, size_t size);
void pack(const Packet& packet, uint8_t out[FRAME_BYTES]);
DecodeError unpack(const uint8_t frame[FRAME_BYTES], Packet& packet);
void encodeSymbols(const uint8_t frame[FRAME_BYTES], uint8_t out[SYMBOLS_PER_FRAME]);
DecodeError decodeSymbols(const uint8_t symbols[SYMBOLS_PER_FRAME], uint8_t out[FRAME_BYTES]);
const char* errorString(DecodeError error);
}
