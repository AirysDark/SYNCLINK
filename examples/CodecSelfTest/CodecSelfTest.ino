// Arduino-ESP32 2.0.17; serial 115200. Software-only codec test, no RF.
#include "../../src/SyncLinkCodec.h"
using namespace SyncLink;
void setup() {
  Serial.begin(115200);
  delay(800);
  Packet tx{1,2,7,1,5,100}, rx{};
  uint8_t frame[FRAME_BYTES], rebuilt[FRAME_BYTES], symbols[SYMBOLS_PER_FRAME];
  pack(tx,frame);
  encodeSymbols(frame,symbols);
  DecodeError e=decodeSymbols(symbols,rebuilt);
  if (e==DecodeError::OK) e=unpack(rebuilt,rx);
  bool pass=(e==DecodeError::OK && rx.src==tx.src && rx.dst==tx.dst &&
    rx.seq==tx.seq && rx.type==tx.type && rx.command==tx.command && rx.value==tx.value);
  Serial.printf("Round trip: %s (%s)\n",pass?"PASS":"FAIL",errorString(e));
  symbols[0]^=1; // one copy corrupted
  e=decodeSymbols(symbols,rebuilt);
  Serial.printf("Pair mismatch detection: %s\n",e==DecodeError::BAD_PAIR?"PASS":"FAIL");
  encodeSymbols(frame,symbols);
  symbols[0]^=1; symbols[1]^=1; // both copies corrupted; pair test alone cannot detect
  e=decodeSymbols(symbols,rebuilt);
  if(e==DecodeError::OK) e=unpack(rebuilt,rx);
  Serial.printf("Double corruption CRC detection: %s (%s)\n",e==DecodeError::BAD_CRC?"PASS":"FAIL",errorString(e));
  Serial.printf("Bits: %u; tone symbols: %u; minimum airtime: %lu ms\n",
    (unsigned)(FRAME_BYTES*8),(unsigned)SYMBOLS_PER_FRAME,
    (unsigned long)(SYMBOLS_PER_FRAME*SYMBOL_MS));
}
void loop() {}
