/*******************************************************************************
 * dtdCommon.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file dtdCommon.c
 * @brief DataType Layer 데이타 타입관련 공통 함수 정의
 */

#include <dtl.h>

#include <dtdDef.h>
#include <dtDef.h>
#include <dtfNumeric.h>
#include <dtsCharacterSet.h>

/**
 * @addtogroup dtdDef
 * @{
 */

#define JOIN(N) N "0", N "1", N "2", N "3", N "4", N "5", N "6", N "7", N "8", N "9"
#define JOIN2(N) JOIN(N "0"), JOIN(N "1"), JOIN(N "2"), JOIN(N "3"), JOIN(N "4"),       \
        JOIN(N "5"), JOIN(N "6"), JOIN(N "7"), JOIN(N "8"), JOIN(N "9")
#define JOIN3(N) JOIN2(N "0"), JOIN2(N "1"), JOIN2(N "2"), JOIN2(N "3"), JOIN2(N "4"),  \
        JOIN2(N "5"), JOIN2(N "6"), JOIN2(N "7"), JOIN2(N "8"), JOIN2(N "9")
#define JOIN4(N) JOIN3(N "0"), JOIN3(N "1"), JOIN3(N "2"), JOIN3(N "3"), JOIN3(N "4"),  \
        JOIN3(N "5"), JOIN3(N "6"), JOIN3(N "7"), JOIN3(N "8"), JOIN3(N "9")

static const stlChar dtdTable1[][4] = { JOIN("") };
static const stlChar dtdTable2[][4] = { JOIN2("") };
static const stlChar dtdTable3[][4] = { JOIN3("") };
static const stlChar dtdTable4[][8] = { JOIN4("") };


const stlInt8 dtdHexLookup[256] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};


const stlChar dtdHexTable[DTL_HEX_CHAR_TABLE_SIZE] = "0123456789ABCDEF";

/**
 * BASE64 문자표
 * <BR>
 * <table>
 *   <tr> <td> 값 </td>  <td> 문자 </td>  <td> 값 </td>  <td> 문자 </td> </tr>
 *   <tr> <td>  0 </td>  <td>  A   </td>  <td> 32 </td>  <td>  g   </td> </tr>
 *   <tr> <td>  1 </td>  <td>  B   </td>  <td> 33 </td>  <td>  h   </td> </tr>
 *   <tr> <td>  2 </td>  <td>  C   </td>  <td> 34 </td>  <td>  i   </td> </tr>
 *   <tr> <td>  3 </td>  <td>  D   </td>  <td> 35 </td>  <td>  j   </td> </tr>
 *   <tr> <td>  4 </td>  <td>  E   </td>  <td> 36 </td>  <td>  k   </td> </tr>
 *   <tr> <td>  5 </td>  <td>  F   </td>  <td> 37 </td>  <td>  l   </td> </tr>
 *   <tr> <td>  6 </td>  <td>  G   </td>  <td> 38 </td>  <td>  m   </td> </tr>
 *   <tr> <td>  7 </td>  <td>  H   </td>  <td> 39 </td>  <td>  n   </td> </tr>
 *   <tr> <td>  8 </td>  <td>  I   </td>  <td> 40 </td>  <td>  o   </td> </tr>
 *   <tr> <td>  9 </td>  <td>  J   </td>  <td> 41 </td>  <td>  p   </td> </tr>
 *   <tr> <td> 10 </td>  <td>  K   </td>  <td> 42 </td>  <td>  q   </td> </tr>
 *   <tr> <td> 11 </td>  <td>  L   </td>  <td> 43 </td>  <td>  r   </td> </tr>
 *   <tr> <td> 12 </td>  <td>  M   </td>  <td> 44 </td>  <td>  s   </td> </tr>
 *   <tr> <td> 13 </td>  <td>  N   </td>  <td> 45 </td>  <td>  t   </td> </tr>
 *   <tr> <td> 14 </td>  <td>  O   </td>  <td> 46 </td>  <td>  u   </td> </tr>
 *   <tr> <td> 15 </td>  <td>  P   </td>  <td> 47 </td>  <td>  v   </td> </tr>
 *   <tr> <td> 16 </td>  <td>  Q   </td>  <td> 48 </td>  <td>  w   </td> </tr>
 *   <tr> <td> 17 </td>  <td>  R   </td>  <td> 49 </td>  <td>  x   </td> </tr>
 *   <tr> <td> 18 </td>  <td>  S   </td>  <td> 50 </td>  <td>  y   </td> </tr>
 *   <tr> <td> 19 </td>  <td>  T   </td>  <td> 51 </td>  <td>  z   </td> </tr>
 *   <tr> <td> 20 </td>  <td>  U   </td>  <td> 52 </td>  <td>  0   </td> </tr>
 *   <tr> <td> 21 </td>  <td>  V   </td>  <td> 53 </td>  <td>  1   </td> </tr>
 *   <tr> <td> 22 </td>  <td>  W   </td>  <td> 54 </td>  <td>  2   </td> </tr>
 *   <tr> <td> 23 </td>  <td>  X   </td>  <td> 55 </td>  <td>  3   </td> </tr>
 *   <tr> <td> 24 </td>  <td>  Y   </td>  <td> 56 </td>  <td>  4   </td> </tr>
 *   <tr> <td> 25 </td>  <td>  Z   </td>  <td> 57 </td>  <td>  5   </td> </tr>
 *   <tr> <td> 26 </td>  <td>  a   </td>  <td> 58 </td>  <td>  6   </td> </tr>
 *   <tr> <td> 27 </td>  <td>  b   </td>  <td> 59 </td>  <td>  7   </td> </tr>
 *   <tr> <td> 28 </td>  <td>  c   </td>  <td> 60 </td>  <td>  8   </td> </tr>
 *   <tr> <td> 29 </td>  <td>  d   </td>  <td> 61 </td>  <td>  9   </td> </tr>
 *   <tr> <td> 30 </td>  <td>  e   </td>  <td> 62 </td>  <td>  +   </td> </tr>
 *   <tr> <td> 31 </td>  <td>  f   </td>  <td> 63 </td>  <td>  /   </td> </tr>
 * </table>   
 */


const stlChar dtdBase64[DTD_BASE64_CHAR_TABLE_SIZE] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


const stlChar dtdBase64Char[257] =
    "\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040"
    "\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040"
    "\040\041\042\043\044\045\046\047\050\051\052\053\054\055\056\057"
    "\060\061\062\063\064\065\066\067\070\071\072\073\074\075\076\077"
    "\100\101\102\103\104\105\106\107\110\111\112\113\114\115\116\117"
    "\120\121\122\123\124\125\126\127\130\131\132\133\134\135\136\137"
    "\140\141\142\143\144\145\146\147\150\151\152\153\154\155\156\157"
    "\160\161\162\163\164\165\166\167\170\171\172\173\174\175\176\040"
    "\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040"
    "\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040"
    "\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040"
    "\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040"
    "\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040"
    "\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040"
    "\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040"
    "\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040\040";


const stlInt8 dtdBase64lookup[256] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
        
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1        
};


/**
 * 1번재: length 대소비교 -1, 0, 1
 * 2번째: digit count
 */
stlInt8 dtlNumericLenCompTable[22][22][2] =
{
    {   /* 0 */
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
        {0, 0}
    },
    {   /* 1 */
        {0, 0}, {0, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, 
        {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, 
        {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0},
        {-1, 0}
    },
    {   /* 2 */
        {0, 0}, {1, 0}, {0, 1}, {-1, 1}, {-1, 1}, {-1, 1}, {-1, 1}, 
        {-1, 1}, {-1, 1}, {-1, 1}, {-1, 1}, {-1, 1}, {-1, 1}, {-1, 1}, 
        {-1, 1}, {-1, 1}, {-1, 1}, {-1, 1}, {-1, 1}, {-1, 1}, {-1, 1},
        {-1, 1}
    },
    {   /* 3 */
        {0, 0}, {1, 0}, {1, 1}, {0, 2}, {-1, 2}, {-1, 2}, {-1, 2}, 
        {-1, 2}, {-1, 2}, {-1, 2}, {-1, 2}, {-1, 2}, {-1, 2}, {-1, 2}, 
        {-1, 2}, {-1, 2}, {-1, 2}, {-1, 2}, {-1, 2}, {-1, 2}, {-1, 2},
        {-1, 2}
    },
    {   /* 4 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {0, 3}, {-1, 3}, {-1, 3}, 
        {-1, 3}, {-1, 3}, {-1, 3}, {-1, 3}, {-1, 3}, {-1, 3}, {-1, 3}, 
        {-1, 3}, {-1, 3}, {-1, 3}, {-1, 3}, {-1, 3}, {-1, 3}, {-1, 3},
        {-1, 3}
    },
    {   /* 5 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {0, 4}, {-1, 4}, 
        {-1, 4}, {-1, 4}, {-1, 4}, {-1, 4}, {-1, 4}, {-1, 4}, {-1, 4}, 
        {-1, 4}, {-1, 4}, {-1, 4}, {-1, 4}, {-1, 4}, {-1, 4}, {-1, 4},
        {-1, 4}
    },
    {   /* 6 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {0, 5}, 
        {-1, 5}, {-1, 5}, {-1, 5}, {-1, 5}, {-1, 5}, {-1, 5}, {-1, 5}, 
        {-1, 5}, {-1, 5}, {-1, 5}, {-1, 5}, {-1, 5}, {-1, 5}, {-1, 5},
        {-1, 5}
    },
    {   /* 7 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {0, 6}, {-1, 6}, {-1, 6}, {-1, 6}, {-1, 6}, {-1, 6}, {-1, 6}, 
        {-1, 6}, {-1, 6}, {-1, 6}, {-1, 6}, {-1, 6}, {-1, 6}, {-1, 6},
        {-1, 6}
    },
    {   /* 8 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {1, 6}, {0, 7}, {-1, 7}, {-1, 7}, {-1, 7}, {-1, 7}, {-1, 7}, 
        {-1, 7}, {-1, 7}, {-1, 7}, {-1, 7}, {-1, 7}, {-1, 7}, {-1, 7},
        {-1, 7}
    },
    {   /* 9 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {1, 6}, {1, 7}, {0, 8}, {-1, 8}, {-1, 8}, {-1, 8}, {-1, 8}, 
        {-1, 8}, {-1, 8}, {-1, 8}, {-1, 8}, {-1, 8}, {-1, 8}, {-1, 8},
        {-1, 8}
    },
    {   /* 10 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {1, 6}, {1, 7}, {1, 8}, {0, 9}, {-1, 9}, {-1, 9}, {-1, 9}, 
        {-1, 9}, {-1, 9}, {-1, 9}, {-1, 9}, {-1, 9}, {-1, 9}, {-1, 9},
        {-1, 9}
    },
    {   /* 11 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {1, 6}, {1, 7}, {1, 8}, {1, 9}, {0, 10}, {-1, 10}, {-1, 10}, 
        {-1, 10}, {-1, 10}, {-1, 10}, {-1, 10}, {-1, 10}, {-1, 10}, {-1, 10},
        {-1, 10}
    },
    {   /* 12 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {0, 11}, {-1, 11}, 
        {-1, 11}, {-1, 11}, {-1, 11}, {-1, 11}, {-1, 11}, {-1, 11}, {-1, 11},
        {-1, 11}
    },
    {   /* 13 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {0, 12}, 
        {-1, 12}, {-1, 12}, {-1, 12}, {-1, 12}, {-1, 12}, {-1, 12}, {-1, 12},
        {-1, 12}
    },
    {   /* 14 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, 
        {0, 13}, {-1, 13}, {-1, 13}, {-1, 13}, {-1, 13}, {-1, 13}, {-1, 13},
        {-1, 13}
    },
    {   /* 15 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, 
        {1, 13}, {0, 14}, {-1, 14}, {-1, 14}, {-1, 14}, {-1, 14}, {-1, 14},
        {-1, 14}
    },
    {   /* 16 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, 
        {1, 13}, {1, 14}, {0, 15}, {-1, 15}, {-1, 15}, {-1, 15}, {-1, 15},
        {-1, 15}
    },
    {   /* 17 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, 
        {1, 13}, {1, 14}, {1, 15}, {0, 16}, {-1, 16}, {-1, 16}, {-1, 16},
        {-1, 16}
    },
    {   /* 18 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, 
        {1, 13}, {1, 14}, {1, 15}, {1, 16}, {0, 17}, {-1, 17}, {-1, 17},
        {-1, 17}
    },
    {   /* 19 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, 
        {1, 13}, {1, 14}, {1, 15}, {1, 16}, {1, 17}, {0, 18}, {-1, 18},
        {-1, 18}
    },
    {   /* 20 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, 
        {1, 13}, {1, 14}, {1, 15}, {1, 16}, {1, 17}, {1, 18}, {0, 19},
        {-1, 19}
    },
    {   /* 21 */
        {0, 0}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, 
        {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, 
        {1, 13}, {1, 14}, {1, 15}, {1, 16}, {1, 17}, {1, 18}, {1, 19},
        {0, 20}
    },
};


/** @} */


/**
 * @addtogroup dtdDataType
 * @{
 */


#define IS_WHITE_CHARACTER(_ch)     ( stlIsspace((_ch)) )

#define SKIP_LEADING_WHITE_SPACE(_str_first, _str_last) {                       \
    while( ((_str_first) <= (_str_last)) && IS_WHITE_CHARACTER(*(_str_first)) ) \
    {                                                                           \
        (_str_first)++;                                                         \
    }                                                                           \
}

#define SKIP_TRAILING_WHITE_SPACE(_str_first, _str_last) {                  \
    while( ((_str_first) <= (_str_last)) &&                                 \
           ( IS_WHITE_CHARACTER(*(_str_last)) || (*(_str_last)=='\0') ) )   \
    {                                                                       \
        (_str_last)--;                                                      \
    }                                                                       \
}

#define SKIP_LEADING_DIGITS(_str_first, _str_last) {                        \
    while( ((_str_first) <= (_str_last)) && DTF_IS_DIGIT(*(_str_first)) )   \
    {                                                                       \
        (_str_first)++;                                                     \
    }                                                                       \
}

#define SKIP_LEADING_ZERO_CHARACTERS(_str_first, _str_last) {           \
        while( ((_str_first) <= (_str_last)) && (*(_str_first) == '0')) \
        {                                                               \
            (_str_first)++;                                             \
        }                                                               \
    }

#define SKIP_TRAILING_ZERO_CHARACTERS(_str_first, _str_last) {              \
    while( ((_str_first) <= (_str_last)) &&                                 \
           ( (*(_str_last) == '0') || (*(_str_last)=='\0') ) )              \
    {                                                                       \
        (_str_last)--;                                                      \
    }                                                                       \
}

#define SKIP_LEADING_ZERO_CHARACTERS_AFTER_DECIMAL(_str_first, _str_last, _skip_count) {        \
        (_skip_count) = 0;                                                                      \
        while( ((_str_first) <= (_str_last)) && (*(_str_first) == '0'))                         \
        {                                                                                       \
            (_str_first)++;                                                                     \
            (_skip_count)++;                                                                    \
        }                                                                                       \
    }

#define CHECK_AND_ASSIGN_SIGNED_CHARACTER(_str_first, _str_last, _is_positive) {    \
    if( *(_str_first) == '+' )                                                      \
    {                                                                               \
        (_is_positive) = STL_TRUE;                                                  \
        (_str_first)++;                                                             \
    }                                                                               \
    else if( *(_str_first) == '-' )                                                 \
    {                                                                               \
        (_is_positive) = STL_FALSE;                                                 \
        (_str_first)++;                                                             \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        (_is_positive) = STL_TRUE;                                                  \
    }                                                                               \
}

#define TRUNCATE_LEADING_ZERO_CHARACTERS(_str, _len, _truncated_count) {    \
    (_truncated_count) = 0;                                                 \
    while( (_len) > 0 )                                                     \
    {                                                                       \
        if( *(_str) == '0' )                                                \
        {                                                                   \
            (_str)++;                                                       \
            (_len)--;                                                       \
            (_truncated_count)++;                                           \
        }                                                                   \
        else                                                                \
        {                                                                   \
            break;                                                          \
        }                                                                   \
    }                                                                       \
    if( (_len) == 0 )                                                       \
    {                                                                       \
        (_str) = NULL;                                                      \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        /* Do Nothing */                                                    \
    }                                                                       \
}

#define TRUNCATE_TRAILING_ZERO_CHARACTERS(_str, _len, _truncated_count) {   \
    (_truncated_count) = 0;                                                 \
    while( (_len) > 0 )                                                     \
    {                                                                       \
        if( *((_str) + (_len) - 1) == '0' )                                 \
        {                                                                   \
            (_len)--;                                                       \
            (_truncated_count)++;                                           \
        }                                                                   \
        else                                                                \
        {                                                                   \
            break;                                                          \
        }                                                                   \
    }                                                                       \
    if( (_len) == 0 )                                                       \
    {                                                                       \
        (_str) = NULL;                                                      \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        /* Do Nothing */                                                    \
    }                                                                       \
}


/**
 * @brief       Binary String 타입의 값이 padding 값인지를 확인한다.
 *        <BR> (internal function)
 * @param[in]   aValue      Binary, Varbinary, Longvarbinary
 * @param[in]   aLength     padding 여부를 검사할 영역의 크기
 * @return      padding 여부 분석 결과 
 * @remark 주어진 value가 모두 padding 값이면 STL_TRUE를 반환, 아니면 STL_FALSE를 반환한다.
 */
stlBool dtdIsBinaryPadding( stlChar   * aValue,
                            stlInt64    aLength )
{
    stlChar    * sValue     = aValue;
    stlUInt64    sLength    = aLength;
    
    while( (sLength > 0) && (*sValue == 0x00) )
    {
        ++sValue;
        --sLength;
    }
    return sLength == 0;
}

/**
 * @brief      space padding을 제거한 value의 길이를 구한다.
 *        <BR> (internal function)
 * @param[in]  aValue       Char type
 * @return     space padding을 제거한 value의 길이
 */
stlInt64 dtdGetLenTrimPadding( dtlDataValue * aValue )
{
    stlInt64    sLength  = aValue->mLength;
    stlChar   * sChar    = & ((stlChar *)aValue->mValue)[ sLength ];
    stlUInt64   sIsBlank = 1;

    while( (sLength > 0) & sIsBlank )
    {
        sChar--;
        sIsBlank = ( *sChar == ' ' );
        sLength -= sIsBlank;
    }
    return sLength;
}

/**
 * @brief Byte Order 변경
 * @param[in]  aSrcValue       byte swapping 대상 
 * @param[in]  aLength         byte swapping 대상의 크기
 * @param[out] aBuffer         byte swapping 결과가 저장될 메모리 공간
 */
void dtdReverseByteOrder( stlChar     * aSrcValue,
                          stlInt64      aLength,
                          stlChar     * aBuffer )
{
    stlInt64   sPosition1;
    stlInt64   sPosition2;

    if( aLength > 1 )
    {
        if( aSrcValue == aBuffer )
        {
            // swap
            sPosition1 = 0;
            sPosition2 = aLength / 2;
        
            for( ; sPosition1 < sPosition2 ; sPosition1++ )
            {
                DTL_SWAP( aSrcValue[ sPosition1 ],
                          aSrcValue[ aLength - sPosition1 - 1 ],
                          stlChar );
            }
        }
        else
        {
            // copy
            sPosition1 = 0;
            sPosition2 = aLength - 1;
        
            for( ; sPosition1 < aLength ; sPosition1++, sPosition2-- )
            {
                aBuffer[sPosition2]= aSrcValue[sPosition1];
            }
        }
    }
    else
    {
        // Nothing.
    }

    return;
}

/**
 * @brief 16진수를 10진수 값으로 변환 (redirect 함수 아님, 내부 함수)
 * @param[in]   aHex        16진수
 * @param[out]  aNumber     10진수
 * @param[out]  aErrorStack 에러 스택
 * 
 * 16진수 값에 대응되는 10진수 값을 리턴한다.
 * 16진수 표현을 벗어나는 문자는 -1의 값을 리턴한다.
 * 
 * ex) '2' => 2
 *     'a' => 10
 *     'x' => -1 
 */
