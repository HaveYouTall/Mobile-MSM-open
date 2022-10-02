//
// Created by HaveYouTall
//
    .section .data
	.global k_0
	.align 8 //@指令32bit（4字节）对齐
k_0:
	.quad 0x8508bfffffffffff

	.section .text
	.global mont_mul_reduce_as, mont_sqr_reduce_as
	.align 4 


//Ref: https://github.com/intel/ipp-crypto/blob/3ae57715fbc7383ef527cab0b824d736b60fa06d/sources/ippcp/asm_intel64/pcpp256funcs_montas.asm#L294
.macro MUL_6x1 p_6,p_5,p_4,p_3,p_2,p_1,p_0
	mul \p_0, a_0, b_
    umulh \p_1, a_0, b_ 
    mul dm, k0, \p_0

    mul tmp0, a_1, b_
    umulh tmp1, a_1, b_  
    adds \p_1, \p_1, tmp0
    adc  \p_2, tmp1, xzr

    mul tmp0, a_2, b_
    umulh tmp1, a_2, b_  
    adds \p_2, \p_2, tmp0
    adc  \p_3, tmp1, xzr

    mul tmp0, a_3, b_
    umulh tmp1, a_3, b_  
    adds \p_3, \p_3, tmp0
    adc  \p_4, tmp1, xzr

    mul tmp0, a_4, b_
    umulh tmp1, a_4, b_  
    adds \p_4, \p_4, tmp0
    adc  \p_5, tmp1, xzr

    mul tmp0, a_5, b_
    umulh tmp1, a_5, b_  
    adds \p_5, \p_5, tmp0
    adc  \p_6, tmp1, xzr

.endm

// p_7 is used to store higher carry result.
.macro MLA_6x1 p_7,p_6,p_5,p_4,p_3,p_2,p_1,p_0
	mul tmp0, a_0, b_
    umulh tmp1, a_0, b_
    adds \p_0, \p_0, tmp0
    adc  \p_7, tmp1, xzr
    mul dm, k0, \p_0

    mul tmp0, a_1, b_
    umulh tmp1, a_1, b_
    adds \p_1, \p_1, \p_7
    adc  tmp1, tmp1, xzr
    adds \p_1, \p_1, tmp0
    adc  \p_7, tmp1, xzr

    mul tmp0, a_2, b_
    umulh tmp1, a_2, b_  
    adds \p_2, \p_2, \p_7
    adc  tmp1, tmp1, xzr
    adds \p_2, \p_2, tmp0
    adc  \p_7, tmp1, xzr

    mul tmp0, a_3, b_
    umulh tmp1, a_3, b_  
    adds \p_3, \p_3, \p_7
    adc  tmp1, tmp1, xzr
    adds \p_3, \p_3, tmp0
    adc  \p_7, tmp1, xzr

    mul tmp0, a_4, b_
    umulh tmp1, a_4, b_
    adds \p_4, \p_4, \p_7
    adc  tmp1, tmp1, xzr
    adds \p_4, \p_4, tmp0
    adc  \p_7, tmp1, xzr

    mul tmp0, a_5, b_
    umulh tmp1, a_5, b_  
    adds \p_5, \p_5, \p_7
    adc  tmp1, tmp1, xzr
    adds \p_5, \p_5, tmp0
    adcs \p_6, \p_6, tmp1
    adc  \p_7, xzr,  xzr

.endm


