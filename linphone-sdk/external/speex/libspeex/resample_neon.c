/* Copyright (C) 2011 Belledonne Communications SARL
 */
/**
   @file resample_neon.h
   @brief Resampler functions (Neon version)
*/
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   
   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   - Neither the name of the Xiph.org Foundation nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.
   
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef OUTSIDE_SPEEX
#include "speex_resampler.h"
#else /* OUTSIDE_SPEEX */
#include "../include/speex/speex.h"             
#include "../include/speex/speex_resampler.h"
#endif
#include "arch.h"


#ifdef __ARM_NEON__
#include <arm_neon.h>
#include "resample_neon.h"


/* intrinsics */
static inline spx_int32_t inner_product_single_neon_intrinsics(const spx_int16_t *a, const spx_int16_t *b, unsigned int len){
    int16x8_t a8;
    int16x8_t b8;
    spx_int32_t sum = 0; // sum is 0
    int32x4_t partial = vdupq_n_s32(0);
    int64x2_t back;

    len >>=3;

    while (len--) {
        a8 = vld1q_s16(a); // load 8 16b from a
        b8 = vld1q_s16(b); // load 8 16b from b

        partial = vmlal_s16(partial, vget_low_s16(a8), vget_low_s16(b8)); // part[n] += a[n] * b[n] vector multiply and add
        partial = vmlal_s16(partial, vget_high_s16(a8), vget_high_s16(b8)); // part[n] += a[n] * b[n] vector multiply and add
        a+=8; b+=8;
    }
    back = vpaddlq_s32(partial); // sum the 4 s32 in 2 64b

    return vgetq_lane_s64(back, 0) + vgetq_lane_s64(back, 1);
}

/* intrinsics */
spx_int32_t inner_product_neon_intrinsics(const spx_int16_t *a, const spx_int16_t *b, unsigned int len){
    int16x8_t a8;
    int16x8_t b8;
    spx_int32_t sum = 0; // sum is 0
    int32x4_t partial = vdupq_n_s32(0);
    int64x2_t back;

    len >>=3;

    while (len--) {
        a8 = vld1q_s16(a); // load 8 16b from a
        b8 = vld1q_s16(b); // load 8 16b from b

        partial = vmlal_s16(partial, vget_low_s16(a8), vget_low_s16(b8)); // part[n] += a[n] * b[n] vector multiply and add
        partial = vmlal_s16(partial, vget_high_s16(a8), vget_high_s16(b8)); // part[n] += a[n] * b[n] vector multiply and add
        a+=8; b+=8;
    }
    back = vpaddlq_s32(partial); // sum the 4 s32 in 2 64b
    back = vshrq_n_s64(back, 6); // shift by 6

    return vgetq_lane_s64(back, 0) + vgetq_lane_s64(back, 1);
}

spx_int32_t inner_product_single_neon(const spx_int16_t *a, const spx_int16_t *b, unsigned int len){
	return inner_product_single_neon_intrinsics(a,b,len);
}

spx_int32_t interpolate_product_single_neon(const spx_int16_t *a, const spx_int16_t *b, unsigned int len, const spx_uint32_t oversample, spx_int16_t *frac){
	int i,j;
	int32x4_t sum = vdupq_n_s32 (0);
	int16x4_t f=vld1_s16 ((const int16_t*)frac);
	int32x4_t f2=vmovl_s16(f);
	int32x2_t tmp;
	
	f2=vshlq_n_s32(f2,16);

	for(i=0,j=0;i<len;i+=2,j+=(2*oversample)) {
		sum=vqdmlal_s16(sum,vld1_dup_s16 ((const int16_t*)(a+i)), vld1_s16 ((const int16_t*)(b+j)));
		sum=vqdmlal_s16(sum,vld1_dup_s16 ((const int16_t*)(a+i+1)), vld1_s16 ((const int16_t*)(b+j+oversample)));
	}
	sum=vshrq_n_s32(sum,1);
	sum=vqdmulhq_s32(f2,sum);
	sum=vshrq_n_s32(sum,1);
	
	tmp=vadd_s32(vget_low_s32(sum),vget_high_s32(sum));
	tmp=vpadd_s32(tmp,tmp);
	
	return vget_lane_s32 (tmp,0);
}

EXPORT spx_int32_t inner_prod(const spx_int16_t *x, const spx_int16_t *y, int len){
	spx_int32_t ret;

	if (!(libspeex_cpu_features & SPEEX_LIB_CPU_FEATURE_NEON)) {
		spx_word32_t sum=0;
		len >>= 2;
		while(len--) {
			spx_word32_t part=0;
			part = MAC16_16(part,*x++,*y++);
			part = MAC16_16(part,*x++,*y++);
			part = MAC16_16(part,*x++,*y++);
			part = MAC16_16(part,*x++,*y++);
			/* HINT: If you had a 40-bit accumulator, you could shift only at the end */
			sum = ADD32(sum,SHR32(part,6));
		}
		return sum;
	} else {
		return inner_product_neon_intrinsics(x, y, len);
	}
}
#endif /* ARM NEON */