stlStatus dtdGetNumberFromHex( stlChar           aHex,
                               stlInt8         * aNumber,
                               stlErrorStack   * aErrorStack )
{
    stlInt8  sNumber;

    STL_PARAM_VALIDATE( aNumber != NULL, aErrorStack);
    
    sNumber = dtdHexLookup[(stlUInt8)aHex];

    STL_TRY_THROW( sNumber != -1, ERROR_OUT_OF_RANGE );

    *aNumber = sNumber;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_HEXADECIMAL_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief 16진수들을 10진수들로 변환 (redirect 함수 아님, 내부 함수)
 * @param[in]  aHex          16진수 배열
 * @param[in]  aHexLength    16진수 배열 길이
 * @param[out] aNumberLength 10진수 배열 길이
 * @param[out] aErrorStack   에러 스택
 *
 * 공백 제거하여 decode 수행.
 */
stlStatus dtdGetLengthOfDecodedHex( stlChar         * aHex,
                                    stlInt64          aHexLength,
                                    stlInt64        * aNumberLength, 
                                    stlErrorStack   * aErrorStack )
{
    const stlChar  * sEndPos;
    stlBool          sIsFirst;
    stlInt8          sNumber;
    stlInt64         sNumberLength;
    
    STL_PARAM_VALIDATE( aHex != NULL, aErrorStack);
    STL_PARAM_VALIDATE( aHexLength >= 0, aErrorStack);

    sEndPos        = aHex + aHexLength;
    sIsFirst       = STL_TRUE;
    sNumberLength  = 0;

    while( aHex < sEndPos )
    {
        if( stlIsspace(*aHex) )
        {
            /* Nothing */
        }
        else
        {
            STL_TRY( dtdGetNumberFromHex( *aHex, & sNumber, aErrorStack )
                     == STL_SUCCESS );
            STL_TRY_THROW( sNumber != -1,
                           ERROR_INVALID_INPUT_DATA );

            if( sIsFirst == STL_TRUE )
            {
                sIsFirst  = STL_FALSE; 
            }
            else
            {
                sNumberLength++;
                
                sIsFirst  = STL_TRUE;
            }
        }
        
        aHex++;
    }

    STL_TRY_THROW( sIsFirst == STL_TRUE,
                   ERROR_INVALID_INPUT_DATA_LENGTH );

    *aNumberLength = sNumberLength;

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_HEXADECIMAL_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_INVALID_INPUT_DATA_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_HEXADECIMAL_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief BINARY STRING을 문자열로 변환 (BINARY, VARYING BINARY, LONG VARYING BINARY)
 *        10진수들을 16진수들로 변환 (redirect 함수 아님, 내부 함수)
 * 
 * @param[in]  aNumber        10진수 배열
 * @param[in]  aNumberLength  10진수 배열 길이
 * @param[in]  aAvailableSize 16진수 배열이 저장될 메모리 공간의 크기
 * @param[out] aHex           16진수 배열
 * @param[out] aHexLength     16진수 배열 길이
 * @param[out] aErrorStack    에러 스택
 */
stlStatus dtdToStringFromBinaryString( stlInt8         * aNumber,
                                       stlInt64          aNumberLength,
                                       stlInt64          aAvailableSize,
                                       stlChar         * aHex,
                                       stlInt64        * aHexLength, 
                                       stlErrorStack   * aErrorStack )
{
    const stlInt8  * sEndPos;
    stlChar        * sHex;
    stlInt64         sHexLength;

    STL_PARAM_VALIDATE( aNumber != NULL, aErrorStack);
    STL_PARAM_VALIDATE( aNumberLength >= 0, aErrorStack);
    STL_PARAM_VALIDATE( aHex != NULL, aErrorStack);

    STL_PARAM_VALIDATE( aAvailableSize >= aNumberLength * 2 , 
                        aErrorStack );
        
    sEndPos     = aNumber + aNumberLength;
    sHexLength  = 0;
    sHex        = aHex;
    
    while( aNumber < sEndPos )
    {
        *sHex = dtdHexTable[ (*aNumber >> 4) & 0xF ];
        sHex++;
        *sHex = dtdHexTable[ *aNumber & 0xF ];
        sHex++;
        
        aNumber++;
        sHexLength += 2;
    }

    STL_DASSERT( sHexLength == aNumberLength * 2 );

    *aHexLength = sHexLength;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief BINARY STRING을 utf16 문자열로 변환 (BINARY, VARYING BINARY, LONG VARYING BINARY)
 *        10진수들을 16진수들로 변환 (redirect 함수 아님, 내부 함수)
 * 
 * @param[in]  aNumber        10진수 배열
 * @param[in]  aNumberLength  10진수 배열 길이
 * @param[in]  aAvailableSize 16진수 배열이 저장될 메모리 공간의 크기
 * @param[out] aHex           16진수 배열
 * @param[out] aHexLength     16진수 배열 길이
 * @param[out] aErrorStack    에러 스택
 */
stlStatus dtdToUtf16WCharStringFromBinaryString( stlInt8         * aNumber,
                                                 stlInt64          aNumberLength,
                                                 stlInt64          aAvailableSize,
                                                 void            * aHex,
                                                 stlInt64        * aHexLength, 
                                                 stlErrorStack   * aErrorStack )
{
    const stlInt8  * sEndPos;
    stlUInt16      * sHex;
    stlInt64         sHexLength;
    stlInt64         sOneHexLength;

    STL_PARAM_VALIDATE( aNumber != NULL, aErrorStack);
    STL_PARAM_VALIDATE( aNumberLength >= 0, aErrorStack);
    STL_PARAM_VALIDATE( aHex != NULL, aErrorStack);

    STL_PARAM_VALIDATE( aAvailableSize >= aNumberLength * 2 * STL_UINT16_SIZE, 
                        aErrorStack );
        
    sEndPos     = aNumber + aNumberLength;
    sHexLength  = 0;
    sOneHexLength = 2 * STL_UINT16_SIZE;
    sHex        = (stlUInt16*)aHex;
    
    while( aNumber < sEndPos )
    {
        *sHex = dtdHexTable[ (*aNumber >> 4) & 0xF ];
        sHex++;
        *sHex = dtdHexTable[ *aNumber & 0xF ];
        sHex++;
        
        aNumber++;
        sHexLength += sOneHexLength; /* ( 2 * STL_UINT16_SIZE ) */
    }

    STL_DASSERT( sHexLength == aNumberLength * sOneHexLength );

    *aHexLength = sHexLength;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief BINARY STRING을 utf32 문자열로 변환 (BINARY, VARYING BINARY, LONG VARYING BINARY)
 *        10진수들을 16진수들로 변환 (redirect 함수 아님, 내부 함수)
 * 
 * @param[in]  aNumber        10진수 배열
 * @param[in]  aNumberLength  10진수 배열 길이
 * @param[in]  aAvailableSize 16진수 배열이 저장될 메모리 공간의 크기
 * @param[out] aHex           16진수 배열
 * @param[out] aHexLength     16진수 배열 길이
 * @param[out] aErrorStack    에러 스택
 */
stlStatus dtdToUtf32WCharStringFromBinaryString( stlInt8         * aNumber,
                                                 stlInt64          aNumberLength,
                                                 stlInt64          aAvailableSize,
                                                 void            * aHex,
                                                 stlInt64        * aHexLength, 
                                                 stlErrorStack   * aErrorStack )
{
    const stlInt8  * sEndPos;
    stlUInt32      * sHex;
    stlInt64         sHexLength;
    stlInt64         sOneHexLength;

    STL_PARAM_VALIDATE( aNumber != NULL, aErrorStack);
    STL_PARAM_VALIDATE( aNumberLength >= 0, aErrorStack);
    STL_PARAM_VALIDATE( aHex != NULL, aErrorStack);

    STL_PARAM_VALIDATE( aAvailableSize >= aNumberLength * 2 * STL_UINT32_SIZE, 
                        aErrorStack );
        
    sEndPos       = aNumber + aNumberLength;
    sHexLength    = 0;
    sOneHexLength = 2 * STL_UINT32_SIZE;
    sHex          = (stlUInt32 *)aHex;
    
    while( aNumber < sEndPos )
    {
        *sHex = dtdHexTable[ (*aNumber >> 4) & 0xF ];
        sHex++;
        *sHex = dtdHexTable[ *aNumber & 0xF ];
        sHex++;
        
        aNumber++;
        sHexLength += sOneHexLength; /* ( 2 * STL_UINT32_SIZE ) */
    }

    STL_DASSERT( sHexLength == aNumberLength * sOneHexLength );

    *aHexLength = sHexLength;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 문자열을 BINARY STRING으로 변환 (BINARY, VARYING BINARY, LONG VARYING BINARY)
 *        16진수들을 10진수들로 변환 (redirect 함수 아님, 내부 함수)
 * @param[in]  aHex           16진수 배열
 * @param[in]  aHexLength     16진수 배열 길이
 * @param[in]  aAvailableSize 10진수 배열이 저장될 메모리 공간의 크기
 * @param[out] aNumber        10진수 배열
 * @param[out] aNumberLength  10진수 배열 길이
 * @param[out] aErrorStack    에러 스택
 *
 * 공백 제거하여 decode 수행.
 */
stlStatus dtdToBinaryStringFromString( stlChar         * aHex,
                                       stlInt64          aHexLength,
                                       stlInt64          aAvailableSize,
                                       stlInt8         * aNumber,
                                       stlInt64        * aNumberLength,
                                       stlErrorStack   * aErrorStack )
{
    const stlChar  * sEndPos;
    stlBool          sIsFirst;
    stlInt8          sNumber;
    stlInt64         sNumberLength;
    
    STL_PARAM_VALIDATE( aHex != NULL, aErrorStack);
    STL_PARAM_VALIDATE( aHexLength >= 0, aErrorStack);
    STL_PARAM_VALIDATE( aNumber != NULL, aErrorStack);

    sEndPos        = aHex + aHexLength;
    sIsFirst       = STL_TRUE;
    sNumberLength  = 0;

    while( aHex < sEndPos )
    {
        if( stlIsspace(*aHex) )
        {
            /* Nothing */
        }
        else
        {
            STL_TRY( dtdGetNumberFromHex( *aHex, & sNumber, aErrorStack )
                     == STL_SUCCESS );
            STL_TRY_THROW( sNumber != -1,
                           ERROR_INVALID_INPUT_DATA );

            if( sIsFirst == STL_TRUE )
            {
                *aNumber = sNumber << 4;
                
                sIsFirst  = STL_FALSE; 
            }
            else
            {
                sNumberLength++;
                STL_PARAM_VALIDATE( sNumberLength <= aAvailableSize, 
                                    aErrorStack );

                *aNumber += sNumber;
                
                (aNumber)++;

                sIsFirst  = STL_TRUE;
            }
        }
        
        aHex++;
    }

    STL_TRY_THROW( sIsFirst == STL_TRUE,
                   ERROR_INVALID_INPUT_DATA_LENGTH );

    *aNumberLength = sNumberLength;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_HEXADECIMAL_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_INVALID_INPUT_DATA_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_HEXADECIMAL_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief unicode( utf16 ) 문자열을 BINARY STRING으로 변환 (BINARY, VARYING BINARY, LONG VARYING BINARY)
 *        16진수들을 10진수들로 변환 (redirect 함수 아님, 내부 함수)
 * @param[in]  aHex           16진수 배열 ( utf16 unicode )
 * @param[in]  aHexLength     16진수 배열 길이
 * @param[in]  aAvailableSize 10진수 배열이 저장될 메모리 공간의 크기
 * @param[out] aNumber        10진수 배열
 * @param[out] aNumberLength  10진수 배열 길이
 * @param[out] aErrorStack    에러 스택
 *
 * 공백 제거하여 decode 수행.
 */
stlStatus dtdToBinaryStringFromUtf16WCharString( stlUInt16          * aHex,
                                                 stlInt64             aHexLength,
                                                 stlInt64             aAvailableSize,
                                                 stlInt8            * aNumber,
                                                 stlInt64           * aNumberLength,
                                                 stlErrorStack      * aErrorStack )
{
    const stlUInt16  * sEndPos;
    stlBool            sIsFirst;
    stlInt8            sNumber;
    stlInt64           sNumberLength;
    
    STL_PARAM_VALIDATE( aHex != NULL, aErrorStack);
    STL_PARAM_VALIDATE( aHexLength >= 0, aErrorStack);
    STL_PARAM_VALIDATE( aNumber != NULL, aErrorStack);

    sEndPos        = aHex + ( aHexLength / STL_UINT16_SIZE );
    sIsFirst       = STL_TRUE;
    sNumberLength  = 0;

    while( aHex < sEndPos )
    {
        if( stlIsspace(*aHex) )
        {
            /* Nothing */
        }
        else
        {
            STL_TRY_THROW( *aHex < 0x80, ERROR_INVALID_INPUT_DATA );

            STL_TRY( dtdGetNumberFromHex( (stlChar)(*aHex), & sNumber, aErrorStack )
                     == STL_SUCCESS );
            STL_TRY_THROW( sNumber != -1,
                           ERROR_INVALID_INPUT_DATA );

            if( sIsFirst == STL_TRUE )
            {
                *aNumber = sNumber << 4;
                
                sIsFirst  = STL_FALSE; 
            }
            else
            {
                sNumberLength++;
                STL_PARAM_VALIDATE( sNumberLength <= aAvailableSize, 
                                    aErrorStack );

                *aNumber += sNumber;
                
                (aNumber)++;

                sIsFirst  = STL_TRUE;
            }
        }
        
        aHex++;
    }

    STL_TRY_THROW( sIsFirst == STL_TRUE,
                   ERROR_INVALID_INPUT_DATA_LENGTH );

    *aNumberLength = sNumberLength;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_HEXADECIMAL_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_INVALID_INPUT_DATA_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_HEXADECIMAL_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief unicode( utf32 ) 문자열을 BINARY STRING으로 변환 (BINARY, VARYING BINARY, LONG VARYING BINARY)
 *        16진수들을 10진수들로 변환 (redirect 함수 아님, 내부 함수)
 * @param[in]  aHex           16진수 배열 ( utf32 unicode )
 * @param[in]  aHexLength     16진수 배열 길이
 * @param[in]  aAvailableSize 10진수 배열이 저장될 메모리 공간의 크기
 * @param[out] aNumber        10진수 배열
 * @param[out] aNumberLength  10진수 배열 길이
 * @param[out] aErrorStack    에러 스택
 *
 * 공백 제거하여 decode 수행.
 */
stlStatus dtdToBinaryStringFromUtf32WCharString( stlUInt32          * aHex,
                                                 stlInt64             aHexLength,
                                                 stlInt64             aAvailableSize,
                                                 stlInt8            * aNumber,
                                                 stlInt64           * aNumberLength,
                                                 stlErrorStack      * aErrorStack )
{
    const stlUInt32  * sEndPos;
    stlBool            sIsFirst;
    stlInt8            sNumber;
    stlInt64           sNumberLength;
    
    STL_PARAM_VALIDATE( aHex != NULL, aErrorStack);
    STL_PARAM_VALIDATE( aHexLength >= 0, aErrorStack);
    STL_PARAM_VALIDATE( aNumber != NULL, aErrorStack);

    sEndPos        = aHex + ( aHexLength / STL_UINT32_SIZE );
    sIsFirst       = STL_TRUE;
    sNumberLength  = 0;

    while( aHex < sEndPos )
    {
        if( stlIsspace(*aHex) )
        {
            /* Nothing */
        }
        else
        {
            STL_TRY_THROW( *aHex < 0x80, ERROR_INVALID_INPUT_DATA );

            STL_TRY( dtdGetNumberFromHex( (stlChar)(*aHex), & sNumber, aErrorStack )
                     == STL_SUCCESS );
            STL_TRY_THROW( sNumber != -1,
                           ERROR_INVALID_INPUT_DATA );

            if( sIsFirst == STL_TRUE )
            {
                *aNumber = sNumber << 4;
                
                sIsFirst  = STL_FALSE; 
            }
            else
            {
                sNumberLength++;
                STL_PARAM_VALIDATE( sNumberLength <= aAvailableSize, 
                                    aErrorStack );

                *aNumber += sNumber;
                
                (aNumber)++;

                sIsFirst  = STL_TRUE;
            }
        }
        
        aHex++;
    }

    STL_TRY_THROW( sIsFirst == STL_TRUE,
                   ERROR_INVALID_INPUT_DATA_LENGTH );

    *aNumberLength = sNumberLength;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_HEXADECIMAL_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_INVALID_INPUT_DATA_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_HEXADECIMAL_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 문자열로부터 Numeric Value를 설정
 * @param[in]  aString             입력 문자열
 * @param[in]  aStringLength       입력 문자열 길이
 * @param[in]  aPrecision          dtlNumericType의 입력 precision
 * @param[in]  aScale              dtlNumericType의 입력 scale
 * @param[out] aNumeric            Numeric Value
 * @param[out] aErrorStack         에러 스택
 *
 * 앞뒤 공백만 제거하여 decode 수행.
 */
stlStatus dtdToNumericFromString( stlChar         * aString,
                                  stlInt64          aStringLength,
                                  stlInt64          aPrecision,
                                  stlInt64          aScale,
                                  dtlDataValue    * aNumeric,
                                  stlErrorStack   * aErrorStack )
{
    stlBool       sIsPositive         = STL_TRUE;
    stlBool       sIsPositiveExponent = STL_TRUE;
    stlChar     * sFirstString        = NULL;
    stlChar     * sLastString         = NULL;
    stlChar     * sTempString         = NULL;

    stlInt32      sExponent           = 0;
    stlInt32      sTempExpo           = 0;
    stlBool       sIsOdd              = STL_FALSE;
    stlBool       sCarry              = STL_FALSE;
    stlBool       sHasDecimalPoint    = STL_FALSE;
    stlInt32      sRoundPos           = 0;
    stlInt32      sNumCnt             = 0;
    stlUInt8      sNumBuf[ DTL_NUMERIC_MAX_DIGIT_COUNT * 2 ] = { 0, };
    stlUInt8    * sNum                = & sNumBuf[1];
    stlUInt8    * sDigits             = NULL;
    stlUInt8      sDigitCnt           = 0;
    stlInt16      sResultExpo         = 0;
    stlInt32      sLeadingZeroNumCnt  = 0;    

    
    dtlNumericType  * sNumeric;
 
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aStringLength >= 0, aErrorStack );
    STL_PARAM_VALIDATE( ( aPrecision >= DTL_NUMERIC_MIN_PRECISION ) &&
                        ( aPrecision <= DTL_NUMERIC_MAX_PRECISION ),
                        aErrorStack );
    STL_PARAM_VALIDATE( (aScale == DTL_SCALE_NA ) || 
                        ( ( aScale >= DTL_NUMERIC_MIN_SCALE ) &&
                          ( aScale <= DTL_NUMERIC_MAX_SCALE ) ),
                        aErrorStack );

    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aNumeric, aErrorStack );

    
    /**************************************
     *  ANALYZE INPUT STRING
     *************************************/

    sNumeric = DTF_NUMERIC( aNumeric );
    
    sFirstString    = aString;
    sLastString     = aString + aStringLength - 1;
    sIsPositive     = STL_TRUE;
    
    /* truncate trailing spaces */
    SKIP_TRAILING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* check sign */
    CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositive );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading zeroes */
    sTempString = sFirstString;
    SKIP_LEADING_ZERO_CHARACTERS( sFirstString, sLastString );
    
    if( sFirstString > sLastString )
    {
        /**
         * 0인 경우
         */
        
        DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength  );
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* ex) '0E+00' */
        sLeadingZeroNumCnt = ( sFirstString - sTempString );
    }

    /**************************************
     *  SET NUM
     *************************************/

    DTL_NUMERIC_RESET_HEADER( sNumeric );

    while( ( sFirstString <= sLastString ) )
    {
        if( DTF_IS_DIGIT( *sFirstString ) )
        {
            if( sNumCnt < 39 )
            {
                *sNum = dtdHexLookup[ (stlUInt8) *sFirstString ];
                sNum++;
                sNumCnt++;
            }
            else
            {
                if( sHasDecimalPoint == STL_FALSE )
                {
                    sExponent++;
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }
        else
        {
            switch( *sFirstString )
            {
                case '.' :
                    {
                        STL_TRY_THROW( sHasDecimalPoint == STL_FALSE,
                                       ERROR_INVALID_INPUT_DATA );
                        
                        if( sNumCnt == 0 )
                        {
                            sFirstString++;
                            SKIP_LEADING_ZERO_CHARACTERS_AFTER_DECIMAL( sFirstString, sLastString, sExponent );
                            sFirstString--;
                            sExponent = -sExponent;
                        }
                        else
                        {
                            sExponent += sNumCnt;
                        }
                        
                        sHasDecimalPoint = STL_TRUE;
                        break;
                    }
                case 'e' :
                case 'E' :
                    {
                        /*
                         * 조건 : ( sLeadingZeroNumCnt > 0 )
                         *        => ex) CAST( '0E+00' AS NUMBER ) => 0
                         *        
                         * 조건 : ( ( sNumCnt == 0 ) && ( *(sFirstString - 2) == '0' ) )
                         *        => ex) CAST( '.0E+00' AS NUMBER ) => 0
                         */ 
                        sFirstString++;
                        STL_TRY_THROW( ( sFirstString <= sLastString ) &&
                                       ( ( sNumCnt > 0 ) || ( sLeadingZeroNumCnt > 0 ) ||
                                         ( ( sNumCnt == 0 ) && ( *(sFirstString - 2) == '0' ) ) ),
                                       ERROR_INVALID_INPUT_DATA );

                        if( sHasDecimalPoint == STL_FALSE )
                        {
                            sExponent += sNumCnt;
                        }
                        else
                        {
                            /* Do Nothing */
                        }

                        STL_THROW( RAMP_CHECK_EXPONENT );
                        break;
                    }
                default :
                    {
                        STL_THROW( ERROR_INVALID_INPUT_DATA );
                        break;
                    }
            }
        }
        
        sFirstString++;
    }

    if( sHasDecimalPoint == STL_FALSE )
    {
        sExponent += sNumCnt;
    }
    else
    {
        /* 소수점이하 tailing zero character 제거. */
        SKIP_TRAILING_ZERO_CHARACTERS( sFirstString, sLastString );
    }
    
    STL_THROW( RAMP_MAKE_NUMERIC );
    
    
    /**************************************
     *  CHECK EXPONENT
     *************************************/
    
    STL_RAMP( RAMP_CHECK_EXPONENT );
    
    /* check exponent sign */
    CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositiveExponent );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );
    
    /* truncate leading zeros on exponent characters */
    SKIP_LEADING_ZERO_CHARACTERS( sFirstString, sLastString );
    
    /* change exponent type string to int64 */
    sTempExpo = 0;
    while( sFirstString <= sLastString )
    {
        STL_TRY_THROW( DTF_IS_DIGIT(*sFirstString),
                       ERROR_INVALID_INPUT_DATA );
        sTempExpo *= 10;
        sTempExpo += dtdHexLookup[ (stlUInt8) *sFirstString ];
        STL_TRY_THROW( sTempExpo <= STL_UINT16_MAX,
                       ERROR_OUT_OF_RANGE );
        sFirstString++;
    }

    if( sIsPositiveExponent == STL_TRUE )
    {
        sExponent += sTempExpo;
    }
    else
    {
        sExponent -= sTempExpo;
    }


    /**************************************
     *  ROUNDING
     *************************************/

    STL_RAMP( RAMP_MAKE_NUMERIC );

    if( aScale == DTL_SCALE_NA )
    {
        STL_TRY_THROW( sExponent <= DTL_NUMBER_MAX_EXPONENT,
                       ERROR_OUT_OF_RANGE );
        sRoundPos = aPrecision ;
    }
    else
    {
        STL_TRY_THROW( sExponent <= aPrecision - aScale,
                       ERROR_OUT_OF_RANGE );
        sRoundPos = sExponent + aScale ;
    }


    if( sRoundPos < 0 )
    {
        DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength);
        STL_THROW( RAMP_FINISH );
    }
    
    if( sRoundPos <= sNumCnt )
    {
        sNum    = & sNumBuf[ sRoundPos ];
        sCarry  = *(sNum + 1 ) >= 5;
        
        while( (sCarry == STL_TRUE) && (sRoundPos > 0) )
        {
            sCarry  = ( ++(*sNum) == 10 );            

            if( sCarry == STL_TRUE )
            {
                (*sNum) = 0;
                sRoundPos--;
                sNum--;                
            }
        }

        if( sRoundPos == 0 )
        {
            sNumCnt = sNum - sNumBuf + 1;
            sNum    = sNumBuf;
            *sNum   = sCarry;
            sExponent++;
        }
        else
        {
            sNumCnt = sNum - sNumBuf;
            sNum    = & sNumBuf[1];
        }

        if( aScale == DTL_SCALE_NA )
        {
            STL_TRY_THROW( sExponent <=  DTL_NUMBER_MAX_EXPONENT,
                           ERROR_OUT_OF_RANGE );
        }
        else
        {
            STL_TRY_THROW( sExponent <= aPrecision - aScale,
                           ERROR_OUT_OF_RANGE );
        }
    }
    else
    {
        sNum = & sNumBuf[1];
    }
    
    
    /**************************************
     *  SET NUMERIC DATA
     *************************************/
    
    sDigits = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
    sIsOdd  = sExponent & 0x01;
    
    *sDigits = 0;
    
    if( sNumCnt > 0 )
    {
        if( sIsOdd == STL_TRUE )
        {
            *sDigits += *sNum;
            sDigits++;
            
            sNumCnt--;
            sNum++;
        }
        else
        {
            /* Do Nothing */
        }
        
        while( sNumCnt >= 2 )
        {
            *sDigits = (*sNum) * 10;
            sNumCnt--;
            sNum++;
            
            *sDigits += *sNum;
            sNumCnt--;
            sNum++;
            
            sDigits++;
        }
        
        if( sNumCnt == 1 )
        {
            *sDigits = (*sNum) * 10;
            sNumCnt--;
            sNum++;
            
            sIsOdd = STL_TRUE;
        }
        else
        {
            sIsOdd = STL_FALSE;
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    STL_DASSERT( sNumCnt == 0 );
    
    sDigitCnt = sDigits - sNumeric->mData - !sIsOdd;
    sDigits = & sNumeric->mData[ sDigitCnt ];
    while( ( sDigitCnt > 0 ) && ( *sDigits == 0 ) )
    {
        sDigitCnt--;
        sDigits--;
    }
    
    aNumeric->mLength = sDigitCnt + 1;
    
    sResultExpo = ( (sExponent + 1) >> 1 ) - 1;
    
    STL_TRY_THROW( sResultExpo <= DTL_NUMERIC_MAX_EXPONENT,
                   ERROR_OUT_OF_RANGE );
        
    if( ( sDigitCnt == 0 ) || ( sResultExpo < DTL_NUMERIC_MIN_EXPONENT ) )
    {
        DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength );
    }
    else
    {
        if( sIsPositive )
        {
            while( sDigitCnt > 0 )
            {
                (*sDigits)++;
                sDigits--;
                sDigitCnt--;
            }

            DTL_NUMERIC_SET_POSITIVE_SIGN( sNumeric );
            DTL_NUMERIC_SET_EXPONENT( sNumeric, STL_FALSE, sResultExpo );
        }
        else
        {
            while( sDigitCnt > 0 )
            {
                *sDigits = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( *sDigits );
                sDigits--;
                sDigitCnt--;
            }
        
            DTL_NUMERIC_SET_NEGATIVE_SIGN( sNumeric );
            DTL_NUMERIC_SET_EXPONENT( sNumeric, STL_TRUE, sResultExpo );
        }
    }

    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;
     
    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_NUMERIC_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_STRING_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Utf16 문자열로부터 Numeric Value를 설정
 * @param[in]  aString             입력 문자열
 * @param[in]  aStringLength       입력 문자열 길이
 * @param[in]  aPrecision          dtlNumericType의 입력 precision
 * @param[in]  aScale              dtlNumericType의 입력 scale
 * @param[out] aNumeric            Numeric Value
 * @param[out] aErrorStack         에러 스택
 *
 * 앞뒤 공백만 제거하여 decode 수행.
 */
stlStatus dtdToNumericFromUtf16WCharString( stlUInt16       * aString,
                                            stlInt64          aStringLength,
                                            stlInt64          aPrecision,
                                            stlInt64          aScale,
                                            dtlDataValue    * aNumeric,
                                            stlErrorStack   * aErrorStack )
{
    stlBool       sIsPositive         = STL_TRUE;
    stlBool       sIsPositiveExponent = STL_TRUE;
    stlUInt16   * sFirstString        = NULL;
    stlUInt16   * sLastString         = NULL;
    stlUInt16   * sTempString         = NULL;

    stlInt32      sExponent           = 0;
    stlInt32      sTempExpo           = 0;
    stlBool       sIsOdd              = STL_FALSE;
    stlBool       sCarry              = STL_FALSE;
    stlBool       sHasDecimalPoint    = STL_FALSE;
    stlInt32      sRoundPos           = 0;
    stlInt32      sNumCnt             = 0;
    stlUInt8      sNumBuf[ DTL_NUMERIC_MAX_DIGIT_COUNT * 2 ] = { 0, };
    stlUInt8    * sNum                = & sNumBuf[1];
    stlUInt8    * sDigits             = NULL;
    stlUInt8      sDigitCnt           = 0;
    stlInt16      sResultExpo         = 0;
    stlInt32      sLeadingZeroNumCnt  = 0;    

    
    dtlNumericType  * sNumeric;
 
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aStringLength >= 0, aErrorStack );
    STL_PARAM_VALIDATE( ( aPrecision >= DTL_NUMERIC_MIN_PRECISION ) && 
                        ( aPrecision <= DTL_NUMERIC_MAX_PRECISION ),
                        aErrorStack );
    STL_PARAM_VALIDATE( (aScale == DTL_SCALE_NA ) || 
                        ( ( aScale >= DTL_NUMERIC_MIN_SCALE ) &&
                          ( aScale <= DTL_NUMERIC_MAX_SCALE ) ),
                        aErrorStack );
    
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aNumeric, aErrorStack );


    /**************************************
     *  ANALYZE INPUT STRING
     *************************************/

    sNumeric = DTF_NUMERIC( aNumeric );
    
    sFirstString    = aString;
    sLastString     = aString + (aStringLength / STL_UINT16_SIZE) - 1;
    sIsPositive     = STL_TRUE;

    /* utf16 string check */
    while( sFirstString <= sLastString )
    {
        if( *sFirstString < 0x80 )
        {
            /* Do Nothing */
        }
        else
        {
            STL_THROW( ERROR_INVALID_INPUT_DATA );
        }
        
        sFirstString++;
    }

    sFirstString    = aString;
    
    /* truncate trailing spaces */
    SKIP_TRAILING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* check sign */
    CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositive );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading zeroes */
    sTempString = sFirstString;
    SKIP_LEADING_ZERO_CHARACTERS( sFirstString, sLastString );
    
    if( sFirstString > sLastString )
    {
        /**
         * 0인 경우
         */
        
        DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength  );
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* ex) '0E+00' */
        sLeadingZeroNumCnt = ( sFirstString - sTempString );
    }

    /**************************************
     *  SET NUM
     *************************************/

    DTL_NUMERIC_RESET_HEADER( sNumeric );

    while( ( sFirstString <= sLastString ) )
    {
        if( DTF_IS_DIGIT( *sFirstString ) )
        {
            if( sNumCnt < 39 )
            {
                *sNum = dtdHexLookup[ (stlUInt8) *sFirstString ];
                sNum++;
                sNumCnt++;
            }
            else
            {
                if( sHasDecimalPoint == STL_FALSE )
                {
                    sExponent++;
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }
        else
        {
            switch( *sFirstString )
            {
                case '.' :
                    {
                        STL_TRY_THROW( sHasDecimalPoint == STL_FALSE,
                                       ERROR_INVALID_INPUT_DATA );
                        
                        if( sNumCnt == 0 )
                        {
                            sFirstString++;
                            SKIP_LEADING_ZERO_CHARACTERS_AFTER_DECIMAL( sFirstString, sLastString, sExponent );
                            sFirstString--;
                            sExponent = -sExponent;
                        }
                        else
                        {
                            sExponent += sNumCnt;
                        }
                        
                        sHasDecimalPoint = STL_TRUE;
                        break;
                    }
                case 'e' :
                case 'E' :
                    {
                        /*
                         * 조건 : ( sLeadingZeroNumCnt > 0 )
                         *        => ex) CAST( '0E+00' AS NUMBER ) => 0
                         *        
                         * 조건 : ( ( sNumCnt == 0 ) && ( *(sFirstString - 2) == '0' ) )
                         *        => ex) CAST( '.0E+00' AS NUMBER ) => 0
                         */ 
                        sFirstString++;
                        STL_TRY_THROW( ( sFirstString <= sLastString ) &&
                                       ( ( sNumCnt > 0 ) || ( sLeadingZeroNumCnt > 0 ) ||
                                         ( ( sNumCnt == 0 ) && ( *(sFirstString - 2) == '0' ) ) ),
                                       ERROR_INVALID_INPUT_DATA );

                        if( sHasDecimalPoint == STL_FALSE )
                        {
                            sExponent += sNumCnt;
                        }
                        else
                        {
                            /* Do Nothing */
                        }

                        STL_THROW( RAMP_CHECK_EXPONENT );
                        break;
                    }
                default :
                    {
                        STL_THROW( ERROR_INVALID_INPUT_DATA );
                        break;
                    }
            }
        }
        
        sFirstString++;
    }

    if( sHasDecimalPoint == STL_FALSE )
    {
        sExponent += sNumCnt;
    }
    else
    {
        /* 소수점이하 tailing zero character 제거. */
        SKIP_TRAILING_ZERO_CHARACTERS( sFirstString, sLastString );
    }
    
    STL_THROW( RAMP_MAKE_NUMERIC );
    
    
    /**************************************
     *  CHECK EXPONENT
     *************************************/
    
    STL_RAMP( RAMP_CHECK_EXPONENT );
    
    /* check exponent sign */
    CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositiveExponent );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );
    
    /* truncate leading zeros on exponent characters */
    SKIP_LEADING_ZERO_CHARACTERS( sFirstString, sLastString );
    
    /* change exponent type string to int64 */
    sTempExpo = 0;
    while( sFirstString <= sLastString )
    {
        STL_TRY_THROW( DTF_IS_DIGIT(*sFirstString),
                       ERROR_INVALID_INPUT_DATA );
        sTempExpo *= 10;
        sTempExpo += dtdHexLookup[ (stlUInt8) *sFirstString ];
        STL_TRY_THROW( sTempExpo <= STL_UINT16_MAX,
                       ERROR_OUT_OF_RANGE );
        sFirstString++;
    }

    if( sIsPositiveExponent == STL_TRUE )
    {
        sExponent += sTempExpo;
    }
    else
    {
        sExponent -= sTempExpo;
    }


    /**************************************
     *  ROUNDING
     *************************************/

    STL_RAMP( RAMP_MAKE_NUMERIC );

    if( aScale == DTL_SCALE_NA )
    {
        STL_TRY_THROW( sExponent <= DTL_NUMBER_MAX_EXPONENT,
                       ERROR_OUT_OF_RANGE );
        sRoundPos = aPrecision ;
    }
    else
    {
        STL_TRY_THROW( sExponent <= aPrecision - aScale,
                       ERROR_OUT_OF_RANGE );
        sRoundPos = sExponent + aScale ;
    }


    if( sRoundPos < 0 )
    {
        DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength);
        STL_THROW( RAMP_FINISH );
    }
    
    if( sRoundPos <= sNumCnt )
    {
        sNum    = & sNumBuf[ sRoundPos ];
        sCarry  = *(sNum + 1 ) >= 5;
        
        while( (sCarry == STL_TRUE) && (sRoundPos > 0) )
        {
            sCarry  = ( ++(*sNum) == 10 );            

            if( sCarry == STL_TRUE )
            {
                (*sNum) = 0;
                sRoundPos--;
                sNum--;                
            }
        }

        if( sRoundPos == 0 )
        {
            sNumCnt = sNum - sNumBuf + 1;
            sNum    = sNumBuf;
            *sNum   = sCarry;
            sExponent++;
        }
        else
        {
            sNumCnt = sNum - sNumBuf;
            sNum    = & sNumBuf[1];
        }

        if( aScale == DTL_SCALE_NA )
        {
            STL_TRY_THROW( sExponent <=  DTL_NUMBER_MAX_EXPONENT,
                           ERROR_OUT_OF_RANGE );
        }
        else
        {
            STL_TRY_THROW( sExponent <= aPrecision - aScale,
                           ERROR_OUT_OF_RANGE );
        }
    }
    else
    {
        sNum = & sNumBuf[1];
    }
    
    
    /**************************************
     *  SET NUMERIC DATA
     *************************************/
    
    sDigits = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
    sIsOdd  = sExponent & 0x01;
    
    *sDigits = 0;
    
    if( sNumCnt > 0 )
    {
        if( sIsOdd == STL_TRUE )
        {
            *sDigits += *sNum;
            sDigits++;
            
            sNumCnt--;
            sNum++;
        }
        else
        {
            /* Do Nothing */
        }
        
        while( sNumCnt >= 2 )
        {
            *sDigits = (*sNum) * 10;
            sNumCnt--;
            sNum++;
            
            *sDigits += *sNum;
            sNumCnt--;
            sNum++;
            
            sDigits++;
        }
        
        if( sNumCnt == 1 )
        {
            *sDigits = (*sNum) * 10;
            sNumCnt--;
            sNum++;
            
            sIsOdd = STL_TRUE;
        }
        else
        {
            sIsOdd = STL_FALSE;
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    STL_DASSERT( sNumCnt == 0 );
    
    sDigitCnt = sDigits - sNumeric->mData - !sIsOdd;
    sDigits = & sNumeric->mData[ sDigitCnt ];
    while( ( sDigitCnt > 0 ) && ( *sDigits == 0 ) )
    {
        sDigitCnt--;
        sDigits--;
    }
    
    aNumeric->mLength = sDigitCnt + 1;
    
    sResultExpo = ( (sExponent + 1) >> 1 ) - 1;
    
    STL_TRY_THROW( sResultExpo <= DTL_NUMERIC_MAX_EXPONENT,
                   ERROR_OUT_OF_RANGE );
        
    if( ( sDigitCnt == 0 ) || ( sResultExpo < DTL_NUMERIC_MIN_EXPONENT ) )
    {
        DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength );
    }
    else
    {
        if( sIsPositive )
        {
            while( sDigitCnt > 0 )
            {
                (*sDigits)++;
                sDigits--;
                sDigitCnt--;
            }

            DTL_NUMERIC_SET_POSITIVE_SIGN( sNumeric );
            DTL_NUMERIC_SET_EXPONENT( sNumeric, STL_FALSE, sResultExpo );
        }
        else
        {
            while( sDigitCnt > 0 )
            {
                *sDigits = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( *sDigits );
                sDigits--;
                sDigitCnt--;
            }
        
            DTL_NUMERIC_SET_NEGATIVE_SIGN( sNumeric );
            DTL_NUMERIC_SET_EXPONENT( sNumeric, STL_TRUE, sResultExpo );
        }
    }

    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;
     
    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_NUMERIC_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_STRING_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Utf32 문자열로부터 Numeric Value를 설정
 * @param[in]  aString             입력 문자열
 * @param[in]  aStringLength       입력 문자열 길이
 * @param[in]  aPrecision          dtlNumericType의 입력 precision
 * @param[in]  aScale              dtlNumericType의 입력 scale
 * @param[out] aNumeric            Numeric Value
 * @param[out] aErrorStack         에러 스택
 *
 * 앞뒤 공백만 제거하여 decode 수행.
 */
stlStatus dtdToNumericFromUtf32WCharString( stlUInt32       * aString,
                                            stlInt64          aStringLength,
                                            stlInt64          aPrecision,
                                            stlInt64          aScale,
                                            dtlDataValue    * aNumeric,
                                            stlErrorStack   * aErrorStack )
{
    stlBool       sIsPositive         = STL_TRUE;
    stlBool       sIsPositiveExponent = STL_TRUE;
    stlUInt32   * sFirstString        = NULL;
    stlUInt32   * sLastString         = NULL;
    stlUInt32   * sTempString         = NULL;

    stlInt32      sExponent           = 0;
    stlInt32      sTempExpo           = 0;
    stlBool       sIsOdd              = STL_FALSE;
    stlBool       sCarry              = STL_FALSE;
    stlBool       sHasDecimalPoint    = STL_FALSE;
    stlInt32      sRoundPos           = 0;
    stlInt32      sNumCnt             = 0;
    stlUInt8      sNumBuf[ DTL_NUMERIC_MAX_DIGIT_COUNT * 2 ] = { 0, };
    stlUInt8    * sNum                = & sNumBuf[1];
    stlUInt8    * sDigits             = NULL;
    stlUInt8      sDigitCnt           = 0;
    stlInt16      sResultExpo         = 0;
    stlInt32      sLeadingZeroNumCnt  = 0;    

    
    dtlNumericType  * sNumeric;
 
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aStringLength >= 0, aErrorStack );
    STL_PARAM_VALIDATE( ( aPrecision >= DTL_NUMERIC_MIN_PRECISION ) && 
                        ( aPrecision <= DTL_NUMERIC_MAX_PRECISION ),
                        aErrorStack );
    STL_PARAM_VALIDATE( (aScale == DTL_SCALE_NA ) || 
                        ( ( aScale >= DTL_NUMERIC_MIN_SCALE ) &&
                          ( aScale <= DTL_NUMERIC_MAX_SCALE ) ),
                        aErrorStack );

    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aNumeric, aErrorStack );


    /**************************************
     *  ANALYZE INPUT STRING
     *************************************/

    sNumeric = DTF_NUMERIC( aNumeric );
    
    sFirstString    = aString;
    sLastString     = aString + (aStringLength / STL_UINT32_SIZE) - 1;
    sIsPositive     = STL_TRUE;

    /* utf32 string check */
    while( sFirstString <= sLastString )
    {
        if( *sFirstString < 0x80 )
        {
            /* Do Nothing */
        }
        else
        {
            STL_THROW( ERROR_INVALID_INPUT_DATA );
        }
        
        sFirstString++;
    }

    sFirstString    = aString;
    
    /* truncate trailing spaces */
    SKIP_TRAILING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* check sign */
    CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositive );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading zeroes */
    sTempString = sFirstString;
    SKIP_LEADING_ZERO_CHARACTERS( sFirstString, sLastString );
    
    if( sFirstString > sLastString )
    {
        /**
         * 0인 경우
         */
        
        DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength  );
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* ex) '0E+00' */
        sLeadingZeroNumCnt = ( sFirstString - sTempString );
    }

    /**************************************
     *  SET NUM
     *************************************/

    DTL_NUMERIC_RESET_HEADER( sNumeric );

    while( ( sFirstString <= sLastString ) )
    {
        if( DTF_IS_DIGIT( *sFirstString ) )
        {
            if( sNumCnt < 39 )
            {
                *sNum = dtdHexLookup[ (stlUInt8) *sFirstString ];
                sNum++;
                sNumCnt++;
            }
            else
            {
                if( sHasDecimalPoint == STL_FALSE )
                {
                    sExponent++;
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }
        else
        {
            switch( *sFirstString )
            {
                case '.' :
                    {
                        STL_TRY_THROW( sHasDecimalPoint == STL_FALSE,
                                       ERROR_INVALID_INPUT_DATA );
                        
                        if( sNumCnt == 0 )
                        {
                            sFirstString++;
                            SKIP_LEADING_ZERO_CHARACTERS_AFTER_DECIMAL( sFirstString, sLastString, sExponent );
                            sFirstString--;
                            sExponent = -sExponent;
                        }
                        else
                        {
                            sExponent += sNumCnt;
                        }
                        
                        sHasDecimalPoint = STL_TRUE;
                        break;
                    }
                case 'e' :
                case 'E' :
                    {
                        /*
                         * 조건 : ( sLeadingZeroNumCnt > 0 )
                         *        => ex) CAST( '0E+00' AS NUMBER ) => 0
                         *        
                         * 조건 : ( ( sNumCnt == 0 ) && ( *(sFirstString - 2) == '0' ) )
                         *        => ex) CAST( '.0E+00' AS NUMBER ) => 0
                         */ 
                        sFirstString++;
                        STL_TRY_THROW( ( sFirstString <= sLastString ) &&
                                       ( ( sNumCnt > 0 ) || ( sLeadingZeroNumCnt > 0 ) ||
                                         ( ( sNumCnt == 0 ) && ( *(sFirstString - 2) == '0' ) ) ),
                                       ERROR_INVALID_INPUT_DATA );

                        if( sHasDecimalPoint == STL_FALSE )
                        {
                            sExponent += sNumCnt;
                        }
                        else
                        {
                            /* Do Nothing */
                        }

                        STL_THROW( RAMP_CHECK_EXPONENT );
                        break;
                    }
                default :
                    {
                        STL_THROW( ERROR_INVALID_INPUT_DATA );
                        break;
                    }
            }
        }
        
        sFirstString++;
    }

    if( sHasDecimalPoint == STL_FALSE )
    {
        sExponent += sNumCnt;
    }
    else
    {
        /* 소수점이하 tailing zero character 제거. */
        SKIP_TRAILING_ZERO_CHARACTERS( sFirstString, sLastString );
    }
    
    STL_THROW( RAMP_MAKE_NUMERIC );
    
    
    /**************************************
     *  CHECK EXPONENT
     *************************************/
    
    STL_RAMP( RAMP_CHECK_EXPONENT );
    
    /* check exponent sign */
    CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositiveExponent );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );
    
    /* truncate leading zeros on exponent characters */
    SKIP_LEADING_ZERO_CHARACTERS( sFirstString, sLastString );
    
    /* change exponent type string to int64 */
    sTempExpo = 0;
    while( sFirstString <= sLastString )
    {
        STL_TRY_THROW( DTF_IS_DIGIT(*sFirstString),
                       ERROR_INVALID_INPUT_DATA );
        sTempExpo *= 10;
        sTempExpo += dtdHexLookup[ (stlUInt8) *sFirstString ];
        STL_TRY_THROW( sTempExpo <= STL_UINT16_MAX,
                       ERROR_OUT_OF_RANGE );
        sFirstString++;
    }

    if( sIsPositiveExponent == STL_TRUE )
    {
        sExponent += sTempExpo;
    }
    else
    {
        sExponent -= sTempExpo;
    }


    /**************************************
     *  ROUNDING
     *************************************/

    STL_RAMP( RAMP_MAKE_NUMERIC );

    if( aScale == DTL_SCALE_NA )
    {
        STL_TRY_THROW( sExponent <= DTL_NUMBER_MAX_EXPONENT,
                       ERROR_OUT_OF_RANGE );
        sRoundPos = aPrecision ;
    }
    else
    {
        STL_TRY_THROW( sExponent <= aPrecision - aScale,
                       ERROR_OUT_OF_RANGE );
        sRoundPos = sExponent + aScale ;
    }


    if( sRoundPos < 0 )
    {
        DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength);
        STL_THROW( RAMP_FINISH );
    }
    
    if( sRoundPos <= sNumCnt )
    {
        sNum    = & sNumBuf[ sRoundPos ];
        sCarry  = *(sNum + 1 ) >= 5;
        
        while( (sCarry == STL_TRUE) && (sRoundPos > 0) )
        {
            sCarry  = ( ++(*sNum) == 10 );            

            if( sCarry == STL_TRUE )
            {
                (*sNum) = 0;
                sRoundPos--;
                sNum--;                
            }
        }

        if( sRoundPos == 0 )
        {
            sNumCnt = sNum - sNumBuf + 1;
            sNum    = sNumBuf;
            *sNum   = sCarry;
            sExponent++;
        }
        else
        {
            sNumCnt = sNum - sNumBuf;
            sNum    = & sNumBuf[1];
        }

        if( aScale == DTL_SCALE_NA )
        {
            STL_TRY_THROW( sExponent <=  DTL_NUMBER_MAX_EXPONENT,
                           ERROR_OUT_OF_RANGE );
        }
        else
        {
            STL_TRY_THROW( sExponent <= aPrecision - aScale,
                           ERROR_OUT_OF_RANGE );
        }
    }
    else
    {
        sNum = & sNumBuf[1];
    }
    
    
    /**************************************
     *  SET NUMERIC DATA
     *************************************/
    
    sDigits = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
    sIsOdd  = sExponent & 0x01;
    
    *sDigits = 0;
    
    if( sNumCnt > 0 )
    {
        if( sIsOdd == STL_TRUE )
        {
            *sDigits += *sNum;
            sDigits++;
            
            sNumCnt--;
            sNum++;
        }
        else
        {
            /* Do Nothing */
        }
        
        while( sNumCnt >= 2 )
        {
            *sDigits = (*sNum) * 10;
            sNumCnt--;
            sNum++;
            
            *sDigits += *sNum;
            sNumCnt--;
            sNum++;
            
            sDigits++;
        }
        
        if( sNumCnt == 1 )
        {
            *sDigits = (*sNum) * 10;
            sNumCnt--;
            sNum++;
            
            sIsOdd = STL_TRUE;
        }
        else
        {
            sIsOdd = STL_FALSE;
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    STL_DASSERT( sNumCnt == 0 );
    
    sDigitCnt = sDigits - sNumeric->mData - !sIsOdd;
    sDigits = & sNumeric->mData[ sDigitCnt ];
    while( ( sDigitCnt > 0 ) && ( *sDigits == 0 ) )
    {
        sDigitCnt--;
        sDigits--;
    }
    
    aNumeric->mLength = sDigitCnt + 1;
    
    sResultExpo = ( (sExponent + 1) >> 1 ) - 1;
    
    STL_TRY_THROW( sResultExpo <= DTL_NUMERIC_MAX_EXPONENT,
                   ERROR_OUT_OF_RANGE );
        
    if( ( sDigitCnt == 0 ) || ( sResultExpo < DTL_NUMERIC_MIN_EXPONENT ) )
    {
        DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength );
    }
    else
    {
        if( sIsPositive )
        {
            while( sDigitCnt > 0 )
            {
                (*sDigits)++;
                sDigits--;
                sDigitCnt--;
            }

            DTL_NUMERIC_SET_POSITIVE_SIGN( sNumeric );
            DTL_NUMERIC_SET_EXPONENT( sNumeric, STL_FALSE, sResultExpo );
        }
        else
        {
            while( sDigitCnt > 0 )
            {
                *sDigits = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( *sDigits );
                sDigits--;
                sDigitCnt--;
            }
        
            DTL_NUMERIC_SET_NEGATIVE_SIGN( sNumeric );
            DTL_NUMERIC_SET_EXPONENT( sNumeric, STL_TRUE, sResultExpo );
        }
    }

    STL_RAMP( RAMP_FINISH );


    return STL_SUCCESS;
     
    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_NUMERIC_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_STRING_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief integer64 로부터 Numeric Value를 설정
 * @param[in]  aInteger            입력 값
 * @param[in]  aPrecision          dtlNumericType의 입력 precision
 * @param[in]  aScale              dtlNumericType의 입력 scale
 * @param[out] aNumeric            Numeric Value
 * @param[out] aErrorStack         에러 스택
 */
stlStatus dtdToNumericFromInt64( stlInt64          aInteger,
                                 stlInt64          aPrecision,
                                 stlInt64          aScale,
                                 dtlDataValue    * aNumeric,
                                 stlErrorStack   * aErrorStack )
{
    stlInt64          sBigInt;
    stlInt64          sDestPrecision;
    stlInt64          sDestScale;
    dtlNumericType  * sNumeric;

    stlBool           sIsNegative;
    stlInt32          sExponent;
    stlUInt8          sTmpDigit[DTL_NUMERIC_MAX_DIGIT_COUNT];
    stlUInt8        * sDigitBeginPtr;
    stlUInt8        * sDigitEndPtr;
    stlUInt8        * sDestPtr;
    stlUInt8          sDigitCount;

    stlUInt64         sNum;
    stlUInt64         sQuot;
    stlUInt64         sRem;

    STL_PARAM_VALIDATE( ( aPrecision >= DTL_NUMERIC_MIN_PRECISION ) && 
                        ( aPrecision <= DTL_NUMERIC_MAX_PRECISION ),
                        aErrorStack );
    STL_PARAM_VALIDATE( ( aScale == DTL_SCALE_NA ) || 
                        ( ( aScale >= DTL_NUMERIC_MIN_SCALE ) &&
                          ( aScale <= DTL_NUMERIC_MAX_SCALE ) ),
                        aErrorStack );
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aNumeric, aErrorStack );


    /**
     * Value 설정
     */

    sBigInt        = aInteger;
    sDestPrecision = aPrecision;
    sDestScale     = aScale;
    sNumeric       = DTF_NUMERIC( aNumeric );
    

    /**
     * Convert BigInt To Numeric
     */

    if( sBigInt < 0 )
    {
        /**
         * Negative BigInt
         */

        sIsNegative = STL_TRUE;
        sQuot = (stlUInt64)(-(sBigInt + 1)) + 1;
        sExponent = 0;
    }
    else
    {
        /**
         * Positive BigInt
         */

        sIsNegative = STL_FALSE;
        sQuot = sBigInt;
        sExponent = 0;
    }


    if( sDestScale == DTL_SCALE_NA ) 
    {
        sRem = 0;
        sDestScale = 0;
        while( sQuot >= STL_POW_10( sDestPrecision ) )
        {
            sNum = sQuot;
            sQuot = sNum / 10;
            sRem = sNum - sQuot * 10;
            sDestScale--;
        }


        /**
         * 반올림 처리
         */

        if( sRem > 4 )
        {
            sQuot++;
        }

        if( sQuot >= STL_POW_10( sDestPrecision ) )
        {
            sQuot /= 10;
            sDestScale--;
        }

        /**
         * Exponent 설정
         */
        
        sExponent = ((-sDestScale) / 2);
        
        if( -sDestScale != sExponent * 2 )
        {
            sQuot *= 10;
        }
    }
    else
    {
        /**
         * Out of Range 체크
         */

        STL_TRY_THROW( sQuot < STL_POW_10( sDestPrecision - sDestScale ),
                       ERROR_OUT_OF_RANGE );


        /**
         * Scale 값이 음수인 경우 자리 조정이 필요하다.
         */

        if( sDestScale < 0 )
        {
            sQuot /= STL_POW_10( -1 - sDestScale );
            sQuot += 5;
            sQuot /= 10;


            /**
             * Exponent 설정
             */

            sExponent = ((-sDestScale) / 2);

            if( -sDestScale != sExponent * 2 )
            {
                sQuot *= 10;
            }
        }
    }


    /**
     * 0인 경우 별도 처리한다.
     * ----------------------
     *  if문 다음부터는 0이 아니다는 가정하에 코딩되어 있다.
     */

    if( sQuot == 0 )
    {
        DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength );
        STL_THROW( RAMP_FINISH );
    }


    /**
     * Digit 변환
     */

    sDigitBeginPtr = &sTmpDigit[DTL_NUMERIC_MAX_DIGIT_COUNT - 1];
    sDigitCount = 0;
    while( sQuot > 0 )
    {
        sNum = sQuot;
        sQuot = sNum / 100; /* sNum * 0.01로 하면 반올림을 해서 결과가 틀려진다. */
        *sDigitBeginPtr = (stlUInt8)(sNum - ( sQuot * 100 ));
        sDigitBeginPtr--;
    }


    /**
     * Trailing Zero 제거
     */

    sDigitEndPtr = &sTmpDigit[DTL_NUMERIC_MAX_DIGIT_COUNT - 1];
    while( (sDigitBeginPtr < sDigitEndPtr) && (*sDigitEndPtr == 0) )
    {
        sDigitEndPtr--;
        sExponent++;
    }


    /**
     * Numeric 데이터 생성
     */

    sDigitCount = sDigitEndPtr - sDigitBeginPtr;
    sExponent += (sDigitCount - 1);
    sDigitBeginPtr++;
    sDestPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

    if( sIsNegative == STL_TRUE )
    {
        /**
         * 음수
         */

        DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sNumeric, sExponent );

        while( sDigitBeginPtr <= sDigitEndPtr )
        {
            *sDestPtr = DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( *sDigitBeginPtr );
            sDigitBeginPtr++;
            sDestPtr++;
        }

        aNumeric->mLength = sDigitCount + 1;
    }
    else
    {
        /**
         * 양수
         */

        DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sNumeric, sExponent );

        while( sDigitBeginPtr <= sDigitEndPtr )
        {
            *sDestPtr = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( *sDigitBeginPtr );
            sDigitBeginPtr++;
            sDestPtr++;
        }

        aNumeric->mLength = sDigitCount + 1;
    }

    STL_RAMP( RAMP_FINISH );
    
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }
        
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief unsigned integer64 로부터 Numeric Value를 설정
 * @param[in]  aUInt64             입력 값
 * @param[in]  aPrecision          dtlNumericType의 입력 precision
 * @param[in]  aScale              dtlNumericType의 입력 scale
 * @param[out] aNumeric            Numeric Value
 * @param[out] aErrorStack         에러 스택
 */
