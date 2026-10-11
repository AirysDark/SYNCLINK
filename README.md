# SYNCLINK — Custom Two-Way RF Command and Telemetry Network

Experimental design, revision 0.1. **Not validated for 20 km and not authorized for on-air transmission.**

## Purpose
Independent network for a motorcycle, home computer, gate and light controllers, sensors, solar equipment, and optional relay towers. Remote nodes send commands, telemetry, and acknowledgements. A motorcycle may buffer logs and upload after reconnection. Initial tower uses a reused 3–5 m telescopic pole; 20+ km is an aspirational link target, not a promise.

## Radio concept
ESP32-S3 (Arduino-ESP32 **2.0.17**) generates binary data as two audio tones for an FM radio audio interface:
- 0 = 1200 Hz
- 1 = 2200 Hz
- 20 ms per tone; 50 raw tone symbols/s
- **Duplicate each bit consecutively**: 1 becomes 11, 0 becomes 00; effective 25 useful bits/s before packet overhead.
- Receiver rejects disagreeing pairs (01 or 10). Matching pairs are NOT proof of correctness: CRC-16 verifies the complete packet.
- Acknowledgments and bounded retries provide delivery confirmation. Duplicate commands are deduplicated by sequence number.
- A 12-byte example packet needs at least 3.84 s airtime at this rate, excluding preamble, PTT delay and ACK. Unsuitable for high-rate live telemetry.
- This is conventional AFSK/BFSK with repetition coding, **not a novel modulation invention**. Experimental DPIFS (frequency + pulse-interval encoding) is a separate research proposal, not implemented here.

## First prototype
1. Generate PCM tone samples in software; test without RF.
2. Encode and decode duplicate-bit frames, test CRC, deliberate corruption and noise.
3. Attach a proper audio DAC / filtered PWM with attenuation to the radio microphone input; receive audio through a level-limited, biased ADC interface. Never connect speaker output directly to an ESP32 pin.
4. PTT via transistor/opto-isolated interface suited to the specific Baofeng model. Identify board pinout and supply requirements before wiring. A Baofeng voice-FM board does not support arbitrary IQ waveforms.
5. Test using a conducted attenuated or shielded setup. Confirm applicable ACMA authorization, radio category, frequency and power limits before radiating. Amateur allocations are generally inappropriate for a private encrypted gate network.
6. Measure packet delivery, latency, receiver sensitivity, occupied bandwidth, antenna matching and link margin; then consider legal outdoor range tests.

## Antennas and range
A quarter-wave at 920 MHz is about 81.5 mm (only a starting dimension). Use a 50-ohm RF feed and measure antenna match with a VNA. Do not assume a Baofeng VHF/UHF board can operate at 920 MHz: **antenna and frequency must match the actual radio and legal band**. A 3–5 m pole and low motorcycle antenna cannot guarantee 20+ km due to terrain and Fresnel clearance. Mast safety, grounding and power-line clearance matter.

## Network and safety
Main base station + motorcycle node + short-range gate station + optional repeaters. Gate proximity must be separately verified (e.g., UWB/local sensors), not inferred solely from received signal strength. Require authenticated, anti-replay commands, safe gate controller interface, obstruction protection, and manual override. For drones, use an independent proven flight-control link and failsafe; treat this as noncritical telemetry initially.

## Repository layout
- `src/SyncLinkCodec.h/.cpp`: packet framing, CRC-16, duplicate-bit tone-symbol encoding/decoding.
- `examples/CodecSelfTest/CodecSelfTest.ino`: Arduino Serial self-test at 115200 baud.
- RF/audio/PTT hardware drivers will be added after identifying the exact radio model and validating its audio levels.

## Frame v0.1
Preamble and physical synchronization are **not implemented yet**. Fixed 12-byte frame:
`sync[2]=A5 5A | src[1] | dst[1] | seq[1] | type[1] | cmd[1] | value[2] big-endian | version[1]=01 | crc16[2] big-endian`.
CRC-16/CCITT-FALSE: polynomial 0x1021, init 0xFFFF, no reflection, xorout 0; covers the first 10 bytes.
No security or replay protection in v0.1: **do not connect this prototype to a real gate, vehicle actuator, or other safety-critical equipment**.

## Roadmap
- [x] Software packet encoding and duplicated bit mapping
- [x] CRC and round-trip test sketch
- [ ] Audio DAC/ADC driver and tone detection with timing recovery
- [ ] PTT timing and receiver squelch handling
- [ ] ACK, retries, deduplication and watchdog
- [ ] Authentication and replay protection
- [ ] Hardware measurements and legal RF tests
- [ ] Motorcycle telemetry, gate and lighting interfaces
- [ ] Relay/store-and-forward and faster data modes
