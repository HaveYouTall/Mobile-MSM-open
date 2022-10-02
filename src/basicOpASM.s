//
// Created by HaveYouTall
//
.section .text
.global _addcarryx_u64, _mulx_u64, raw_mul, raw_pow, raw_mul_reduce_R
.align 4 //@指令32bit（4字节）对齐
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@                                                                                 @
//@ ---  u1 _addcarryx_u64(u1 carry, uint64_t a, uint64_t b, uint64_t *result); --- @
//@ ---  carry -> x0, a -> x1, b -> x2, result-> x3                             --- @
//@ ---  测试成功                                                                                                                          @
//@                                                                                 @
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
_addcarryx_u64:
	sub sp, sp, #0x10  //@开辟栈空间 注意开辟的大小
	str x4, [sp]

	adds x4, x1, x0 //res = a + carry
	adc x0, xzr, xzr //check c
	adds x4, x4, x2 //res += b
	adc x0, x0, xzr //check c
	str x4, [x3] // save result

//@	恢复现场
	ldr x4, [sp]
	add sp, sp, #0x10  //@栈平衡
	ret


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@                                                                                 @
//@ ---  uint64_t _mulx_u64(uint64_t a, uint64_t b, uint64_t *hi);              --- @
//@ ---  a -> x0, b -> x1, hi -> x2                                             --- @
//@ ---  测试成功                                                                    @
//@                                                                                 @
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
_mulx_u64:
//@开辟栈空间 注意开辟的大小
	sub sp, sp, #0x10
	str x4, [sp]

	umulh x4, x0, x1 //calc high 64bit of result
	mul x0, x0, x1 //calc low 64bit of result
	str x4, [x2] //save high 64bit
