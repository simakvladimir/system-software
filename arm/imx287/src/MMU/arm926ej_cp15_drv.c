/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : arm926ej_cp15_drv.c
 *    Description : Driver for the ARM926EJ's CP15
 *
 *    History :
 *    1. Date        : September, 8 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 44589 $
 **************************************************************************/

#include "arm926ej_cp15_drv.h"


int temp = 0;
/*************************************************************************
 * Function Name: CP15_GetID
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Function returns the ID register
 *
 *************************************************************************/
Int32U CP15_GetID (void)
{
  return(__MRC(15,0,CP15_ID,0,0));
}

/*************************************************************************
 * Function Name: CP15_GetCacheType
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Function returns the Cache type
 *
 *************************************************************************/
Int32U CP15_GetCacheType (void)
{
  return(__MRC(15,0,CP15_ID,0,1));
}

/*************************************************************************
 * Function Name: CP15_GetTCM_Status
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Function returns the TCM status
 *
 *************************************************************************/
Int32U CP15_GetTCM_Status (void)
{
  return(__MRC(15,0,CP15_ID,0,2));
}

/*************************************************************************
 * Function Name: CP15_GetTtb
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Function returns the TTB register
 *
 *************************************************************************/
Int32U CP15_GetTtb (void)
{
  return(__MRC(15,0,CP15_TTB_ADDR,0,0));
}

/*************************************************************************
 * Function Name: CP15_GetStatus
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Function returns the MMU control register
 *
 *************************************************************************/
Int32U CP15_GetStatus (void)
{
  return( __MRC(15,0,CP15_CTRL,0,0));
}

/*************************************************************************
 * Function Name: CP15_GetDomain
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Function returns the MMU domain access register
 *
 *************************************************************************/
Int32U CP15_GetDomain (void)
{
  return(__MRC(15,0,CP15_DA_CTRL,0,0));
}

/*************************************************************************
 * Function Name: CP15_SetDomain
 * Parameters: Int32U DomainAccess
 *
 * Return: Int32U
 *
 * Description: Function set the MMU domain access register
 *
 *************************************************************************/
void CP15_SetDomain (Int32U DomainAccess)
{
 Int32U Val = DomainAccess;
  __MCR(15,0,Val,CP15_DA_CTRL,0,0);
}

/*************************************************************************
 * Function Name: CP15_InvalAllCache
 * Parameters: none
 *
 * Return: none
 *
 * Description: Invalidate entire cache
 *
 *************************************************************************/
void CP15_InvalAllCache (void)
{
 volatile Int32U Dummy = 0;
  __MCR(15,0,Dummy,CP15_CACHE_OPR,7,0);
}

/*************************************************************************
 * Function Name: CP15_InvalAllTbl
 * Parameters: none
 *
 * Return: none
 *
 * Description: Invalidate TLB
 *
 *************************************************************************/
void CP15_InvalAllTbl (void)
{
 volatile Int32U Dummy = 0;
  __MCR(15,0,Dummy,CP15_TBL_OPR,7,0);
}

/*************************************************************************
 * Function Name: CP15_SetMmu
 * Parameters: Int32U Ctrl
 *
 * Return: none
 *
 * Description: Set CP15 control register
 *
 *************************************************************************/
void CP15_SetMmu (Int32U Ctrl)
{
 volatile Int32U Val = Ctrl;
  __MCR(15,0,Val,CP15_CTRL,0,0);
}

/*************************************************************************
 * Function Name: CP15_SetTtb
 * Parameters: pInt32U pTtb
 *
 * Return: none
 *
 * Description: Set CP15 TTB base address register
 *
 *************************************************************************/
void CP15_SetTtb (pInt32U pTtb)
{
 volatile Int32U Val = (Int32U)pTtb;
  __MCR(15,0,Val,CP15_TTB_ADDR,0,0);
}

