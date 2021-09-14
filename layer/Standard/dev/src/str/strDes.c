/*******************************************************************************
 * strDes.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    DES Implementation.
 *    polarssl에서 소스를 가져왔으며, 아래는 Copyright 문서를 그대로 옮겨놓음.
 *    여기서 사용되는 Macro는 이 소스 내에서만 이용되므로 이름을 변경하지 않고 그대로 사용함
 *
 *
 *  FIPS-46-3 compliant Triple-DES implementation
 *
 *  Copyright (C) 2006-2010, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 ******************************************************************************/


/**
 * @file strDes.c
 * @brief Standard Layer DES Routines
 */

#include <str.h>
#include <strDes.h>
#include <sts.h>

/**
 * @addtogroup strDes DES implementation
 * @{
 */

#include <strDes.h>

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef STR_GET_UINT32_BE
#define STR_GET_UINT32_BE(n,b,i)                    \
    {                                               \
        (n) = ( (stlUInt32) (b)[(i)    ] << 24 )    \
            | ( (stlUInt32) (b)[(i) + 1] << 16 )    \
            | ( (stlUInt32) (b)[(i) + 2] <<  8 )    \
            | ( (stlUInt32) (b)[(i) + 3]       );   \
    }
#endif

#ifndef STR_PUT_UINT32_BE
#define STR_PUT_UINT32_BE(n,b,i)                    \
    {                                               \
        (b)[(i)    ] = (stlUInt8) ( (n) >> 24 );    \
        (b)[(i) + 1] = (stlUInt8) ( (n) >> 16 );    \
        (b)[(i) + 2] = (stlUInt8) ( (n) >>  8 );    \
        (b)[(i) + 3] = (stlUInt8) ( (n)       );    \
    }
#endif

/*
 * Expanded DES S-boxes
 */
static const stlUInt32 SB1[64] =
{
    0x01010400, 0x00000000, 0x00010000, 0x01010404,
    0x01010004, 0x00010404, 0x00000004, 0x00010000,
    0x00000400, 0x01010400, 0x01010404, 0x00000400,
    0x01000404, 0x01010004, 0x01000000, 0x00000004,
    0x00000404, 0x01000400, 0x01000400, 0x00010400,
    0x00010400, 0x01010000, 0x01010000, 0x01000404,
    0x00010004, 0x01000004, 0x01000004, 0x00010004,
    0x00000000, 0x00000404, 0x00010404, 0x01000000,
    0x00010000, 0x01010404, 0x00000004, 0x01010000,
    0x01010400, 0x01000000, 0x01000000, 0x00000400,
    0x01010004, 0x00010000, 0x00010400, 0x01000004,
    0x00000400, 0x00000004, 0x01000404, 0x00010404,
    0x01010404, 0x00010004, 0x01010000, 0x01000404,
    0x01000004, 0x00000404, 0x00010404, 0x01010400,
    0x00000404, 0x01000400, 0x01000400, 0x00000000,
    0x00010004, 0x00010400, 0x00000000, 0x01010004
};

static const stlUInt32 SB2[64] =
{
    0x80108020, 0x80008000, 0x00008000, 0x00108020,
    0x00100000, 0x00000020, 0x80100020, 0x80008020,
    0x80000020, 0x80108020, 0x80108000, 0x80000000,
    0x80008000, 0x00100000, 0x00000020, 0x80100020,
    0x00108000, 0x00100020, 0x80008020, 0x00000000,
    0x80000000, 0x00008000, 0x00108020, 0x80100000,
    0x00100020, 0x80000020, 0x00000000, 0x00108000,
    0x00008020, 0x80108000, 0x80100000, 0x00008020,
    0x00000000, 0x00108020, 0x80100020, 0x00100000,
    0x80008020, 0x80100000, 0x80108000, 0x00008000,
    0x80100000, 0x80008000, 0x00000020, 0x80108020,
    0x00108020, 0x00000020, 0x00008000, 0x80000000,
    0x00008020, 0x80108000, 0x00100000, 0x80000020,
    0x00100020, 0x80008020, 0x80000020, 0x00100020,
    0x00108000, 0x00000000, 0x80008000, 0x00008020,
    0x80000000, 0x80100020, 0x80108020, 0x00108000
};