//@	恢复现场
	ldr x4, [sp]
	add sp, sp, #0x10  //@栈平衡
	ret


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@                                                                                            @
//@ ---  void raw_mul(const uint64_t a[6], const uint64_t b[6], uint64_t result[12]);       --- @
//@ ---  a -> x0,  b->x1, result->x2                                                       --- @
//@ ---                                                                                        @
//@                                                                                            @
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
raw_mul:
	sub sp, sp, #0x90  
	str x20,[sp, #0x80]
	stp x18,x19,[sp, #0x70]
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

	b_0 .req x10
	b_1 .req x11
	b_2 .req x12
	b_3 .req x13
	b_4 .req x14
	b_5 .req x15

	c_0 .req x16
	c_1 .req x17

	t_0 .req x18
	t_1 .req x19
	t_2 .req x20

	//load a
	ldp a_0, a_1, [x0]
	ldp a_2, a_3, [x0, #0x10]
	ldp a_4, a_5, [x0, #0x20]
	//load b
	ldp b_0, b_1, [x1]
	ldp b_2, b_3, [x1, #0x10]
	ldp b_4, b_5, [x1, #0x20]

	eor t_0, t_0, t_0
	eor t_1, t_1, t_1
	eor t_2, t_2, t_2

	//result[0]
	//a[0] * b[0]
	mul t_0, a_0, b_0
	umulh t_1, a_0, b_0
	// Store result[0]
	str t_0, [x2]
	eor t_0, t_0, t_0

	//result[1]
	//a_0 * b_1
	mul c_0, a_0, b_1
	umulh c_1, a_0, b_1
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr 
	//a_1 * b_0
	mul c_0, a_1, b_0
	umulh c_1, a_1, b_0
	adds t_1, t_1, c_0 
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	// Store result[1]
	str t_1, [x2, #0x08]
	eor t_1, t_1, t_1

	//result[2]
	//a_0 * b_2
	mul c_0, a_0, b_2
	umulh c_1, a_0, b_2
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	//a_1 * b_1
	mul c_0, a_1, b_1
	umulh c_1, a_1, b_1
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	//a_2 * b_0
	mul c_0, a_2, b_0
	umulh c_1, a_2, b_0
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	// Store result[2]
	str t_2, [x2, #0x10]
	eor t_2, t_2, t_2

	//result[3]
	//a_0 * b_3
	mul c_0, a_0, b_3
	umulh c_1, a_0, b_3
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	//a_1 * b_2
	mul c_0, a_1, b_2
	umulh c_1, a_1, b_2
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	//a_2 * b_1
	mul c_0, a_2, b_1
	umulh c_1, a_2, b_1
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	//a_3 * b_0
	mul c_0, a_3, b_0
	umulh c_1, a_3, b_0
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	// Store result[3]
	str t_0, [x2, #0x18]
	eor t_0, t_0, t_0

	//result[4]
	//a_0 * b_4
	mul c_0, a_0, b_4
	umulh c_1, a_0, b_4
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	//a_1 * b_3
	mul c_0, a_1, b_3
	umulh c_1, a_1, b_3
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	//a_2 * b_2
	mul c_0, a_2, b_2
	umulh c_1, a_2, b_2
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	//a_3 * b_1
	mul c_0, a_3, b_1
	umulh c_1, a_3, b_1
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	//a_4 * b_0
	mul c_0, a_4, b_0
	umulh c_1, a_4, b_0
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	// Store result[4]
	str t_1, [x2, #0x20]
	eor t_1, t_1, t_1

	//result[5]
	//a[0] * b[5]
	mul c_0, a_0, b_5
	umulh c_1, a_0, b_5
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	//a[1] * b[4]
	mul c_0, a_1, b_4
	umulh c_1, a_1, b_4
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	//a[2] * b[3]
	mul c_0, a_2, b_3
	umulh c_1, a_2, b_3
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	//a[3] * b[2]
	mul c_0, a_3, b_2
	umulh c_1, a_3, b_2
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	//a[4] * b[1]
	mul c_0, a_4, b_1
	umulh c_1, a_4, b_1
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	//a[5] * b[0]
	mul c_0, a_5, b_0
	umulh c_1, a_5, b_0
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	// Store result[5]
	str t_2, [x2, #0x28]
	eor t_2, t_2, t_2
	

	//result[6]
	//a_1 * b_5
	mul c_0, a_1, b_5
	umulh c_1, a_1, b_5
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	//a_2 * b_4
	mul c_0, a_2, b_4
	umulh c_1, a_2, b_4
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	//a_3 * b_3
	mul c_0, a_3, b_3
	umulh c_1, a_3, b_3
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	//a_4 * b_2
	mul c_0, a_4, b_2
	umulh c_1, a_4, b_2
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	//a_5 * b_1
	mul c_0, a_5, b_1
	umulh c_1, a_5, b_1
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	// Store result[6]
	str t_0, [x2, #0x30]
	eor t_0, t_0, t_0

	//result[7]
	//a_2 * b_5
	mul c_0, a_2, b_5
	umulh c_1, a_2, b_5
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	//a_3 * b_4
	mul c_0, a_3, b_4
	umulh c_1, a_3, b_4
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	//a_4 * b_3
	mul c_0, a_4, b_3
	umulh c_1, a_4, b_3
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	//a_5 * b_2
	mul c_0, a_5, b_2
	umulh c_1, a_5, b_2
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	// Store result[7]
	str t_1,[x2, #0x38]
	eor t_1, t_1, t_1

	//result[8]
	//a_3 * b_5
	mul c_0, a_3, b_5
	umulh c_1, a_3, b_5
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	//a_4 * b_4
	mul c_0, a_4, b_4
	umulh c_1, a_4, b_4
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	//a_5 * b_3
	mul c_0, a_5, b_3
	umulh c_1, a_5, b_3
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	// Store result[8]
	str t_2,[x2, #0x40]
	eor t_2, t_2, t_2

	//result[9]
	//a_4 * b_5
	mul c_0, a_4, b_5
	umulh c_1, a_4, b_5
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	//a_5 * b_4
	mul c_0, a_5, b_4
	umulh c_1, a_5, b_4
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	// Store result[9]
	str t_0,[x2, #0x48]
	// eor t_0, t_0, t_0

	//result[10]
	//a_5 * b_5
	mul c_0, a_5, b_5
	umulh c_1, a_5, b_5
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	// adc t_0, t_0, xzr
	// Store result[10]
	str t_1,[x2, #0x50]
	// eor t_1, t_1, t_1

	//result[11]
	str t_2, [x2, #0x58]


//@	
	ldr x20,[sp, #0x80]
	ldp x18,x19,[sp, #0x70]
	ldp x4,x5,[sp, #0x60]
	ldp x6,x7,[sp, #0x50]
	ldp x8,x9,[sp, #0x40]
	ldp x10,x11,[sp, #0x30]
	ldp x12,x13,[sp, #0x20]
	ldp x14,x15,[sp, #0x10]
	ldp x16,x17,[sp]
	add sp, sp, #0x90  //@
	ret






//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@                                                                                       @
//@ ---  void raw_pow(const uint64_t a[6], uint64_t result[12]);                      --- @
//@ ---  a -> x0, result->x1                                                          --- @
//@ ---                                                                                   @
//@                                                                                       @
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
raw_pow:
    sub sp, sp, #0x60
	stp x14,x15,[sp, #0x50]
	stp x4,x5,[sp, #0x40]
	stp x6,x7,[sp, #0x30]
	stp x8,x9,[sp, #0x20]
	stp x10,x11,[sp, #0x10]
	stp x12,x13,[sp]
	

	pow_a_0 .req x4
	pow_a_1 .req x5
	pow_a_2 .req x6

	pow_t_0 .req x13
	pow_t_1 .req x14
	pow_t_2 .req x15

	x_0 .req x7
	x_1 .req x8
	x_2 .req x9
	x_3 .req x10
	x_4 .req x11
	x_5 .req x12

	eor pow_a_0, pow_a_0, pow_a_0
	eor pow_a_1, pow_a_1, pow_a_1
	eor pow_a_2, pow_a_2, pow_a_2

	ldp x_0, x_1, [x0]
	ldp x_2, x_3, [x0, #0x10]
	ldp x_4, x_5, [x0, #0x20]

	eor pow_t_0, pow_t_0, pow_t_0
	eor pow_t_1, pow_t_1, pow_t_1
	eor pow_t_2, pow_t_2, pow_t_2

	//result[0]
	//a_0 * b_0
	mul pow_a_0, x_0, x_0
	umulh pow_a_1, x_0, x_0
	// Store result[0]
	str pow_a_0, [x1]
	eor pow_a_0 ,pow_a_0, pow_a_0

	//result[1]
	//a_0 * b_1 + a_1 * b_0 //i.e. 2 a_0 * a_1
	mul pow_t_0, x_0, x_1
	umulh pow_t_1, x_0, x_1
	//double self
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_1, pow_a_1, pow_t_0
	adcs pow_a_2, pow_a_2, pow_t_1
	adc pow_a_0, pow_a_0, pow_t_2
	// Store result[1]
	str pow_a_1, [x1, #0x08]
	eor pow_a_1, pow_a_1, pow_a_1

	//result[2]
	//a_0 * a_2 + a_2 * a_0
	mul pow_t_0, x_0, x_2
	umulh pow_t_1, x_0, x_2
	//double self
	eor pow_t_2, pow_t_2, pow_t_2
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_2, pow_a_2, pow_t_0
	adcs pow_a_0, pow_a_0, pow_t_1
	adc pow_a_1, pow_a_1, pow_t_2
	//a_1 * a_1
	mul pow_t_0, x_1, x_1
	umulh pow_t_1, x_1, x_1
	adds pow_a_2, pow_a_2, pow_t_0
	adcs pow_a_0, pow_a_0, pow_t_1
	adc pow_a_1, pow_a_1 , xzr
	// Store result[2]
	str pow_a_2, [x1, #0x10]
	eor pow_a_2, pow_a_2, pow_a_2

	//result[3]
	//a_0 * a3 + a3 * a_0
	mul pow_t_0, x_0, x_3
	umulh pow_t_1, x_0, x_3
	//double self
	eor pow_t_2, pow_t_2, pow_t_2
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_0, pow_a_0, pow_t_0
	adcs pow_a_1, pow_a_1, pow_t_1
	adc pow_a_2, pow_a_2, pow_t_2
	//a_1 * b2 + a_2 * b1
	mul pow_t_0, x_1, x_2
	umulh pow_t_1, x_1, x_2
	//double self
	eor pow_t_2, pow_t_2, pow_t_2
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_0, pow_a_0, pow_t_0
	adcs pow_a_1, pow_a_1, pow_t_1
	adc pow_a_2, pow_a_2, pow_t_2
	// Store result[3]
	str pow_a_0, [x1, #0x18]
	eor pow_a_0, pow_a_0, pow_a_0

	//result[4]
	//a_0 * a4 + a4 * a_0
	mul pow_t_0, x_0, x_4
	umulh pow_t_1, x_0, x_4
	//double self
	eor pow_t_2, pow_t_2, pow_t_2
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_1, pow_a_1, pow_t_0
	adcs pow_a_2, pow_a_2, pow_t_1
	adc pow_a_0, pow_a_0, pow_t_2
	//a_1 * a3 + a3 * a_1
	mul pow_t_0, x_1, x_3
	umulh pow_t_1, x_1, x_3
	//double self
	eor pow_t_2, pow_t_2, pow_t_2
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_1, pow_a_1, pow_t_0
	adcs pow_a_2, pow_a_2, pow_t_1
	adc pow_a_0, pow_a_0, pow_t_2
	//a_2 * a_2
	mul pow_t_0, x_2, x_2
	umulh pow_t_1, x_2, x_2
	adds pow_a_1, pow_a_1, pow_t_0
	adcs pow_a_2, pow_a_2, pow_t_1
	adc pow_a_0, pow_a_0, xzr
	// Store result[4]
	str pow_a_1, [x1, #0x20]
	eor pow_a_1, pow_a_1, pow_a_1

	//result[5]
	//a_0 * a5 + a5 * a0
	mul pow_t_0, x_0, x_5
	umulh pow_t_1, x_0, x_5
	//double self
	eor pow_t_2, pow_t_2, pow_t_2
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_2, pow_a_2, pow_t_0
	adcs pow_a_0, pow_a_0, pow_t_1
	adc pow_a_1, pow_a_1, pow_t_2
	//a_1 * a4 + a4 * a1
	mul pow_t_0, x_1, x_4
	umulh pow_t_1, x_1, x_4
	//double self
	eor pow_t_2, pow_t_2, pow_t_2
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_2, pow_a_2, pow_t_0
	adcs pow_a_0, pow_a_0, pow_t_1
	adc pow_a_1, pow_a_1, pow_t_2
	//a_2 * a3 + a3 * a2
	mul pow_t_0, x_2, x_3
	umulh pow_t_1, x_2, x_3
	//double self
	eor pow_t_2, pow_t_2, pow_t_2
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_2, pow_a_2, pow_t_0
	adcs pow_a_0, pow_a_0, pow_t_1
	adc pow_a_1, pow_a_1, pow_t_2
	// Store result[5]
	str pow_a_2, [x1, #0x28]
	eor pow_a_2, pow_a_2, pow_a_2

	//result[6]
	//a_1 * a5 + a5 * a1
	mul pow_t_0, x_1, x_5
	umulh pow_t_1, x_1, x_5
	//double self
	eor pow_t_2, pow_t_2, pow_t_2
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_0, pow_a_0, pow_t_0
	adcs pow_a_1, pow_a_1, pow_t_1
	adc pow_a_2, pow_a_2, pow_t_2
	//a_2 * a4 + a4 * a2
	mul pow_t_0, x_2, x_4
	umulh pow_t_1, x_2, x_4
	//double self
	eor pow_t_2, pow_t_2, pow_t_2
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_0, pow_a_0, pow_t_0
	adcs pow_a_1, pow_a_1, pow_t_1
	adc pow_a_2, pow_a_2, pow_t_2
	//a3 * a3
	mul pow_t_0, x_3, x_3
	umulh pow_t_1, x_3, x_3
	adds pow_a_0, pow_a_0, pow_t_0
	adcs pow_a_1, pow_a_1, pow_t_1
	adc pow_a_2, pow_a_2, xzr
	// Store result[6]
	str pow_a_0, [x1, #0x30]
	eor pow_a_0, pow_a_0, pow_a_0

	//result[7]
	//a_2 * a5 + a5 * a2
	mul pow_t_0, x_2, x_5
	umulh pow_t_1, x_2, x_5
	//double self
	eor pow_t_2, pow_t_2, pow_t_2
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_1, pow_a_1, pow_t_0
	adcs pow_a_2, pow_a_2, pow_t_1
	adc pow_a_0, pow_a_0, pow_t_2
	//a_3 * a4 + a4 * a3
	mul pow_t_0, x_3, x_4
	umulh pow_t_1, x_3, x_4
	//double self
	eor pow_t_2, pow_t_2, pow_t_2
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_1, pow_a_1, pow_t_0
	adcs pow_a_2, pow_a_2, pow_t_1
	adc pow_a_0, pow_a_0, pow_t_2
	//result[7]
	str pow_a_1, [x1, #0x38]
	eor pow_a_1, pow_a_1, pow_a_1

	//result[8]
	//a_3 * a5 + a5 * a3
	mul pow_t_0, x_3, x_5
	umulh pow_t_1, x_3, x_5
	//double self
	eor pow_t_2, pow_t_2, pow_t_2
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_2, pow_a_2, pow_t_0
	adcs pow_a_0, pow_a_0, pow_t_1
	adc pow_a_1, pow_a_1, pow_t_2
	//a4 * a4
	mul pow_t_0, x_4, x_4
	umulh pow_t_1, x_4, x_4
	adds pow_a_2, pow_a_2, pow_t_0
	adcs pow_a_0, pow_a_0, pow_t_1
	adc pow_a_1, pow_a_1, xzr
	//result[8]
	str pow_a_2, [x1, #0x40]
	eor pow_a_2, pow_a_2, pow_a_2

	//result[9]
	//a_4 * a5 + a5 * a4
	mul pow_t_0, x_4, x_5
	umulh pow_t_1, x_4, x_5
	//double self
	eor pow_t_2, pow_t_2, pow_t_2
	add pow_t_2, pow_t_2, pow_t_1, lsr #63
	lsl pow_t_1, pow_t_1, #1
	add pow_t_1, pow_t_1, pow_t_0, lsr #63
	lsl pow_t_0, pow_t_0, #1
	adds pow_a_0, pow_a_0, pow_t_0
	adcs pow_a_1, pow_a_1, pow_t_1
	adc pow_a_2, pow_a_2, pow_t_2
	//result[9]
	str pow_a_0, [x1, #0x48]
	//eor pow_a_0, pow_a_0, pow_a_0

	//result[10]
	//a5 * a5
	mul pow_t_0, x_5, x_5
	umulh pow_t_1, x_5, x_5
	adds pow_a_1, pow_a_1, pow_t_0
	adcs pow_a_2, pow_a_2, pow_t_1
	//adc pow_a_0, pow_a_0, xzr
	//result[10]
	str pow_a_1, [x1, #0x50]

	//result[11]
	str pow_a_2, [x1, #0x58]

//
	ldp x14,x15,[sp, #0x50]
	ldp x4,x5,[sp, #0x40]
	ldp x6,x7,[sp, #0x30]
	ldp x8,x9,[sp, #0x20]
	ldp x10,x11,[sp, #0x10]
	ldp x12,x13,[sp]
	add sp, sp, #0x60  //
	ret



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@                                                                                                 @
//@ ---  void raw_mul_reduce_R(const uint64_t a[6], const uint64_t b[6], uint64_t result[6]);  --- @
//@ ---  a -> x0,  b->x1, result->x2                                                            --- @
//@ ---                                                                                             @
//@                                                                                                 @
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
raw_mul_reduce_R:
	sub sp, sp, #0x90  
	str x20,[sp, #0x80]
	stp x18,x19,[sp, #0x70]
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

	b_0 .req x10
	b_1 .req x11
	b_2 .req x12
	b_3 .req x13
	b_4 .req x14
	b_5 .req x15

	c_0 .req x16
	c_1 .req x17

	t_0 .req x18
	t_1 .req x19
	t_2 .req x20

	//load a
	ldp a_0, a_1, [x0]
	ldp a_2, a_3, [x0, #0x10]
	ldp a_4, a_5, [x0, #0x20]
	//load b
	ldp b_0, b_1, [x1]
	ldp b_2, b_3, [x1, #0x10]
	ldp b_4, b_5, [x1, #0x20]

	eor t_0, t_0, t_0
	eor t_1, t_1, t_1
	eor t_2, t_2, t_2

	//result[0]
	//a[0] * b[0]
	mul t_0, a_0, b_0
	umulh t_1, a_0, b_0
	// Store result[0]
	str t_0, [x2]
	eor t_0, t_0, t_0

	//result[1]
	//a_0 * b_1
	mul c_0, a_0, b_1
	umulh c_1, a_0, b_1
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr 
	//a_1 * b_0
	mul c_0, a_1, b_0
	umulh c_1, a_1, b_0
	adds t_1, t_1, c_0 
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	// Store result[1]
	str t_1, [x2, #0x08]
	eor t_1, t_1, t_1

	//result[2]
	//a_0 * b_2
	mul c_0, a_0, b_2
	umulh c_1, a_0, b_2
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	//a_1 * b_1
	mul c_0, a_1, b_1
	umulh c_1, a_1, b_1
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	//a_2 * b_0
	mul c_0, a_2, b_0
	umulh c_1, a_2, b_0
	adds t_2, t_2, c_0
	adcs t_0, t_0, c_1
	adc t_1, t_1, xzr
	// Store result[2]
	str t_2, [x2, #0x10]
	eor t_2, t_2, t_2

	//result[3]
	//a_0 * b_3
	mul c_0, a_0, b_3
	umulh c_1, a_0, b_3
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	//a_1 * b_2
	mul c_0, a_1, b_2
	umulh c_1, a_1, b_2
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	//a_2 * b_1
	mul c_0, a_2, b_1
	umulh c_1, a_2, b_1
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	//a_3 * b_0
	mul c_0, a_3, b_0
	umulh c_1, a_3, b_0
	adds t_0, t_0, c_0
	adcs t_1, t_1, c_1
	adc t_2, t_2, xzr
	// Store result[3]
	str t_0, [x2, #0x18]
	eor t_0, t_0, t_0

	//result[4]
	//a_0 * b_4
	mul c_0, a_0, b_4
	umulh c_1, a_0, b_4
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	//a_1 * b_3
	mul c_0, a_1, b_3
	umulh c_1, a_1, b_3
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	//a_2 * b_2
	mul c_0, a_2, b_2
	umulh c_1, a_2, b_2
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	//a_3 * b_1
	mul c_0, a_3, b_1
	umulh c_1, a_3, b_1
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	adc t_0, t_0, xzr
	//a_4 * b_0
	mul c_0, a_4, b_0
	umulh c_1, a_4, b_0
	adds t_1, t_1, c_0
	adcs t_2, t_2, c_1
	// adc t_0, t_0, xzr
	// Store result[4]
	str t_1, [x2, #0x20]
	// eor t_1, t_1, t_1

	//result[5]
	//a[0] * b[5]
	mul c_0, a_0, b_5
	// umulh c_1, a_0, b_5
	adds t_2, t_2, c_0
	// adcs t_0, t_0, c_1
	// adc t_1, t_1, xzr
	//a[1] * b[4]
	mul c_0, a_1, b_4
	// umulh c_1, a_1, b_4
	adds t_2, t_2, c_0
	// adcs t_0, t_0, c_1
	// adc t_1, t_1, xzr
	//a[2] * b[3]
	mul c_0, a_2, b_3
	// umulh c_1, a_2, b_3
	adds t_2, t_2, c_0
	// adcs t_0, t_0, c_1
	// adc t_1, t_1, xzr
	//a[3] * b[2]
	mul c_0, a_3, b_2
	// umulh c_1, a_3, b_2
	adds t_2, t_2, c_0
	// adcs t_0, t_0, c_1
	// adc t_1, t_1, xzr
	//a[4] * b[1]
	mul c_0, a_4, b_1
	// umulh c_1, a_4, b_1
	adds t_2, t_2, c_0
	// adcs t_0, t_0, c_1
	// adc t_1, t_1, xzr
	//a[5] * b[0]
	mul c_0, a_5, b_0
	// umulh c_1, a_5, b_0
	adds t_2, t_2, c_0
	// adcs t_0, t_0, c_1
	// adc t_1, t_1, xzr
	// Store result[5]
	str t_2, [x2, #0x28]
	// eor t_2, t_2, t_2


//@	
	ldr x20,[sp, #0x80]
	ldp x18,x19,[sp, #0x70]
	ldp x4,x5,[sp, #0x60]
	ldp x6,x7,[sp, #0x50]
	ldp x8,x9,[sp, #0x40]
	ldp x10,x11,[sp, #0x30]
	ldp x12,x13,[sp, #0x20]
	ldp x14,x15,[sp, #0x10]
	ldp x16,x17,[sp]
	add sp, sp, #0x90  //@
	ret