.macro MRED_6x1 p_7,p_6,p_5,p_4,p_3,p_2,p_1,p_0

	mul tmp0, M_0, dm
    umulh tmp1, M_0, dm
    adds \p_0, \p_0, tmp0
    adc  \p_0, tmp1, xzr

    mul tmp0, M_1, dm
    umulh tmp1, M_1, dm
    adds \p_1, \p_1, \p_0
    adc  tmp1, tmp1, xzr 
    adds \p_1, \p_1, tmp0
    adc \p_0, tmp1, xzr

    mul tmp0, M_2, dm
    umulh tmp1, M_2, dm
    adds \p_2, \p_2, \p_0
    adc  tmp1, tmp1, xzr 
    adds \p_2, \p_2, tmp0
    adc \p_0, tmp1, xzr

    mul tmp0, M_3, dm
    umulh tmp1, M_3, dm  
    adds \p_3, \p_3, \p_0
    adc  tmp1, tmp1, xzr 
    adds \p_3, \p_3, tmp0
    adc \p_0, tmp1, xzr

    mul tmp0, M_4, dm
    umulh tmp1, M_4, dm  
    adds \p_4, \p_4, \p_0
    adc  tmp1, tmp1, xzr 
    adds \p_4, \p_4, tmp0
    adc \p_0, tmp1, xzr

    mul tmp0, M_5, dm
    umulh tmp1, M_5, dm  
    adds \p_5, \p_5, \p_0
    adc  tmp1, tmp1, xzr 
    adds \p_5, \p_5, tmp0
    //adcs \p_6, tmp1, xzr
    adcs \p_6, \p_6, tmp1

    adc  \p_7, \p_7, xzr
    eor  \p_0, \p_0, \p_0
