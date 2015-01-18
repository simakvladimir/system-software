/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2011
 *
 *    File name   : low_level_init.c
 *    Description : low level init module - trim OSC amplitude
 *    set MPLL and SPLL 266MHz and 300MHz, init CS0 (NOR) and
 *    CSD0 (DDR RAM), init MUX and AITC
 *
 *    History :
 *    1. Date        : May 28, 2011
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 44589 $
 *
 **************************************************************************/
#include <types.h>

#include <freescale/iomcimx28.h>

#define VDDIO_V           10  // 3.3V
#define VDDD_V            30  // 1.55V
#define VDDA_V            12  // 1.8V

static void clear_auto_restart(void)
{
  HW_RTC_CTRL_bit.SFTRST = 0;
  while(HW_RTC_CTRL_bit.SFTRST);
  HW_RTC_CTRL_bit.CLKGATE = 0;
  while(HW_RTC_CTRL_bit.CLKGATE);
  /* Due to the hardware design bug of mx28 EVK-A
  *  we need to set the AUTO_RESTART bit. */
  if(!HW_RTC_PERSISTENT0_bit.AUTO_RESTART)
  {
    while(HW_RTC_STAT_bit.NEW_REGS);
    HW_RTC_PERSISTENT0_bit.AUTO_RESTART = 1;
    HW_RTC_CTRL_bit.FORCE_UPDATE=1;
    HW_RTC_CTRL_bit.FORCE_UPDATE=0;
    while(HW_RTC_STAT_bit.NEW_REGS);
    while(HW_RTC_STAT_bit.STALE_REGS);
  }
}

static void  power_init(void)
{
  /* Select ref_xtal path to generate the CPU clock domain */
  HW_CLKCTRL_CLKSEQ_bit.BYPASS_CPU = 1;
  clear_auto_restart();
  /* Set LDO outputs 25mV below DC-DC*/
	HW_POWER_VDDDCTRL_bit.LINREG_OFFSET  = 2;
	HW_POWER_VDDACTRL_bit.LINREG_OFFSET  = 2;
	HW_POWER_VDDIOCTRL_bit.LINREG_OFFSET = 2;

	HW_POWER_5VCTRL_bit.VBUSVALID_5VDETECT = 1;
	HW_POWER_CTRL_bit.POLARITY_VDD5V_GT_VDDIO = 1;
	HW_POWER_CTRL_bit.POLARITY_VBUSVALID = 0;

  HW_POWER_VDDDCTRL_bit.ENABLE_LINREG = 1;
	HW_POWER_VDDDCTRL_bit.PWDN_BRNOUT = 1;
  HW_POWER_VDDDCTRL_bit.DISABLE_FET = 1;
  HW_POWER_VDDACTRL_bit.ENABLE_LINREG = 1;
	HW_POWER_VDDACTRL_bit.PWDN_BRNOUT = 1;
  HW_POWER_VDDACTRL_bit.DISABLE_FET = 1;
  HW_POWER_VDDIOCTRL_bit.DISABLE_FET = 1;
  HW_POWER_VDDMEMCTRL_bit.ENABLE_LINREG = 0;

  /* Improve efficieny and reduce transient ripple */
  /* Change stepping voltage only after the diff control loop has toggled as well. */
  HW_POWER_LOOPCTRL_bit.TOGGLE_DIF = 1;
  /* Enable the commom mode and differential mode hysterisis */
  HW_POWER_LOOPCTRL_bit.EN_CM_HYST = 1;
  HW_POWER_LOOPCTRL_bit.EN_DF_HYST = 1;
  HW_POWER_DCLIMITS_bit.POSLIMIT_BUCK = 0x30;
	HW_POWER_LOOPCTRL_bit.RCSCALE_THRESH = 1;
  HW_POWER_LOOPCTRL_bit.EN_RCSCALE = 3;
  HW_POWER_MINPWR_bit.HALF_FETS = 0;
  HW_POWER_MINPWR_bit.DOUBLE_FETS = 1;

  HW_POWER_5VCTRL_bit.ENABLE_DCDC = 1;

	/* disable automatic battery voltage measurements */
	HW_LRADC_CONVERSION_bit.AUTOMATIC = 0;
	HW_POWER_BATTMONITOR_bit.BATT_VAL = 0;

  HW_POWER_VDDDCTRL_bit.DISABLE_FET  = 0;
  HW_POWER_VDDACTRL_bit.DISABLE_FET  = 0;
  HW_POWER_VDDIOCTRL_bit.DISABLE_FET = 0;

	/* VDDIO 3.3V is necessary to achieve best power supply capability
	 * and best EMI I/O performance. */
  HW_POWER_VDDIOCTRL_bit.TRG = VDDIO_V;
  volatile int i;
  for( i = 1500; i; i--);
  HW_POWER_VDDIOCTRL_bit.BO_OFFSET = 3;

	/* VDDD 1.55V is necessary to achieve 454 MHZ clock	*/
  HW_POWER_VDDDCTRL_bit.TRG = VDDD_V;
  for( i = 1500; i; i--);
  HW_POWER_VDDDCTRL_bit.BO_OFFSET = 4;

	/* VDDA 1.8V */
  HW_POWER_VDDACTRL_bit.TRG = VDDA_V;
  for( i = 1500; i; i--);
  HW_POWER_VDDACTRL_bit.BO_OFFSET = 4;

	/* setup the auto power down if we lost 5V.*/
  HW_POWER_5VCTRL_bit.PWDN_5VBRNOUT = 1;
}

