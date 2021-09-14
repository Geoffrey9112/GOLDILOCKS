
/*******************************************************************************
 * knpPropertyGrammar.l
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *        Property 관련 함수 원형
 *
 ******************************************************************************/
 
%{

#include <knl.h>

#include <knDef.h>    
#include <knpDef.h>
#include <knpPropertyManager.h>
#include <knpPropertyFile.h>

enum {
    KNP_NAME              = 1,
    KNP_STRING            = 2,
    KNP_INTEGER           = 3,
    KNP_EQUALS            = 4,
    KNP_UNQUOTED_STRING   = 5,
    KNP_QUALIFIED_ID      = 6,
    KNP_COMMENT           = 7,
    KNP_EOL               = 99,
    KNP_ERROR             = 100
};

stlInt32 gKnpPropertyFileLineNo;

%}

%option 8bit
%option never-interactive
%option noinput
%option nounput
%option noyywrap
        
SIGN            ("-"|"+")
DIGIT           [0-9]
HEXDIGIT        [0-9a-fA-F]

UNIT_LETTER     [a-zA-Z]

INTEGER         {SIGN}?({DIGIT}+|0x{HEXDIGIT}+){UNIT_LETTER}*

LETTER          [A-Za-z_]
LETTER_OR_DIGIT [A-Za-z_0-9]

ID              {LETTER}{LETTER_OR_DIGIT}*
QUALIFIED_ID    {ID}"."{ID}

UNQUOTED_STRING {LETTER}({LETTER_OR_DIGIT}|[-._:/])*
STRING          \'([^'\\\n]|\\.|\'\')*\'
        
%%    
        
\n                 gKnpPropertyFileLineNo++; return KNP_EOL;
[ \t\r]+           /* whitespace */
#.*                return KNP_COMMENT; /* eat comment (.* matches anything until newline) */

{ID}               return KNP_NAME;
{QUALIFIED_ID}     return KNP_QUALIFIED_ID;
{STRING}           return KNP_STRING;
{UNQUOTED_STRING}  return KNP_UNQUOTED_STRING;
{INTEGER}          return KNP_INTEGER;
=                  return KNP_EQUALS;
.                  return KNP_ERROR;
%%
        