stlStatus dtdToNumericFromUInt64( stlUInt64         aUInt64,
                                  stlInt64          aPrecision,
                                  stlInt64          aScale,
                                  dtlDataValue    * aNumeric,
                                  stlErrorStack   * aErrorStack )
{
    stlUInt64         sUInt64;
    stlInt64          sDestPrecision;
    stlInt64          sDestScale;
    dtlNumericType  * sNumeric;
    
    stlInt32          sExponent;
    stlUInt8          sTmpDigit[DTL_NUMERIC_MAX_DIGIT_COUNT];
    stlUInt8        * sDigitBeginPtr;
    stlUInt8        * sDigitEndPtr;
    stlUInt8        * sDestPtr;
    stlUInt8          sDigitCount;

    stlUInt64         sNum;
    stlUInt64         sQuot;
    stlUInt64         sRem;

    STL_PARAM_VALIDATE( ( aPrecision >= DTL_NUMERIC_MIN_PRECISION ) && 
                        ( aPrecision <= DTL_NUMERIC_MAX_PRECISION ),
                        aErrorStack );
    STL_PARAM_VALIDATE( ( aScale == DTL_SCALE_NA ) || 
                        ( ( aScale >= DTL_NUMERIC_MIN_SCALE ) &&
                          ( aScale <= DTL_NUMERIC_MAX_SCALE ) ),
                        aErrorStack );
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aNumeric, aErrorStack );


    /**
     * Value 설정
     */
    
    sUInt64        = aUInt64;
    sDestPrecision = aPrecision;
    sDestScale     = aScale;
    sNumeric       = DTF_NUMERIC( aNumeric );
    

    /**
     * Convert UInt64 To Numeric
     */

    sQuot = sUInt64;
    sExponent = 0;

    if( sDestScale == DTL_SCALE_NA ) 
    {
        sRem = 0;
        sDestScale = 0;
        while( sQuot >= STL_POW_10( sDestPrecision ) )
        {
            sNum = sQuot;
            sQuot = sNum / 10;
            sRem = sNum - sQuot * 10;
            sDestScale--;
        }


        /**
         * 반올림 처리
         */

        if( sRem > 4 )
        {
            sQuot++;
        }

        if( sQuot >= STL_POW_10( sDestPrecision ) )
        {
            sQuot /= 10;
            sDestScale--;
        }

        /**
         * Exponent 설정
         */
        
        sExponent = ((-sDestScale) / 2);
        
        if( -sDestScale != sExponent * 2 )
        {
            sQuot *= 10;
        }
    }
    else
    {
        /**
         * Out of Range 체크
         */

        STL_TRY_THROW( sQuot < STL_POW_10( sDestPrecision - sDestScale ),
                       ERROR_OUT_OF_RANGE );


        /**
         * Scale 값이 음수인 경우 자리 조정이 필요하다.
         */

        if( sDestScale < 0 )
        {
            sQuot /= STL_POW_10( -1 - sDestScale );
            sQuot += 5;
            sQuot /= 10;


            /**
             * Exponent 설정
             */

            sExponent = ((-sDestScale) / 2);

            if( -sDestScale != sExponent * 2 )
            {
                sQuot *= 10;
            }
        }
    }


    /**
     * 0인 경우 별도 처리한다.
     * ----------------------
     *  if문 다음부터는 0이 아니다는 가정하에 코딩되어 있다.
     */

    if( sQuot == 0 )
    {
        DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength );
        STL_THROW( RAMP_FINISH );
    }


    /**
     * Digit 변환
     */

    sDigitBeginPtr = &sTmpDigit[DTL_NUMERIC_MAX_DIGIT_COUNT - 1];
    sDigitCount = 0;
    while( sQuot > 0 )
    {
        sNum = sQuot;
        sQuot = sNum / 100; /* sNum * 0.01로 하면 반올림을 해서 결과가 틀려진다. */
        *sDigitBeginPtr = (stlUInt8)(sNum - ( sQuot * 100 ));
        sDigitBeginPtr--;
    }


    /**
     * Trailing Zero 제거
     */

    sDigitEndPtr = &sTmpDigit[DTL_NUMERIC_MAX_DIGIT_COUNT - 1];
    while( (sDigitBeginPtr < sDigitEndPtr) && (*sDigitEndPtr == 0) )
    {
        sDigitEndPtr--;
        sExponent++;
    }


    /**
     * Numeric 데이터 생성
     */

    sDigitCount = sDigitEndPtr - sDigitBeginPtr;
    sExponent += (sDigitCount - 1);
    sDigitBeginPtr++;
    sDestPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

    DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sNumeric, sExponent );
    
    while( sDigitBeginPtr <= sDigitEndPtr )
    {
        *sDestPtr = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( *sDigitBeginPtr );
        sDigitBeginPtr++;
        sDestPtr++;
    }
    
    aNumeric->mLength = sDigitCount + 1;
    
    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }
        
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief float32 로부터 Numeric Value를 설정
 * @param[in]  aFloat32            입력 값
 * @param[in]  aPrecision          dtlNumericType의 입력 precision
 * @param[in]  aScale              dtlNumericType의 입력 scale
 * @param[out] aNumeric            Numeric Value
 * @param[out] aErrorStack         에러 스택
 *
 * 앞뒤 공백만 제거하여 decode 수행.
 */
