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
  ptdec.c : Common Floating-Point Library: 

  $Log$
******************************************************************************/

#include "typedef.h"
#include "bvcommon.h"

void pp3dec(
            short 	idx,
            Float	*b)
{
   
   Float	*fp;
   int	i;
   
   fp = pp9cb + idx*9;
   for (i=0;i<3;i++) b[i] = fp[i]*0.5;
   
}
