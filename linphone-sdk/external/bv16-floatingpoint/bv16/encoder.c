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
  encoder.c : BV16 Floating-Point Encoder Main Subroutines

  $Log$
******************************************************************************/

#include <math.h>
#include "typedef.h"
#include "bv16cnst.h"
#include "utility.h"
#include "bvcommon.h"
#include "bv16externs.h"
#include "bv16strct.h"

void Reset_BV16_Encoder(struct BV16_Encoder_State *c)
{
   int k;
   
   Fzero(c->lgpm,LGPORDER);
   c->old_A[0] = 1.0;
   Fzero(c->old_A+1, LPCO);  
   for(k=0; k<LPCO; k++)
      c->lsplast[k] = (Float)(k+1)/(Float)(LPCO+1);
   Fzero(c->lsppm,LPCO*LSPPORDER);
   Fzero(c->x,XOFF);
   Fzero(c->xwd, XDOFF);
   Fzero(c->dq, XOFF);
   Fzero(c->stpem, LPCO);
   Fzero(c->stwpm, LPCO);
   Fzero(c->dfm, DFO);
   Fzero(c->stsym,LPCO);
   Fzero(c->stnfz,NSTORDER);
   Fzero(c->stnfp,NSTORDER);
   Fzero(c->ltsym,MAXPP1+FRSZ);
   Fzero(c->ltnfm,MAXPP1+FRSZ); 
   Fzero(c->hpfzm,HPO);
   Fzero(c->hpfpm,HPO);
   Fzero(c->prevlg, 2);
   c->cpplast = 12*cpp_scale;
   c->lmax = -100.;
   c->lmin = 100.;
   c->lmean = 12.5;
   c->x1 = 17.;
   c->level = 17.;
}

void BV16_Encode(
                 struct BV16_Bit_Stream *bs,
                 struct BV16_Encoder_State *cs,
                 short  *inx)
{
   
   Float x[LX];			/* signal buffer */
   Float dq[LX];		/* quantized short term pred error, low-band */
   Float xw[FRSZ];		/* perceptually weighted low-band signal */   
   Float r[NSTORDER+1];
   Float a[LPCO+1];
   Float aw[LPCO+1];
   Float fsz[1+NSTORDER];
   Float fsp[1+NSTORDER];
   Float lsp[LPCO];  
   Float lspq[LPCO];
   Float cbs[VDIM*CBSZ];
   Float bq[3], beta;
   Float gainq, lg, ppt;
   Float lth;
   Float dummy;
   int	pp, cpp;
   int	i;
   
   /* copy state memory to local memory buffers */
   Fcopy(x, cs->x, XOFF);
   for (i=0;i<FRSZ;i++) x[XOFF+i] = (Float) inx[i];
   
   /* 150 Hz HighPass filtering */
   azfilter(hpfb,HPO,x+XOFF,x+XOFF,FRSZ,cs->hpfzm,1);
   apfilter(hpfa,HPO,x+XOFF,x+XOFF,FRSZ,cs->hpfpm,1);
   
   /* update highpass filtered signal buffer */
   Fcopy(cs->x,x+FRSZ,XOFF);
   
   /* perform lpc analysis with asymmetrical window */   
   Autocor(r,x+LX-WINSZ,winl,WINSZ,NSTORDER); /* get autocorrelation lags */
   for (i=0;i<=NSTORDER;i++) r[i]*=sstwin[i]; /* apply spectral smoothing */
   Levinson(r,a,cs->old_A,LPCO); 			/* Levinson-Durbin recursion */
   
   /* pole-zero noise feedback filter */
   for (i=0;i<=NSTORDER;i++){
      fsz[i] = a[i] * gfsz[i];
      fsp[i] = a[i] * gfsp[i];
   }
   
   /* bandwidth expansion */
   for (i=0;i<=LPCO;i++) a[i] *= bwel[i];	
   
   /* LPC -> LSP Conversion */
   a2lsp(a,lsp,cs->lsplast);
   
   /* Spectral Quantization */
   lspquan(lspq,bs->lspidx,lsp,cs->lsppm); 
   
   lsp2a(lspq,a);
   
   /* calculate lpc prediction residual */
   Fcopy(dq,cs->dq,XOFF); 			/* copy dq() state to buffer */
   azfilter(a,LPCO,x+XOFF,dq+XOFF,FRSZ,cs->stpem,1);
   
   /* weighted version of lpc filter to generate weighted speech */
   for (i=0;i<=LPCO; i++) aw[i] = STWAL[i]*a[i];
   
   /* get perceptually weighted speech signal */
   apfilter(aw,LPCO,dq+XOFF,xw,FRSZ,cs->stwpm,1);
   
   /* get the coarse version of pitch period using 4:1 decimation */
   cpp = coarsepitch(xw, cs->xwd, cs->dfm, cs->cpplast);
   cs->cpplast=cpp;
   
   /* refine the pitch period in the neighborhood of coarse pitch period
   also calculate the pitch predictor tap for single-tap predictor */
   pp = refinepitch(dq, cpp, &ppt);
   bs->ppidx = (pp - MINPP);	
   
   /* vector quantize 3 pitch predictor taps with minimum residual energy */
   bs->bqidx= pitchtapquan(dq, pp, bq, &lg);
   
   /* get coefficients of long-term noise feedback filter */
   if (ppt > 1.) beta = LTWFL;
   else if (ppt < 0.) beta = 0.;
   else beta = LTWFL*ppt;
   
   /* gain quantization */
   lg = (lg < FRSZ) ? 0 : log(lg/FRSZ)/log(2.0);
   bs->gidx = gainquan(&gainq,lg,cs->lgpm,cs->prevlg,cs->level); 
   
   /* level estimation */
   dummy = estl_alpha;
   lth = estlevel(cs->prevlg[0],&cs->level,&cs->lmax,&cs->lmin,
      &cs->lmean,&cs->x1,LGPORDER+1,Nfdm+1,&dummy);
   
   /* scale the scalar quantizer codebook */
   for (i=0;i<(VDIM*CBSZ);i++) cbs[i] = gainq*cccb[i];
   
   /* perform noise feedback coding of the excitation signal */
   excquan(bs->qvidx,x+XOFF,a,fsz,fsp,bq,beta,cs->stsym,
      cs->ltsym,cs->ltnfm,cs->stnfz,cs->stnfp,cbs,pp);
   
   /* update state memory */
   Fcopy(dq+XOFF, cs->ltsym+MAXPP1, FRSZ);
   Fcopy(cs->dq, dq+FRSZ, XOFF);
   
}
