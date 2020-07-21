/*****************************************************************************/
/* BroadVoice(R)16 (BV16) Floating-Point ANSI-C Source Code                  */
/* Revision Date: October 5, 2012                                            */
/* Version 1.2                                                               */
/*****************************************************************************/

/*****************************************************************************/
/* Copyright 2000-2012 Broadcom Corporation                                  */
/*                                                                           */
/* This software is provided under the GNU Lesser General Public License,    */
/* version 2.1, as published by the Free Software Foundation ("LGPL").       */
/* This program is distributed in the hope that it will be useful, but       */
/* WITHOUT ANY SUPPORT OR WARRANTY; without even the implied warranty of     */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the LGPL for     */
/* more details.  A copy of the LGPL is available at                         */
/* http://www.broadcom.com/licenses/LGPLv2.1.php,                            */
/* or by writing to the Free Software Foundation, Inc.,                      */
/* 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.                 */
/*****************************************************************************/


/*****************************************************************************
  levelest.c : Input level estimation

  $Log$
******************************************************************************/

#include "typedef.h"
#include "bv16cnst.h"

Float	estlevel(
               Float	lg,
               Float	*level,
               Float	*lmax,
               Float	*lmin,
               Float	*lmean,
               Float	*x1,
               short   ngfae,
               short   nggalgc,
               Float   *estl_alpha_min)
{
   Float	lth;
   
   /* Reset forgetting factor for Lmin to fast decay.  This is to avoid Lmin staying at an 
   incorrect low level compensating for the possibility it has caused incorrect bit-error 
   declaration by making the estimated level too low. */
   if(nggalgc == 0){
      *estl_alpha_min = estl_alpha1;
   }
   /* Reset forgetting factor for Lmin to regular decay if fast decay has taken place for
   the past Nfdm frames. */
   else if(nggalgc == Nfdm+1){
      *estl_alpha_min = estl_alpha;
   }
   
   /* update the new maximum, minimum, & mean of log-gain */
   if (lg > *lmax){
      *lmax=lg;	/* use new log-gain as max if it is > max */
   }
   else{
      *lmax=*lmean+estl_alpha*(*lmax-*lmean); /* o.w. attenuate toward lmean */
   }
   
   if (lg < *lmin && ngfae == LGPORDER+1 && nggalgc > LGPORDER
      ){
      *lmin=lg;	/* use new log-gain as min if it is < min */
                  /* Reset forgetting factor for Lmin to regular decay in case it has been on 
      fast decay since it has now found a new minimum level. */
      *estl_alpha_min = estl_alpha;
   }
   else{
      *lmin=*lmean+(*estl_alpha_min)*(*lmin-*lmean); /* o.w. attenuate toward lmean */
   }
   
   *lmean=estl_beta*(*lmean)+estl_beta1*(0.5*(*lmax+*lmin));
   
  	/* update estimated input level, by calculating a running average
   (using an exponential window) of log-gains exceeding lmean */
   lth=*lmean+estl_TH*(*lmax-*lmean);
   if (lg > lth) {
      *x1=estl_a*(*x1)+estl_a1*lg;
      *level=estl_a*(*level)+estl_a1*(*x1);
   }
   
   return	lth;
   
}
