/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2011
 *
 *    File name   : ttbl.c
 *    Description : MMU Translation tables
 *
 *    History :
 *    1. Date        : June, 1 2011
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *        The physical memory map is:
 *                                                                      Instruction cache
 *                                                                        Data cache
 *                                                                          User Access
 *                                                                            Supervisor Access
 *                                                                              Read Only
 *  Segment                      Description      Size                   I D U S R
 *  0x00000000 - 0x0001FFFF      OCRAM           128KB                   * * * * -
 *  0x00200000 - 0x0003FFFF      OCRAM           128KB                   - - * * -
 *  0x40000000 - 0x43FFFFFF      EBI RAM          64MB                   * * * * -
 *  0x44000000 - 0x47FFFFFF      EBI RAM          64MB                   - - * * -
 *  0x80000000 - 0x80FFFFFF      SFR AHB           1MB                   - - - * -
 *  0xC0000000 – 0xFFEFFFFF      OCROM          1023MB                   * * * * *
 *  0xFFF00000 – vector handlers OCRAM or EMI      1MB                   * * * * *
 *
 *  Domain 0 - 01 Client
 *  Domain 1 - 11 Manager
 *
 *    $Revision: 44589 $
 **************************************************************************/
#include "ttbl.h"

#pragma segment="MMU_TT"
extern void __vector(void);

#pragma location="MMU_TT"
#pragma data_alignment=16384
Int32U L1Table[L1_ENTRIES_NUMB];

#pragma location="MMU_TT"
#pragma data_alignment=2048
Int32U L2Coarse1[L2_CP_ENTRIES_NUMB];
#pragma location="MMU_TT"
#pragma data_alignment=2048
Int32U L2Coarse2[L2_CP_ENTRIES_NUMB];

const TtSectionBlock_t TtSB[] =
{
  // L1
  //   1 MB coarses table 1 0x0000 0000
  L1_COARSES_PAGE_ENTRY(  1,0x00000000,(Int32U)L2Coarse1  ,  0    ),
  // 64 MB RW section cached (DDRRAM)
  L1_SECTIONS_ENTRY(     64,0x40000000,0x40000000         ,3,0,1,1),
  // 64 MB RW section no cached (DDRRAM)
  L1_SECTIONS_ENTRY(     64,0x44000000,0x44000000         ,3,0,0,0),
  // 1MB RW section no cached (SFR)
  L1_SECTIONS_ENTRY(      1,0x80000000,0x80000000         ,2,1,0,0),
  // 1023MB RW section cached, read only (OCROM)
  L1_SECTIONS_ENTRY(   1023,0xC0000000,0xC0000000         ,3,0,1,1),
  //1 MB coarses table 4  vectors remap
  L1_COARSES_PAGE_ENTRY(  1,0xFFF00000,(Int32U)L2Coarse2  ,  0    ),
  TSB_INVALID,

  // L2 coarses table 1
  // 128KB 2*64 large pages OCRAM
  L2_CT_LARGE_PAGE_ENTRY( 2,0x00000000,0x00000000         ,3,3,3,3,1,1),
  // 128KB 2*64 large pages OCRAM
  L2_CT_LARGE_PAGE_ENTRY( 2,0x00200000,0x00200000         ,3,3,3,3,0,0),
  TSB_INVALID,

  // L2 coarses table 2
  // 4KB 4*1 large pages OCRAM
  L2_CT_SMALL_PAGE_ENTRY( 1,0xFFFF0000,(Int32U)__vector   ,2,2,2,2,1,1),
  TSB_INVALID,
};

const TtTableBlock_t TtTB[] =
{
  {L1Table,TableL1},
  {L2Coarse1,TableL2_Coarse},
  {L2Coarse2,TableL2_Coarse},
  TTB_INVALID
};