stlStatus dtdToNumericFromFloat32( stlFloat32        aFloat32,
                                   stlInt64          aPrecision,
                                   stlInt64          aScale,
                                   dtlDataValue    * aNumeric,
                                   stlErrorStack   * aErrorStack )
{
    stlFloat64    sFloat64;
    stlFloat64    sRoundFloat64;
    stlFloat64    sPow;

    stlInt32      sDigitValue;
    stlInt32      sMinExponent;
    stlInt32      sMaxExponent;
    stlInt32      sDigitLength;
    stlInt32      sDestScale;
    stlInt32      sQuot;
    
    stlBool       sIsPositive;
    stlBool       sIsLastDigit;

    stlUInt8    * sDigit;
    stlUInt8      sValue;
    stlInt32      sLoop;

    stlInt32      sDigitValueCount;
    stlInt32      sFloatValueCount;

    dtlNumericType  * sNumeric;
    
    STL_PARAM_VALIDATE( ( aPrecision >= DTL_NUMERIC_MIN_PRECISION ) && 
                        ( aPrecision <= DTL_NUMERIC_MAX_PRECISION ),
                        aErrorStack );
    STL_PARAM_VALIDATE( ( aScale == DTL_SCALE_NA ) ||
                        ( ( aScale >= DTL_NUMERIC_MIN_SCALE ) &&
                          ( aScale <= DTL_NUMERIC_MAX_SCALE ) ),
                        aErrorStack );
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aNumeric, aErrorStack );


    sNumeric = DTF_NUMERIC( aNumeric );
        
    STL_DASSERT( stlIsinfinite( aFloat32 ) == STL_IS_INFINITE_FALSE );
    STL_DASSERT( stlIsnan( aFloat32 ) == STL_IS_NAN_FALSE );

    if( aFloat32 == 0.0 )
    {
        /* set zero */
        DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength );
    }
    else
    {
        if( aFloat32 < 0 )
        {
            sFloat64 = -aFloat32;
            sIsPositive = STL_FALSE;
        }
        else
        {
            sFloat64 = aFloat32;
            sIsPositive = STL_TRUE;
        }
        
        sMaxExponent = stlLog10( sFloat64 );

        STL_TRY_THROW( stlGet10Pow( sMaxExponent - STL_FLT_DIG - 1, & sPow, aErrorStack ) == STL_TRUE,
                       ERROR_OUT_OF_RANGE );
        
        sRoundFloat64 = sFloat64 + sPow * 5;

        if( sRoundFloat64 < 1 )
        {
            STL_TRY_THROW( stlGet10Pow( sMaxExponent, &sPow, aErrorStack ) == STL_TRUE,
                           ERROR_OUT_OF_RANGE );
            if( sRoundFloat64 == sPow )
            {
                /*
                 * 1E-XX
                 * ( 예: 0.1, 0.01, 0.001 ..... )
                 */

                sFloat64 = sRoundFloat64;
            }
            else
            {
                /*
                 * 1이 아닌 XE-XX
                 * ( 예: 0.2, 0.3, ... )
                 */
                if( sMaxExponent == (stlInt32) stlLog10( sRoundFloat64 ) )
                {
                    sMaxExponent--;
                }
                else
                {
                    sFloat64 = sRoundFloat64;
                }
            }
            
        }
        else
        {
            if( sMaxExponent == (stlInt32) stlLog10( sRoundFloat64 ) )
            {
                /* Do Nothing */
            }
            else
            {
                sFloat64 = sRoundFloat64;
                sMaxExponent++;
            }
        }               

        
        /**
         * numeric의 digit 구성 부분 얻기
         */
        
        if( aScale == DTL_SCALE_NA )
        {
            /* float32는 DTL_NUMBER_MAX_EXPONENT 보다 작은 값이다. */
            /* STL_TRY_THROW( sMaxExponent <= DTL_NUMBER_MAX_EXPONENT, */
            /*                ERROR_OUT_OF_RANGE ); */
            
            aPrecision   = ( aPrecision > STL_FLT_DIG ? STL_FLT_DIG : aPrecision );
            sMinExponent = sMaxExponent - aPrecision;
        }
        else
        {
            STL_TRY_THROW( sMaxExponent < aPrecision - aScale,
                           ERROR_OUT_OF_RANGE );
        
            sDestScale   = -aScale - 1;
            aPrecision   = ( aPrecision > STL_FLT_DIG ? STL_FLT_DIG : aPrecision );
            sMinExponent = sMaxExponent - aPrecision;
        
            if( sMinExponent < sDestScale )
            {
                aPrecision   += ( sMinExponent - sDestScale );
                sMinExponent = sDestScale;
            }
            else
            {
                /* Do Nothing */
            }
        }
        
        STL_TRY_THROW( stlGet10Pow( -sMinExponent, & sPow, aErrorStack ) == STL_TRUE,
                       ERROR_OUT_OF_RANGE );

        sDigitValue = ( ((stlInt32) ( sFloat64 * sPow )) + 5 ) / 10;
        
        /**
         * 반올림되어서 sDigitValue의 자리수가 하나 늘어나는 경우에 대한 처리
         */

        sDigitValueCount = stlLog10(sDigitValue) + 1;
        sFloatValueCount = sMaxExponent - sMinExponent;

        if( sDigitValueCount > sFloatValueCount )
        {
            aPrecision++;
            sMaxExponent++;
        }
        
        if( aPrecision <= 0 )
        {
            /* set zero */
            DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength );
            STL_THROW( RAMP_FINISH );
        }

        STL_TRY_THROW( sDigitValue < gPreDefinedPow[ aPrecision ],
                       ERROR_OUT_OF_RANGE );

        
        /**
         * Zero 체크
         */

        if( sDigitValue == 0 )
        {
            /* set zero */
            DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength );
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            /* Do Nothing */
        }

        
        /**
         * Numeric 구성
         */

        sDigit = sNumeric->mData;
        sIsLastDigit = STL_TRUE;
        
        if( sMinExponent & 0x01 )
        {
            /* 맨마지막 digit이 두자리 수 */
            sDigitLength = ( aPrecision + 1 ) >> 1;
        }
        else
        {
            /* 맨마지막 digit이 한자리 수 */
            sDigitLength = ( aPrecision >> 1 ) + 1;
            sDigitValue *= 10;
        }

        if( sIsPositive )
        {
            for( sLoop = sDigitLength ; sLoop > 0 ; sLoop-- )
            {
                sQuot  = sDigitValue / 100;
                sValue = sDigitValue - ( sQuot * 100 );

                if( ( sIsLastDigit ) && ( sValue == 0 ) )
                {
                    sDigitLength--;
                }
                else
                {
                    sIsLastDigit = STL_FALSE;
                    sDigit[ sLoop ] =
                        DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( sValue );
                }
                
                sDigitValue = sQuot;
            }
            DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sNumeric, sMaxExponent >> 1 );
        }
        else
        {
            for( sLoop = sDigitLength ; sLoop > 0 ; sLoop-- )
            {
                sQuot  = sDigitValue / 100;
                sValue = sDigitValue - ( sQuot * 100 );

                if( ( sIsLastDigit ) && ( sValue == 0 ) )
                {
                    sDigitLength--;
                }
                else
                {
                    sIsLastDigit = STL_FALSE;
                    sDigit[ sLoop ] =
                        DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( sValue );
                }
                
                sDigitValue = sQuot;
            }
            DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sNumeric, sMaxExponent >> 1 );
        }

        aNumeric->mLength = sDigitLength + 1;
    }

    
    STL_RAMP( RAMP_FINISH );
    
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }
        
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief float64 로부터 Numeric Value를 설정
 * @param[in]  aFloat64            입력 값
 * @param[in]  aPrecision          dtlNumericType의 입력 precision
 * @param[in]  aScale              dtlNumericType의 입력 scale
 * @param[out] aNumeric            Numeric Value
 * @param[out] aErrorStack         에러 스택
 *
 * 앞뒤 공백만 제거하여 decode 수행.
 */
stlStatus dtdToNumericFromFloat64( stlFloat64        aFloat64,
                                   stlInt64          aPrecision,
                                   stlInt64          aScale,
                                   dtlDataValue    * aNumeric,
                                   stlErrorStack   * aErrorStack )
{
    stlFloat64    sFloat64;
    stlFloat64    sRoundFloat64;
    stlFloat64    sPow;
    
    stlInt64      sDigitValue;
    stlInt32      sMinExponent;
    stlInt32      sMaxExponent;
    stlInt32      sDigitLength;
    stlInt64      sDestScale;
    stlInt64      sQuot;
    
    stlBool       sIsPositive;
    stlBool       sIsLastDigit;

    stlUInt8    * sDigit;
    stlUInt8      sValue;
    stlInt32      sLoop;

    stlInt32      sDigitValueCount;
    stlInt32      sFloatValueCount;

    dtlNumericType  * sNumeric;
    
    STL_PARAM_VALIDATE( ( aPrecision >= DTL_NUMERIC_MIN_PRECISION ) && 
                        ( aPrecision <= DTL_NUMERIC_MAX_PRECISION ),
                        aErrorStack );
    STL_PARAM_VALIDATE( ( aScale == DTL_SCALE_NA ) ||
                        ( ( aScale >= DTL_NUMERIC_MIN_SCALE ) &&
                          ( aScale <= DTL_NUMERIC_MAX_SCALE ) ),
                        aErrorStack );
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aNumeric, aErrorStack );


    sNumeric = DTF_NUMERIC( aNumeric );
    
    STL_DASSERT( stlIsinfinite( aFloat64 ) == STL_IS_INFINITE_FALSE );
    STL_DASSERT( stlIsnan( aFloat64 ) == STL_IS_NAN_FALSE );

    if( aFloat64 == 0.0 )
    {
        /* set zero */
        DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength );
    }
    else
    {
        if( aFloat64 < 0 )
        {
            sFloat64 = -aFloat64;
            sIsPositive = STL_FALSE;
        }
        else
        {
            sFloat64 = aFloat64;
            sIsPositive = STL_TRUE;
        }
        
        sMaxExponent = stlLog10( sFloat64 );

        /* 1차로 numeric의 min 범위를 검사해서 벗어나는 경우, numeric 0 의 값으로 반환한다. */
        if( sMaxExponent < (DTL_NUMERIC_MIN_EXPONENT * 2) )
        {
            /* set zero */
            DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength );
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            /* Do Nothing */
        }

        STL_TRY_THROW( stlGet10Pow( sMaxExponent - STL_DBL_DIG - 1, & sPow, aErrorStack ) == STL_TRUE,
                       ERROR_OUT_OF_RANGE );

        sRoundFloat64 = sFloat64 + sPow * 5;
        
        if( sRoundFloat64 < 1 )
        {
            STL_TRY_THROW( stlGet10Pow( sMaxExponent, &sPow, aErrorStack ) == STL_TRUE,
                           ERROR_OUT_OF_RANGE );
            if( sRoundFloat64 == sPow )
            {
                /*
                 * 1E-XX
                 * ( 예: 0.1, 0.01, 0.001 ..... )
                 */
                
                sFloat64 = sRoundFloat64;
            }
            else
            {
                /*
                 * 1이 아닌 XE-XX
                 * ( 예: 0.2, 0.3, ... )
                 */
                if( sMaxExponent == (stlInt32) stlLog10( sRoundFloat64 ) )
                {
                    sMaxExponent--;
                }
                else
                {
                    sFloat64 = sRoundFloat64;
                }
            }
        }
        else
        {
            if( sMaxExponent == (stlInt32) stlLog10( sRoundFloat64 ) )
            {
                /* Do Nothing */
            }
            else
            {
                sFloat64 = sRoundFloat64;
                sMaxExponent++;
            }
        }

        
        /**
         * numeric의 digit 구성 부분 얻기
         */
        
        if( aScale == DTL_SCALE_NA )
        {
            STL_TRY_THROW( sMaxExponent < DTL_NUMBER_MAX_EXPONENT,
                           ERROR_OUT_OF_RANGE );
            
            aPrecision   = ( aPrecision > STL_DBL_DIG ? STL_DBL_DIG : aPrecision );
            sMinExponent = sMaxExponent - aPrecision;
        }
        else
        {
            STL_TRY_THROW( sMaxExponent < aPrecision - aScale,
                           ERROR_OUT_OF_RANGE );

            sDestScale   = -aScale - 1;
            aPrecision   = ( aPrecision > STL_DBL_DIG ? STL_DBL_DIG : aPrecision );
            sMinExponent = sMaxExponent - aPrecision;

            if( sMinExponent < sDestScale )
            {
                aPrecision   += ( sMinExponent - sDestScale );
                sMinExponent = sDestScale;
            }
            else
            {
                /* Do Nothing */
            }
        }

        STL_TRY_THROW( stlGet10Pow( -sMinExponent, & sPow, aErrorStack ) == STL_TRUE,
                       ERROR_OUT_OF_RANGE );
        
        sDigitValue = ( ((stlInt64) ( sFloat64 * sPow )) + 5 ) / 10;
        
        /**
         * 반올림되어서 sDigitValue의 자리수가 하나 늘어나는 경우에 대한 처리
         */

        sDigitValueCount = stlLog10(sDigitValue) + 1;
        sFloatValueCount = sMaxExponent - sMinExponent;

        if( sDigitValueCount > sFloatValueCount )
        {
            aPrecision++;
            sMaxExponent++;
        }
        
        if( aPrecision <= 0 )
        {
            /* set zero */
            DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength );
            STL_THROW( RAMP_FINISH );
        }
        
        STL_TRY_THROW( sDigitValue < gPreDefinedPow[ aPrecision ],
                       ERROR_OUT_OF_RANGE );
        
        
        /**
         * Zero 체크
         */
        
        if( ( sDigitValue == 0 ) || ( sMaxExponent < ( DTL_NUMERIC_MIN_EXPONENT * 2 ) ) )
        {
            /* set zero */
            DTL_NUMERIC_SET_ZERO( sNumeric, aNumeric->mLength );
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            /* Do Nothing */
        }

        
        /**
         * Numeric 구성
         */

        sDigit = sNumeric->mData;
        sIsLastDigit = STL_TRUE;
        
        if( sMinExponent & 0x01 )
        {
            /* 맨마지막 digit이 두자리 수 */
            sDigitLength = ( aPrecision + 1 ) >> 1;
        }
        else
        {
            /* 맨마지막 digit이 한자리 수 */
            sDigitLength = ( aPrecision >> 1 ) + 1;
            sDigitValue *= 10;
        }

        if( sIsPositive )
        {
            for( sLoop = sDigitLength ; sLoop > 0 ; sLoop-- )
            {
                sQuot  = sDigitValue / 100;
                sValue = sDigitValue - ( sQuot * 100 );

                if( ( sIsLastDigit ) && ( sValue == 0 ) )
                {
                    sDigitLength--;
                }
                else
                {
                    sIsLastDigit = STL_FALSE;
                    sDigit[ sLoop ] =
                        DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( sValue );
                }
                
                sDigitValue = sQuot;
            }
            DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sNumeric, sMaxExponent >> 1 );
        }
        else
        {
            for( sLoop = sDigitLength ; sLoop > 0 ; sLoop-- )
            {
                sQuot  = sDigitValue / 100;
                sValue = sDigitValue - ( sQuot * 100 );

                if( ( sIsLastDigit ) && ( sValue == 0 ) )
                {
                    sDigitLength--;
                }
                else
                {
                    sIsLastDigit = STL_FALSE;
                    sDigit[ sLoop ] =
                        DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( sValue );
                }
                
                sDigitValue = sQuot;
            }
            DTL_NUMERIC_SET_SIGN_NEGATIVE_AND_EXPONENT( sNumeric, sMaxExponent >> 1 );
        }

        aNumeric->mLength = sDigitLength + 1;
    }

    
    STL_RAMP( RAMP_FINISH );
    
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }
        
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 문자열을 Numeric Value로 변환 가능한지 확인 
 * @param[in]  aString             입력 문자열
 * @param[in]  aStringLength       입력 문자열 길이
 * @param[in]  aPrecision          dtlNumericType의 입력 precision
 * @param[out] aErrorStack         에러 스택
 *
 * 앞뒤 공백만 제거하여 decode 수행.
 */
stlStatus dtdValidateNumericFromString( stlChar         * aString,
                                        stlInt64          aStringLength,
                                        stlInt64          aPrecision,
                                        stlErrorStack   * aErrorStack )
{
    stlInt32      sCount;
//    stlBool       sIsPositive       = STL_TRUE;   /* asign만 하고 사용하지 않는 변수 주석처리 */
    stlBool       sIsPositiveExponent   = STL_TRUE;
    stlChar     * sFirstString      = NULL;
    stlChar     * sLastString       = NULL;
    stlChar     * sIntegerPos       = NULL; /* 정수형 숫자 시작 위치 */
    stlChar     * sDecimalPos       = NULL; /* 소수 숫자 시작 위치 */
    stlChar     * sExponentPos      = NULL; /* exponent 숫자 시작 위치 */
    stlInt32      sIntegerLen       = -1;
    stlInt32      sDecimalLen       = -1;
    stlInt32      sExponentLen      = -1;
    stlInt32      sTruncatedCount   = 0;
    stlInt64      sExponent         = 0;
    stlInt64      sScale            = DTL_SCALE_NA;

    
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    STL_PARAM_VALIDATE( aStringLength >= 0, aErrorStack);
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_NUMERIC_MIN_PRECISION) &&
                         (aPrecision <= DTL_NUMERIC_MAX_PRECISION)),
                        aErrorStack );
    
    /**************************************
     *  ANALYZE INPUT STRING
     *************************************/
    sFirstString    = aString;
    sLastString     = aString + aStringLength - 1;
//    sIsPositive     = STL_TRUE;

    /* truncate trailing spaces */
    SKIP_TRAILING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* check sign */
//    CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositive );
    if( (*sFirstString == '+') || (*sFirstString == '-') )
    {
        sFirstString++;
    }                                                                               \
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );         \

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* if current character is digit... */
    if( (sFirstString <= sLastString) && DTF_IS_DIGIT(*sFirstString) )
    {
        sIntegerPos = sFirstString;

        /* skip leading digits */
        SKIP_LEADING_DIGITS( sFirstString, sLastString );
        sIntegerLen = sFirstString - sIntegerPos;
    }
    else
    {
        sIntegerPos = NULL;
        sIntegerLen = 0;
    }

    /* if current character is point... */
    if( (sFirstString <= sLastString) && *sFirstString == '.' )
    {
        sFirstString++;
        if( sFirstString > sLastString )
        {
            STL_TRY_THROW( sIntegerLen > 0, ERROR_INVALID_INPUT_DATA );
            sDecimalPos = NULL;
            sDecimalLen = 0;
        }
        else
        {
            if( DTF_IS_DIGIT(*sFirstString) )
            {
                sDecimalPos = sFirstString;

                /* skip leading digits */
                SKIP_LEADING_DIGITS( sFirstString, sLastString );
                sDecimalLen = sFirstString - sDecimalPos;
            }
            else
            {
                STL_TRY_THROW( sIntegerLen > 0, ERROR_INVALID_INPUT_DATA );
                sDecimalPos = NULL;
                sDecimalLen = 0;
            }
        }
    }
    else
    {
        sDecimalPos = NULL;
        sDecimalLen = 0;
    }

    /* if current character is exponent... */
    if( (sFirstString <= sLastString) &&
        ( (*sFirstString == 'e') || (*sFirstString == 'E') ) )
    {
        sFirstString++;
        
        /* check sign */
        CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositiveExponent );
        STL_TRY_THROW( (sFirstString <= sLastString) && DTF_IS_DIGIT(*sFirstString),
                       ERROR_INVALID_INPUT_DATA );
        
        sExponentPos = sFirstString;
        
        /* skip leading digits */
        SKIP_LEADING_DIGITS( sFirstString, sLastString );
        sExponentLen = sFirstString - sExponentPos;
    }
    else
    {
        sExponentPos = NULL;
        sExponentLen = 0;
    }

    /* check end of string */
    STL_TRY_THROW( sFirstString == (sLastString + 1), ERROR_INVALID_INPUT_DATA );

    /**************************************
     * ADJUST ANALIZED STRING
     *************************************/

    /* truncate leading zeros on integer characters */
    TRUNCATE_LEADING_ZERO_CHARACTERS( sIntegerPos, sIntegerLen, sTruncatedCount );

    /* truncate trailing zeros on decimal characters */
    TRUNCATE_TRAILING_ZERO_CHARACTERS( sDecimalPos, sDecimalLen, sTruncatedCount );

    /* truncate leading zeros on exponent characters */
    TRUNCATE_LEADING_ZERO_CHARACTERS( sExponentPos, sExponentLen, sTruncatedCount );

    /**************************************
     *  VALIDATE ANALIZED STRING
     *************************************/
    
    /* change exponent type string to int64 */
    sExponent = 0;
    for( sCount = 0; sCount < sExponentLen; sCount++ )
    {
        sExponent = sExponent * 10;
        sExponent = sExponent + (*(sExponentPos + sCount) - '0');

        STL_TRY_THROW( sExponent <= STL_UINT32_MAX,
                       ERROR_OUT_OF_RANGE );
    }
    
    if( sIsPositiveExponent == STL_FALSE )
    {
        sExponent = sExponent * (-1);
    }
    else
    {
        /* Do Nothing */
    }
    
    STL_TRY_THROW( ( sExponent >= STL_INT32_MIN ) && ( sExponent <= STL_INT32_MAX ),
                   ERROR_OUT_OF_RANGE );
 
    /* truncated trailing zeros on integer characters 
     * and add truncated count to exponent value */
    if( sDecimalLen == 0 )
    {
        TRUNCATE_TRAILING_ZERO_CHARACTERS( sIntegerPos, sIntegerLen, sTruncatedCount );
        sExponent = sExponent + sTruncatedCount;
    }

    /* truncated leading zeros on decimal characters 
     * and subtract truncated count to exponent value */
    if( sIntegerLen == 0 )
    {
        TRUNCATE_LEADING_ZERO_CHARACTERS( sDecimalPos, sDecimalLen, sTruncatedCount );
        sExponent = sExponent - sTruncatedCount;
    }

    /* calculate scale value */
    sScale = DTL_SCALE_NA;
    
    sScale = aPrecision - ( sExponent + sIntegerLen );
    if( sScale > DTL_NUMERIC_MAX_SCALE )
    {
        sScale = DTL_NUMBER_MAX_SCALE;
    }
    else if( sScale < DTL_NUMERIC_MIN_SCALE )
    {
        sScale = DTL_NUMBER_MIN_SCALE;
    }
    else
    {
        /* Do Nothing */
    }

    /* check out of range by scale */
    /* scale값이 양수이면 소수점 이하 값이 존재하므로 precision에서
     * 이 값을 뺀 값이 정수가 가질 수 있는 최대 자리수 이다.
     * scale값이 음수이면 정수부분의 마지막 자리부터 scale값만큼 
     * 0으로 변경된다. precision은 정수인 경우 마지막 0들을 제외한
     * 정수들의 자리수로 계산되므로 최대 허용 정수 자리수는 
     * precision에 scale 값을 더한 것이 된다. */
    STL_TRY_THROW( (sExponent + sIntegerLen) <= (aPrecision - sScale),
                   ERROR_OUT_OF_RANGE );
    
    return STL_SUCCESS;
     
    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_NUMERIC_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_STRING_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Numeric value를 문자열로 변환
 * @param[in]  aNumeric             Numeric value
 * @param[in]  aAvailableSize       aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer              문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength              변환된 문자열의 길이
 * @param[out] aErrorStack          에러 스택
 */
stlStatus dtdToStringFromNumeric( dtlDataValue    * aNumeric,
                                  stlInt64          aAvailableSize,
                                  void            * aBuffer,
                                  stlInt64        * aLength,
                                  stlErrorStack   * aErrorStack )
{
    dtlNumericType     * sNumeric;
    stlChar            * sResult      = (stlChar*) aBuffer;
    stlInt64             sLength      = 0;
    stlInt32             sExponent    = 0;
    stlInt32             sDigitLen    = 0;
    stlChar            * sDigitString = NULL;
    stlInt32             sZeroLen     = 0;
    stlUInt8           * sDigit       = NULL;
    stlInt32             sIsPositive  = STL_FALSE;
    stlInt32             sExpoLen     = 0;
    stlChar            * sDigitPos    = NULL;
    stlUInt8             sCarry       = 0;
    stlBool              sIsOdd       = STL_FALSE;
    stlInt32             sRoundDigit  = 0;
    dtlExponentString  * sExpoStrInfo = NULL;
    stlChar            * sExpoStr     = NULL;
    
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aNumeric, aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aLength != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize > 0, aErrorStack );


    sNumeric = DTF_NUMERIC( aNumeric );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric, aNumeric->mLength ) == STL_TRUE )
    {
        *aLength  = 1;
        *sResult = '0';
        STL_THROW( RAMP_EXIT );
    }
    else
    {
        sDigitLen = DTL_NUMERIC_GET_DIGIT_COUNT( aNumeric->mLength );
        sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );
        sDigit    = & sNumeric->mData[1];
        sResult   = (stlChar*) aBuffer;

        sIsPositive = DTL_NUMERIC_IS_POSITIVE( sNumeric );
        if( sIsPositive )
        {
            /* Do Nothing */
        }
        else
        {
            /* sign */
            sLength++;
            *sResult = '-';
            sResult++;
        }

        
        /**
         * 실수형 표현
         */
        
        if( sExponent < 0 )
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : -2
             * String : .0001234
             *
             * @remark truncate trailing zero가 적용되어 digit은 구성됨을 가정
             *    <BR> invalid digit  :  1.234000   ( 01 | 23 | 40 | "00", length = 4 )
             */

            /* digits : truncate trailing zero */
            sLength += ( ( ( sDigitLen - sExponent - 1 ) << 1 ) -
                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen - 1 ] )[ 1 ] == '0' ) );
                
            /* decimal point */
            sLength++;
                
            /* check buffer size */
            STL_TRY_THROW( sLength <= aAvailableSize, RAMP_EXPONENT_EXPRESSION );

            /* set length */
            *aLength = sLength;
                
            /* set decimal point */
            *sResult = '.';
            sResult++;
            sZeroLen = -((++sExponent) << 1);

            while( sZeroLen )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }

            /* set digits (n-1) */
            while( sDigitLen > 1 )
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[ 0 ];
                sResult++;
                *sResult = sDigitString[ 1 ];
                sResult++;
                sDigit++;
                sDigitLen--;
            }

            /* set last digit */
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
            *sResult = sDigitString[ 0 ];
            sResult++;
            if( sDigitString[ 1 ] == '0' )
            {
                /* Do Nothing */
            }
            else
            {
                *sResult = sDigitString[ 1 ];
            }
        }
        else if( sDigitLen > sExponent + 1 )
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : 1
             * String : 123.4
             */
            
            /* digits : truncate leading zero & truncate trailing zero */
            sLength += ( ( sDigitLen << 1 ) -
                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) -
                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen - 1 ] )[ 1 ] == '0' ) );

            /* decimal point */
            sLength++;

            /* check buffer size */
            STL_TRY_THROW( sLength <= aAvailableSize, RAMP_EXPONENT_EXPRESSION );

            /* set length */
            *aLength = sLength;

            /* set first digit */
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
            if( sDigitString[ 0 ] == '0' )
            {
                /* Do Nothing */
            }
            else
            {
                *sResult = sDigitString[ 0 ];
                sResult++;
            }
            *sResult = sDigitString[ 1 ];
            sResult++;
            sDigit++;
            sDigitLen--;

            /* set digits : before decimal point */
            while( sExponent > 0 )
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[ 0 ];
                sResult++;
                *sResult = sDigitString[ 1 ];
                sResult++;
                sDigit++;
                sDigitLen--;
                sExponent--;
            }
                
            /* set decimal point */
            *sResult = '.';
            sResult++;

            /* set digits : after decimal point */
            while( sDigitLen > 1 )
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[ 0 ];
                sResult++;
                *sResult = sDigitString[ 1 ];
                sResult++;
                sDigit++;
                sDigitLen--;
            }

            /* set last digit */
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
            *sResult = sDigitString[ 0 ];
            sResult++;
            if( sDigitString[ 1 ] == '0' )
            {
                /* Do Nothing */
            }
            else
            {
                *sResult = sDigitString[ 1 ];
            }
        }
        else
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : 3
             * String : 1234000
             */
                
            /* digits : truncate leading zero & add trailing zeroes */
            sZeroLen = ( sExponent - sDigitLen + 1 ) << 1;
            sLength += ( ( sZeroLen + ( sDigitLen << 1 ) ) -
                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) );

            /* check buffer size */
            STL_TRY_THROW( sLength <= aAvailableSize, RAMP_EXPONENT_EXPRESSION );

            /* set length */
            *aLength = sLength;

            /* set first digit */
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
            if( sDigitString[ 0 ] == '0' )
            {
                /* Do Nothing */
            }
            else
            {
                *sResult = sDigitString[ 0 ];
                sResult++;
            }
            *sResult = sDigitString[ 1 ];
            sResult++;
            sDigit++;
            sDigitLen--;

            /* set digits (n) */
            while( sDigitLen > 0 )
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[ 0 ];
                sResult++;
                *sResult = sDigitString[ 1 ];
                sResult++;
                sDigit++;
                sDigitLen--;
            }

            while( sZeroLen )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }

        STL_THROW( RAMP_EXIT );

        
        /**
         * 지수형 표현
         */

        STL_RAMP( RAMP_EXPONENT_EXPRESSION );

        aAvailableSize -= (!sIsPositive);
        
        STL_TRY_THROW( aAvailableSize > 3,
                       ERROR_NOT_ENOUGH_BUFFER );

        sDigitLen = DTL_NUMERIC_GET_DIGIT_COUNT( aNumeric->mLength );
        sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );


        /**
         * Digit 설정
         */

        sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
        
        sExponent    = sExponent << 1;
        sIsOdd       = ( sDigitString[ 0 ] == '0' );
        sExponent   += (!sIsOdd);
        
        if( sExponent < 0 )
        {
            sExpoLen = 3 + ( sExponent < -99 ) + ( sExponent < -9 );
        }
        else
        {
            sExpoLen = 3 + ( sExponent > 99 ) + ( sExponent > 9 );
        }
        
        sRoundDigit  = aAvailableSize - sExpoLen;
        sRoundDigit -= ( sRoundDigit > 1 );
        
        STL_TRY_THROW( ( aAvailableSize > 0 ) && ( aAvailableSize > sExpoLen ),
                       ERROR_NOT_ENOUGH_BUFFER );
        
        sDigitPos = sResult;
        sDigitPos++;

        while( sRoundDigit > 0 && sDigitLen > 0 )
        {
            if( sIsOdd )
            {
                *sDigitPos = sDigitString[ 1 ];
                sDigit++;
                sDigitLen--;
            }
            else
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sDigitPos = sDigitString[ 0 ];
            }
                
            sIsOdd = !sIsOdd;
            sRoundDigit--;
            sDigitPos++;
        }

        
        /**
         * 반올림 수행
         */

        if( sDigitLen > 0 )
        {
            if( sIsOdd )
            {
                sCarry = sDigitString[ 1 ] > '4';
            }
            else
            {
                sCarry = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] > '4';
            }
        }
        else
        {
            sCarry = 0;
        }


        sDigitPos--;
        if( sCarry == 0 )
        {
            sRoundDigit = sDigitPos - sResult;
        }
        else
        {
            sRoundDigit = sDigitPos - sResult + 1;
            
            while( ( sCarry == 1 ) && ( sRoundDigit > 1 ) )
            {
                *sDigitPos += sCarry;
            
                if( *sDigitPos > '9' )
                {
                    *sDigitPos = '0';
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }
            
                sRoundDigit--;
                sDigitPos--;
            }
        }


        /**
         * Trailing Zero
         */
        
        while( (sRoundDigit > 0) &&
               (sResult[ sRoundDigit ] == '0') )
        {
            sRoundDigit--;
        }

        
        /**
         * Decimal Point 설정
         */
        
        if( sRoundDigit == 0 )
        {
            sResult[0] = '1';
            sResult++;
            sExponent++;
        }
        else 
        {
            sResult[0] = sResult[1];
            sResult++;
            
            if( sRoundDigit == 1 )
            {
                /* Do Nothing */
            }
            else
            {
                *sResult = '.';
                sResult += sRoundDigit;
            }
        }

        
        /**
         * Exponent 설정
         */

        sExpoStrInfo = (dtlExponentString *) & dtdNumberExponentString[ sExponent ];
        sExpoStr     = (stlChar *) sExpoStrInfo->mString;
        
        switch( sExpoStrInfo->mStringCnt )
        {
            case 5 :
                {
                    *sResult = *sExpoStr++;
                    sResult++;
                }
            case 4 :
                {
                    *sResult = *sExpoStr++;
                    sResult++;
                }
            case 3 :
                {
                    *sResult = *sExpoStr++;
                    sResult++;
                }
            case 2 :
                {
                    *sResult = *sExpoStr++;
                    sResult++;
                }
            case 1 :
                {
                    *sResult = *sExpoStr;
                    sResult++;
                    break;
                }
                
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
            
        *aLength = sResult - (stlChar*)aBuffer;
    }


    STL_RAMP( RAMP_EXIT );
    
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_BYTE_LENGTH_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Numeric value를 문자열로 변환
 * @param[in]  aNumeric             Numeric value
 * @param[in]  aAvailableSize       aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer              문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength              변환된 문자열의 길이
 * @param[out] aErrorStack          에러 스택
 */
