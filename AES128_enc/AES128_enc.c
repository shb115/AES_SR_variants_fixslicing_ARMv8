#include "stddefs.h"

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

static void unpacking(unsigned char* out0, unsigned char* out1, unsigned char* out2, unsigned char* out3, uint64_t* in) {
	uint64_t tmp;
	for (int i = 0; i < 8; i++)
	{
		tmp = in[i];
		in[i] = 0x0000000000000000;
		in[i] |= ((tmp & 0xF000000000000000) >> 32);
		in[i] |= ((tmp & 0x0F00000000000000) << 4);
		in[i] |= ((tmp & 0x00F0000000000000) >> 28);
		in[i] |= ((tmp & 0x000F000000000000) << 8);
		in[i] |= ((tmp & 0x0000F00000000000) >> 24);
		in[i] |= ((tmp & 0x00000F0000000000) << 12);
		in[i] |= ((tmp & 0x000000F000000000) >> 20);
		in[i] |= ((tmp & 0x0000000F00000000) << 16);
		in[i] |= ((tmp & 0x00000000F0000000) >> 16);
		in[i] |= ((tmp & 0x000000000F000000) << 20);
		in[i] |= ((tmp & 0x0000000000F00000) >> 12);
		in[i] |= ((tmp & 0x00000000000F0000) << 24);
		in[i] |= ((tmp & 0x000000000000F000) >> 8);
		in[i] |= ((tmp & 0x0000000000000F00) << 28);
		in[i] |= ((tmp & 0x00000000000000F0) >> 4);
		in[i] |= ((tmp & 0x000000000000000f) << 32);
	}
	SWAPMOVE(in[4], in[0], 0x0f0f0f0f0f0f0f0f, 4);
	SWAPMOVE(in[5], in[1], 0x0f0f0f0f0f0f0f0f, 4);
	SWAPMOVE(in[6], in[2], 0x0f0f0f0f0f0f0f0f, 4);
	SWAPMOVE(in[7], in[3], 0x0f0f0f0f0f0f0f0f, 4);
	SWAPMOVE(in[2], in[0], 0x3333333333333333, 2);
	SWAPMOVE(in[3], in[1], 0x3333333333333333, 2);
	SWAPMOVE(in[6], in[4], 0x3333333333333333, 2);
	SWAPMOVE(in[7], in[5], 0x3333333333333333, 2);
	SWAPMOVE(in[1], in[0], 0x5555555555555555, 1);
	SWAPMOVE(in[3], in[2], 0x5555555555555555, 1);
	SWAPMOVE(in[5], in[4], 0x5555555555555555, 1);
	SWAPMOVE(in[7], in[6], 0x5555555555555555, 1);
	LE_STORE_64(out0, in[0]);
	LE_STORE_64(out0 + 8, in[4]);
	LE_STORE_64(out1, in[1]);
	LE_STORE_64(out1 + 8, in[5]);
	LE_STORE_64(out2, in[2]);
	LE_STORE_64(out2 + 8, in[6]);
	LE_STORE_64(out3, in[3]);
	LE_STORE_64(out3 + 8, in[7]);
}

static void ark(uint64_t* state, const uint64_t* rkey) {
    for(int i = 0; i < 8; i++)
		state[i] ^= rkey[i];
}

