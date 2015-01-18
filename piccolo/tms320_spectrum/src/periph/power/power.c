/*
 * power.c
 *
 *  Created on: 21.10.2014
 *      Author: Simakov Vladimir
 */

#include <cpu/dsp2802x/DSP2802x_Device.h>
#include <cpu/dsp2802x/DSP2802x_Examples.h>
#include <periph/watchdog/wdg.h>

static void power_clock_init_osc_sel (
  void
)
{
    EALLOW;
    SysCtrlRegs.CLKCTL.bit.INTOSC1OFF = 0;
    SysCtrlRegs.CLKCTL.bit.OSCCLKSRCSEL=0;  // Clk Src = INTOSC1
    SysCtrlRegs.CLKCTL.bit.XCLKINOFF=1;     // Turn off XCLKIN
    SysCtrlRegs.CLKCTL.bit.XTALOSCOFF=1;    // Turn off XTALOSC
    SysCtrlRegs.CLKCTL.bit.INTOSC2OFF=1;    // Turn off INTOSC2
    EDIS;
}

static void power_clock_init_pll(
  Uint16 val,
  Uint16 divsel
)
{
    volatile Uint16 iVol;

    // Make sure the PLL is not running in limp mode
    if (SysCtrlRegs.PLLSTS.bit.MCLKSTS != 0)
    {
       EALLOW;
       // OSCCLKSRC1 failure detected. PLL running in limp mode.
       // Re-enable missing clock logic.
       SysCtrlRegs.PLLSTS.bit.MCLKCLR = 1;
       EDIS;
       // Replace this line with a call to an appropriate
       // SystemShutdown(); function.
       asm("        ESTOP0");     // Uncomment for debugging purposes
    }

    // DIVSEL MUST be 0 before PLLCR can be changed from
    // 0x0000. It is set to 0 by an external reset XRSn
    // This puts us in 1/4
    if (SysCtrlRegs.PLLSTS.bit.DIVSEL != 0)
    {
        EALLOW;
        SysCtrlRegs.PLLSTS.bit.DIVSEL = 0;
        EDIS;
    }

    // Change the PLLCR
    if (SysCtrlRegs.PLLCR.bit.DIV != val)
    {

       EALLOW;
       // Before setting PLLCR turn off missing clock detect logic
       SysCtrlRegs.PLLSTS.bit.MCLKOFF = 1;
       SysCtrlRegs.PLLCR.bit.DIV = val;
       EDIS;

       // Optional: Wait for PLL to lock.
       // During this time the CPU will switch to OSCCLK/2 until
       // the PLL is stable.  Once the PLL is stable the CPU will
       // switch to the new PLL value.
       //
       // This time-to-lock is monitored by a PLL lock counter.
       //
       // Code is not required to sit and wait for the PLL to lock.
       // However, if the code does anything that is timing critical,
       // and requires the correct clock be locked, then it is best to
       // wait until this switching has completed.

       // Wait for the PLL lock bit to be set.

       // The watchdog should be disabled before this loop, or fed within
       // the loop via ServiceDog().

       // Uncomment to disable the watchdog
       watchdog_disable();

       while(SysCtrlRegs.PLLSTS.bit.PLLLOCKS != 1)
       {
           // Uncomment to service the watchdog
           // ServiceDog();
       }

       EALLOW;
       SysCtrlRegs.PLLSTS.bit.MCLKOFF = 0;
       EDIS;
     }

     // If switching to 1/2
     if((divsel == 1)||(divsel == 2))
     {
         EALLOW;
         SysCtrlRegs.PLLSTS.bit.DIVSEL = divsel;
         EDIS;
     }

     // If switching to 1/1
     // * First go to 1/2 and let the power settle
     //   The time required will depend on the system, this is only an example
     // * Then switch to 1/1
     if(divsel == 3)
     {
         EALLOW;
         SysCtrlRegs.PLLSTS.bit.DIVSEL = 2;
//         DELAY_US(50L);
         SysCtrlRegs.PLLSTS.bit.DIVSEL = 3;
         EDIS;
     }
}

static void power_disable_all(
   void
)
{
	EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK    = 0;     // ADC
    SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 0; // GPIO
    SysCtrlRegs.PCLKCR3.bit.COMP1ENCLK  = 0;  // COMP1
    SysCtrlRegs.PCLKCR3.bit.COMP2ENCLK  = 0;  // COMP2
    SysCtrlRegs.PCLKCR0.bit.I2CAENCLK   = 0;    // I2C
    SysCtrlRegs.PCLKCR0.bit.SPIAENCLK   = 0;      // SPI-A
    SysCtrlRegs.PCLKCR0.bit.SCIAENCLK   = 0;   // SCI-A
    SysCtrlRegs.PCLKCR1.bit.ECAP1ENCLK  = 0;  // eCAP1
    SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK  = 0;   // ePWM1
    SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK  = 0;   // ePWM2
    SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK  = 0;   // ePWM3
    SysCtrlRegs.PCLKCR1.bit.EPWM4ENCLK  = 0;   // ePWM4
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC   = 0;    // Enable TBCLK
    EDIS;
}


void power_clock_init(
   void
)
{
	power_clock_init_osc_sel();
	power_clock_init_pll(DSP28_PLLCR, DSP28_DIVSEL);
	power_disable_all();
}