stlStatus dtdCastNumericToString( dtlDataValue    * aNumeric,
                                  stlInt64          aAvailableSize,
                                  void            * aBuffer,
                                  stlInt64        * aLength,
                                  stlErrorStack   * aErrorStack )
{
    dtlNumericType     * sNumeric;
    stlChar            * sResult      = (stlChar*) aBuffer;
    stlInt64             sLength      = 0;
    stlInt32             sExponent    = 0;
    stlInt32             sDigitLen    = 0;
    stlInt32             sIntDigitLen = 0;
    stlChar            * sDigitString = NULL;
    stlInt32             sZeroLen     = 0;
    stlInt32             sRealZeroLen = 0;
    stlUInt8           * sDigit       = NULL;
    stlInt32             sIsPositive  = STL_FALSE;
    stlInt32             sExpoLen     = 0;
    stlChar            * sDigitPos    = NULL;
    stlUInt8             sCarry       = 0;
    stlBool              sIsOdd       = STL_FALSE;
    stlInt32             sRoundDigit  = 0;
    dtlExponentString  * sExpoStrInfo = NULL;
    stlChar            * sExpoStr     = NULL;
    stlInt32             sExpLength   = 0;
    stlInt32             sExponentForExpExpression = 0;
    stlInt64             sAvailableSize = aAvailableSize;    
    
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aNumeric, aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aLength != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize > 0, aErrorStack );


    sNumeric = DTF_NUMERIC( aNumeric );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric, aNumeric->mLength ) == STL_TRUE )
    {
        *aLength  = 1;
        *sResult = '0';
        STL_THROW( RAMP_EXIT );
    }
    else
    {
        sDigitLen = DTL_NUMERIC_GET_DIGIT_COUNT( aNumeric->mLength );
        sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );
        sDigit    = & sNumeric->mData[1];
        sResult   = (stlChar*) aBuffer;

        sIsPositive = DTL_NUMERIC_IS_POSITIVE( sNumeric );
        if( sIsPositive )
        {
            /* Do Nothing */
        }
        else
        {
            /* sign */
            sLength++;
            *sResult = '-';
            sResult++;
        }
        
        /**
         * 실수형 표현
         */
        
        if( sExponent < 0 )
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : -2
             * String : .0001234
             *
             * @remark truncate trailing zero가 적용되어 digit은 구성됨을 가정
             *    <BR> invalid digit  :  1.234000   ( 01 | 23 | 40 | "00", length = 4 )
             */

            /* digits : truncate trailing zero */
            sLength += ( ( ( sDigitLen - sExponent - 1 ) << 1 ) -
                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen - 1 ] )[ 1 ] == '0' ) );
                
            /* decimal point */
            sLength++;
                

            if( sLength > sAvailableSize )
            {
                sExponentForExpExpression = -( sExponent << 1 ) -
                    ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen - 1 ] )[ 1 ] == '0' );
                
                sExpLength = ( sDigitLen << 1 ) + 1/* decimal point*/ + ( !sIsPositive ) -
                    ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0] == '0' ) -
                    ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen - 1 ] )[ 1 ] == '0' );
                sExpLength += 1 /* 'E' */ +
                    ( sExponentForExpExpression > 10 ? ( sExponentForExpExpression > 100 ? 3 : 2 ) : 1 );

                /* exponent 형식으로 모든 유효자리를 ( 소수포함 ) 표현 가능하다면 exponent 형태로 출력 */
                if( sExpLength <= sAvailableSize )
                {
                    STL_THROW( RAMP_EXPONENT_EXPRESSION );
                }
                else
                {
                    /* Do Nothing */
                }

                sZeroLen = -((++sExponent) << 1);

                /* aVailableSize 로 '0' 와 '.' 로 만 출력 가능하다면 '0'
                 * ex ) 0.000012345 , sAvailableSize 6
                 */

                sRealZeroLen = sZeroLen + ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' );
                if( sRealZeroLen + ( !sIsPositive ) + 1 /* decimal point */ == sAvailableSize )
                {
                    if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' )
                    {
                        if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 1 ] != '9' )
                        {
                            STL_THROW( RAMP_EXPONENT_EXPRESSION );
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    }
                    else
                    {
                        if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] != '9' )
                        {
                            STL_THROW( RAMP_EXPONENT_EXPRESSION );
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    }
                }
                else if( sRealZeroLen + ( !sIsPositive ) + 1 /* decimal point */ > sAvailableSize )
                {
                    STL_THROW( RAMP_EXPONENT_EXPRESSION );
                }
                else
                {
                    /* Do Nothing */
                }
                    
                #if 0
                sRealZeroLen = sZeroLen + ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' );
                if( sRealZeroLen + ( !sIsPositive ) + 1 /* decimal point */ >= sAvailableSize )
                {
                    if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' )
                    {
                        if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 1 ] < '5' )
                        {
                            STL_THROW( RAMP_EXPONENT_EXPRESSION );
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    }
                    else
                    {
                        if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] < '5' )
                        {
                            STL_THROW( RAMP_EXPONENT_EXPRESSION );
                        }
                        else
                        {
                            
                            /* Do Nothing */
                        }
                    }
                }
                else
                {
                    /* Do Nothing */
                }
                #endif

            
                sRoundDigit = sAvailableSize - 1 /* decimal point */ - ( !sIsPositive );
                *sResult = '.';
                sResult++;
                sDigitPos = sResult;
                
                while( sZeroLen )
                {
                    *sDigitPos = '0';
                    sDigitPos++;
                    sZeroLen--;
                    sRoundDigit--;
                }

                while( sRoundDigit > 0 && sDigitLen > 0 )
                {
                    if( sIsOdd )
                    {
                        *sDigitPos = sDigitString[ 1 ];
                        sDigit++;
                        sDigitLen--;
                        
                    }
                    else
                    {
                        sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                        *sDigitPos = sDigitString[ 0 ];
                    }

                    sIsOdd = ( !sIsOdd );
                    sRoundDigit--;
                    sDigitPos++;
                }

                if( sDigitLen > 0 )
                {
                    if( sIsOdd )
                    {
                        sCarry = sDigitString[ 1 ] > '4';
                    }
                    else
                    {
                        sCarry = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive , *sDigit )[ 0 ] > '4';
                    }
                }
                else
                {
                    sCarry = 0;
                }

                /*
                 * round truncate
                 */
                sDigitPos--;

                if( sCarry == 1 )
                {
                    sRoundDigit = sDigitPos - sResult + 1;

                    while( ( sCarry == 1 ) && ( sRoundDigit > 0 ) )
                    {
                        *sDigitPos += sCarry;

                        if( *sDigitPos > '9' )
                        {
                            *sDigitPos = '0';
                            sCarry = 1;
                            sAvailableSize--;
                        }
                        else
                        {
                            sCarry = 0;
                        }

                        sRoundDigit--;
                        sDigitPos--;
                    }

                    if( sRoundDigit == 0 && sCarry == 1 )
                    {
                        if( sIsPositive == STL_TRUE)
                        {
                            *aLength = 1;
                            (*(stlChar *)aBuffer) = '1';
                        }
                        else
                        {
                            *aLength = 2;
                            (*((stlChar *)aBuffer+1)) = '1';
                        }
                        
                        STL_THROW( RAMP_EXIT );
                    }
                    else
                    {
                        /* Do Nothing */
                    }                   
                }
                else
                {
                    /* Do Nothing */
                }
                
                sRoundDigit = sAvailableSize - 1 /* decimal point */ - ( !sIsPositive );
                sDigitPos = sResult + sRoundDigit -1;

                while( *sDigitPos == '0' )
                {
                    sAvailableSize--;
                    sDigitPos--;
                }

                *aLength = sAvailableSize;
            }
            else
            {
                /* set length */
                *aLength = sLength;
                
                /* set decimal point */
                *sResult = '.';
                sResult++;
                sZeroLen = -((++sExponent) << 1);

                while( sZeroLen )
                {
                    *sResult = '0';
                    sResult++;
                    sZeroLen--;
                }

                /* set digits (n-1) */
                while( sDigitLen > 1 )
                {
                    sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                    *sResult = sDigitString[ 0 ];
                    sResult++;
                    *sResult = sDigitString[ 1 ];
                    sResult++;
                    sDigit++;
                    sDigitLen--;
                }

                /* set last digit */
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[ 0 ];
                sResult++;
                if( sDigitString[ 1 ] == '0' )  
                {
                    /* Do Nothing */
                }
                else
                {
                    *sResult = sDigitString[ 1 ];
                }
            }
        }
        else if( sDigitLen > sExponent + 1 )
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : 1
             * String : 123.4
             */
            
            /* digits : truncate leading zero & truncate trailing zero */
            sLength += ( ( sDigitLen << 1 ) -
                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) -
                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen - 1 ] )[ 1 ] == '0' ) );

            /* decimal point */
            sLength++;



            if( sLength > sAvailableSize )
            {
                /* integer digit count */
                sIntDigitLen = ( sExponent <<  1 ) +
                               ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ? 1 : 2 );

                /* decimal point 이상 값을 표현 할 수 없다면 exponent 형태로 출력 */
                STL_TRY_THROW( sIntDigitLen + ( !sIsPositive ) <= sAvailableSize, RAMP_EXPONENT_EXPRESSION );


                
                sRoundDigit = sAvailableSize - ( !sIsPositive );
                
                sDigitPos = sResult;

                /* set first digit */
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                if( sDigitString[ 0 ] == '0' )
                {
                    /* Do Nothing */
                }
                else
                {
                    *sDigitPos = sDigitString[ 0 ];
                    sDigitPos++;
                    sRoundDigit--;
                }
                *sDigitPos = sDigitString[ 1 ];
                sDigitPos++;
                sDigit++;
                sDigitLen--;
                sRoundDigit--;

                /* set digits : before decimal point */
                while( sExponent > 0 )
                {
                    sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                    *sDigitPos = sDigitString[ 0 ];
                    sDigitPos++;
                    *sDigitPos = sDigitString[ 1 ];
                    sDigitPos++;
                    sDigit++;
                    sDigitLen--;
                    sExponent--;
                    sRoundDigit -= 2;
                }
               
                *sDigitPos = '.';
                sDigitPos++;
                sRoundDigit--;
                    
                /* set digits : after decimal point */
                while( sRoundDigit > 0 && sDigitLen > 0 )
                {
                    if( sIsOdd )
                    {
                        *sDigitPos = sDigitString[ 1 ];
                        sDigit++;
                        sDigitLen--;
                    }
                    else
                    {
                        sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive , *sDigit );
                        *sDigitPos = sDigitString[ 0 ];
                    }

                    sIsOdd = ( !sIsOdd );
                    sRoundDigit--;
                    sDigitPos++;                  
                }

                /* check carry after decimal point */
                if( sDigitLen > 0 )
                {
                    if( sIsOdd )
                    {
                        sCarry = sDigitString[ 1 ] > '4';
                    }
                    else
                    {
                        sCarry = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive , *sDigit )[ 0 ] > '4';
                    }
                }
                else
                {
                    sCarry = 0;
                }

                sDigitPos--;

                if( sCarry == 0 )
                {
                    /* do nothing */
                }
                else
                {
                    sRoundDigit = sDigitPos - sResult; /* contain decimal point length */
                    sLength = sIntDigitLen + ( !sIsPositive );

                    /**
                     * 반올림 결과가 aAvailableSize 보다 크다면 오류
                     * ex) 999.9 -> 1000 ( aAvailableSize : 3 )
                     */

                    while( ( sCarry == 1 ) && ( sRoundDigit > 0 ) )
                    {
                        if( *sDigitPos == '.' )
                        {
                            sRoundDigit--;
                            sDigitPos--;
                            continue;
                        }
                        
                        *sDigitPos += sCarry;

                        if( *sDigitPos > '9' )
                        {
                            *sDigitPos = '0';
                            sCarry = 1;
                        }
                        else
                        {
                            sCarry = 0;
                        }

                        sRoundDigit--;
                        sDigitPos--;
                    }

                    if( sRoundDigit == 0 && sCarry == 1 )
                    {
                        STL_TRY_THROW( sLength + 1<=  sAvailableSize, RAMP_EXPONENT_EXPRESSION );
                        *aLength = sLength + 1;
                        sResult[0] = '1';
                        sResult++;

                        while( sIntDigitLen > 0 )
                        {
                            *sResult = '0';
                            sResult++;
                            sIntDigitLen--;
                        }  
                        STL_THROW( RAMP_EXIT );
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }

                sRoundDigit = sAvailableSize - ( !sIsPositive );

                /*
                 * decimal point (포함) 이후 모두 '0' ( 반올림 연산에 의한 ) 라면 truncate
                 */
                while( sRoundDigit > 0 && sRoundDigit > sIntDigitLen &&
                       ( sResult[ sRoundDigit -1 ] == '.' ||
                         sResult[ sRoundDigit -1 ] == '0' ) )
                {
                    sRoundDigit--;
                    sAvailableSize--;
                }
                
                *aLength = sAvailableSize;

            }
            else
            {
                /* set length */
                *aLength = sLength;

                /* set first digit */
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                if( sDigitString[ 0 ] == '0' )
                {
                    /* Do Nothing */
                }
                else
                {
                    *sResult = sDigitString[ 0 ];
                    sResult++;
                }
                *sResult = sDigitString[ 1 ];
                sResult++;
                sDigit++;
                sDigitLen--;

                /* set digits : before decimal point */
                while( sExponent > 0 )
                {
                    sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                    *sResult = sDigitString[ 0 ];
                    sResult++;
                    *sResult = sDigitString[ 1 ];
                    sResult++;
                    sDigit++;
                    sDigitLen--;
                    sExponent--;
                }
                
                /* set decimal point */
                *sResult = '.';
                sResult++;

                /* set digits : after decimal point */
                while( sDigitLen > 1 )
                {
                    sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                    *sResult = sDigitString[ 0 ];
                    sResult++;
                    *sResult = sDigitString[ 1 ];
                    sResult++;
                    sDigit++;
                    sDigitLen--;
                }

                /* set last digit */
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[ 0 ];
                sResult++;
                if( sDigitString[ 1 ] == '0' )
                {
                    /* Do Nothing */
                }
                else
                {
                    *sResult = sDigitString[ 1 ];
                }
            }
        }
        else
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : 3
             * String : 1234000
             */
                
            /* digits : truncate leading zero & add trailing zeroes */
            sZeroLen = ( sExponent - sDigitLen + 1 ) << 1;
            sLength += ( ( sZeroLen + ( sDigitLen << 1 ) ) -
                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) );

            /* 유효자리를 모두 표현 하지 못할 경우 exponent 형태로 출력  */
            STL_TRY_THROW( sLength <= sAvailableSize, RAMP_EXPONENT_EXPRESSION );

            /* set length */
            *aLength = sLength;

            /* set first digit */
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
            if( sDigitString[ 0 ] == '0' )
            {
                /* Do Nothing */
            }
            else
            {
                *sResult = sDigitString[ 0 ];
                sResult++;
            }
            *sResult = sDigitString[ 1 ];
            sResult++;
            sDigit++;
            sDigitLen--;

            /* set digits (n) */
            while( sDigitLen > 0 )
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[ 0 ];
                sResult++;
                *sResult = sDigitString[ 1 ];
                sResult++;
                sDigit++;
                sDigitLen--;
            }

            while( sZeroLen )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }

        STL_THROW( RAMP_EXIT );

        
        /**
         * 지수형 표현
         */

        STL_RAMP( RAMP_EXPONENT_EXPRESSION );

        sAvailableSize -= (!sIsPositive);
        
        STL_TRY_THROW( sAvailableSize > 3,
                       ERROR_NOT_ENOUGH_BUFFER );

        sDigitLen = DTL_NUMERIC_GET_DIGIT_COUNT( aNumeric->mLength );
        sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );
        sDigit    = & sNumeric->mData[1];
        sResult   = (stlChar*) aBuffer;
        
        if( sIsPositive )
        {
            /* Do Nothing */
        }
        else
        {
            *sResult = '-';
            sResult++;
        }


        /**
         * Digit 설정
         */
        
        sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
        
        sExponent    = sExponent << 1;
        sIsOdd       = ( sDigitString[ 0 ] == '0' );
        sExponent   += (!sIsOdd);
        
        if( sExponent < 0 )
        {
            if( sExponent < -99 )
            {
                sExpoLen = 5;
            }
            else if( sExponent < -9 )
            {
                sExpoLen = 4;
            }
            else
            {
                sExpoLen = 3;
            }
        }
        else
        {
            if( sExponent > 99 )
            {
                sExpoLen = 5;
            }
            else if( sExponent > 9 )
            {
                sExpoLen = 4;                
            }
            else
            {
                sExpoLen = 3;                
            }
        }
        
        sRoundDigit  = sAvailableSize - sExpoLen;
        sRoundDigit -= ( sRoundDigit > 1 );
        
        STL_TRY_THROW( ( sRoundDigit > 0 ) && ( sAvailableSize >= ( sExpoLen + sRoundDigit ) ),
                       ERROR_NOT_ENOUGH_BUFFER );
        
        sDigitPos = sResult;
        sDigitPos++;

        while( sRoundDigit > 0 && sDigitLen > 0 )
        {
            if( sIsOdd )
            {
                *sDigitPos = sDigitString[ 1 ];
                sDigit++;
                sDigitLen--;
            }
            else
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sDigitPos = sDigitString[ 0 ];
            }
                
            sIsOdd = !sIsOdd;
            sRoundDigit--;
            sDigitPos++;
        }

        
        /**
         * 반올림 수행
         */

        if( sDigitLen > 0 )
        {
            if( sIsOdd )
            {
                sCarry = sDigitString[ 1 ] > '4';
            }
            else
            {
                sCarry = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] > '4';
            }
        }
        else
        {
            sCarry = 0;
        }


        sDigitPos--;
        if( sCarry == 0 )
        {
            sRoundDigit = sDigitPos - sResult;
        }
        else
        {
            sRoundDigit = sDigitPos - sResult + 1;
            
            while( ( sCarry == 1 ) && ( sRoundDigit > 1 ) )
            {
                *sDigitPos += sCarry;
            
                if( *sDigitPos > '9' )
                {
                    *sDigitPos = '0';
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }
            
                sRoundDigit--;
                sDigitPos--;
            }
        }


        /**
         * Trailing Zero
         */
        
        while( (sRoundDigit > 0) &&
               (sResult[ sRoundDigit ] == '0') )
        {
            sRoundDigit--;
        }

        
        /**
         * Decimal Point 설정
         */
        
        if( sRoundDigit == 0 )
        {
            sResult[0] = '1';
            sResult++;
            sExponent++;
        }
        else 
        {
            sResult[0] = sResult[1];
            sResult++;
            
            if( sRoundDigit == 1 )
            {
                /* Do Nothing */
            }
            else
            {
                *sResult = '.';
                sResult += sRoundDigit;
            }
        }
        
        
        /**
         * Exponent 설정
         */
        
        sExpoStrInfo = (dtlExponentString *) & dtdNumberExponentString[ sExponent ];
        sExpoStr     = (stlChar *) sExpoStrInfo->mString;
        
        /* 반올림되어 exonent string이 늘어나는 경우, buffer size check */
        STL_TRY_THROW( ( ( (sResult + sExpoStrInfo->mStringCnt) - (stlChar*)aBuffer )
                         <= aAvailableSize ),
                       ERROR_NOT_ENOUGH_BUFFER );
        
        switch( sExpoStrInfo->mStringCnt )
        {
            case 5 :
                {
                    *sResult = *sExpoStr++;
                    sResult++;
                }
            case 4 :
                {
                    *sResult = *sExpoStr++;
                    sResult++;
                }
            case 3 :
                {
                    *sResult = *sExpoStr++;
                    sResult++;
                }
            case 2 :
                {
                    *sResult = *sExpoStr++;
                    sResult++;
                }
            case 1 :
                {
                    *sResult = *sExpoStr;
                    sResult++;
                    break;
                }
                
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }
            
        *aLength = sResult - (stlChar*)aBuffer;

        STL_DASSERT( *aLength <= aAvailableSize );
    }


    STL_RAMP( RAMP_EXIT );
    
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_BYTE_LENGTH_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Numeric value를 Non Truncate 문자열로 변환
 * @param[in]  aNumeric             Numeric value
 * @param[in]  aAvailableSize       aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer              문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength              변환된 문자열의 길이
 * @param[out] aErrorStack          에러 스택
 */
stlStatus dtdCastNumericToNonTruncateString( dtlDataValue    * aNumeric,
                                             stlInt64          aAvailableSize,
                                             void            * aBuffer,
                                             stlInt64        * aLength,
                                             stlErrorStack   * aErrorStack )
{
    dtlNumericType     * sNumeric;
    stlChar            * sResult      = (stlChar*) aBuffer;
    stlInt64             sLength      = 0;
    stlInt32             sExponent    = 0;
    stlInt32             sDigitLen    = 0;
    stlChar            * sDigitString = NULL;
    stlInt32             sZeroLen     = 0;
    stlUInt8           * sDigit       = NULL;
    stlInt32             sIsPositive  = STL_FALSE;
    
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aNumeric, aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aLength != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize > 0, aErrorStack );


    sNumeric = DTF_NUMERIC( aNumeric );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric, aNumeric->mLength ) == STL_TRUE )
    {
        *aLength  = 1;
        *sResult = '0';
        STL_THROW( RAMP_EXIT );
    }
    else
    {
        sDigitLen = DTL_NUMERIC_GET_DIGIT_COUNT( aNumeric->mLength );
        sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );
        sDigit    = & sNumeric->mData[1];
        sResult   = (stlChar*) aBuffer;

        sIsPositive = DTL_NUMERIC_IS_POSITIVE( sNumeric );
        if( sIsPositive )
        {
            /* Do Nothing */
        }
        else
        {
            /* sign */
            sLength++;
            *sResult = '-';
            sResult++;
        }

        
        /**
         * 실수형 표현
         */
        
        if( sExponent < 0 )
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : -2
             * String : .0001234
             *
             * @remark truncate trailing zero가 적용되어 digit은 구성됨을 가정
             *    <BR> invalid digit  :  1.234000   ( 01 | 23 | 40 | "00", length = 4 )
             */

            /* digits : truncate trailing zero */
            sLength += ( ( ( sDigitLen - sExponent - 1 ) << 1 ) -
                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen - 1 ] )[ 1 ] == '0' ) );
                
            /* decimal point */
            sLength++;
                
            /* check buffer size */
            STL_TRY_THROW( sLength <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER );

            /* set length */
            *aLength = sLength;
                
            /* set decimal point */
            *sResult = '.';
            sResult++;
            sZeroLen = -((++sExponent) << 1);

            while( sZeroLen )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }

            /* set digits (n-1) */
            while( sDigitLen > 1 )
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[ 0 ];
                sResult++;
                *sResult = sDigitString[ 1 ];
                sResult++;
                sDigit++;
                sDigitLen--;
            }

            /* set last digit */
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
            *sResult = sDigitString[ 0 ];
            sResult++;
            if( sDigitString[ 1 ] == '0' )
            {
                /* Do Nothing */
            }
            else
            {
                *sResult = sDigitString[ 1 ];
            }
        }
        else if( sDigitLen > sExponent + 1 )
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : 1
             * String : 123.4
             */
            
            /* digits : truncate leading zero & truncate trailing zero */
            sLength += ( ( sDigitLen << 1 ) -
                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) -
                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen - 1 ] )[ 1 ] == '0' ) );

            /* decimal point */
            sLength++;

            /* check buffer size */
            STL_TRY_THROW( sLength <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER );

            /* set length */
            *aLength = sLength;

            /* set first digit */
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
            if( sDigitString[ 0 ] == '0' )
            {
                /* Do Nothing */
            }
            else
            {
                *sResult = sDigitString[ 0 ];
                sResult++;
            }
            *sResult = sDigitString[ 1 ];
            sResult++;
            sDigit++;
            sDigitLen--;

            /* set digits : before decimal point */
            while( sExponent > 0 )
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[ 0 ];
                sResult++;
                *sResult = sDigitString[ 1 ];
                sResult++;
                sDigit++;
                sDigitLen--;
                sExponent--;
            }
                
            /* set decimal point */
            *sResult = '.';
            sResult++;

            /* set digits : after decimal point */
            while( sDigitLen > 1 )
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[ 0 ];
                sResult++;
                *sResult = sDigitString[ 1 ];
                sResult++;
                sDigit++;
                sDigitLen--;
            }

            /* set last digit */
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
            *sResult = sDigitString[ 0 ];
            sResult++;
            if( sDigitString[ 1 ] == '0' )
            {
                /* Do Nothing */
            }
            else
            {
                *sResult = sDigitString[ 1 ];
            }
        }
        else
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : 3
             * String : 1234000
             */
                
            /* digits : truncate leading zero & add trailing zeroes */
            sZeroLen = ( sExponent - sDigitLen + 1 ) << 1;
            sLength += ( ( sZeroLen + ( sDigitLen << 1 ) ) -
                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) );

            /* check buffer size */
            STL_TRY_THROW( sLength <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER );

            /* set length */
            *aLength = sLength;

            /* set first digit */
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
            if( sDigitString[ 0 ] == '0' )
            {
                /* Do Nothing */
            }
            else
            {
                *sResult = sDigitString[ 0 ];
                sResult++;
            }
            *sResult = sDigitString[ 1 ];
            sResult++;
            sDigit++;
            sDigitLen--;

            /* set digits (n) */
            while( sDigitLen > 0 )
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[ 0 ];
                sResult++;
                *sResult = sDigitString[ 1 ];
                sResult++;
                sDigit++;
                sDigitLen--;
            }

            while( sZeroLen )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }
    }

    STL_RAMP( RAMP_EXIT );
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_BYTE_LENGTH_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 
 * @param[in]  aString             입력 문자열
 * @param[in]  aStringLength       입력 문자열 길이
 * @param[out] aResult             integer 변환 값
 * @param[out] aErrorStack         에러 스택
 *
 * 앞뒤 공백만 제거하여 decode 수행.
 * 
 * @remark 10진수로 구성된 문자열만 지원한다. (16진수의 'E'와 expoent의 'E'를 구분할 수 없기 때문에)
 */
