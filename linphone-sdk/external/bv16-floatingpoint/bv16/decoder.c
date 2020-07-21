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
  decoder.c : BV16 Floating-Point Decoder Main Subroutines

  $Log$
******************************************************************************/

#include <math.h>
#include "typedef.h"
#include "bv16cnst.h"
#include "bvcommon.h"
#include "bv16strct.h"
#include "bv16externs.h"

#include "utility.h"
#include "postfilt.h"

void Reset_BV16_Decoder(struct BV16_Decoder_State *c)
{
   int i;
   
   for (i=0;i<LPCO;i++) 
      c->lsplast[i] = (Float)(i+1)/(Float)(LPCO+1);
   Fzero(c->stsym,LPCO);
   Fzero(c->ltsym,LTMOFF);
   Fzero(c->xq,XQOFF);
   Fzero(c->lgpm,LGPORDER);
   Fzero(c->lsppm,LPCO*LSPPORDER);
   Fzero(c->prevlg,2);
   c->pp_last = 50;
   c->cfecount = 0;
   c->idum=0;
   c->per=0;
   c->E = 0.0;
   for(i=0; i<LPCO; i++)
      c->atplc[i+1] = 0.0;
   c->ngfae = LGPORDER+1;
   c->lmax = -100.;
   c->lmin = 100.;
   c->lmean = 12.5;
   c->x1 = 17.;
   c->level = 17.;
   c->nggalgc = Nfdm+1;
   c->estl_alpha_min = estl_alpha;
   c->ma_a = 0.0;
   c->b_prv[0] = 1.0;
   c->b_prv[1] = 0.0;
   c->pp_prv = 100;
}

void BV16_Decode(
                 struct BV16_Bit_Stream     *bs,
                 struct BV16_Decoder_State  *ds,
                 short   *out)
{
   
   Float xq[LXQ];		/* quantized 8 kHz low-band signal */   
   Float ltsym[LTMOFF+FRSZ];
   Float a[LPCO+1];
   Float lspq[LPCO];
   Float bq[3];
   Float gainq;
   Float lgq;
   Float lg_el;
   Float xpf[FRSZ];
   short pp;
   Float bss;
   Float E;
   
   /* set frame erasure flags */
   if (ds->cfecount != 0) {
      ds->ngfae = 1;
   } else {
      ds->ngfae++;
      if (ds->ngfae>LGPORDER) ds->ngfae=LGPORDER+1;
   }
   
   /* reset frame erasure counter */
   ds->cfecount = 0;
   
   /* decode pitch period */
   pp = (bs->ppidx + MINPP);
   
   /* decode spectral information */
   lspdec(lspq,bs->lspidx,ds->lsppm,ds->lsplast);
   lsp2a(lspq, a);
   Fcopy(ds->lsplast,lspq,LPCO);
   
   /* decode pitch taps */
   pp3dec(bs->bqidx, bq);
   
   /* decode gain */
   gainq = gaindec(&lgq,bs->gidx,ds->lgpm,ds->prevlg,
      ds->level,&ds->nggalgc, &lg_el);
   
   /* copy state memory to buffer */
   Fcopy(ltsym, ds->ltsym, LTMOFF);
   Fcopy(xq, ds->xq, XQOFF);
   
   /* Decode the excitation signal including long-term synthesis and codevector scaling */
   excdec_w_LT_synth(ltsym,bs->qvidx,gainq,bq,pp,cccb,&E);
   
   ds->E = E;
   
   /* lpc synthesis filtering of short-term excitation */
   apfilter(a,LPCO,ltsym+LTMOFF,xq+XQOFF,FRSZ,ds->stsym,1);
   
   /* update the remaining state memory */
   ds->pp_last = pp;
   Fcopy(ds->xq, xq+FRSZ, XQOFF);
   Fcopy(ds->ltsym, ltsym+FRSZ, LTMOFF);
   Fcopy(ds->bq_last, bq, 3);
   
   /* level estimation */
   estlevel(lg_el,&ds->level,&ds->lmax,&ds->lmin,&ds->lmean,&ds->x1,ds->ngfae,
      ds->nggalgc,&ds->estl_alpha_min);
   
   /* adaptive postfiltering */
   postfilter(xq, pp, &(ds->ma_a), ds->b_prv, &(ds->pp_prv), xpf);
   F2s(out, xpf, FRSZ);
   
   Fcopy(ds->atplc, a , LPCO+1);

   bss = bq[0]+bq[1]+bq[2];
   if(bss > 1.0)
      bss = 1.0;
   else if(bss < 0.0)
      bss = 0.0;
   ds->per = 0.5*ds->per+0.5*bss;
}
