#include <cpu/dsp2802x/DSP28x_Project.h>

// periph
#include <periph/power/power.h>
#include <periph/pie/pie.h>
#include <periph/timer/timer.h>
#include <periph/adc/adc.h>

#include <math/fix_fft.h>
#include <math/fir.h>
#include <math/math.h>

#include <lcd/lcd.h>

#include <st/service.h>

void initialize_peripheral();
void paint(void);
interrupt void  adc_isr(void);
interrupt void fps_isr(void);

#define SAMPLE_RATE_IN_HZ 128000
#define INTERRUPT_NUMBER  1
#define AIO_PIN_NUMBER    4
#define ADC_SOC_NUMBER    0

#define FFT_WINDOW_SIZE   64

/* Create an Instance of FFT module                 */

#pragma DATA_SECTION(input_data_i, "FFT_I");
#pragma DATA_SECTION(input_data_q, "FFT_Q");

int16 input_data_i[FFT_WINDOW_SIZE+FIR_LENGTH];
int16 input_data_q[FFT_WINDOW_SIZE+FIR_LENGTH];

Uint16 sample_counter   = 0;
Uint16 fps_actual   = 0;
Uint16 fps_runnable = 0;

adc_options_t adc_options = {
   .soc_number       = ADC_SOC_NUMBER,
   .interrupt_number = INTERRUPT_NUMBER,
   .timer            = &CpuTimer0,
   .io_input         = AIO_PIN_NUMBER,
   .freq_in_Hz       = SAMPLE_RATE_IN_HZ
};

#define FPS_CHECK_IN_MS   100

#define X0  0
#define Y0  52
#define W   64
#define H   50
#define FNT_WIDTH    4
#define FNT_HEIGHT   6
#define Ytext  Y0+FNT_HEIGHT+5
#define W2CH   FNT_WIDTH+FNT_WIDTH+1
#define W2CH_HALF   (W2CH>>1)
#define W3CH   W2CH+FNT_WIDTH+1
#define W3CH_HALF   (W3CH>>1)


#define LCD_MAX_FREQ      20000
#define LCD_MAX_X         90
#define LCD_MAX_COL_COUNT (LCD_MAX_FREQ / (SAMPLE_RATE_IN_HZ / FFT_WINDOW_SIZE))
#define LCD_COL_STEP      LCD_MAX_X / LCD_MAX_COL_COUNT

void paint(
  void
)
// draw X,Y-axis and mag[] graph
{
    int i, x;
    long y;
    int32 curvalue, maxvalue;

    fps_runnable++;

    maxvalue = 16382;

    lcd_paintbegin();


    int res;
    int16 value;
	int column_width = LCD_MAX_X / LCD_MAX_COL_COUNT;

    for(x = LCD_COL_STEP, i = 1; i <= LCD_MAX_COL_COUNT; i++, x+= LCD_COL_STEP)
    {
    	curvalue = FIX_MPY(input_data_i[i], input_data_i[i]) +
                   FIX_MPY(input_data_q[i], input_data_q[i]);
    	res = sqrt_q15(curvalue,&value);
    	curvalue = res >= 0 ? value : 0;

        if (maxvalue != 0.0)
        {
           curvalue = H*curvalue/maxvalue;     ///10*log(curvalue/maxvalue);               // 0..100 dB scale
           y = (int) curvalue;
           if (y < 0 ) y = -y;
        }
        else
          y = 0;

        lcd_rect(x - column_width / 2, Y0 - y, column_width, y);
//      lcd_line(x, Y0, x, Y0 - y);
    }

    lcd_line(X0, Y0, X0 + LCD_MAX_X, Y0);
    lcd_line(X0, Y0, X0, Y0 - H);


   // 1pt corresponding 217 Hz

    print_str("0", 0, Ytext);
    lcd_line(0, Y0+1, 0, Y0+3);
    print_str("6k", 3*LCD_COL_STEP-W2CH_HALF, Ytext);
    lcd_line(3*LCD_COL_STEP, Y0+1, 3*LCD_COL_STEP, Y0+3);
    print_str("12k", 6*LCD_COL_STEP-W2CH_HALF, Ytext);
    lcd_line(6*LCD_COL_STEP, Y0+1, 6*LCD_COL_STEP, Y0+3);
    print_str("20k", 10*LCD_COL_STEP-W2CH_HALF, Ytext);
    lcd_line(10*LCD_COL_STEP, Y0+1, 10*LCD_COL_STEP, Y0+3);
//   print_str("10k", 46-W3CH_HALF, Ytext);
//   lcd_line(46, Y0+1, 46, Y0+3);
//   print_str("15k", 69-W3CH_HALF, Ytext);
//   lcd_line(69, Y0+1, 69, Y0+3);
//   print_str("20k", 92-W3CH_HALF, Ytext);
//   lcd_line(92, Y0+1, 92, Y0+3);
//   print_str("25k", 115-W3CH_HALF, Ytext);
//   lcd_line(115, Y0+1, 115, Y0+3);

    print_str("fps", 105, 30);
    lcd_print_num(fps_actual, 10, 0, 107, 38);


    lcd_paintend();
 }


void initialize_peripheral()
 {
    volatile Uint16 iVol;



// PLL, WatchDog, enable Peripheral Clocks
   power_clock_init();

   pie_init();

   timers_init();
   timer_configure(&CpuTimer1, 1000 / FPS_CHECK_IN_MS);
   timer_register_isr(CpuTimer1.number, fps_isr);


   pie_allow_interrupt(1,adc_options.interrupt_number);
   adc_init();
   adc_configure(&adc_options);
   adc_register_isr(adc_isr, &adc_options);

   EINT;   // Enable Global interrupt INTMv


}


void main(void)
 {


   initialize_peripheral();

   lcd_gpio_congigure();
   lcd_init();

   fir_init();
   
   adc_start(&adc_options);
   timer_start(&CpuTimer1);

   for(;;)
   {
       if (sample_counter >= FFT_WINDOW_SIZE+FIR_LENGTH){
           DINT;

           fir_process_real(input_data_i, FFT_WINDOW_SIZE+FIR_LENGTH);
           fix_fft_iq(input_data_i, input_data_q, 6, 0);

           paint();

           service_clear_array(input_data_q, FFT_WINDOW_SIZE+FIR_LENGTH);

           sample_counter = 0;
           EINT;
       }
   }
 }

interrupt void  adc_isr(void)
{
   input_data_i[sample_counter] = -32768 + (adc_read_result(ADC_SOC_NUMBER) << 4);

//   int32 test;
//   test = input_data_i[sample_counter];
//   test++;
   sample_counter++;


   AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;		//Clear ADCINT1 flag reinitialize for next SOC
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE
   return;
}

interrupt void fps_isr(void)
{
    fps_actual   = (1000 / FPS_CHECK_IN_MS) * fps_runnable;
    fps_runnable = 0;
}
