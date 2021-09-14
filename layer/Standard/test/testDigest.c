#include <stl.h>

static stlInt32 gIndex = 0;

stlInt32 strSnprintf(stlChar        *aOutputBuffer,
                     stlSize         aOutputBufferLength,
                     const stlChar  *aFormat,
                     ...);

stlStatus testDigest( stlChar           * aInput,
                      stlChar           aResult[STL_DIGEST_SIZE*2 + 1],
                      stlErrorStack     * aErrorStack );
/*
 *
 *    아래 값으로 검증함.
 *    input : "abc"
 *    sha1  : "a9993e364706816aba3e25717850c26c9cd0d89d"
 *    input : "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
 *    sha1  : "84983e441c3bd26ebaae4aa1f95129e5e54670f1"
 */

stlInt32 main( stlInt32 aArgc, stlChar** aArgv )
{
    stlErrorStack     sErrorStack;

    STL_TRY( stlInitialize() == STL_SUCCESS );

    STL_INIT_ERROR_STACK( &sErrorStack );

    STL_TRY( testDigest( "abc",
                         "a9993e364706816aba3e25717850c26c9cd0d89d",
                         &sErrorStack )
             == STL_SUCCESS );
    STL_TRY( testDigest( "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
                         "84983e441c3bd26ebaae4aa1f95129e5e54670f1",
                         &sErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus testDigest( stlChar           * aInput,
                      stlChar           aResult[STL_DIGEST_SIZE*2 + 1],
                      stlErrorStack     * aErrorStack )
{
    stlUInt8          sResult[STL_DIGEST_SIZE];
    stlChar           sStrResult[STL_DIGEST_SIZE*2 + 1];
    stlInt32          sCmpResult;
    stlInt32          i;

    STL_TRY( stlDigest( (stlUInt8 *)aInput,
                        stlStrlen( aInput ),
                        sResult,
                        aErrorStack )
             == STL_SUCCESS );

    for( i=0; i<STL_DIGEST_SIZE; i++ )
    {
        strSnprintf( &sStrResult[i*2],
                     (STL_DIGEST_SIZE-i)*2 + 1,
                     "%02x",
                     sResult[i] );
    }

    sCmpResult = stlStrcmp( sStrResult, aResult );

    gIndex++;
    if ( sCmpResult == 0 )
    {   /* success */
        stlPrintf( "%d. Digest test :%s\n"
                   "  => PASS\n",
                   gIndex,
                   aInput );
    }
    else
    {   /* failure */
        stlPrintf( "%d. Digest test :%s\n"
                   "  => not PASS\n",
                   gIndex,
                   aInput );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