static const stlUInt32 SB3[64] =
{
    0x00000208, 0x08020200, 0x00000000, 0x08020008,
    0x08000200, 0x00000000, 0x00020208, 0x08000200,
    0x00020008, 0x08000008, 0x08000008, 0x00020000,
    0x08020208, 0x00020008, 0x08020000, 0x00000208,
    0x08000000, 0x00000008, 0x08020200, 0x00000200,
    0x00020200, 0x08020000, 0x08020008, 0x00020208,
    0x08000208, 0x00020200, 0x00020000, 0x08000208,
    0x00000008, 0x08020208, 0x00000200, 0x08000000,
    0x08020200, 0x08000000, 0x00020008, 0x00000208,
    0x00020000, 0x08020200, 0x08000200, 0x00000000,
    0x00000200, 0x00020008, 0x08020208, 0x08000200,
    0x08000008, 0x00000200, 0x00000000, 0x08020008,
    0x08000208, 0x00020000, 0x08000000, 0x08020208,
    0x00000008, 0x00020208, 0x00020200, 0x08000008,
    0x08020000, 0x08000208, 0x00000208, 0x08020000,
    0x00020208, 0x00000008, 0x08020008, 0x00020200
};

static const stlUInt32 SB4[64] =
{
    0x00802001, 0x00002081, 0x00002081, 0x00000080,
    0x00802080, 0x00800081, 0x00800001, 0x00002001,
    0x00000000, 0x00802000, 0x00802000, 0x00802081,
    0x00000081, 0x00000000, 0x00800080, 0x00800001,
    0x00000001, 0x00002000, 0x00800000, 0x00802001,
    0x00000080, 0x00800000, 0x00002001, 0x00002080,
    0x00800081, 0x00000001, 0x00002080, 0x00800080,
    0x00002000, 0x00802080, 0x00802081, 0x00000081,
    0x00800080, 0x00800001, 0x00802000, 0x00802081,
    0x00000081, 0x00000000, 0x00000000, 0x00802000,
    0x00002080, 0x00800080, 0x00800081, 0x00000001,
    0x00802001, 0x00002081, 0x00002081, 0x00000080,
    0x00802081, 0x00000081, 0x00000001, 0x00002000,
    0x00800001, 0x00002001, 0x00802080, 0x00800081,
    0x00002001, 0x00002080, 0x00800000, 0x00802001,
    0x00000080, 0x00800000, 0x00002000, 0x00802080
};

static const stlUInt32 SB5[64] =
{
    0x00000100, 0x02080100, 0x02080000, 0x42000100,
    0x00080000, 0x00000100, 0x40000000, 0x02080000,
    0x40080100, 0x00080000, 0x02000100, 0x40080100,
    0x42000100, 0x42080000, 0x00080100, 0x40000000,
    0x02000000, 0x40080000, 0x40080000, 0x00000000,
    0x40000100, 0x42080100, 0x42080100, 0x02000100,
    0x42080000, 0x40000100, 0x00000000, 0x42000000,
    0x02080100, 0x02000000, 0x42000000, 0x00080100,
    0x00080000, 0x42000100, 0x00000100, 0x02000000,
    0x40000000, 0x02080000, 0x42000100, 0x40080100,
    0x02000100, 0x40000000, 0x42080000, 0x02080100,
    0x40080100, 0x00000100, 0x02000000, 0x42080000,
    0x42080100, 0x00080100, 0x42000000, 0x42080100,
    0x02080000, 0x00000000, 0x40080000, 0x42000000,
    0x00080100, 0x02000100, 0x40000100, 0x00080000,
    0x00000000, 0x40080000, 0x02080100, 0x40000100
};

static const stlUInt32 SB6[64] =
{
    0x20000010, 0x20400000, 0x00004000, 0x20404010,
    0x20400000, 0x00000010, 0x20404010, 0x00400000,
    0x20004000, 0x00404010, 0x00400000, 0x20000010,
    0x00400010, 0x20004000, 0x20000000, 0x00004010,
    0x00000000, 0x00400010, 0x20004010, 0x00004000,
    0x00404000, 0x20004010, 0x00000010, 0x20400010,
    0x20400010, 0x00000000, 0x00404010, 0x20404000,
    0x00004010, 0x00404000, 0x20404000, 0x20000000,
    0x20004000, 0x00000010, 0x20400010, 0x00404000,
    0x20404010, 0x00400000, 0x00004010, 0x20000010,
    0x00400000, 0x20004000, 0x20000000, 0x00004010,
    0x20000010, 0x20404010, 0x00404000, 0x20400000,
    0x00404010, 0x20404000, 0x00000000, 0x20400010,
    0x00000010, 0x00004000, 0x20400000, 0x00404010,
    0x00004000, 0x00400010, 0x20004010, 0x00000000,
    0x20404000, 0x20000000, 0x00400010, 0x20004010
};