stlStatus dtdToInt64FromString( stlChar         * aString,
                                stlInt64          aStringLength,
                                stlInt64        * aResult,
                                stlErrorStack   * aErrorStack )
{
    stlInt32      sCount;
    stlBool       sIsPositive       = STL_TRUE;
    stlBool       sIsPositiveExponent   = STL_TRUE;
    stlChar     * sFirstString      = NULL;
    stlChar     * sLastString       = NULL;
    stlChar     * sIntegerPos       = NULL; /* 정수형 숫자 시작 위치 */
    stlChar     * sDecimalPos       = NULL; /* 소수 숫자 시작 위치 */
    stlChar     * sExponentPos      = NULL; /* exponent 숫자 시작 위치 */
    stlInt32      sIntegerLen       = -1;
    stlInt32      sDecimalLen       = -1;
    stlInt32      sExponentLen      = -1;
    stlInt32      sTruncatedCount   = 0;
    stlInt64      sExponent         = 0;
    stlUInt64     sUInt64           = 0;
    stlUInt64     sCarry            = 0;

    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    DTL_PARAM_VALIDATE( aStringLength >= 0, aErrorStack);
    DTL_PARAM_VALIDATE( aResult != NULL, aErrorStack);

    /**************************************
     *  ANALYZE INPUT STRING
     *************************************/
    sFirstString    = aString;
    sLastString     = aString + aStringLength - 1;
    sIsPositive     = STL_TRUE;

    /* truncate trailing spaces */
    SKIP_TRAILING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* check sign */
    CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositive );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* if current character is digit... */
    if( (sFirstString <= sLastString) && DTF_IS_DIGIT(*sFirstString) )
    {
        sIntegerPos = sFirstString;

        /* skip leading digits */
        SKIP_LEADING_DIGITS( sFirstString, sLastString );
        sIntegerLen = sFirstString - sIntegerPos;
    }
    else
    {
        sIntegerPos = NULL;
        sIntegerLen = 0;
    }

    /* if current character is point... */
    if( (sFirstString <= sLastString) && (*sFirstString == '.') )
    {
        sFirstString++;
        if( sFirstString > sLastString )
        {
            STL_TRY_THROW( sIntegerLen > 0, ERROR_INVALID_INPUT_DATA );
            sDecimalPos = NULL;
            sDecimalLen = 0;
        }
        else
        {
            if( DTF_IS_DIGIT(*sFirstString) )
            {
                sDecimalPos = sFirstString;

                /* skip leading digits */
                SKIP_LEADING_DIGITS( sFirstString, sLastString );
                sDecimalLen = sFirstString - sDecimalPos;
            }
            else
            {
                STL_TRY_THROW( sIntegerLen > 0, ERROR_INVALID_INPUT_DATA );
                sDecimalPos = NULL;
                sDecimalLen = 0;
            }
        }
    }
    else
    {
        sDecimalPos = NULL;
        sDecimalLen = 0;
    }

    /* if current character is exponent... */
    if( (sFirstString <= sLastString) && 
        ( (*sFirstString == 'e') || (*sFirstString == 'E') ) )
    {
        sFirstString++;

        /* check sign */
        CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositiveExponent );
        STL_TRY_THROW( (sFirstString <= sLastString) && DTF_IS_DIGIT(*sFirstString), ERROR_INVALID_INPUT_DATA );

        sExponentPos = sFirstString;

        /* skip leading digits */
        SKIP_LEADING_DIGITS( sFirstString, sLastString );
        sExponentLen = sFirstString - sExponentPos;
    }
    else
    {
        sExponentPos = NULL;
        sExponentLen = 0;
    }

    /* check end of string */
    STL_TRY_THROW( sFirstString == (sLastString + 1), ERROR_INVALID_INPUT_DATA );

    /**************************************
     * ADJUST ANALIZED STRING
     *************************************/

    /* truncate leading zeros on integer characters */
    TRUNCATE_LEADING_ZERO_CHARACTERS( sIntegerPos, sIntegerLen, sTruncatedCount );

    /* truncate trailing zeros on decimal characters */
    TRUNCATE_TRAILING_ZERO_CHARACTERS( sDecimalPos, sDecimalLen, sTruncatedCount );

    /* truncate leading zeros on exponent characters */
    TRUNCATE_LEADING_ZERO_CHARACTERS( sExponentPos, sExponentLen, sTruncatedCount );

    /**************************************
     *  VALIDATE ANALIZED STRING
     *************************************/

    /* change exponent type string to int64 */
    sExponent = 0;
    for( sCount = 0; sCount < sExponentLen; sCount++ )
    {
        sExponent = sExponent * 10;
        sExponent = sExponent + (*(sExponentPos + sCount) - '0');
    }
    if( sIsPositiveExponent == STL_FALSE )
    {
        sExponent = sExponent * (-1);
    }

    /* truncated trailing zeros on integer characters 
     * and add truncated count to exponent value */
    if( sDecimalLen == 0 )
    {
        TRUNCATE_TRAILING_ZERO_CHARACTERS( sIntegerPos, sIntegerLen, sTruncatedCount );
        sExponent = sExponent + sTruncatedCount;
    }

    /* truncated leading zeros on decimal characters 
     * and subtract truncated count to exponent value */
    if( sIntegerLen == 0 )
    {
        TRUNCATE_LEADING_ZERO_CHARACTERS( sDecimalPos, sDecimalLen, sTruncatedCount );
        sExponent = sExponent - sTruncatedCount;
    }

    /* precision validation */
    STL_TRY_THROW( sIntegerLen + sExponent <= DTL_INT64_MAX_DIGIT_COUNT,
                   ERROR_OUT_OF_RANGE );
    
    /* set carry & rearrange integer and decimal length */
    if( sIntegerLen + sExponent == 0 )
    {
        if( sIntegerLen > 0 )
        {
            /* ex) CAST( '12.5E-2' AS NATIVE_BIGINT ) */
            sCarry = ((sIntegerPos[0] - '0') + 5) / 10;
        }
        else
        {
            if( sDecimalLen > 0 )
            {
                /* ex) CAST( '0.1234' AS NATIVE_BIGINT ) */
                sCarry = ((sDecimalPos[0] - '0') + 5) / 10;
            }
            else
            {
                sCarry = 0;
            }
        }

        sIntegerLen = 0;
        sDecimalLen = 0;
    }
    else if( sIntegerLen + sExponent < 0 )
    {
        sCarry = 0;
        sIntegerLen = 0;
        sDecimalLen = 0;
    }
    else
    {
        /* sIntegerLen + sExponent > 0 */
        if( sExponent >= 0 )
        {
            if( sDecimalLen > sExponent )
            {
                sCarry = ((sDecimalPos[sExponent] - '0') + 5) / 10;
                sDecimalLen = sExponent;
            }
            else
            {
                sCarry = 0;
            }
        }
        else
        {
            sCarry = ((sIntegerPos[sIntegerLen + sExponent] - '0') + 5) / 10;
            sIntegerLen = sIntegerLen + sExponent;
            sDecimalLen = 0;
        }
    }

    /**************************************
     *  MAKE RESULT
     *************************************/

    /* uint64의 max digit count가 int64보다 1 크기 때문에
     * uint64에 값을 넣으면 절대로 overflow가 발생하지 않는다. 
     * 따라서 uint64에 값을 넣은 다음 overflow체크를 수행하고
     * 최종 결과에 값을 넣는다. */
    sUInt64 = 0;
    /* integer part */
    for( sCount = 0; sCount < sIntegerLen; sCount++ )
    {
        sUInt64 = sUInt64 * 10;
        sUInt64 = sUInt64 + (sIntegerPos[sCount] - '0');
    }

    /* decimal part */
    for( sCount = 0; sCount < sDecimalLen; sCount++ )
    {
        sUInt64 = sUInt64 * 10;
        sUInt64 = sUInt64 + (sDecimalPos[sCount] - '0');
    }

    /* exponent part */
    for( sCount = (sExponent - sDecimalLen); sCount > 0; sCount-- )
    {
        sUInt64 = sUInt64 * 10;
    }

    /* round 결과 반영 */
    sUInt64 += sCarry;
    
    /* check overflow and assign result */
    if( sIsPositive == STL_TRUE )
    {
        STL_TRY_THROW( sUInt64 <= (stlUInt64)STL_INT64_MAX,
                       ERROR_OUT_OF_RANGE );
        *aResult = (stlInt64)sUInt64;
    }
    else
    {
        /* int64 min 값의 절대값은 int64 max 값에 1을 더한 값과 같다. */
        STL_TRY_THROW( sUInt64 <= ((stlUInt64)STL_INT64_MAX + 1),
                       ERROR_OUT_OF_RANGE );
        if( sUInt64 <= (stlUInt64)STL_INT64_MAX )
        {
            *aResult = -(stlInt64)sUInt64;
        }
        else
        {
            *aResult = STL_INT64_MIN;
        }
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_NUMERIC_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_CONVERSION_DATA_LOSS_WHOLE_DIGITS,
                      NULL,
                      aErrorStack );
    }
        
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 
 * @param[in]  aString             입력 문자열
 * @param[in]  aStringLength       입력 문자열 길이
 * @param[out] aResult             integer 변환 값
 * @param[out] aErrorStack         에러 스택
 *
 * 앞뒤 공백만 제거하여 decode 수행.
 * 
 * @remark 10진수로 구성된 문자열만 지원한다. (16진수의 'E'와 expoent의 'E'를 구분할 수 없기 때문에)
 */
stlStatus dtdToInt64FromStringWithoutRound( stlChar         * aString,
                                            stlInt64          aStringLength,
                                            stlInt64        * aResult,
                                            stlErrorStack   * aErrorStack )
{
    stlInt32      sCount;
    stlBool       sIsPositive         = STL_TRUE;
    stlBool       sIsPositiveExponent = STL_TRUE;
    stlChar     * sFirstString        = NULL;
    stlChar     * sLastString         = NULL;
    stlChar     * sIntegerPos         = NULL; /* 정수형 숫자 시작 위치 */
    stlChar     * sDecimalPos         = NULL; /* 소수 숫자 시작 위치 */
    stlChar     * sExponentPos        = NULL; /* exponent 숫자 시작 위치 */
    stlInt32      sIntegerLen         = -1;
    stlInt32      sDecimalLen         = -1;
    stlInt32      sExponentLen        = -1;
    stlInt32      sTruncatedCount     = 0;
    stlInt64      sExponent           = 0;
    stlUInt64     sUInt64             = 0;

    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    DTL_PARAM_VALIDATE( aStringLength >= 0, aErrorStack);
    DTL_PARAM_VALIDATE( aResult != NULL, aErrorStack);

    /**************************************
     *  ANALYZE INPUT STRING
     *************************************/
    sFirstString    = aString;
    sLastString     = aString + aStringLength - 1;
    sIsPositive     = STL_TRUE;

    /* truncate trailing spaces */
    SKIP_TRAILING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* check sign */
    CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositive );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* if current character is digit... */
    if( (sFirstString <= sLastString) && DTF_IS_DIGIT(*sFirstString) )
    {
        sIntegerPos = sFirstString;

        /* skip leading digits */
        SKIP_LEADING_DIGITS( sFirstString, sLastString );
        sIntegerLen = sFirstString - sIntegerPos;
    }
    else
    {
        sIntegerPos = NULL;
        sIntegerLen = 0;
    }

    /* if current character is point... */
    if( (sFirstString <= sLastString) && (*sFirstString == '.') )
    {
        sFirstString++;
        if( sFirstString > sLastString )
        {
            STL_TRY_THROW( sIntegerLen > 0, ERROR_INVALID_INPUT_DATA );
            sDecimalPos = NULL;
            sDecimalLen = 0;
        }
        else
        {
            if( DTF_IS_DIGIT(*sFirstString) )
            {
                sDecimalPos = sFirstString;

                /* skip leading digits */
                SKIP_LEADING_DIGITS( sFirstString, sLastString );
                sDecimalLen = sFirstString - sDecimalPos;
            }
            else
            {
                STL_TRY_THROW( sIntegerLen > 0, ERROR_INVALID_INPUT_DATA );
                sDecimalPos = NULL;
                sDecimalLen = 0;
            }
        }
    }
    else
    {
        sDecimalPos = NULL;
        sDecimalLen = 0;
    }

    /* if current character is exponent... */
    if( (sFirstString <= sLastString) && 
        ( (*sFirstString == 'e') || (*sFirstString == 'E') ) )
    {
        sFirstString++;

        /* check sign */
        CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositiveExponent );
        STL_TRY_THROW( (sFirstString <= sLastString) && DTF_IS_DIGIT(*sFirstString), ERROR_INVALID_INPUT_DATA );

        sExponentPos = sFirstString;

        /* skip leading digits */
        SKIP_LEADING_DIGITS( sFirstString, sLastString );
        sExponentLen = sFirstString - sExponentPos;
    }
    else
    {
        sExponentPos = NULL;
        sExponentLen = 0;
    }

    /* check end of string */
    STL_TRY_THROW( sFirstString == (sLastString + 1), ERROR_INVALID_INPUT_DATA );

    /**************************************
     * ADJUST ANALIZED STRING
     *************************************/

    /* truncate leading zeros on integer characters */
    TRUNCATE_LEADING_ZERO_CHARACTERS( sIntegerPos, sIntegerLen, sTruncatedCount );

    /* truncate trailing zeros on decimal characters */
    TRUNCATE_TRAILING_ZERO_CHARACTERS( sDecimalPos, sDecimalLen, sTruncatedCount );

    /* truncate leading zeros on exponent characters */
    TRUNCATE_LEADING_ZERO_CHARACTERS( sExponentPos, sExponentLen, sTruncatedCount );

    /**************************************
     *  VALIDATE ANALIZED STRING
     *************************************/

    /* change exponent type string to int64 */
    sExponent = 0;
    for( sCount = 0; sCount < sExponentLen; sCount++ )
    {
        sExponent = sExponent * 10;
        sExponent = sExponent + (*(sExponentPos + sCount) - '0');
    }
    if( sIsPositiveExponent == STL_FALSE )
    {
        sExponent = sExponent * (-1);
    }

    /* truncated trailing zeros on integer characters 
     * and add truncated count to exponent value */
    if( sDecimalLen == 0 )
    {
        TRUNCATE_TRAILING_ZERO_CHARACTERS( sIntegerPos, sIntegerLen, sTruncatedCount );
        sExponent = sExponent + sTruncatedCount;
    }

    /* truncated leading zeros on decimal characters 
     * and subtract truncated count to exponent value */
    if( sIntegerLen == 0 )
    {
        TRUNCATE_LEADING_ZERO_CHARACTERS( sDecimalPos, sDecimalLen, sTruncatedCount );
        sExponent = sExponent - sTruncatedCount;
    }

    /* precision validation */
    STL_TRY_THROW( sIntegerLen + sExponent <= DTL_INT64_MAX_DIGIT_COUNT,
                   ERROR_OUT_OF_RANGE );

    /* set carry & rearrange integer and decimal length */
    if( sIntegerLen + sExponent <= 0 )
    {
        sIntegerLen = 0;
        sDecimalLen = 0;
    }
    else
    {
        /* sIntegerLen + sExponent > 0 */
        if( sExponent >= 0 )
        {
            if( sDecimalLen > sExponent )
            {
                sDecimalLen = sExponent;
            }
            else
            {
                sDecimalLen = 0;
            }
        }
        else
        {
            sIntegerLen = sIntegerLen + sExponent;
            sDecimalLen = 0;
        }
    }

    /**************************************
     *  MAKE RESULT
     *************************************/

    /* uint64의 max digit count가 int64보다 1 크기 때문에
     * uint64에 값을 넣으면 절대로 overflow가 발생하지 않는다. 
     * 따라서 uint64에 값을 넣은 다음 overflow체크를 수행하고
     * 최종 결과에 값을 넣는다. */
    sUInt64 = 0;
    /* integer part */
    for( sCount = 0; sCount < sIntegerLen; sCount++ )
    {
        sUInt64 = sUInt64 * 10;
        sUInt64 = sUInt64 + (sIntegerPos[sCount] - '0');
    }

    /* decimal part */
    for( sCount = 0; sCount < sDecimalLen; sCount++ )
    {
        sUInt64 = sUInt64 * 10;
        sUInt64 = sUInt64 + (sDecimalPos[sCount] - '0');
    }

    /* exponent part */
    for( sCount = (sExponent - sDecimalLen); sCount > 0; sCount-- )
    {
        sUInt64 = sUInt64 * 10;
    }
    
    /* check overflow and assign result */
    if( sIsPositive == STL_TRUE )
    {
        STL_TRY_THROW( sUInt64 <= (stlUInt64)STL_INT64_MAX,
                       ERROR_OUT_OF_RANGE );
        *aResult = (stlInt64)sUInt64;
    }
    else
    {
        /* int64 min 값의 절대값은 int64 max 값에 1을 더한 값과 같다. */
        STL_TRY_THROW( sUInt64 <= ((stlUInt64)STL_INT64_MAX + 1),
                       ERROR_OUT_OF_RANGE );
        if( sUInt64 <= (stlUInt64)STL_INT64_MAX )
        {
            *aResult = -(stlInt64)sUInt64;
        }
        else
        {
            *aResult = STL_INT64_MIN;
        }
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_NUMERIC_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_CONVERSION_DATA_LOSS_WHOLE_DIGITS,
                      NULL,
                      aErrorStack );
    }
        
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 
 * @param[in]  aString             입력 문자열
 * @param[in]  aStringLength       입력 문자열 길이
 * @param[out] aResult             integer 변환 값
 * @param[out] aErrorStack         에러 스택
 *
 * 앞뒤 공백만 제거하여 decode 수행.
 * 
 * @remark 10진수로 구성된 문자열만 지원한다. (16진수의 'E'와 expoent의 'E'를 구분할 수 없기 때문에)
 */
stlStatus dtdToUInt64FromString( stlChar         * aString,
                                 stlInt64          aStringLength,
                                 stlUInt64       * aResult,
                                 stlErrorStack   * aErrorStack )
{
    stlInt32      sCount;
    stlBool       sIsPositive       = STL_TRUE;
    stlBool       sIsPositiveExponent   = STL_TRUE;
    stlChar     * sFirstString      = NULL;
    stlChar     * sLastString       = NULL;
    stlChar     * sIntegerPos       = NULL; /* 정수형 숫자 시작 위치 */
    stlChar     * sDecimalPos       = NULL; /* 소수 숫자 시작 위치 */
    stlChar     * sExponentPos      = NULL; /* exponent 숫자 시작 위치 */
    stlInt32      sIntegerLen       = -1;
    stlInt32      sDecimalLen       = -1;
    stlInt32      sExponentLen      = -1;
    stlInt32      sTruncatedCount   = 0;
    stlInt64      sExponent         = 0;
    stlUInt64     sUInt64           = 0;
    stlUInt64     sCarry            = 0;

    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    DTL_PARAM_VALIDATE( aStringLength >= 0, aErrorStack);
    DTL_PARAM_VALIDATE( aResult != NULL, aErrorStack);

    /**************************************
     *  ANALYZE INPUT STRING
     *************************************/
    sFirstString    = aString;
    sLastString     = aString + aStringLength - 1;
    sIsPositive     = STL_TRUE;

    /* truncate trailing spaces */
    SKIP_TRAILING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* check sign */
    CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositive );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* if current character is digit... */
    if( (sFirstString <= sLastString) && DTF_IS_DIGIT(*sFirstString) )
    {
        sIntegerPos = sFirstString;

        /* skip leading digits */
        SKIP_LEADING_DIGITS( sFirstString, sLastString );
        sIntegerLen = sFirstString - sIntegerPos;
    }
    else
    {
        sIntegerPos = NULL;
        sIntegerLen = 0;
    }

    /* if current character is point... */
    if( (sFirstString <= sLastString) && (*sFirstString == '.') )
    {
        sFirstString++;
        if( sFirstString > sLastString )
        {
            STL_TRY_THROW( sIntegerLen > 0, ERROR_INVALID_INPUT_DATA );
            sDecimalPos = NULL;
            sDecimalLen = 0;
        }
        else
        {
            if( DTF_IS_DIGIT(*sFirstString) )
            {
                sDecimalPos = sFirstString;

                /* skip leading digits */
                SKIP_LEADING_DIGITS( sFirstString, sLastString );
                sDecimalLen = sFirstString - sDecimalPos;
            }
            else
            {
                STL_TRY_THROW( sIntegerLen > 0, ERROR_INVALID_INPUT_DATA );
                sDecimalPos = NULL;
                sDecimalLen = 0;
            }
        }
    }
    else
    {
        sDecimalPos = NULL;
        sDecimalLen = 0;
    }

    /* if current character is exponent... */
    if( (sFirstString <= sLastString) && 
        ( (*sFirstString == 'e') || (*sFirstString == 'E') ) )
    {
        sFirstString++;

        /* check sign */
        CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositiveExponent );
        STL_TRY_THROW( (sFirstString <= sLastString) && DTF_IS_DIGIT(*sFirstString), ERROR_INVALID_INPUT_DATA );

        sExponentPos = sFirstString;

        /* skip leading digits */
        SKIP_LEADING_DIGITS( sFirstString, sLastString );
        sExponentLen = sFirstString - sExponentPos;
    }
    else
    {
        sExponentPos = NULL;
        sExponentLen = 0;
    }

    /* check end of string */
    STL_TRY_THROW( sFirstString == (sLastString + 1), ERROR_INVALID_INPUT_DATA );

    /**************************************
     * ADJUST ANALIZED STRING
     *************************************/

    /* truncate leading zeros on integer characters */
    TRUNCATE_LEADING_ZERO_CHARACTERS( sIntegerPos, sIntegerLen, sTruncatedCount );

    /* truncate trailing zeros on decimal characters */
    TRUNCATE_TRAILING_ZERO_CHARACTERS( sDecimalPos, sDecimalLen, sTruncatedCount );

    /* truncate leading zeros on exponent characters */
    TRUNCATE_LEADING_ZERO_CHARACTERS( sExponentPos, sExponentLen, sTruncatedCount );

    /**************************************
     *  VALIDATE ANALIZED STRING
     *************************************/

    /* change exponent type string to int64 */
    sExponent = 0;
    for( sCount = 0; sCount < sExponentLen; sCount++ )
    {
        sExponent = sExponent * 10;
        sExponent = sExponent + (*(sExponentPos + sCount) - '0');
    }
    if( sIsPositiveExponent == STL_FALSE )
    {
        sExponent = sExponent * (-1);
    }

    /* truncated trailing zeros on integer characters 
     * and add truncated count to exponent value */
    if( sDecimalLen == 0 )
    {
        TRUNCATE_TRAILING_ZERO_CHARACTERS( sIntegerPos, sIntegerLen, sTruncatedCount );
        sExponent = sExponent + sTruncatedCount;
    }

    /* truncated leading zeros on decimal characters 
     * and subtract truncated count to exponent value */
    if( sIntegerLen == 0 )
    {
        TRUNCATE_LEADING_ZERO_CHARACTERS( sDecimalPos, sDecimalLen, sTruncatedCount );
        sExponent = sExponent - sTruncatedCount;
    }

    /* precision validation */
    STL_TRY_THROW( sIntegerLen + sExponent <= DTL_UINT64_MAX_DIGIT_COUNT,
                   ERROR_OUT_OF_RANGE );

    /* set carry & rearrange integer and decimal length */
    if( sIntegerLen + sExponent == 0 )
    {
        if( sDecimalLen > 0 )
        {
            sCarry = ((sDecimalPos[0] - '0') + 5) / 10;
        }
        else
        {
            sCarry = 0;
        }
        sIntegerLen = 0;
        sDecimalLen = 0;
    }
    else if( sIntegerLen + sExponent < 0 )
    {
        sCarry = 0;
        sIntegerLen = 0;
        sDecimalLen = 0;
    }
    else
    {
        /* sIntegerLen + sExponent > 0 */
        if( sExponent >= 0 )
        {
            if( sDecimalLen > sExponent )
            {
                sCarry = ((sDecimalPos[sExponent] - '0') + 5) / 10;
                sDecimalLen = sExponent;
            }
            else
            {
                sCarry = 0;
            }
        }
        else
        {
            sCarry = ((sIntegerPos[sIntegerLen + sExponent] - '0') + 5) / 10;
            sIntegerLen = sIntegerLen + sExponent;
            sDecimalLen = 0;
        }
    }

    /**************************************
     *  MAKE RESULT
     *************************************/

    if( (sIntegerLen + sExponent) == DTL_UINT64_MAX_DIGIT_COUNT )
    {
        /* 최종 결과의 digit count가 uint64의 max digit count와 같으므로
         * overflow가 발생할 수 있다. */

        sUInt64 = 0;
        /* integer part */
        for( sCount = 0; sCount < sIntegerLen; sCount++ )
        {
            sUInt64 = sUInt64 * 10;
            sUInt64 = sUInt64 + (sIntegerPos[sCount] - '0');
        }

        /* decimal part */
        for( sCount = 0; sCount < sDecimalLen; sCount++ )
        {
            sUInt64 = sUInt64 * 10;
            sUInt64 = sUInt64 + (sDecimalPos[sCount] - '0');
        }

        /* exponent part */
        for( sCount = (sExponent - sDecimalLen); sCount > 0; sCount-- )
        {
            sUInt64 = sUInt64 * 10;
        }

        STL_TRY_THROW( ( sUInt64 / 10 < gPreDefinedPow[ DTL_UINT64_MAX_DIGIT_COUNT - 1 ] ) &&
                       ( sUInt64 / 10 >= gPreDefinedPow[ DTL_UINT64_MAX_DIGIT_COUNT - 2 ] ),
                       ERROR_OUT_OF_RANGE );
    }
    else
    {
        /* 최종 결과의 digit count가 uint64의 max digit count보다 항상 작으므로
         * overflow가 발생하지 않는다. */
        sUInt64 = 0;
        /* integer part */
        for( sCount = 0; sCount < sIntegerLen; sCount++ )
        {
            sUInt64 = sUInt64 * 10;
            sUInt64 = sUInt64 + (sIntegerPos[sCount] - '0');
        }

        /* decimal part */
        for( sCount = 0; sCount < sDecimalLen; sCount++ )
        {
            sUInt64 = sUInt64 * 10;
            sUInt64 = sUInt64 + (sDecimalPos[sCount] - '0');
        }

        /* exponent part */
        for( sCount = (sExponent - sDecimalLen); sCount > 0; sCount-- )
        {
            sUInt64 = sUInt64 * 10;
        }

        /* round 결과 반영 */
        sUInt64 += sCarry;
    }

    STL_TRY_THROW( ( (sIsPositive == STL_TRUE) || (sUInt64 == 0) ),
                   ERROR_OUT_OF_RANGE );

    *aResult = sUInt64;

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_NUMERIC_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_CONVERSION_DATA_LOSS_WHOLE_DIGITS,
                      NULL,
                      aErrorStack );
    }
        
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 
 * @param[in]  aString             입력 문자열
 * @param[in]  aStringLength       입력 문자열 길이
 * @param[out] aResult             float32 변환 값
 * @param[out] aErrorStack         에러 스택
 *
 * 앞뒤 공백만 제거하여 decode 수행.
 * 
 * @remark 10진수로 구성된 문자열만 지원한다. (16진수의 'E'와 expoent의 'E'를 구분할 수 없기 때문에)
 */