void sbox(uint64_t* state) {
	uint64_t t0, t1, t2, t3, t4, t5,
		t6, t7, t8, t9, t10, t11, t12,
		t13, t14, t15, t16, t17;
	t0			= state[3] ^ state[5];
	t1			= state[0] ^ state[6];
	t2			= t1 ^ t0;
	t3			= state[4] ^ t2;
	t4			= t3 ^ state[5];
	t5			= t2 & t4;
	t6			= t4 ^ state[7];
	t7			= t3 ^ state[1];
	t8			= state[0] ^ state[3]; 
	t9			= t7 ^ t8;
	t10			= t8 & t9;
	t11			= state[7] ^ t9; 
	t12			= state[0] ^ state[5];
	t13			= state[1] ^ state[2];
	t14			= t4 ^ t13;
	t15			= t14 ^ t9;
	t16			= t0 & t15;
	t17			= t16 ^ t10;
	state[1]	= t14 ^ t12; 
	state[2]	= t12 & t14;
	state[2] 	^= t10;
	state[4]	= t13 ^ t9;
	state[5]	= t1 ^ state[4];
	t3			= t1 & state[4];
	t10			= state[0] ^ state[4];
	t13 		^= state[7];
	state[3] 	^= t13; 
	t16			= state[3] & state[7];
	t16 		^= t5;
	t16 		^= state[2];
	state[1] 	^= t16;
	state[0] 	^= t13;
	t16			= state[0] & t11;
	t16 		^= t3;
	state[2] 	^= t16;
	state[2] 	^= t10;
	state[6] 	^= t13;
	t10			= state[6] & t13;
	t3 			^= t10;
	t3 			^= t17;
	state[5] 	^= t3;
	t3			= state[6] ^ t12;
	t10			= t3 & t6;
	t5 			^= t10;
	t5 			^= t7;
	t5 			^= t17;
	t7			= t5 & state[5];
	t10			= state[2] ^ t7;
	t7 			^= state[1];
	t5 			^= state[1];
	t16			= t5 & t10;
	state[1] 	^= t16;
	t17			= state[1] & state[0];
	t11			= state[1] & t11;
	t16			= state[5] ^ state[2];
	t7 			&= t16;
	t7 			^= state[2];
	t16			= t10 ^ t7;
	state[2] 	&= t16;
	t10 		^= state[2];
	t10 		&= state[1];
	t5 			^= t10;
	t10			= state[1] ^ t5;
	state[4] 	&= t10; 
	t11 		^= state[4];
	t1 			&= t10;
	state[6] 	&= t5; 
	t10			= t5 & t13;
	state[4] 	^= t10;
	state[5] 	^= t7;
	state[2] 	^= state[5];
	state[5]	= t5 ^ state[2];
	t5			= state[5] & t14;
	t10			= state[5] & t12;
	t12			= t7 ^ state[2];
	t4 			&= t12;
	t2 			&= t12;
	t3 			&= state[2]; 
	state[2] 	&= t6;
	state[2] 	^= t4;
	t13			= state[4] ^ state[2];
	state[3] 	&= t7;
	state[1] 	^= t7;
	state[5] 	^= state[1];
	t6			= state[5] & t15;
	state[4] 	^= t6; 
	t0 			&= state[5];
	state[5]	= state[1] & t9; 
	state[5] 	^= state[4];
	state[1] 	&= t8;
	t6			= state[1] ^ state[5];
	t0 			^= state[1];
	state[1]	= t3 ^ t0;
	t15			= state[1] ^ state[3];
	t2 			^= state[1];
	state[0]	= t2 ^ state[5];
	state[3]	= t2 ^ t13;
	state[1]	= state[3] ^ state[5];
	//state[1] 	^= 0xffffffff;
	t0 			^= state[6];
	state[5]	= t7 & state[7];
	t14			= t4 ^ state[5];
	state[6]	= t1 ^ t14;
	state[6] 	^= t5; 
	state[6] 	^= state[4];
	state[2]	= t17 ^ state[6];
	state[5]	= t15 ^ state[2];
	state[2] 	^= t6;
	state[2] 	^= t10;
	//state[2] 	^= 0xffffffff;
	t14 		^= t11;
	t0 			^= t14;
	state[6] 	^= t0;
	//state[6] 	^= 0xffffffff;
	state[7]	= t1 ^ t0;
	//state[7] 	^= 0xffffffff;
	state[4]	= t14 ^ state[3]; 
}

static void double_shiftrows(uint64_t* state) {
    uint64_t tmp;
	for(int i = 0; i < 8; i++)
        SWAPMOVE(state[i], state[i], 0x00ff000000ff0000, 8);
}