stlStatus knpProcessTextPropertyFile( stlChar  * aFileName, 
                                      knlEnv   * aKnlEnv )
{
    knpPropertyContent * sContent  = NULL;
    stlChar              sPropertyName[KNL_PROPERTY_NAME_MAX_LENGTH]  = {0};
    stlChar              sPropertyValue[KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
    
    stlInt64             sUnitValue  = 0;
    stlInt32             sStatus     = 0;
            
    stlFile              sFile;
    stlOffset            sOffset          = 0;
    stlOffset            sFileSize        = 0;
    stlChar            * sFileContent     = NULL;
    stlSize              sReadBytes       = 0;
    stlBool              sFileExist       = STL_FALSE;
    knlPropertyDataType  sDataType        = KNL_PROPERTY_TYPE_MAX;
    
    stlInt32             sToken           = 0;
    stlInt32             sLen             = 0;
    
    STL_TRY( knpGetFileExist( aFileName, 
                              &sFileExist,
                              aKnlEnv ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
    
    /** File Open */
    STL_TRY ( stlOpenFile( &sFile,
                           aFileName,
                           STL_FOPEN_READ,
                           STL_FPERM_OS_DEFAULT,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    sStatus = 1;
    
    /** Get FileSize for Memory Allocation */
    sFileSize = 0;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sFileSize,
                          KNL_ERROR_STACK(aKnlEnv) ) == STL_SUCCESS );
    
    /** Memory Allocation */

    STL_TRY( knlAllocRegionMem( &(gKnpPropertyMgr->mHeapMem),
                                sFileSize + 1,
                                (void**)&sFileContent,
                                aKnlEnv ) == STL_SUCCESS );  
    
    stlMemset( sFileContent, 0x00, sFileSize + 1 );
    
    sOffset = 0;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK(aKnlEnv) ) == STL_SUCCESS );
    
    /** FILE Read into Buffer */
    sReadBytes = 0;
    STL_TRY( stlReadFile( &sFile,
                          sFileContent,
                          sFileSize,
                          &sReadBytes,
                          KNL_ERROR_STACK(aKnlEnv) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFileSize == sReadBytes, RAMP_ERR_INVALID_FILE_PROCESS );
    
    (void)yy_scan_bytes( sFileContent, sFileSize );
    sStatus = 2;
    
    
    gKnpPropertyFileLineNo = 1;
            
    /**
     * Per logical line in File
     */
    while( (sToken = yylex()) )
    {
        /** Re-Initialize */
        stlMemset( sPropertyName,  0x00, STL_SIZEOF( sPropertyName ) );
        stlMemset( sPropertyValue, 0x00, STL_SIZEOF( sPropertyValue ) );
        
        sContent = NULL;
        
        if( sToken == KNP_EOL ||
            sToken == KNP_COMMENT )
        {
            continue;      
        }
        
        /** Identify KNP_NAME */
        if( sToken == KNP_NAME )
        {
            sLen = stlStrlen( yytext );
            STL_TRY( sLen <= STL_SIZEOF( sPropertyName ) );
            stlStrncpy( sPropertyName, yytext, sLen + 1 );
            
            //stlPrintf("TEXT NAME = %s[%d]\n", sPropertyName, sLen);
            
            STL_TRY( knpGetPropertyHandleByName ( sPropertyName,
                                                  (void**)&sContent,
                                                  aKnlEnv ) == STL_SUCCESS );
            STL_TRY( sContent != NULL );
            
        }
        else
        {
            //주석일 경우에는 괜찮음
            if ( sToken !=  KNP_COMMENT )
            {
                STL_THROW( RAMP_ERR_FILE_PARSING_ERROR );
            }
        }
        
        /** Identify KNP_EQUALS */
        sToken = yylex();
        
        if (sToken == KNP_EQUALS)
        {
            /** Identify KNP_VALUE */
            sToken = yylex();
            if ( ( sToken == KNP_NAME ) ||
                 ( sToken == KNP_STRING ) ||   
                 ( sToken == KNP_UNQUOTED_STRING ) ||
                 ( sToken == KNP_INTEGER ) )
            {
                /**
                 * Read-Only Property는 Text 파일에서만 변경할 수 있도록 한다.
                 * Create DB시에만 변경되며 Control File이나 DB에 저장된다.
                 */
                
                /**
                 * get Value
                 */                 
                sLen = stlStrlen( yytext );
                STL_TRY( sLen <= STL_SIZEOF( sPropertyValue ) );
                
                stlStrncpy( sPropertyValue, yytext, sLen + 1 );
                //stlPrintf("TEXT VALUE = %s[%d]\n", sPropertyValue, sLen);

                /*
                 * <GOLDILOCKS_DATA> Directory 변경
                 * backslash 처리.. 해야하나?
                 */
        
                if( sToken == KNP_STRING )
                {
                    if( sLen >= 2 )
                    {
                        /** Single Quotation 제거 */
                        if( stlStrncmp( sPropertyValue, "'", 1 ) == 0 )
                        {
                            stlStrncpy( sPropertyValue, &sPropertyValue[1], sLen );
                            sLen--;
                            
                            STL_TRY_THROW( stlStrncmp( &sPropertyValue[sLen-1], "'", 1 ) == 0, RAMP_ERR_FILE_PARSING_ERROR );
                            
                            sPropertyValue[sLen-1] = '\0';
                            sLen--;
                        }
                    }    
                }
                
                /**
                 * UNIT SIZE 변경 
                 * TIME/FILESIZE
                 */
                STL_TRY( knpGetPropertyDataType( sContent,
                                                 &sDataType,
                                                 aKnlEnv ) == STL_SUCCESS );

                /**
                 * PROPERTY의 DataType이 BIGINT인데 STRING DataType이 왔을 경우에는 
                 * UNIT을 포함하고 있는 경우인지 확인한다.
                 */
                //stlPrintf("%s %s %d %d\n", sPropertyName, sPropertyValue, sDataType, sToken);
                if( ( sDataType == KNL_PROPERTY_TYPE_BIGINT ) &&
                    ( sToken == KNP_STRING || sToken == KNP_NAME || sToken == KNP_INTEGER ) )
                {
                    STL_TRY( knpConvertUnitToValue( sContent,
                                                    sPropertyValue,
                                                    &sUnitValue,
                                                    aKnlEnv ) == STL_SUCCESS );
                    
                    stlMemset( sPropertyValue, 0x00, STL_SIZEOF( sPropertyValue ) );
                    
                    stlSnprintf( sPropertyValue, STL_SIZEOF( sPropertyValue ),
                                 "%ld", sUnitValue );
                }
                      
                
                
                STL_TRY( knpUpdatePropertyValueUseStr( sContent, 
                                                       sPropertyValue,
                                                       KNL_PROPERTY_MODE_SYS,
                                                       STL_FALSE,
                                                       KNL_PROPERTY_SCOPE_MEMORY,
                                                       aKnlEnv ) == STL_SUCCESS );
                /**
                 * SOURCE 변경
                 */
                sContent->mSource = KNP_SOURCE_TEXTFILE;
            }
            else
            {
                STL_THROW( RAMP_ERR_FILE_PARSING_ERROR );
            }
        }
        else
        {
            STL_THROW( RAMP_ERR_FILE_PARSING_ERROR );
        }
        
    }

    sStatus = 1;
    (void)yylex_destroy();          /* to avoid leakage */
    
    sStatus = 0;
    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
       
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aFileName );
    }
    STL_CATCH( RAMP_ERR_INVALID_FILE_PROCESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERR_FILE_PARSING_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_FILE_PARSING_ERROR,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      gKnpPropertyFileLineNo );
    }
    STL_FINISH;
    
    switch( sStatus )
    {
        case 2:
            (void)yylex_destroy();
        case 1:
            (void)stlCloseFile( &sFile, KNL_ERROR_STACK( aKnlEnv ) );    
        default:
            break;
    }
    
    return STL_FAILURE;
}



