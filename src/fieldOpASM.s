//
// Created by HaveYouTall
//
	.section .data
	.global BLS12_377_G1_P
	.align 8 //@指令32bit（4字节）对齐
BLS12_377_G1_P:
	.quad 0x8508c00000000001, 0x170b5d4430000000, 0x1ef3622fba094800, 0x1a22d9f300f5138f ,0xc63b05c06ca1493b, 0x1ae3a4617c510ea

	.section .text
	.global add_mod_p, sub_mod_p, neg_mod_p
	.global double_mod_p, mul_by_3_mod_p, div_by_2_mod_p
	.align 4 

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@                                                                                        @
//@ ---  void add_mod_p(const uint64_t a[6], const uint64_t b[6], uint64_t result[6]); --- @
//@ ---  a -> x0,  b->x1, result->x2                                                   --- @
//@ ---                                                                                    @
//@                                                                                        @
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
add_mod_p:
	sub sp, sp, #0xa0  //
    stp x18,x19,[sp, #0x90]
    stp x20,x21,[sp, #0x80]
    stp x22,x23,[sp, #0x70]
	stp x4,x5,[sp, #0x60]
	stp x6,x7,[sp, #0x50]
	stp x8,x9,[sp, #0x40]
	stp x10,x11,[sp, #0x30]
	stp x12,x13,[sp, #0x20]
	stp x14,x15,[sp, #0x10]
	stp x16,x17,[sp]

	a_0 .req x4
	a_1 .req x5
	a_2 .req x6
	a_3 .req x7
    a_4 .req x8
    a_5 .req x9

	res0 .req x10
	res1 .req x11
	res2 .req x12
	res3 .req x13
	res4 .req x14
    res5 .req x15
    res6 .req x16

	P_0 .req x17
	P_1 .req x18
	P_2 .req x19
	P_3 .req x20
    P_4 .req x21
    P_5 .req x22

    //load a and b
	ldp a_0,a_1,[x0] //load a
	ldp a_2,a_3,[x0, #0x10]
    ldp a_4,a_5,[x0, #0x20]
	ldp res0,res1,[x1] //load b
	ldp res2,res3,[x1, #0x10]
    ldp res4,res5,[x1, #0x20]


    //eor res6, res6, res6
// calc a + b
	adds res0, a_0, res0
	adcs res1, a_1, res1
	adcs res2, a_2, res2
	adcs res3, a_3, res3
    adcs res4, a_4, res4
    adcs res5, a_5, res5
	adc res6, xzr, xzr // get carry

	mov a_0, res0
	mov a_1, res1
	mov a_2, res2
	mov a_3, res3
    mov a_4, res4
    mov a_5, res5

// get modulus constant addr, stored in x23 register
	adrp x23, BLS12_377_G1_P
	add x23, x23, :lo12:BLS12_377_G1_P
	//adrp x23, :got:BLS12_377_G1_P
    //ldr  x23, [x23, :got_lo12:BLS12_377_G1_P]
// load modulus
	ldp P_0,P_1,[x23] 
	ldp P_2,P_3,[x23, #0x10]
    ldp P_4,P_5,[x23, #0x20]
// calc a - modulus
	subs a_0, a_0, P_0
	sbcs a_1, a_1, P_1
	sbcs a_2, a_2, P_2
	sbcs a_3, a_3, P_3
    sbcs a_4, a_4, P_4
    sbcs a_5, a_5, P_5

	sbcs res6, res6, xzr

	//bne _next
	bne 2f
	mov res0, a_0
	mov res1, a_1
	mov res2, a_2
	mov res3, a_3
    mov res4, a_4
    mov res5, a_5

2:
// store result
	stp res0, res1, [x2]
	stp res2, res3, [x2, #0x10]
    stp res4, res5, [x2, #0x20]
// recover
    ldp x18,x19,[sp, #0x90]
    ldp x20,x21,[sp, #0x80]
    ldp x22,x23,[sp, #0x70]
	ldp x4,x5,[sp, #0x60]
	ldp x6,x7,[sp, #0x50]
	ldp x8,x9,[sp, #0x40]
	ldp x10,x11,[sp, #0x30]
	ldp x12,x13,[sp, #0x20]
	ldp x14,x15,[sp, #0x10]
	ldp x16,x17,[sp]
	add sp, sp, #0xa0 
	ret






//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@                                                                                        @
//@ ---  void sub_mod_p(const uint64_t a[6], const uint64_t b[6], uint64_t result[6]); --- @
//@ ---  a -> x0,  b->x1, result->x2                                                   --- @
//@ ---                                                                                    @
//@                                                                                        @
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
sub_mod_p:
    sub sp, sp, #0xa0  //
    stp x18,x19,[sp, #0x90]
    stp x20,x21,[sp, #0x80]
    stp x22,x23,[sp, #0x70]
	stp x4,x5,[sp, #0x60]
	stp x6,x7,[sp, #0x50]
	stp x8,x9,[sp, #0x40]
	stp x10,x11,[sp, #0x30]
	stp x12,x13,[sp, #0x20]
	stp x14,x15,[sp, #0x10]
	stp x16,x17,[sp]

	a_0 .req x4
	a_1 .req x5
	a_2 .req x6
	a_3 .req x7
    a_4 .req x8
    a_5 .req x9

	res0 .req x10
	res1 .req x11
	res2 .req x12
	res3 .req x13
	res4 .req x14
    res5 .req x15
    res6 .req x16

	P_0 .req x17
	P_1 .req x18
	P_2 .req x19
	P_3 .req x20
    P_4 .req x21
    P_5 .req x22

//load a and b
	ldp a_0,a_1,[x0] //load a
	ldp a_2,a_3,[x0, #0x10]
    ldp a_4,a_5,[x0, #0x20]
	ldp res0,res1,[x1] //load b
	ldp res2,res3,[x1, #0x10]
    ldp res4,res5,[x1, #0x20]

    //eor res6, res6, res6
// calc a - b
	subs res0, a_0, res0
	sbcs res1, a_1, res1
	sbcs res2, a_2, res2
	sbcs res3, a_3, res3
    sbcs res4, a_4, res4
    sbcs res5, a_5, res5

	sbc res6, xzr, xzr

// backup
	mov a_0, res0
	mov a_1, res1
	mov a_2, res2
	mov a_3, res3
    mov a_4, res4
    mov a_5, res5

// get modulus constant addr, stored in x23 register
	adrp x23, BLS12_377_G1_P
	add x23, x23, :lo12:BLS12_377_G1_P
	//adrp x23, :got:BLS12_377_G1_P
    //ldr  x23, [x23, :got_lo12:BLS12_377_G1_P]
// load modulus
	ldp P_0,P_1,[x23] 
	ldp P_2,P_3,[x23, #0x10]
    ldp P_4,P_5,[x23, #0x20]

	adds a_0, a_0, P_0
	adcs a_1, a_1, P_1
	adcs a_2, a_2, P_2
	adcs a_3, a_3, P_3
    adcs a_4, a_4, P_4
    adc a_5, a_5, P_5

	tst res6, res6 // if res6 == 0 than a-b >= 0, else a-b < 0

	beq 2f
	mov res0, a_0
	mov res1, a_1
	mov res2, a_2
	mov res3, a_3
    mov res4, a_4
    mov res5, a_5
2:
// store result
	stp res0, res1, [x2]
	stp res2, res3, [x2, #0x10]
    stp res4, res5, [x2, #0x20]
// recover
    ldp x18,x19,[sp, #0x90]
    ldp x20,x21,[sp, #0x80]
    ldp x22,x23,[sp, #0x70]
	ldp x4,x5,[sp, #0x60]
	ldp x6,x7,[sp, #0x50]
	ldp x8,x9,[sp, #0x40]
	ldp x10,x11,[sp, #0x30]
	ldp x12,x13,[sp, #0x20]
	ldp x14,x15,[sp, #0x10]
	ldp x16,x17,[sp]
	add sp, sp, #0xa0
	ret



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@                                                                                  @
//@ ---  void neg_mod_p(const uint64_t a[6], uint64_t result[6]);                --- @
//@ ---  a -> x0, result -> x1                                                   --- @
//@ ---                                                                              @
//@                                                                                  @
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
neg_mod_p:
    sub sp, sp, #0xa0  //
    stp x18,x19,[sp, #0x90]
    stp x20,x21,[sp, #0x80]
    stp x22,x23,[sp, #0x70]
	stp x4,x5,[sp, #0x60]
	stp x6,x7,[sp, #0x50]
	stp x8,x9,[sp, #0x40]
	stp x10,x11,[sp, #0x30]
	stp x12,x13,[sp, #0x20]
	stp x14,x15,[sp, #0x10]
	stp x16,x17,[sp]

	a_0 .req x4
	a_1 .req x5
	a_2 .req x6
	a_3 .req x7
    a_4 .req x8
    a_5 .req x9

	res0 .req x10
	res1 .req x11
	res2 .req x12
	res3 .req x13
	res4 .req x14
    res5 .req x15
    res6 .req x16

	P_0 .req x17
	P_1 .req x18
	P_2 .req x19
	P_3 .req x20
    P_4 .req x21
    P_5 .req x22

//load a
	ldp a_0,a_1,[x0] //load a
	ldp a_2,a_3,[x0, #0x10]
    ldp a_4,a_5,[x0, #0x20]

    //eor res6, res6, res6
// cal  0 - a
	subs res0, xzr, a_0
	sbcs res1, xzr, a_1
	sbcs res2, xzr, a_2
	sbcs res3, xzr, a_3
    sbcs res4, xzr, a_4
    sbcs res5, xzr, a_5
	
    sbc res6, xzr, xzr

// backup
	mov a_0, res0
	mov a_1, res1
	mov a_2, res2
	mov a_3, res3
    mov a_4, res4
    mov a_5, res5

// get modulus constant addr, stored in x23 register
	adrp x23, BLS12_377_G1_P
	add x23, x23, :lo12:BLS12_377_G1_P
	//adrp x23, :got:BLS12_377_G1_P
    //ldr  x23, [x23, :got_lo12:BLS12_377_G1_P]
// load modulus
	ldp P_0,P_1,[x23] 
	ldp P_2,P_3,[x23, #0x10]
    ldp P_4,P_5,[x23, #0x20]

	adds a_0, a_0, P_0
	adcs a_1, a_1, P_1
	adcs a_2, a_2, P_2
    adcs a_3, a_3, P_3
    adcs a_4, a_4, P_4
	adc a_5, a_5, P_5

	tst res6, res6  // if res6 == 0 than 0-a >= 0, else 0-a < 0

	beq 2f
	mov res0, a_0
	mov res1, a_1
	mov res2, a_2
	mov res3, a_3
    mov res4, a_4
    mov res5, a_5
2:
// store result
	stp res0, res1, [x1]
	stp res2, res3, [x1, #0x10]
    stp res4, res5, [x1, #0x20]
// recover
    ldp x18,x19,[sp, #0x90]
    ldp x20,x21,[sp, #0x80]
    ldp x22,x23,[sp, #0x70]
	ldp x4,x5,[sp, #0x60]
	ldp x6,x7,[sp, #0x50]
	ldp x8,x9,[sp, #0x40]
	ldp x10,x11,[sp, #0x30]
	ldp x12,x13,[sp, #0x20]
	ldp x14,x15,[sp, #0x10]
	ldp x16,x17,[sp]
	add sp, sp, #0xa0
	ret




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@                                                                                  @
//@ ---  void double_mod_p(const uint64_t a[6], uint64_t result[6]);             --- @
//@ ---  a -> x0, result->x1                                                     --- @
//@ ---                                                                              @
//@                                                                                  @
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
double_mod_p:
	sub sp, sp, #0xa0  //
    stp x18,x19,[sp, #0x90]
    stp x20,x21,[sp, #0x80]
    stp x22,x23,[sp, #0x70]
	stp x4,x5,[sp, #0x60]
	stp x6,x7,[sp, #0x50]
	stp x8,x9,[sp, #0x40]
	stp x10,x11,[sp, #0x30]
	stp x12,x13,[sp, #0x20]
	stp x14,x15,[sp, #0x10]
	stp x16,x17,[sp]

	a_0 .req x4
	a_1 .req x5
	a_2 .req x6
	a_3 .req x7
    a_4 .req x8
    a_5 .req x9

	res0 .req x10
	res1 .req x11
	res2 .req x12
	res3 .req x13
	res4 .req x14
    res5 .req x15
    res6 .req x16

	P_0 .req x17
	P_1 .req x18
	P_2 .req x19
	P_3 .req x20
    P_4 .req x21
    P_5 .req x22

//load a
	ldp a_0,a_1,[x0]
	ldp a_2,a_3,[x0, #0x10]
    ldp a_4,a_5,[x0, #0x20]

    //eor res6, res6, res6
//calc	a * 2 -- double
    add res6, xzr, a_5, lsr #63 
	lsl res5, a_5, #1
    add res5, res5, a_4, lsr #63 
	lsl res4, a_4, #1
	add res4, res4, a_3, lsr #63 
	lsl res3, a_3, #1
	add res3, res3, a_2, lsr #63
	lsl res2, a_2, #1
	add res2, res2, a_1, lsr #63
	lsl res1, a_1, #1
	add res1, res1, a_0, lsr #63
	lsl res0, a_0, #1

// buckup
	mov a_0, res0
	mov a_1, res1
	mov a_2, res2
	mov a_3, res3
    mov a_4, res4
	mov a_5, res5

// get modulus constant addr, stored in x23 register
	adrp x23, BLS12_377_G1_P
	add x23, x23, :lo12:BLS12_377_G1_P
	//adrp x23, :got:BLS12_377_G1_P
    //ldr  x23, [x23, :got_lo12:BLS12_377_G1_P]
// load modulus
	ldp P_0,P_1,[x23] 
	ldp P_2,P_3,[x23, #0x10]
    ldp P_4,P_5,[x23, #0x20]

	subs a_0, a_0, P_0
	sbcs a_1, a_1, P_1
	sbcs a_2, a_2, P_2
	sbcs a_3, a_3, P_3
    sbcs a_4, a_4, P_4
    sbcs a_5, a_5, P_5
	sbcs res6, res6, xzr

	//bne _next
	bne 2f
	mov res0, a_0
	mov res1, a_1
	mov res2, a_2
	mov res3, a_3
    mov res4, a_4
    mov res5, a_5
2:
// store result
	stp res0, res1, [x1]
	stp res2, res3, [x1, #0x10]
    stp res4, res5, [x1, #0x20]
// recover
    ldp x18,x19,[sp, #0x90]
    ldp x20,x21,[sp, #0x80]
    ldp x22,x23,[sp, #0x70]
	ldp x4,x5,[sp, #0x60]
	ldp x6,x7,[sp, #0x50]
	ldp x8,x9,[sp, #0x40]
	ldp x10,x11,[sp, #0x30]
	ldp x12,x13,[sp, #0x20]
	ldp x14,x15,[sp, #0x10]
	ldp x16,x17,[sp]
	add sp, sp, #0xa0
	ret


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@                                                                                    @
//@ ---  void mul_by_3_mod_p(const uint64_t a[6], uint64_t result[6]);             --- @
//@ ---  a -> x0, result->x1                                                       --- @
//@ ---                                                                                @
//@                                                                                    @
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
mul_by_3_mod_p:
	sub sp, sp, #0xa0  //
    stp x18,x19,[sp, #0x90]
    stp x20,x21,[sp, #0x80]
    stp x22,x23,[sp, #0x70]
	stp x4,x5,[sp, #0x60]
	stp x6,x7,[sp, #0x50]
	stp x8,x9,[sp, #0x40]
	stp x10,x11,[sp, #0x30]
	stp x12,x13,[sp, #0x20]
	stp x14,x15,[sp, #0x10]
	stp x16,x17,[sp]

	a_0 .req x4
	a_1 .req x5
	a_2 .req x6
	a_3 .req x7
    a_4 .req x8
    a_5 .req x9

	res0 .req x10
	res1 .req x11
	res2 .req x12
	res3 .req x13
	res4 .req x14
    res5 .req x15
    res6 .req x16

	P_0 .req x17
	P_1 .req x18
	P_2 .req x19
	P_3 .req x20
    P_4 .req x21
    P_5 .req x22

//load a
	ldp a_0,a_1,[x0]
	ldp a_2,a_3,[x0, #0x10]
    ldp a_4,a_5,[x0, #0x20]

    //eor res6, res6, res6
//calc	a * 2 -- double
    add res6, xzr, a_5, lsr #63 
	lsl res5, a_5, #1
    add res5, res5, a_4, lsr #63 
	lsl res4, a_4, #1
	add res4, res4, a_3, lsr #63 
	lsl res3, a_3, #1
	add res3, res3, a_2, lsr #63
	lsl res2, a_2, #1
	add res2, res2, a_1, lsr #63
	lsl res1, a_1, #1
	add res1, res1, a_0, lsr #63
	lsl res0, a_0, #1

// buckup
	mov a_0, res0
	mov a_1, res1
	mov a_2, res2
	mov a_3, res3
    mov a_4, res4
	mov a_5, res5

// get modulus constant addr, stored in x23 register
	adrp x23, BLS12_377_G1_P
	add x23, x23, :lo12:BLS12_377_G1_P
	//adrp x23, :got:BLS12_377_G1_P
    //ldr  x23, [x23, :got_lo12:BLS12_377_G1_P]
// load modulus
	ldp P_0,P_1,[x23] 
	ldp P_2,P_3,[x23, #0x10]
    ldp P_4,P_5,[x23, #0x20]
    

	subs a_0, a_0, P_0
	sbcs a_1, a_1, P_1
	sbcs a_2, a_2, P_2
	sbcs a_3, a_3, P_3
    sbcs a_4, a_4, P_4
    sbcs a_5, a_5, P_5
	sbcs res6, res6, xzr

	bne 2f
	mov res0, a_0
	mov res1, a_1
	mov res2, a_2
	mov res3, a_3
    mov res4, a_4
    mov res5, a_5
2:
	//eor res6, res6, res6
	// load a again
	ldp a_0,a_1,[x0]
	ldp a_2,a_3,[x0, #0x10]
    ldp a_4,a_5,[x0, #0x20]

	adds res0, res0, a_0
	adcs res1, res1, a_1
	adcs res2, res2, a_2
	adcs res3, res3, a_3
    adcs res4, res4, a_4
    adcs res5, res5, a_5
	adc res6, xzr, xzr 

	//backup
	mov a_0, res0
	mov a_1, res1
	mov a_2, res2
	mov a_3, res3
    mov a_4, res4
	mov a_5, res5

	//Sub p
	subs a_0, a_0, P_0
	sbcs a_1, a_1, P_1
	sbcs a_2, a_2, P_2
	sbcs a_3, a_3, P_3
    sbcs a_4, a_4, P_4
    sbcs a_5, a_5, P_5
	sbcs res6, res6, xzr

	bne 3f
	mov res0, a_0
	mov res1, a_1
	mov res2, a_2
	mov res3, a_3
    mov res4, a_4
	mov res5, a_5
3:
// store result
	stp res0, res1, [x1]
	stp res2, res3, [x1, #0x10]
    stp res4, res5, [x1, #0x20]
// recover
    ldp x18,x19,[sp, #0x90]
    ldp x20,x21,[sp, #0x80]
    ldp x22,x23,[sp, #0x70]
	ldp x4,x5,[sp, #0x60]
	ldp x6,x7,[sp, #0x50]
	ldp x8,x9,[sp, #0x40]
	ldp x10,x11,[sp, #0x30]
	ldp x12,x13,[sp, #0x20]
	ldp x14,x15,[sp, #0x10]
	ldp x16,x17,[sp]
	add sp, sp, #0xa0
	ret


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@                                                                                    @
//@ ---  void div_by_2_mod_p(const uint64_t a[6], uint64_t result[6]);             --- @
//@ ---  a -> x0, result->x1                                                       --- @
//@ ---                                                                                @
//@                                                                                    @
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
div_by_2_mod_p:
	sub sp, sp, #0xb0  //
    str x24,[sp, #0xa0]
    stp x18,x19,[sp, #0x90]
    stp x20,x21,[sp, #0x80]
    stp x22,x23,[sp, #0x70]
	stp x4,x5,[sp, #0x60]
	stp x6,x7,[sp, #0x50]
	stp x8,x9,[sp, #0x40]
	stp x10,x11,[sp, #0x30]
	stp x12,x13,[sp, #0x20]
	stp x14,x15,[sp, #0x10]
	stp x16,x17,[sp]

	a_0 .req x4
	a_1 .req x5
	a_2 .req x6
	a_3 .req x7
    a_4 .req x8
    a_5 .req x9

	res0 .req x10
	res1 .req x11
	res2 .req x12
	res3 .req x13
	res4 .req x14
    res5 .req x15
    res6 .req x16

	P_0 .req x17
	P_1 .req x18
	P_2 .req x19
	P_3 .req x20
    P_4 .req x21
    P_5 .req x22

    u0 .req x24

//load a
	ldp a_0,a_1,[x0]
	ldp a_2,a_3,[x0, #0x10]
    ldp a_4,a_5,[x0, #0x20]

    eor res6, res6, res6
	//eor u0, u0, u0 

	//Backup
	mov res0, a_0
	mov res1, a_1
	mov res2, a_2
	mov res3, a_3
    mov res4, a_4
	mov res5, a_5


// get modulus constant addr, stored in x23 register
	adrp x23, BLS12_377_G1_P
	add x23, x23, :lo12:BLS12_377_G1_P
	//adrp x23, :got:BLS12_377_G1_P
    //ldr  x23, [x23, :got_lo12:BLS12_377_G1_P]
// load modulus
	ldp P_0,P_1,[x23] 
	ldp P_2,P_3,[x23, #0x10]
    ldp P_4,P_5,[x23, #0x20]
// add P
	adds a_0, a_0, P_0
	adcs a_1, a_1, P_1
	adcs a_2, a_2, P_2
	adcs a_3, a_3, P_3
    adcs a_4, a_4, P_4
	adcs a_5, a_5, P_5

	adc u0, xzr, xzr

	// make sure the last bit is 0
	cmp xzr, res0, lsl #63
	beq 2f
	mov res0, a_0
	mov res1, a_1
	mov res2, a_2
	mov res3, a_3
    mov res4, a_4
	mov res5, a_5
	mov res6, u0
2:
//	a / 2 -- div
	lsr res0, res0, #1
	add res0, res0, res1, lsl #63
	lsr res1, res1, #1
	add res1, res1, res2, lsl #63
	lsr res2, res2, #1
	add res2, res2, res3, lsl #63
	lsr res3, res3, #1
	add res3, res3, res4, lsl #63
    lsr res4, res4, #1
	add res4, res4, res5, lsl #63
    lsr res5, res5, #1
	add res5, res5, res6, lsl #63

// store result
	stp res0, res1, [x1]
	stp res2, res3, [x1, #0x10]
    stp res4, res5, [x1, #0x20]
// recover
    ldr x24,[sp, #0xa0]
    ldp x18,x19,[sp, #0x90]
    ldp x20,x21,[sp, #0x80]
    ldp x22,x23,[sp, #0x70]
	ldp x4,x5,[sp, #0x60]
	ldp x6,x7,[sp, #0x50]
	ldp x8,x9,[sp, #0x40]
	ldp x10,x11,[sp, #0x30]
	ldp x12,x13,[sp, #0x20]
	ldp x14,x15,[sp, #0x10]
	ldp x16,x17,[sp]
	add sp, sp, #0xb0
	ret