static const stlUInt32 SB7[64] =
{
    0x00200000, 0x04200002, 0x04000802, 0x00000000,
    0x00000800, 0x04000802, 0x00200802, 0x04200800,
    0x04200802, 0x00200000, 0x00000000, 0x04000002,
    0x00000002, 0x04000000, 0x04200002, 0x00000802,
    0x04000800, 0x00200802, 0x00200002, 0x04000800,
    0x04000002, 0x04200000, 0x04200800, 0x00200002,
    0x04200000, 0x00000800, 0x00000802, 0x04200802,
    0x00200800, 0x00000002, 0x04000000, 0x00200800,
    0x04000000, 0x00200800, 0x00200000, 0x04000802,
    0x04000802, 0x04200002, 0x04200002, 0x00000002,
    0x00200002, 0x04000000, 0x04000800, 0x00200000,
    0x04200800, 0x00000802, 0x00200802, 0x04200800,
    0x00000802, 0x04000002, 0x04200802, 0x04200000,
    0x00200800, 0x00000000, 0x00000002, 0x04200802,
    0x00000000, 0x00200802, 0x04200000, 0x00000800,
    0x04000002, 0x04000800, 0x00000800, 0x00200002
};

static const stlUInt32 SB8[64] =
{
    0x10001040, 0x00001000, 0x00040000, 0x10041040,
    0x10000000, 0x10001040, 0x00000040, 0x10000000,
    0x00040040, 0x10040000, 0x10041040, 0x00041000,
    0x10041000, 0x00041040, 0x00001000, 0x00000040,
    0x10040000, 0x10000040, 0x10001000, 0x00001040,
    0x00041000, 0x00040040, 0x10040040, 0x10041000,
    0x00001040, 0x00000000, 0x00000000, 0x10040040,
    0x10000040, 0x10001000, 0x00041040, 0x00040000,
    0x00041040, 0x00040000, 0x10041000, 0x00001000,
    0x00000040, 0x10040040, 0x00001000, 0x00041040,
    0x10001000, 0x00000040, 0x10000040, 0x10040000,
    0x10040040, 0x10000000, 0x00040000, 0x10001040,
    0x00000000, 0x10041040, 0x00040040, 0x10000040,
    0x10040000, 0x10001000, 0x10001040, 0x00000000,
    0x10041040, 0x00041000, 0x00041000, 0x00001040,
    0x00001040, 0x00040040, 0x10000000, 0x10041000
};

/*
 * PC1: left and right halves bit-swap
 */
static const stlUInt32 LHs[16] =
{
    0x00000000, 0x00000001, 0x00000100, 0x00000101,
    0x00010000, 0x00010001, 0x00010100, 0x00010101,
    0x01000000, 0x01000001, 0x01000100, 0x01000101,
    0x01010000, 0x01010001, 0x01010100, 0x01010101
};

static const stlUInt32 RHs[16] =
{
    0x00000000, 0x01000000, 0x00010000, 0x01010000,
    0x00000100, 0x01000100, 0x00010100, 0x01010100,
    0x00000001, 0x01000001, 0x00010001, 0x01010001,
    0x00000101, 0x01000101, 0x00010101, 0x01010101,
};

/*
 * Initial Permutation macro
 */
#define STR_DES_IP(X,Y)                                                 \
    {                                                               \
        T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
        T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
        T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
        T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
        Y = ((Y << 1) | (Y >> 31)) & 0xFFFFFFFF;                    \
        T = (X ^ Y) & 0xAAAAAAAA; Y ^= T; X ^= T;                   \
        X = ((X << 1) | (X >> 31)) & 0xFFFFFFFF;                    \
    }