/*************************************************************************
 * Function Name: CP15_SetDac
 * Parameters: Int32U da
 *
 * Return: none
 *
 * Description: Set CP15 domain access register
 *
 *************************************************************************/
void CP15_SetDac (Int32U da)
{
 volatile Int32U Val = da;
  __MCR(15,0,Val,CP15_DA_CTRL,0,0);
}

/*************************************************************************
 * Function Name: CP15_WriteBuffFlush
 * Parameters: none
 *
 * Return: none
 *
 * Description:  Flush the write buffer and wait for completion
 *              of the flush.
 *
 *************************************************************************/
void CP15_WriteBuffFlush (void)
{
 volatile Int32U Val;
  __MCR(15,0,Val,CP15_CACHE_OPR,10,4);
}

/*************************************************************************
 * Function Name: CP15_GetFaultStat
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Function returns the MMU fault status register
 *
 *************************************************************************/
Int32U CP15_GetFaultStat (void)
{
  __MRC(15,0,CP15_FAULT_STAT,0,0);
  return temp;
}

/*************************************************************************
 * Function Name: CP15_GetFaultAddr
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Function returns the MMU fault address register
 *
 *************************************************************************/
Int32U CP15_GetFaultAddr (void)
{
  __MRC(15,0,CP15_FAULT_ADDR,0,0);
  return temp;
}

/*************************************************************************
 * Function Name: CP15_GetFcsePid
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Function returns the MMU Process identifier
 *             FCSE PID register
 *
 *************************************************************************/
Int32U CP15_GetFcsePid (void)
{
  __MRC(15,0,CP15_PROCESS_IDNF,0,0);
  return temp;
}

/*************************************************************************
 * Function Name: CP15_GetPraceProcId
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Function returns the MMU Trace Process identifier
 *             register
 *
 *************************************************************************/
Int32U CP15_GetPraceProcId (void)
{
  __MRC(15,0,CP15_PROCESS_IDNF,0,1);
  return temp;
}

/*************************************************************************
 * Function Name: CP15_SetFcsePid
 * Parameters: Int32U FcsePid
 *
 * Return: none
 *
 * Description: Function set the MMU Process identifier
 *             FCSE PID register
 *
 *************************************************************************/
void CP15_SetFcsePid (Int32U FcsePid)
{
 Int32U Val = FcsePid;
  __MCR(15,0,Val,CP15_PROCESS_IDNF,0,0);
}

/*************************************************************************
 * Function Name: CP15_GetPraceProcId
 * Parameters: Int32U
 *
 * Return: none
 *
 * Description: Function set the MMU Trace Process identifier
 *             register
 *
 *************************************************************************/
void CP15_SetPraceProcId (Int32U Trace)
{
 Int32U Val = Trace;
  __MCR(15,0,Val,CP15_PROCESS_IDNF,0,1);
}

/*************************************************************************
 * Function Name: CP15_WriteBuffFlush
 * Parameters: pTtSectionBlock_t pTtSB, pTtTableBlock_t pTtTB
 *
 * Return: Boolean
 *
 *  Returns error if MMU is enabled or if target
 * Translation Table address is not 16K aligned. Clear the
 * Translation Table area. Build the Translation Table from the
 * initialization data in the Section Block array. Return no error.
 *
 * Description:  Initializes the MMU tables.
 *
 *
 *************************************************************************/