static void mixcolumns_0(uint64_t* state) {
	uint64_t t0, t1, t2, t3, t4;
	t3 = ROR(BYTE_ROR_12(state[0]),16);
	t0 = state[0] ^ t3;
	t1 = ROR(BYTE_ROR_12(state[7]),16);
	t2 = state[7] ^ t1;
	state[7] = ROR(BYTE_ROR_8(t2),32) ^ t1 ^ t0;
	t1 = ROR(BYTE_ROR_12(state[6]),16);
	t4 = t1 ^ state[6];
	state[6] = t2 ^ t0 ^ t1 ^ ROR(BYTE_ROR_8(t4),32);
	t1 = ROR(BYTE_ROR_12(state[5]),16);
	t2 = t1 ^ state[5];
	state[5] = t4 ^ t1 ^ ROR(BYTE_ROR_8(t2),32);
	t1 = ROR(BYTE_ROR_12(state[4]),16);
	t4 = t1 ^ state[4];
	state[4] = t2 ^ t0 ^ t1 ^ ROR(BYTE_ROR_8(t4),32);
	t1 = ROR(BYTE_ROR_12(state[3]),16);
	t2 = t1 ^ state[3];
	state[3] = t4 ^ t0 ^ t1 ^ ROR(BYTE_ROR_8(t2),32);
	t1 = ROR(BYTE_ROR_12(state[2]),16);
	t4 = t1 ^ state[2];
	state[2] = t2 ^ t1 ^ ROR(BYTE_ROR_8(t4),32);
	t1 = ROR(BYTE_ROR_12(state[1]),16);
	t2 = t1 ^ state[1];
	state[1] = t4 ^ t1 ^ ROR(BYTE_ROR_8(t2),32);
	state[0] = t2 ^ t3 ^ ROR(BYTE_ROR_8(t0),32);
}

static void mixcolumns_1(uint64_t* state) {
	uint64_t t0, t1, t2;
	t0 = state[0] ^ ROR(BYTE_ROR_8(state[0]),16);
	t1 = state[7] ^ ROR(BYTE_ROR_8(state[7]),16);
	t2 = state[6];
	state[6] = t1 ^ t0;
	state[7] ^= state[6] ^ ROR(t1,32);
	t1 =  ROR(BYTE_ROR_8(t2),16);
	state[6] ^= t1;
	t1 ^= t2;
	state[6] ^= ROR(t1,32);
	t2 = state[5];
	state[5] = t1;
	t1 =  ROR(BYTE_ROR_8(t2),16);
	state[5] ^= t1;
	t1 ^= t2;
	state[5] ^= ROR(t1,32);
	t2 = state[4];
	state[4] = t1 ^ t0;
	t1 =  ROR(BYTE_ROR_8(t2),16);
	state[4] ^= t1;
	t1 ^= t2;
	state[4] ^= ROR(t1,32);
	t2 = state[3];
	state[3] = t1 ^ t0;
	t1 =  ROR(BYTE_ROR_8(t2),16);
	state[3] ^= t1;
	t1 ^= t2;
	state[3] ^= ROR(t1,32);
	t2 = state[2];
	state[2] = t1;
	t1 = ROR(BYTE_ROR_8(t2),16);
	state[2] ^= t1;
	t1 ^= t2;
	state[2] ^= ROR(t1,32);
	t2 = state[1];
	state[1] = t1;
	t1 = ROR(BYTE_ROR_8(t2),16);
	state[1] ^= t1;
	t1 ^= t2;
	state[1] ^= ROR(t1,32);
	t2 = state[0];
	state[0] = t1;
	t1 = ROR(BYTE_ROR_8(t2),16);
	state[0] ^= t1;
	t1 ^= t2;
	state[0] ^= ROR(t1,32);
}

static void mixcolumns_2(uint64_t* state) {
	uint64_t t0, t1, t2, t3, t4;
	t3 = ROR(BYTE_ROR_4(state[0]),16);
	t0 = state[0] ^ t3;
	t1 = ROR(BYTE_ROR_4(state[7]),16);
	t2 = state[7] ^ t1;
	state[7] = ROR(BYTE_ROR_8(t2),32) ^ t1 ^ t0;
	t1 = ROR(BYTE_ROR_4(state[6]),16);
	t4 = t1 ^ state[6];
	state[6] = t2 ^ t0 ^ t1 ^ ROR(BYTE_ROR_8(t4),32);
	t1 = ROR(BYTE_ROR_4(state[5]),16);
	t2 = t1 ^ state[5];
	state[5] = t4 ^ t1 ^ ROR(BYTE_ROR_8(t2),32);
	t1 = ROR(BYTE_ROR_4(state[4]),16);
	t4 = t1 ^ state[4];
	state[4] = t2 ^ t0 ^ t1 ^ ROR(BYTE_ROR_8(t4),32);
	t1 = ROR(BYTE_ROR_4(state[3]),16);
	t2 = t1 ^ state[3];
	state[3] = t4 ^ t0 ^ t1 ^ ROR(BYTE_ROR_8(t2),32);
	t1 = ROR(BYTE_ROR_4(state[2]),16);
	t4 = t1 ^ state[2];
	state[2] = t2 ^ t1 ^ ROR(BYTE_ROR_8(t4),32);
	t1 = ROR(BYTE_ROR_4(state[1]),16);
	t2 = t1 ^ state[1];
	state[1] = t4 ^ t1 ^ ROR(BYTE_ROR_8(t2),32);
	state[0] = t2 ^ t3 ^ ROR(BYTE_ROR_8(t0),32);
}

