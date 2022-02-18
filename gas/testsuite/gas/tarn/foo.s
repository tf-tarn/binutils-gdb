	.file	"foo.c"
	.text

        .equ result16, 0x6030

        ;; .global temp_sum_lo
        ;; .global temp_sum_hi
        ;; .global temp_carry
        .equ a16, 0x5012
        .equ b16, 0x5014

        .equ temp_space,     0x4000
        .equ temp_sum_lo,    0x4022
        .equ temp_sum_hi,    0x4024
        .equ temp_carry,     0x4026


	.macro plus a, b
        mov ara   \a   ,0
        mov arb   \b  ,0
        .endm

	.macro plusk a, k
        mov ara   \a   ,0
        mov arb   il   ,\k
        .endm

        .macro increment a
        mov ara   \a   ,0
        mov arb   il   ,1
        mov \a    arc  ,0
        .endm

        .macro lad s
	mov     adh il   ,hi8(\s)
	mov	adl il   ,lo8(\s)
        .endm

        .macro ljmp s
	mov     jmph il   ,hi8(\s)
	mov	jmpl il   ,lo8(\s)
        .endm

        .macro store addr r
        mov adl il ,lo8(\addr)
        mov mem \r
        .endm

        .macro storek addr k
        mov adl il ,lo8(\addr)
        mov mem il ,\k
        .endm

        .macro storeg addr r
        lad \addr
        mov mem \r
        .endm

        .macro storegk addr k
        lad \addr
        mov mem il ,\k
        .endm

        .macro load dest addr
        mov adl il ,lo8(\addr)
        mov \dest mem
        .endm

        .macro loadg dest addr
        lad \addr
        mov \dest mem
        .endm

        .macro loadnext dest addr
        mov adl   il ,lo8(1+\addr)
        mov \dest mem
        .endm

        .macro storenext addr src
        mov adl   il ,lo8(1+\addr)
        mov mem \src
        .endm

        .macro plus16 sa, sb, result

        ;; work in input space, so we only have to change adl
        mov adh il, hi8(\sa)
        ;; load sa low byte
        load ara \sa+1
        ;; load sb low byte
        load arb \sb+1

        ;; work in temp space
        mov adh il, hi8(temp_space)

        ;; record the sum
        store temp_sum_lo arc

        ;; get carry bit and store it
        update_status
        store temp_carry status

        ;; work in input space
        mov adh il, hi8(\sa)
        ;; load sa high byte
        load ara \sa
        ;; load sb high byte
        load arb \sb

        ;; work in temp space
        mov adh il, hi8(temp_space)
        ;; save the sum
        mov r arc
	;; now add the sum and the carry
        mov ara r
        load arb temp_carry
        load r temp_sum_lo

        ;; work in output space
        mov adh il, hi8(\result)
        ;; save result
	store \result   arc
        store \result+1 r

        .endm

        ;;  Just a hack, because status isn't loaded automatically.
        .macro update_status
        mov status p0
        .endm

        .macro disp reg n
        .if \n
        mov p0 \reg
        disp \reg "\n-1"
        .endif
        .endm

        .macro halt
        ;; Stop simulated execution with a bad jump instruction.
        .byte 0x99
        .byte 0x99
        .endm

;;; program follows

	.text
	.p2align	1       ; instructions must be aligned to 2-byte boundary
	.global	_start
	.type	_start, @function
_start:
# BLOCK 2 seq:0
        # PRED: ENTRY (FALLTHRU)

        storegk a16   0x12
        storegk a16+1 0x34
        storegk b16   0x56
        storegk b16+1 0xd8

        plus16 a16 b16 result16

        nop
        nop
        nop
        halt

        ljmp fib_loop

	.global	fib
	.type	fib, @function
fib:
        mov ara il ,1
        mov arb il ,0

	.global	fib_loop
	.type	fib_loop, @function
fib_loop:

        mov r arc
        mov arb r
        disp arc 1

        mov r arc
        mov ara r
        disp arc 1

        jump                    ; loop forever
# SUCC: EXIT [always]
	#TODO: implement return	# 18	[c=0 l=2]  returner
	.size	_start, .-_start
	.ident	"GCC: (GNU) 11.1.0"
