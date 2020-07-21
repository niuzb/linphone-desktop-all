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
  gaindec.c : gain decoding functions

  $Log$
******************************************************************************/

#include <math.h>
#include "typedef.h"
#include "bv16cnst.h"
#include "bvcommon.h"
#include "bv16externs.h"

Float gaindec(
              Float   *lgq,
              short   gidx,	
              Float   *lgpm,
              Float   *prevlg,		/* previous log gains (last two frames) */
              Float   level,
              short   *nggalgc,
              Float   *lg_el
              )
{
   Float gainq, elg, lgc, lgq_nh;
   int i, n, k;
   
   /* calculate estimated log-gain */
   elg = 0;;
   for (i = 0; i < LGPORDER; i++) {
      elg += lgp[i] * lgpm[i];
   }
   
   elg += lgmean;
   
   /* calculate decoded log-gain */
   *lgq = lgpecb[gidx] + elg;
   
   /* next higher gain */
   if(gidx < LGPECBSZ-1){
      lgq_nh = lgpecb_nh[gidx] + elg;
      if(*lgq < 0.0 && fabs(lgq_nh) < fabs(*lgq)){ 
         /* To avoid thresholding when the enc Q makes it below the threshold */
         *lgq = 0.0;
      }
   }
   
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
   
   /* update log-gain predictor memory, 
   check whether decoded log-gain exceeds lgclimit */
   for (k = LGPORDER - 1; k > 0; k--) {
      lgpm[k] = lgpm[k-1];
   }
   
   lgc = *lgq - prevlg[0];
   if ((lgc>lgclimit[i]) && (gidx>0)) { /* if decoded log-gain exceeds limit */
      *lgq = prevlg[0];	 /* use the log-gain of previous frame */
      lgpm[0] = *lgq - elg;
      *nggalgc = 0;
      *lg_el = lgclimit[i]+prevlg[0];
   } else {
      lgpm[0] = lgpecb[gidx];
      *nggalgc = *nggalgc+1;
      if(*nggalgc > Nfdm) *nggalgc = Nfdm+1;
      *lg_el = *lgq;
   }
   
   /* update log-gain predictor memory */
   prevlg[1] = prevlg[0];
   prevlg[0] = *lgq;
   
   /* convert quantized log-gain to linear domain */
   gainq = pow(2.0F, 0.5F * *lgq);
   
   return gainq;
}
              
Float gaindec_fe(
                 Float	lgq_last,
                 Float   *lgpm)
{
   Float  elg;
   int i;
   
   /* calculate estimated log-gain */
   elg = 0.0F;
   for (i = 0; i < LGPORDER; i++) {
      elg += lgp[i] * lgpm[i];
   }
   
   /* update log-gain predictor memory */
   for (i = LGPORDER - 1; i > 0; i--) {
      lgpm[i] = lgpm[i-1];
   }
   lgpm[0] = lgq_last-lgmean-elg;
   
   return lgq_last;
}

void gainplc(Float E,
             Float *lgeqm,
             Float *lgqm)
{
   int k;
   Float pe, lg, mrlg, elg, lge;  
   
   pe = INVFRSZ * E;
   
   if(pe - TMinlg > 0.0)
      lg = log(pe)/log(2.0);
   else
      lg = Minlg;
   
   mrlg = lg - lgmean;
   
   elg = 0.0;
   for(k=0; k<GPO; k++)
      elg += lgp[k] * lgeqm[k];
   
   /* predicted log-gain error */
   lge = mrlg - elg;
   
   /* update quantizer memory */
   for(k=GPO-1; k>0; k--)
      lgeqm[k] = lgeqm[k-1];
   lgeqm[0] = lge;
   
   /* update quantized log-gain memory */
   lgqm[1] = lgqm[0];
   lgqm[0] = lg;
   
   
   return;
}