stlStatus dtdToFloat32FromString( stlChar         * aString,
                                  stlInt64          aStringLength,
                                  stlFloat32      * aResult,
                                  stlErrorStack   * aErrorStack )
{
    stlBool                   sSign;
    stlBool                   sExpSign;
    stlBool                   sHasCarry;
    register const stlChar  * sPtr;
    register const stlChar  * sFence;
    const stlChar           * sExpPtr;
    register stlInt32         sChar;
    stlInt32                  sMantSize;
    stlInt32                  sIntegerLen;
    stlInt32                  sDecPt;
    stlInt32                  sFracExp;
    stlInt32                  sFrac1;
    stlInt32                  sFrac2;
    stlInt32                  sExpValue;
    stlFloat32                sFraction;
    stlFloat32                sResult;
    stlFloat64                sPow;
    stlBool                   sIsExistLeadingZeroNum = STL_FALSE;

    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    DTL_PARAM_VALIDATE( aStringLength >= 0, aErrorStack);
    DTL_PARAM_VALIDATE( aResult != NULL, aErrorStack);

    sPtr = aString;
    sFence = aString + aStringLength;

    /* skip leading space */
    while( (sPtr < sFence) && stlIsspace(*sPtr) )
    {
        sPtr += 1;
    }
    STL_TRY_THROW( sPtr < sFence, ERROR_INVALID_INPUT_DATA );

    /* skip trailing space */
    while( (sPtr < sFence) && stlIsspace(*(sFence - 1)) )
    {
        sFence -= 1;
    }
    STL_TRY_THROW( sPtr < sFence, ERROR_INVALID_INPUT_DATA );

    /* check sign */
    if( *sPtr == '-' )
    {
        sSign = STL_TRUE;
        sPtr += 1;
    }
    else
    {
        if( *sPtr == '+' )
        {
            sPtr += 1;
        }
        sSign = STL_FALSE;
    }
    STL_TRY_THROW( sPtr < sFence, ERROR_INVALID_INPUT_DATA );
    
    /* skip space */
    while( (sPtr < sFence) && stlIsspace(*sPtr) )
    {
        sPtr += 1;
    }
    STL_TRY_THROW( sPtr < sFence, ERROR_INVALID_INPUT_DATA );
    
    /* skip leading zero characters */
    while( (sPtr < sFence) && (*sPtr == '0') )
    {
        sPtr += 1;
        sIsExistLeadingZeroNum = STL_TRUE;
    }

    sFracExp = 0;
    if( ( sPtr + 1 ) < sFence )
    {
        if( (*sPtr == '.') && (*(sPtr+1) == '0') )
        {
            sPtr += 1;
            while( (sPtr < sFence) && (*sPtr == '0') )
            {
                sFracExp -= 1;
                sPtr += 1;
            }
        }
        else
        {
            if( (*sPtr == '.') && ((*(sPtr+1) == 'E') || (*(sPtr+1) == 'e')) )
            {
                sPtr += 1;
            }
            else
            {
                /* do nothing */
            }
        }
    }
    else
    {
        /* do nothing */
    }

    /* count the number of digits */
    sDecPt = -1;
    for( sMantSize = 0; sPtr < sFence; sMantSize += 1 )
    {
        sChar = *sPtr;
        if( !stlIsdigit(sChar) )
        {
            if( (sChar != '.') || (sDecPt >= 0) )
            {
                break;
            }
            sDecPt = sMantSize;
        }
        sPtr += 1;
    }
    STL_TRY_THROW( (sPtr <= sFence) || (*sPtr == 'e') || (*sPtr == 'E'),
                   ERROR_INVALID_INPUT_DATA );

    sExpPtr = sPtr;
    sPtr -= sMantSize;
    if( sDecPt < 0 )
    {
        sDecPt = sMantSize;
    }
    else
    {
        sMantSize -= 1;
    }

    if( sMantSize > STL_FLT_DIG )
    {
        /* check carry */
        if( *(sPtr + STL_FLT_DIG + ((sDecPt > STL_FLT_DIG) ? 0 : 1)) < '5' )
        {
            sHasCarry = STL_FALSE;
        }
        else
        {
            sHasCarry = STL_TRUE;
        }

        if( sFracExp == 0 )
        {
            sFracExp += (sDecPt - STL_FLT_DIG);
        }
        else
        {
            sFracExp -= STL_FLT_DIG;
        }
        sMantSize = STL_FLT_DIG;
    }
    else
    {
        sHasCarry = STL_FALSE;
        if( sFracExp == 0 )
        {
            sFracExp += (sDecPt - sMantSize);
        }
        else
        {
            sFracExp -= sMantSize;
        }
    }

    if( sMantSize == 0 )
    {
        while( (sPtr < sFence) && stlIsspace(*sPtr) )
        {
            sPtr += 1;
        }

        /*
         * ex) '.0'
         * ex) '0E+00', '.0E+00'
         */
        STL_TRY_THROW( (sPtr == sFence) ||
                       ( (sPtr < sFence)
                         && ((*sPtr == 'E') || (*sPtr == 'e'))
                         && ((sIsExistLeadingZeroNum == STL_TRUE) || (sFracExp < 0)) ),
                       ERROR_INVALID_INPUT_DATA );
        
        if( (sPtr < sFence) && ((*sPtr == 'E') || (*sPtr == 'e')) )
        {
            /* ( (sPtr < sFence) && ((*sPtr == 'E') || (*sPtr == 'e')) ) */

            /* check exponent */
            sPtr = sExpPtr;
            STL_TRY_THROW( (*sPtr == 'E') || (*sPtr == 'e'),
                           ERROR_INVALID_INPUT_DATA );
            sPtr += 1;
            STL_TRY_THROW( sPtr < sFence, ERROR_INVALID_INPUT_DATA );
            if( ( *sPtr == '-' ) || ( *sPtr == '+' ) )
            {
                sPtr += 1;
            }
            else
            {
                /* Do Nothing */
            }
            
            STL_TRY_THROW( (sPtr < sFence) && stlIsdigit(*sPtr),
                           ERROR_INVALID_INPUT_DATA );
            while( (sPtr < sFence) && stlIsdigit(*sPtr) )
            {
                sPtr += 1;
            }

            /* skip space */
            while( (sPtr < sFence) && stlIsspace(*sPtr) )
            {
                sPtr += 1;
            }
            STL_TRY_THROW( sPtr == sFence, ERROR_INVALID_INPUT_DATA );
        }
        else
        {
            /* (sPtr == sFence) */
            /* Do Nothing */
        }
        
        sResult = 0.0;
        STL_THROW( RAMP_SUCCESS );
    }

    sIntegerLen = sMantSize;

    sFrac1 = 0;
    for( ; sMantSize > 9; sMantSize -= 1 )
    {
        sChar = *sPtr;
        sPtr += 1;
        if( sChar == '.')
        {
            sChar = *sPtr;
            sPtr +=1;
        }
        sFrac1 = 10 * sFrac1 + (sChar - '0');
    }
    sFrac2 = 0;
    for( ; sMantSize > 0; sMantSize -= 1 )
    {
        sChar = *sPtr;
        sPtr += 1;
        if( sChar == '.' )
        {
            sChar = *sPtr;
            sPtr += 1;
        }
        sFrac2 = 10 * sFrac2 + (sChar - '0');
    }

    sFraction = (1.0e9 * sFrac1) + sFrac2 + ((sHasCarry == STL_TRUE) ? 1: 0);

    /* check exponent */
    sPtr = sExpPtr;
    sExpValue = 0;
    sExpSign = STL_FALSE;
    if( (sPtr < sFence) && ((*sPtr == 'E') || (*sPtr == 'e')) )
    {
        STL_TRY_THROW( (*sPtr == 'E') || (*sPtr == 'e'),
                       ERROR_INVALID_INPUT_DATA );
        sPtr += 1;
        STL_TRY_THROW( sPtr < sFence, ERROR_INVALID_INPUT_DATA );
        if( *sPtr == '-' )
        {
            sExpSign = STL_TRUE;
            sPtr += 1;
        }
        else
        {
            if( *sPtr == '+' )
            {
                sPtr += 1;
            }
            sExpSign = STL_FALSE;
        }

        STL_TRY_THROW( (sPtr < sFence) && stlIsdigit(*sPtr),
                       ERROR_INVALID_INPUT_DATA );
        while( (sPtr < sFence) && stlIsdigit(*sPtr) )
        {
            sExpValue = sExpValue * 10 + (*sPtr - '0');
            sPtr += 1;
        }
    }

    /* skip space */
    while( (sPtr < sFence) && stlIsspace(*sPtr) )
    {
        sPtr += 1;
    }
    STL_TRY_THROW( sPtr == sFence, ERROR_INVALID_INPUT_DATA );

    if( sExpSign )
    {
        sExpValue = sFracExp - sExpValue;
    }
    else
    {
        sExpValue = sFracExp + sExpValue;
    }

    if( sIntegerLen > 1 )
    {
        STL_TRY_THROW( stlGet10Pow( sIntegerLen - 1, &sPow, aErrorStack ) == STL_TRUE,
                       ERROR_OUT_OF_RANGE );
        sFraction /= sPow;
        sExpValue += (sIntegerLen - 1);
    }

    STL_TRY_THROW( stlGet10Pow( sExpValue, &sPow, aErrorStack ) == STL_TRUE,
                   ERROR_OUT_OF_RANGE );

    sResult = sFraction * sPow;

    STL_TRY_THROW( stlIsfinite(sResult)
                   != STL_IS_FINITE_FALSE,
                   ERROR_OUT_OF_RANGE );

    STL_TRY_THROW( stlIsnan(sResult)
                   == STL_IS_NAN_FALSE,
                   ERROR_OUT_OF_RANGE );

    STL_RAMP( RAMP_SUCCESS );

    if( sSign )
    {
        *aResult = -sResult;
    }
    else
    {
        *aResult = sResult;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_STRING_NOT_NUMERIC_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_OUTSIDE_RANGE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 
 * @param[in]  aString             입력 문자열
 * @param[in]  aStringLength       입력 문자열 길이
 * @param[out] aResult             float64 변환 값
 * @param[out] aErrorStack         에러 스택
 *
 * 앞뒤 공백만 제거하여 decode 수행.
 * 
 * @remark 10진수로 구성된 문자열만 지원한다. (16진수의 'E'와 expoent의 'E'를 구분할 수 없기 때문에)
 */
stlStatus dtdToFloat64FromString( stlChar         * aString,
                                  stlInt64          aStringLength,
                                  stlFloat64      * aResult,
                                  stlErrorStack   * aErrorStack )
{
    stlBool                   sSign;
    stlBool                   sExpSign;
    stlBool                   sHasCarry;
    register const stlChar  * sPtr;
    register const stlChar  * sFence;
    const stlChar           * sExpPtr;
    register stlInt32         sChar;
    stlInt32                  sMantSize;
    stlInt32                  sIntegerLen;
    stlInt32                  sDecPt;
    stlInt32                  sDiffExp;
    stlInt32                  sFracExp;
    stlInt32                  sFrac1;
    stlInt32                  sFrac2;
    stlInt32                  sExpValue;
    stlFloat64                sFraction;
    stlFloat64                sResult;
    stlFloat64                sPow;
    stlBool                   sIsExistLeadingZeroNum = STL_FALSE;

    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack);
    DTL_PARAM_VALIDATE( aStringLength >= 0, aErrorStack);
    DTL_PARAM_VALIDATE( aResult != NULL, aErrorStack);

    sPtr = aString;
    sFence = aString + aStringLength;

    /* skip leading space */
    while( (sPtr < sFence) && stlIsspace(*sPtr) )
    {
        sPtr += 1;
    }
    STL_TRY_THROW( sPtr < sFence, ERROR_INVALID_INPUT_DATA );

    /* skip trailing space */
    while( (sPtr < sFence) && stlIsspace(*(sFence - 1)) )
    {
        sFence -= 1;
    }
    STL_TRY_THROW( sPtr < sFence, ERROR_INVALID_INPUT_DATA );

    /* check sign */
    if( *sPtr == '-' )
    {
        sSign = STL_TRUE;
        sPtr += 1;
    }
    else
    {
        if( *sPtr == '+' )
        {
            sPtr += 1;
        }
        sSign = STL_FALSE;
    }
    STL_TRY_THROW( sPtr < sFence, ERROR_INVALID_INPUT_DATA );

    /* skip space */
    while( (sPtr < sFence) && stlIsspace(*sPtr) )
    {
        sPtr += 1;
    }
    STL_TRY_THROW( sPtr < sFence, ERROR_INVALID_INPUT_DATA );

    /* skip leading zero characters */
    while( (sPtr < sFence) && (*sPtr == '0') )
    {
        sPtr += 1;
        sIsExistLeadingZeroNum = STL_TRUE;
    }
    
    sFracExp = 0;
    if( ( sPtr + 1 ) < sFence )
    {
        if( (*sPtr == '.') && (*(sPtr+1) == '0') )
        {
            sPtr += 1;
            while( (sPtr < sFence) && (*sPtr == '0') )
            {
                sFracExp -= 1;
                sPtr += 1;
            }
        }
        else
        {
            if( (*sPtr == '.') && ((*(sPtr+1) == 'E') || (*(sPtr+1) == 'e')) )
            {
                sPtr += 1;
            }
            else
            {
                /* do nothing */
            }
        }
    }
    else
    {
        /* do nothing */
    }
    
    /* count the number of digits */
    sDecPt = -1;
    for( sMantSize = 0; sPtr < sFence; sMantSize += 1 )
    {
        sChar = *sPtr;
        if( !stlIsdigit(sChar) )
        {
            if( (sChar != '.') || (sDecPt >= 0) )
            {
                break;
            }
            sDecPt = sMantSize;
        }
        sPtr += 1;
    }
    STL_TRY_THROW( (sPtr <= sFence) || (*sPtr == 'e') || (*sPtr == 'E'),
                   ERROR_INVALID_INPUT_DATA );

    sExpPtr = sPtr;
    sPtr -= sMantSize;
    if( sDecPt < 0 )
    {
        sDecPt = sMantSize;
    }
    else
    {
        sMantSize -= 1;
    }

    if( sMantSize > STL_DBL_DIG )
    {
        /* check carry */
        if( *(sPtr + STL_DBL_DIG + ((sDecPt > STL_DBL_DIG) ? 0 : 1)) < '5' )
        {
            sHasCarry = STL_FALSE;
        }
        else
        {
            sHasCarry = STL_TRUE;
        }

        if( sFracExp == 0 )
        {
            sFracExp += (sDecPt - STL_DBL_DIG);
        }
        else
        {
            sFracExp -= STL_DBL_DIG;
        }
        sMantSize = STL_DBL_DIG;
    }
    else
    {
        sHasCarry = STL_FALSE;
        if( sFracExp == 0 )
        {
            sFracExp += (sDecPt - sMantSize);
        }
        else
        {
            sFracExp -= sMantSize;
        }
    }

    if( sMantSize == 0 )
    {
        while( (sPtr < sFence) && stlIsspace(*sPtr) )
        {
            sPtr += 1;
        }

        /*
         * ex) '.0'
         * ex) '0E+00', '.0E+00'
         */
        STL_TRY_THROW( (sPtr == sFence) ||
                       ( (sPtr < sFence)
                         && ((*sPtr == 'E') || (*sPtr == 'e'))
                         && ((sIsExistLeadingZeroNum == STL_TRUE) || (sFracExp < 0)) ),
                       ERROR_INVALID_INPUT_DATA );
        
        if( (sPtr < sFence) && ((*sPtr == 'E') || (*sPtr == 'e')) )
        {
            /* ( (sPtr < sFence) && ((*sPtr == 'E') || (*sPtr == 'e')) ) */

            /* check exponent */
            sPtr = sExpPtr;
            STL_TRY_THROW( (*sPtr == 'E') || (*sPtr == 'e'),
                           ERROR_INVALID_INPUT_DATA );
            sPtr += 1;
            STL_TRY_THROW( sPtr < sFence, ERROR_INVALID_INPUT_DATA );
            if( ( *sPtr == '-' ) || ( *sPtr == '+' ) )
            {
                sPtr += 1;
            }
            else
            {
                /* Do Nothing */
            }
            
            STL_TRY_THROW( (sPtr < sFence) && stlIsdigit(*sPtr),
                           ERROR_INVALID_INPUT_DATA );
            while( (sPtr < sFence) && stlIsdigit(*sPtr) )
            {
                sPtr += 1;
            }

            /* skip space */
            while( (sPtr < sFence) && stlIsspace(*sPtr) )
            {
                sPtr += 1;
            }
            STL_TRY_THROW( sPtr == sFence, ERROR_INVALID_INPUT_DATA );
        }
        else
        {
            /* (sPtr == sFence) */
            /* Do Nothing */
        }

        sResult = 0.0;
        STL_THROW( RAMP_SUCCESS );
    }

    sIntegerLen = sMantSize;

    sFrac1 = 0;
    for( ; sMantSize > 9; sMantSize -= 1 )
    {
        sChar = *sPtr;
        sPtr += 1;
        if( sChar == '.')
        {
            sChar = *sPtr;
            sPtr +=1;
        }
        sFrac1 = 10 * sFrac1 + (sChar - '0');
    }
    sFrac2 = 0;
    for( ; sMantSize > 0; sMantSize -= 1 )
    {
        sChar = *sPtr;
        sPtr += 1;
        if( sChar == '.' )
        {
            sChar = *sPtr;
            sPtr += 1;
        }
        sFrac2 = 10 * sFrac2 + (sChar - '0');
    }

    sFraction = (1.0e9 * sFrac1) + sFrac2 + ((sHasCarry == STL_TRUE) ? 1: 0);

    /* check exponent */
    sPtr = sExpPtr;
    sExpValue = 0;
    sExpSign = STL_FALSE;
    if( (sPtr < sFence) && ((*sPtr == 'E') || (*sPtr == 'e')) )
    {
        STL_TRY_THROW( (*sPtr == 'E') || (*sPtr == 'e'),
                       ERROR_INVALID_INPUT_DATA );
        sPtr += 1;
        STL_TRY_THROW( sPtr < sFence, ERROR_INVALID_INPUT_DATA );
        if( *sPtr == '-' )
        {
            sExpSign = STL_TRUE;
            sPtr += 1;
        }
        else
        {
            if( *sPtr == '+' )
            {
                sPtr += 1;
            }
            sExpSign = STL_FALSE;
        }

        STL_TRY_THROW( (sPtr < sFence) && stlIsdigit(*sPtr),
                       ERROR_INVALID_INPUT_DATA );
        while( (sPtr < sFence) && stlIsdigit(*sPtr) )
        {
            sExpValue = sExpValue * 10 + (*sPtr - '0');
            sPtr += 1;
        }
    }

    /* skip space */
    while( (sPtr < sFence) && stlIsspace(*sPtr) )
    {
        sPtr += 1;
    }
    STL_TRY_THROW( sPtr == sFence, ERROR_INVALID_INPUT_DATA );

    if( sExpSign )
    {
        sExpValue = sFracExp - sExpValue;
    }
    else
    {
        sExpValue = sFracExp + sExpValue;
    }

    /**
     * 1E-323D == 10E-324D == 9.8....E-324D
     * min_exponent 는 -323 이지만 위처럼 -324 형태로
     * 표혈 될 수 있기 때문에 아래와 같이 처리
     * (1) 9.8...  / 10 -> 0.98...
     * (2) E-324D  * 10 -> E-323D
     *    ( stlGet10Pow() 은 -323 까지 사용 가능함으로 (1),(2) 단계를 거친다. )
     * (3) 0.98... * E-323D -> 1E-323D
     * (4) stlIsfinite() 로 validation 확인
     */
    if( sIntegerLen > 1 )
    {
        if( sExpSign )
        {
            /* - */
            sDiffExp = (sExpValue + sIntegerLen - 1) < DTL_FLOAT64_MIN_EXPONENT ?
                ( -DTL_FLOAT64_MIN_EXPONENT + sExpValue + sIntegerLen - 1) : 0;
        }
        else
        {
            /* + */
            sDiffExp = (sExpValue + sIntegerLen - 1) > DTL_FLOAT64_MAX_EXPONENT ?
                ( -DTL_FLOAT64_MAX_EXPONENT + sExpValue + sIntegerLen - 1) : 0;
        }
        STL_TRY_THROW( stlGet10Pow( sIntegerLen - 1 - sDiffExp , &sPow, aErrorStack ) == STL_TRUE,
                       ERROR_OUT_OF_RANGE );
        sFraction /= sPow;
        sExpValue += (sIntegerLen - 1 - sDiffExp);
    }
    else
    {
        /* Do Nothing */
    }

    STL_TRY_THROW( stlGet10Pow( sExpValue, &sPow, aErrorStack ) == STL_TRUE,
                   ERROR_OUT_OF_RANGE );
    
    sResult = sFraction * sPow;
    
    STL_TRY_THROW( DTL_CHECK_FLOAT_VALUE_UNDERFLOW( sResult, sFraction == 0 )
                   == STL_TRUE,
                   ERROR_OUT_OF_RANGE );
    
    STL_TRY_THROW( stlIsfinite(sResult)
                   != STL_IS_FINITE_FALSE,
                   ERROR_OUT_OF_RANGE );

    STL_TRY_THROW( stlIsnan(sResult)
                   == STL_IS_NAN_FALSE,
                   ERROR_OUT_OF_RANGE );

    STL_RAMP( RAMP_SUCCESS );

    if( sSign )
    {
        *aResult = -sResult;
    }
    else
    {
        *aResult = sResult;
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_STRING_NOT_NUMERIC_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_OUTSIDE_RANGE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief stlInt64를 string으로 변환한다.
 * @param[in]  aValue         stlInt64 value
 * @param[out] aResult        string으로 변환된 결과값이 저장될 주소
 * @param[out] aResultLength  string으로 변환된 후 전체 길이 
 */
void dtdInt64ToString( stlInt64        aValue,
                       stlChar       * aResult,
                       stlInt64      * aResultLength )
{
    stlInt64    sLength;
    stlInt64    sValue;
    stlInt64    sQuotient;
    stlInt16    sRemain[6];
    stlInt32    i;

    if( aValue == 0 )
    {
        *aResult = '0';
        *aResultLength = 1;
    }
    else
    {
        if( aValue < 0 )
        {
            if( aValue == STL_INT64_MIN )
            {
                stlMemcpy( aResult, "-9223372036854775808", 20 );
                *aResultLength = 20;
                STL_THROW( RAMP_FINISH );
            }
            sValue = -aValue;
            *aResult = '-';
            ++aResult;
            sLength = 1;
        }
        else
        {
            sValue = aValue;
            sLength = 0;
        }

        i = 0;
        while( sValue >= 10000 )
        {
            sQuotient = sValue / 10000;
            sRemain[i] = sValue - (sQuotient * 10000);
            ++i;
            sValue = sQuotient;
        }
        sRemain[i] = sValue;
        if( sRemain[i] < 10 )
        {
            *aResult = dtdTable1[sRemain[i]][0];
            ++aResult;
            sLength += 1;
        }
        else if( sRemain[i] < 100 )
        {
            *aResult = dtdTable2[sRemain[i]][0];
            ++aResult;
            *aResult = dtdTable2[sRemain[i]][1];
            ++aResult;
            sLength += 2;
        }
        else if( sRemain[i] < 1000 )
        {
            *aResult = dtdTable3[sRemain[i]][0];
            ++aResult;
            *aResult = dtdTable3[sRemain[i]][1];
            ++aResult;
            *aResult = dtdTable3[sRemain[i]][2];
            ++aResult;
            sLength += 3;
        }
        else
        {
            *aResult = dtdTable4[sRemain[i]][0];
            ++aResult;
            *aResult = dtdTable4[sRemain[i]][1];
            ++aResult;
            *aResult = dtdTable4[sRemain[i]][2];
            ++aResult;
            *aResult = dtdTable4[sRemain[i]][3];
            ++aResult;
            sLength += 4;
        }

        *aResultLength = sLength + (4 * i);

        while( i > 0 )
        {
            --i;
            *aResult = dtdTable4[sRemain[i]][0];
            ++aResult;
            *aResult = dtdTable4[sRemain[i]][1];
            ++aResult;
            *aResult = dtdTable4[sRemain[i]][2];
            ++aResult;
            *aResult = dtdTable4[sRemain[i]][3];
            ++aResult;
        }
    }

    STL_RAMP( RAMP_FINISH );
}


/**
 * @brief stlUInt64를 string으로 변환한다.
 * @param[in]  aValue         stlUInt64 value
 * @param[out] aResult        string으로 변환된 결과값이 저장될 주소
 * @param[out] aResultLength  string으로 변환된 후 전체 길이 
 */
void dtdUInt64ToString( stlUInt64        aValue,
                        stlChar        * aResult,
                        stlInt64       * aResultLength )
{
    stlUInt64     sAbsValue;
    stlInt64      sLength;
    stlChar     * sBufferPtr;
    stlChar       sNumber[11] = "0123456789";
        
    if( aValue == 0 )
    {
        *aResult = '0';
        *aResultLength = 1;
    }
    else
    {
        sAbsValue = aValue;
        sLength = 0;
        while( sAbsValue != 0 )
        {
            sLength++;
            sAbsValue /= 10;
        }

        sBufferPtr = aResult + sLength;
        sAbsValue = aValue;
        while( sAbsValue != 0 )
        {
            sBufferPtr--;
            *sBufferPtr = sNumber[sAbsValue % 10];
            sAbsValue /= 10;
        }
        *aResultLength = sLength;
    }
}


/**
 * @brief 숫자문자열을 주어진 길이만큼 resizing 한다.
 * @param[in]  aNumberString         value
 * @param[in]  aNumberStringLength   value 길이
 * @param[in]  aAvailableSize        resizing 길이 
 * @param[out] aResult               resizing 된 문자열이 저장될 buffer
 * @param[out] aResultLength         resizing 된 문자열의 길이
 * @param[out] aErrorStack           에러 스택 
 */
stlStatus dtdResizeNumberStringToString( stlChar         * aNumberString,
                                         stlInt64          aNumberStringLength,
                                         stlInt64          aAvailableSize,
                                         stlChar         * aResult,
                                         stlInt64        * aResultLength,
                                         stlErrorStack   * aErrorStack )
{
    stlChar    sNumberString[ DTL_NUMERIC_STRING_MAX_SIZE ];
    stlBool    sIsPositive           = STL_TRUE;
    stlBool    sIsPositiveExponent   = STL_TRUE;
    stlChar  * sFirstString          = NULL;
    stlChar  * sLastString           = NULL;
    stlInt32   sExponent             = 0;
    stlInt32   sTempExpo             = 0;
    stlBool    sHasDecimalPoint      = STL_FALSE;
    stlInt32   sNumCnt               = 0;
    stlChar  * sNum                  = sNumberString;
    stlChar  * sResult               = aResult;
    stlChar  * sDigitPos             = NULL;
    stlInt32   sNonExpLength         = 0;
    stlInt32   sExpLength            = 0;
    stlInt32   sMinExpLength         = 0;
    stlInt32   sZeroLen              = 0;
    stlInt32   sRoundDigit           = 0;
    stlInt32   sCarry                = 0;
    dtlExponentString * sExpoStrInfo = NULL;


    STL_PARAM_VALIDATE( aNumberString != NULL , aErrorStack );
    STL_PARAM_VALIDATE( aNumberStringLength > 0 , aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize > 0 , aErrorStack );
    STL_PARAM_VALIDATE( aResult != NULL , aErrorStack );
    STL_PARAM_VALIDATE( aResultLength != NULL , aErrorStack );

    
    sFirstString = aNumberString;
    sLastString = aNumberString + aNumberStringLength -1;

     /* truncate trailing spaces */
    SKIP_TRAILING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* check sign */
    CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositive );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading spaces */
    SKIP_LEADING_WHITE_SPACE( sFirstString, sLastString );
    STL_TRY_THROW( sFirstString <= sLastString, ERROR_INVALID_INPUT_DATA );

    /* truncate leading zeroes */
    SKIP_LEADING_ZERO_CHARACTERS( sFirstString, sLastString );

    if( sFirstString > sLastString )
    {
        *aResult = '0';
        *aResultLength = 1;

        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* do Nothing */
    }

    /*
     * number string parsing
     */
    
    while( ( sFirstString <= sLastString ) )
    {
        if( DTF_IS_DIGIT( *sFirstString ) )
        {
            STL_TRY_THROW( sNumCnt <= DTL_NUMERIC_STRING_MAX_SIZE -1 , ERROR_OUT_OF_RANGE );
                   
            *sNum = *sFirstString;
            sNum++;
            sNumCnt++;
        }
        else
        {
            switch( *sFirstString )
            {
                case '.':
                    {
                        STL_TRY_THROW( sHasDecimalPoint == STL_FALSE,
                                       ERROR_INVALID_INPUT_DATA );

                        if( sNumCnt == 0 )
                        {
                            sFirstString++;
                            SKIP_LEADING_ZERO_CHARACTERS_AFTER_DECIMAL( sFirstString, sLastString, sExponent );
                            sFirstString--;
                            sExponent = -sExponent;
                            sExponent--;
                        }
                        else
                        {
                            sExponent += sNumCnt -1;
                        }

                        sHasDecimalPoint = STL_TRUE;
                        break;
                    }
                case 'e':
                case 'E':
                    {
                        sFirstString++;
                        STL_TRY_THROW( ( sFirstString <= sLastString ) && ( sNumCnt > 0 ),
                                       ERROR_INVALID_INPUT_DATA );

                        if( sHasDecimalPoint == STL_FALSE )
                        {
                            sExponent += sNumCnt -1;
                        }
                        else
                        {
                            /* Do Nothing */
                        }

                        STL_THROW( RAMP_CHECK_EXPONENT );
                        break;
                    }
                default:
                    {
                        break;
                    }
            }
        }

        sFirstString++;
    }

    if( sHasDecimalPoint == STL_FALSE )
    {
        sExponent += sNumCnt -1;

        /* truncate trailing zero */
        sNum = sNumberString + sNumCnt -1;
        while( *sNum == '0' )
        {
            sNum--;
            sNumCnt--;
        }
    }
    else
    {
        /* Do Nothing */
    }

    

    STL_THROW( RAMP_MAKE_NUMBER_STRING );


    
    STL_RAMP( RAMP_CHECK_EXPONENT );

    CHECK_AND_ASSIGN_SIGNED_CHARACTER( sFirstString, sLastString, sIsPositiveExponent );
    STL_TRY_THROW( ( sFirstString <= sLastString ),
                   ERROR_INVALID_INPUT_DATA );

    SKIP_LEADING_ZERO_CHARACTERS( sFirstString, sLastString );

    sTempExpo = 0;
    while( sFirstString <= sLastString )
    {
        STL_TRY_THROW( DTF_IS_DIGIT( *sFirstString ),
                       ERROR_INVALID_INPUT_DATA );

        sTempExpo *= 10;
        sTempExpo += dtdHexLookup[ (stlUInt8) *sFirstString ];
        STL_TRY_THROW( sTempExpo <= STL_UINT16_MAX,
                       ERROR_OUT_OF_RANGE );
        sFirstString++;
    }

    if( sIsPositiveExponent == STL_TRUE )
    {
        sExponent += sTempExpo;
    }
    else
    {
        sExponent -= sTempExpo;
    }


    
    STL_RAMP( RAMP_MAKE_NUMBER_STRING );

    sNonExpLength = !sIsPositive;
    sExpLength = !sIsPositive;
    sExpoStrInfo = (dtlExponentString *) & dtdNumberExponentString[ sExponent ];
    
    if( sExponent < 0 )
    {
        sNonExpLength += sNumCnt - sExponent -1 + 1 /* decimal point */;
        sZeroLen = sNonExpLength - sNumCnt - 1 - (!sIsPositive );
    }
    else if( sNumCnt > sExponent + 1 )
    {
        sNonExpLength += sNumCnt + 1 /* decimal point */;
    }
    else
    {
        sZeroLen = sExponent - sNumCnt + 1;
        sNonExpLength += sZeroLen + sNumCnt;
    }
    sExpLength += sNumCnt + sExpoStrInfo->mStringCnt;
    sExpLength += sNumCnt > 1 ? 1 : 0 /* decimal point */;
    

    if( !sIsPositive )
    {
        *sResult = '-';
        sResult++;
    }
    sNum = sNumberString;

    if( aAvailableSize >= sNonExpLength )
    {
        /*
        * Exact Expression
        */
        
        *aResultLength = sNonExpLength;
        if( sExponent < 0 )
        {
            /**
             * < Example >
             * Digits : 1234   ( length 4 )
             * Expo   : -4
             * String : .0001234
             */
            *sResult = '.';
            sResult++;
            

            while( sZeroLen )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }

            while( sNumCnt )
            {
                *sResult = *sNum;
                sResult++;
                sNum++;
                sNumCnt--;
            }
        }
        else if( sNumCnt > sExponent + 1 )
        {
            /**
             * < Example >
             * Digits : 1234 ( length = 4 )
             * Expo   : 2
             * String : 123.4
             */

            *sResult = *sNum;
            sResult++;
            sNum++;
            sNumCnt--;
            
            while( sExponent > 0 )
            {
                *sResult = *sNum;
                sResult++;
                sNum++;
                sNumCnt--;
                sExponent--;
            }

            *sResult = '.';
            sResult++;

            while( sNumCnt > 0 )
            {
                *sResult = *sNum;
                sResult++;
                sNum++;
                sNumCnt--;
            }
        }
        else
        {
            /**
             * < Example >
             * Digits : 1234   ( length = 4 )
             * Expo   : 6
             * String : 1234000
             */
            while( sNumCnt )
            {
                *sResult = *sNum;
                sResult++;
                sNum++;
                sNumCnt--;
            }

            while( sZeroLen )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }

        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /*
         * Exponent Expression
         */
        sTempExpo = sNum[0] > '4' ? 1 : 0;
        sMinExpLength = 1 + sExpoStrInfo->mStringCnt + (!sIsPositive ) + sTempExpo;
        
        sRoundDigit = aAvailableSize - sExpoStrInfo->mStringCnt - (!sIsPositive );
        sRoundDigit -= ( sRoundDigit > 1 );
        

        STL_TRY( (( aAvailableSize > 0 ) && ( aAvailableSize >= sMinExpLength )) == STL_TRUE );

        sDigitPos = sResult;
        sDigitPos++;

        while( sRoundDigit > 0 && sNumCnt > 0 )
        {
            *sDigitPos = *sNum;
            sDigitPos++;
            sNum++;
            sRoundDigit--;
            sNumCnt--;
        }

        if( sNumCnt > 0 )
        {
            sCarry = sNum[0] > '4';
        }
        else
        {
            sCarry = 0;
        }

        sDigitPos--;

        if( sCarry == 0 )
        {
            sRoundDigit = sDigitPos - sResult;
        }
        else
        {
            sRoundDigit = sDigitPos - sResult + 1;

            while( ( sCarry == 1 ) && ( sRoundDigit > 1 ) )
            {
                *sDigitPos += sCarry;

                if( *sDigitPos > '9' )
                {
                    *sDigitPos = '0';
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }

                sRoundDigit--;
                sDigitPos--;
            }
        }

        while( ( sRoundDigit > 0 ) && ( sResult[ sRoundDigit ] == '0' ) )
        {
            sRoundDigit--;
        }

        if( sRoundDigit == 0 )
        {
            sResult[0] = '1';
            sResult++;
            sExponent++;
        }
        else
        {
            sResult[0] = sResult[1];
            sResult++;

            if( sRoundDigit == 1 )
            {
                /* Do Nothing */
            }
            else
            {
                *sResult = '.';
                sResult += sRoundDigit;
            }
        }

        sExpoStrInfo = (dtlExponentString *) & dtdNumberExponentString[ sExponent ];
        stlMemcpy( sResult , sExpoStrInfo->mString , sExpoStrInfo->mStringCnt );
        sResult += sExpoStrInfo->mStringCnt;

        *aResultLength = sResult - (stlChar *)aResult;
    }

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_INPUT_DATA )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_NUMERIC_LITERAL,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_STRING_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Numeric value를 Float32로 변환
 * @param[in]  aNumeric             Numeric value
 * @param[out] aResult              변환된 Float32 Value
 * @param[out] aErrorStack          에러 스택
 */
stlStatus dtdToFloat32FromNumeric( dtlDataValue    * aNumeric,
                                   stlFloat32      * aResult,
                                   stlErrorStack   * aErrorStack )
{
    stlInt32          sDigitLength;
    stlInt32          sExponent;
    stlUInt8        * sDigit;
    stlFloat64        sFloat64;
    dtlNumericType  * sNumeric;
    
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aNumeric, aErrorStack );

    sNumeric = DTF_NUMERIC( aNumeric );        

    if( DTL_NUMERIC_IS_ZERO( sNumeric, aNumeric->mLength ) )
    {
        *aResult = 0.0;
    }
    else
    {
        if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
        {
            sExponent = ( DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric ) << 1);

            sDigitLength = DTL_NUMERIC_GET_DIGIT_COUNT( aNumeric->mLength );
            sDigitLength = ( sDigitLength > 4 ? 4 : sDigitLength );
            sDigit       = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

            sFloat64 = 0.0;
            while( sDigitLength > 0 )
            {
                sFloat64 += ( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit ) *
                              gPredefined10PowValue[ sExponent ] );
                sDigit++;
                sDigitLength--;
                sExponent -= 2;
            }
        }
        else
        {
            sExponent = ( DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sNumeric ) << 1);

            sDigitLength = DTL_NUMERIC_GET_DIGIT_COUNT( aNumeric->mLength );
            sDigitLength = ( sDigitLength > 4 ? 4 : sDigitLength );
            sDigit       = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

            sFloat64 = 0.0;
            while( sDigitLength > 0 )
            {
                sFloat64 -= ( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit ) *
                              gPredefined10PowValue[ sExponent ] );
                sDigit++;
                sDigitLength--;
                sExponent -= 2;
            }
        }

        STL_TRY_THROW( stlIsfinite( (stlFloat32)sFloat64 ) != STL_IS_FINITE_FALSE,
                       ERROR_OUT_OF_RANGE );
        
        *aResult = (stlFloat32)sFloat64;
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_OUTSIDE_RANGE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Numeric value를 Float64로 변환
 * @param[in]  aNumeric             Numeric value
 * @param[out] aResult              변환된 Float64 Value
 * @param[out] aErrorStack          에러 스택
 */
