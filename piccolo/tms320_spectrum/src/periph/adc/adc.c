/*
 * adc.c
 *
 *  Created on: 14.10.2014
 *      Author: Simakov Vladimir
 */
#include <cpu/dsp2802x/DSP28x_Project.h>
#include <periph/adc/adc.h>
#include <periph/timer/timer.h>
#include  <periph/gpio/gpio.h>

#define ADC_INT_MAX_COUNT 9
#define ADC_INT_MIN_COUNT 1

static interrupt void adc_interrupt_default_handler(
  void
)
{
	   AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;		//Clear ADCINT1 flag reinitialize for next SOC
	   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE
	   return;
}

static void adc_configure_soc(
  Uint16 soc_number,
  Uint16 int_number,
  Uint16 timer_number,
  Uint16 io_input
)
{
    Uint16 index;
    Uint16 mod;

	EALLOW;

	// Int after end
	AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;

	mod   = int_number % 2;
	index = (int_number - 1) >> 1;

	union INTSEL_REG *sel_reg = (union INTSEL_REG *)&AdcRegs.INTSEL1N2.all;

	// Configure internal ADC interrupt of socs
	if (mod){
		sel_reg[index].bit.INTODDSEL  = soc_number;
		sel_reg[index].bit.INTODDCONT = 0;
		sel_reg[index].bit.INTODDE    = 1;
	} else {
		sel_reg[index].bit.INTEVENSEL  = soc_number;
		sel_reg[index].bit.INTEVENCONT = 0;
		sel_reg[index].bit.INTEVENE    = 1;
	}

	union  ADCSOCxCTL_REG *soc_ctl = (union  ADCSOCxCTL_REG *)&AdcRegs.ADCSOC0CTL.all;
	soc_ctl[soc_number].bit.CHSEL    = io_input;        //set SOC0 channel select to ADCINA4
	soc_ctl[soc_number].bit.TRIGSEL  = timer_number + 1;    //set SOC0 start trigger on Timer0
	soc_ctl[soc_number].bit.ACQPS    = 6;

	EDIS;
}

void adc_init(
  void
)
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1; // clk on

    AdcRegs.ADCCTL1.bit.ADCREFSEL = 0;      // Select interal Bandgap (Voltage)
    AdcRegs.ADCCTL1.bit.ADCBGPWD  = 1;      // Power ADC BG // on Bandgap
    AdcRegs.ADCCTL1.bit.ADCREFPWD = 1;      // Power reference
    AdcRegs.ADCCTL1.bit.ADCPWDN   = 1;      // Power ADC
    AdcRegs.ADCCTL1.bit.ADCENABLE = 1;      // Enable ADC

    asm(" RPT#100 || NOP"); // Nop 100 cycles

    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;   //ADCINT1 trips after AdcResults latch

    EDIS;
}

void adc_configure(
  adc_options_t *options
)
{
    if (   options->interrupt_number < ADC_INT_MIN_COUNT
		|| options->interrupt_number > ADC_INT_MAX_COUNT )
    	return;

    timer_configure(options->timer, options->freq_in_Hz);

    adc_configure_soc(options->soc_number,
					  options->interrupt_number,
					  options->timer->number,
					  options->io_input);

    adc_register_isr(adc_interrupt_default_handler, options);

    gpio_aio_configure(options->io_input,GPIO_AIO_ADC_FN);
//    GpioCtrlRegs.AIOMUX1.bit.AIO4    = 2;      // Configure AIO4 for A4 (analog input) operation
}

void adc_start(
  adc_options_t *options
)
{
   IER |= 1 << (options->interrupt_number - 1);

   timer_start(options->timer);
}

Uint16 adc_read_result(
	Uint16 soc_number
)
{
	Uint16 *res_regs = (Uint16 *)&AdcResult.ADCRESULT0;

	return res_regs[soc_number];
}


void adc_register_isr(
  PINT   isr,
  adc_options_t *options
)
{
	Uint16 int_number = options->interrupt_number;

    if (int_number < ADC_INT_MIN_COUNT || int_number  > ADC_INT_MAX_COUNT)
        return;

    EALLOW;

    Uint32 *table;
    Uint32  index = 0;
    if (int_number < 3){
        table = (Uint32 *)&PieVectTable.ADCINT1;
        index = int_number - 1;
    } else if (int_number < 9) {
        table = (Uint32 *)&PieVectTable.ADCINT3;
        index = int_number - 3;
    } else {
        table = (Uint32 *)&PieVectTable.ADCINT9;
    }

    table[index] = (Uint32)isr;

    EDIS;
}


