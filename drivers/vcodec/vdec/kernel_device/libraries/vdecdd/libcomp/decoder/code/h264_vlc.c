/*!
 *****************************************************************************
 *
 * @file       h264_vlc.c
 *
 * : H264 vlc table
 * ---------------------------------------------------------------------------
 *
 * Copyright (c) Imagination Technologies Ltd.
 * 
 * The contents of this file are subject to the MIT license as set out below.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 * 
 * Alternatively, the contents of this file may be used under the terms of the 
 * GNU General Public License Version 2 ("GPL")in which case the provisions of
 * GPL are applicable instead of those above. 
 * 
 * If you wish to allow use of your version of this file only under the terms 
 * of GPL, and not to allow others to use your version of this file under the 
 * terms of the MIT license, indicate your decision by deleting the provisions 
 * above and replace them with the notice and other provisions required by GPL 
 * as set out in the file called �GPLHEADER� included in this distribution. If 
 * you do not delete the provisions above, a recipient may use your version of 
 * this file under the terms of either the MIT license or GPL.
 * 
 * This License is also included in this distribution in the file called 
 * "MIT_COPYING".
 *
 *****************************************************************************/

/* format is: opcode, width, symbol */
/* all VLC tables are concatenated */
/* index infomation is stored in gui16H264VlcIndexData[] */

#include <img_types.h>

extern IMG_UINT16 gaui16h264VlcTableData[];
extern IMG_UINT32 gaui32h264VlcPackedTableData[];
extern const IMG_UINT16 gui16h264VlcTableSize;

