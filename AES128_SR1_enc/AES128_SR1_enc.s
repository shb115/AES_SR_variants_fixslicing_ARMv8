/******************************************************************************
* Assembly fixsliced implementation of AES-variants (encryption only).
*
* Code can be checked on .
*
* Code is based on https://github.com/aadomn/aes.
*
* @author   Hanbeom Shin, Korea University, South Korea
*           newonetiger@korea.ac.kr
*
* @date     October 2023
******************************************************************************/

.text

/******************************************************************************
* Macro to compute the SWAPMOVE technique: swap the bits in 'in1' masked by 'm'
* by the bits in 'in0' masked by 'm << n' and put the results in 'out0', 'out1'
******************************************************************************/
.macro swpmv out0, out1, in0, in1, m, n, tmp
    eor     \tmp, \in1, \in0, lsr \n
    and     \tmp, \tmp, \m
    eor     \out1, \in1, \tmp
    eor     \out0, \in0, \tmp, lsl \n
.endm

.macro inalign in    
    mov     x19, \in
    mov     \in, #0
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #4
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #8
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #12
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #16
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #20
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #24
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #28
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #32
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #32
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #28
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #24
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #20
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #16
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #12
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #8
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #4
.endm

.macro oualign in
    mov     x19, \in
    mov     \in, #0
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #32
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #4
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #28
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #8
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #24
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #12
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #20
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #16
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #16
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #20
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #12
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #24
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #8
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #28
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsr #4
    lsr     x20, x20, #4
    and     x21, x19, x20
    orr     \in, \in, x21, lsl #32
.endm

.macro byteror  out, in, m, n0, n1, tmp
    and     \out, \m, \in, lsr \n0
    bic     \tmp, \in, \m, ror \n1
    orr     \out, \out, \tmp, lsl \n1
.endm

/******************************************************************************
* Packs four 128-bit input blocs stored in reg0-reg7
* , respectively, into the 512-bit internal state
* reg0 = a_63  || ... || a_0
* reg1 = b_63  || ... || b_0
* reg2 = c_63  || ... || c_0
* reg3 = d_63  || ... || d_0
* reg4 = a_127 || ... || a_64
* reg5 = b_127 || ... || b_64
* reg6 = c_127 || ... || c_64
* reg7 = d_127 || ... || d_64
*
* =>
*
* reg0 = r_24 r_56 r_88 r_120 || ... || r_0 r_32 r_64 r_96
* reg1 = r_25 r_57 r_89 r_121 || ... || r_1 r_33 r_65 r_97
* reg2 = r_26 r_58 r_90 r_122 || ... || r_2 r_34 r_66 r_98
* reg3 = r_27 r_59 r_91 r_123 || ... || r_3 r_35 r_67 r_99
* reg4 = r_28 r_60 r_92 r_124 || ... || r_4 r_36 r_68 r_100
* reg5 = r_29 r_61 r_93 r_125 || ... || r_5 r_37 r_69 r_101
* reg6 = r_30 r_62 r_94 r_126 || ... || r_6 r_38 r_70 r_102
* reg7 = r_31 r_63 r_95 r_127 || ... || r_7 r_39 r_71 r_103
******************************************************************************/
.align 2
packing:
    mov     x19, #0x0f0f
    movk    x19, #0x0f0f, lsl #16
    movk    x19, #0x0f0f, lsl #32
    movk    x19, #0x0f0f, lsl #48               // x19 <- 0x0f0f0f0f0f0f0f0f (mask for SWAPMOVE)
    eor     x20, x19, x19, lsl #2               // x20 <- 0x3333333333333333 (mask for SWAPMOVE)
    eor     x21, x20, x20, lsl #1               // x21 <- 0x18555555555555555 (mask for SWAPMOVE)

    swpmv   x11, x9,  x11, x9,  x21, #1, x22
    swpmv   x12, x10, x12, x10, x21, #1, x22
    swpmv   x15, x13, x15, x13, x21, #1, x22
    swpmv   x16, x14, x16, x14, x21, #1, x22
    

    swpmv   x23, x9,  x13, x9,  x20, #2, x22
    swpmv   x24, x13, x14, x10, x20, #2, x22
    swpmv   x25, x10, x15, x11, x20, #2, x22
    swpmv   x16, x14, x16, x12, x20, #2, x22
    
    swpmv   x13, x9,  x13, x9,  x19, #4, x22
    swpmv   x14, x10, x14, x10, x19, #4, x22
    swpmv   x15, x11, x24, x23, x19, #4, x22
    swpmv   x16, x12, x16, x25, x19, #4, x22

    mov     x20, #0
    movk    x20, #0xf000, lsl #48
    inalign x9
    lsl     x20, x20, #60
    inalign x10
    lsl     x20, x20, #60
    inalign x11
    lsl     x20, x20, #60
    inalign x12
    lsl     x20, x20, #60
    inalign x13
    lsl     x20, x20, #60
    inalign x14
    lsl     x20, x20, #60
    inalign x15
    lsl     x20, x20, #60
    inalign x16
    
    ret

