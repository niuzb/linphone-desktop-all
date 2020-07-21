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
  excquan.c : Vector Quantizer for 2-Stage Noise Feedback Coding 
            with long-term predictive noise feedback coding embedded 
            inside the short-term predictive noise feedback coding loop.

  Note that the Noise Feedback Coding of the excitation signal is implemented
  using the Zero-State Responsse and Zero-input Response decomposition as 
  described in: J.-H. Chen, "Novel Codec Structures for Noise Feedback 
  Coding of Speech," Proc. ICASSP, 2006.  The principle of the Noise Feedback
  Coding of the excitation signal is described in: "BV16 Speech Codec 
  Specification for Voice over IP Applications in Cable Telephony," American 
  National Standard, ANSI/SCTE 24-21 2006.

  $Log$
******************************************************************************/

#include "typedef.h"
#include "bv16cnst.h"
#include "bvcommon.h"

void excquan(
             short   *idx,   /* quantizer codebook index for uq[] vector */
             Float   *s,     /* input speech signal vector */
             Float   *aq,    /* short-term predictor coefficient array */
             Float   *fsz,   /* short-term noise feedback filter - numerator */
             Float   *fsp,   /* short-term noise feedback filter - denominator */
             Float   *b,     /* coefficient of 3-tap pitch predictor */
             Float   beta,   /* coefficient of 1-tap LT noise feedback filter */
             Float   *stsym, /* short-term synthesis filter memory */
             Float   *ltsym, /* long-term synthesis filter memory */
             Float   *ltnfm, /* long-term noise feedback filter memory */
             Float   *stnfz,
             Float   *stnfp,
             Float   *cb,    /* scalar quantizer codebook */
             int     pp)     /* pitch period */
{
   Float qzir[VDIM];           /* zero-input response                             */
   Float qzsr[VDIM*CBSZ];   /* negated zero-state response of codebook         */
   Float uq[VDIM];             /* selected codebook vector (incl. sign)           */ 
   Float buf1[LPCO+FRSZ]; /* buffer for filter memory & signal               */
   Float buf2[NSTORDER+FRSZ]; /* buffer for filter memory                        */
   Float buf3[NSTORDER+FRSZ]; /* buffer for filter memory                        */
   Float buf4[VDIM];           /* buffer for filter memory                        */
   Float a0, a1, a2, *fp1, *fp2, *fp3, *fp4, sign;
   Float *fpa, *fpb;
   Float ltfv[VDIM], ppv[VDIM];
   int i, j, m, n, jmin, iv;
   Float buf5[VDIM];           /* buffer for filter memory                        */
   Float buf6[VDIM];           /* buffer for filter memory                        */
   Float e, E, Emin;
   Float *p_ppv, *p_ltfv, *p_uq, v;
   
   /* copy filter memory to beginning part of temporary buffer */
   fp1 = &stsym[LPCO-1];
   for (i = 0; i < LPCO; i++) {
      buf1[i] = *fp1--;    /* this buffer is used to avoid memory shifts */
   }
   
   /* copy noise feedback filter memory */
   fp1 = &stnfz[NSTORDER-1];
   fp2 = &stnfp[NSTORDER-1];
   for (i = 0; i < NSTORDER; i++) {
      buf2[i] = *fp1--;
      buf3[i] = *fp2--;
   }
   
   /************************************************************************************/
   /*                       Z e r o - S t a t e   R e s p o n s e                      */
   /************************************************************************************/
   /* Calculate negated Zero State Response */
   fp2 = cb;   /* fp2 points to start of first codevector */
   fp3 = qzsr; /* fp3 points to start of first zero-state response vector */
   
   /* For each codevector */
   for(j=0; j<CBSZ; j++) {
      
      /* Calculate the elements of the negated ZSR */
      for(i=0; i<VDIM; i++){
         /* Short-term prediction */
         a0 = 0.0;
         fp1 = buf4;
         for (n = i; n > 0; n--)
            a0 -= *fp1++ * aq[n];
         
         /* Update memory of short-term prediction filter */
         *fp1++ = a0 + *fp2;
         
         /* noise feedback filter */
         a1 = 0.0;
         fpa = buf5;
         fpb = buf6;
         for (n = i; n > 0; n--)
            a1 += ((*fpa++ * fsz[n]) - (*fpb++ * fsp[n]));
         
         /* Update memory of pole section of noise feedback filter */
         *fpb++ = a1;
         
         /* ZSR */
         *fp3 = *fp2++ + a0 + a1;
         
         /* Update memory of zero section of noise feedback filter */
         *fpa++ = -(*fp3++);
      }
   }
   
   /* loop through every vector of the current subframe */
   iv = 0;     /* iv = index of the current vector */
   for (m = 0; m < FRSZ; m += VDIM) {
      
      /********************************************************************************/
      /*                     Z e r o - I n p u t   R e s p o n s e                    */
      /********************************************************************************/
      /* compute pitch-predicted vector, which should be independent of the
      residual vq codevectors being tried if vdim < min. pitch period */
      fp2 = ltfv;
      fp3 = ppv;
      for (n = m; n < m + VDIM; n++) {
         fp1 = &ltsym[MAXPP1+n-pp+1];
         a1  = b[0] * *fp1--;
         a1 += b[1] * *fp1--;
         a1 += b[2] * *fp1--;/* a1=pitch predicted vector of LT syn filt */
         *fp3++ = a1;            /* write result to ppv[] vector */
         
         *fp2++ = a1 + beta * ltnfm[MAXPP1+n-pp];
      }
      
      /* compute zero-input response */
      fp2 = ppv;
      fp4 = ltfv;
      fp3 = qzir;
      for (n = m; n < m + VDIM; n++) {
         
         /* perform multiply-adds along the delay line of the predictor */
         fp1 = &buf1[n];
         a0 = 0.;
         for (i = LPCO; i > 0; i--) {
            a0 -= *fp1++ * aq[i];
         }
         
         /* perform multiply-adds along the noise feedback filter */
         fpa = &buf2[n];
         fpb = &buf3[n];
         a1 = 0.;
         for (i = NSTORDER; i > 0; i--) {
            a1 += ((*fpa++ * fsz[i]) - (*fpb++ * fsp[i]));
         }
         *fpb = a1;		/* update output of the noise feedback filter */
         
         a2 = s[n] - (a0 + a1);		/* v[n] */
         
                                    /* a2 now contains v[n]; subtract the sum of the two long-term
         filters to get the zero-input response */
         *fp3++ = a2 - *fp4++;	/* q[n] = u[n] during ZIR computation */
         
         /* update short-term noise feedback filter memory */
         a0 += *fp2;		 /* a0 now conatins the qs[n] */
         *fp1 = a0;
         a2 -= *fp2++;    /* a2 now contains qszi[n] */
         *fpa = a2; /* update short-term noise feedback filter memory */
      }
      /********************************************************************************/
      
      
      /********************************************************************************/
      /*                         S e a r c h   C o d e b o o k                        */
      /********************************************************************************/
      /* loop through every codevector of the residual vq codebook */
      /* and find the one that minimizes the energy of q[n] */
      Emin = 1e30;
      jmin = 0;
      sign = 1.0;
      fp4 = qzsr;
      for(j = 0; j < CBSZ; j++) {
         /* Try positive sign */
         fp2 = qzir;
         E   = 0.0;
         for(n = 0; n < VDIM; n++){
            e = *fp2++ - *fp4++; // sign impacted by negated ZSR
            E += e*e;
         }
         if(E < Emin) { 
            jmin = j;
            Emin = E;
            sign = +1.0F;
         }
         /* Try negative sign */
         fp4 -= VDIM;
         fp2 = qzir;
         E   = 0.0;
         for(n = 0; n < VDIM; n++){
            e = *fp2++ + *fp4++; // sign impacted by negated ZSR
            E += e*e;
         }
         if(E < Emin) { 
            jmin = j;
            Emin = E;
            sign = -1.0F;
         }
      }
      
      /* the best codevector has been found; assign vq codebook index */
      if (sign == 1.0F) 
         idx[iv++] = jmin;
      else
         idx[iv++] = jmin + CBSZ; /* MSB of index is sign bit */
      
      fp3 = &cb[jmin*VDIM]; /* fp3 points to start of best codevector */
      for (n = 0; n < VDIM; n++) {
         uq[n] = sign * *fp3++;
      }
      /********************************************************************************/
      
      
      /********************************************************************************/
      /*                    U p d a t e   F i l t e r   M e m o r y                   */
      /********************************************************************************/
      fp3    = ltsym+MAXPP1+m;
      fp4    = ltnfm+MAXPP1+m;
      p_ltfv = ltfv;
      p_ppv  = ppv;
      p_uq   = uq;
      for (n = m; n < m + VDIM; n++) {
         
         /* Update memory of long-term synthesis filter */
         *fp3 = *p_ppv++ + *p_uq;
         
         /* Short-term prediction */
         a0 = 0.0;
         fp1 = &buf1[n];
         for (i = LPCO; i > 0; i--)
            a0 -= *fp1++ * aq[i];
         
         /* Update memory of short-term synthesis filter */
         *fp1++ = a0 + *fp3;
         
         /* Short-term pole-zero noise feedback filter */
         fpa = &buf2[n];
         fpb = &buf3[n];
         a1 = 0.0;
         for (i = NSTORDER; i > 0; i--)
            a1 += ((*fpa++ * fsz[i]) - (*fpb++ * fsp[i]));
         
         /* Update memory of pole section of noise feedback filter */
         *fpb++ = a1;
         
         v = s[n] - a0 - a1;
         
         /* Update memory of zero section of noise feedback filter */
         *fpa++ = v - *fp3++;
         
         /* Update memory of long-term noise feedback filter */
         *fp4++ = v - *p_ltfv++ - *p_uq++;
      }
      /********************************************************************************/
      
    }
    
    /* update short-term predictor and noise feedback filter memories after subframe */
    for (i = 0; i < LPCO; i++) {
       stsym[i] = *--fp1;
    }
    
    for (i = 0; i < NSTORDER; i++) {
       stnfz[i] = *--fpa;
       stnfp[i] = *--fpb;
    }
    
    /* update long-term predictor and noise feedback filter memories after subframe */
    fp2 = &ltnfm[FRSZ];
    fp3 = &ltsym[FRSZ];
    for (i = 0; i < MAXPP1; i++) {
       ltnfm[i] = fp2[i];
       ltsym[i] = fp3[i];
    }
    
}
