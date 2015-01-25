/*!
 *****************************************************************************
 *
 * @file       mpeg2_vlc.c
 *
 * : mpeg2 vlc table
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
/* index infomation is stored in gaui16mpeg2VlcIndexData[] */

#include <img_types.h>

IMG_UINT16 gaui16mpeg2VlcTableData[] = {
/* b12.out */
    6, 0, 0,
    0, 0, 6,
    4, 2, 4,
    4, 3, 5,
    4, 4, 6,
    4, 5, 7,
    1, 1, 3,
    4, 0, 0,
    4, 0, 3,
    4, 0, 8,
    4, 1, 9,
    5, 0, 5,
/* b13.out */
    5, 0, 0,
    4, 1, 2,
    4, 2, 3,
    4, 3, 4,
    4, 4, 5,
    4, 5, 6,
    1, 2, 1,
    4, 0, 7,
    4, 1, 8,
    4, 2, 9,
    5, 0, 5,
/* DCT_coefficients_table_0_b14A.out */
    0, 2, 16,
    0, 1, 87,
    2, 3, 90,
    0, 0, 98,
    5, 0, 253,
    5, 0, 366,
    4, 3, 380,
    4, 3, 381,
    4, 1, 508,
    4, 1, 508,
    4, 1, 508,
    4, 1, 508,
    4, 1, 509,
    4, 1, 509,
    4, 1, 509,
    4, 1, 509,
    0, 4, 8,
    0, 2, 63,
    4, 1, 255,
    4, 1, 255,
    5, 0, 365,
    5, 0, 470,
    5, 0, 251,
    5, 0, 471,
    3, 4, 0,
    0, 1, 31,
    0, 0, 40,
    2, 2, 41,
    5, 2, 224,
    5, 2, 228,
    5, 2, 232,
    5, 2, 236,
    5, 1, 437,
    0, 0, 39,
    0, 0, 40,
    0, 0, 41,
    5, 1, 241,
    0, 0, 41,
    5, 1, 454,
    5, 1, 456,
    5, 0, 244,
    5, 0, 439,
    5, 0, 348,
    5, 0, 245,
    5, 0, 363,
    5, 0, 325,
    5, 0, 458,
    5, 0, 459,
    5, 0, 246,
    5, 0, 460,
    5, 0, 461,
    5, 0, 186,
    5, 0, 356,
    5, 0, 247,
    5, 0, 333,
    5, 0, 462,
    5, 2, 173,
    2, 1, 3,
    1, 1, 5,
    5, 2, 449,
    5, 1, 432,
    5, 0, 431,
    5, 0, 332,
    5, 1, 434,
    5, 0, 436,
    5, 0, 448,
    5, 2, 215,
    5, 2, 219,
    5, 2, 180,
    5, 1, 178,
    5, 0, 177,
    5, 0, 223,
    5, 0, 340,
    5, 0, 355,
    5, 0, 362,
    5, 0, 184,
    5, 0, 185,
    5, 0, 240,
    5, 0, 243,
    5, 0, 453,
    5, 0, 463,
    5, 0, 341,
    5, 0, 248,
    5, 0, 364,
    5, 0, 187,
    5, 0, 464,
    5, 0, 465,
    5, 0, 349,
    5, 0, 326,
    5, 0, 334,
    5, 0, 189,
    5, 0, 342,
    5, 0, 252,
    0, 1, 4,
    5, 1, 467,
    5, 0, 249,
    5, 0, 466,
    5, 0, 357,
    5, 0, 188,
    5, 0, 250,
    5, 0, 469,
    5, 0, 350,
    5, 0, 358,
/* DCT_coefficients_table_0_b14B.out */
    0, 2, 16,
    0, 1, 87,
    2, 3, 90,
    0, 0, 98,
    5, 0, 253,
    5, 0, 366,
    4, 3, 380,
    4, 3, 381,
    4, 1, 254,
    4, 1, 254,
    4, 1, 254,
    4, 1, 254,
    4, 2, 508,
    4, 2, 508,
    4, 2, 509,
    4, 2, 509,
    0, 4, 8,
    0, 2, 63,
    4, 1, 255,
    4, 1, 255,
    5, 0, 365,
    5, 0, 470,
    5, 0, 251,
    5, 0, 471,
    3, 4, 0,
    0, 1, 31,
    0, 0, 40,
    2, 2, 41,
    5, 2, 224,
    5, 2, 228,
    5, 2, 232,
    5, 2, 236,
    5, 1, 437,
    0, 0, 39,
    0, 0, 40,
    0, 0, 41,
    5, 1, 241,
    0, 0, 41,
    5, 1, 454,
    5, 1, 456,
    5, 0, 244,
    5, 0, 439,
    5, 0, 348,
    5, 0, 245,
    5, 0, 363,
    5, 0, 325,
    5, 0, 458,
    5, 0, 459,
    5, 0, 246,
    5, 0, 460,
    5, 0, 461,
    5, 0, 186,
    5, 0, 356,
    5, 0, 247,
    5, 0, 333,
    5, 0, 462,
    5, 2, 173,
    2, 1, 3,
    1, 1, 5,
    5, 2, 449,
    5, 1, 432,
    5, 0, 431,
    5, 0, 332,
    5, 1, 434,
    5, 0, 436,
    5, 0, 448,
    5, 2, 215,
    5, 2, 219,
    5, 2, 180,
    5, 1, 178,
    5, 0, 177,
    5, 0, 223,
    5, 0, 340,
    5, 0, 355,
    5, 0, 362,
    5, 0, 184,
    5, 0, 185,
    5, 0, 240,
    5, 0, 243,
    5, 0, 453,
    5, 0, 463,
    5, 0, 341,
    5, 0, 248,
    5, 0, 364,
    5, 0, 187,
    5, 0, 464,
    5, 0, 465,
    5, 0, 349,
    5, 0, 326,
    5, 0, 334,
    5, 0, 189,
    5, 0, 342,
    5, 0, 252,
    0, 1, 4,
    5, 1, 467,
    5, 0, 249,
    5, 0, 466,
    5, 0, 357,
    5, 0, 188,
    5, 0, 250,
    5, 0, 469,
    5, 0, 350,
    5, 0, 358,
/* DCT_coefficients_table_1_b15.out */
    2, 2, 32,
    0, 1, 87,
    5, 1, 248,
    0, 0, 89,
    1, 2, 90,
    5, 0, 366,
    5, 0, 189,
    5, 0, 358,
    4, 3, 380,
    4, 3, 380,
    4, 3, 381,
    4, 3, 381,
    4, 3, 254,
    4, 3, 254,
    4, 4, 504,
    4, 4, 505,
    4, 2, 508,
    4, 2, 508,
    4, 2, 508,
    4, 2, 508,
    4, 2, 509,
    4, 2, 509,
    4, 2, 509,
    4, 2, 509,
    4, 3, 506,
    4, 3, 506,
    4, 3, 507,
    4, 3, 507,
    5, 0, 251,
    5, 0, 250,
    0, 1, 71,
    0, 2, 74,
    4, 0, 255,
    0, 1, 3,
    0, 2, 8,
    0, 3, 17,
    5, 0, 341,
    5, 0, 465,
    0, 0, 2,
    5, 0, 464,
    5, 0, 363,
    5, 0, 463,
    5, 1, 438,
    5, 1, 348,
    5, 1, 324,
    5, 1, 458,
    5, 1, 459,
    5, 1, 461,
    5, 1, 356,
    0, 0, 1,
    5, 0, 333,
    5, 0, 462,
    3, 3, 0,
    0, 1, 15,
    0, 0, 24,
    2, 2, 25,
    5, 2, 224,
    5, 2, 228,
    5, 2, 232,
    5, 2, 236,
    5, 1, 437,
    0, 0, 23,
    0, 0, 24,
    5, 1, 185,
    3, 3, 0,
    5, 1, 452,
    5, 1, 454,
    5, 1, 456,
    5, 2, 173,
    2, 1, 3,
    1, 1, 5,
    5, 2, 449,
    5, 1, 432,
    5, 0, 431,
    5, 0, 332,
    5, 1, 434,
    5, 0, 436,
    5, 0, 448,
    5, 2, 215,
    5, 2, 219,
    5, 2, 180,
    5, 1, 178,
    5, 0, 177,
    5, 0, 223,
    5, 0, 340,
    5, 0, 355,
    5, 0, 362,
    5, 0, 184,
    5, 0, 326,
    5, 0, 471,
    5, 0, 334,
    5, 0, 365,
    5, 0, 350,
    5, 0, 342,
    2, 1, 4,
    5, 1, 466,
    5, 0, 357,
    5, 0, 187,
    5, 1, 244,
    5, 0, 468,
    5, 0, 186,
    5, 0, 470,
    5, 0, 188,
    5, 0, 469,
    5, 0, 247,
    4, 2, 492,
    4, 2, 493,
    5, 0, 243,
    5, 0, 242,
    5, 0, 364,
    5, 0, 349,
    5, 0, 241,
    5, 0, 240,
/* macroblock_address_increment_b1.out */
    4, 0, 30,
    5, 0, 14,
    5, 0, 13,
    5, 0, 12,
    0, 0, 3,
    2, 2, 4,
    0, 1, 7,
    5, 1, 9,
    5, 0, 11,
    5, 0, 8,
    5, 1, 6,
    5, 0, 5,
    5, 1, 3,
    3, 1, 0,
    2, 2, 3,
    3, 1, 0,
    2, 1, 5,
    3, 2, 0,
    3, 2, 0,
    3, 2, 0,
    4, 2, 226,
    5, 1, 1,
    5, 0, 0,
    5, 0, 31,
/* motion_code_b10.out */
    4, 0, 62,
    5, 0, 30,
    5, 0, 29,
    5, 0, 28,
    0, 0, 3,
    2, 2, 4,
    1, 1, 7,
    5, 1, 25,
    5, 0, 27,
    5, 0, 24,
    5, 1, 22,
    5, 0, 21,
    5, 1, 19,
    3, 1, 0,
    3, 1, 0,
    5, 2, 15,
};

IMG_UINT32 gaui32mpeg2VlcPackedTableData[382];
const IMG_UINT16 gui16mpeg2VlcTableSize = 382;

/* EOF */