/* Clock init CPUCLK - 454.73MHz, AHBCLK - 151.57MHz, EMICLK - 205.71MHz */
void clock_init (void)
{
  /* Power on PLL*/
  HW_CLKCTRL_PLL0CTRL0_bit.POWER = 1;
  volatile int i;
  for( i = 1000; i; i--);
  /* Set PLL and clock dividers */
	HW_CLKCTRL_FRAC0_bit.CLKGATEEMI = 1;
  HW_CLKCTRL_FRAC0_bit.EMIFRAC = 21;
	HW_CLKCTRL_FRAC0_bit.CLKGATEEMI = 0;
  for( i = 1000; i; i--);
  HW_CLKCTRL_EMI_bit.CLKGATE = 0;
  HW_CLKCTRL_EMI_bit.DIV_XTAL = 1;
  HW_CLKCTRL_EMI_bit.DIV_EMI = 2;
  HW_CLKCTRL_CLKSEQ_bit.BYPASS_EMI = 0;
  HW_CLKCTRL_FRAC0_bit.CPUFRAC = 19;
  HW_CLKCTRL_HBUS_bit.DIV = 3;
  for( i = 1000; i; i--);
  HW_CLKCTRL_CPU_bit.DIV_CPU = 1;
	HW_CLKCTRL_CLKSEQ_bit.BYPASS_CPU = 0;
}