/*
 * Final Permutation macro
 */
#define STR_DES_FP(X,Y)                                                 \
    {                                                               \
        X = ((X << 31) | (X >> 1)) & 0xFFFFFFFF;                    \
        T = (X ^ Y) & 0xAAAAAAAA; X ^= T; Y ^= T;                   \
        Y = ((Y << 31) | (Y >> 1)) & 0xFFFFFFFF;                    \
        T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
        T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
        T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
        T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
    }

/*
 * DES round macro
 */
#define STR_DES_ROUND(X,Y)                          \
    {                                           \
        T = *SK++ ^ X;                          \
        Y ^= SB8[ (T      ) & 0x3F ] ^          \
             SB6[ (T >>  8) & 0x3F ] ^          \
             SB4[ (T >> 16) & 0x3F ] ^          \
             SB2[ (T >> 24) & 0x3F ];           \
                                                \
        T = *SK++ ^ ((X << 28) | (X >> 4));     \
        Y ^= SB7[ (T      ) & 0x3F ] ^          \
             SB5[ (T >>  8) & 0x3F ] ^          \
             SB3[ (T >> 16) & 0x3F ] ^          \
             SB1[ (T >> 24) & 0x3F ];           \
    }

#define SWAP(a,b) { stlUInt32 t = a; a = b; b = t; t = 0; }

static void strDesSetKey( stlUInt32       SK[32],
                          const stlUInt8  aKey[STR_DES_KEY_SIZE] )
{
    int i;
    stlUInt32 X, Y, T;

    STR_GET_UINT32_BE( X, aKey, 0 );
    STR_GET_UINT32_BE( Y, aKey, 4 );

    /*
     * Permuted Choice 1
     */
    T =  ((Y >>  4) ^ X) & 0x0F0F0F0F;  X ^= T; Y ^= (T <<  4);
    T =  ((Y      ) ^ X) & 0x10101010;  X ^= T; Y ^= (T      );

    X =   (LHs[ (X      ) & 0xF] << 3) | (LHs[ (X >>  8) & 0xF ] << 2)
        | (LHs[ (X >> 16) & 0xF] << 1) | (LHs[ (X >> 24) & 0xF ]     )
        | (LHs[ (X >>  5) & 0xF] << 7) | (LHs[ (X >> 13) & 0xF ] << 6)
        | (LHs[ (X >> 21) & 0xF] << 5) | (LHs[ (X >> 29) & 0xF ] << 4);

    Y =   (RHs[ (Y >>  1) & 0xF] << 3) | (RHs[ (Y >>  9) & 0xF ] << 2)
        | (RHs[ (Y >> 17) & 0xF] << 1) | (RHs[ (Y >> 25) & 0xF ]     )
        | (RHs[ (Y >>  4) & 0xF] << 7) | (RHs[ (Y >> 12) & 0xF ] << 6)
        | (RHs[ (Y >> 20) & 0xF] << 5) | (RHs[ (Y >> 28) & 0xF ] << 4);

    X &= 0x0FFFFFFF;
    Y &= 0x0FFFFFFF;

    /*
     * calculate subkeys
     */
    for( i = 0; i < 16; i++ )
    {
        if( i < 2 || i == 8 || i == 15 )
        {
            X = ((X <<  1) | (X >> 27)) & 0x0FFFFFFF;
            Y = ((Y <<  1) | (Y >> 27)) & 0x0FFFFFFF;
        }
        else
        {
            X = ((X <<  2) | (X >> 26)) & 0x0FFFFFFF;
            Y = ((Y <<  2) | (Y >> 26)) & 0x0FFFFFFF;
        }

        *SK++ =   ((X <<  4) & 0x24000000) | ((X << 28) & 0x10000000)
            | ((X << 14) & 0x08000000) | ((X << 18) & 0x02080000)
            | ((X <<  6) & 0x01000000) | ((X <<  9) & 0x00200000)
            | ((X >>  1) & 0x00100000) | ((X << 10) & 0x00040000)
            | ((X <<  2) & 0x00020000) | ((X >> 10) & 0x00010000)
            | ((Y >> 13) & 0x00002000) | ((Y >>  4) & 0x00001000)
            | ((Y <<  6) & 0x00000800) | ((Y >>  1) & 0x00000400)
            | ((Y >> 14) & 0x00000200) | ((Y      ) & 0x00000100)
            | ((Y >>  5) & 0x00000020) | ((Y >> 10) & 0x00000010)
            | ((Y >>  3) & 0x00000008) | ((Y >> 18) & 0x00000004)
            | ((Y >> 26) & 0x00000002) | ((Y >> 24) & 0x00000001);

        *SK++ =   ((X << 15) & 0x20000000) | ((X << 17) & 0x10000000)
            | ((X << 10) & 0x08000000) | ((X << 22) & 0x04000000)
            | ((X >>  2) & 0x02000000) | ((X <<  1) & 0x01000000)
            | ((X << 16) & 0x00200000) | ((X << 11) & 0x00100000)
            | ((X <<  3) & 0x00080000) | ((X >>  6) & 0x00040000)
            | ((X << 15) & 0x00020000) | ((X >>  4) & 0x00010000)
            | ((Y >>  2) & 0x00002000) | ((Y <<  8) & 0x00001000)
            | ((Y >> 14) & 0x00000808) | ((Y >>  9) & 0x00000400)
            | ((Y      ) & 0x00000200) | ((Y <<  7) & 0x00000100)
            | ((Y >>  7) & 0x00000020) | ((Y >>  3) & 0x00000011)
            | ((Y <<  2) & 0x00000004) | ((Y >> 21) & 0x00000002);
    }
}

