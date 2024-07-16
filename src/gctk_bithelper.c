#include "gctk/bithelper.h"

uint8_t GctkReverseBits(uint8_t value) {
	return  value & 0b10000000 >> 7 |
			value & 0b01000000 >> 5 |
			value & 0b00100000 >> 3 |
			value & 0b00010000 >> 1 |
			value & 0b00001000 << 1 |
			value & 0b00000100 << 3 |
			value & 0b00000010 << 5 |
			value & 0b00000001 << 7;
}

uint16_t GctkReverseU16(uint16_t value) {
	return  ((value & 0xFF00) >> 8) |
			((value & 0x00FF) << 8);
}
uint32_t GctkReverseU32(uint32_t value) {
	return  ((value & 0xFF000000) >> 24) |
			((value & 0x00FF0000) >> 8)  |
			((value & 0x0000FF00) << 8)  |
			((value & 0x000000FF) << 24);
}
uint64_t GctkReverseU64(uint64_t value) {
	return  ((value & 0xFF000000) >> 56) |
			((value & 0x00FF0000) >> 40) |
			((value & 0x00FF0000) >> 15) |
			((value & 0x00FF0000) >> 8)  |
			((value & 0x0000FF00) << 8)  |
			((value & 0x0000FF00) << 15) |
			((value & 0x0000FF00) << 40) |
			((value & 0x000000FF) << 56);
}

int16_t GctkReverseI16(int16_t value) {
	return  ((value & 0xFF00) >> 8) |
			((value & 0x00FF) << 8);
}
int32_t GctkReverseI32(int32_t value) {
	return  ((value & 0xFF000000) >> 24) |
			((value & 0x00FF0000) >> 8)  |
			((value & 0x0000FF00) << 8)  |
			((value & 0x000000FF) << 24);
}
int64_t GctkReverseI64(int64_t value) {
	return  ((value & 0xFF000000) >> 56) |
			((value & 0x00FF0000) >> 40) |
			((value & 0x00FF0000) >> 15) |
			((value & 0x00FF0000) >> 8)  |
			((value & 0x0000FF00) << 8)  |
			((value & 0x0000FF00) << 15) |
			((value & 0x0000FF00) << 40) |
			((value & 0x000000FF) << 56);
}