.syntax unified
.cpu cortex-m7
.fpu fpv5-sp-d16
.thumb

.global g_pfnVectors
.global Reset_Handler
.global Default_Handler

.extern main
.extern SystemInit

.word _sidata
.word _sdata
.word _edata
.word _sbss
.word _ebss

.section .text.Reset_Handler
.type Reset_Handler, %function

Reset_Handler:
    ldr sp, =_estack

    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata

CopyData:
    cmp r0, r1
    bcs ZeroBss

    ldr r3, [r2], #4
    str r3, [r0], #4
    b CopyData

ZeroBss:
    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r2, #0

ClearBss:
    cmp r0, r1
    bcs CallSystemInit

    str r2, [r0], #4
    b ClearBss

CallSystemInit:
    bl SystemInit
    bl main

Hang:
    b Hang

.size Reset_Handler, .-Reset_Handler

.section .text.Default_Handler,"ax",%progbits
Default_Handler:
    b Default_Handler

.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object

g_pfnVectors:
    .word _estack
    .word Reset_Handler

    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word Default_Handler
    .word Default_Handler
    .word 0
    .word Default_Handler
    .word Default_Handler

    /* STM32H743 external interrupts.
       For the first build, all point to Default_Handler. */

    .rept 150
    .word Default_Handler
    .endr

.size g_pfnVectors, .-g_pfnVectors