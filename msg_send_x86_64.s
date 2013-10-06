//
//  msg_send_x86_64.s
//  Objective-SEG
//
//  Created by Samuel E. Giddins on 10/6/13.
//
//

#ifdef __x86_64__
.macro ENTRY
	.text
	.globl	$0
	.align	6, 0x90
$0:
.endmacro

ENTRY _seg_generic_msg_send
//    testq %rsi, %rsi
//    leave
//    ret
    pushq %rsi
    pushq %rdi
    pushq %rdx
    pushq %rcx
    pushq %r8
    pushq %r9
    pushq %r12
//    mov %rsp, %r12
//    andq $-0x10, %rsp
    callq _seg_get_imp
    mov %rax, %r11
//    mov %r12, %rsp
//    popq %r12
    popq %rax
    popq %r9
    popq %r8
    popq %rcx
    popq %rdx
    popq %rdi
    popq %rsi
//    mov %rsp, %r12
//    andq $-0x10, %rsp
    cmp	%r11, %r11
    jmp *%r11
#endif