stlStatus dtdToFloat64FromNumeric( dtlDataValue    * aNumeric,
                                   stlFloat64      * aResult,
                                   stlErrorStack   * aErrorStack )
{
    stlInt32          sDigitLength;
    stlInt32          sExponent;
    stlUInt8        * sDigit;
    stlFloat64        sFloat64;
    dtlNumericType  * sNumeric;
    

    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aNumeric, aErrorStack );

    sNumeric = DTF_NUMERIC( aNumeric );        

    if( DTL_NUMERIC_IS_ZERO( sNumeric, aNumeric->mLength ) )
    {
        *aResult = 0.0;
    }
    else
    {
        if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
        {
            sExponent = ( DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric ) << 1);
            
            sDigitLength = DTL_NUMERIC_GET_DIGIT_COUNT( aNumeric->mLength );
            sDigitLength = ( sDigitLength > 9 ? 9 : sDigitLength );
            sDigit       = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

            sFloat64 = 0.0;
            while( sDigitLength > 0 )
            {
                sFloat64 += ( DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigit ) *
                              gPredefined10PowValue[ sExponent ] );
                sDigit++;
                sDigitLength--;
                sExponent -= 2;
            }
        }
        else
        {
            sExponent = ( DTL_NUMERIC_GET_EXPONENT_FROM_NEGATIVE_VALUE( sNumeric ) << 1);
            
            sDigitLength = DTL_NUMERIC_GET_DIGIT_COUNT( aNumeric->mLength );
            sDigitLength = ( sDigitLength > 9 ? 9 : sDigitLength );
            sDigit       = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

            sFloat64 = 0.0;
            while( sDigitLength > 0 )
            {
                sFloat64 -= ( DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigit ) *
                              gPredefined10PowValue[ sExponent ] );
                sDigit++;
                sDigitLength--;
                sExponent -= 2;
            }
        }

        *aResult = sFloat64;
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief dtlRowIdType의 value를 bas64 encoding string으로 변환하는 함수.
 * @param[in]  aRowIdType      rowid data type
 * @param[in]  aAvailableSize  aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer         문자로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength         변환된 문자열의 길이
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdToStringFromDtlRowId( dtlRowIdType   * aRowIdType,
                                   stlInt64         aAvailableSize,
                                   stlChar        * aBuffer,
                                   stlInt64       * aLength,
                                   stlErrorStack  * aErrorStack )
{
    stlChar  * sBufferPtr;
    
    DTL_PARAM_VALIDATE( aRowIdType != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_ROWID_TO_STRING_LENGTH, aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aLength != NULL, aErrorStack );

    sBufferPtr = aBuffer;

    /*
     * OBJECT ID
     */

    DTD_ROWID_OBJECT_ID_TO_BASE64( aRowIdType, sBufferPtr );
    sBufferPtr += DTD_ROWID_OBJECT_ID_TO_BASE64_LENGTH;
   
    /*
     * TABLESPACE ID
     */
    
    DTD_ROWID_TABLESPACE_ID_TO_BASE64( aRowIdType, sBufferPtr );
    sBufferPtr += DTD_ROWID_TABLESPACE_ID_TO_BASE64_LENGTH;

    /*
     * PAGE ID
     */

    DTD_ROWID_PAGE_ID_TO_BASE64( aRowIdType, sBufferPtr );
    sBufferPtr += DTD_ROWID_PAGE_ID_TO_BASE64_LENGTH;
    
    /*
     * ROW NUMBER (PAGE OFFSET)
     */

    DTD_ROWID_ROW_NUMBER_TO_BASE64( aRowIdType, sBufferPtr );
    
    /* null terminator 1byte 뺀 실제 길이 */
    *aLength = DTL_ROWID_TO_STRING_LENGTH;  

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief dtlRowIdType의 value를 bas64 encoding utf16 string으로 변환하는 함수.
 * @param[in]  aRowIdType      rowid data type
 * @param[in]  aAvailableSize  aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer         문자로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength         변환된 문자열의 길이
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdToUtf16WCharStringFromDtlRowId( dtlRowIdType   * aRowIdType,
                                             stlInt64         aAvailableSize,
                                             stlUInt16      * aBuffer,
                                             stlInt64       * aLength,
                                             stlErrorStack  * aErrorStack )
{
    stlUInt16  * sBufferPtr;
    
    DTL_PARAM_VALIDATE( aRowIdType != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_ROWID_TO_UTF16_STRING_LENGTH, aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aLength != NULL, aErrorStack );

    sBufferPtr = aBuffer;

    /*
     * OBJECT ID
     */

    DTD_ROWID_OBJECT_ID_TO_BASE64( aRowIdType, sBufferPtr );
    sBufferPtr += DTD_ROWID_OBJECT_ID_TO_BASE64_LENGTH;
   
    /*
     * TABLESPACE ID
     */
    
    DTD_ROWID_TABLESPACE_ID_TO_BASE64( aRowIdType, sBufferPtr );
    sBufferPtr += DTD_ROWID_TABLESPACE_ID_TO_BASE64_LENGTH;

    /*
     * PAGE ID
     */

    DTD_ROWID_PAGE_ID_TO_BASE64( aRowIdType, sBufferPtr );
    sBufferPtr += DTD_ROWID_PAGE_ID_TO_BASE64_LENGTH;
    
    /*
     * ROW NUMBER (PAGE OFFSET)
     */

    DTD_ROWID_ROW_NUMBER_TO_BASE64( aRowIdType, sBufferPtr );
    
    /* null terminator 1byte 뺀 실제 길이 */
    *aLength = DTL_ROWID_TO_UTF16_STRING_LENGTH;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief dtlRowIdType의 value를 bas64 encoding utf32 string으로 변환하는 함수.
 * @param[in]  aRowIdType      rowid data type
 * @param[in]  aAvailableSize  aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer         문자로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength         변환된 문자열의 길이
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdToUtf32WCharStringFromDtlRowId( dtlRowIdType   * aRowIdType,
                                             stlInt64         aAvailableSize,
                                             stlUInt32      * aBuffer,
                                             stlInt64       * aLength,
                                             stlErrorStack  * aErrorStack )
{
    stlUInt32  * sBufferPtr;
    
    DTL_PARAM_VALIDATE( aRowIdType != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_ROWID_TO_UTF32_STRING_LENGTH, aErrorStack );
    DTL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( aLength != NULL, aErrorStack );

    sBufferPtr = aBuffer;

    /*
     * OBJECT ID
     */

    DTD_ROWID_OBJECT_ID_TO_BASE64( aRowIdType, sBufferPtr );
    sBufferPtr += DTD_ROWID_OBJECT_ID_TO_BASE64_LENGTH;
   
    /*
     * TABLESPACE ID
     */
    
    DTD_ROWID_TABLESPACE_ID_TO_BASE64( aRowIdType, sBufferPtr );
    sBufferPtr += DTD_ROWID_TABLESPACE_ID_TO_BASE64_LENGTH;

    /*
     * PAGE ID
     */

    DTD_ROWID_PAGE_ID_TO_BASE64( aRowIdType, sBufferPtr );
    sBufferPtr += DTD_ROWID_PAGE_ID_TO_BASE64_LENGTH;
    
    /*
     * ROW NUMBER (PAGE OFFSET)
     */

    DTD_ROWID_ROW_NUMBER_TO_BASE64( aRowIdType, sBufferPtr );
    
    /* null terminator 1byte 뺀 실제 길이 */
    *aLength = DTL_ROWID_TO_UTF32_STRING_LENGTH;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Char string value를 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다.
 *     <BR> 마지막 문자가 짤리는 경우, 이전 문자까지 변환한다.
 * @param[in]  aValue          dtlDataValue
 * @param[in]  aAvailableSize  aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer         문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength         변환된 문자열의 길이
 * @param[in]  aVectorFunc     Function Vector
 * @param[in]  aVectorArg      Vector Argument
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdCharStringToStringForAvailableSize( dtlDataValue    * aValue,
                                                 stlInt64          aAvailableSize,
                                                 void            * aBuffer,
                                                 stlInt64        * aLength,
                                                 dtlFuncVector   * aVectorFunc,
                                                 void            * aVectorArg,
                                                 stlErrorStack   * aErrorStack )
{
    stlInt64          sLength = 0;
    stlInt8           i;
    stlInt8           sMaxCharLen       = 0;
    stlInt8           sTempLen          = 0;
    stlInt8           sTempLen2         = 0;    
    stlChar         * sTempStr          = NULL;    
    stlStatus         sVerify           = STL_FAILURE;
    dtlCharacterSet   sCharSetID        = DTL_CHARACTERSET_MAX;

    DTF_CHECK_DTL_GROUP2( aValue,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          aErrorStack );
    STL_PARAM_VALIDATE( aBuffer != NULL, aErrorStack );
    
    
    if( aAvailableSize >= aValue->mLength )
    {
        sLength = aValue->mLength;
    }
    else
    {
        sTempStr = ((stlChar*)aValue->mValue + aAvailableSize);
        
        sCharSetID = aVectorFunc->mGetCharSetIDFunc(aVectorArg);
        sMaxCharLen = dtsGetMbMaxLengthFuncList[sCharSetID]();
        sMaxCharLen = ( aAvailableSize <= sMaxCharLen ) ? aAvailableSize : sMaxCharLen;
        
        for( i = sMaxCharLen; i > 0; i-- )
        {
            sVerify = dtsMbVerifyFuncList[sCharSetID]( sTempStr - i,
                                                       i,
                                                       &sTempLen2,
                                                       aErrorStack );
            if( sVerify == STL_SUCCESS )
            {
                sTempLen += sTempLen2;
                /*
                 * char를 찾았음으로 1Char만큼 건너뛰도록.
                 */
                
                i = i - sTempLen2 + 1;
            }
            else
            {
                /*
                 * aErrorStack에 쌓이지 않도록 함.
                 */
                stlPopError( aErrorStack );
            }
        }

        if( sVerify == STL_SUCCESS )
        {
            STL_DASSERT( i == 0 );
            sLength = aAvailableSize;
        }
        else
        {
            /* 마지막 문자가 짤리는 경우, 그 만큼의 길이를 뺀다. */
            sLength = aAvailableSize - ( sMaxCharLen - sTempLen );
        }
    }

    STL_DASSERT( aAvailableSize >= sLength );

    stlMemcpy( aBuffer, aValue->mValue, sLength );
    *aLength = sLength;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Date/Time string value를 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다.
 * @param[in]  aValue          dtlDataValue
 * @param[in]  aPrecision      precision
 * @param[in]  aScale          scale
 * @param[in]  aAvailableSize  aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer         문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength         변환된 문자열의 길이
 * @param[in]  aVectorFunc     Function Vector
 * @param[in]  aVectorArg      Vector Argument
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtdDateTimeToStringForAvailableSize( dtlDataValue    * aValue,
                                               stlInt64          aPrecision,
                                               stlInt64          aScale,
                                               stlInt64          aAvailableSize,
                                               void            * aBuffer,
                                               stlInt64        * aLength,
                                               dtlFuncVector   * aVectorFunc,
                                               void            * aVectorArg,
                                               stlErrorStack   * aErrorStack )
{
    stlInt64   sLength = 0;
    stlChar    sDateTimeBuffer[DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE];

    STL_DASSERT( (aValue->mType == DTL_TYPE_DATE ) ||
                 (aValue->mType == DTL_TYPE_TIME ) ||
                 (aValue->mType == DTL_TYPE_TIMESTAMP ) ||
                 (aValue->mType == DTL_TYPE_TIME_WITH_TIME_ZONE ) ||
                 (aValue->mType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ) );

    STL_TRY( dtdToStringFuncList[ aValue->mType ]( aValue,
                                                   aPrecision,
                                                   aScale,
                                                   DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                                   sDateTimeBuffer,
                                                   &sLength,
                                                   aVectorFunc,
                                                   aVectorArg,
                                                   aErrorStack )
             == STL_SUCCESS );
    
    sLength = ( sLength >= aAvailableSize ) ? aAvailableSize : sLength;
    stlMemcpy( aBuffer, sDateTimeBuffer, sLength );        
    
    *aLength = sLength;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief 목적 character set의 string length unit으로 계산된 precison 을 얻는다.
 * @param[in]  aDataType                   data type
 * @param[in]  aString                     value에 저장될 문자열
 * @param[in]  aStringLength               aString의 길이 
 * @param[in]  aStringUnit                 string length unit (dtlStringLengthUnit 참조)
 * @param[out] aPrecForDestStrUnitLength   aString으로 계산된 precision
 * @param[in]  aVectorFunc                 Function Vector
 * @param[in]  aVectorArg                  Vector Argument 
 * @param[in]  aErrorStack                 에러 스택
 * 
 */
stlStatus dtdGetPrecisionForDestStringUnitLength( dtlDataType            aDataType,
                                                  stlChar              * aString,
                                                  stlInt64               aStringLength,
                                                  dtlStringLengthUnit    aStringUnit,
                                                  stlInt64             * aPrecForDestStrUnitLength,
                                                  dtlFuncVector        * aVectorFunc,
                                                  void                 * aVectorArg,
                                                  stlErrorStack        * aErrorStack )
{
    stlInt64  sPrecisionForDestStringUnitLength = 0;    
    
    STL_PARAM_VALIDATE( aString != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aPrecForDestStrUnitLength != NULL, aErrorStack );
    DTL_PARAM_VALIDATE( (aStringUnit > DTL_STRING_LENGTH_UNIT_NA ) &&
                        (aStringUnit < DTL_STRING_LENGTH_UNIT_MAX),
                        aErrorStack );
    
    if( aStringUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS )
    {
        /*
         * 예) char(10 characters) 또는  varchar(10 characters) 로 선언된 경우 
         */ 
        
        /**
         * (1) aString의 character length를 얻는다.<BR>
         *     (character length를 문자 갯수 또는 value의 precision이라고도 표현할 수 있다.)<BR>
         */
        STL_TRY( dtsGetMbstrlenWithLenFuncList[ aVectorFunc->mGetCharSetIDFunc(aVectorArg) ](
                     aString,
                     aStringLength,
                     & sPrecisionForDestStringUnitLength,
                     aVectorFunc,
                     aVectorArg,
                     aErrorStack )
                 == STL_SUCCESS );
    }
    else if( aStringUnit == DTL_STRING_LENGTH_UNIT_OCTETS )
    {
        /*
         * 예) char(10 octets) 또는 varchar(10 octets) 로 선언된 경우 
         */
        sPrecisionForDestStringUnitLength = aStringLength;                 
    }
    else
    {
        STL_DASSERT( STL_FALSE );
        STL_THROW( RAMP_ERR_CHARACTER_LENGTH_UNIT_INVALID );
    }

    *aPrecForDestStrUnitLength = sPrecisionForDestStringUnitLength;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CHARACTER_LENGTH_UNIT_INVALID )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_CHARACTER_LENGTH_UNIT_INVALID,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[ aDataType ],
                      aStringUnit,
                      gDataTypeInfoDomain[ aDataType ].mMinStringLengthUnit,
                      gDataTypeInfoDomain[ aDataType ].mMaxStringLengthUnit );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */


