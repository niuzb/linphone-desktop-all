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
  postfilt.c : Pitch postfilter.

  $Log$
******************************************************************************/

#include <math.h>
#include "typedef.h"
#include "bv16cnst.h"
#include "bvcommon.h"
#include "bv16strct.h"

/* Standard Long-Term Postfilter */

void postfilter(
                Float *s,     /* input : quantized speech signal         */
                int pp,       /* input : pitch period                    */
                Float *ma_a,
                Float *b_prv,
                int *pp_prv,
                Float *e)     /* output: enhanced speech signal          */
{
   int n;
   Float gain, w1, w2;
   int ppt, pptmin, pptmax, ppnew;
   Float Rx0x1Sq, Rx0Rx1, Rx0x1Sqmax, Rx0Rx1max, Rx0x1max, bi0, bi1c, bi1p;
   Float Rx0x1;
   Float Rx0;
   Float Rx1;
   Float Rxf;
   Float a;
   Float b[2];
   
   /********************************************************************/
   /*                 pitch search around decoded pitch                */
   /********************************************************************/
   pptmin = pp - DPPQNS;
   pptmax = pp + DPPQNS;
   if(pptmin<MINPP){
      pptmin = MINPP;
      pptmax = pptmin + 2 * DPPQNS;
   }
   else if(pptmax>MAXPP){
      pptmax = MAXPP;
      pptmin = pptmax - 2 * DPPQNS;
   }
   Rx0   = 0.0;
   Rx1   = 0.0;
   Rx0x1 = 0.0;
   for(n=0; n<FRSZ; n++){
      Rx0   += s[XQOFF+n] * s[XQOFF+n];
      Rx1   += s[XQOFF+n-pptmin] * s[XQOFF+n-pptmin];
      Rx0x1 += s[XQOFF+n] * s[XQOFF+n-pptmin];
   }
   ppnew = pptmin;
   Rx0Rx1max  = Rx0 * Rx1;
   Rx0x1max   = Rx0x1;
   Rx0x1Sqmax = Rx0x1 * Rx0x1;
   for(ppt=pptmin+1; ppt<=pptmax; ppt++){
      Rx1 -= s[XQOFF+FRSZ-ppt] * s[XQOFF+FRSZ-ppt];
      Rx1 += s[XQOFF-ppt] * s[XQOFF-ppt];
      Rx0Rx1 = Rx0 * Rx1;
      Rx0x1 = 0.0;
      for(n=0; n<FRSZ; n++){
         Rx0x1 += s[XQOFF+n] * s[XQOFF+n-ppt];
      }
      Rx0x1Sq = Rx0x1 * Rx0x1;
      if(Rx0x1Sq * Rx0Rx1max > Rx0x1Sqmax * Rx0Rx1){
         ppnew = ppt;
         Rx0x1Sqmax = Rx0x1Sq;
         Rx0x1max   = Rx0x1;
         Rx0Rx1max  = Rx0Rx1;
      }
   }
   
   
   /******************************************************************/
   /*               calculate all-zero pitch postfilter              */
   /******************************************************************/
   if(Rx0Rx1max == 0.0 || Rx0x1max <= 0.0){
      a = 0.0;
   }
   else{
      a = Rx0x1max / sqrt(Rx0Rx1max);
   }
   *ma_a = 0.75*(*ma_a) + 0.25*a;
   if(*ma_a < ATHLD1 && a < ATHLD2)
      a = 0.0;
   b[1] = ScLTPF * a;
   
   
   /******************************************************************/
   /*             calculate normalization energies                   */
   /******************************************************************/
   Rxf = 0.0;
   for(n=0; n<FRSZ; n++){
      e[n] = s[XQOFF+n] + b[1] * s[XQOFF+n-ppnew];
      Rxf += e[n] * e[n];
   }
   if(Rx0 == 0.0 || Rxf == 0.0)
      gain = 1.0;
   else
      gain = sqrt(Rx0 / Rxf);
   
   
   /******************************************************************/
   /*    interpolate from the previous postfilter to the current     */
   /******************************************************************/
   b[0] = gain;
   b[1] = gain * b[1];
   for(n=0; n<NINT; n++){
      w1 = ((Float)(n+1))/((Float)(NINT+1));
      w2 = 1.0 - w1;
      
      /* interpolate between two filters */
      bi0  = w1 * b[0] + w2 * b_prv[0];
      bi1c = w1 * b[1];
      bi1p = w2 * b_prv[1];
      e[n] = bi1c * s[XQOFF+n-ppnew] + bi1p * s[XQOFF+n-(*pp_prv)] + bi0 * s[XQOFF+n];
      
   }
   for(n=NINT; n<FRSZ; n++){
      e[n] = gain * e[n];
   }
   
   /******************************************************************/
   /*                       save state memory                        */
   /******************************************************************/
   b_prv[0] = b[0];
   b_prv[1] = b[1];
   *pp_prv = ppnew;
   
   return;
}
