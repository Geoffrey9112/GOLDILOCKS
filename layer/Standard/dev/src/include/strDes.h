/*******************************************************************************
 * strDes.h
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
 * @file des.h
 *
 * @brief DES block cipher
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


#ifndef _STRDES_H_
#define _STRDES_H_ 1

/**
 * @file strDes.h
 * @brief DES for Standard Layer
 */

#include <stl.h>

#define STR_DES_ENCRYPT     1
#define STR_DES_DECRYPT     0

#define STR_DES_KEY_SIZE    8
#define STR_DES_BLK_SIZE    8

/**
 * @brief          DES context structure
 */
typedef struct strDesContext
{
    stlInt32   mMode;            /*!<  encrypt/decrypt   */
    stlUInt32  mSubKeys[32];     /*!<  DES subkeys       */
} strDesContext;

/**
 * @brief          Triple-DES context structure
 */
typedef struct strDes3Context
{
    stlInt32   mMode;            /*!<  encrypt/decrypt   */
    stlUInt32  mSubKeys[96];     /*!<  3DES subkeys      */
} strDes3Context;

stlStatus strDesSetKeyEnc( strDesContext  *aCtx,
                           const stlUInt8  aKey[STR_DES_KEY_SIZE] );

stlStatus strDesSetKeyDec( strDesContext  *aCtx,
                           const stlUInt8  aKey[STR_DES_KEY_SIZE] );

stlStatus strDes3Set2KeyEnc( strDes3Context *aCtx,
                             const stlUInt8  aKey[STR_DES_KEY_SIZE * 2] );

stlStatus strDes3Set2KeyDec( strDes3Context *aCtx,
                             const stlUInt8  aKey[STR_DES_KEY_SIZE * 2] );

stlStatus strDes3Set3KeyEnc( strDes3Context *aCtx,
                             const stlUInt8  aKey[STR_DES_KEY_SIZE * 3] );

stlStatus strDes3Set3KeyDec( strDes3Context *aCtx,
                             const stlUInt8  aKey[STR_DES_KEY_SIZE * 3] );

stlStatus strDesCryptEcb( strDesContext  *aCtx,
                          const stlUInt8  aInput[STR_DES_BLK_SIZE],
                          stlUInt8        aOutput[STR_DES_BLK_SIZE] );

stlStatus strDes3CryptEcb( strDes3Context *aCtx,
                           const stlUInt8  aInput[STR_DES_BLK_SIZE],
                           stlUInt8        aOutput[STR_DES_BLK_SIZE] );

stlInt32 strHex2Num( stlChar  aHexChar );

stlStatus strEncryptKey( stlChar        *aPlainText,
                         stlInt32        aPlainTextLen,
                         stlChar        *aLicenseBuffer );

stlStatus strDecryptKey( stlChar        *aLicenseKey,
                         stlInt32        aLicenseKeyLen,
                         stlChar        *aPlainText );

#endif  /* ! _STRDES_H_ */
