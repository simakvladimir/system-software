#include <board/hw_types.h>
#include <board/hw_memmap.h>
#include <board/hw_sysctl.h>
#include <board/hw_adc.h>
#include <board/hw_ints.h>
#include <drivers/timer.h>
#include <drivers/adc.h>
#include <drivers/interrupt.h>
#include <drivers/sysctl.h>
#include <dft.h>
#include <lcd.h>

#define CPU_FREQ_HZ 25000000
#define ADC_SAMPLE_RATE_HZ 64000

static short adc_data[DFT_WINDOW_SIZE];
static short dft_out[DFT_WINDOW_SIZE];
static short adc_samples_counter = 0;

void handlerADC(
  void
)
{
    ADCIntClear(ADC_BASE, 3);

    adc_data[adc_samples_counter % DFT_WINDOW_SIZE] = HWREG(ADC_BASE + ADC_O_SSFIFO3);
    adc_samples_counter++;

}

static void configureTimer(
  void
)
{
    HWREG(SYSCTL_RCGC1) |= SYSCTL_RCGC1_TIMER0;
    TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
    TimerLoadSet(TIMER0_BASE, TIMER_A, CPU_FREQ_HZ / ADC_SAMPLE_RATE_HZ);
    TimerControlTrigger(TIMER0_BASE, TIMER_A, true);
}

static void configureADC(
  void
)
{
    HWREG(SYSCTL_RCGC0) |= SYSCTL_RCGC0_ADC0;          // enable ADC0
    HWREG(SYSCTL_RCGC0) |= SYSCTL_RCGC0_ADCSPD250K;    // maximum 250K samples/second
    ADCSequenceConfigure(ADC_BASE, 3, ADC_TRIGGER_TIMER, 0); // one sample
    ADCSequenceStepConfigure(ADC_BASE, 3, 0, (ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END));
    ADCSequenceEnable(ADC_BASE, 3);
    ADCIntEnable(ADC_BASE, 3);
    IntEnable(INT_ADC3);
}



#define LCD_MAX_FREQ      10000
#define LCD_MAX_X         128
#define LCD_MAX_COL_COUNT (LCD_MAX_FREQ / (ADC_SAMPLE_RATE_HZ / DFT_WINDOW_SIZE))
#define LCD_WIDTH_COL     LCD_MAX_X / LCD_MAX_COL_COUNT
#define LCD_WIDTH_BETWEEN_COL LCD_WIDTH_COL / 6
#define X0  0
#define Y0  52
#define Ytext  Y0+10
#define H   50
static void paint(
  void
)
{
    int x, y, i;

    lcd_paintbegin();

    for(x = 0, i = 1; i <= LCD_MAX_COL_COUNT; i++, x+= LCD_WIDTH_COL)
    {
        y = (int) dft_out[i] / 90;
        if (y < 0 )
            y = -y;

        lcd_rect(x + LCD_WIDTH_BETWEEN_COL, Y0 - y, LCD_WIDTH_COL - 2 * LCD_WIDTH_BETWEEN_COL, y);
    }

    lcd_line(X0, Y0, X0 + LCD_MAX_X, Y0);
    lcd_line(X0, Y0, X0, Y0 - H);

    print_str("1k",  LCD_WIDTH_COL / 4, Ytext);
    print_str("10k", LCD_WIDTH_COL * 9 + LCD_WIDTH_COL / 4, Ytext);

    lcd_paintend();
}




int main(
  void
)
{
    lcd_configure();
    lcd_init();
    configureADC();
    configureTimer();

    // start acquisition
    TimerEnable(TIMER0_BASE, TIMER_A);

    // infinite event loop
    for(;;){

        if (adc_samples_counter >= DFT_WINDOW_SIZE){
            TimerDisable(TIMER0_BASE, TIMER_A);

            dft(adc_data, dft_out);
            paint();
//
            TimerEnable(TIMER0_BASE, TIMER_A);
            adc_samples_counter = 0;
        }
    }

    return 0;
}

