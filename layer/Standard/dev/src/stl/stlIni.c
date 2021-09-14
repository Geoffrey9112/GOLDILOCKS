/*******************************************************************************
 * stlIni.c
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

#include <stlDef.h>
#include <stlError.h>
#include <stlRing.h>
#include <stlAllocator.h>
#include <stlFile.h>
#include <stlMemorys.h>
#include <stlStrings.h>

/**
 * @file stlIni.c
 * @brief Standard Layer Ini Routines
 */


/**
 * @addtogroup stlIni
 * @{
 */

#define STL_INI_COMMENT       "#;"
#define STL_INI_LEFT_BRACKET  '['
#define STL_INI_RIGHT_BRACKET ']'
#define STL_INI_EQUAL         "="

#define STL_INI_REGION_INIT_SIZE (4096)
#define STL_INI_REGION_NEXT_SIZE (4096)

/**
 * @brief ini 파일을 열어 Object와 Property 정보를 구성한다.
 * @param[in] aIni         INI 핸들
 * @param[in] aFileName    ini 파일 경로
 * @param[out] aErrorStack 에러 스택
 */
stlStatus stlOpenIni( stlIni        * aIni,
                      stlChar       * aFileName,
                      stlErrorStack * aErrorStack )
{
    stlFile          sFile;
    stlInt32         sState = 0;
    stlChar        * sLine = NULL;
    stlChar          sString[STL_MAX_INI_LINE + 1];
    stlChar          sObjectName[STL_MAX_INI_OBJECT_NAME + 1];
    stlChar          sPropertyName[STL_MAX_INI_PROPERTY_NAME + 1];
    stlChar          sPropertyValue[STL_MAX_INI_PROPERTY_VALUE + 1];
    stlBool          sAllocator = STL_FALSE;
    stlInt32         sPos;
    stlIniObject   * sCurrentObject = NULL;
    stlIniObject   * sObject;
    stlIniProperty * sProperty;
    stlChar        * sSavePtr;
    stlChar        * sPropertyString;
    stlChar          sDetailErrorMessage[1024];
    stlInt32         sLineNo = 0;
    stlInt32         i;
    
    STL_PARAM_VALIDATE( aIni != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aFileName != NULL, aErrorStack );

    STL_TRY( stlOpenFile( &sFile,
                          aFileName,
                          STL_FOPEN_READ,
                          STL_FPERM_OS_DEFAULT,
                          aErrorStack ) == STL_SUCCESS );
    sState = 1;

    STL_RING_INIT_HEADLINK( &aIni->mObjectRing, STL_OFFSETOF( stlIniObject, mLink ) );

    STL_TRY( stlCreateRegionAllocator( &aIni->mAllocator,
                                       STL_INI_REGION_INIT_SIZE,
                                       STL_INI_REGION_NEXT_SIZE,
                                       aErrorStack ) == STL_SUCCESS );
    sAllocator = STL_TRUE;
    
    sString[0] = '\0';
    sLine = &sString[0];

    /*
     * 처음 Object를 구한다.
     */
    while( STL_TRUE )
    {
        sLineNo++;
        if( stlGetStringFile( sString,
                              STL_MAX_INI_LINE + 1,
                              &sFile,
                              aErrorStack ) != STL_SUCCESS )
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF );
            (void)stlPopError( aErrorStack );

            break;
        }

        for( i = 0; (stlIsspace(sString[i]) == STL_TRUE) && (sString[i] != '\0'); i++ );

        sLine = &(sString[i]);

        if( sLine[0] == STL_INI_LEFT_BRACKET )
        {
            break;
        }

        stlTrimString( sLine );

        if( sLine[0] == '\0' )
        {
            continue;
        }

        STL_TRY_THROW( stlStrchr( STL_INI_COMMENT, sLine[0] ) != NULL,
                       RAMP_ERR_INVALID_INI_FORMAT );
    }

    while( STL_TRUE )
    {
        if( sLine[0] == STL_INI_LEFT_BRACKET )
        {
            /*
             * OBJECT
             */
            sPos = 1;
            while( STL_TRUE )
            {
                if( (sLine[sPos] == '\0') || (sPos == STL_MAX_INI_OBJECT_NAME) )
                {
                    sObjectName[sPos - 1] = '\0';
                    break;
                }

                if( sLine[sPos] == STL_INI_RIGHT_BRACKET )
                {
                    sObjectName[sPos - 1] = '\0';
                    break;
                }

                sObjectName[sPos - 1] = sLine[sPos];
                sPos++;
            }

            stlTrimString( sObjectName );

            STL_TRY( stlAllocRegionMem( &aIni->mAllocator,
                                        STL_SIZEOF( stlIniObject ),
                                        (void**)&sObject,
                                        aErrorStack ) == STL_SUCCESS );

            STL_RING_INIT_HEADLINK( &sObject->mPropertyRing,
                                    STL_OFFSETOF( stlIniProperty, mLink ) );

            STL_RING_INIT_DATALINK( sObject, STL_OFFSETOF( stlIniObject, mLink ) );

            stlStrcpy( sObject->mName, sObjectName );

            STL_RING_ADD_LAST( &aIni->mObjectRing, sObject );

            sCurrentObject = sObject;
        }
        else if( ( stlStrchr( STL_INI_COMMENT, sLine[0] ) == NULL ) &&
                 ( stlIsspace( sLine[0] ) == STL_FALSE ) )
        {
            /*
             * PROPERTY
             */

            STL_TRY_THROW( sCurrentObject != NULL,
                           RAMP_ERR_INVALID_INI_FORMAT );

            sPropertyName[0]  = '\0';
            sPropertyValue[0] = '\0';

            sPropertyString = stlStrtok( sLine, STL_INI_EQUAL, &sSavePtr );
            STL_TRY_THROW( sPropertyString != NULL ,
                           RAMP_ERR_INVALID_INI_FORMAT );

            stlStrcpy( sPropertyName, sPropertyString );
            stlTrimString( sPropertyName );
            
            sPropertyString = stlStrtok( NULL, STL_INI_EQUAL, &sSavePtr );
            STL_TRY_THROW( sPropertyString != NULL,
                           RAMP_ERR_INVALID_INI_FORMAT );

            stlStrcpy( sPropertyValue, sPropertyString );
            stlTrimString( sPropertyValue );

            STL_TRY( stlAllocRegionMem( &aIni->mAllocator,
                                        STL_SIZEOF( stlIniProperty ),
                                        (void**)&sProperty,
                                        aErrorStack ) == STL_SUCCESS );

            STL_RING_INIT_DATALINK( sProperty, STL_OFFSETOF( stlIniProperty, mLink ) );

            stlStrcpy( sProperty->mName, sPropertyName );
            stlStrcpy( sProperty->mValue, sPropertyValue );

            STL_RING_ADD_LAST( &sCurrentObject->mPropertyRing, sProperty );
        }

        sLineNo++;
        if( stlGetStringFile( sLine,
                              STL_MAX_INI_LINE + 1,
                              &sFile,
                              aErrorStack ) != STL_SUCCESS )
        {
            STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF );
            (void)stlPopError( aErrorStack );

            break;
        }
        
        for( i = 0; (stlIsspace( sString[i] ) == STL_TRUE) && (sString[i] != '\0'); i++ );

        sLine = &(sString[i]);
    }

    sState = 0;
    STL_TRY( stlCloseFile( &sFile, aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_INI_FORMAT )
    {
        stlSnprintf( sDetailErrorMessage,
                     STL_SIZEOF( sDetailErrorMessage ),
                     "%s:%d",
                     aFileName, sLineNo );
                     
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_INI_FILE_FORMAT,
                      sDetailErrorMessage,
                      aErrorStack );
    }

    STL_FINISH;

    if( sAllocator == STL_TRUE )
    {
        (void)stlDestroyRegionAllocator( &aIni->mAllocator, aErrorStack );
    }

    switch( sState )
    {
        case 1 :
            (void)stlCloseFile( &sFile, aErrorStack );
        default :
            break;
    }

    return STL_FAILURE;
}