/******************************************************************************
* Unpacks the 512-bit internal state in two 128-bit blocs.
******************************************************************************/
.align 2
unpacking:
    mov     x20, #0
    movk    x20, #0xf000, lsl #48
    oualign x9
    lsl     x20, x20, #60
    oualign x10
    lsl     x20, x20, #60
    oualign x11
    lsl     x20, x20, #60
    oualign x12
    lsl     x20, x20, #60
    oualign x13
    lsl     x20, x20, #60
    oualign x14
    lsl     x20, x20, #60
    oualign x15
    lsl     x20, x20, #60
    oualign x16

    mov     x19, #0x0f0f
    movk    x19, #0x0f0f, lsl #16
    movk    x19, #0x0f0f, lsl #32
    movk    x19, #0x0f0f, lsl #48               // x19 <- 0x0f0f0f0f0f0f0f0f (mask for SWAPMOVE)
    eor     x20, x19, x19, lsl #2               // x20 <- 0x3333333333333333 (mask for SWAPMOVE)
    eor     x21, x20, x20, lsl #1               // x21 <- 0x18555555555555555 (mask for SWAPMOVE)

    swpmv   x16, x25, x16, x12, x19, #4, x22
    swpmv   x24, x23, x15, x11, x19, #4, x22
    swpmv   x14, x10, x14, x10, x19, #4, x22
    swpmv   x13, x9,  x13, x9,  x19, #4, x22
    
    swpmv   x16, x12, x16, x14, x20, #2, x22
    swpmv   x15, x11, x25, x10, x20, #2, x22
    swpmv   x14, x10, x24, x13, x20, #2, x22
    swpmv   x13, x9,  x23, x9,  x20, #2, x22
    
    swpmv   x11, x9,  x11, x9,  x21, #1, x22
    swpmv   x12, x10, x12, x10, x21, #1, x22
    swpmv   x15, x13, x15, x13, x21, #1, x22
    swpmv   x16, x14, x16, x14, x21, #1, x22   

    ret

