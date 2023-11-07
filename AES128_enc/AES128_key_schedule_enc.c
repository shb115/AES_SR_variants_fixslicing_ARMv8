#include "stddefs.h"
#include <stdio.h>

static unsigned char sbox_lut[256] = {
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
	0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
	0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
	0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
	0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
	0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
	0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
	0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
	0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
	0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
	0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
	0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
	0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
	0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
	0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
	0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
	0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static unsigned char rcon[11] = {
	0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

static void packing(uint64_t* out, const unsigned char* in0, const unsigned char* in1, const unsigned char* in2, const unsigned char* in3) {
	uint64_t tmp;
	out[0] = LE_LOAD_64(in0);
	out[1] = LE_LOAD_64(in1);
	out[2] = LE_LOAD_64(in2);
	out[3] = LE_LOAD_64(in3);
	out[4] = LE_LOAD_64(in0 + 8);
	out[5] = LE_LOAD_64(in1 + 8);
	out[6] = LE_LOAD_64(in2 + 8);
	out[7] = LE_LOAD_64(in3 + 8);
	SWAPMOVE(out[1], out[0], 0x5555555555555555, 1);
	SWAPMOVE(out[3], out[2], 0x5555555555555555, 1);
	SWAPMOVE(out[5], out[4], 0x5555555555555555, 1);
	SWAPMOVE(out[7], out[6], 0x5555555555555555, 1);
	SWAPMOVE(out[2], out[0], 0x3333333333333333, 2);
	SWAPMOVE(out[3], out[1], 0x3333333333333333, 2);
	SWAPMOVE(out[6], out[4], 0x3333333333333333, 2);
	SWAPMOVE(out[7], out[5], 0x3333333333333333, 2);
	SWAPMOVE(out[4], out[0], 0x0f0f0f0f0f0f0f0f, 4);
	SWAPMOVE(out[5], out[1], 0x0f0f0f0f0f0f0f0f, 4);
	SWAPMOVE(out[6], out[2], 0x0f0f0f0f0f0f0f0f, 4);
	SWAPMOVE(out[7], out[3], 0x0f0f0f0f0f0f0f0f, 4);
	for (int i = 0; i < 8; i++)
	{
		tmp = out[i];
		out[i] = 0x0000000000000000;
		out[i] |= ((tmp & 0xf000000000000000) >> 4);
		out[i] |= ((tmp & 0x0F00000000000000) >> 8);
		out[i] |= ((tmp & 0x00F0000000000000) >> 12);
		out[i] |= ((tmp & 0x000F000000000000) >> 16);
		out[i] |= ((tmp & 0x0000F00000000000) >> 20);
		out[i] |= ((tmp & 0x00000F0000000000) >> 24);
		out[i] |= ((tmp & 0x000000F000000000) >> 28);
		out[i] |= ((tmp & 0x0000000F00000000) >> 32);
		out[i] |= ((tmp & 0x00000000F0000000) << 32);
		out[i] |= ((tmp & 0x000000000F000000) << 28);
		out[i] |= ((tmp & 0x0000000000F00000) << 24);
		out[i] |= ((tmp & 0x00000000000F0000) << 20);
		out[i] |= ((tmp & 0x000000000000F000) << 16);
		out[i] |= ((tmp & 0x0000000000000F00) << 12);
		out[i] |= ((tmp & 0x00000000000000F0) << 8);
		out[i] |= ((tmp & 0x000000000000000f) << 4);
	}	
}

void AES128_key_schedule_enc(uint64_t* rkeys_ffs, const unsigned char* key){
	uint64_t t0, t1, tmp;
	uint64_t rkeys[22];
	int i;
	// key schedule in the classical representation
	rkeys[0] = LE_LOAD_64(key);
	rkeys[1] = LE_LOAD_64(key + 8);
	for(int i = 2; i < 22; i+=2) {
		rkeys[i] = (rkeys[i-2] & 0x00000000FFFFFFFF) ^ ((uint64_t)rcon[i/2]);
		// First  32bit
		rkeys[i] ^= ((uint64_t)sbox_lut[(rkeys[i-1] & 0x000000ff00000000) >> 32] << 24);
		rkeys[i] ^= ((uint64_t)sbox_lut[(rkeys[i-1] & 0x0000ff0000000000) >> 40]);
		rkeys[i] ^= ((uint64_t)sbox_lut[(rkeys[i-1] & 0x00ff000000000000) >> 48] << 8);
		rkeys[i] ^= ((uint64_t)sbox_lut[(rkeys[i-1] & 0xff00000000000000) >> 56] << 16);
		// Second 32bit
		rkeys[i] ^= (rkeys[i] << 32) ^ (rkeys[i-2] & 0xFFFFFFFF00000000);
		// Third  32bit
		rkeys[i+1] = ((rkeys[i] & 0xFFFFFFFF00000000) >> 32) ^ (rkeys[i-1] & 0x00000000FFFFFFFF);
		// Fourth 32bit
		rkeys[i+1] ^= (rkeys[i+1] << 32) ^ (rkeys[i-1] & 0xFFFFFFFF00000000);
	}
	// applying ShiftRows^(-i) to match the fully-fixsliced representation
	for(int i = 2; i < 20; i+=2) {
		t0 = rkeys[i];
		t1 = rkeys[i+1];
		switch ((i/2) % 4) {
			case 1: 					// Applies ShiftRows^(-1)
				// First  32bit
				rkeys[i] 	&= 0x000000FF000000FF;
				rkeys[i]	|= ((t0 & 0xFF00000000000000) >> 32);				
				rkeys[i]	|= ((t1 & 0x0000000000FF0000));			
				rkeys[i]	|= ((t1 & 0x0000FF0000000000) >> 32);	
				// Second 32bit
				rkeys[i]	|= ((t0 & 0x000000000000FF00) << 32);
				rkeys[i]	|= ((t1 & 0x00000000FF000000) << 32);
				rkeys[i]	|= ((t1 & 0x00FF000000000000));
				// Third  32bit
				rkeys[i+1] 	&= 0x000000FF000000FF;
				rkeys[i+1]	|= ((t0 & 0x0000000000FF0000));
				rkeys[i+1]	|= ((t0 & 0x0000FF0000000000) >> 32);				
				rkeys[i+1]	|= ((t1 & 0xFF00000000000000) >> 32);
				// Fourth 32bit
				rkeys[i+1]	|= ((t0 & 0x00000000FF000000) << 32);
				rkeys[i+1]	|= ((t0 & 0x00FF000000000000));
				rkeys[i+1]	|= ((t1 & 0x000000000000FF00) << 32);
				break;
			case 2: 					// Applies ShiftRows^(-2)
				SWAPMOVE(rkeys[i+1], rkeys[i], 0xFF00FF00FF00FF00, 0);
				break;
			case 3: 					// Applies ShiftRows^(-3)
				// First  32bit
				rkeys[i] 	&= 0x000000FF000000FF;
				rkeys[i]	|= ((t1 & 0xFF00000000000000) >> 32);
				rkeys[i]	|= ((t1 & 0x0000000000FF0000));
				rkeys[i]	|= ((t0 & 0x0000FF0000000000) >> 32);
				// Second 32bit
				rkeys[i]	|= ((t0 & 0x00000000FF000000) << 32);
				rkeys[i]	|= ((t1 & 0x000000000000FF00) << 32);
				rkeys[i]	|= ((t1 & 0x00FF000000000000));
				// Third  32bit
				rkeys[i+1] 	&= 0x000000FF000000FF;
				rkeys[i+1]	|= ((t1 & 0x0000FF0000000000) >> 32);
				rkeys[i+1]	|= ((t0 & 0x0000000000FF0000));
				rkeys[i+1]	|= ((t0 & 0xFF00000000000000) >> 32);
				// Fourth 32bit
				rkeys[i+1]	|= ((t0 & 0x000000000000FF00) << 32);
				rkeys[i+1]	|= ((t0 & 0x00FF000000000000));
				rkeys[i+1]	|= ((t1 & 0x00000000FF000000) << 32);
				break;
		}
	}
	// PACKING all round keys to match the fully-fixsliced representation
	packing(rkeys_ffs, (unsigned char*)rkeys, (unsigned char*)rkeys, (unsigned char*)rkeys, (unsigned char*)rkeys);
	for(int i = 1; i < 11; i++) {
		packing(rkeys_ffs + i*8, (unsigned char*)(rkeys + i*2), (unsigned char*)(rkeys + i*2), (unsigned char*)(rkeys + i*2), (unsigned char*)(rkeys + i*2));
		rkeys_ffs[i*8 + 1] ^= 0xffffffffffffffff; 	// NOT to speed up SBox calculations
		rkeys_ffs[i*8 + 2] ^= 0xffffffffffffffff; 	// NOT to speed up SBox calculations
		rkeys_ffs[i*8 + 6] ^= 0xffffffffffffffff; 	// NOT to speed up SBox calculations
		rkeys_ffs[i*8 + 7] ^= 0xffffffffffffffff; 	// NOT to speed up SBox calculations
	}
}