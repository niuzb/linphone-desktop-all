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
  g192.c : Implementation of optional G.192 bit-stream format

  $Log$
******************************************************************************/

#include <stdio.h>
#include "typedef.h"
#include "bvcommon.h"
#include "bv16cnst.h"
#include "bv16strct.h"

#define NBIT		80					/* number of bits per frame */
#define BIT_0     (short)0x007f
#define BIT_1     (short)0x0081
#define SYNC_WORD (short)0x6b21

extern short bfi;

short	bit_table[] = { 
   7, 7,							/* LSP */
   7,								/* Pitch Lag */
   5,								/* Pitch Gain */
   4,								/* Excitation Vector Log-Gain */
   5, 5, 5, 5, 5, 5, 5, 5, 5, 5	/* Excitation Vector */
};

#define	Nindices	15				/* number of Q indices per frame */

short bin2int(short no_of_bits, short *bitstream)
{
   short   value, i;
   short bit;
   
   value = 0;
   for (i = 0; i < no_of_bits; i++)
   {
      value <<= 1;
      bit = *bitstream++;
      if (bit == BIT_1)  value += 1;
   }
   return(value);
}

void int2bin(short value, short no_of_bits, short *bitstream)
{
   short *pt_bitstream;
   short i, bit;
   
   pt_bitstream = bitstream + no_of_bits;
   
   for (i = 0; i < no_of_bits; i++){
      bit = value & (short)0x0001;
      if (bit == 0)
         *--pt_bitstream = BIT_0;
      else
         *--pt_bitstream = BIT_1;
      value >>= 1;
   }
}

void bv16_fwrite_g192bitstrm(struct BV16_Bit_Stream *bs, FILE *fo)
{
   short n, m;
   short bitstream[NBIT+2], *p_bitstream, *pbs;
   
   bitstream[0] = SYNC_WORD;
   bitstream[1] = NBIT;
   p_bitstream = bitstream + 2;
   
   pbs = (short *) bs;
   
   for (n=0;n<Nindices;n++) {
      m = bit_table[n];
      int2bin(*pbs++, m, p_bitstream);
      p_bitstream += m;
   };
   
   fwrite(bitstream, sizeof(short), NBIT+2, fo);
   
   return;
}

/* function to read bit-stream in G.192 compliant format */
short bv16_fread_g192bitstrm(struct BV16_Bit_Stream *bs, FILE *fi)
{
   short sync_word, n, m, nread;
   short bitstream[NBIT+1], *p_bitstream;
   short *pbs;
   
   nread=fread(&sync_word, sizeof(short), 1, fi);
   if(sync_word == SYNC_WORD)
      bfi = 0;
   else
      bfi = 1;
   
   fread(bitstream, sizeof(short), NBIT+1, fi);
   p_bitstream = bitstream + 1;
   
   pbs = (short *) bs;
   
   /* LSP indices */
   for (n=0;n<Nindices;n++) {
      m = bit_table[n]; 
      *pbs++ = bin2int(m, p_bitstream);
      p_bitstream += m;
   }
   
   return nread;
}

