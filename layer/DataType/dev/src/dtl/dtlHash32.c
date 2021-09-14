/*******************************************************************************
 * dtlDataType.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlDataType.c 8852 2013-07-01 02:54:29Z mkkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file dtlDataType.c
 * @brief DataType Layer 데이타 타입관련 redirect 함수 정의
 */

#include <dtl.h>
#include <dtDef.h>

/**
 * @addtogroup dtlHash32
 * @{
 */

#define DTL_HASH_UINT32_ALIGN_MASK (sizeof(stlUInt32) - 1)

#define DTL_HASH_ROT( x, k ) (((x)<<(k)) | ((x)>>(32-(k))))

#define DTL_HASH_MIX( a, b, c )                         \
    {                                                   \
        a -= c;  a ^= DTL_HASH_ROT(c, 4);	c += b; \
        b -= a;  b ^= DTL_HASH_ROT(a, 6);	a += c; \
        c -= b;  c ^= DTL_HASH_ROT(b, 8);	b += a; \
        a -= c;  a ^= DTL_HASH_ROT(c,16);	c += b; \
        b -= a;  b ^= DTL_HASH_ROT(a,19);	a += c; \
        c -= b;  c ^= DTL_HASH_ROT(b, 4);	b += a; \
    }

#define DTL_HASH_FINAL( a, b, c )               \
    {                                           \
        c ^= b; c -= DTL_HASH_ROT(b,14);        \
        a ^= c; a -= DTL_HASH_ROT(c,11);        \
        b ^= a; b -= DTL_HASH_ROT(a,25);        \
        c ^= b; c -= DTL_HASH_ROT(b,16);        \
        a ^= c; a -= DTL_HASH_ROT(c, 4);        \
        b ^= a; b -= DTL_HASH_ROT(a,14);        \
        c ^= b; c -= DTL_HASH_ROT(b,24);        \
    }


dtlHash32Func gDtlHash32[DTL_TYPE_MAX] =
{
    dtlHash32Any,             /* BOOLEAN */ 
    dtlHash32Smallint,        /* NATIVE_SMALLINT */
    dtlHash32Int,             /* NATIVE_INTEGER */
    dtlHash32BigInt,          /* NATIVE_BIGINT */
    dtlHash32Int,             /* NATIVE_REAL */
    dtlHash32BigInt,          /* NATIVE_DOUBLE */
    dtlHash32Numeric,         /* FLOAT */    
    dtlHash32Numeric,         /* NUMBER */
    NULL,                     /* DECIMAL */
    dtlHash32CharString,      /* CHARACTER */ 
    dtlHash32CharString,      /* CHARACTER VARYING */ 
    dtlHash32CharString,      /* CHARACTER LONG VARYING */ 
    NULL,                     /* CLOB */ 
    dtlHash32BinaryString,    /* BINARY */ 
    dtlHash32BinaryString,    /* BINARY VARYING */ 
    dtlHash32BinaryString,    /* BINARY LONG VARYING */ 
    NULL,                     /* BLOB */
    
    dtlHash32Int,             /* DATE */ 
    dtlHash32BigInt,          /* TIME WITHOUT TIME ZONE */ 
    dtlHash32BigInt,          /* TIMESTAMP WITHOUT TIME ZONE */ 
    dtlHash32BigInt,          /* TIME WITH TIME ZONE */ 
    dtlHash32BigInt,          /* TIMESTAMP WITH TIME ZONE */ 
    
    dtlHash32IntervalYM,      /* INTERVAL_YEAR_TO_MONTH */ 
    dtlHash32IntervalDS,      /* INTERVAL_DAY_TO_SECOND */ 
    dtlHash32Any              /* ROWID */ 
};

stlUInt32 dtlHash32Numeric( register const stlUInt8 * aKeyValue,
                            register stlInt64         aKeyLength )
{
    register stlInt64   i;
    register stlUInt32  sHashKey = 0;

    for( i = 1; i < aKeyLength; i++ )
    {
        sHashKey *= 100;
        sHashKey += ((stlUInt32)(aKeyValue[i]) - 1);
    }

    return sHashKey;
}