/**
 * @brief DES key schedule (56-bit, encryption)
 *
 * @param[in] aCtx     DES context to be initialized
 * @param[in] aKey     8-byte secret key
 *
 * @return STL_SUCCESS
 */
stlStatus strDesSetKeyEnc( strDesContext  *aCtx,
                           const stlUInt8  aKey[STR_DES_KEY_SIZE] )
{
    strDesSetKey( aCtx->mSubKeys, aKey );

    return STL_SUCCESS;
}

/**
 * @brief              DES key schedule (56-bit, decryption)
 *
 * @param[in] aCtx     DES context to be initialized
 * @param[in] aKey     8-byte secret key
 *
 * @return             STL_SUCCESS
 */
stlStatus strDesSetKeyDec( strDesContext  *aCtx,
                           const stlUInt8  aKey[STR_DES_KEY_SIZE] )
{
    int i;

    strDesSetKey( aCtx->mSubKeys, aKey );

    for( i = 0; i < 16; i += 2 )
    {
        SWAP( aCtx->mSubKeys[i    ], aCtx->mSubKeys[30 - i] );
        SWAP( aCtx->mSubKeys[i + 1], aCtx->mSubKeys[31 - i] );
    }

    return STL_SUCCESS;
}

static void strDes3Set2Key( stlUInt32      aEsk[96],
                            stlUInt32      aDsk[96],
                            const stlUInt8 aKey[STR_DES_KEY_SIZE*2] )
{
    int i;

    strDesSetKey( aEsk, aKey );
    strDesSetKey( aDsk + 32, aKey + STR_DES_KEY_SIZE );

    for( i = 0; i < 32; i += 2 )
    {
        aDsk[i     ] = aEsk[30 - i];
        aDsk[i +  1] = aEsk[31 - i];

        aEsk[i + 32] = aDsk[62 - i];
        aEsk[i + 33] = aDsk[63 - i];

        aEsk[i + 64] = aEsk[i    ];
        aEsk[i + 65] = aEsk[i + 1];

        aDsk[i + 64] = aDsk[i    ];
        aDsk[i + 65] = aDsk[i + 1];
    }
}

/**
 * @brief              Triple-DES key schedule (112-bit, encryption)
 *
 * @param[in] aCtx     3DES context to be initialized
 * @param[in] aKey     16-byte secret key
 *
 * @return             STL_SUCCESS
 */
stlStatus strDes3Set2KeyEnc( strDes3Context *aCtx,
                             const stlUInt8  aKey[STR_DES_KEY_SIZE * 2] )
{
    stlUInt32 sSubKeys[96];

    strDes3Set2Key( aCtx->mSubKeys, sSubKeys, aKey );
    stlMemset( sSubKeys, 0, sizeof( sSubKeys ) );

    return STL_SUCCESS;
}