static void mixcolumns_3(uint64_t* state) {
	uint64_t t0, t1, t2;
	t0 = state[7] ^ ROR(state[7],16);
	t2 = state[0] ^ ROR(state[0],16);
	state[7] = t2 ^ ROR(state[7],16) ^ ROR(t0, 32);
	t1 = state[6] ^ ROR(state[6],16);
	state[6] = t0 ^ t2 ^ ROR(state[6], 16) ^ ROR(t1,32);
	t0 = state[5] ^ ROR(state[5],16);
	state[5] = t1 ^ ROR(state[5],16) ^ ROR(t0,32);
	t1 = state[4] ^ ROR(state[4],16);
	state[4] = t0 ^ t2 ^ ROR(state[4],16) ^ ROR(t1,32);
	t0 = state[3] ^ ROR(state[3],16);
	state[3] = t1 ^ t2 ^ ROR(state[3],16) ^ ROR(t0,32);
	t1 = state[2] ^ ROR(state[2],16);
	state[2] = t0 ^ ROR(state[2],16) ^ ROR(t1,32);
	t0 = state[1] ^ ROR(state[1],16);
	state[1] = t1 ^ ROR(state[1],16) ^ ROR(t0,32);
	state[0] = t0 ^ ROR(state[0],16) ^ ROR(t2,32);
}

void AES128_enc(unsigned char* ctext0, unsigned char* ctext1, 
                        unsigned char* ctext2, unsigned char* ctext3, 
						const unsigned char* ptext0, const unsigned char* ptext1, 
						const unsigned char* ptext2, const unsigned char* ptext3, 
						const uint64_t* rkeys_ffs) {
	uint64_t state[8]; 									// 256-bit internal state
	packing(state, ptext0, ptext1, ptext2, ptext3);		// packs into bitsliced representation
	ark(state, rkeys_ffs); 								// key whitening
	sbox(state); 										// 1st round
	mixcolumns_0(state); 								// 1st round
	ark(state, rkeys_ffs + 8); 							// 1st round
	sbox(state); 										// 2nd round
	mixcolumns_1(state); 								// 2nd round
	ark(state, rkeys_ffs + 16); 						// 2nd round	
	sbox(state); 										// 3rd round
	mixcolumns_2(state); 								// 3rd round
	ark(state, rkeys_ffs + 24); 						// 3rd round
	sbox(state); 										// 4th round
	mixcolumns_3(state); 								// 4th round
	ark(state, rkeys_ffs + 32); 						// 4th round
	sbox(state); 										// 5th round
	mixcolumns_0(state); 								// 5th round
	ark(state, rkeys_ffs + 40); 						// 5th round
	sbox(state);										// 6th round
	mixcolumns_1(state); 								// 6th round
	ark(state, rkeys_ffs + 48); 						// 6th round
	sbox(state); 										// 7th round
	mixcolumns_2(state); 								// 7th round
	ark(state, rkeys_ffs + 56); 						// 7th round
	sbox(state); 										// 8th round
	mixcolumns_3(state); 								// 8th round
	ark(state, rkeys_ffs + 64); 						// 8th round
	sbox(state); 										// 9th round
	mixcolumns_0(state); 								// 9th round
	ark(state, rkeys_ffs + 72); 						// 9th round
	sbox(state); 										// 10th round
	double_shiftrows(state); 							// 10th round (resynchronization)
	ark(state, rkeys_ffs + 80); 						// 10th round
	unpacking(ctext0, ctext1, ctext2, ctext3, state);	// unpacks the state to the output
}