stlUInt32 dtlHash32Smallint( register const stlUInt8 * aKeyValue,
                             register stlInt64         aKeyLength )
{
    stlInt16 sHashKey = 0;

    if( aKeyLength > 0 )
    {
        STL_WRITE_INT16( &sHashKey, aKeyValue );
    }

    return (stlUInt32)sHashKey;
}

stlUInt32 dtlHash32Int( register const stlUInt8 * aKeyValue,
                        register stlInt64         aKeyLength )
{
    stlInt32 sHashKey = 0;

    if( aKeyLength > 0 )
    {
        STL_WRITE_INT32( &sHashKey, aKeyValue );
    }

    return sHashKey;
}

stlUInt32 dtlHash32BigInt( register const stlUInt8 * aKeyValue,
                           register stlInt64         aKeyLength )
{
    register stlUInt32         a;
    register stlUInt32         b;
    register stlUInt32         c;
    register const stlUInt32 * sKeyValue;

    if( aKeyLength > 0 )
    {
        /* Set up the internal state */
        a = b = c = 0x9e3779b9 + 8 /*length*/ + 3923095;
 
        /* If the source pointer is word-aligned, we use word-wide fetches */
        if( (STL_INT_FROM_POINTER(aKeyValue) & DTL_HASH_UINT32_ALIGN_MASK) == 0 )
        {
            /* Code path for aligned source data */
            sKeyValue = (const stlUInt32 *) aKeyValue;
        
            /* handle the last 11 bytes */
            aKeyValue = (const stlUInt8 *) sKeyValue;

            b += sKeyValue[1];
            a += sKeyValue[0];
        }
        else
        {
            /* Code path for non-aligned source data */
#ifdef STL_IS_BIGENDIAN
            b += aKeyValue[7];
            b += ((stlUInt32) aKeyValue[6] << 8);
            b += ((stlUInt32) aKeyValue[5] << 16);
            b += ((stlUInt32) aKeyValue[4] << 24);
            a += aKeyValue[3];
            a += ((stlUInt32) aKeyValue[2] << 8);
            a += ((stlUInt32) aKeyValue[1] << 16);
            a += ((stlUInt32) aKeyValue[0] << 24);
#else
            b += ((stlUInt32) aKeyValue[7] << 24);
            b += ((stlUInt32) aKeyValue[6] << 16);
            b += ((stlUInt32) aKeyValue[5] << 8);
            b += aKeyValue[4];
            a += ((stlUInt32) aKeyValue[3] << 24);
            a += ((stlUInt32) aKeyValue[2] << 16);
            a += ((stlUInt32) aKeyValue[1] << 8);
            a += aKeyValue[0];
#endif   /* STL_IS_BIGENDIAN */
        }
    }
    else
    {
        a = b = c = 0x9e3779b9 + 3923095;
    }

    DTL_HASH_FINAL( a, b, c );

    /* report the result */
    return c;
}

/**
 * @brief 32bit Hash Key를 반환한다.
 * @param[in] aKeyValue   Key Value
 * @param[in] aKeyLength  Key Length
 * @return Hash Key
 * @remarks 해당 함수는 postgres로 부터 수정되었음
 */