Boolean CP15_InitMmuTtb(const TtSectionBlock_t * pTtSB,
                        const TtTableBlock_t * pTtTB)
{
Int32U i, pa, pa_inc, va_ind;
pInt32U pTtb;
TableType_t TableType;
  while(1)
  {
    TableType = pTtTB->TableType;
    switch(TableType)
    {
    case TableL1:
      pTtb = pTtTB->TableAddr;
      if((Int32U)pTtb & L1_ENTRIES_NUMB-1)
      {
        return(FALSE);
      }
      pa_inc = 0x100000;
      pa = L1_ENTRIES_NUMB;
      break;
    case TableL2_Coarse:
      pTtb = pTtTB->TableAddr;
      if((Int32U)pTtb & L2_CP_ENTRIES_NUMB-1)
      {
        return(FALSE);
      }
      pa_inc = 0x1000;
      pa = L2_CP_ENTRIES_NUMB;
      break;
    case TableL2_Fine:
      pTtb = pTtTB->TableAddr;
      if((Int32U)pTtb & L2_FP_ENTRIES_NUMB-1)
      {
        return(FALSE);
      }
      pa_inc = 0x400;
      pa = L2_FP_ENTRIES_NUMB;
      break;
    default:
      return(TRUE);
    }

    // Clear the entire Translation Table This results in LxD_TYPE_FAULT
    // being the default for any uninitialized entries.
    for(i = 0; i < pa; ++i)
    {
      *(pTtb+i) = TT_ENTRY_INVALID;
    }

    // Build the translation table from user provided pTtSectionBlock_t array
    while(pTtSB->NubrOfSections != 0)
    {
      pa = pTtSB->PhysAddr;
      switch(TableType)
      {
      case TableL1:
        va_ind = (pTtSB->VirtAddr >> 20) & (L1_ENTRIES_NUMB-1);
        if((va_ind + pTtSB->NubrOfSections) > L1_ENTRIES_NUMB)
        {
          return(FALSE);
        }
        break;
      case TableL2_Coarse:
        va_ind = (pTtSB->VirtAddr >> 12) & (L2_CP_ENTRIES_NUMB-1);
        if((va_ind + pTtSB->NubrOfSections) > L2_CP_ENTRIES_NUMB)
        {
          return(FALSE);
        }
        break;
      case TableL2_Fine:
        va_ind = (pTtSB->VirtAddr >> 10) & (L2_FP_ENTRIES_NUMB-1);
        if((va_ind + pTtSB->NubrOfSections) > L2_FP_ENTRIES_NUMB)
        {
          return(FALSE);
        }
      }
      for(i = 0; i < pTtSB->NubrOfSections; ++i, ++va_ind)
      {
        switch(TableType)
        {
        case TableL1:
          switch(pTtSB->Entry.Type)
          {
          case TtL1CoarsePage:
            *(pTtb+va_ind) |= pTtSB->Entry.Data | (pa & TTL1_CP_PADDR_MASK);
            break;
          case TtL1Section:
            *(pTtb+va_ind) |= pTtSB->Entry.Data | (pa & TTL1_SECTION_PADDR_MASK);
            break;
          case TtL1FinePage:
            *(pTtb+va_ind) |= pTtSB->Entry.Data | (pa & TTL1_FP_PADDR_MASK);
            break;
          default:
            return(FALSE);
          }
          break;
        case TableL2_Coarse:
          switch(pTtSB->Entry.Type)
          {
          case TtL2LargePage:
            *(pTtb+va_ind) |= pTtSB->Entry.Data | (pa & TTL2_LP_PADDR_MASK);
            break;
          case TtL2SmallPage:
            *(pTtb+va_ind) |= pTtSB->Entry.Data | (pa & TTL2_SP_PADDR_MASK);
            break;
          default:
            return(FALSE);
          }
          break;
        case TableL2_Fine:
          switch(pTtSB->Entry.Type)
          {
          case TtL2LargePage:
            *(pTtb+va_ind) |= pTtSB->Entry.Data | (pa & TTL2_LP_PADDR_MASK);
            break;
          case TtL2SmallPage:
            *(pTtb+va_ind) |= pTtSB->Entry.Data | (pa & TTL2_SP_PADDR_MASK);
            break;
          case TtL2TinyPage:
            *(pTtb+va_ind) |= pTtSB->Entry.Data | (pa & TTL2_TP_PADDR_MASK);
            break;
          default:
            return(FALSE);
          }
          break;
        }
        pa += pa_inc;
      }
      ++pTtSB;
    }
    ++pTtSB;
    ++pTtTB;
  }
}

