

        .global  __vector
        .global   _start

		    .global	undef_handler, swi_handler, prefetch_handler
		    .global	data_handler, IRQ_Handler, FIQ_Handler
                //ARM	; Always ARM mode after reset
__vector:
		ldr	pc,=_start

__undef_handler:
    ldr	pc,[pc,#24]
__swi_handler:
		ldr	pc,[pc,#24]
__prefetch_handler:
		ldr	pc,[pc,#24]
__data_handler:
		ldr	pc,[pc,#24]
		  .word 0xFFFFFFFF
__irq_handler:
		ldr	pc,=ARM_irq
__fiq_handler:
		ldr	pc,[pc,#24]


      .word	_start
      .word	__undef_handler
      .word	__swi_handler
      .word	__prefetch_handler
      .word	__data_handler
	  .word 0xFFFFFFFF
      .word	__irq_handler
      .word	__fiq_handler


#define MODE_MSK  0x1F

#define USR_MODE 0x10
#define FIQ_MODE  0x11
#define IRQ_MODE  0x12
#define SVC_MODE  0x13
#define ABT_MODE  0x17
#define UND_MODE  0x1B
#define SYS_MODE  0x1F

#define NO_IRQ  0x80
#define NO_FIQ  0x40
#define NO_INT (NO_IRQ | NO_FIQ)

#define CP_DIS_MASK           0xFFFFEFFA

        .text

        .global  main
       // REQUIRE __vector

ARM_irq:
      /* IRQ entry {{{ */
         MOV     r13,r0                /* save r0 in r13_IRQ */
         SUB     r0,lr,#4              /* put return address in r0_SYS */
         MOV     lr,r1                 /* save r1 in r14_IRQ (lr) */
         MRS     r1,spsr               /* put the SPSR in r1_SYS */

         MSR     cpsr_c,#(SYS_MODE | NO_IRQ)
                                          /* SYSTEM, no IRQ, but FIQ enabled! */
         STMFD   sp!,{r0,r1}           /* save SPSR and PC on SYS stack */
         STMFD   sp!,{r2-r3,r12,lr}    /* save APCS-regs on SYS stack */
         MOV     r0,sp                 /* make the sp_SYS visible to IRQ mode */
         SUB     sp,sp,#(2*4)          /* make room for stacking (r0, r1) */

         MSR     cpsr_c,#(IRQ_MODE | NO_IRQ)
                                          /* IRQ mode, IRQ/FIQ disabled */
         STMFD   r0!,{r13,r14}         /* finish saving the context (r0,r1)*/

         MSR     cpsr_c,#(SYS_MODE | NO_IRQ) /* SYSTEM mode, IRQ disabled */
      /* IRQ entry }}} */

      /* NOTE: BSP_irq might re-enable IRQ interrupts (the FIQ is enabled
      * already), if IRQs are prioritized by an interrupt controller.
      */
         LDR     r12,=IRQ_Handler
         MOV     lr,pc                 /* copy the return address to link */
         BX      r12                   /* call the C IRQ-handler */

      /* IRQ exit {{{ */
         MSR     cpsr_c,#(SYS_MODE | NO_INT)
                                          /* SYSTEM mode, IRQ/FIQ disabled */
         MOV     r0,sp                 /* make sp_SYS visible to IRQ mode */
         ADD     sp,sp,#(8*4)          /* fake unstacking 8 regs from sp_SYS */

         MSR     cpsr_c,#(IRQ_MODE | NO_INT)
                                          /* IRQ mode, both IRQ/FIQ disabled */
         MOV     sp,r0                 /* copy sp_SYS to sp_IRQ */
         LDR     r0,[sp,#(7*4)]        /* load the saved SPSR from the stack */
         MSR     spsr_cxsf,r0          /* copy it into spsr_IRQ */

         LDMFD   sp,{r0-r3,r12,lr}^    /* unstack all saved SYS registers */
         NOP                           /* can't access banked reg immediately */
         LDR     lr,[sp,#(6*4)]        /* load return address from the SYS stack */
         MOVS    pc,lr                 /* return restoring CPSR from SPSR */
      /* IRQ exit }}} */



_start:

        MRC         p15,0,R1,C1,C0,0
        LDR         R0,=CP_DIS_MASK
        AND         R1,R1,R0
        ORR         R1,R1,#(1<<12) //Mask for enabke instruction cache
        MCR         p15,0,R1,C1,C0,0


        mrs     r0,cpsr
        bic     r0,r0,#MODE_MSK
        orr     r0,r0,#SVC_MODE
        msr     cpsr_c,r0
        ldr     sp,=__svc_stack_top__

        bic     r0,r0,#MODE_MSK
        orr     r0,r0,#ABT_MODE
        msr     cpsr_c,r0
        ldr     sp,=__abt_stack_top__

        bic     r0,r0,#MODE_MSK
        orr     r0,r0,#UND_MODE
        msr     cpsr_c,r0
        ldr     sp,=__und_stack_top__

        bic     r0,r0,#MODE_MSK
        orr     r0,r0,#FIQ_MODE
        msr     cpsr_c,r0
        ldr     sp,=__fiq_stack_top__

        bic     r0,r0,#MODE_MSK
        orr     r0,r0,#IRQ_MODE
        msr     cpsr_c,r0
        ldr     sp,=__irq_stack_top__

        bic     r0,r0,#MODE_MSK
        orr     r0,r0,#SYS_MODE
        msr     cpsr_c,r0
        ldr     sp,=__c_stack_top__

#ifdef __ARMVFP__

        mov         r0, #BASE_ARD_EIM
        fmxr        fpexc, r0


        mov         r0, #0x01000000
        fmxr        fpscr, r0
#endif


        ldr         r0,=main
        bx          r0


                .end