stlUInt32 dtlHash32Any( register const stlUInt8 * aKeyValue,
                        register stlInt64         aKeyLength )
{
    register stlUInt32         a;
    register stlUInt32         b;
    register stlUInt32         c;
    register stlUInt32         sLen;
    register const stlUInt32 * sKeyValue;

    /* Set up the internal state */
    if( aKeyLength > 0 )
    {
        sLen = aKeyLength;
        a = b = c = 0x9e3779b9 + sLen + 3923095;

        /* If the source pointer is word-aligned, we use word-wide fetches */
        if( (STL_INT_FROM_POINTER(aKeyValue) & DTL_HASH_UINT32_ALIGN_MASK) == 0 )
        {
            /* Code path for aligned source data */
            sKeyValue = (const stlUInt32 *) aKeyValue;
        
            /* handle most of the key */
            while( sLen >= 12 )
            {
                a += sKeyValue[0];
                b += sKeyValue[1];
                c += sKeyValue[2];
                DTL_HASH_MIX( a, b, c );
                sKeyValue += 3;
                sLen -= 12;
            }

            /* handle the last 11 bytes */
            aKeyValue = (const stlUInt8 *) sKeyValue;
#ifdef STL_IS_BIGENDIAN
            switch( sLen )
            {
                case 11:
                    c += ((stlUInt32) aKeyValue[10] << 8);
                    /* fall through */
                case 10:
                    c += ((stlUInt32) aKeyValue[9] << 16);
                    /* fall through */
                case 9:
                    c += ((stlUInt32) aKeyValue[8] << 24);
                    /* the lowest byte of c is reserved for the length */
                    /* fall through */
                case 8:
                    b += sKeyValue[1];
                    a += sKeyValue[0];
                    break;
                case 7:
                    b += ((stlUInt32) aKeyValue[6] << 8);
                    /* fall through */
                case 6:
                    b += ((stlUInt32) aKeyValue[5] << 16);
                    /* fall through */
                case 5:
                    b += ((stlUInt32) aKeyValue[4] << 24);
                    /* fall through */
                case 4:
                    a += sKeyValue[0];
                    break;
                case 3:
                    a += ((stlUInt32) aKeyValue[2] << 8);
                    /* fall through */
                case 2:
                    a += ((stlUInt32) aKeyValue[1] << 16);
                    /* fall through */
                case 1:
                    a += ((stlUInt32) aKeyValue[0] << 24);
                    /* case 0: nothing left to add */
            }
#else							/* !STL_IS_BIGENDIAN */
            switch( sLen )
            {
                case 11:
                    c += ((stlUInt32) aKeyValue[10] << 24);
                    /* fall through */
                case 10:
                    c += ((stlUInt32) aKeyValue[9] << 16);
                    /* fall through */
                case 9:
                    c += ((stlUInt32) aKeyValue[8] << 8);
                    /* the lowest byte of c is reserved for the length */
                    /* fall through */
                case 8:
                    b += sKeyValue[1];
                    a += sKeyValue[0];
                    break;
                case 7:
                    b += ((stlUInt32) aKeyValue[6] << 16);
                    /* fall through */
                case 6:
                    b += ((stlUInt32) aKeyValue[5] << 8);
                    /* fall through */
                case 5:
                    b += aKeyValue[4];
                    /* fall through */
                case 4:
                    a += sKeyValue[0];
                    break;
                case 3:
                    a += ((stlUInt32) aKeyValue[2] << 16);
                    /* fall through */
                case 2:
                    a += ((stlUInt32) aKeyValue[1] << 8);
                    /* fall through */
                case 1:
                    a += aKeyValue[0];
                    /* case 0: nothing left to add */
            }
#endif   /* STL_IS_BIGENDIAN */
        }
        else
        {
            /* Code path for non-aligned source data */

            /* handle most of the key */
            while( sLen >= 12 )
            {
#ifdef STL_IS_BIGENDIAN
                a += ( aKeyValue[3] + ((stlUInt32) aKeyValue[2] << 8) +
                       ((stlUInt32) aKeyValue[1] << 16) + ((stlUInt32) aKeyValue[0] << 24) );
                b += ( aKeyValue[7] + ((stlUInt32) aKeyValue[6] << 8) +
                       ((stlUInt32) aKeyValue[5] << 16) + ((stlUInt32) aKeyValue[4] << 24) );
                c += ( aKeyValue[11] + ((stlUInt32) aKeyValue[10] << 8) +
                       ((stlUInt32) aKeyValue[9] << 16) + ((stlUInt32) aKeyValue[8] << 24) );
#else
                a += ( aKeyValue[0] + ((stlUInt32) aKeyValue[1] << 8) +
                       ((stlUInt32) aKeyValue[2] << 16) + ((stlUInt32) aKeyValue[3] << 24) );
                b += ( aKeyValue[4] + ((stlUInt32) aKeyValue[5] << 8) +
                       ((stlUInt32) aKeyValue[6] << 16) + ((stlUInt32) aKeyValue[7] << 24) );
                c += ( aKeyValue[8] + ((stlUInt32) aKeyValue[9] << 8) +
                       ((stlUInt32) aKeyValue[10] << 16) + ((stlUInt32) aKeyValue[11] << 24) );
#endif   /* STL_IS_BIGENDIAN */
                DTL_HASH_MIX( a, b, c );
                aKeyValue += 12;
                sLen -= 12;
            }

            /* handle the last 11 bytes */
#ifdef STL_IS_BIGENDIAN
            switch( sLen )       /* all the case statements fall through */
            {
                case 11:
                    c += ((stlUInt32) aKeyValue[10] << 8);
                case 10:
                    c += ((stlUInt32) aKeyValue[9] << 16);
                case 9:
                    c += ((stlUInt32) aKeyValue[8] << 24);
                    /* the lowest byte of c is reserved for the length */
                case 8:
                    b += aKeyValue[7];
                case 7:
                    b += ((stlUInt32) aKeyValue[6] << 8);
                case 6:
                    b += ((stlUInt32) aKeyValue[5] << 16);
                case 5:
                    b += ((stlUInt32) aKeyValue[4] << 24);
                case 4:
                    a += aKeyValue[3];
                case 3:
                    a += ((stlUInt32) aKeyValue[2] << 8);
                case 2:
                    a += ((stlUInt32) aKeyValue[1] << 16);
                case 1:
                    a += ((stlUInt32) aKeyValue[0] << 24);
                    /* case 0: nothing left to add */
            }
#else
            switch( sLen )          /* all the case statements fall through */
            {
                case 11:
                    c += ((stlUInt32) aKeyValue[10] << 24);
                case 10:
                    c += ((stlUInt32) aKeyValue[9] << 16);
                case 9:
                    c += ((stlUInt32) aKeyValue[8] << 8);
                    /* the lowest byte of c is reserved for the length */
                case 8:
                    b += ((stlUInt32) aKeyValue[7] << 24);
                case 7:
                    b += ((stlUInt32) aKeyValue[6] << 16);
                case 6:
                    b += ((stlUInt32) aKeyValue[5] << 8);
                case 5:
                    b += aKeyValue[4];
                case 4:
                    a += ((stlUInt32) aKeyValue[3] << 24);
                case 3:
                    a += ((stlUInt32) aKeyValue[2] << 16);
                case 2:
                    a += ((stlUInt32) aKeyValue[1] << 8);
                case 1:
                    a += aKeyValue[0];
                    /* case 0: nothing left to add */
            }
#endif   /* STL_IS_BIGENDIAN */
        }
    }
    else
    {
        a = b = c = 0x9e3779b9 + 3923095;
    }
    
    DTL_HASH_FINAL( a, b, c );

    /* report the result */
    return c;
}


