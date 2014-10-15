/*
 * adc.c
 *
 *  Created on: 14.10.2014
 *      Author: BoBaHouD
 */
#include <DSP2802x/DSP28x_Project.h>
#include <adc/adc.h>

static void adc_interrupt_default_handler(
  void
)
{
  // empty
}



void adc_init(
  void
)
{
    AdcRegs.ADCCTL1.bit.ADCREFSEL = 0;      // Select interal Bandgap (Voltage)
    AdcRegs.ADCCTL1.bit.ADCBGPWD  = 1;      // Power ADC BG // on Bandgap
    AdcRegs.ADCCTL1.bit.ADCREFPWD = 1;      // Power reference
    AdcRegs.ADCCTL1.bit.ADCPWDN   = 1;      // Power ADC
    AdcRegs.ADCCTL1.bit.ADCENABLE = 1;      // Enable ADC

    asm(" RPT#100 || NOP"); // Nop 100 cycles

    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;   //ADCINT1 trips after AdcResults latch
    AdcRegs.ADCSOC0CTL.bit.ACQPS    = 6;   //set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    AdcRegs.INTSEL1N2.bit.INT1SEL   = 0;        //setup EOC0 to trigger ADCINT1 to fire
    AdcRegs.INTSEL1N2.bit.INT1CONT  = 0;      //Disable ADCINT1 Continuous mode
    AdcRegs.INTSEL1N2.bit.INT1E     = 1;      //Enabled ADCINT1
    AdcRegs.ADCSOC0CTL.bit.CHSEL    = 4;        //set SOC0 channel select to ADCINA4
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL  = 1;      //set SOC0 start trigger on Timer0
    GpioCtrlRegs.AIOMUX1.bit.AIO4    = 2;      // Configure AIO4 for A4 (analog input) operation

    PieVectTable.ADCINT1 = &adc_interrupt_default_handler;
}