.endm


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@                                                                                                 @
//@ ---  void mont_mul_reduce_as(const uint64_t a[6], const uint64_t b[6], uint64_t result[6]); --- @
//@ ---  a -> x0,  b->x1, result->x2                                                            --- @
//@ ---                                                                                             @
//@                                                                                                 @
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
mont_mul_reduce_as:
    sub sp, sp, #0xd0  //
    str x3, [sp, #0xc0]
    stp x26,x27,[sp, #0xb0]
    stp x24,x25,[sp, #0xa0]
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

    acc0 .req x10
	acc1 .req x11
	acc2 .req x12
	acc3 .req x13
	acc4 .req x14
    acc5 .req x15
    acc6 .req x16
    acc7 .req x17

    M_0 .req x18
	M_1 .req x19
	M_2 .req x20
	M_3 .req x21
    M_4 .req x22
    M_5 .req x23

    tmp0 .req x24 
    tmp1 .req x25

    dm .req x26
    b_  .req x27
    k0 .req x3

// get modulus constant addr, stored in x26 register
	adrp x26, BLS12_377_G1_P // Defined in fieldOpASM.s
	add x26, x26, :lo12:BLS12_377_G1_P
    //adrp x26, :got:BLS12_377_G1_P // Defined in fieldOpASM.s
    //ldr  x26, [x26, :got_lo12:BLS12_377_G1_P]
// load modulus
	ldp M_0,M_1,[x26] 
	ldp M_2,M_3,[x26, #0x10]
    ldp M_4,M_5,[x26, #0x20]
// get k_0 addr, stored in x26 register
	adrp x26, k_0
	add x26, x26, :lo12:k_0
    //adrp x26, :got:k_0
    //ldr  x26, [x26, :got_lo12:k_0]
// load k_0
    ldr k0, [x26]

//load a
	ldp a_0,a_1,[x0]
	ldp a_2,a_3,[x0, #0x10]
    ldp a_4,a_5,[x0, #0x20]

// P = a[] * b[0]
    //load b[0]
    ldr b_, [x1]
    // load k_0
	//ldr dm, [x3]
    MUL_6x1 acc6, acc5, acc4, acc3, acc2, acc1, acc0
    eor acc7, acc7, acc7
// First reduction
    MRED_6x1 acc7, acc6, acc5, acc4, acc3, acc2, acc1, acc0

// P += a[] * b[1]
    // load b[1]
    ldr b_, [x1, #0x08]
    // load k_0
	//ldr dm, [x3]
    MLA_6x1 acc0, acc7, acc6, acc5, acc4, acc3, acc2, acc1
// Second reduction
    MRED_6x1 acc0, acc7, acc6, acc5, acc4, acc3, acc2, acc1

// P += a[] * b[2]
    // load b[2]
    ldr b_, [x1, #0x10]
    // load k_0
	//ldr dm, [x3]
    MLA_6x1 acc1, acc0, acc7, acc6, acc5, acc4, acc3, acc2
// Third reduction
    MRED_6x1 acc1, acc0, acc7, acc6, acc5, acc4, acc3, acc2

// P += a[] * b[3]
    // load b[3]
    ldr b_, [x1, #0x18]
    // load k_0
	//ldr dm, [x3]
    MLA_6x1 acc2, acc1, acc0, acc7, acc6, acc5, acc4, acc3
// Fourth reduction
    MRED_6x1 acc2, acc1, acc0, acc7, acc6, acc5, acc4, acc3

// P += a[] * b[4]
    // load b[4]
    ldr b_, [x1, #0x20]
    // load k_0
	//ldr dm, [x3]
    MLA_6x1 acc3, acc2, acc1, acc0, acc7, acc6, acc5, acc4
// Fifth reduction
    MRED_6x1 acc3, acc2, acc1, acc0, acc7, acc6, acc5, acc4

// P += a[] * b[5]
    // load b[5]
    ldr b_, [x1, #0x28] // #0x28 was accidently changed to #0x3.... when replace all x28 reg to x3...
    // load k_0
	//ldr dm, [x3]
    MLA_6x1 acc4, acc3, acc2, acc1, acc0, acc7, acc6, acc5
// Sixth reduction
    MRED_6x1 acc4, acc3, acc2, acc1, acc0, acc7, acc6, acc5

// Final reduction
//acc3, acc2, acc1, acc0, acc7, acc6
    mov a_5, acc3
    mov a_4, acc2
    mov a_3, acc1
    mov a_2, acc0
    mov a_1, acc7
    mov a_0, acc6

    // Sub modulus
    subs a_0, a_0, M_0
	sbcs a_1, a_1, M_1
	sbcs a_2, a_2, M_2
	sbcs a_3, a_3, M_3
    sbcs a_4, a_4, M_4
    sbcs a_5, a_5, M_5

    sbcs acc4, acc4, xzr

    bne 2f
    mov acc6, a_0
	mov acc7, a_1
	mov acc0, a_2
	mov acc1, a_3
    mov acc2, a_4
    mov acc3, a_5

2:
// store result
	stp acc6, acc7, [x2]
	stp acc0, acc1, [x2, #0x10]
    stp acc2, acc3, [x2, #0x20]
// recover
    ldr x3, [sp, #0xc0]
    ldp x26,x27,[sp, #0xb0]
    ldp x24,x25,[sp, #0xa0]
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
	add sp, sp, #0xd0 
	ret


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@                                                                                                 @
//@ ---  void mont_sqr_reduce_as(const uint64_t a[6], uint64_t result[6]);                      --- @
//@ ---  a -> x0, result->x1                                                                    --- @
//@ ---                                                                                             @
//@                                                                                                 @
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
mont_sqr_reduce_as:
    sub sp, sp, #0xe0  //
    str x28, [sp, #0xd0]
    stp x2, x3, [sp, #0xc0]
    stp x26,x27,[sp, #0xb0]
    stp x24,x25,[sp, #0xa0]
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

    acc0 .req x10
	acc1 .req x11
	acc2 .req x12
	acc3 .req x13
	acc4 .req x14
    acc5 .req x15
    acc6 .req x16
    acc7 .req x17
    acc8 .req x2
    acc9 .req x3
    acc10 .req x27
    acc11 .req x28

    M_0 .req x18
	M_1 .req x19
	M_2 .req x20
	M_3 .req x21
    M_4 .req x22
    M_5 .req x23

    tmp0 .req x24 
    tmp1 .req x25

    dm .req x26
    k0_sqr .req x4

// get modulus constant addr, stored in x26 register
	adrp x26, BLS12_377_G1_P // Defined in fieldOpASM.s
	add x26, x26, :lo12:BLS12_377_G1_P
    //adrp x26, :got:BLS12_377_G1_P // Defined in fieldOpASM.s
    //ldr  x26, [x26, :got_lo12:BLS12_377_G1_P]
// load modulus
	ldp M_0,M_1,[x26] 
	ldp M_2,M_3,[x26, #0x10]
    ldp M_4,M_5,[x26, #0x20]

//load a
	ldp a_0,a_1,[x0]
	ldp a_2,a_3,[x0, #0x10]
    ldp a_4,a_5,[x0, #0x20]

// get k_0 addr, stored in x0 register
	adrp x0, k_0
	add x0, x0, :lo12:k_0
    //adrp x26, :got:k_0
    //ldr  x26, [x26, :got_lo12:k_0]
// load k_0
    //ldr k0, [x0]

// Diagonal
    // a[1:end] * a[0]
    mul   acc1, a_0, a_1
    umulh acc2, a_0, a_1

    mul   tmp0, a_0, a_2
    umulh tmp1, a_0, a_2
    adds  acc2, acc2, tmp0
    adc   acc3, tmp1, xzr

    mul   tmp0, a_0, a_3
    umulh tmp1, a_0, a_3
    adds  acc3, acc3, tmp0
    adc   acc4, tmp1, xzr

    mul   tmp0, a_0, a_4
    umulh tmp1, a_0, a_4
    adds  acc4, acc4, tmp0
    adc   acc5, tmp1, xzr

    mul   tmp0, a_0, a_5
    umulh tmp1, a_0, a_5
    adds  acc5, acc5, tmp0
    adc   acc6, tmp1, xzr
    
    // a[2:end] * a[1]
    mul   tmp0, a_1, a_2
    umulh tmp1, a_1, a_2
    adds  acc3, acc3, tmp0
    adc   acc11, tmp1, xzr

    mul   tmp0, a_1, a_3
    umulh tmp1, a_1, a_3
    adds  acc4, acc4, acc11
    adc   tmp1, tmp1, xzr
    adds  acc4, acc4, tmp0
    adc   acc11, tmp1, xzr

    mul   tmp0, a_1, a_4
    umulh tmp1, a_1, a_4
    adds  acc5, acc5, acc11
    adc   tmp1, tmp1, xzr
    adds  acc5, acc5, tmp0
    adc   acc11, tmp1, xzr

    mul   tmp0, a_1, a_5
    umulh tmp1, a_1, a_5
    adds  acc6, acc6, acc11
    adc   tmp1, tmp1, xzr
    adds  acc6, acc6, tmp0
    adc   acc7, tmp1, xzr

    // a[3:end] * a[2]
    mul   tmp0, a_2, a_3
    umulh tmp1, a_2, a_3
    adds  acc5, acc5, tmp0
    adc   acc11, tmp1, xzr

    mul   tmp0, a_2, a_4
    umulh tmp1, a_2, a_4
    adds  acc6, acc6, acc11
    adc   tmp1, tmp1, xzr
    adds  acc6, acc6, tmp0
    adc   acc11, tmp1, xzr

    mul   tmp0, a_2, a_5
    umulh tmp1, a_2, a_5
    adds  acc7, acc7, acc11
    adc   tmp1, tmp1, xzr
    adds  acc7, acc7, tmp0
    adc   acc8, tmp1, xzr

    // a[4:end] * a[3]
    mul   tmp0, a_3, a_4
    umulh tmp1, a_3, a_4
    adds  acc7, acc7, tmp0
    adc   acc11, tmp1, xzr

    mul   tmp0, a_3, a_5
    umulh tmp1, a_3, a_5
    adds  acc8, acc8, acc11
    adc   tmp1, tmp1, xzr
    adds  acc8, acc8, tmp0
    adc   acc9, tmp1, xzr

    // a[5] * a[4]
    mul   tmp0, a_4, a_5
    umulh tmp1, a_4, a_5
    adds  acc9, acc9, tmp0
    adc   acc10, tmp1, xzr

// Doubling
    add   acc11, xzr,   acc10, lsr #63
    lsl   acc10, acc10, #1
    add   acc10, acc10, acc9, lsr #63
    lsl   acc9,  acc9,  #1
    add   acc9,  acc9,  acc8, lsr #63
    lsl   acc8,  acc8,  #1
    add   acc8,  acc8,  acc7, lsr #63
    lsl   acc7,  acc7,  #1
    add   acc7,  acc7,  acc6, lsr #63
    lsl   acc6,  acc6,  #1
    add   acc6,  acc6,  acc5, lsr #63
    lsl   acc5,  acc5,  #1
    add   acc5,  acc5,  acc4, lsr #63
    lsl   acc4,  acc4,  #1
    add   acc4,  acc4,  acc3, lsr #63
    lsl   acc3,  acc3,  #1
    add   acc3,  acc3,  acc2, lsr #63
    lsl   acc2,  acc2,  #1
    add   acc2,  acc2,  acc1, lsr #63
    lsl   acc1,  acc1,  #1

// Add a[0]^2 a[1]^2 ... a[5]^2
    mul   acc0, a_0, a_0
    umulh tmp1, a_0, a_0
    adds  acc1, acc1, tmp1
    adc   acc2, acc2, xzr

    mul   tmp0, a_1, a_1
    umulh tmp1, a_1, a_1
    adds  acc2, acc2, tmp0
    adcs  acc3, acc3, tmp1
    adc   acc4, acc4, xzr

    mul   tmp0, a_2, a_2
    umulh tmp1, a_2, a_2
    adds  acc4, acc4, tmp0
    adcs  acc5, acc5, tmp1
    adc   acc6, acc6, xzr

    mul   tmp0, a_3, a_3
    umulh tmp1, a_3, a_3
    adds  acc6, acc6, tmp0
    adcs  acc7, acc7, tmp1
    adc   acc8, acc8, xzr

    mul   tmp0, a_4, a_4
    umulh tmp1, a_4, a_4
    adds  acc8, acc8, tmp0
    adcs  acc9, acc9, tmp1
    adc   acc10, acc10, xzr

    mul   tmp0, a_5, a_5
    umulh tmp1, a_5, a_5
    adds  acc10, acc10, tmp0
    adc   acc11, acc11, tmp1

// Reduction
    ldr k0_sqr, [x0]

    mul dm, k0_sqr, acc0
    MRED_6x1 acc7, acc6, acc5, acc4, acc3, acc2, acc1, acc0

    mul dm, k0_sqr, acc1
    MRED_6x1 acc8, acc7, acc6, acc5, acc4, acc3, acc2, acc1

    mul dm, k0_sqr, acc2
    MRED_6x1 acc9, acc8, acc7, acc6, acc5, acc4, acc3, acc2
    
    mul dm, k0_sqr, acc3
    MRED_6x1 acc10, acc9, acc8, acc7, acc6, acc5, acc4, acc3

    mul dm, k0_sqr, acc4
    MRED_6x1 acc11, acc10, acc9, acc8, acc7, acc6, acc5, acc4

    mul dm, k0_sqr, acc5
    MRED_6x1 acc0, acc11, acc10, acc9, acc8, acc7, acc6, acc5

// Final reduction
    mov a_5, acc11
    mov a_4, acc10
    mov a_3, acc9
    mov a_2, acc8
    mov a_1, acc7
    mov a_0, acc6

    // Sub modulus
    subs a_0, a_0, M_0
	sbcs a_1, a_1, M_1
	sbcs a_2, a_2, M_2
	sbcs a_3, a_3, M_3
    sbcs a_4, a_4, M_4
    sbcs a_5, a_5, M_5

    sbcs acc0, acc0, xzr

    bne 2f
    mov acc6, a_0
	mov acc7, a_1
	mov acc8, a_2
	mov acc9, a_3
    mov acc10, a_4
    mov acc11, a_5

2:
// store result
	stp acc6, acc7, [x1]
	stp acc8, acc9, [x1, #0x10]
    stp acc10, acc11, [x1, #0x20]
// recover
    ldr x28, [sp, #0xd0]
    ldp x2, x3, [sp, #0xc0]
    ldp x26,x27,[sp, #0xb0]
    ldp x24,x25,[sp, #0xa0]
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
	add sp, sp, #0xe0 
	ret