/**
 * @brief character string에 대한 32bit Hash Key를 반환한다.
 * @param[in] aKeyValue   Key Value
 * @param[in] aKeyLength  Key Length
 * @return Hash Key
 * @remarks trailing space를 제거하여 hash key 구성 (해당 함수는 postgres로 부터 수정되었음)
 */
stlUInt32 dtlHash32CharString( register const stlUInt8 * aKeyValue,
                               register stlInt64         aKeyLength )
{
    register stlUInt32         a;
    register stlUInt32         b;
    register stlUInt32         c;
    register stlUInt32         sLen;
    register const stlUInt32 * sKeyValue;

    /* Set up the internal state */
    sLen = aKeyLength;
    if( ( sLen > 0 ) && ( aKeyValue[sLen-1] == ' ' ) )
    {
        while( sLen > 0 )
        {
            sLen--;
            if( aKeyValue[sLen] != ' ' )
            {
                sLen++;
                break;
            }
        }
    }
    
    if( sLen > 0 )
    {
        a = b = c = 0x9e3779b9 + sLen + 3923095;

        /* If the source pointer is word-aligned, we use word-wide fetches */
        if( (STL_INT_FROM_POINTER(aKeyValue) & DTL_HASH_UINT32_ALIGN_MASK) == 0 )
        {
            /* Code path for aligned source data */
            sKeyValue = (const stlUInt32 *) aKeyValue;
        
            /* handle most of the key */
            while( sLen >= 12 )
            {
                a += sKeyValue[0];
                b += sKeyValue[1];
                c += sKeyValue[2];
                DTL_HASH_MIX( a, b, c );
                sKeyValue += 3;
                sLen -= 12;
            }

            /* handle the last 11 bytes */
            aKeyValue = (const stlUInt8 *) sKeyValue;
#ifdef STL_IS_BIGENDIAN
            switch( sLen )
            {
                case 11:
                    c += ((stlUInt32) aKeyValue[10] << 8);
                    /* fall through */
                case 10:
                    c += ((stlUInt32) aKeyValue[9] << 16);
                    /* fall through */
                case 9:
                    c += ((stlUInt32) aKeyValue[8] << 24);
                    /* the lowest byte of c is reserved for the length */
                    /* fall through */
                case 8:
                    b += sKeyValue[1];
                    a += sKeyValue[0];
                    break;
                case 7:
                    b += ((stlUInt32) aKeyValue[6] << 8);
                    /* fall through */
                case 6:
                    b += ((stlUInt32) aKeyValue[5] << 16);
                    /* fall through */
                case 5:
                    b += ((stlUInt32) aKeyValue[4] << 24);
                    /* fall through */
                case 4:
                    a += sKeyValue[0];
                    break;
                case 3:
                    a += ((stlUInt32) aKeyValue[2] << 8);
                    /* fall through */
                case 2:
                    a += ((stlUInt32) aKeyValue[1] << 16);
                    /* fall through */
                case 1:
                    a += ((stlUInt32) aKeyValue[0] << 24);
                    /* case 0: nothing left to add */
            }
#else							/* !STL_IS_BIGENDIAN */
            switch( sLen )
            {
                case 11:
                    c += ((stlUInt32) aKeyValue[10] << 24);
                    /* fall through */
                case 10:
                    c += ((stlUInt32) aKeyValue[9] << 16);
                    /* fall through */
                case 9:
                    c += ((stlUInt32) aKeyValue[8] << 8);
                    /* the lowest byte of c is reserved for the length */
                    /* fall through */
                case 8:
                    b += sKeyValue[1];
                    a += sKeyValue[0];
                    break;
                case 7:
                    b += ((stlUInt32) aKeyValue[6] << 16);
                    /* fall through */
                case 6:
                    b += ((stlUInt32) aKeyValue[5] << 8);
                    /* fall through */
                case 5:
                    b += aKeyValue[4];
                    /* fall through */
                case 4:
                    a += sKeyValue[0];
                    break;
                case 3:
                    a += ((stlUInt32) aKeyValue[2] << 16);
                    /* fall through */
                case 2:
                    a += ((stlUInt32) aKeyValue[1] << 8);
                    /* fall through */
                case 1:
                    a += aKeyValue[0];
                    /* case 0: nothing left to add */
            }
#endif   /* STL_IS_BIGENDIAN */
        }
        else
        {
            /* Code path for non-aligned source data */

            /* handle most of the key */
            while( sLen >= 12 )
            {
#ifdef STL_IS_BIGENDIAN
                a += ( aKeyValue[3] + ((stlUInt32) aKeyValue[2] << 8) +
                       ((stlUInt32) aKeyValue[1] << 16) + ((stlUInt32) aKeyValue[0] << 24) );
                b += ( aKeyValue[7] + ((stlUInt32) aKeyValue[6] << 8) +
                       ((stlUInt32) aKeyValue[5] << 16) + ((stlUInt32) aKeyValue[4] << 24) );
                c += ( aKeyValue[11] + ((stlUInt32) aKeyValue[10] << 8) +
                       ((stlUInt32) aKeyValue[9] << 16) + ((stlUInt32) aKeyValue[8] << 24) );
#else
                a += ( aKeyValue[0] + ((stlUInt32) aKeyValue[1] << 8) +
                       ((stlUInt32) aKeyValue[2] << 16) + ((stlUInt32) aKeyValue[3] << 24) );
                b += ( aKeyValue[4] + ((stlUInt32) aKeyValue[5] << 8) +
                       ((stlUInt32) aKeyValue[6] << 16) + ((stlUInt32) aKeyValue[7] << 24) );
                c += ( aKeyValue[8] + ((stlUInt32) aKeyValue[9] << 8) +
                       ((stlUInt32) aKeyValue[10] << 16) + ((stlUInt32) aKeyValue[11] << 24) );
#endif   /* STL_IS_BIGENDIAN */
                DTL_HASH_MIX( a, b, c );
                aKeyValue += 12;
                sLen -= 12;
            }

            /* handle the last 11 bytes */
#ifdef STL_IS_BIGENDIAN
            switch( sLen )       /* all the case statements fall through */
            {
                case 11:
                    c += ((stlUInt32) aKeyValue[10] << 8);
                case 10:
                    c += ((stlUInt32) aKeyValue[9] << 16);
                case 9:
                    c += ((stlUInt32) aKeyValue[8] << 24);
                    /* the lowest byte of c is reserved for the length */
                case 8:
                    b += aKeyValue[7];
                case 7:
                    b += ((stlUInt32) aKeyValue[6] << 8);
                case 6:
                    b += ((stlUInt32) aKeyValue[5] << 16);
                case 5:
                    b += ((stlUInt32) aKeyValue[4] << 24);
                case 4:
                    a += aKeyValue[3];
                case 3:
                    a += ((stlUInt32) aKeyValue[2] << 8);
                case 2:
                    a += ((stlUInt32) aKeyValue[1] << 16);
                case 1:
                    a += ((stlUInt32) aKeyValue[0] << 24);
                    /* case 0: nothing left to add */
            }
#else
            switch( sLen )          /* all the case statements fall through */
            {
                case 11:
                    c += ((stlUInt32) aKeyValue[10] << 24);
                case 10:
                    c += ((stlUInt32) aKeyValue[9] << 16);
                case 9:
                    c += ((stlUInt32) aKeyValue[8] << 8);
                    /* the lowest byte of c is reserved for the length */
                case 8:
                    b += ((stlUInt32) aKeyValue[7] << 24);
                case 7:
                    b += ((stlUInt32) aKeyValue[6] << 16);
                case 6:
                    b += ((stlUInt32) aKeyValue[5] << 8);
                case 5:
                    b += aKeyValue[4];
                case 4:
                    a += ((stlUInt32) aKeyValue[3] << 24);
                case 3:
                    a += ((stlUInt32) aKeyValue[2] << 16);
                case 2:
                    a += ((stlUInt32) aKeyValue[1] << 8);
                case 1:
                    a += aKeyValue[0];
                    /* case 0: nothing left to add */
            }
#endif   /* STL_IS_BIGENDIAN */
        }
    }
    else
    {
        a = b = c = 0x9e3779b9 + 3923095;
    }
    
    DTL_HASH_FINAL( a, b, c );

    /* report the result */
    return c;
}