/******************************************************************************
* Subroutine that computes the AddRoundKey and the S-box.
* Credits to https://github.com/Ko-/aes-armcortexm for the S-box implementation
******************************************************************************/
.macro ark_sbox
    // add round key
    ldr     x19, [sp, #8]
    ldr     x20, [sp, #16]
    ldr     x21, [sp, #24]
    ldr     x22, [sp, #32]
    ldr     x23, [sp, #40]
    ldr     x24, [sp, #48]
    ldr     x25, [sp, #56]
    ldr     x26, [sp, #64]
    eor     x9,  x9,  x19
    eor     x10, x10, x20
    eor     x11, x11, x21
    eor     x12, x12, x22
    eor     x13, x13, x23
    eor     x14, x14, x24
    eor     x15, x15, x25
    eor     x16, x16, x26
    add     sp, sp, #64
    //sbox: credits to https://github.com/Ko-/aes-armcortexm
    eor     x19, x12, x14
	eor     x20, x9,  x15
	eor     x21, x20, x19
	eor     x22, x13, x21
	eor     x23, x22, x14
	and     x24, x21, x23
	eor     x25, x23, x16
	eor     x26, x22, x10
	eor     x27, x9,  x12
	eor     x28, x26, x27
	and     x29, x27, x28
	eor     x18, x16, x28
	eor     x4,  x9,  x14
	eor     x5,  x10, x11
	eor     x6,  x23, x5
    eor     x30,  x6 , x28
	and     x8,  x19, x30
	eor     x17, x8,  x29
    eor 	x10, x6,  x4 
	and     x11, x4,  x6
	eor     x11, x11, x29
    eor 	x13, x5,  x28
	eor     x14, x20, x13
	and     x22, x20, x13
	eor     x29, x9,  x13
	eor     x5,  x5,  x16
	eor     x12, x12, x5 
	and     x8,  x12, x16
	eor     x8,  x8,  x24
    eor     x8,  x8,  x11
	eor     x10, x10, x8
	eor     x9,  x9,  x5
    and    	x8,	 x9,  x18
	eor     x8,  x8,  x22
	eor     x11, x11, x8
	eor     x11, x11, x29
	eor     x15, x15, x5
	and     x29, x15, x5
	eor     x22, x22, x29
	eor     x22, x22, x17
	eor     x14, x14, x22
	eor     x22, x15, x4	
	and     x29, x22, x25
	eor     x24, x24, x29
	eor     x24, x24, x26
	eor     x24, x24, x17
	and     x26, x24, x14
	eor     x29, x11, x26
	eor     x26, x26, x10
	eor     x24, x24, x10
	and     x8,  x24, x29
	eor     x10, x10, x8
	and     x17, x10, x9
	and     x18, x10, x18
	eor     x8,  x14, x11
	and     x26, x26, x8
    eor 	x26, x26, x11
	eor     x8,  x29, x26
	and     x11, x11, x8
	eor     x29, x29, x11
	and     x29, x29, x10
	eor     x24, x24, x29
	eor     x29, x10, x24
	and     x13, x13, x29 
	eor     x18, x18, x13
	and     x20, x20, x29
	and     x15, x15, x24 
	and     x29, x24, x5
	eor     x13, x13, x29
	eor     x14, x14, x26
	eor     x11, x11, x14
	eor     x14, x24, x11
	and     x24, x14, x6
	and     x29, x14, x4
	eor     x4,  x26, x11
	and     x23, x23, x4
	and     x21, x21, x4
	and     x22, x22, x11 
	and     x11, x11, x25
	eor     x11, x11, x23
    eor 	x5,  x13, x11
	and     x12, x12, x26
	eor     x10, x10, x26
	eor     x14, x14, x10
	and     x25, x14, x30
	eor     x13, x13, x25 
	and     x19, x19, x14
	and     x14, x10, x28 
	eor     x14, x14, x13
	and     x10, x10, x27
	eor     x25, x10, x14
	eor     x19, x19, x10
	eor     x10, x22, x19
	eor     x30,  x10, x12
	eor     x21, x21, x10
	eor     x9,  x21, x14
	eor     x12, x21, x5
	eor     x10, x12, x14
	//x10 	^= 0xffffffff;
	eor     x19, x19, x15
	and     x14, x26, x16
	eor     x6,  x23, x14
	eor     x15, x20, x6
	eor     x15, x15, x24 
	eor     x15, x15, x13
	eor     x11, x17, x15
	eor     x14, x30,  x11
	eor     x11, x11, x25
	eor     x11, x11, x29
	//x11 	^= 0xffffffff;
	eor     x6,  x6,  x18
	eor     x19, x19, x6
	eor     x15, x15, x19
	//x15 	^= 0xffffffff;
	eor     x16, x20, x19
	//x16 	^= 0xffffffff;
	eor     x13, x6,  x12 
.endm

/******************************************************************************
* Computation of the MixColumns transformation in the fixsliced representation.
* For fully-fixsliced implementations, it is used for rounds i s.t. (i%4) == 0.
* For semi-fixsliced implementations, it is used for rounds i s.t. (i%2) == 0.
******************************************************************************/
.align 2
mixcolumns_0:
    mov     x17, #0x0f0f
    movk    x17, #0xf0f0, lsl #16
    movk    x17, #0x0f0f, lsl #32
    movk    x17, #0xf0f0, lsl #48
    and     x19, x9 , x17
    and     x20, x10, x17
    and     x21, x11, x17
    and     x22, x12, x17
    and     x23, x13, x17
    and     x24, x14, x17
    and     x25, x15, x17
    and     x26, x16, x17

    mov     x17, #0x0fff
    movk    x17, #0x0fff, lsl #16
    movk    x17, #0x0fff, lsl #32
    movk    x17, #0x0fff, lsl #48
    //t3 = x27, t0 = x28, t1 = x4, t2 = x5, t4 = x7
    byteror x27, x19, x17, #4, #12, x29
    ror     x27, x27, #16
    eor     x28, x27, x19           
    byteror x4 , x26, x17, #4, #12, x29 
    ror     x4 , x4 , #16
    eor     x5 , x4 , x26           
    mov     x6 , #0x00ff
    movk    x6 , #0x00ff, lsl #16
    movk    x6 , #0x00ff, lsl #32
    movk    x6 , #0x00ff, lsl #48
    byteror x26, x5 , x6, #8, #8, x29
    eor     x26, x4 , x26, ror #32
    eor     x26, x28, x26
    byteror x4 , x25, x17, #4, #12, x29
    ror     x4 , x4 , #16
    eor     x7 , x4 , x25 
    byteror x25, x7 , x6, #8, #8, x29
    eor     x25, x4 , x25, ror #32
    eor     x25, x25, x28
    eor     x25, x25, x5
    byteror x4 , x24, x17, #4, #12, x29
    ror     x4 , x4 , #16
    eor     x5 , x4 , x24
    byteror x24, x5 , x6 , #8, #8 , x29
    eor     x24, x4 , x24, ror #32
    eor     x24, x7 , x24
    byteror x4 , x23, x17, #4, #12, x29
    ror     x4 , x4 , #16
    eor     x7 , x4 , x23
    byteror x23, x7 , x6 , #8, #8, x29
    eor     x23, x4 , x23, ror #32
    eor     x23, x28, x23
    eor     x23, x5 , x23
    byteror x4 , x22, x17, #4, #12, x29
    ror     x4 , x4 , #16
    eor     x5 , x4 , x22
    byteror x22, x5 , x6 , #8, #8 , x29
    eor     x22, x4 , x22, ror #32
    eor     x22, x28, x22
    eor     x22, x7 , x22
    byteror x4 , x21, x17, #4, #12, x29
    ror     x4 , x4 , #16
    eor     x7 , x4 , x21
    byteror x21, x7 , x6 , #8, #8, x29
    eor     x21, x4 , x21, ror #32
    eor     x21, x5 , x21
    byteror x4 , x20, x17, #4, #12, x29
    ror     x4 , x4 , #16
    eor     x5 , x4 , x20
    byteror x20, x5 , x6 , #8, #8 , x29
    eor     x20, x4 , x20, ror #32
    eor     x20, x7 , x20
    byteror x19, x28, x6 , #8, #8 , x29
    eor     x19, x27, x19, ror #32
    eor     x19, x5 , x19

    mov     x17, #0xf0f0
    movk    x17, #0x0f0f, lsl #16
    movk    x17, #0xf0f0, lsl #32
    movk    x17, #0x0f0f, lsl #48
    and     x9 , x9 , x17
    and     x10, x10, x17
    and     x11, x11, x17
    and     x12, x12, x17
    and     x13, x13, x17
    and     x14, x14, x17
    and     x15, x15, x17
    and     x16, x16, x17

    mov     x17, #0x000f
    movk    x17, #0x000f, lsl #16
    movk    x17, #0x000f, lsl #32
    movk    x17, #0x000f, lsl #48
    
    byteror x27, x9 , x17, #12, #4, x29
    ror     x27, x27, #16
    eor     x28, x27, x9           
    byteror x4 , x16, x17, #12, #4, x29 
    ror     x4 , x4 , #16
    eor     x5 , x4 , x16

    byteror x16, x5 , x6, #8,  #8, x29
    eor     x16, x4 , x16, ror #32
    eor     x16, x28, x16
    byteror x4 , x15, x17, #12, #4, x29
    ror     x4 , x4 , #16
    eor     x7 , x4 , x15 
    byteror x15, x7 , x6, #8, #8, x29
    eor     x15, x4 , x15, ror #32
    eor     x15, x15, x28
    eor     x15, x15, x5
    byteror x4 , x14, x17, #12, #4, x29
    ror     x4 , x4 , #16
    eor     x5 , x4 , x14
    byteror x14, x5 , x6 , #8, #8 , x29
    eor     x14, x4 , x14, ror #32
    eor     x14, x7 , x14
    byteror x4 , x13, x17, #12, #4, x29
    ror     x4 , x4 , #16
    eor     x7 , x4 , x13
    byteror x13, x7 , x6 , #8, #8, x29
    eor     x13, x4 , x13, ror #32
    eor     x13, x28, x13
    eor     x13, x5 , x13
    byteror x4 , x12, x17, #12, #4, x29
    ror     x4 , x4 , #16
    eor     x5 , x4 , x12
    byteror x12, x5 , x6 , #8, #8 , x29
    eor     x12, x4 , x12, ror #32
    eor     x12, x28, x12
    eor     x12, x7 , x12
    byteror x4 , x11, x17, #12, #4, x29
    ror     x4 , x4 , #16
    eor     x7 , x4 , x11
    byteror x11, x7 , x6 , #8, #8, x29
    eor     x11, x4 , x11, ror #32
    eor     x11, x5 , x11
    byteror x4 , x10, x17, #12, #4, x29
    ror     x4 , x4 , #16
    eor     x5 , x4 , x10
    byteror x10, x5 , x6 , #8, #8 , x29
    eor     x10, x4 , x10, ror #32
    eor     x10, x7 , x10
    byteror x9 , x28, x6 , #8, #8 , x29
    eor     x9 , x27, x9 , ror #32
    eor     x9 , x5 , x9 

    eor     x9 , x9 , x19
    eor     x10, x10, x20
    eor     x11, x11, x21
    eor     x12, x12, x22
    eor     x13, x13, x23
    eor     x14, x14, x24
    eor     x15, x15, x25
    eor     x16, x16, x26
    
    ret

/******************************************************************************
* Computation of the MixColumns transformation in the fixsliced representation.
* For fully-fixsliced implementations, it is used for rounds i s.t. (i%4) == 3.
* For semi-fixsliced implementations, it is used for rounds i s.t. (i%2) == 1.
* Based on Käsper-Schwabe, similar to https://github.com/Ko-/aes-armcortexm.
******************************************************************************/
.align 2
mixcolumns_1:
    eor     x19, x16, x16, ror #16
    eor     x20, x9,  x9,  ror #16
    eor     x16, x20, x16, ror #16
    eor     x16, x16, x19, ror #32
    eor     x21, x15, x15, ror #16
    eor     x15, x19, x15, ror #16
    eor     x15, x15, x20
    eor     x15, x15, x21, ror #32
    eor     x19, x14, x14, ror #16
    eor     x14, x21, x14, ror #16
    eor     x14, x14, x19, ror #32
    eor     x21, x13, x13, ror #16
    eor     x13, x19, x13, ror #16
    eor     x13, x13, x20
    eor     x13, x13, x21, ror #32
    eor     x19, x12, x12, ror #16
    eor     x12, x21, x12, ror #16
    eor     x12, x20, x12
    eor     x12, x12, x19, ror #32
    eor     x21, x11, x11, ror #16
    eor     x11, x19, x11, ror #16
    eor     x11, x11, x21, ror #32
    eor     x19, x10, x10, ror #16
    eor     x10, x21, x10, ror #16
    eor     x10, x10, x19, ror #32
    eor     x9,  x19, x9,  ror #16
    eor     x9,  x9,  x20, ror #32
    ret

/******************************************************************************
* Fully-fixsliced implementation of AES-variant.
*
* Four blocks are encrypted in parallel, without any operating mode.
*
* Note that additional 4 bytes are allocated on the stack as the function takes
* 5 arguments as input.
******************************************************************************/
// void aes128_encrypt_ffs(unsigned char ctexx19[16], unsigned char ctexx20[16], 
//						unsigned char ctexx21[16], unsigned char ctexx22[16],	
//						const unsigned char ptexx19[16], const unsigned char ptexx20[16],
//						const unsigned char ptexx21[16], const unsigned char ptexx22[16],	
//						const uinx254_t rkeys[88]);
.global _AES128_SR1_enc
.align 2
_AES128_SR1_enc:
    stp     x19, x20, [sp, #-16]!   // store x19-x20 on the stack
    stp     x21, x22, [sp, #-16]!   // store x21-x22 on the stack
    stp     x23, x24, [sp, #-16]!   // store x23-x24 on the stack
    stp     x25, x26, [sp, #-16]!   // store x25-x26 on the stack
    stp     x27, x28, [sp, #-16]!   // store x27-x28 on the stack
    stp     x29, x30, [sp, #-16]!   // store x29-x18 on the stack
    add     sp,  sp,  #96
    ldr     x9,  [x4]               // load 1st 128-bit blocks in x9-x10
    ldr     x10, [x4, #8]
    ldr     x11, [x5]               // load 2nd 128-bit blocks in x11-x12
    ldr     x12, [x5, #8]
    ldr     x13, [x6]               // load 3rd 128-bit blocks in x13-x14
    ldr     x14, [x6, #8]
    ldr     x15, [x7]               // load 4th 128-bit blocks in x15-x16
    ldr     x16, [x7, #8]
    bl      packing                 // pack the 4 input blocks    
    ark_sbox                        // ark + sbox (round 0)
    bl      mixcolumns_0            // mixcolumns (round 0)    
    ark_sbox                        // ark + sbox (round 1)
    bl      mixcolumns_1            // mixcolumns (round 1)
    ark_sbox                        // ark + sbox (round 2)
    bl      mixcolumns_0            // mixcolumns (round 2)
    ark_sbox                        // ark + sbox (round 3)
    bl      mixcolumns_1            // mixcolumns (round 3)
    ark_sbox                        // ark + sbox (round 4)
    bl      mixcolumns_0            // mixcolumns (round 4)
    ark_sbox                        // ark + sbox (round 5)
    bl      mixcolumns_1            // mixcolumns (round 5)
    ark_sbox                        // ark + sbox (round 6)
    bl      mixcolumns_0            // mixcolumns (round 6)
    ark_sbox                        // ark + sbox (round 7)
    bl      mixcolumns_1            // mixcolumns (round 7)
    ark_sbox                        // ark + sbox (round 8)
    bl      mixcolumns_0            // mixcolumns (round 8)
    ark_sbox                        // ark + sbox (round 9)
    //Last add_round_key
    ldr     x19, [sp, #8]
    ldr     x20, [sp, #16]
    ldr     x21, [sp, #24]
    ldr     x22, [sp, #32]
    ldr     x23, [sp, #40]
    ldr     x24, [sp, #48]
    ldr     x25, [sp, #56]
    ldr     x26, [sp, #64]
    eor     x9,  x9,  x19
    eor     x10, x10, x20
    eor     x11, x11, x21
    eor     x12, x12, x22
    eor     x13, x13, x23
    eor     x14, x14, x24
    eor     x15, x15, x25
    eor     x16, x16, x26
    add     sp,  sp, #-640
    bl      unpacking               // unpack the internal state    
    str     x9,  [x0]               // store the ciphertext
    str     x10, [x0, #8]
    str     x11, [x1]               // store the ciphertext
    str     x12, [x1, #8]
    str     x13, [x2]               // store the ciphertext
    str     x14, [x2, #8]
    str     x15, [x3]               // store the ciphertext
    str     x16, [x3, #8]
    add     sp,  sp,  #-96
    ldp     x29, x30, [sp], #16     // restore x29-x18
    ldp     x27, x28, [sp], #16     // restore x27-x28
    ldp     x25, x26, [sp], #16     // restore x25-x26
    ldp     x23, x24, [sp], #16     // restore x23-x24
    ldp     x21, x22, [sp], #16     // restore x21-x22
    ldp     x19, x20, [sp], #16     // restore x19-x20
    
    ret