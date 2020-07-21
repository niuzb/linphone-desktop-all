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
  lspquan.c : Lsp quantization based on inter-frame moving-average
               prediction and two-stage VQ.

  $Log$
******************************************************************************/

#include <stdio.h>
#include "typedef.h"
#include "bv16cnst.h"
#include "bvcommon.h"
#include "bv16externs.h"

void vqmse(
           Float *xq, 
           short *idx, 
           Float *x, 
           Float *cb, 
           int vdim, 
           int cbsz);

void svqwmse(
             Float *xq, 
             short *idx, 
             Float *x, 
             Float *xa, 
             Float *w, 
             Float *cb, 
             int vdim, 
             int cbsz);

void lspquan(
             Float   *lspq,  
             short   *lspidx,  
             Float   *lsp,     
             Float   *lsppm
             )
{
   Float d[LPCO], w[LPCO];
   Float elsp[LPCO], lspe[LPCO], lspa[LPCO]; 
   Float lspeq1[LPCO], lspeq2[LPCO];
   Float a0, *fp1, *fp2;
   int i, k;
   
   /* calculate the weights for weighted mean-square error distortion */
   for (i = 0; i < LPCO - 1 ; i++) {
      d[i] = lsp[i+1] - lsp[i];       /* LSP difference vector */
   }
   w[0] = 1.0F / d[0];
   for (i = 1; i < LPCO - 1 ; i++) {
      if (d[i] < d[i-1]) 
         w[i] = 1.0F / d[i];
      else
         w[i] = 1.0F / d[i-1];
   }
   w[LPCO-1] = 1.0F / d[LPCO-2];        
   
   /* calculate estimated (ma-predicted) lsp vector */
   fp1 = lspp;
   fp2 = lsppm;
   for (i = 0; i < LPCO; i++) {
      a0 = 0.0F;
      for (k = 0; k < LSPPORDER; k++) {
         a0 += *fp1++ * *fp2++;
      }
      elsp[i] = a0;
   }
   
   /* subtract lsp mean value & estimated lsp to get prediction error */
   for (i = 0; i < LPCO; i++) {
      lspe[i] = lsp[i] - lspmean[i] - elsp[i];
   }
   
   /* perform first-stage mse vq codebook search */
   vqmse(lspeq1,&lspidx[0],lspe,lspecb1,LPCO,LSPECBSZ1);
   
   /* calculate quantization error vector of first-stage vq */
   for (i = 0; i < LPCO; i++) {
      d[i] = lspe[i] - lspeq1[i];
   }
   
   /* perform second-stage vq codebook search, signed codebook with wmse */
   for (i = 0; i < LPCO; i++) 
      lspa[i] = lspmean[i] + elsp[i] + lspeq1[i];
   svqwmse(lspeq2,&lspidx[1],d,lspa, w,lspecb2,LPCO,LSPECBSZ2);
   
   /* get overall quantizer output vector of the two-stage vq */
   for (i = 0; i < LPCO; i++) {
      lspe[i] = lspeq1[i] + lspeq2[i];
   }
   
   /* update lsp ma predictor memory */
   i = LPCO * LSPPORDER - 1;
   fp1 = &lsppm[i];
   fp2 = &lsppm[i - 1];
   for (i = LPCO - 1; i >= 0; i--) {
      for (k = LSPPORDER; k > 1; k--) {
         *fp1-- = *fp2--;
      }
      *fp1-- = lspe[i];
      fp2--;
   }
   
   /* calculate quantized lsp */
   for (i = 0; i < LPCO; i++) {
      lspq[i] = lspa[i] + lspeq2[i];
   }
   
   /* ensure correct ordering of lsp to guarantee lpc filter stability */
   stblz_lsp(lspq, LPCO);
   
}

/*==========================================================================*/

void vqmse(
           Float   *xq,    /* VQ output vector (quantized version of input vector) */
           short   *idx,   /* VQ codebook index for the nearest neighbor */
           Float   *x,     /* input vector */
           Float   *cb,    /* VQ codebook */
           int     vdim,   /* vector dimension */
           int     cbsz    /* codebook size (number of codevectors) */
           )
{
   Float *fp1, dmin, d;
   int j, k;
   
   Float e;
   
   fp1 = cb;
   dmin = 1e30;
   for (j = 0; j < cbsz; j++) {
      d = 0.0F;
      for (k = 0; k < vdim; k++) {
         e = x[k] - (*fp1++);
         d += e * e;
      }
      if (d < dmin) {
         dmin = d;
         *idx = j;
      }
   }
   
   j = *idx * vdim;
   for (k = 0; k < vdim; k++) {
      xq[k] = cb[j + k];
   }
}  


/* Signed WMSE VQ */
void svqwmse(
             Float   *xq,    /* VQ output vector (quantized version of input vector) */
             short   *idx,   /* VQ codebook index for the nearest neighbor */
             Float   *x,     /* input vector */
             Float   *xa,    /* approximation prior to current stage */
             Float   *w,     /* weights for weighted Mean-Square Error */
             Float   *cb,    /* VQ codebook */
             int     vdim,   /* vector dimension */
             int     cbsz    /* codebook size (number of codevectors) */
             )
{
   Float *fp1, *fp2, dmin, d;
   Float xqc[STBLDIM];
   int j, k, stbl, sign=1;
   Float e;
   
   fp1  = cb;
   dmin = 1e30;
   *idx = -1;
   
   for (j = 0; j < cbsz; j++) {
      
      /* Try negative sign */
      d = 0.0;
      fp2 = fp1;
      
      for(k=0; k<vdim; k++){
         e = x[k] + *fp1++;
         d += w[k]*e*e;
      }
      
      /* check candidate - negative sign */
      if (d < dmin) {
         
         for(k=0; k<STBLDIM; k++)
            xqc[k]  = xa[k] - *fp2++;
         
         /* check stability - negative sign */
         stbl = stblchck(xqc, STBLDIM);
         
         if(stbl > 0){
            dmin = d;
            *idx = j;
            sign = -1;
         }
      }
      
      /* Try positive sign */
      fp1 -= vdim;
      d = 0.0;
      fp2 = fp1;
      
      for(k=0; k<vdim; k++){
         e = x[k] - *fp1++;
         d += w[k]*e*e;
      }
      
      /* check candidate - positive sign */
      if (d < dmin) {
         
         for(k=0; k<STBLDIM; k++)
            xqc[k]  = xa[k] + *fp2++;
         
         /* check stability - positive sign */
         stbl = stblchck(xqc, STBLDIM);
         
         if(stbl > 0){
            dmin = d;
            *idx = j;
            sign = +1;
         }
      }
   }
   
   if(*idx == -1){
      printf("\nWARNING: Encoder-decoder synchronization lost for clean channel!!!\n");
      *idx = 0;
      sign = 1;
   }
   
   fp1 = cb + (*idx)*vdim;
   for (k = 0; k < vdim; k++) {
      xq[k] = (double)sign*(*fp1++);
   }
   if(sign < 0)
      *idx = (2*cbsz-1)-(*idx);
   
   return;
}  