/*************************************************************************
 * Function Name: CP15_Mmu
 * Parameters: Boolean Enable
 *
 * Return: none
 *
 * Description: Enable/Disable MMU
 *
 *************************************************************************/
void CP15_Mmu (Boolean Enable)
{
Int32U Val = CP15_GetStatus();
  if(Enable)
  {
    Val |= CP15_CTRL_M;
  }
  else
  {
    Val &= ~(CP15_CTRL_M | CP15_CTRL_C);
  }
  CP15_InvalAllTbl();
  CP15_SetMmu(Val);
}

/*************************************************************************
 * Function Name: CP15_Cache
 * Parameters: Boolean Enable
 *
 * Return: none
 *
 * Description: Enable/Disable Both Cache
 *
 *************************************************************************/
void CP15_Cache (Boolean Enable)
{
Int32U Val = CP15_GetStatus();
  if(Enable)
  {
    CP15_InvalAllCache();
    Val |= CP15_CTRL_M | CP15_CTRL_C | CP15_CTRL_I;
  }
  else
  {
    Val &= ~CP15_CTRL_C;
  }
  CP15_SetMmu(Val);
}

/*************************************************************************
 * Function Name: CP15_ICache
 * Parameters: Boolean Enable
 *
 * Return: none
 *
 * Description: Enable/Disable I cache
 *
 *************************************************************************/
void CP15_ICache (Boolean Enable)
{
Int32U Val = CP15_GetStatus();
  if(Enable)
  {
    Val |= CP15_CTRL_I;
  }
  else
  {
    Val &= ~CP15_CTRL_I;
  }
  CP15_SetMmu(Val);
}

/*************************************************************************
 * Function Name: CP15_DCache
 * Parameters: Boolean Enable
 *
 * Return: none
 *
 * Description: Enable/Disable D cache
 *
 *************************************************************************/
void CP15_DCache (Boolean Enable)
{
Int32U Val = CP15_GetStatus();
  if(Enable)
  {
    Val |= CP15_CTRL_M | CP15_CTRL_C;
  }
  else
  {
    Val &= ~CP15_CTRL_C;
  }
  CP15_SetMmu(Val);
}

/*************************************************************************
 * Function Name: CP15_SysProt
 * Parameters: Boolean Enable
 *
 * Return: none
 *
 * Description: Enable/Disable S cache
 *
 *************************************************************************/
void CP15_SysProt (Boolean Enable)
{
Int32U Val = CP15_GetStatus();
  if(Enable)
  {
    Val |= CP15_CTRL_S;
  }
  else
  {
    Val &= ~CP15_CTRL_S;
  }
  CP15_SetMmu(Val);
}

/*************************************************************************
 * Function Name: CP15_RomProt
 * Parameters: Boolean Enable
 *
 * Return: none
 *
 * Description: Enable/Disable ROM protection.
 *
 *************************************************************************/
void CP15_RomProt (Boolean Enable)
{
Int32U Val = CP15_GetStatus();
  if(Enable)
  {
    Val |= CP15_CTRL_R;
  }
  else
  {
    Val &= ~CP15_CTRL_R;
  }
  CP15_SetMmu(Val);
}

/*************************************************************************
 * Function Name: CP15_exception TODO Added by Simakov
 * Parameters: Boolean Enable
 *
 * Return: none
 *
 * Description: Choose exception vector (0x00000000-0x0000001C) - false OR (0xFFFF0000-0xFFFF001C) - true
 *
 *************************************************************************/
void CP15_exception (Boolean Enable)
{
Int32U Val = CP15_GetStatus();
  if(Enable)
  {
    CP15_InvalAllCache();
    Val |= CP15_CTRL_V;
  }
  else
  {
    Val &= ~CP15_CTRL_V;
  }
  CP15_SetMmu(Val);
}
