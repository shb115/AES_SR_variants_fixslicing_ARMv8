#ifndef __AES_H__
#define __AES_H__

#include <stdint.h>
#include <stdlib.h>

#define AES128_ROUNDS	 10
#define AES128_BLOCK_BSZ 128
#define AES128_KEY_BSZ 	 128
#define AES128_BLOCK_LEN AES128_BLOCK_BSZ/8
#define AES128_KEY_LEN 	 AES128_KEY_BSZ/8
#define AES128_BLOCK_NUM 4

typedef uint8_t 	AES128_STATE_t[AES128_BLOCK_LEN];

void AES128_enc(AES128_STATE_t C0, AES128_STATE_t C1, AES128_STATE_t C2, AES128_STATE_t C3,	
				const AES128_STATE_t P0, const AES128_STATE_t P1, const AES128_STATE_t P2, const AES128_STATE_t P3,
				const uint64_t rk[AES128_BLOCK_NUM * 2 * (AES128_ROUNDS + 1)]);

void AES128_key_schedule_enc(uint64_t rk[AES128_BLOCK_NUM * 2 * (AES128_ROUNDS + 1)], const uint8_t mk[AES128_KEY_LEN]);

#define ROR(x,y) 		(((x) >> (y)) | ((x) << (64 - (y))))

#define BYTE_ROR_12(x) 										\
	((((x) >> 12) & 0x000F000F000F000F)	| (((x) & 0x0FFF0FFF0FFF0FFF) << 4))

#define BYTE_ROR_8(x) 										\
	((((x) >> 8)  & 0x00FF00FF00FF00FF)	| (((x) & 0x00FF00FF00FF00FF) << 8))

#define BYTE_ROR_4(x) 										\
	((((x) >> 4)  & 0x0FFF0FFF0FFF0FFF)	| (((x) & 0x000F000F000F000F) << 12))

#define SWAPMOVE(a, b, mask, n)	({							\
	tmp = (b ^ (a >> n)) & mask;							\
	b ^= tmp;												\
	a ^= (tmp << n);										\
})

#define LE_LOAD_64(x) 									\
	(((uint64_t)((x)[7])) << 56) 	| 					\
	(((uint64_t)((x)[6])) << 48) 	| 					\
	(((uint64_t)((x)[5])) << 40) 	| 					\
	(((uint64_t)((x)[4])) << 32)	|					\
    (((uint64_t)((x)[3])) << 24)  	| 					\
    (((uint64_t)((x)[2])) << 16)  	| 					\
    (((uint64_t)((x)[1])) << 8)   	| 					\
    ((uint64_t)((x)[0]))

#define LE_STORE_64(x, y)									\
	(x)[0] = (y) & 0xff; 									\
	(x)[1] = ((y) >> 8) & 0xff; 							\
	(x)[2] = ((y) >> 16) & 0xff; 							\
	(x)[3] = ((y) >> 24) & 0xff;							\
	(x)[4] = ((y) >> 32) & 0xff;							\
	(x)[5] = ((y) >> 40) & 0xff;							\
	(x)[6] = ((y) >> 48) & 0xff;							\
	(x)[7] = (y) >> 56;										\

#endif