/**
 * @brief 구축된 ini 정보를 정리한다.
 * @param[in] aIni         INI 핸들
 * @param[out] aErrorStack 에러 스택
 */
stlStatus stlCloseIni( stlIni        * aIni,
                       stlErrorStack * aErrorStack )
{
    STL_TRY( stlDestroyRegionAllocator( &aIni->mAllocator,
                                        aErrorStack ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ini에서 주어진 Object 이름을 찾는다.
 * @param[in] aIni         INI 핸들
 * @param[in] aObjectName  Object 이름
 * @param[out] aFound      Object 존재 여부
 * @param[out] aObject     Object 포인터
 * @param[out] aErrorStack 에러 스택
 */
stlStatus stlFindIniObject( stlIni         * aIni,
                            stlChar        * aObjectName,
                            stlBool        * aFound,
                            stlIniObject  ** aObject,
                            stlErrorStack  * aErrorStack )
{
    stlIniObject * sObject;

    STL_ASSERT( aObject != NULL );

    *aObject = NULL;
    *aFound  = STL_FALSE;
    
    STL_RING_FOREACH_ENTRY( &aIni->mObjectRing, sObject )
    {
        if( stlStrcasecmp( sObject->mName, aObjectName ) == 0 )
        {
            *aObject = sObject;
            *aFound = STL_TRUE;
        }
    }

    return STL_SUCCESS;
}

/**
 * @brief 현재 가리키고 있는 Object에서 Property 이름에 해당하는 값을 구한다.
 * @param[in] aObject         Object 포인터
 * @param[in] aPropertyName   Property 이름
 * @param[out] aPropertyValue Property 값
 * @param[out] aFound         Property 존재 여부
 * @param[out] aErrorStack    에러 스택
 */
stlStatus stlFindIniProperty( stlIniObject  * aObject,
                              stlChar       * aPropertyName,
                              stlChar       * aPropertyValue,
                              stlBool       * aFound,
                              stlErrorStack * aErrorStack )
{
    stlIniObject   * sObject;
    stlIniProperty * sProperty;
    
    STL_TRY_THROW( aObject != NULL, RAMP_ERR_INVALID_INI_FORMAT );

    sObject = aObject;
    *aFound = STL_FALSE;

    STL_RING_FOREACH_ENTRY( &sObject->mPropertyRing, sProperty )
    {
        if( stlStrncasecmp( sProperty->mName,
                            aPropertyName,
                            STL_MAX_INI_PROPERTY_NAME ) == 0 )
        {
            stlStrcpy( aPropertyValue, sProperty->mValue );
            *aFound = STL_TRUE;
        }
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_INI_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_INI_FILE_FORMAT,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