void ddr_ede1116_200MHz_init(void)
{
	volatile unsigned int * DRAM_REG = (volatile unsigned int*)&HW_DRAM_CTL00;
  /* Set EMI pins */
  /* Configure Bank-3 Pins 0-15 as EMI pins*/
  HW_PINCTRL_MUXSEL10_CLR = 0xFFFFFFFF;
  HW_PINCTRL_MUXSEL11_CLR = 0x0030FFFF;
  HW_PINCTRL_MUXSEL12_CLR = 0x3FFFFFFF;
  HW_PINCTRL_MUXSEL13_CLR = 0x0003FFFF;
	/* set to ddr2 mode*/
	HW_PINCTRL_EMI_DS_CTRL_bit.DDR_MODE = 0x3;
  /* Disable controller */
  HW_DRAM_CTL16_bit.START = 0;
  /* Init EMI register*/
	DRAM_REG[0] = 0x00000000;
	DRAM_REG[1] = 0x00000000;
	DRAM_REG[2] = 0x00000000;
	DRAM_REG[3] = 0x00000000;
	DRAM_REG[4] = 0x00000000;
	DRAM_REG[5] = 0x00000000;
	DRAM_REG[6] = 0x00000000;
	DRAM_REG[7] = 0x00000000;
	DRAM_REG[8] = 0x00000000;
	DRAM_REG[9] = 0x00000000;
	DRAM_REG[10] = 0x00000000;
	DRAM_REG[11] = 0x00000000;
	DRAM_REG[12] = 0x00000000;
	DRAM_REG[13] = 0x00000000;
	DRAM_REG[14] = 0x00000000;
	DRAM_REG[15] = 0x00000000;
	DRAM_REG[16] = 0x00000000;
	DRAM_REG[17] = 0x00000100;
	DRAM_REG[21] = 0x00000000;
	DRAM_REG[22] = 0x00000000;
	DRAM_REG[23] = 0x00000000;
	DRAM_REG[24] = 0x00000000;
	DRAM_REG[25] = 0x00000000;
	DRAM_REG[26] = 0x00010101;
	DRAM_REG[27] = 0x01010101;
	DRAM_REG[28] = 0x000f0f01;
	DRAM_REG[29] = 0x0f02020a;
	DRAM_REG[30] = 0x00000000;
	DRAM_REG[31] = 0x00010101;
	DRAM_REG[32] = 0x00000100;
	DRAM_REG[33] = 0x00000100;
	DRAM_REG[34] = 0x00000000;
	DRAM_REG[35] = 0x00000002;
	DRAM_REG[36] = 0x01010000;
	DRAM_REG[37] = 0x07080403;
	DRAM_REG[38] = 0x06005003;
	DRAM_REG[39] = 0x0a0000c8;
	DRAM_REG[40] = 0x02009c40;
	DRAM_REG[41] = 0x0002030c;
	DRAM_REG[42] = 0x0036a609;
	DRAM_REG[43] = 0x031a0612;
	DRAM_REG[44] = 0x02030202;
	DRAM_REG[45] = 0x00c8001c;
	DRAM_REG[46] = 0x00000000;
	DRAM_REG[47] = 0x00000000;
	DRAM_REG[48] = 0x00012100;
	DRAM_REG[49] = 0xffff0303;
	DRAM_REG[50] = 0x00012100;
	DRAM_REG[51] = 0xffff0303;
	DRAM_REG[52] = 0x00012100;
	DRAM_REG[53] = 0xffff0303;
	DRAM_REG[54] = 0x00012100;
	DRAM_REG[55] = 0xffff0303;
	DRAM_REG[56] = 0x00000003;
	DRAM_REG[57] = 0x00000000;
	DRAM_REG[58] = 0x00000000;
	DRAM_REG[59] = 0x00000000;
	DRAM_REG[60] = 0x00000000;
	DRAM_REG[61] = 0x00000000;
	DRAM_REG[62] = 0x00000000;
	DRAM_REG[63] = 0x00000000;
	DRAM_REG[64] = 0x00000000;
	DRAM_REG[65] = 0x00000000;
	DRAM_REG[66] = 0x00000612;
	DRAM_REG[67] = 0x01000f02;
	DRAM_REG[68] = 0x06120612;
	DRAM_REG[69] = 0x00000200;
	DRAM_REG[70] = 0x00020007;
	DRAM_REG[71] = 0xf4004a27;
	DRAM_REG[72] = 0xf4004a27;
	DRAM_REG[73] = 0xf4004a27;
	DRAM_REG[74] = 0xf4004a27;
	DRAM_REG[75] = 0x07000300;
	DRAM_REG[76] = 0x07000300;
	DRAM_REG[77] = 0x07400300;
	DRAM_REG[78] = 0x07400300;
	DRAM_REG[79] = 0x00000005;
	DRAM_REG[80] = 0x00000000;
	DRAM_REG[81] = 0x00000000;
	DRAM_REG[82] = 0x01000000;
	DRAM_REG[83] = 0x01020408;
	DRAM_REG[84] = 0x08040201;
	DRAM_REG[85] = 0x000f1133;
	DRAM_REG[86] = 0x00000000;
	DRAM_REG[87] = 0x00001f04;
	DRAM_REG[88] = 0x00001f04;
	DRAM_REG[89] = 0x00001f04;
	DRAM_REG[90] = 0x00001f04;
	DRAM_REG[91] = 0x00001f04;
	DRAM_REG[92] = 0x00001f04;
	DRAM_REG[93] = 0x00001f04;
	DRAM_REG[94] = 0x00001f04;
	DRAM_REG[95] = 0x00000000;
	DRAM_REG[96] = 0x00000000;
	DRAM_REG[97] = 0x00000000;
	DRAM_REG[98] = 0x00000000;
	DRAM_REG[99] = 0x00000000;
	DRAM_REG[100] = 0x00000000;
	DRAM_REG[101] = 0x00000000;
	DRAM_REG[102] = 0x00000000;
	DRAM_REG[103] = 0x00000000;
	DRAM_REG[104] = 0x00000000;
	DRAM_REG[105] = 0x00000000;
	DRAM_REG[106] = 0x00000000;
	DRAM_REG[107] = 0x00000000;
	DRAM_REG[108] = 0x00000000;
	DRAM_REG[109] = 0x00000000;
	DRAM_REG[110] = 0x00000000;
	DRAM_REG[111] = 0x00000000;
	DRAM_REG[112] = 0x00000000;
	DRAM_REG[113] = 0x00000000;
	DRAM_REG[114] = 0x00000000;
	DRAM_REG[115] = 0x00000000;
	DRAM_REG[116] = 0x00000000;
	DRAM_REG[117] = 0x00000000;
	DRAM_REG[118] = 0x00000000;
	DRAM_REG[119] = 0x00000000;
	DRAM_REG[120] = 0x00000000;
	DRAM_REG[121] = 0x00000000;
	DRAM_REG[122] = 0x00000000;
	DRAM_REG[123] = 0x00000000;
	DRAM_REG[124] = 0x00000000;
	DRAM_REG[125] = 0x00000000;
	DRAM_REG[126] = 0x00000000;
	DRAM_REG[127] = 0x00000000;
	DRAM_REG[128] = 0x00000000;
	DRAM_REG[129] = 0x00000000;
	DRAM_REG[130] = 0x00000000;
	DRAM_REG[131] = 0x00000000;
	DRAM_REG[132] = 0x00000000;
	DRAM_REG[133] = 0x00000000;
	DRAM_REG[134] = 0x00000000;
	DRAM_REG[135] = 0x00000000;
	DRAM_REG[136] = 0x00000000;
	DRAM_REG[137] = 0x00000000;
	DRAM_REG[138] = 0x00000000;
	DRAM_REG[139] = 0x00000000;
	DRAM_REG[140] = 0x00000000;
	DRAM_REG[141] = 0x00000000;
	DRAM_REG[142] = 0x00000000;
	DRAM_REG[143] = 0x00000000;
	DRAM_REG[144] = 0x00000000;
	DRAM_REG[145] = 0x00000000;
	DRAM_REG[146] = 0x00000000;
	DRAM_REG[147] = 0x00000000;
	DRAM_REG[148] = 0x00000000;
	DRAM_REG[149] = 0x00000000;
	DRAM_REG[150] = 0x00000000;
	DRAM_REG[151] = 0x00000000;
	DRAM_REG[152] = 0x00000000;
	DRAM_REG[153] = 0x00000000;
	DRAM_REG[154] = 0x00000000;
	DRAM_REG[155] = 0x00000000;
	DRAM_REG[156] = 0x00000000;
	DRAM_REG[157] = 0x00000000;
	DRAM_REG[158] = 0x00000000;
	DRAM_REG[159] = 0x00000000;
	DRAM_REG[160] = 0x00000000;
	DRAM_REG[161] = 0x00000000;
	DRAM_REG[162] = 0x00010000;
	DRAM_REG[163] = 0x00030404;
	DRAM_REG[164] = 0x00000003;
	DRAM_REG[165] = 0x00000000;
	DRAM_REG[166] = 0x00000000;
	DRAM_REG[167] = 0x00000000;
	DRAM_REG[168] = 0x00000000;
	DRAM_REG[169] = 0x00000000;
	DRAM_REG[170] = 0x00000000;
	DRAM_REG[171] = 0x01010000;
	DRAM_REG[172] = 0x01000000;
	DRAM_REG[173] = 0x03030000;
	DRAM_REG[174] = 0x00010303;
	DRAM_REG[175] = 0x01020202;
	DRAM_REG[176] = 0x00000000;
	DRAM_REG[177] = 0x02040303;
	DRAM_REG[178] = 0x21002103;
	DRAM_REG[179] = 0x00061200;
	DRAM_REG[180] = 0x06120612;
	DRAM_REG[181] = 0x04420442;
	DRAM_REG[182] = 0x04420442;
	DRAM_REG[183] = 0x00040004;
	DRAM_REG[184] = 0x00040004;
	DRAM_REG[185] = 0x00000000;
	DRAM_REG[186] = 0x00000000;
	DRAM_REG[187] = 0x00000000;
	DRAM_REG[188] = 0x00000000;
	DRAM_REG[189] = 0xffffffff;
  /* Disable self refresh*/
  HW_DRAM_CTL17_bit.SREFRESH = 0;
  /* Start initialization */
  HW_DRAM_CTL16_bit.START = 1;
  while(!(HW_DRAM_CTL58 & 1ul<<20));
}

/*************************************************************************
 * Function Name: low_level_init
 * Parameters: none
 *
 * Return: none
 *
 * Description: Low level init code
 *
 *************************************************************************/
void __low_level_init (void)
{
  /* Power init */
  power_init();
  /* Clock init CPUCLK - 454.73MHz, AHBCLK - 151.57MHz, EMICLK - 205.71MHz */
  clock_init();
  /* DDR memory init */
  ddr_ede1116_200MHz_init();
#if 0
  /* simple memory test*/
  volatile unsigned int * p = (volatile unsigned int *)0x40000000;
  int end;
  int wr_delta;
  int rd_delta;
  int start = HW_DIGCTL_MICROSECONDS;
  for(int i = 0; i < 0x8000000/4; i++)
  {
    *p++ = i;
  }
  end = HW_DIGCTL_MICROSECONDS;
  wr_delta = end - start;
  p = (volatile unsigned int *)0x40000000;
  start = HW_DIGCTL_MICROSECONDS;
  for(int i = 0; i < 0x8000000/4; i++)
  {
    if(*p++ != i)
    {
      __no_operation();
    }
  }
  end = HW_DIGCTL_MICROSECONDS;
  rd_delta = end - start;
  wr_delta = rd_delta;
#endif

}