/**
 * @brief binary string에 대한 32bit Hash Key를 반환한다.
 * @param[in] aKeyValue   Key Value
 * @param[in] aKeyLength  Key Length
 * @return Hash Key
 * @remarks trailing space를 제거하여 hash key 구성 (해당 함수는 postgres로 부터 수정되었음)
 */
stlUInt32 dtlHash32BinaryString( register const stlUInt8 * aKeyValue,
                                 register stlInt64         aKeyLength )
{
    register stlUInt32         a;
    register stlUInt32         b;
    register stlUInt32         c;
    register stlUInt32         sLen;
    register const stlUInt32 * sKeyValue;

    /* Set up the internal state */
    sLen = aKeyLength;
    if( ( sLen > 0 ) && ( aKeyValue[sLen-1] == 0x00 ) )
    {
        while( sLen > 0 )
        {
            sLen--;
            if( aKeyValue[sLen] != 0x00 )
            {
                sLen++;
                break;
            }
        }
    }
    
    if( sLen > 0 )
    {
        a = b = c = 0x9e3779b9 + sLen + 3923095;

        /* If the source pointer is word-aligned, we use word-wide fetches */
        if( (STL_INT_FROM_POINTER(aKeyValue) & DTL_HASH_UINT32_ALIGN_MASK) == 0 )
        {
            /* Code path for aligned source data */
            sKeyValue = (const stlUInt32 *) aKeyValue;
        
            /* handle most of the key */
            while( sLen >= 12 )
            {
                a += sKeyValue[0];
                b += sKeyValue[1];
                c += sKeyValue[2];
                DTL_HASH_MIX( a, b, c );
                sKeyValue += 3;
                sLen -= 12;
            }

            /* handle the last 11 bytes */
            aKeyValue = (const stlUInt8 *) sKeyValue;
#ifdef STL_IS_BIGENDIAN
            switch( sLen )
            {
                case 11:
                    c += ((stlUInt32) aKeyValue[10] << 8);
                    /* fall through */
                case 10:
                    c += ((stlUInt32) aKeyValue[9] << 16);
                    /* fall through */
                case 9:
                    c += ((stlUInt32) aKeyValue[8] << 24);
                    /* the lowest byte of c is reserved for the length */
                    /* fall through */
                case 8:
                    b += sKeyValue[1];
                    a += sKeyValue[0];
                    break;
                case 7:
                    b += ((stlUInt32) aKeyValue[6] << 8);
                    /* fall through */
                case 6:
                    b += ((stlUInt32) aKeyValue[5] << 16);
                    /* fall through */
                case 5:
                    b += ((stlUInt32) aKeyValue[4] << 24);
                    /* fall through */
                case 4:
                    a += sKeyValue[0];
                    break;
                case 3:
                    a += ((stlUInt32) aKeyValue[2] << 8);
                    /* fall through */
                case 2:
                    a += ((stlUInt32) aKeyValue[1] << 16);
                    /* fall through */
                case 1:
                    a += ((stlUInt32) aKeyValue[0] << 24);
                    /* case 0: nothing left to add */
            }
#else							/* !STL_IS_BIGENDIAN */
            switch( sLen )
            {
                case 11:
                    c += ((stlUInt32) aKeyValue[10] << 24);
                    /* fall through */
                case 10:
                    c += ((stlUInt32) aKeyValue[9] << 16);
                    /* fall through */
                case 9:
                    c += ((stlUInt32) aKeyValue[8] << 8);
                    /* the lowest byte of c is reserved for the length */
                    /* fall through */
                case 8:
                    b += sKeyValue[1];
                    a += sKeyValue[0];
                    break;
                case 7:
                    b += ((stlUInt32) aKeyValue[6] << 16);
                    /* fall through */
                case 6:
                    b += ((stlUInt32) aKeyValue[5] << 8);
                    /* fall through */
                case 5:
                    b += aKeyValue[4];
                    /* fall through */
                case 4:
                    a += sKeyValue[0];
                    break;
                case 3:
                    a += ((stlUInt32) aKeyValue[2] << 16);
                    /* fall through */
                case 2:
                    a += ((stlUInt32) aKeyValue[1] << 8);
                    /* fall through */
                case 1:
                    a += aKeyValue[0];
                    /* case 0: nothing left to add */
            }
#endif   /* STL_IS_BIGENDIAN */
        }
        else
        {
            /* Code path for non-aligned source data */

            /* handle most of the key */
            while( sLen >= 12 )
            {
#ifdef STL_IS_BIGENDIAN
                a += ( aKeyValue[3] + ((stlUInt32) aKeyValue[2] << 8) +
                       ((stlUInt32) aKeyValue[1] << 16) + ((stlUInt32) aKeyValue[0] << 24) );
                b += ( aKeyValue[7] + ((stlUInt32) aKeyValue[6] << 8) +
                       ((stlUInt32) aKeyValue[5] << 16) + ((stlUInt32) aKeyValue[4] << 24) );
                c += ( aKeyValue[11] + ((stlUInt32) aKeyValue[10] << 8) +
                       ((stlUInt32) aKeyValue[9] << 16) + ((stlUInt32) aKeyValue[8] << 24) );
#else
                a += ( aKeyValue[0] + ((stlUInt32) aKeyValue[1] << 8) +
                       ((stlUInt32) aKeyValue[2] << 16) + ((stlUInt32) aKeyValue[3] << 24) );
                b += ( aKeyValue[4] + ((stlUInt32) aKeyValue[5] << 8) +
                       ((stlUInt32) aKeyValue[6] << 16) + ((stlUInt32) aKeyValue[7] << 24) );
                c += ( aKeyValue[8] + ((stlUInt32) aKeyValue[9] << 8) +
                       ((stlUInt32) aKeyValue[10] << 16) + ((stlUInt32) aKeyValue[11] << 24) );
#endif   /* STL_IS_BIGENDIAN */
                DTL_HASH_MIX( a, b, c );
                aKeyValue += 12;
                sLen -= 12;
            }

            /* handle the last 11 bytes */
#ifdef STL_IS_BIGENDIAN
            switch( sLen )       /* all the case statements fall through */
            {
                case 11:
                    c += ((stlUInt32) aKeyValue[10] << 8);
                case 10:
                    c += ((stlUInt32) aKeyValue[9] << 16);
                case 9:
                    c += ((stlUInt32) aKeyValue[8] << 24);
                    /* the lowest byte of c is reserved for the length */
                case 8:
                    b += aKeyValue[7];
                case 7:
                    b += ((stlUInt32) aKeyValue[6] << 8);
                case 6:
                    b += ((stlUInt32) aKeyValue[5] << 16);
                case 5:
                    b += ((stlUInt32) aKeyValue[4] << 24);
                case 4:
                    a += aKeyValue[3];
                case 3:
                    a += ((stlUInt32) aKeyValue[2] << 8);
                case 2:
                    a += ((stlUInt32) aKeyValue[1] << 16);
                case 1:
                    a += ((stlUInt32) aKeyValue[0] << 24);
                    /* case 0: nothing left to add */
            }
#else
            switch( sLen )          /* all the case statements fall through */
            {
                case 11:
                    c += ((stlUInt32) aKeyValue[10] << 24);
                case 10:
                    c += ((stlUInt32) aKeyValue[9] << 16);
                case 9:
                    c += ((stlUInt32) aKeyValue[8] << 8);
                    /* the lowest byte of c is reserved for the length */
                case 8:
                    b += ((stlUInt32) aKeyValue[7] << 24);
                case 7:
                    b += ((stlUInt32) aKeyValue[6] << 16);
                case 6:
                    b += ((stlUInt32) aKeyValue[5] << 8);
                case 5:
                    b += aKeyValue[4];
                case 4:
                    a += ((stlUInt32) aKeyValue[3] << 24);
                case 3:
                    a += ((stlUInt32) aKeyValue[2] << 16);
                case 2:
                    a += ((stlUInt32) aKeyValue[1] << 8);
                case 1:
                    a += aKeyValue[0];
                    /* case 0: nothing left to add */
            }
#endif   /* STL_IS_BIGENDIAN */
        }
    }
    else
    {
        a = b = c = 0x9e3779b9 + 3923095;
    }

    DTL_HASH_FINAL( a, b, c );

    /* report the result */
    return c;
}


stlUInt32 dtlHash32IntervalYM( register const stlUInt8 * aKeyValue,
                               register stlInt64         aKeyLength )
{
    stlInt32 sHashKey = 0;

    if( aKeyLength > 0 )
    {
        STL_DASSERT( aKeyLength == DTL_INTERVAL_YEAR_TO_MONTH_SIZE );
        STL_WRITE_INT32( &sHashKey, aKeyValue );
    }

    return sHashKey;
}

stlUInt32 dtlHash32IntervalDS( register const stlUInt8 * aKeyValue,
                               register stlInt64         aKeyLength )
{
    if( aKeyLength > 0 )
    {
        STL_DASSERT( aKeyLength == DTL_INTERVAL_DAY_TO_SECOND_SIZE );

        return dtlHash32Any( aKeyValue,
                             12 );
    }
    else
    {
        return dtlHash32Any( aKeyValue,
                             0 );
    }
}

/** @} */
