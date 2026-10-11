#include "SyncLinkCodec.h"

namespace SyncLink {
uint16_t crc16(const uint8_t* data, size_t size) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < size; ++i) {
    crc ^= static_cast<uint16_t>(data[i]) << 8;
    for (uint8_t b = 0; b < 8; ++b)
      crc = (crc & 0x8000) ? static_cast<uint16_t>((crc << 1) ^ 0x1021) : static_cast<uint16_t>(crc << 1);
  }
  return crc;
}
void pack(const Packet& p, uint8_t out[FRAME_BYTES]) {
  out[0]=0xA5; out[1]=0x5A; out[2]=p.src; out[3]=p.dst;
  out[4]=p.seq; out[5]=p.type; out[6]=p.command;
  out[7]=static_cast<uint8_t>(p.value >> 8); out[8]=static_cast<uint8_t>(p.value);
  out[9]=VERSION;
  const uint16_t crc=crc16(out,10);
  out[10]=static_cast<uint8_t>(crc>>8); out[11]=static_cast<uint8_t>(crc);
}
DecodeError unpack(const uint8_t in[FRAME_BYTES], Packet& p) {
  if (in[0]!=0xA5 || in[1]!=0x5A) return DecodeError::BAD_SYNC;
  if (in[9]!=VERSION) return DecodeError::BAD_VERSION;
  if (static_cast<uint16_t>((in[10]<<8)|in[11]) != crc16(in,10)) return DecodeError::BAD_CRC;
  p={in[2],in[3],in[4],in[5],in[6],static_cast<uint16_t>((in[7]<<8)|in[8])};
  return DecodeError::OK;
}
void encodeSymbols(const uint8_t in[FRAME_BYTES], uint8_t out[SYMBOLS_PER_FRAME]) {
  size_t k=0;
  for (size_t i=0;i<FRAME_BYTES;++i)
    for (int b=7;b>=0;--b) {
      const uint8_t bit=(in[i]>>b)&1;
      out[k++]=bit; out[k++]=bit;
    }
}
DecodeError decodeSymbols(const uint8_t in[SYMBOLS_PER_FRAME], uint8_t out[FRAME_BYTES]) {
  size_t k=0;
  for (size_t i=0;i<FRAME_BYTES;++i) {
    uint8_t byte=0;
    for (uint8_t b=0;b<8;++b) {
      const uint8_t a=in[k++], c=in[k++];
      if (a>1 || c>1) return DecodeError::BAD_SYMBOL;
      if (a!=c) return DecodeError::BAD_PAIR;
      byte=static_cast<uint8_t>((byte<<1)|a);
    }
    out[i]=byte;
  }
  return DecodeError::OK;
}
const char* errorString(DecodeError e) {
  switch(e) {
    case DecodeError::OK: return "OK";
    case DecodeError::BAD_SYNC: return "BAD_SYNC";
    case DecodeError::BAD_VERSION: return "BAD_VERSION";
    case DecodeError::BAD_CRC: return "BAD_CRC";
    case DecodeError::BAD_PAIR: return "BAD_PAIR";
    default: return "BAD_SYMBOL";
  }
}
}