/**
 * @brief              Triple-DES key schedule (112-bit, decryption)
 *
 * @param[in] aCtx     3DES context to be initialized
 * @param[in] aKey     16-byte secret key
 *
 * @return             STL_SUCCESS
 */
stlStatus strDes3Set2KeyDec( strDes3Context *aCtx,
                             const stlUInt8  aKey[STR_DES_KEY_SIZE * 2] )
{
    stlUInt32 sSubKeys[96];

    strDes3Set2Key( sSubKeys, aCtx->mSubKeys, aKey );
    stlMemset( sSubKeys, 0, sizeof( sSubKeys ) );

    return STL_SUCCESS;
}

static void strDes3Set3Key( stlUInt32      aEsk[96],
                            stlUInt32      aDsk[96],
                            const stlUInt8 aKey[STR_DES_KEY_SIZE * 3] )
{
    int i;

    strDesSetKey( aEsk, aKey );
    strDesSetKey( aDsk + 32, aKey + STR_DES_KEY_SIZE );
    strDesSetKey( aEsk + 64, aKey + STR_DES_KEY_SIZE * 2 );

    for( i = 0; i < 32; i += 2 )
    {
        aDsk[i     ] = aEsk[94 - i];
        aDsk[i +  1] = aEsk[95 - i];

        aEsk[i + 32] = aDsk[62 - i];
        aEsk[i + 33] = aDsk[63 - i];

        aDsk[i + 64] = aEsk[30 - i];
        aDsk[i + 65] = aEsk[31 - i];
    }
}

/**
 * @brief              Triple-DES key schedule (168-bit, encryption)
 *
 * @param[in] aCtx     3DES context to be initialized
 * @param[in] aKey     24-byte secret key
 *
 * @return             STL_SUCCESS
 */
stlStatus strDes3Set3KeyEnc( strDes3Context *aCtx,
                             const stlUInt8  aKey[STR_DES_KEY_SIZE * 3] )
{
    stlUInt32 sSubKeys[96];

    strDes3Set3Key( aCtx->mSubKeys, sSubKeys, aKey );
    stlMemset( sSubKeys, 0, sizeof( sSubKeys ) );

    return STL_SUCCESS;
}

/**
 * @brief              Triple-DES key schedule (168-bit, decryption)
 *
 * @param[in] aCtx     3DES context to be initialized
 * @param[in] aKey     24-byte secret key
 *
 * @return             STL_SUCCESS
 */
stlStatus strDes3Set3KeyDec( strDes3Context *aCtx,
                             const stlUInt8  aKey[STR_DES_KEY_SIZE * 3] )
{
    stlUInt32 sSubKeys[96];

    strDes3Set3Key( sSubKeys, aCtx->mSubKeys, aKey );
    stlMemset( sSubKeys, 0, sizeof( sSubKeys ) );

    return STL_SUCCESS;
}

/**
 * @brief               DES-ECB block encryption/decryption
 *
 * @param[in]  aCtx     DES context
 * @param[in]  aInput   64-bit input block
 * @param[out] aOutput  64-bit output block
 *
 * @return              STL_SUCCESS if successful
 */
stlStatus strDesCryptEcb( strDesContext  *aCtx,
                          const stlUInt8  aInput[STR_DES_BLK_SIZE],
                          stlUInt8        aOutput[STR_DES_BLK_SIZE] )
{
    int i;
    stlUInt32 X, Y, T, *SK;

    SK = aCtx->mSubKeys;

    STR_GET_UINT32_BE( X, aInput, 0 );
    STR_GET_UINT32_BE( Y, aInput, 4 );

    STR_DES_IP( X, Y );

    for( i = 0; i < 8; i++ )
    {
        STR_DES_ROUND( Y, X );
        STR_DES_ROUND( X, Y );
    }

    STR_DES_FP( Y, X );

    STR_PUT_UINT32_BE( Y, aOutput, 0 );
    STR_PUT_UINT32_BE( X, aOutput, 4 );

    return STL_SUCCESS;
}

/**
 * @brief               3DES-ECB block encryption/decryption
 * @param[in]  aCtx     3DES context
 * @param[in]  aInput   64-bit input block
 * @param[out] aOutput  64-bit output block
 * @return              STL_SUCCESS if successful
 */
