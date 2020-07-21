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
  gainquan.c : gain quantization based on inter-subframe 
           moving-average prediction of logarithmic gain.

  $Log$
******************************************************************************/

#include <math.h>
#include "typedef.h"
#include "bv16cnst.h"
#include "bvcommon.h"
#include "bv16externs.h"

int gainquan(
             Float   *gainq,  
             Float   lg,     
             Float   *lgpm,
             Float   *prevlg,        /* previous log gains (last two frames) */
             Float	level
             )
{
   Float elg, lgpe, limit, dmin, d;
   int i, n, gidx=0, *p_gidx;
   
   /* calculate estimated log-gain */
   elg = lgmean;
   for (i = 0; i < LGPORDER; i++) {
      elg += lgp[i] * lgpm[i];
   }
   
   /* subtract log-gain mean & estimated log-gain to get prediction error */
   lgpe = lg - elg;
   
   /* scalar quantization of log-gain prediction error */
   dmin = 1e30;
   p_gidx = idxord;
   for (i = 0; i < LGPECBSZ; i++) {
      d = lgpe - lgpecb[*p_gidx++];
      if (d < 0.0F) {
         d = -d;
      }
      if (d < dmin) {
         dmin = d;
         gidx= i;
      }
   }
   
   /* calculate quantized log-gain */
   *gainq = lgpecb[idxord[gidx]] + elg;
   
   /* look up from lgclimit() table the maximum log gain change allowed */
   i = (int) ((prevlg[0] - level - LGLB) * 0.5F); /* get column index */
   if (i >= NGB) {
      i = NGB - 1;
   } else if (i < 0) {
      i = 0;
   }
   n = (int) ((prevlg[0] - prevlg[1] - LGCLB) * 0.5F);  /* get row index */
   if (n >= NGCB) {
      n = NGCB - 1;
   } else if (n < 0) {
      n = 0;
   }
   i = i * NGCB + n;
   
   /* check whether quantized log-gain cause a gain change > lgclimit */
   limit = prevlg[0] + lgclimit[i];/* limit that log-gain shouldn't exceed */
   while ((*gainq > limit) && (gidx > 0) ) { /* if q log-gain exceeds limit */
      gidx -= 1;     /* decrement gain quantizer index by 1 */
      *gainq = lgpecb[idxord[gidx]] + elg; /* use next quantizer output*/
   }
   
   /* get true codebook index */
   gidx = idxord[gidx];
   
   /* update log-gain predictor memory */
   prevlg[1] = prevlg[0];
   prevlg[0] = *gainq;
   for (i = LGPORDER - 1; i > 0; i--) {
      lgpm[i] = lgpm[i-1];
   }
   lgpm[0] = lgpecb[gidx];
   
   /* convert quantized log-gain to linear domain */
   *gainq = pow(2.0F, 0.5F * *gainq);
   
   return gidx;
}