IMG_UINT16 gaui16h264VlcTableData[] = {
/* NumCoeffTrailingOnes_Table9-5_nC_0-1.out */
	4, 0, 0,
	4, 1, 5,
	4, 2, 10,
	2, 1, 4,
	2, 1, 6,
	0, 1, 8,
	0, 2, 11,
	4, 0, 15,
	4, 1, 4,
	4, 1, 9,
	4, 0, 19,
	4, 1, 14,
	4, 1, 23,
	4, 1, 27,
	4, 1, 18,
	4, 1, 13,
	4, 1, 8,
	2, 5, 8,
	0, 1, 50,
	0, 0, 53,
	0, 0, 54,
	4, 2, 31,
	4, 2, 22,
	4, 2, 17,
	4, 2, 12,
	0, 2, 7,
	0, 2, 14,
	0, 2, 21,
	0, 2, 28,
	0, 1, 35,
	4, 5, 53,
	3, 5, 0,
	4, 2, 32,
	4, 2, 38,
	4, 2, 33,
	4, 2, 28,
	4, 2, 43,
	4, 2, 34,
	4, 2, 29,
	4, 2, 24,
	4, 2, 51,
	4, 2, 46,
	4, 2, 41,
	4, 2, 40,
	4, 2, 47,
	4, 2, 42,
	4, 2, 37,
	4, 2, 36,
	4, 2, 59,
	4, 2, 54,
	4, 2, 49,
	4, 2, 48,
	4, 2, 55,
	4, 2, 50,
	4, 2, 45,
	4, 2, 44,
	4, 2, 67,
	4, 2, 62,
	4, 2, 61,
	4, 2, 56,
	4, 2, 63,
	4, 2, 58,
	4, 2, 57,
	4, 2, 52,
	4, 1, 64,
	4, 1, 66,
	4, 1, 65,
	4, 1, 60,
	4, 1, 39,
	4, 1, 30,
	4, 1, 25,
	4, 1, 20,
	4, 0, 35,
	4, 0, 26,
	4, 0, 21,
	4, 0, 16,
/* NumCoeffTrailingOnes_Table9-5_nC_2-3.out */
	0, 2, 16,
	0, 1, 73,
	0, 1, 76,
	0, 0, 79,
	4, 3, 19,
	4, 3, 15,
	4, 2, 10,
	4, 2, 10,
	4, 1, 5,
	4, 1, 5,
	4, 1, 5,
	4, 1, 5,
	4, 1, 0,
	4, 1, 0,
	4, 1, 0,
	4, 1, 0,
	2, 5, 8,
	0, 1, 49,
	0, 0, 52,
	0, 0, 53,
	4, 2, 35,
	4, 2, 22,
	4, 2, 21,
	4, 2, 12,
	0, 2, 7,
	0, 2, 14,
	0, 2, 21,
	1, 1, 28,
	0, 1, 34,
	4, 5, 63,
	3, 5, 0,
	4, 2, 47,
	4, 2, 38,
	4, 2, 37,
	4, 2, 32,
	4, 2, 43,
	4, 2, 34,
	4, 2, 33,
	4, 2, 28,
	4, 2, 44,
	4, 2, 46,
	4, 2, 45,
	4, 2, 40,
	4, 2, 51,
	4, 2, 42,
	4, 2, 41,
	4, 2, 36,
	4, 2, 59,
	4, 2, 54,
	4, 2, 53,
	4, 2, 52,
	4, 2, 55,
	4, 2, 50,
	4, 2, 49,
	4, 2, 48,
	0, 1, 3,
	4, 1, 58,
	4, 1, 56,
	4, 1, 61,
	4, 1, 60,
	4, 1, 62,
	4, 1, 57,
	4, 1, 67,
	4, 1, 66,
	4, 1, 65,
	4, 1, 64,
	4, 1, 39,
	4, 1, 30,
	4, 1, 29,
	4, 1, 24,
	4, 0, 20,
	4, 0, 26,
	4, 0, 25,
	4, 0, 16,
	4, 1, 31,
	4, 1, 18,
	4, 1, 17,
	4, 1, 8,
	4, 1, 27,
	4, 1, 14,
	4, 1, 13,
	4, 1, 4,
	4, 0, 23,
	4, 0, 9,
/* NumCoeffTrailingOnes_Table9-5_nC_4-7.out */
	2, 1, 16,
	0, 2, 50,
	0, 1, 57,
	0, 1, 60,
	6, 0, 10,
	6, 0, 8,
	0, 0, 61,
	0, 0, 62,
	4, 3, 31,
	4, 3, 27,
	4, 3, 23,
	4, 3, 19,
	4, 3, 15,
	4, 3, 10,
	4, 3, 5,
	4, 3, 0,
	0, 2, 3,
	0, 2, 10,
	0, 3, 17,
	4, 2, 51,
	4, 2, 46,
	4, 2, 41,
	4, 2, 36,
	4, 2, 47,
	4, 2, 42,
	4, 2, 37,
	4, 2, 32,
	4, 2, 48,
	4, 2, 54,
	4, 2, 49,
	4, 2, 44,
	4, 2, 55,
	4, 2, 50,
	4, 2, 45,
	4, 2, 40,
	3, 3, 0,
	4, 3, 64,
	4, 3, 67,
	4, 3, 66,
	4, 3, 65,
	4, 3, 60,
	4, 3, 63,
	4, 3, 62,
	4, 3, 61,
	4, 3, 56,
	4, 3, 59,
	4, 3, 58,
	4, 3, 57,
	4, 3, 52,
	4, 2, 53,
	4, 2, 53,
	4, 2, 28,
	4, 2, 24,
	4, 2, 38,
	4, 2, 20,
	4, 2, 43,
	4, 2, 34,
	4, 2, 33,
	4, 2, 16,
	4, 1, 12,
	4, 1, 30,
	4, 1, 29,
	4, 1, 8,
	4, 1, 39,
	4, 1, 26,
	4, 1, 25,
	4, 1, 4,
	4, 0, 13,
	4, 0, 35,
	4, 0, 14,
	4, 0, 9,
/* NumCoeffTrailingOnesFixedLen.out */
	2, 1, 8,
	5, 2, 6,
	5, 2, 10,
	5, 2, 14,
	5, 2, 18,
	5, 2, 22,
	5, 2, 26,
	5, 2, 30,
	5, 1, 4,
	0, 0, 2,
	5, 0, 2,
	3, 0, 0,
	4, 0, 0,
/* NumCoeffTrailingOnesChromaDC_YUV420.out */
	4, 0, 5,
	4, 1, 0,
	4, 2, 10,
	0, 2, 1,
	1, 1, 8,
	0, 0, 10,
	4, 2, 16,
	4, 2, 12,
	4, 2, 8,
	4, 2, 15,
	4, 2, 9,
	4, 2, 4,
	4, 0, 19,
	4, 1, 18,
	4, 1, 17,
	4, 0, 14,
	4, 0, 13,
/* NumCoeffTrailingOnesChromaDC_YUV422.out */
	4, 0, 0,
	4, 1, 5,
	4, 2, 10,
	0, 2, 4,
	4, 4, 15,
	4, 5, 19,
	2, 3, 9,
	4, 2, 27,
	4, 2, 23,
	4, 2, 18,
	4, 2, 14,
	4, 2, 13,
	4, 2, 9,
	4, 2, 8,
	4, 2, 4,
	0, 1, 5,
	0, 1, 8,
	0, 1, 11,
	0, 1, 14,
	1, 2, 17,
	4, 1, 22,
	4, 1, 17,
	4, 1, 16,
	4, 1, 12,
	4, 1, 31,
	4, 1, 26,
	4, 1, 21,
	4, 1, 20,
	4, 1, 35,
	4, 1, 30,
	4, 1, 25,
	4, 1, 24,
	4, 1, 34,
	4, 1, 33,
	4, 1, 29,
	4, 1, 28,
	3, 2, 0,
	3, 2, 0,
	3, 2, 0,
	4, 2, 32,
/* TotalZeros_00.out */
	4, 0, 0,
	0, 0, 6,
	0, 0, 7,
	0, 0, 8,
	0, 0, 9,
	0, 0, 10,
	0, 2, 11,
	4, 0, 2,
	4, 0, 1,
	4, 0, 4,
	4, 0, 3,
	4, 0, 6,
	4, 0, 5,
	4, 0, 8,
	4, 0, 7,
	4, 0, 10,
	4, 0, 9,
	3, 2, 0,
	4, 2, 15,
	4, 2, 14,
	4, 2, 13,
	4, 1, 12,
	4, 1, 12,
	4, 1, 11,
	4, 1, 11,
/* TotalZeros_01.out */
	1, 1, 8,
	0, 0, 14,
	0, 0, 15,
	4, 2, 4,
	4, 2, 3,
	4, 2, 2,
	4, 2, 1,
	4, 2, 0,
	0, 1, 3,
	4, 1, 10,
	4, 1, 9,
	4, 1, 14,
	4, 1, 13,
	4, 1, 12,
	4, 1, 11,
	4, 0, 8,
	4, 0, 7,
	4, 0, 6,
	4, 0, 5,
/* TotalZeros_02.out */
	0, 1, 8,
	0, 0, 13,
	0, 0, 14,
	4, 2, 7,
	4, 2, 6,
	4, 2, 3,
	4, 2, 2,
	4, 2, 1,
	0, 0, 4,
	4, 1, 12,
	4, 1, 10,
	4, 1, 9,
	4, 0, 13,
	4, 0, 11,
	4, 0, 8,
	4, 0, 5,
	4, 0, 4,
	4, 0, 0,
/* TotalZeros_03.out */
	0, 1, 8,
	0, 0, 11,
	0, 0, 12,
	4, 2, 8,
	4, 2, 6,
	4, 2, 5,
	4, 2, 4,
	4, 2, 1,
	4, 1, 12,
	4, 1, 11,
	4, 1, 10,
	4, 1, 0,
	4, 0, 9,
	4, 0, 7,
	4, 0, 3,
	4, 0, 2,
/* TotalZeros_04.out */
	2, 1, 8,
	0, 0, 10,
	0, 0, 11,
	4, 2, 7,
	4, 2, 6,
	4, 2, 5,
	4, 2, 4,
	4, 2, 3,
	4, 0, 10,
	4, 1, 9,
	4, 1, 11,
	4, 0, 8,
	4, 0, 2,
	4, 0, 1,
	4, 0, 0,
/* TotalZeros_05.out */
	2, 2, 8,
	4, 2, 9,
	4, 2, 7,
	4, 2, 6,
	4, 2, 5,
	4, 2, 4,
	4, 2, 3,
	4, 2, 2,
	4, 0, 8,
	4, 1, 1,
	4, 2, 0,
	4, 2, 10,
/* TotalZeros_06.out */
	2, 2, 8,
	4, 2, 8,
	4, 2, 6,
	4, 2, 4,
	4, 2, 3,
	4, 2, 2,
	4, 1, 5,
	4, 1, 5,
	4, 0, 7,
	4, 1, 1,
	4, 2, 0,
	4, 2, 9,
/* TotalZeros_07.out */
	2, 3, 4,
	0, 0, 8,
	4, 1, 5,
	4, 1, 4,
	4, 0, 7,
	4, 1, 1,
	4, 2, 2,
	4, 3, 0,
	4, 3, 8,
	4, 0, 6,
	4, 0, 3,
/* TotalZeros_08.out */
	2, 3, 4,
	4, 1, 6,
	4, 1, 4,
	4, 1, 3,
	4, 0, 5,
	4, 1, 2,
	4, 2, 7,
	4, 3, 0,
	4, 3, 1,
/* TotalZeros_09.out */
	2, 2, 4,
	4, 1, 5,
	4, 1, 4,
	4, 1, 3,
	4, 0, 2,
	4, 1, 6,
	4, 2, 0,
	4, 2, 1,
/* TotalZeros_10.out */
	4, 0, 4,
	0, 0, 3,
	4, 2, 2,
	5, 0, 0,
	4, 0, 3,
	4, 0, 5,
/* TotalZeros_11.out */
	4, 0, 3,
	4, 1, 2,
	4, 2, 4,
	5, 0, 0,
/* TotalZeros_12.out */
	4, 0, 2,
	4, 1, 3,
	5, 0, 0,
/* TotalZeros_13.out */
	5, 0, 0,
	4, 0, 2,
/* TotalZeros_14.out */
	4, 0, 0,
	4, 0, 1,
/* TotalZerosChromaDC_YUV420_00.out */
	4, 0, 0,
	4, 1, 1,
	4, 2, 2,
	4, 2, 3,
/* TotalZerosChromaDC_YUV420_01.out */
	4, 0, 0,
	4, 1, 1,
	4, 1, 2,
/* TotalZerosChromaDC_YUV420_02.out */
	4, 0, 1,
	4, 0, 0,
/* Run_00.out */
	4, 0, 1,
	4, 0, 0,
/* Run_01.out */
	4, 0, 0,
	4, 1, 1,
	4, 1, 2,
/* Run_02.out */
	4, 1, 3,
	4, 1, 2,
	4, 1, 1,
	4, 1, 0,
/* Run_03.out */
	0, 0, 4,
	4, 1, 2,
	4, 1, 1,
	4, 1, 0,
	4, 0, 4,
	4, 0, 3,
/* Run_04.out */
	0, 1, 3,
	4, 1, 1,
	4, 1, 0,
	4, 1, 5,
	4, 1, 4,
	4, 1, 3,
	4, 1, 2,
/* Run_05.out */
	4, 2, 1,
	4, 2, 2,
	4, 2, 4,
	4, 2, 3,
	4, 2, 6,
	4, 2, 5,
	4, 1, 0,
	4, 1, 0,
/* Run_06.out */
	2, 5, 8,
	4, 2, 6,
	4, 2, 5,
	4, 2, 4,
	4, 2, 3,
	4, 2, 2,
	4, 2, 1,
	4, 2, 0,
	4, 0, 7,
	4, 1, 8,
	4, 2, 9,
	4, 3, 10,
	4, 4, 11,
	4, 5, 12,
	2, 1, 1,
	4, 0, 13,
	4, 1, 14,
	3, 1, 0,
/* TotalZerosChromaDC_YUV422_00.out */
    4, 0, 0,
    6, 0, 0,
    6, 0, 1,
    4, 3, 5,
    4, 4, 6,
    4, 4, 7,
/* TotalZerosChromaDC_YUV422_01.out */
    6, 1, 1,
    4, 1, 1,
    4, 2, 2,
    4, 2, 0,
/* TotalZerosChromaDC_YUV422_02.out */
    5, 0, 0,
    4, 1, 2,
    4, 1, 3,
    5, 0, 2,
/* TotalZerosChromaDC_YUV422_03.out */
    6, 0, 0,
    4, 1, 3,
    4, 2, 0,
    4, 2, 4,
/* TotalZerosChromaDC_YUV422_04.out */
    5, 0, 0,
    5, 0, 1,
/* TotalZerosChromaDC_YUV422_05.out */
    5, 0, 0,
    4, 0, 2,
/* TotalZerosChromaDC_YUV422_06.out */
    4, 0, 0,
    4, 0, 1
};

IMG_UINT32 gaui32h264VlcPackedTableData[544];      //544
const IMG_UINT16 gui16h264VlcTableSize = 544;

/* EOF */