stlStatus knpConvertPropertyTextToBinaryFile( stlChar  * aTFileName,  
                                              stlChar  * aBFileName,  
                                              knlEnv   * aKnlEnv )
{
    knpPropertyContent * sContent  = NULL;
    stlFile              sSrcFile;
    
    stlOffset            sOffset          = 0;
    stlOffset            sFileSize        = 0;
    stlChar            * sFileContent     = NULL;
    stlSize              sReadBytes       = 0;
    stlInt32             sStatus          = 0;
    
    stlBool              sModifiable      = STL_FALSE;
    stlBool              sFileExist       = STL_FALSE;
    knlPropertyDataType  sType            = KNL_PROPERTY_TYPE_MAX;
    
    /** Temp Value */
    stlInt64             sTmpNumberValue  = 0;
    stlInt64             sTmpBoolValue    = 0;
    stlBool              sTmpValue        = STL_FALSE;
    
    stlChar              sPropertyName[KNL_PROPERTY_NAME_MAX_LENGTH]  = {0};
    stlChar              sPropertyValue[KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
    
    /** For Lexer */
    stlInt32             sToken           = 0;
    stlInt32             sLen             = 0;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aTFileName != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aBFileName != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    
    /** File Exist ? 
     * Source가 되는 Text PropertyFile은 반드시 있어야 하며,
     * Dest가 되는 Binary PropertyFile은 파일이 존재하더라도 덮어씌운다.
     */
    STL_TRY( knpGetFileExist( aTFileName,
                              &sFileExist,
                              aKnlEnv ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
        
    /** Source File Open */
    STL_TRY ( stlOpenFile( &sSrcFile,
                           aTFileName,
                           STL_FOPEN_READ,
                           STL_FPERM_OS_DEFAULT,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus = 1;
    
    /** Create destination Property file with Blank */
    STL_TRY( knpCreateBinaryPropertyWithBlank( aBFileName, 
                                               aKnlEnv ) == STL_SUCCESS );
    

    /** Get FileSize for Memory Allocation */
    sFileSize = 0;
    STL_TRY( stlSeekFile( &sSrcFile,
                          STL_FSEEK_END,
                          &sFileSize,
                          KNL_ERROR_STACK(aKnlEnv) ) == STL_SUCCESS );
    
    
    /** Memory Allocation */
    STL_TRY( stlAllocHeap( (void**)&sFileContent,
                           sFileSize + 1,
                           KNL_ERROR_STACK(aKnlEnv) ) == STL_SUCCESS );
    sStatus = 2;
    
    stlMemset( sFileContent, 0x00, sFileSize + 1 );
    
    sOffset = 0;
    STL_TRY( stlSeekFile( &sSrcFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK(aKnlEnv) ) == STL_SUCCESS );
    
    /** FILE Read into Buffer */
    sReadBytes = 0;
    STL_TRY( stlReadFile( &sSrcFile,
                          sFileContent,
                          sFileSize,
                          &sReadBytes,
                          KNL_ERROR_STACK(aKnlEnv) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFileSize == sReadBytes, RAMP_ERR_INVALID_FILE_PROCESS );
    
    (void)yy_scan_bytes(sFileContent, sFileSize);
    sStatus = 3;
    
    gKnpPropertyFileLineNo = 1;
            
    /**
     * Per logical line in File
     */
    while( (sToken = yylex()) )
    {
        /** Re-Initialize */
        stlMemset( sPropertyName,  0x00, STL_SIZEOF( sPropertyName ) );
        stlMemset( sPropertyValue, 0x00, STL_SIZEOF( sPropertyValue ) );
        
        sContent = NULL;
        
        if( sToken == KNP_EOL ||
            sToken == KNP_COMMENT )
        {
            continue;      
        }
        
        /** Identify KNP_NAME */
        if( sToken == KNP_NAME )
        {
            sLen = stlStrlen( yytext );
            STL_TRY( sLen <= STL_SIZEOF( sPropertyName ) );
            stlStrncpy( sPropertyName, yytext, sLen + 1 );
            
            //stlPrintf("NAME = %s[%d]\n", sPropertyName, sLen);
            
            STL_TRY( knpGetPropertyHandleByName( sPropertyName,
                                                 (void**)&sContent,
                                                 aKnlEnv ) == STL_SUCCESS );
            
            STL_TRY( sContent != NULL );
        }
        else
        {
            //주석일 경우에는 괜찮음
            if ( sToken !=  KNP_COMMENT )
            {
                STL_THROW( RAMP_ERR_FILE_PARSING_ERROR );
            }
        }
        
        /** Identify KNP_EQUALS */
        sToken = yylex();
        
        if (sToken == KNP_EQUALS)
        {
            /** Identify KNP_VALUE */
            sToken = yylex();
            
            if ( ( sToken == KNP_NAME ) ||
                 ( sToken == KNP_STRING ) ||   
                 ( sToken == KNP_UNQUOTED_STRING ) ||
                 ( sToken == KNP_INTEGER ) )
            {
                /**
                 * Property TEXT File에 READ_ONLY Property가 있을 경우에 SKIP
                 */
                STL_TRY( knpGetPropertyModifiable ( sContent,
                                                    &sModifiable,
                                                    aKnlEnv ) == STL_SUCCESS );
                
                if( sModifiable == STL_FALSE )
                {
                    continue;
                }
                
                /**
                 * get Value
                 */                 
                sLen = stlStrlen( yytext );
                STL_TRY( sLen <= KNL_PROPERTY_STRING_MAX_LENGTH );
                
                stlStrncpy( sPropertyValue, yytext, sLen + 1 );
                
                //stlPrintf("VALUE = %s[%d]\n", sPropertyValue, sLen);
                
                /** BINARY 처리 */
                STL_TRY( knpGetPropertyDataType ( sContent,
                                                  &sType,
                                                  aKnlEnv ) == STL_SUCCESS );
                switch( sType )
                {
                    case KNL_PROPERTY_TYPE_BOOL:
                    {
                        /**
                         * 0/1, ON/OFF, TRUE/FALSE, YES/NO, ENABLE/DISABLE 의 문자를 해당 값으로 변경한다.
                         */
                        
                        if( sLen == 1 )
                        {
                            if( stlStrncasecmp( sPropertyValue, "1", 1 ) == 0 )
                            {
                                sTmpValue = STL_TRUE;
                            }
                            else if( stlStrncasecmp( sPropertyValue, "0", 1 ) == 0 )
                            {
                                sTmpValue = STL_FALSE;
                            }
                            else
                            {
                                STL_THROW( RAMP_ERR_INVALID_VALUE );
                            }
                        }
                        else if( sLen == 2 )
                        {
                            if( stlStrncasecmp( sPropertyValue, "ON", 2 ) == 0 )
                            {
                                sTmpValue = STL_TRUE;
                            }
                            else if( stlStrncasecmp( sPropertyValue, "NO", 2 ) == 0 )
                            {
                                sTmpValue = STL_FALSE;
                            }
                            else
                            {
                                STL_THROW( RAMP_ERR_INVALID_VALUE );
                            }
                        }
                        else if( sLen == 3 )
                        {
                            if( stlStrncasecmp( sPropertyValue, "YES", 3 ) == 0 )
                            {
                                sTmpValue = STL_TRUE;
                            }
                            else if( stlStrncasecmp( sPropertyValue, "OFF", 3 ) == 0 )
                            {
                                sTmpValue = STL_FALSE;   
                            }
                            else
                            {
                                STL_THROW( RAMP_ERR_INVALID_VALUE );
                            }
                        }
                        else if( sLen == 4 )
                        {
                            if( stlStrncasecmp( sPropertyValue, "TRUE", 4 ) == 0 )
                            {
                                sTmpValue = STL_TRUE;
                            }
                            else
                            {
                                STL_THROW( RAMP_ERR_INVALID_VALUE );
                            }
                        }
                        else if( sLen == 5 )
                        {
                            if( stlStrncasecmp( sPropertyValue, "FALSE", 5 ) == 0 )
                            {
                                sTmpValue = STL_FALSE;
                            }
                            else
                            {
                                STL_THROW( RAMP_ERR_INVALID_VALUE );
                            }
                        }
                        else if( sLen == 6 )
                        {
                            if( stlStrncasecmp( sPropertyValue, "ENABLE", 6 ) == 0 )
                            {
                                sTmpValue = STL_TRUE;
                            }
                            else
                            {
                                STL_THROW( RAMP_ERR_INVALID_VALUE );
                            }
                        }
                        else if( sLen == 7 )
                        {
                            if( stlStrncasecmp( sPropertyValue, "DISABLE", 7 ) == 0 )
                            {
                                sTmpValue = STL_FALSE;
                            }
                            else
                            {
                                STL_THROW( RAMP_ERR_INVALID_VALUE );
                            }
                        }
                        else
                        {
                            STL_THROW( RAMP_ERR_INVALID_VALUE );
                        }                        
                        
                        sTmpBoolValue = (stlInt64)sTmpValue;
                        
                        STL_TRY( knpWritePropertyAtBinaryFile ( aBFileName,
                                                                sContent,
                                                                (void*)&sTmpBoolValue,
                                                                aKnlEnv ) == STL_SUCCESS );
                        break;
                    }
                    case KNL_PROPERTY_TYPE_BIGINT:
                    {
                        STL_TRY( knpConvertUnitToValue( sContent,
                                                        sPropertyValue,
                                                        &sTmpNumberValue,
                                                        aKnlEnv ) == STL_SUCCESS );
                        
                        
                        STL_TRY( knpWritePropertyAtBinaryFile( aBFileName,
                                                               sContent,
                                                               (void*)&sTmpNumberValue,
                                                               aKnlEnv ) == STL_SUCCESS );
                        break;
                    }
                    case KNL_PROPERTY_TYPE_VARCHAR:
                    {
                        if( sToken == KNP_STRING )
                        {
                            if( sLen >= 2 )
                            {
                                /** Single Quotation 제거 */
                                if( stlStrncmp( sPropertyValue, "'", 1 ) == 0 )
                                {
                                    stlStrncpy( sPropertyValue, &sPropertyValue[1], sLen );
                                    sLen--;
                            
                                    STL_TRY_THROW( stlStrncmp( &sPropertyValue[sLen-1], "'", 1 ) == 0, RAMP_ERR_FILE_PARSING_ERROR ); 
                            
                                    sPropertyValue[sLen-1] = '\0';
                                    sLen--;
                                }
                            }    
                        }
                        
                        STL_TRY( knpWritePropertyAtBinaryFile ( aBFileName,
                                                                sContent,
                                                                (void*)sPropertyValue,
                                                                aKnlEnv ) == STL_SUCCESS );
                        break;
                    }
                    case KNL_PROPERTY_TYPE_MAX:
                    default:
                    {
                        STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
                        break;
                    }
                }

            }
            else
            {
                STL_THROW( RAMP_ERR_FILE_PARSING_ERROR );
            }
        }
        else
        {
            STL_THROW( RAMP_ERR_FILE_PARSING_ERROR );
        }
        
    }
    
    sStatus = 2;
    (void)yylex_destroy();       /* to avoid leakage */
    
    sStatus = 1;
    (void) stlFreeHeap( sFileContent );
    
    sStatus = 0;
    STL_TRY( stlCloseFile( &sSrcFile,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    
    return STL_SUCCESS;   
    
    STL_CATCH( RAMP_ERR_INVALID_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_VALUE, 
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      sPropertyValue );
    }
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aTFileName );
    }
    STL_CATCH( RAMP_ERR_FILE_PARSING_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_FILE_PARSING_ERROR,
                      "Line No : [%d]",
                      KNL_ERROR_STACK(aKnlEnv),
                      gKnpPropertyFileLineNo );
    }
    STL_CATCH( RAMP_ERR_INVALID_FILE_PROCESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_FINISH;
    
    switch ( sStatus )
    {
        case 3:
            (void)yylex_destroy();  
        case 2:
            (void)stlFreeHeap( sFileContent );
        case 1:
            (void)stlCloseFile( &sSrcFile, KNL_ERROR_STACK( aKnlEnv ) );
        default:
            break;
    }

    return STL_FAILURE;
}