stlStatus strDes3CryptEcb( strDes3Context *aCtx,
                           const stlUInt8  aInput[STR_DES_BLK_SIZE],
                           stlUInt8        aOutput[STR_DES_BLK_SIZE] )
{
    int i;
    stlUInt32 X, Y, T, *SK;

    SK = aCtx->mSubKeys;

    STR_GET_UINT32_BE( X, aInput, 0 );
    STR_GET_UINT32_BE( Y, aInput, 4 );

    STR_DES_IP( X, Y );

    for( i = 0; i < 8; i++ )
    {
        STR_DES_ROUND( Y, X );
        STR_DES_ROUND( X, Y );
    }

    for( i = 0; i < 8; i++ )
    {
        STR_DES_ROUND( X, Y );
        STR_DES_ROUND( Y, X );
    }

    for( i = 0; i < 8; i++ )
    {
        STR_DES_ROUND( Y, X );
        STR_DES_ROUND( X, Y );
    }

    STR_DES_FP( Y, X );

    STR_PUT_UINT32_BE( Y, aOutput, 0 );
    STR_PUT_UINT32_BE( X, aOutput, 4 );

    return STL_SUCCESS;
}

static stlChar gHexMap[16] = { '0', '1', '2', '3',
                               '4', '5', '6', '7',
                               '8', '9', 'A', 'B',
                               'C', 'D', 'E', 'F' };

static const stlUInt8 gStrDes3Keys[STR_DES_KEY_SIZE * 3] =
{
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
    0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23
};

stlInt32 strHex2Num( stlChar  aHexChar )
{
    stlInt32 sRet = -1;

    if( isxdigit( (stlInt32)aHexChar ) )
    {
        if( isdigit( (stlInt32)aHexChar ) )
        {
            sRet = aHexChar - '0';
        }
        else if( isalpha( (stlInt32)aHexChar ) )
        {
            sRet = toupper( aHexChar ) - 'A' + 10;
        }
    }

    return sRet;
}

stlStatus strEncryptKey( stlChar        *aPlainText,
                         stlInt32        aPlainTextLen,
                         stlChar        *aLicenseBuffer )
{
    stlInt32         i;
    strDes3Context   sCtx3;
    stlUInt8         sTempBlock[STR_DES_BLK_SIZE];
    stlInt32         sBlock;
    stlInt32         sLicenseBufferOffset = 0;

    strDes3Set3KeyEnc( &sCtx3, (stlUInt8 *) gStrDes3Keys );

    for( sBlock = 0; sBlock < aPlainTextLen; sBlock += 8 )
    {
        stlMemcpy( sTempBlock, aPlainText + sBlock, 8 );
        for( i = 0; i < 10000; i++ )
        {
            strDes3CryptEcb( &sCtx3, sTempBlock, sTempBlock );
        }

        for( i = 0; i < 8; i ++ )
        {
            aLicenseBuffer[sLicenseBufferOffset] = gHexMap[(sTempBlock[i] >> 4) & 0xF];
            aLicenseBuffer[sLicenseBufferOffset + 1] = gHexMap[sTempBlock[i] & 0xF];
            sLicenseBufferOffset += 2;
        }
    }

    return STL_SUCCESS;
}

stlStatus strDecryptKey( stlChar        *aLicenseKey,
                         stlInt32        aLicenseKeyLen,
                         stlChar        *aPlainText )
{
    stlInt32        i;
    strDes3Context  sCtx3;
    stlUInt8        sTempBlock[STR_DES_BLK_SIZE];
    stlInt32        sBlock;

    strDes3Set3KeyDec( &sCtx3, (stlUInt8 *) gStrDes3Keys );

    for( sBlock = 0; sBlock < aLicenseKeyLen; sBlock += 16 )
    {
        for( i = 0; i < 16; i += 2 )
        {
            sTempBlock[i/2] =
                strHex2Num( aLicenseKey[sBlock + i] ) << 4 |
                strHex2Num( aLicenseKey[sBlock + i + 1] );
        }

        for( i = 0; i < 10000; i++ )
        {
            strDes3CryptEcb( &sCtx3, sTempBlock, sTempBlock );
        }

        stlMemcpy( aPlainText + sBlock / 2, sTempBlock, 8 );
    }

    return STL_SUCCESS;
}

/** @} */
