/*******************************************************************************
 * qlpParse.c
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
 * @file qlpParse.c
 * @brief SQL Processor Layer Parser
 */

#include <qll.h>
#include <qlDef.h>
#include <qlpParse.h>


/**
 * @addtogroup qlpParser
 * @{
 */


/**
 * @brief string buffer 생성
 * @param[in]   aParam      Parse Parameter
 * @param[in]   aSize       Buffer 크기
 * 
 * @return stlChar
 */
inline stlChar * qlpMakeBuffer( stlParseParam  * aParam,
                                stlInt32         aSize )
{
    stlChar  * sStr = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            aSize,
                            (void**) & sStr,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sStr = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        stlMemset( sStr, 0x00, aSize );
    }

    return sStr;
}

    
/**
 * @brief string으로부터 integer 값 얻기
 * @param[in]   aParam      Parse Parameter
 * @param[in]   aStr        Integer String
 * 
 * @return stlInt64
 */
inline stlInt64 qlpToIntegerFromString( stlParseParam  * aParam,
                                        stlChar        * aStr )
{
    stlInt64  sInt = 0;
    
    if( dtlGetIntegerFromString( aStr,
                                 stlStrlen( aStr ),
                                 & sInt,
                                 QLL_ERROR_STACK( QLP_PARSE_GET_ENV( aParam ) ) )
        == STL_SUCCESS )
    {
        /* Do Nothing */
    }
    else
    {
        aParam->mErrStatus = STL_FAILURE;
    }

    return sInt;
}


/********************************************************************************
 * Authorization DDL
 ********************************************************************************/

/**
 * @brief user def 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aUserName       User Name
 * @param[in]   aPassword       Password
 * @param[in]   aDefaultSpace   Default Tablespace Name
 * @param[in]   aTempSpace      Temporary Tablespace Name
 * @param[in]   aSchemaName     Schema Name
 * @param[in]   aProfileName    Profile Name
 * @param[in]   aPasswordExpire PASSWORD EXPIRE 여부
 * @param[in]   aAccountLock    ACCOUNT LOCK 여부 
 *
 * @return qlpUserDef 노드 
 */
inline qlpUserDef * qlpMakeUserDef( stlParseParam      * aParam,
                                    stlInt32             aNodePos,
                                    qlpValue           * aUserName,
                                    qlpValue           * aPassword,
                                    qlpValue           * aDefaultSpace,
                                    qlpValue           * aTempSpace,
                                    qlpValue           * aSchemaName,
                                    qlpValue           * aProfileName,
                                    stlBool              aPasswordExpire,
                                    stlBool              aAccountLock )
{
    qlpUserDef  * sUserDef = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpUserDef ),
                            (void**) & sUserDef,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sUserDef = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sUserDef->mType = QLL_STMT_CREATE_USER_TYPE;
        sUserDef->mNodePos = aNodePos;
        sUserDef->mUserName = aUserName;
        sUserDef->mPassword = aPassword;
        sUserDef->mDefaultSpace = aDefaultSpace;
        sUserDef->mTempSpace = aTempSpace;
        sUserDef->mSchemaName = aSchemaName;
        sUserDef->mProfileName = aProfileName;
        sUserDef->mPasswordExpire = aPasswordExpire;
        sUserDef->mAccountLock = aAccountLock;
    }

    return sUserDef;
}


/**
 * @brief drop User 노드 생성
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aIfExists           IF EXISTS option
 * @param[in]   aUserName           User Name
 * @param[in]   aIsCascade          CASCADE  여부 
 * @return qlpDropUser node
 */
inline qlpDropUser * qlpMakeDropUser( stlParseParam    * aParam,
                                      stlInt32           aNodePos,
                                      stlBool            aIfExists,
                                      qlpValue         * aUserName,
                                      stlBool            aIsCascade )
{
    qlpDropUser  * sDropUser = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDropUser ),
                            (void**) & sDropUser,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDropUser = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDropUser->mType             = QLL_STMT_DROP_USER_TYPE;
        sDropUser->mNodePos          = aNodePos;
        sDropUser->mIfExists         = aIfExists;
        sDropUser->mName             = aUserName;
        sDropUser->mIsCascade        = aIsCascade;
    }

    return sDropUser;
}



/**
 * @brief Alter User 노드 생성
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aAlterAction        ALTER USER Action
 * @param[in]   aUserName           User Name
 * @param[in]   aNewPassword        New Password
 * @param[in]   aOrgPassword        Org Password
 * @param[in]   aSpaceName          Tablespace Name
 * @param[in]   aSchemaList         Schema Name List
 * @param[in]   aProfileName        Profile Name
 * @return qlpAlterUser node
 */
inline qlpAlterUser * qlpMakeAlterUser( stlParseParam      * aParam,
                                        stlInt32             aNodePos,
                                        qlpAlterUserAction   aAlterAction,
                                        qlpValue           * aUserName,
                                        qlpValue           * aNewPassword,
                                        qlpValue           * aOrgPassword,
                                        qlpValue           * aSpaceName,
                                        qlpList            * aSchemaList,
                                        qlpValue           * aProfileName )
{
    qlpAlterUser  * sAlterUser = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterUser ),
                            (void**) & sAlterUser,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterUser = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterUser->mType             = QLL_STMT_ALTER_USER_TYPE;
        sAlterUser->mNodePos          = aNodePos;

        sAlterUser->mAlterAction  = aAlterAction;
        sAlterUser->mUserName     = aUserName;
        
        sAlterUser->mNewPassword  = aNewPassword;
        sAlterUser->mOrgPassword  = aOrgPassword;

        sAlterUser->mSpaceName    = aSpaceName;
        sAlterUser->mSchemaList   = aSchemaList;

        sAlterUser->mProfileName  = aProfileName;
    }

    return sAlterUser;
}

/**
 * @brief 중복된 Password Parameter가 설정되어 있는지 확인  
 * @param[in]      aParam          Parse Parameter
 * @param[in]      aList           Constraints List
 * @param[in]      aParameter      Parameter
 * @return 없음
 */
inline void qlpCheckDuplicatePasswordParameter( stlParseParam        * aParam,
                                                qlpList              * aList,
                                                qlpPasswordParameter * aParameter)
{
    qlpListElement       * sListElem = NULL;
    qlpPasswordParameter * sCurParameter;

    QLP_LIST_FOR_EACH( aList, sListElem )
    {
        sCurParameter = (qlpPasswordParameter *) QLP_LIST_GET_POINTER_VALUE( sListElem );

        if( sCurParameter->mParameterType == aParameter->mParameterType )
        {
            aParam->mErrStatus = STL_FAILURE;
            
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_INVALID_PROFILE_PARAMETER,
                          qlgMakeErrPosString( aParam->mBuffer,
                                               aParameter->mNodePos,
                                               QLP_PARSE_GET_ENV( aParam ) ),
                          aParam->mErrorStack );
            
            break;
        }
        else
        {
            continue;
        }
    }

    return;
}

/**
 * @brief Password Parameter 노드 생성
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aParameterType      Parameter Type
 * @param[in]   aIsDEFAULT          Is DEFAULT
 * @param[in]   aIsUNLIMITED        Is UNLIMITED
 * @param[in]   aValue              Value
 * @param[in]   aValueStrLen        Value String Length
 * @return qlpPasswordParameter
 */
inline qlpPasswordParameter * qlpMakePasswordParameter( stlParseParam   * aParam,
                                                        stlInt32          aNodePos,
                                                        ellProfileParam   aParameterType,
                                                        stlBool           aIsDEFAULT,
                                                        stlBool           aIsUNLIMITED,
                                                        qlpValue        * aValue,
                                                        stlInt32          aValueStrLen  )
{
    qlpPasswordParameter  * sPasswordParameter = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpPasswordParameter ),
                            (void**) & sPasswordParameter,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sPasswordParameter = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sPasswordParameter->mType             = QLL_PASSWORD_PARAMETER_TYPE;
        sPasswordParameter->mNodePos          = aNodePos;
        
        sPasswordParameter->mParameterType    = aParameterType;
        
        sPasswordParameter->mIsDEFAULT        = aIsDEFAULT;
        sPasswordParameter->mIsUNLIMITED      = aIsUNLIMITED;
        sPasswordParameter->mValue            = aValue;
        sPasswordParameter->mValueStrLen      = aValueStrLen;
    }

    return sPasswordParameter;
}


/**
 * @brief profile def 노드 생성
 * @param[in]   aParam               Parse Parameter
 * @param[in]   aNodePos             Node Position
 * @param[in]   aName                Name
 * @param[in]   aPasswordParameters  Password Parameters 
 *
 * @return qlpProfileDef 노드 
 */
inline qlpProfileDef * qlpMakeProfileDef( stlParseParam        * aParam,
                                          stlInt32               aNodePos,
                                          qlpValue             * aName,
                                          qlpList              * aPasswordParameters )
{
    qlpProfileDef  * sProfileDef = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpProfileDef ),
                            (void**) & sProfileDef,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sProfileDef = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sProfileDef->mType               = QLL_STMT_CREATE_PROFILE_TYPE;
        sProfileDef->mNodePos            = aNodePos;
        sProfileDef->mProfileName        = aName;
        sProfileDef->mPasswordParameters = aPasswordParameters;
    }

    return sProfileDef;
}


/**
 * @brief drop Profile 노드 생성
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aIfExists           IF EXISTS option
 * @param[in]   aName               Profile Name
 * @param[in]   aIsCascade          CASCADE  여부 
 * @return qlpDropProfile node
 */
inline qlpDropProfile * qlpMakeDropProfile( stlParseParam    * aParam,
                                            stlInt32           aNodePos,
                                            stlBool            aIfExists,
                                            qlpValue         * aName,
                                            stlBool            aIsCascade )
{
    qlpDropProfile  * sDropProfile = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDropProfile ),
                            (void**) & sDropProfile,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDropProfile = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDropProfile->mType             = QLL_STMT_DROP_PROFILE_TYPE;
        sDropProfile->mNodePos          = aNodePos;
        sDropProfile->mIfExists         = aIfExists;
        sDropProfile->mProfileName      = aName;
        sDropProfile->mIsCascade        = aIsCascade;
    }

    return sDropProfile;
}


/**
 * @brief alter profile 노드 생성
 * @param[in]   aParam               Parse Parameter
 * @param[in]   aNodePos             Node Position
 * @param[in]   aName                Name
 * @param[in]   aPasswordParameters  Password Parameters 
 *
 * @return qlpAlterProfile 노드 
 */
inline qlpAlterProfile * qlpMakeAlterProfile( stlParseParam        * aParam,
                                              stlInt32               aNodePos,
                                              qlpValue             * aName,
                                              qlpList              * aPasswordParameters )
{
    qlpAlterProfile  * sAlterProfile = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterProfile ),
                            (void**) & sAlterProfile,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterProfile = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterProfile->mType               = QLL_STMT_ALTER_PROFILE_TYPE;
        sAlterProfile->mNodePos            = aNodePos;
        sAlterProfile->mProfileName        = aName;
        sAlterProfile->mPasswordParameters = aPasswordParameters;
    }

    return sAlterProfile;
}


/**
 * @brief Privilege Action 노드 생성
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aPrivObject         Privilege Object
 * @param[in]   aPrivAction         Privilege Action
 * @param[in]   aColumnList         Column List for Column Privilege
 * @return qlpPrivAction node
 */
inline qlpPrivAction * qlpMakePrivAction( stlParseParam    * aParam,
                                          stlInt32           aNodePos,
                                          ellPrivObject      aPrivObject,
                                          stlInt32           aPrivAction,
                                          qlpList          * aColumnList )
{
    qlpPrivAction  * sPriv = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpPrivAction ),
                            (void**) & sPriv,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sPriv = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sPriv->mType             = QLL_PRIV_ACTION_TYPE;
        sPriv->mNodePos          = aNodePos;
        sPriv->mPrivObject       = aPrivObject;
        sPriv->mPrivAction       = aPrivAction;
        sPriv->mColumnList       = aColumnList;
    }

    return sPriv;
}

/**
 * @brief Privilege Object 노드 생성
 * @param[in]   aParam                Parse Parameter
 * @param[in]   aNodePos              Node Position
 * @param[in]   aPrivObject           Privilege Object
 * @param[in]   aNonSchemaObjectName  Non-Schema Object Name  (nullable)
 * @param[in]   aSchemaObjectName     SQL-Schema Object Name  (nullable)
 * @return qlpPrivObject node
 */
inline qlpPrivObject * qlpMakePrivObject( stlParseParam    * aParam,
                                          stlInt32           aNodePos,
                                          ellPrivObject      aPrivObject,
                                          qlpValue         * aNonSchemaObjectName,
                                          qlpObjectName    * aSchemaObjectName )
{
    qlpPrivObject  * sPriv = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpPrivObject ),
                            (void**) & sPriv,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sPriv = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sPriv->mType                = QLL_PRIV_OBJECT_TYPE;
        sPriv->mNodePos             = aNodePos;
        sPriv->mPrivObjType         = aPrivObject;
        sPriv->mNonSchemaObjectName = aNonSchemaObjectName;
        sPriv->mSchemaObjectName    = aSchemaObjectName;
    }

    return sPriv;
}


/**
 * @brief Privilege 노드 생성
 * @param[in]   aParam                Parse Parameter
 * @param[in]   aNodePos              Node Position
 * @param[in]   aPrivObject           Privilege Object
 * @param[in]   aPrivActionList       Privilege Action List (nullable, ALL PRIVILEGES)
 * @return qlpPrivilege node
 */
inline qlpPrivilege * qlpMakePrivilege( stlParseParam    * aParam,
                                        stlInt32           aNodePos,
                                        qlpPrivObject    * aPrivObject,
                                        qlpList          * aPrivActionList )
{
    qlpPrivilege  * sPriv = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpPrivilege ),
                            (void**) & sPriv,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sPriv = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sPriv->mType                = QLL_PRIVILEGE_TYPE;
        sPriv->mNodePos             = aNodePos;
        sPriv->mPrivObject          = aPrivObject;
        sPriv->mPrivActionList      = aPrivActionList;
    }

    return sPriv;
}


/**
 * @brief Grante Privilege 노드 생성
 * @param[in]   aParam                Parse Parameter
 * @param[in]   aNodePos              Node Position
 * @param[in]   aPrivilege            Privilege 
 * @param[in]   aGranteeList          Grantee List
 * @param[in]   aWithGrant            WITH GRANT OPTION
 * @return qlpGrantPriv node
 */
inline qlpGrantPriv * qlpMakeGrantPriv( stlParseParam    * aParam,
                                        stlInt32           aNodePos,
                                        qlpPrivilege     * aPrivilege,
                                        qlpList          * aGranteeList,
                                        stlBool            aWithGrant )
{
    qlpGrantPriv  * sPriv = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpGrantPriv ),
                            (void**) & sPriv,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sPriv = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        switch (aPrivilege->mPrivObject->mPrivObjType)
        {
            case ELL_PRIV_DATABASE:
                sPriv->mType = QLL_STMT_GRANT_DATABASE_TYPE;
                break;
            case ELL_PRIV_SPACE:
                sPriv->mType = QLL_STMT_GRANT_TABLESPACE_TYPE;
                break;
            case ELL_PRIV_SCHEMA:
                sPriv->mType = QLL_STMT_GRANT_SCHEMA_TYPE;
                break;
            case ELL_PRIV_TABLE:
                sPriv->mType = QLL_STMT_GRANT_TABLE_TYPE;
                break;
            case ELL_PRIV_USAGE:
                sPriv->mType = QLL_STMT_GRANT_USAGE_TYPE;
                break;
            default:
                STL_ASSERT(0);
                break;
        }
        
        sPriv->mNodePos             = aNodePos;
        sPriv->mPrivilege           = aPrivilege;
        sPriv->mGranteeList         = aGranteeList;
        sPriv->mWithGrant           = aWithGrant;
    }

    return sPriv;
}



/**
 * @brief Revoke Privilege 노드 생성
 * @param[in]   aParam                Parse Parameter
 * @param[in]   aNodePos              Node Position
 * @param[in]   aPrivilege            Privilege 
 * @param[in]   aRevokeeList          Revokee List
 * @param[in]   aGrantOption          GRANT OPTION FOR
 * @param[in]   aRevokeBehavior       Revoke Behavior
 * @return qlpRevokePriv node
 */
inline qlpRevokePriv * qlpMakeRevokePriv( stlParseParam    * aParam,
                                          stlInt32           aNodePos,
                                          qlpPrivilege     * aPrivilege,
                                          qlpList          * aRevokeeList,
                                          stlBool            aGrantOption,
                                          qlpValue         * aRevokeBehavior )
{
    qlpRevokePriv  * sPriv = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpRevokePriv ),
                            (void**) & sPriv,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sPriv = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        switch (aPrivilege->mPrivObject->mPrivObjType)
        {
            case ELL_PRIV_DATABASE:
                sPriv->mType = QLL_STMT_REVOKE_DATABASE_TYPE;
                break;
            case ELL_PRIV_SPACE:
                sPriv->mType = QLL_STMT_REVOKE_TABLESPACE_TYPE;
                break;
            case ELL_PRIV_SCHEMA:
                sPriv->mType = QLL_STMT_REVOKE_SCHEMA_TYPE;
                break;
            case ELL_PRIV_TABLE:
                sPriv->mType = QLL_STMT_REVOKE_TABLE_TYPE;
                break;
            case ELL_PRIV_USAGE:
                sPriv->mType = QLL_STMT_REVOKE_USAGE_TYPE;
                break;
            default:
                STL_ASSERT(0);
                break;
        }
        
        sPriv->mNodePos             = aNodePos;
        sPriv->mPrivilege           = aPrivilege;
        sPriv->mRevokeeList         = aRevokeeList;
        sPriv->mGrantOption         = aGrantOption;
        sPriv->mRevokeBehavior      = QLP_GET_INT_VALUE( aRevokeBehavior );
    }

    return sPriv;
}


/********************************************************************************
 * Tablespace DDL
 ********************************************************************************/

/**
 * @brief tablespace def 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aNodeType       Node Type
 * @param[in]   aMediaType      Tablespace Media Type
 * @param[in]   aUsageType      Tablespace Usage Type
 * @param[in]   aName           Tablespace Name
 * @param[in]   aDataFileSpecs  Datafile Spec List
 * @param[in]   aTbsAttrs       Tablespace Attributes
 *
 * @return qlpTablespaceDef 노드 
 */
inline qlpTablespaceDef * qlpMakeTablespaceDef( stlParseParam      * aParam,
                                                stlInt32             aNodePos,
                                                qllNodeType          aNodeType,
                                                qlpSpaceMediaType    aMediaType,
                                                qlpSpaceUsageType    aUsageType,
                                                qlpValue           * aName,
                                                qlpList            * aDataFileSpecs,
                                                qlpTablespaceAttr  * aTbsAttrs )
{
    qlpTablespaceDef  * sTbs = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpTablespaceDef ),
                            (void**) & sTbs,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sTbs = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sTbs->mType          = aNodeType;
        sTbs->mNodePos       = aNodePos;
        sTbs->mMediaType     = aMediaType;
        sTbs->mUsageType     = aUsageType;
        sTbs->mName          = aName;
        sTbs->mFileSpecs     = aDataFileSpecs;
        sTbs->mTbsAttrs      = aTbsAttrs;
    }

    return sTbs;
}


/**
 * @brief drop tablespace 노드 생성
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aIfExists           IF EXISTS option
 * @param[in]   aTbsName            Tablespace Name
 * @param[in]   aIsIncluding        INCLUDING CONTENTS 여부 
 * @param[in]   aDropDatafiles      Drop Datafiles Option
 * @param[in]   aIsCascade          CASCADE CONSTRAINTS 여부 
 * @return qlpDropTablespace node
 */
inline qlpDropTablespace * qlpMakeDropTablespace( stlParseParam    * aParam,
                                                  stlInt32           aNodePos,
                                                  stlBool            aIfExists,
                                                  qlpValue         * aTbsName,
                                                  qlpValue         * aIsIncluding,
                                                  qlpValue         * aDropDatafiles,
                                                  qlpValue         * aIsCascade )
{
    qlpDropTablespace  * sDropTbs = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDropTablespace ),
                            (void**) & sDropTbs,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDropTbs = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDropTbs->mType             = QLL_STMT_DROP_TABLESPACE_TYPE;
        sDropTbs->mNodePos          = aNodePos;
        sDropTbs->mIfExists         = aIfExists;
        sDropTbs->mName             = aTbsName;
        sDropTbs->mDropDatafiles    = aDropDatafiles;
        sDropTbs->mIsIncluding      = aIsIncluding;
        sDropTbs->mIsCascade        = aIsCascade;
    }

    return sDropTbs;
}


/**
 * @brief ALTER TABLESPACE ADD FILE 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aNodeType       Node Type
 * @param[in]   aName           Tablespace Name
 * @param[in]   aUsageTypePos   Usage 유형의 Position
 * @param[in]   aMediaType      Space Media 유형 
 * @param[in]   aUsageType      Usage 유형 
 * @param[in]   aFileSpecs      File Spec List
 * 
 * @return qlpAlterSpaceAddFile 노드 
 */
inline qlpAlterSpaceAddFile * qlpMakeAlterSpaceAddFile( stlParseParam    * aParam,
                                                        stlInt32           aNodePos,
                                                        qllNodeType        aNodeType,
                                                        qlpValue         * aName,
                                                        stlInt32           aUsageTypePos,
                                                        qlpSpaceMediaType  aMediaType,
                                                        qlpSpaceUsageType  aUsageType,
                                                        qlpList          * aFileSpecs )
{
    qlpAlterSpaceAddFile  * sAlterSpace = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterSpaceAddFile),
                            (void**) & sAlterSpace,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSpace = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSpace->mType         = aNodeType;
        sAlterSpace->mNodePos      = aNodePos;
        sAlterSpace->mName         = aName;
        sAlterSpace->mUsageTypePos = aUsageTypePos;
        sAlterSpace->mMediaType    = aMediaType;
        sAlterSpace->mUsageType    = aUsageType;
        sAlterSpace->mFileSpecs    = aFileSpecs;
    }

    return sAlterSpace;
}

/**
 * @brief qlpAlterTablespaceBackup 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aNodeType       Node Type
 * @param[in]   aName           Tablespace Name
 * @param[in]   aCommand        BACKUP command
 * @param[in]   aBackupType     BACKUP type(Full, Incremental)
 * @param[in]   aOption         Incremental Backup Option(Differential, Cumulative)
 * 
 * @return qlpAlterDatabaseBackup node
 */
inline qlpAlterTablespaceBackup * qlpMakeAlterTablespaceBackup( stlParseParam  * aParam,
                                                                stlInt32         aNodePos,
                                                                qllNodeType      aNodeType,
                                                                qlpValue       * aName,
                                                                qlpValue       * aCommand,
                                                                stlUInt8         aBackupType,
                                                                qlpValue       * aOption )
{
    qlpAlterTablespaceBackup   * sAlterDbLog;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterTablespaceBackup),
                            (void**) & sAlterDbLog,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterDbLog = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterDbLog->mType       = aNodeType;
        sAlterDbLog->mNodePos    = aNodePos;
        sAlterDbLog->mName       = aName;
        sAlterDbLog->mCommand    = aCommand;
        sAlterDbLog->mBackupType = aBackupType;
        sAlterDbLog->mOption     = aOption;
    }

    return sAlterDbLog;
}

/**
 * @brief ALTER TABLESPACE DROP FILE 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aNodeType       Node Type
 * @param[in]   aName           Tablespace Name
 * @param[in]   aUsageTypePos   Usage 유형의 Position
 * @param[in]   aMediaType      Space Media 유형 
 * @param[in]   aUsageType      Usage 유형 
 * @param[in]   aFileName       File Name
 * 
 * @return qlpAlterSpaceDropFile 노드 
 */
inline qlpAlterSpaceDropFile * qlpMakeAlterSpaceDropFile( stlParseParam    * aParam,
                                                          stlInt32           aNodePos,
                                                          qllNodeType        aNodeType,
                                                          qlpValue         * aName,
                                                          stlInt32           aUsageTypePos,
                                                          qlpSpaceMediaType  aMediaType,
                                                          qlpSpaceUsageType  aUsageType,
                                                          qlpValue         * aFileName )
{
    qlpAlterSpaceDropFile  * sAlterSpace = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterSpaceDropFile),
                            (void**) & sAlterSpace,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSpace = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSpace->mType         = aNodeType;
        sAlterSpace->mNodePos      = aNodePos;
        sAlterSpace->mName         = aName;
        sAlterSpace->mUsageTypePos = aUsageTypePos;
        sAlterSpace->mMediaType    = aMediaType;
        sAlterSpace->mUsageType    = aUsageType;
        sAlterSpace->mFileName     = aFileName;
    }

    return sAlterSpace;
}


/**
 * @brief ALTER TABLESPACE ONLINE 를 위한 Parse Node 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeType        Node Type
 * @param[in]   aName            Tablespace Name
 * 
 * @return qlpAlterSpaceOnline 노드 
 */
inline qlpAlterSpaceOnline * qlpMakeAlterSpaceOnline( stlParseParam  * aParam,
                                                      stlInt32         aNodePos,
                                                      qllNodeType      aNodeType,
                                                      qlpValue       * aName )
{
    qlpAlterSpaceOnline  * sAlterSpace = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterSpaceOnline),
                            (void**) & sAlterSpace,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSpace = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSpace->mType            = aNodeType;
        sAlterSpace->mNodePos         = aNodePos;
        sAlterSpace->mName            = aName;
    }

    return sAlterSpace;
}


/**
 * @brief ALTER TABLESPACE OFFLINE 를 위한 Parse Node 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeType        Node Type
 * @param[in]   aName            Tablespace Name
 * @param[in]   aOfflineBehavior File 유형 
 * 
 * @return qlpAlterSpaceOffline 노드 
 */
inline qlpAlterSpaceOffline * qlpMakeAlterSpaceOffline( stlParseParam  * aParam,
                                                        stlInt32         aNodePos,
                                                        qllNodeType      aNodeType,
                                                        qlpValue       * aName,
                                                        qlpValue       * aOfflineBehavior )
{
    qlpAlterSpaceOffline  * sAlterSpace = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterSpaceOffline),
                            (void**) & sAlterSpace,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSpace = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSpace->mType            = aNodeType;
        sAlterSpace->mNodePos         = aNodePos;
        sAlterSpace->mName            = aName;
        sAlterSpace->mOfflineBehavior = aOfflineBehavior;
    }

    return sAlterSpace;
}

/**
 * @brief ALTER DATABASE ADD LOGFILE GROUP 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aNodeType       Node Type
 * @param[in]   aLogGroupId     생성될 Log Group의 Id
 * @param[in]   aLogfileList    Logfile list
 * @param[in]   aLogfileSize    Logfile 크기
 * @param[in]   aLogfileReuse   Logfile Reuse
 * 
 * @return qlpAlterDatabaseAddLogfile 노드
 */
inline qlpAlterDatabaseAddLogfileGroup * qlpMakeAlterDatabaseAddLogfileGroup(
    stlParseParam    * aParam,
    stlInt32           aNodePos,
    qllNodeType        aNodeType,
    qlpValue         * aLogGroupId,
    qlpList          * aLogfileList,
    qlpSize          * aLogfileSize,
    qlpValue         * aLogfileReuse )
{
    qlpAlterDatabaseAddLogfileGroup  * sAlterDbLog = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterDatabaseAddLogfileGroup),
                            (void**) & sAlterDbLog,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterDbLog = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterDbLog->mType         = aNodeType;
        sAlterDbLog->mNodePos      = aNodePos;
        sAlterDbLog->mLogGroupId   = aLogGroupId;
        sAlterDbLog->mLogfileList  = aLogfileList;
        sAlterDbLog->mLogfileSize  = aLogfileSize;
        sAlterDbLog->mLogfileReuse = aLogfileReuse;
    }

    return sAlterDbLog;
}

/**
 * @brief ALTER DATABASE DROP LOGFILE GROUP 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aNodeType       Node Type
 * @param[in]   aLogGroupId     Log Group Id to drop
 * 
 * @return qlpAlterDatabaseDropLogfile 노드
 */
inline qlpAlterDatabaseDropLogfileGroup * qlpMakeAlterDatabaseDropLogfileGroup(
    stlParseParam    * aParam,
    stlInt32           aNodePos,
    qllNodeType        aNodeType,
    qlpValue         * aLogGroupId )
{
    qlpAlterDatabaseDropLogfileGroup  * sAlterDbLog = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterDatabaseDropLogfileGroup),
                            (void**) & sAlterDbLog,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterDbLog = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterDbLog->mType         = aNodeType;
        sAlterDbLog->mNodePos      = aNodePos;
        sAlterDbLog->mLogGroupId   = aLogGroupId;
    }

    return sAlterDbLog;
}

/**
 * @brief ALTER DATABASE DROP LOGFILE MEMBER를 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aNodeType       Node Type
 * @param[in]   aMemberList    Log File List to drop
 * 
 * @return qlpAlterDatabaseDropLogfile 노드
 */
inline qlpAlterDatabaseDropLogfileMember * qlpMakeAlterDatabaseDropLogfileMember(
    stlParseParam    * aParam,
    stlInt32           aNodePos,
    qllNodeType        aNodeType,
    qlpList          * aMemberList )
{
    qlpAlterDatabaseDropLogfileMember  * sAlterDbLog = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterDatabaseDropLogfileMember),
                            (void**) & sAlterDbLog,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterDbLog = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterDbLog->mType         = aNodeType;
        sAlterDbLog->mNodePos      = aNodePos;
        sAlterDbLog->mLogFileList  = aMemberList;
    }

    return sAlterDbLog;
}

/**
 * @brief ALTER DATABASE RENAME LOGFILE 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aNodeType       Node Type
 * @param[in]   aFromFileList   From File List
 * @param[in]   aToFileList     To File List
 * 
 * @return qlpAlterDatabaseRenamLogfile 노드
 */
inline qlpAlterDatabaseRenameLogfile * qlpMakeAlterDatabaseRenameLogfile(
    stlParseParam    * aParam,
    stlInt32           aNodePos,
    qllNodeType        aNodeType,
    qlpList          * aFromFileList,
    qlpList          * aToFileList )
{
    qlpAlterDatabaseRenameLogfile  * sAlterDbLog = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterDatabaseRenameLogfile),
                            (void**) & sAlterDbLog,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterDbLog = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterDbLog->mType         = aNodeType;
        sAlterDbLog->mNodePos      = aNodePos;
        sAlterDbLog->mFromFileList = aFromFileList;
        sAlterDbLog->mToFileList   = aToFileList;
    }

    return sAlterDbLog;
}

/**
 * @brief ALTER DATABASE ADD LOGFILE MEMBER 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aNodeType       Node Type
 * @param[in]   aMemberList     Member List
 * @param[in]   aGroupId        생성될 Log Group의 Id
 * 
 * @return qlpAlterDatabaseAddLogfileMember 노드
 */
inline qlpAlterDatabaseAddLogfileMember * qlpMakeAlterDatabaseAddLogfileMember(
    stlParseParam    * aParam,
    stlInt32           aNodePos,
    qllNodeType        aNodeType,
    qlpList          * aMemberList,
    qlpValue         * aGroupId )
{
    qlpAlterDatabaseAddLogfileMember  * sAlterDbLog = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterDatabaseAddLogfileMember),
                            (void**) & sAlterDbLog,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterDbLog = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterDbLog->mType           = aNodeType;
        sAlterDbLog->mNodePos        = aNodePos;
        sAlterDbLog->mMemberList     = aMemberList;
        sAlterDbLog->mLogGroupId     = aGroupId;
    }

    return sAlterDbLog;
}

/**
 * @brief qlpAlterDatabaseArchivelogMode 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aMode            ARCHIVELOG mode
 * @return qlpAlterDatabaseArchivelogMode node
 */
inline qlpAlterDatabaseArchivelogMode * qlpMakeAlterDatabaseArchivelogMode(
    stlParseParam  * aParam,
    stlInt32         aNodePos,
    stlInt32         aMode )
{
    qlpAlterDatabaseArchivelogMode   * sAlterDbLog;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterDatabaseArchivelogMode),
                            (void**) & sAlterDbLog,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterDbLog = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        if ( aMode == SML_ARCHIVELOG_MODE )
        {
            sAlterDbLog->mType = QLL_STMT_ALTER_DATABASE_ARCHIVELOG_MODE_TYPE;
        }
        else
        {
            STL_DASSERT( aMode == SML_NOARCHIVELOG_MODE );
            sAlterDbLog->mType = QLL_STMT_ALTER_DATABASE_NOARCHIVELOG_MODE_TYPE;
        }
        
        sAlterDbLog->mNodePos = aNodePos;
        sAlterDbLog->mMode    = aMode;
    }

    return sAlterDbLog;
}

/**
 * @brief qlpAlterDatabaseBackup 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeType        Node Type
 * @param[in]   aCommand         BACKUP command
 * @param[in]   aBackupType      BACKUP type(Full, Incremental)
 * @param[in]   aOption          Incremental Backup Option(Differential, Cumulative)
 * 
 * @return qlpAlterDatabaseBackup node
 */
inline qlpAlterDatabaseBackup * qlpMakeAlterDatabaseBackup( stlParseParam  * aParam,
                                                            stlInt32         aNodePos,
                                                            qllNodeType      aNodeType,
                                                            qlpValue       * aCommand,
                                                            stlUInt8         aBackupType,
                                                            qlpValue       * aOption )
{
    qlpAlterDatabaseBackup   * sAlterDbLog;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterDatabaseBackup),
                            (void**) & sAlterDbLog,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterDbLog = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterDbLog->mType       = aNodeType;
        sAlterDbLog->mNodePos    = aNodePos;
        sAlterDbLog->mCommand    = aCommand;
        sAlterDbLog->mBackupType = aBackupType;
        sAlterDbLog->mOption     = aOption;
    }

    return sAlterDbLog;
}

/**
 * @brief qlpAlterDatabaseDeleteBackupList 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeType        Node Type
 * @param[in]   aTarget          Delete Backup List Target( ALL or OBSOLETE )
 * @param[in]   aIsIncluding     INCLUDING BACKUP FILE 여부
 * 
 * @return qlpAlterDatabaseBackup node
 */
inline qlpAlterDatabaseDeleteBackupList * qlpMakeDeleteBackupList( stlParseParam  * aParam,
                                                                   stlInt32         aNodePos,
                                                                   qllNodeType      aNodeType,
                                                                   qlpValue       * aTarget,
                                                                   stlBool          aIsIncluding )
{
    qlpAlterDatabaseDeleteBackupList   * sDeleteBackupList;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterDatabaseDeleteBackupList),
                            (void**) & sDeleteBackupList,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDeleteBackupList = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDeleteBackupList->mType       = aNodeType;
        sDeleteBackupList->mNodePos    = aNodePos;
        sDeleteBackupList->mTarget     = aTarget;
        sDeleteBackupList->mDeleteFile = aIsIncluding;
    }

    return sDeleteBackupList;
}

/**
 * @brief INCREMENTAL BACKUP OPTION 노드 생성
 * @param[in]   aParam     Parse Parameter
 * @param[in]   aLevel     Incremental Backup Option Level
 * @param[in]   aOption    Incremental Backup Option(Differential/Cumulative)
 * @return qlpIncrementalBackupOption 노드
 */
inline qlpIncrementalBackupOption * qlpMakeIncrementalBackupOption( stlParseParam  * aParam,
                                                                    qlpValue       * aLevel,
                                                                    qlpValue       * aOption )
{
    qlpIncrementalBackupOption  * sOption = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpIncrementalBackupOption ),
                            (void**) & sOption,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sOption = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sOption->mLevel  = aLevel;
        sOption->mOption = aOption;
    }

    return sOption;
}

/**
 * @brief qlpBackupCtrlfile 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aTarget          Backup target
 * 
 * @return qlpBackupCtrlfile node
 */
inline qlpBackupCtrlfile * qlpMakeBackupCtrlfile( stlParseParam  * aParam,
                                                  stlInt32         aNodePos,
                                                  qlpValue       * aTarget )
{
    qlpBackupCtrlfile   * sBackupCtrlfile;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpBackupCtrlfile),
                            (void**) & sBackupCtrlfile,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sBackupCtrlfile = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sBackupCtrlfile->mType = QLL_STMT_ALTER_DATABASE_BACKUP_CTRLFILE_TYPE;
        sBackupCtrlfile->mNodePos = aNodePos;
        sBackupCtrlfile->mTarget = aTarget;
    }

    return sBackupCtrlfile;
}

/**
 * @brief qlpRestoreCtrlfile 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aTarget          Backup target
 * 
 * @return qlpRestoreCtrlfile node
 */
inline qlpRestoreCtrlfile * qlpMakeRestoreCtrlfile( stlParseParam  * aParam,
                                                    stlInt32         aNodePos,
                                                    qlpValue       * aTarget )
{
    qlpRestoreCtrlfile   * sRestoreCtrlfile;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpRestoreCtrlfile),
                            (void**) & sRestoreCtrlfile,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sRestoreCtrlfile = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sRestoreCtrlfile->mType = QLL_STMT_ALTER_DATABASE_RESTORE_CTRLFILE_TYPE;
        sRestoreCtrlfile->mNodePos = aNodePos;
        sRestoreCtrlfile->mTarget = aTarget;
    }

    return sRestoreCtrlfile;
}

/**
 * @brief qlpAlterDatabaseIrrecoverable 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aSegmentList     Segment identifier List
 * 
 * @return qlpAlterDatabaseIrrecoverable node
 */
inline qlpAlterDatabaseIrrecoverable * qlpMakeAlterDatabaseIrrecoverable( stlParseParam  * aParam,
                                                                          stlInt32         aNodePos,
                                                                          qlpList        * aSegmentList )
{
    qlpAlterDatabaseIrrecoverable   * sAlterDb;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterDatabaseIrrecoverable),
                            (void**) & sAlterDb,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterDb = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterDb->mType = QLL_STMT_ALTER_DATABASE_IRRECOVERABLE_TYPE;
        sAlterDb->mNodePos = aNodePos;
        sAlterDb->mSegmentList = aSegmentList;
    }

    return sAlterDb;
}

/**
 * @brief Alter Database RECOVER 노드 생성
 * @param[in]   aParam     Parse Parameter
 * @param[in]   aNodePos   Node Position
 * @param[in]   aRecoveryOption 불완전 복구 Option + USING BACKUP CONTROLFILE
 * @return qlpAlterDatabaseRecover 노드
 */
inline qlpAlterDatabaseRecover * qlpMakeAlterDatabaseRecover( stlParseParam  * aParam,
                                                              stlInt32         aNodePos,
                                                              qlpValue       * aRecoveryOption )
{
    qlpAlterDatabaseRecover  * sAlterDatabaseRecover = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterDatabaseRecover ),
                            (void**) & sAlterDatabaseRecover,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterDatabaseRecover = NULL;
        aParam->mErrStatus    = STL_FAILURE;
    }
    else
    {
        sAlterDatabaseRecover->mNodePos        = aNodePos;
        sAlterDatabaseRecover->mRecoveryOption = aRecoveryOption;

        if( aRecoveryOption == NULL )
        {
            sAlterDatabaseRecover->mType = QLL_STMT_ALTER_DATABASE_RECOVER_DATABASE_TYPE;
        }
        else
        {
            sAlterDatabaseRecover->mType = QLL_STMT_ALTER_DATABASE_INCOMPLETE_RECOVERY_TYPE;
        }
    }

    return sAlterDatabaseRecover;
}

/**
 * @brief 불완전 복구 option 노드 생성
 * @param[in]   aParam     Parse Parameter
 * @param[in]   aImrOption 불완전 복구 option(BATCH, INTERACTIVE)
 * @param[in]   aImrCondition 불완전 복구 option별로 사용되는 condition
 * @param[in]   aConditionValue 불완전 복구 condition별로 사용되는 value
 * @param[in]   aBackupCtrlfile USING BACKUP CONTROLFILE
 * @return qlpAlterDatabaseRecover 노드
 */
inline qlpImrOption * qlpMakeImrOption( stlParseParam  * aParam,
                                        stlUInt8         aImrOption,
                                        stlUInt8         aImrCondition,
                                        qlpValue       * aConditionValue,
                                        stlBool          aBackupCtrlfile )
{
    qlpImrOption  * sImrOption = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpImrOption ),
                            (void**) & sImrOption,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sImrOption = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sImrOption->mImrOption      = aImrOption;
        sImrOption->mImrCondition   = aImrCondition;
        sImrOption->mConditionValue = aConditionValue;
        sImrOption->mBackupCtrlfile = aBackupCtrlfile;
    }

    return sImrOption;
}

/**
 * @brief Interactive 불완전 복구 Condition 노드 생성
 * @param[in] aParam     Parse Parameter
 * @param[in] aImrCondition Interactive Recovery Condition(BEGIN, SUGGESTION, MANUAL, AUTO, END)
 * @param[in] aLogfileName User typed logfile name
 * @return qlpInteractiveImrCondition 노드
 */
inline qlpInteractiveImrCondition * qlpMakeInteractiveImrCondition( stlParseParam  * aParam,
                                                                    stlUInt8         aImrCondition,
                                                                    qlpValue       * aLogfileName )
{
    qlpInteractiveImrCondition  * sImrCondition = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpInteractiveImrCondition ),
                            (void**) & sImrCondition,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sImrCondition = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sImrCondition->mImrCondition = aImrCondition;
        sImrCondition->mLogfileName = aLogfileName;
    }

    return sImrCondition;
}

/**
 * @brief UNTIL OPTION 노드 생성
 * @param[in]   aParam     Parse Parameter
 * @param[in]   aUntilType 불완전 복구 시 until의 type(TIME,CHANGE,CANCEL)
 * @param[in]   aUntilValue 불완전 복구 시 until의 value
 * @return qlpUntilOption 노드
 */
inline qlpRestoreOption * qlpMakeUntilOption( stlParseParam  * aParam,
                                              stlUInt8         aUntilType,
                                              qlpValue       * aUntilValue )
{
    qlpRestoreOption  * sUntilOption = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpRestoreOption ),
                            (void**) & sUntilOption,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sUntilOption = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sUntilOption->mUntilType  = aUntilType;
        sUntilOption->mUntilValue = aUntilValue;
    }

    return sUntilOption;
}

/**
 * @brief ALTER DATABASE RECOVER TABLESPACE 를 위한 Parse Node 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aName            Tablespace Name
 * 
 * @return qlpAlterDatabaseRecoverTablespace 노드 
 */
inline qlpAlterDatabaseRecoverTablespace * qlpMakeAlterDatabaseRecoverTablespace(
    stlParseParam  * aParam,
    stlInt32         aNodePos,
    qlpValue       * aName )
{
    qlpAlterDatabaseRecoverTablespace  * sAlterTbs = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterDatabaseRecoverTablespace),
                            (void**) & sAlterTbs,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterTbs = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterTbs->mType = QLL_STMT_ALTER_DATABASE_RECOVER_TABLESPACE_TYPE;
        sAlterTbs->mNodePos = aNodePos;
        sAlterTbs->mName = aName;
    }

    return sAlterTbs;
}

/**
 * @brief ALTER DATABASE RECOVER DATAFILE 를 위한 Parse Node 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aDatafileList    Recovery Target Datafile List
 * 
 * @return qlpAlterDatabaseRecoverDatafile 노드 
 */
inline qlpAlterDatabaseRecoverDatafile * qlpMakeAlterDatabaseRecoverDatafile(
    stlParseParam * aParam,
    stlInt32        aNodePos,
    qlpList       * aDatafileList )
{
    qlpAlterDatabaseRecoverDatafile * sRecoverDatafile = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterDatabaseRecoverDatafile),
                            (void**) & sRecoverDatafile,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sRecoverDatafile = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sRecoverDatafile->mType          = QLL_STMT_ALTER_DATABASE_RECOVER_DATAFILE_TYPE;
        sRecoverDatafile->mNodePos       = aNodePos;
        sRecoverDatafile->mRecoverList   = aDatafileList;
    }
    
    return sRecoverDatafile;
}

/**
 * @brief ALTER DATABASE RECOVER DATAFILE 를 위한 Parse Node 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aDatafileName    Datafile Name
 * @param[in]   aBackup          Backup Name
 * @param[in]   aForCorruption   Corruption 위한 Recovery Flag
 * 
 * @return qlpDatafileList
 */
inline qlpDatafileList * qlpMakeRecoverDatafileTarget( stlParseParam * aParam,
                                                       qlpValue      * aDatafileName,
                                                       qlpValue      * aBackup,
                                                       stlBool         aForCorruption )
{
    qlpDatafileList * sDatafileNode = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpDatafileList),
                            (void **) &sDatafileNode,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDatafileNode = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDatafileNode->mDatafileName = aDatafileName;
        sDatafileNode->mBackupName   = aBackup;
        
        if( aBackup == NULL )
        {
            sDatafileNode->mUsingBackup = STL_FALSE;
        }
        else
        {
            sDatafileNode->mUsingBackup = STL_TRUE;
        }
        
        sDatafileNode->mForCorruption = aForCorruption;
    }

    return sDatafileNode;
}

/**
 * @brief Alter Database RESTORE 노드 생성
 * @param[in]   aParam     Parse Parameter
 * @param[in]   aNodePos   Node Position
 * @param[in]   aNodeType  Node Type
 * @param[in]   aRestoreOption 불완전 복구 시 until type(TIME,CHANGE,CANCEL)과 value
 * @return qlpAlterDatabaseRestore 노드
 */
inline qlpAlterDatabaseRestore * qlpMakeAlterDatabaseRestore( stlParseParam  * aParam,
                                                              stlInt32         aNodePos,
                                                              qllNodeType      aNodeType,
                                                              qlpValue       * aRestoreOption )
{
    qlpAlterDatabaseRestore  * sAlterDatabaseRestore = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterDatabaseRestore ),
                            (void**) & sAlterDatabaseRestore,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterDatabaseRestore = NULL;
        aParam->mErrStatus    = STL_FAILURE;
    }
    else
    {
        sAlterDatabaseRestore->mType          = aNodeType;
        sAlterDatabaseRestore->mNodePos       = aNodePos;
        sAlterDatabaseRestore->mRestoreOption = aRestoreOption;
    }

    return sAlterDatabaseRestore;
}

/**
 * @brief Alter Database RESTORE 노드 생성
 * @param[in]   aParam     Parse Parameter
 * @param[in]   aNodePos   Node Position
 * @param[in]   aNodeType  Node Type
 * @param[in]   aTbsName   Tablespace Name
 * @return qlpAlterDatabaseRestore 노드
 */
inline qlpAlterDatabaseRestoreTablespace * qlpMakeAlterDatabaseRestoreTablespace(
    stlParseParam  * aParam,
    stlInt32         aNodePos,
    qllNodeType      aNodeType,
    qlpValue       * aTbsName )
{
    qlpAlterDatabaseRestoreTablespace  * sAlterTbs = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterDatabaseRestoreTablespace ),
                            (void**) & sAlterTbs,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterTbs = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterTbs->mType    = aNodeType;
        sAlterTbs->mNodePos = aNodePos;
        sAlterTbs->mTbsName = aTbsName;
    }

    return sAlterTbs;
}


/**
 * @brief Alter Database Clear Password History 노드 생성
 * @param[in]   aParam     Parse Parameter
 * @param[in]   aNodePos   Node Position
 * @return qlpAlterDatabaseClearPassHistory 노드
 */
inline qlpAlterDatabaseClearPassHistory * qlpMakeAlterDatabaseClearPassHistory( stlParseParam  * aParam,
                                                                                stlInt32         aNodePos )
{
    qlpAlterDatabaseClearPassHistory  * sAlterDatabase = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterDatabaseClearPassHistory ),
                            (void**) & sAlterDatabase,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterDatabase = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterDatabase->mType    = QLL_STMT_ALTER_DATABASE_CLEAR_PASSWORD_HISTORY_TYPE;
        sAlterDatabase->mNodePos = aNodePos;
    }

    return sAlterDatabase;
}

/**
 * @brief ALTER TABLESPACE RENAME FILE 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aNodeType       Node Type
 * @param[in]   aName           Tablespace Name
 * @param[in]   aUsageTypePos   Space Usage 유형의 Position
 * @param[in]   aMediaType      Space Media 유형 
 * @param[in]   aUsageType      Space Usage 유형 
 * @param[in]   aFromFileList   From File List
 * @param[in]   aToFileList     To File List
 * 
 * @return qlpAlterSpaceRenameFile 노드
 */
inline qlpAlterSpaceRenameFile * qlpMakeAlterSpaceRenameFile( stlParseParam    * aParam,
                                                              stlInt32           aNodePos,
                                                              qllNodeType        aNodeType,
                                                              qlpValue         * aName,
                                                              stlInt32           aUsageTypePos,
                                                              qlpSpaceMediaType  aMediaType,
                                                              qlpSpaceUsageType  aUsageType,
                                                              qlpList          * aFromFileList,
                                                              qlpList          * aToFileList )
{
    qlpAlterSpaceRenameFile  * sAlterSpace = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterSpaceRenameFile),
                            (void**) & sAlterSpace,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSpace = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSpace->mType         = aNodeType;
        sAlterSpace->mNodePos      = aNodePos;
        sAlterSpace->mName         = aName;
        sAlterSpace->mUsageTypePos = aUsageTypePos;
        sAlterSpace->mMediaType    = aMediaType;
        sAlterSpace->mUsageType    = aUsageType;
        sAlterSpace->mFromFileList = aFromFileList;
        sAlterSpace->mToFileList   = aToFileList;
    }

    return sAlterSpace;
}


/**
 * @brief ALTER TABLESPACE RENAME TO 를 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aNodeType       Node Type
 * @param[in]   aName           Tablespace Name
 * @param[in]   aNewName        New Tablespace Name
 * 
 * @return qlpAlterSpaceRename 노드 
 */
inline qlpAlterSpaceRename * qlpMakeAlterSpaceRename( stlParseParam    * aParam,
                                                      stlInt32           aNodePos,
                                                      qllNodeType        aNodeType,
                                                      qlpValue         * aName,
                                                      qlpValue         * aNewName )
{
    qlpAlterSpaceRename  * sAlterSpace = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterSpaceRename),
                            (void**) & sAlterSpace,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSpace = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSpace->mType        = aNodeType;
        sAlterSpace->mNodePos     = aNodePos;
        sAlterSpace->mName        = aName;
        sAlterSpace->mNewName     = aNewName;
    }

    return sAlterSpace;
}


/********************************************************************************
 * Schema DDL
 ********************************************************************************/


/**
 * @brief schema definition 노드  추가 
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aSchemaName     Schema Name
 * @param[in]   aOwnerName      Owner Name
 * @param[in]   aSchemaElemList Schema Element(DDL) list
 * @return qlpSchemaDef node
 */
inline qlpSchemaDef * qlpMakeSchemaDef( stlParseParam  * aParam,
                                        stlInt32         aNodePos,
                                        qlpValue       * aSchemaName,
                                        qlpValue       * aOwnerName,
                                        qlpList        * aSchemaElemList )
{
    qlpSchemaDef  * sSchemaDef = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpSchemaDef),
                            (void**) & sSchemaDef,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSchemaDef = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sSchemaDef->mType           = QLL_STMT_CREATE_SCHEMA_TYPE;
        sSchemaDef->mNodePos        = aNodePos;
        sSchemaDef->mSchemaName     = aSchemaName;
        sSchemaDef->mOwnerName      = aOwnerName;
        sSchemaDef->mSchemaElemList = aSchemaElemList;
        sSchemaDef->mRefSchemaObjectList = (qlpList *) aParam->mSchemaObjectList;
    }

    return sSchemaDef;
}


/**
 * @brief drop Schema 노드 생성
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aIfExists           IF EXISTS option
 * @param[in]   aSchemaName         Schema Name
 * @param[in]   aIsCascade          CASCADE  여부 
 * @return qlpDropSchema node
 */
inline qlpDropSchema * qlpMakeDropSchema( stlParseParam    * aParam,
                                          stlInt32           aNodePos,
                                          stlBool            aIfExists,
                                          qlpValue         * aSchemaName,
                                          stlBool            aIsCascade )
{
    qlpDropSchema  * sDropSchema = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDropSchema ),
                            (void**) & sDropSchema,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDropSchema = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDropSchema->mType             = QLL_STMT_DROP_SCHEMA_TYPE;
        sDropSchema->mNodePos          = aNodePos;
        sDropSchema->mIfExists         = aIfExists;
        sDropSchema->mName             = aSchemaName;
        sDropSchema->mIsCascade        = aIsCascade;
    }

    return sDropSchema;
}


/********************************************************************************
 * Table DDL
 ********************************************************************************/

/**
 * @brief table definition 노드  추가 
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aRelName        base table name
 * @param[in]   aRelPosition    base table name token position
 * @param[in]   aTableElems     table columns & table constraints
 * @param[in]   aRelationAttrs  relation attributes
 * @param[in]   aColumnNameList CREATE TABLE column_list AS SUBQUERY
 * @param[in]   aSubquery       CREATE TABLE AS subquery
 * @param[in]   aWithData       CREATE TABLE AS subquery with_data_option   
 * 
 * @return qlpTableDef node
 */
inline qlpTableDef * qlpMakeTableDef( stlParseParam  * aParam,
                                      stlInt32         aNodePos, 
                                      qlpList        * aRelName, 
                                      stlInt32         aRelPosition,
                                      qlpList        * aTableElems,
                                      qlpList        * aRelationAttrs,
                                      qlpList        * aColumnNameList,
                                      qlpQueryExpr   * aSubquery,
                                      stlBool          aWithData )
{
    qlpTableDef          * sTable               = NULL;

    qlpListElement       * sTableOptionListElem = NULL;
    qlpDefElem           * sTableOptionElement  = NULL;
    qllNode              * sTableOptionArgument = NULL;
    qlpObjectAttrType      sTableOptionType;

    qlpListElement       * sSegmentListElem     = NULL;
    qlpDefElem           * sSegmentElement      = NULL;
    qllNode              * sSegmentArgument     = NULL;
    qlpObjectAttrType      sSegmentType;

    if ( aParam->mAllocator( aParam->mContext,
                             STL_SIZEOF( qlpTableDef ),
                             (void**) & sTable,
                             aParam->mErrorStack )
         != STL_SUCCESS )
    {
        sTable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        if ( aSubquery == NULL )
        {
            /* CREATE TABLE */
            STL_DASSERT( aColumnNameList == NULL );
            
            sTable->mType           = QLL_STMT_CREATE_TABLE_TYPE;
            sTable->mTableElems     = aTableElems;

            sTable->mColumnNameList = NULL;
            sTable->mSubquery       = NULL;
            sTable->mWithData       = STL_FALSE;
        }
        else
        {
            /* CREATE TABLE AS SELECT */
            STL_DASSERT( aTableElems == NULL );

            sTable->mType           = QLL_STMT_CREATE_TABLE_AS_SELECT_TYPE;
            sTable->mTableElems     = NULL;
            
            sTable->mColumnNameList = aColumnNameList;
            sTable->mSubquery       = aSubquery;
            sTable->mWithData       = aWithData;
        }

        sTable->mNodePos        = aNodePos;
        sTable->mTableDefType   = QLP_TABLE_DEFINITION_TYPE_NONE;
        sTable->mRelation       = qlpMakeRelInfo( aParam,
                                                  aRelPosition,
                                                  aRelName,
                                                  NULL );
        
        /* segment attribute values 초기화 */
        sTable->mSegInitialSize = NULL;
        sTable->mSegNextSize    = NULL;
        sTable->mSegMinSize     = NULL;
        sTable->mSegMaxSize     = NULL;

        /* permanent attribute values 초기화 */
        sTable->mPermanent      = NULL;

        /* physical attribute values 초기화 */
        sTable->mPctFree        = NULL;
        sTable->mPctUsed        = NULL;
        sTable->mIniTrans       = NULL;
        sTable->mMaxTrans       = NULL;
        

        if( aRelationAttrs != NULL )
        {
            sTableOptionListElem = QLP_LIST_GET_FIRST( aRelationAttrs );
            while( ( sTableOptionListElem != NULL ) && ( aParam->mErrStatus == STL_SUCCESS ) )
            {
                sTableOptionElement     = (qlpDefElem*) QLP_LIST_GET_POINTER_VALUE( sTableOptionListElem );

                sTableOptionType        = sTableOptionElement->mElementType;
                sTableOptionArgument    = qlpGetElemArgument( aParam, sTableOptionElement );

                switch( sTableOptionType )
                {
                    case QLP_OBJECT_ATTR_SEGMENT_CLAUSE :
                        {
                            sSegmentListElem = QLP_LIST_GET_FIRST( (qlpList*)sTableOptionArgument );
                            while( ( sSegmentListElem != NULL ) && ( aParam->mErrStatus == STL_SUCCESS ) )
                            {
                                sSegmentElement    = (qlpDefElem*) QLP_LIST_GET_POINTER_VALUE( sSegmentListElem );
                                sSegmentType       = sSegmentElement->mElementType;
                                sSegmentArgument   = qlpGetElemArgument( aParam, sSegmentElement );

                                switch( sSegmentType )
                                {
                                    case QLP_OBJECT_ATTR_SEGMENT_INITIAL :
                                        {
                                            sTable->mSegInitialSize = (qlpSize*) sSegmentArgument;
                                            break;
                                        }
                                    case QLP_OBJECT_ATTR_SEGMENT_NEXT :
                                        {
                                            sTable->mSegNextSize = (qlpSize*) sSegmentArgument;
                                            break;
                                        }
                                    case QLP_OBJECT_ATTR_SEGMENT_MINSIZE :
                                        {
                                            sTable->mSegMinSize = (qlpSize*) sSegmentArgument;
                                            break;
                                        }
                                    case QLP_OBJECT_ATTR_SEGMENT_MAXSIZE :
                                        {
                                            sTable->mSegMaxSize = (qlpSize*) sSegmentArgument;
                                            break;
                                        }
                                    default :
                                        {
                                            aParam->mErrStatus = STL_FAILURE;
                                            break;
                                        }
                                }

                                sSegmentListElem = QLP_LIST_GET_NEXT_ELEM( sSegmentListElem );
                            }

                            break;
                        }
                    case QLP_OBJECT_ATTR_PHYSICAL_PCTFREE :
                        {
                            sTable->mPctFree = (qlpValue*) sTableOptionArgument;
                            break;
                        }
                    case QLP_OBJECT_ATTR_PHYSICAL_PCTUSED :
                        {
                            sTable->mPctUsed = (qlpValue*) sTableOptionArgument;
                            break;
                        }
                    case QLP_OBJECT_ATTR_PHYSICAL_INITRANS :
                        {
                            sTable->mIniTrans = (qlpValue*) sTableOptionArgument;
                            break;
                        }
                    case QLP_OBJECT_ATTR_PHYSICAL_MAXTRANS :
                        {
                            sTable->mMaxTrans = (qlpValue*) sTableOptionArgument;
                            break;
                        }
                    case QLP_OBJECT_ATTR_TABLESPACE :
                        {
                            sTable->mTablespaceName = (qlpValue*) sTableOptionArgument;
                            break;
                        }
                    case QLP_OBJECT_ATTR_COLUMNAR_OPTION :
                        {
                            sTable->mTableDefType = QLP_GET_INT_VALUE(sTableOptionArgument);
                            break;
                        }
                    default :
                        {
                            aParam->mErrStatus = STL_FAILURE;
                            break;
                        }
                }

                sTableOptionListElem = QLP_LIST_GET_NEXT_ELEM( sTableOptionListElem );
            }
        }
        else
        {
            /* Do Nothing */
        }
    }

    return sTable;
}


/**
 * @brief drop table 노드 생성
 * @param[in]   aParam        Parse Parameter
 * @param[in]   aNodePos      Node Position
 * @param[in]   aIfExists     IF EXISTS option
 * @param[in]   aName         Table Name
 * @param[in]   aIsCascade    CASCADE CONSTRAINTS 여부 
 * 
 * @return qlpDropTable node
 */
inline qlpDropTable * qlpMakeDropTable( stlParseParam    * aParam,
                                        stlInt32           aNodePos,
                                        stlBool            aIfExists,
                                        qlpList          * aName,
                                        stlBool            aIsCascade )
{
    qlpDropTable    * sDropTable = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDropTable ),
                            (void**) & sDropTable,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDropTable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDropTable->mType        = QLL_STMT_DROP_TABLE_TYPE;
        sDropTable->mNodePos     = aNodePos;
        sDropTable->mIfExists    = aIfExists;
        sDropTable->mName        = qlpMakeObjectName( aParam, aName );
        sDropTable->mIsCascade   = aIsCascade;
    }

    return sDropTable;
}


/**
 * @brief truncate table 노드 생성
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aName               Table Name
 * @param[in]   aIdentityRestart    Identity Restart Option
 * @param[in]   aDropStorage        Drop Storage Option
 * @return qlpTruncateTable node
 * @remarks
 */
inline qlpTruncateTable * qlpMakeTruncateTable( stlParseParam         * aParam,
                                                stlInt32                aNodePos,
                                                qlpList               * aName,
                                                qlpValue              * aIdentityRestart,
                                                qlpValue              * aDropStorage )
{
    qlpTruncateTable  * sTruncate = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpTruncateTable ),
                            (void**) & sTruncate,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sTruncate = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sTruncate->mType               = QLL_STMT_TRUNCATE_TABLE_TYPE;
        sTruncate->mNodePos            = aNodePos;
        sTruncate->mName               = qlpMakeObjectName( aParam, aName );
        sTruncate->mIdentityRestart    = aIdentityRestart;
        sTruncate->mDropStorage        = aDropStorage;
    }

    return sTruncate;
}



/**
 * @brief ALTER TABLE .. ADD COLUMN .. 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aTableName      Table Name
 * @param[in]   aColDefList     Column Definition List
 * @return qlpAlterTableAddColumn 노드
 * @remarks
 */
inline qlpAlterTableAddColumn * qlpMakeAlterTableAddColumn( stlParseParam * aParam,
                                                            stlInt32        aNodePos,
                                                            qlpList       * aTableName,
                                                            qlpList       * aColDefList )
{
    qlpAlterTableAddColumn * sAlterTable    = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterTableAddColumn ),
                            (void**) & sAlterTable,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterTable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterTable->mType          = QLL_STMT_ALTER_TABLE_ADD_COLUMN_TYPE;
        sAlterTable->mNodePos       = aNodePos;

        sAlterTable->mTableName     = qlpMakeObjectName( aParam, aTableName );
        sAlterTable->mColumnDefList = aColDefList;
    }

    return sAlterTable;
}



/**
 * @brief ALTER TABLE .. DROP COLUMN .. 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aTableName      Table Name
 * @param[in]   aDropActionPos  Drop Action Position
 * @param[in]   aDropAction     Drop Action
 * @param[in]   aColNameList    Column Name List
 * @param[in]   aIsCascade      CASCADE CONSTRAINTS
 * @return qlpAlterTableDropColumn 노드
 * @remarks
 */
inline qlpAlterTableDropColumn * qlpMakeAlterTableDropColumn( stlParseParam       * aParam,
                                                              stlInt32              aNodePos,
                                                              qlpList             * aTableName,
                                                              stlInt32              aDropActionPos,
                                                              qlpDropColumnAction   aDropAction,
                                                              qlpList             * aColNameList,
                                                              stlBool               aIsCascade )
{
    qlpAlterTableDropColumn * sAlterTable    = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterTableDropColumn ),
                            (void**) & sAlterTable,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterTable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        switch ( aDropAction )
        {
            case QLP_DROP_COLUMN_ACTION_SET_UNUSED_COLUMN:
                sAlterTable->mType = QLL_STMT_ALTER_TABLE_SET_UNUSED_COLUMN_TYPE;
                break;
            case QLP_DROP_COLUMN_ACTION_DROP_COLUMN:
                sAlterTable->mType = QLL_STMT_ALTER_TABLE_DROP_COLUMN_TYPE;
                break;
            case QLP_DROP_COLUMN_ACTION_DROP_UNUSED_COLUMNS:
                sAlterTable->mType = QLL_STMT_ALTER_TABLE_DROP_UNUSED_COLUMNS_TYPE;
                STL_ASSERT( aColNameList == NULL );
                break;
            default:
                STL_ASSERT(0);
                break;
        }
        
        sAlterTable->mNodePos        = aNodePos;

        sAlterTable->mTableName      = qlpMakeObjectName( aParam, aTableName );
        sAlterTable->mDropActionPos  = aDropActionPos;
        sAlterTable->mDropAction     = aDropAction;
        sAlterTable->mColumnNameList = aColNameList;
        sAlterTable->mIsCascade      = aIsCascade;
    }

    return sAlterTable;
}



/**
 * @brief ALTER Identity Spec 을 위한 Parse Node 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aIdentType       Identity Type
 * @param[in]   aIdentSeqOptions Identity Options
 * @return qlpAlterIdentity 노드
 * @remarks
 */
inline qlpAlterIdentity * qlpMakeAlterIdentitySpec( stlParseParam       * aParam,
                                                    qlpIdentityType       aIdentType,
                                                    qlpSequenceOption   * aIdentSeqOptions )
{
    qlpAlterIdentity * sAlterIdentity    = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterIdentity ),
                            (void**) & sAlterIdentity,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterIdentity = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterIdentity->mIdentType = aIdentType;
        sAlterIdentity->mIdentSeqOptions = aIdentSeqOptions;
    }

    return sAlterIdentity;
}


/**
 * @brief ALTER TABLE .. ALTER COLUMN .. 을 위한 Parse Node 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aTableName       Table Name
 * @param[in]   aColumnName      Column Name
 * @param[in]   aAlterActionPos  Alter Action Position
 * @param[in]   aAlterAction     Alter Action
 * @param[in]   aNotNullName     Not Null Constraint Name
 * @param[in]   aNotNullConstAttr  Not Null Constraint Attribute
 * @param[in]   aAlterOption     Alter Action Option
 * @return qlpAlterTableAlterColumn 노드
 * @remarks
 */
inline qlpAlterTableAlterColumn * qlpMakeAlterTableAlterColumn( stlParseParam       * aParam,
                                                                stlInt32              aNodePos,
                                                                qlpList             * aTableName,
                                                                qlpValue            * aColumnName,
                                                                stlInt32              aAlterActionPos,
                                                                qlpAlterColumnAction  aAlterAction,
                                                                qlpList             * aNotNullName,
                                                                qlpConstraintAttr   * aNotNullConstAttr,
                                                                qllNode             * aAlterOption )
{
    qlpAlterTableAlterColumn * sAlterTable    = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterTableAlterColumn ),
                            (void**) & sAlterTable,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterTable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        switch ( aAlterAction )
        {
            case QLP_ALTER_COLUMN_SET_DEFAULT:
                sAlterTable->mType = QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_DEFAULT_TYPE;
                break;
            case QLP_ALTER_COLUMN_DROP_DEFAULT:
                sAlterTable->mType = QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_DEFAULT_TYPE;
                break;
            case QLP_ALTER_COLUMN_SET_NOT_NULL:
                sAlterTable->mType = QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_NOT_NULL_TYPE;
                break;
            case QLP_ALTER_COLUMN_DROP_NOT_NULL:
                sAlterTable->mType = QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_NOT_NULL_TYPE;
                break;
            case QLP_ALTER_COLUMN_SET_DATA_TYPE:
                sAlterTable->mType = QLL_STMT_ALTER_TABLE_ALTER_COLUMN_SET_DATA_TYPE_TYPE;
                break;
            case QLP_ALTER_COLUMN_ALTER_IDENTITY_SPEC:
                sAlterTable->mType = QLL_STMT_ALTER_TABLE_ALTER_COLUMN_ALTER_IDENTITY_TYPE;
                break;
            case QLP_ALTER_COLUMN_DROP_IDENTITY:
                sAlterTable->mType = QLL_STMT_ALTER_TABLE_ALTER_COLUMN_DROP_IDENTITY_TYPE;
                break;
            default:
                STL_ASSERT(0);
                break;
        }
        
        sAlterTable->mNodePos           = aNodePos;

        sAlterTable->mTableName         = qlpMakeObjectName( aParam, aTableName );
        sAlterTable->mColumnName        = aColumnName;
        sAlterTable->mAlterActionPos    = aAlterActionPos;
        sAlterTable->mAlterColumnAction = aAlterAction;

        switch ( aAlterAction )
        {
            case QLP_ALTER_COLUMN_SET_DEFAULT:
                sAlterTable->mNotNullName  = NULL;
                sAlterTable->mNotNullConstAttr = NULL;
                sAlterTable->mDefault      = aAlterOption;
                sAlterTable->mDataType     = NULL;
                sAlterTable->mIdentitySpec = NULL;
                break;
            case QLP_ALTER_COLUMN_DROP_DEFAULT:
                sAlterTable->mNotNullName  = NULL;
                sAlterTable->mNotNullConstAttr = NULL;
                sAlterTable->mDefault      = NULL;
                sAlterTable->mDataType     = NULL;
                sAlterTable->mIdentitySpec = NULL;
                break;
            case QLP_ALTER_COLUMN_SET_NOT_NULL:
                if ( aNotNullName == NULL )
                {
                    sAlterTable->mNotNullName  = NULL;
                }
                else
                {
                    sAlterTable->mNotNullName  = qlpMakeObjectName( aParam, aNotNullName );
                }
                sAlterTable->mNotNullConstAttr = aNotNullConstAttr;
                sAlterTable->mDefault      = NULL;
                sAlterTable->mDataType     = NULL;
                sAlterTable->mIdentitySpec = NULL;
                break;
            case QLP_ALTER_COLUMN_DROP_NOT_NULL:
                sAlterTable->mNotNullName  = NULL;
                sAlterTable->mNotNullConstAttr = NULL;
                sAlterTable->mDefault      = NULL;
                sAlterTable->mDataType     = NULL;
                sAlterTable->mIdentitySpec = NULL;
                break;
            case QLP_ALTER_COLUMN_SET_DATA_TYPE:
                sAlterTable->mNotNullName  = NULL;
                sAlterTable->mNotNullConstAttr = NULL;
                sAlterTable->mDefault      = NULL;
                sAlterTable->mDataType     = aAlterOption;
                sAlterTable->mIdentitySpec = NULL;
                break;
            case QLP_ALTER_COLUMN_ALTER_IDENTITY_SPEC:
                sAlterTable->mNotNullName  = NULL;
                sAlterTable->mNotNullConstAttr = NULL;
                sAlterTable->mDefault      = NULL;
                sAlterTable->mDataType     = NULL;
                sAlterTable->mIdentitySpec = aAlterOption;
                break;
            case QLP_ALTER_COLUMN_DROP_IDENTITY:
                sAlterTable->mNotNullName  = NULL;
                sAlterTable->mNotNullConstAttr = NULL;
                sAlterTable->mDefault      = NULL;
                sAlterTable->mDataType     = NULL;
                sAlterTable->mIdentitySpec = NULL;
                break;
            default:
                STL_ASSERT(0);
                break;
        }
    }

    return sAlterTable;
}



/**
 * @brief ALTER TABLE .. DROP CONSTRAINT .. 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aTableName      Table Name
 * @param[in]   aConstObject    Constraint Object
 * @param[in]   aKeepIndex      KEEP INDEX or DROP INDEX
 * @param[in]   aIsCascade      CASCADE CONSTRAINTS
 * @return qlpAlterTableDropConstraint 노드
 * @remarks
 */
inline qlpAlterTableDropConstraint * qlpMakeAlterTableDropConstraint( stlParseParam      * aParam,
                                                                      stlInt32             aNodePos,
                                                                      qlpList            * aTableName,
                                                                      qlpDropConstObject * aConstObject,
                                                                      stlBool              aKeepIndex,
                                                                      stlBool              aIsCascade )
{
    qlpAlterTableDropConstraint * sAlterTable    = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterTableDropConstraint ),
                            (void**) & sAlterTable,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterTable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterTable->mType          = QLL_STMT_ALTER_TABLE_DROP_CONSTRAINT_TYPE;
        sAlterTable->mNodePos       = aNodePos;

        sAlterTable->mTableName     = qlpMakeObjectName( aParam, aTableName );

        sAlterTable->mConstObject = aConstObject;
        sAlterTable->mKeepIndex   = aKeepIndex;
        sAlterTable->mIsCascade   = aIsCascade;
    }

    return sAlterTable;
}



/**
 * @brief ALTER TABLE .. ALTER CONSTRAINT .. 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aTableName      Table Name
 * @param[in]   aConstObject    Constraint Object
 * @param[in]   aConstAttr      Constraint Attribute
 * @return qlpAlterTableAlterConstraint 노드
 * @remarks
 */
inline qlpAlterTableAlterConstraint * qlpMakeAlterTableAlterConstraint( stlParseParam      * aParam,
                                                                        stlInt32             aNodePos,
                                                                        qlpList            * aTableName,
                                                                        qlpDropConstObject * aConstObject,
                                                                        qlpConstraintAttr  * aConstAttr )
{
    qlpAlterTableAlterConstraint * sAlterTable    = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterTableAlterConstraint ),
                            (void**) & sAlterTable,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterTable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterTable->mType          = QLL_STMT_ALTER_TABLE_ALTER_CONSTRAINT_TYPE;
        sAlterTable->mNodePos       = aNodePos;

        sAlterTable->mTableName     = qlpMakeObjectName( aParam, aTableName );

        sAlterTable->mConstObject = aConstObject;
        sAlterTable->mConstAttr   = aConstAttr;
    }

    return sAlterTable;
}


/**
 * @brief ALTER TABLE .. ADD CONSTRAINT .. 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aTableName      Table Name
 * @param[in]   aConstDef       Constraint Definition
 * @return qlpAlterTableAddConstraint 노드
 * @remarks
 */
inline qlpAlterTableAddConstraint * qlpMakeAlterTableAddConstraint( stlParseParam      * aParam,
                                                                    stlInt32             aNodePos,
                                                                    qlpList            * aTableName,
                                                                    qlpList            * aConstDef )
{
    qlpAlterTableAddConstraint * sAlterTable    = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterTableAddConstraint ),
                            (void**) & sAlterTable,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterTable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterTable->mType          = QLL_STMT_ALTER_TABLE_ADD_CONSTRAINT_TYPE;
        sAlterTable->mNodePos       = aNodePos;

        sAlterTable->mTableName     = qlpMakeObjectName( aParam, aTableName );

        sAlterTable->mConstDefinition = aConstDef;
    }

    return sAlterTable;
}


inline qlpDropConstObject * qlpMakeDropConstObject( stlParseParam    * aParam,
                                                    stlInt32           aNodePos,
                                                    qlpDropConstType   aDropConstType,
                                                    qlpList          * aConstraintName,
                                                    qlpList          * aUniqueColumnList )
{
    qlpDropConstObject * sDropObject = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDropConstObject ),
                            (void**) & sDropObject,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDropObject = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDropObject->mType          = QLL_DROP_CONSTRAINT_OBJECT_TYPE;
        sDropObject->mNodePos       = aNodePos;

        sDropObject->mDropConstType = aDropConstType;

        switch( sDropObject->mDropConstType )
        {
            case QLP_DROP_CONST_NAME:
                sDropObject->mConstraintName = qlpMakeObjectName( aParam, aConstraintName );
                sDropObject->mUniqueColumnList = NULL;
                break;
            case QLP_DROP_CONST_PRIMARY_KEY:
                sDropObject->mConstraintName   = NULL;
                sDropObject->mUniqueColumnList = NULL;
                break;
            case QLP_DROP_CONST_UNIQUE:
                sDropObject->mConstraintName   = NULL;
                sDropObject->mUniqueColumnList = aUniqueColumnList;
                break;
            default:
                STL_ASSERT(0);
                break;
        }
    }

    return sDropObject;
}
    

/**
 * @brief ALTER TABLE .. STORAGE .. 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aTableName      Table Name
 * @param[in]   aPhysicalAttrs  relation attributes
 * @return qlpAlterTablePhysicalAttr 노드
 * @remarks
 */
inline qlpAlterTablePhysicalAttr * qlpMakeAlterTablePhysicalAttr( stlParseParam * aParam,
                                                                  stlInt32        aNodePos,
                                                                  qlpList       * aTableName,
                                                                  qlpList       * aPhysicalAttrs )
{
    qlpAlterTablePhysicalAttr * sAlterTable    = NULL;
    qlpListElement            * sTableOptionListElem = NULL;
    qlpDefElem                * sTableOptionElement  = NULL;
    qllNode                   * sTableOptionArgument = NULL;
    qlpObjectAttrType           sTableOptionType;

    qlpListElement            * sSegmentListElem     = NULL;
    qlpDefElem                * sSegmentElement      = NULL;
    qllNode                   * sSegmentArgument     = NULL;
    qlpObjectAttrType           sSegmentType;  

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterTablePhysicalAttr ),
                            (void**) & sAlterTable,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterTable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterTable->mType          = QLL_STMT_ALTER_TABLE_PHYSICAL_ATTR_TYPE;
        sAlterTable->mNodePos       = aNodePos;

        sAlterTable->mTableName     = qlpMakeObjectName( aParam, aTableName );

        /* segment attribute values 초기화 */
        sAlterTable->mSegInitialSize = NULL;
        sAlterTable->mSegNextSize    = NULL;
        sAlterTable->mSegMinSize     = NULL;
        sAlterTable->mSegMaxSize     = NULL;

        /* physical attribute values 초기화 */
        sAlterTable->mPctFree        = NULL;
        sAlterTable->mPctUsed        = NULL;
        sAlterTable->mIniTrans       = NULL;
        sAlterTable->mMaxTrans       = NULL;

        if( aPhysicalAttrs != NULL )
        {
            sTableOptionListElem = QLP_LIST_GET_FIRST( aPhysicalAttrs );
            while( ( sTableOptionListElem != NULL ) && ( aParam->mErrStatus == STL_SUCCESS ) )
            {
                sTableOptionElement     = (qlpDefElem*) QLP_LIST_GET_POINTER_VALUE( sTableOptionListElem );

                sTableOptionType        = sTableOptionElement->mElementType;
                sTableOptionArgument    = qlpGetElemArgument( aParam, sTableOptionElement );

                switch( sTableOptionType )
                {
                    case QLP_OBJECT_ATTR_SEGMENT_CLAUSE :
                        {
                            sSegmentListElem = QLP_LIST_GET_FIRST( (qlpList*)sTableOptionArgument );
                            while( ( sSegmentListElem != NULL ) && ( aParam->mErrStatus == STL_SUCCESS ) )
                            {
                                sSegmentElement    = (qlpDefElem*) QLP_LIST_GET_POINTER_VALUE( sSegmentListElem );
                                sSegmentType       = sSegmentElement->mElementType;
                                sSegmentArgument   = qlpGetElemArgument( aParam, sSegmentElement );

                                switch( sSegmentType )
                                {
                                    case QLP_OBJECT_ATTR_SEGMENT_INITIAL :
                                        {
                                            sAlterTable->mSegInitialSize = (qlpSize*) sSegmentArgument;
                                            break;
                                        }
                                    case QLP_OBJECT_ATTR_SEGMENT_NEXT :
                                        {
                                            sAlterTable->mSegNextSize = (qlpSize*) sSegmentArgument;
                                            break;
                                        }
                                    case QLP_OBJECT_ATTR_SEGMENT_MINSIZE :
                                        {
                                            sAlterTable->mSegMinSize = (qlpSize*) sSegmentArgument;
                                            break;
                                        }
                                    case QLP_OBJECT_ATTR_SEGMENT_MAXSIZE :
                                        {
                                            sAlterTable->mSegMaxSize = (qlpSize*) sSegmentArgument;
                                            break;
                                        }
                                    default :
                                        {
                                            aParam->mErrStatus = STL_FAILURE;
                                            break;
                                        }
                                }

                                sSegmentListElem = QLP_LIST_GET_NEXT_ELEM( sSegmentListElem );
                            }

                            break;
                        }
                    case QLP_OBJECT_ATTR_PHYSICAL_PCTFREE :
                        {
                            sAlterTable->mPctFree = (qlpValue*) sTableOptionArgument;
                            break;
                        }
                    case QLP_OBJECT_ATTR_PHYSICAL_PCTUSED :
                        {
                            sAlterTable->mPctUsed = (qlpValue*) sTableOptionArgument;
                            break;
                        }
                    case QLP_OBJECT_ATTR_PHYSICAL_INITRANS :
                        {
                            sAlterTable->mIniTrans = (qlpValue*) sTableOptionArgument;
                            break;
                        }
                    case QLP_OBJECT_ATTR_PHYSICAL_MAXTRANS :
                        {
                            sAlterTable->mMaxTrans = (qlpValue*) sTableOptionArgument;
                            break;
                        }
                    case QLP_OBJECT_ATTR_TABLESPACE :
                        {
                            STL_DASSERT( STL_FALSE );
                            break;
                        }
                    default :
                        {
                            aParam->mErrStatus = STL_FAILURE;
                            break;
                        }
                }

                sTableOptionListElem = QLP_LIST_GET_NEXT_ELEM( sTableOptionListElem );
            }
        }
        else
        {
            /* Do Nothing */
        }
    }

    return sAlterTable;
}


/**
 * @brief ALTER TABLE .. RENAME COLUMN .. 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aTableName      Table Name
 * @param[in]   aOrgName        Original Column Name
 * @param[in]   aNewName        New Column Name
 * @return qlpAlterTableRenameColumn 노드
 * @remarks
 */
inline qlpAlterTableRenameColumn * qlpMakeAlterTableRenameColumn( stlParseParam * aParam,
                                                                  stlInt32        aNodePos,
                                                                  qlpList       * aTableName,
                                                                  qlpValue      * aOrgName,
                                                                  qlpValue      * aNewName )
{
    qlpAlterTableRenameColumn * sAlterTable    = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterTableRenameColumn ),
                            (void**) & sAlterTable,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterTable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterTable->mType          = QLL_STMT_ALTER_TABLE_RENAME_COLUMN_TYPE;
        sAlterTable->mNodePos       = aNodePos;

        sAlterTable->mTableName     = qlpMakeObjectName( aParam, aTableName );
        sAlterTable->mOrgColName    = aOrgName;
        sAlterTable->mNewColName    = aNewName;
    }

    return sAlterTable;
}



/**
 * @brief ALTER TABLE .. RENAME TO .. 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aTableName      Original Table Name
 * @param[in]   aNewName        New Table Name
 * @return qlpAlterTableRenameTable 노드
 * @remarks
 */
inline qlpAlterTableRenameTable * qlpMakeAlterTableRenameTable( stlParseParam * aParam,
                                                                stlInt32        aNodePos,
                                                                qlpList       * aTableName,
                                                                qlpValue      * aNewName )
{
    qlpAlterTableRenameTable * sAlterTable    = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterTableRenameTable ),
                            (void**) & sAlterTable,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterTable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterTable->mType           = QLL_STMT_ALTER_TABLE_RENAME_TABLE_TYPE;
        sAlterTable->mNodePos        = aNodePos;

        sAlterTable->mOrgName      = qlpMakeObjectName( aParam, aTableName );
        sAlterTable->mNewName      = aNewName;
    }

    return sAlterTable;
}


/**
 * @brief ALTER TABLE .. ADD/DROP SUPPLEMENTAL LOG DATA 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aTableName      Table Name
 * @param[in]   aIsAdd          TRUE(ADD), FALSE(DROP)
 * @return qlpAlterTableSupplementLog 노드
 * @remarks
 */
inline qlpAlterTableSuppLog * qlpMakeAlterTableSuppLog( stlParseParam * aParam,
                                                        stlInt32        aNodePos,
                                                        qlpList       * aTableName,
                                                        stlBool         aIsAdd )
{
    qlpAlterTableSuppLog * sAlterTable    = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterTableSuppLog ),
                            (void**) & sAlterTable,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterTable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        if ( aIsAdd == STL_TRUE )
        {
            sAlterTable->mType = QLL_STMT_ALTER_TABLE_ADD_SUPPLOG_TYPE;
        }
        else
        {
            sAlterTable->mType = QLL_STMT_ALTER_TABLE_DROP_SUPPLOG_TYPE;
        }
        sAlterTable->mNodePos   = aNodePos;

        sAlterTable->mTableName = qlpMakeObjectName( aParam, aTableName );
    }

    return sAlterTable;
    
}

inline void qlpSetIndexAttr( stlParseParam  * aParam,
                             qlpList        * aAttrList,
                             qlpIndexAttr   * aIndexAttr )
{
    qlpListElement       * sIndexOptionListElem = NULL;
    qlpDefElem           * sIndexOptionElement  = NULL;
    qllNode              * sIndexOptionArgument = NULL;
    qlpObjectAttrType      sIndexOptionType;

    qlpListElement       * sSegmentListElem     = NULL;
    qlpDefElem           * sSegmentElement      = NULL;
    qllNode              * sSegmentArgument     = NULL;
    qlpObjectAttrType      sSegmentType;  

    if( aAttrList == NULL )
    {
        /* segment attribute values 초기화 */
        aIndexAttr->mSegInitialSize      = NULL;
        aIndexAttr->mSegNextSize         = NULL;
        aIndexAttr->mSegMinSize          = NULL;
        aIndexAttr->mSegMaxSize          = NULL;

        /* physical attribute values 초기화 */
        aIndexAttr->mIniTrans            = NULL;
        aIndexAttr->mMaxTrans            = NULL;

        /* index attribute values 초기화 */
        aIndexAttr->mLogging             = NULL;
        aIndexAttr->mParallel            = NULL;
    }
    else
    {
        sIndexOptionListElem = QLP_LIST_GET_FIRST( aAttrList );
        while( ( sIndexOptionListElem != NULL ) && ( aParam->mErrStatus == STL_SUCCESS ) )
        {
            sIndexOptionElement     = (qlpDefElem*) QLP_LIST_GET_POINTER_VALUE( sIndexOptionListElem );

            sIndexOptionType        = sIndexOptionElement->mElementType;
            sIndexOptionArgument    = qlpGetElemArgument( aParam, sIndexOptionElement );

            switch( sIndexOptionType )
            {
                case QLP_OBJECT_ATTR_SEGMENT_CLAUSE :
                    {
                        sSegmentListElem = QLP_LIST_GET_FIRST( (qlpList*)sIndexOptionArgument );
                        while( ( sSegmentListElem != NULL ) && ( aParam->mErrStatus == STL_SUCCESS ) )
                        {
                            sSegmentElement    = (qlpDefElem*) QLP_LIST_GET_POINTER_VALUE( sSegmentListElem );
                            sSegmentType       = sSegmentElement->mElementType;
                            sSegmentArgument   = qlpGetElemArgument( aParam, sSegmentElement );

                            switch( sSegmentType )
                            {
                                case QLP_OBJECT_ATTR_SEGMENT_INITIAL :
                                    {
                                        aIndexAttr->mSegInitialSize = (qlpSize*) sSegmentArgument;
                                        break;
                                    }
                                case QLP_OBJECT_ATTR_SEGMENT_NEXT :
                                    {
                                        aIndexAttr->mSegNextSize = (qlpSize*) sSegmentArgument;
                                        break;
                                    }
                                case QLP_OBJECT_ATTR_SEGMENT_MINSIZE :
                                    {
                                        aIndexAttr->mSegMinSize = (qlpSize*) sSegmentArgument;
                                        break;
                                    }
                                case QLP_OBJECT_ATTR_SEGMENT_MAXSIZE :
                                    {
                                        aIndexAttr->mSegMaxSize = (qlpSize*) sSegmentArgument;
                                        break;
                                    }
                                default :
                                    {
                                        aParam->mErrStatus = STL_FAILURE;
                                        break;
                                    }
                            }

                            sSegmentListElem = QLP_LIST_GET_NEXT_ELEM( sSegmentListElem );
                        }

                        break;
                    }
                case QLP_OBJECT_ATTR_PHYSICAL_INITRANS :
                    {
                        aIndexAttr->mIniTrans = (qlpValue*) sIndexOptionArgument;
                        break;
                    }
                case QLP_OBJECT_ATTR_PHYSICAL_MAXTRANS :
                    {
                        aIndexAttr->mMaxTrans = (qlpValue*) sIndexOptionArgument;
                        break;
                    }
                case QLP_OBJECT_ATTR_PHYSICAL_PCTFREE :
                    {
                        aIndexAttr->mPctFree = (qlpValue*) sIndexOptionArgument;
                        break;
                    }
                case  QLP_OBJECT_ATTR_LOGGING:
                    {
                        aIndexAttr->mLogging = (qlpValue *) sIndexOptionArgument;
                        break;
                    }
                case  QLP_OBJECT_ATTR_PARALLEL:
                    {
                        aIndexAttr->mParallel = (qlpValue *) sIndexOptionArgument;
                        break;
                    }
                default :
                    {
                        aParam->mErrStatus = STL_FAILURE;
                        break;
                    }
            }

            sIndexOptionListElem = QLP_LIST_GET_NEXT_ELEM( sIndexOptionListElem );
        }
    }
}
                      
/**
 * @brief index definition 노드  추가 
 * @param[in]   aParam            Parse Parameter
 * @param[in]   aNodePos          Node Position
 * @param[in]   aIsUnique         unique index 
 * @param[in]   aIsUniquePos      unique index position
 * @param[in]   aIsPrimary        primary index
 * @param[in]   aIsFromConstr     index from constraint
 * @param[in]   aIndexName        index name
 * @param[in]   aRelName          base table name
 * @param[in]   aRelPosition      base table name position
 * @param[in]   aElems            index columns
 * @param[in]   aIndexAttrs       idnex attributes
 * @param[in]   aTbsName          tablespace name
 * 
 * @return qlpIndexDef node
 */
inline qlpIndexDef * qlpMakeIndexDef( stlParseParam  * aParam,
                                      stlInt32         aNodePos,
                                      stlBool          aIsUnique,
                                      stlInt32         aIsUniquePos,
                                      stlBool          aIsPrimary,
                                      stlBool          aIsFromConstr,
                                      qlpList        * aIndexName,
                                      qlpList        * aRelName,
                                      stlInt32         aRelPosition,
                                      qlpList        * aElems,
                                      qlpList        * aIndexAttrs,
                                      qlpValue       * aTbsName )
{
    qlpIndexDef          * sIndex               = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpIndexDef ),
                            (void**) & sIndex,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sIndex = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sIndex->mType                = QLL_STMT_CREATE_INDEX_TYPE;
        sIndex->mNodePos             = aNodePos;
        sIndex->mIsUnique            = aIsUnique;
        sIndex->mIsUniquePos         = aIsUniquePos;
        sIndex->mIsPrimary           = aIsPrimary;
        sIndex->mIsFromConstraint    = aIsFromConstr;
        sIndex->mName                = qlpMakeObjectName( aParam, aIndexName );
        sIndex->mRelInfo             = qlpMakeRelInfo( aParam,
                                                       aRelPosition,
                                                       aRelName,
                                                       NULL );
        sIndex->mElems               = aElems;

        qlpSetIndexAttr( aParam, aIndexAttrs, & sIndex->mIndexAttr );
        
        sIndex->mTablespaceName      = aTbsName;
    }

    return sIndex;
}


/**
 * @brief drop index 노드 생성
 * @param[in]   aParam        Parse Parameter
 * @param[in]   aNodePos      Node Position
 * @param[in]   aIfExists     IF EXISTS option
 * @param[in]   aName         Index Name
 * 
 * @return qlpDropIndex node
 */
inline qlpDropIndex * qlpMakeDropIndex( stlParseParam    * aParam,
                                        stlInt32           aNodePos,
                                        stlBool            aIfExists,
                                        qlpList          * aName )
{
    qlpDropIndex    * sDropIndex = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDropIndex ),
                            (void**) & sDropIndex,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDropIndex = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDropIndex->mType        = QLL_STMT_DROP_INDEX_TYPE;
        sDropIndex->mNodePos     = aNodePos;
        sDropIndex->mIfExists    = aIfExists;
        sDropIndex->mName        = qlpMakeObjectName( aParam, aName ); 
    }

    return sDropIndex;
}



/**
 * @brief ALTER INDEX .. STORAGE .. 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aIndexName      Index Name
 * @param[in]   aPhysicalAttrs  relation attributes
 * @return qlpAlterIndexPhysicalAttr 노드
 * @remarks
 */
inline qlpAlterIndexPhysicalAttr * qlpMakeAlterIndexPhysicalAttr( stlParseParam * aParam,
                                                                  stlInt32        aNodePos,
                                                                  qlpList       * aIndexName,
                                                                  qlpList       * aPhysicalAttrs )
{
    qlpAlterIndexPhysicalAttr * sAlterIndex    = NULL;
    qlpListElement            * sIndexOptionListElem = NULL;
    qlpDefElem                * sIndexOptionElement  = NULL;
    qllNode                   * sIndexOptionArgument = NULL;
    qlpObjectAttrType           sIndexOptionType;

    qlpListElement            * sSegmentListElem     = NULL;
    qlpDefElem                * sSegmentElement      = NULL;
    qllNode                   * sSegmentArgument     = NULL;
    qlpObjectAttrType           sSegmentType;  

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterIndexPhysicalAttr ),
                            (void**) & sAlterIndex,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterIndex = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterIndex->mType          = QLL_STMT_ALTER_INDEX_PHYSICAL_ATTR_TYPE;
        sAlterIndex->mNodePos       = aNodePos;

        sAlterIndex->mIndexName     = qlpMakeObjectName( aParam, aIndexName );

        /* segment attribute values 초기화 */
        sAlterIndex->mSegInitialSize = NULL;
        sAlterIndex->mSegNextSize    = NULL;
        sAlterIndex->mSegMinSize     = NULL;
        sAlterIndex->mSegMaxSize     = NULL;

        /* physical attribute values 초기화 */
        sAlterIndex->mPctFree        = NULL;
        sAlterIndex->mIniTrans       = NULL;
        sAlterIndex->mMaxTrans       = NULL;

        if( aPhysicalAttrs != NULL )
        {
            sIndexOptionListElem = QLP_LIST_GET_FIRST( aPhysicalAttrs );
            while( ( sIndexOptionListElem != NULL ) && ( aParam->mErrStatus == STL_SUCCESS ) )
            {
                sIndexOptionElement     = (qlpDefElem*) QLP_LIST_GET_POINTER_VALUE( sIndexOptionListElem );

                sIndexOptionType        = sIndexOptionElement->mElementType;
                sIndexOptionArgument    = qlpGetElemArgument( aParam, sIndexOptionElement );

                switch( sIndexOptionType )
                {
                    case QLP_OBJECT_ATTR_SEGMENT_CLAUSE :
                        {
                            sSegmentListElem = QLP_LIST_GET_FIRST( (qlpList*)sIndexOptionArgument );
                            while( ( sSegmentListElem != NULL ) && ( aParam->mErrStatus == STL_SUCCESS ) )
                            {
                                sSegmentElement    = (qlpDefElem*) QLP_LIST_GET_POINTER_VALUE( sSegmentListElem );
                                sSegmentType       = sSegmentElement->mElementType;
                                sSegmentArgument   = qlpGetElemArgument( aParam, sSegmentElement );

                                switch( sSegmentType )
                                {
                                    case QLP_OBJECT_ATTR_SEGMENT_INITIAL :
                                        {
                                            sAlterIndex->mSegInitialSize = (qlpSize*) sSegmentArgument;
                                            break;
                                        }
                                    case QLP_OBJECT_ATTR_SEGMENT_NEXT :
                                        {
                                            sAlterIndex->mSegNextSize = (qlpSize*) sSegmentArgument;
                                            break;
                                        }
                                    case QLP_OBJECT_ATTR_SEGMENT_MINSIZE :
                                        {
                                            sAlterIndex->mSegMinSize = (qlpSize*) sSegmentArgument;
                                            break;
                                        }
                                    case QLP_OBJECT_ATTR_SEGMENT_MAXSIZE :
                                        {
                                            sAlterIndex->mSegMaxSize = (qlpSize*) sSegmentArgument;
                                            break;
                                        }
                                    default :
                                        {
                                            aParam->mErrStatus = STL_FAILURE;
                                            break;
                                        }
                                }

                                sSegmentListElem = QLP_LIST_GET_NEXT_ELEM( sSegmentListElem );
                            }

                            break;
                        }
                    case QLP_OBJECT_ATTR_PHYSICAL_PCTFREE :
                        {
                            sAlterIndex->mPctFree = (qlpValue*) sIndexOptionArgument;
                            break;
                        }
                    case QLP_OBJECT_ATTR_PHYSICAL_INITRANS :
                        {
                            sAlterIndex->mIniTrans = (qlpValue*) sIndexOptionArgument;
                            break;
                        }
                    case QLP_OBJECT_ATTR_PHYSICAL_MAXTRANS :
                        {
                            sAlterIndex->mMaxTrans = (qlpValue*) sIndexOptionArgument;
                            break;
                        }
                    case QLP_OBJECT_ATTR_PHYSICAL_PCTUSED :
                    case QLP_OBJECT_ATTR_TABLESPACE :
                        {
                            STL_DASSERT( STL_FALSE );
                            break;
                        }
                    default :
                        {
                            aParam->mErrStatus = STL_FAILURE;
                            break;
                        }
                }

                sIndexOptionListElem = QLP_LIST_GET_NEXT_ELEM( sIndexOptionListElem );
            }
        }
        else
        {
            /* Do Nothing */
        }
    }

    return sAlterIndex;
}



/**
 * @brief view definition 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aOrReplace       OR REPLACE 여부
 * @param[in]   aForce           FORCE or NO FORCE
 * @param[in]   aViewName        View Name
 * @param[in]   aColumnList      aColumnList
 * @param[in]   aViewQuery       View Query
 * @param[in]   aViewColStart    View Columns String 의 시작 위치
 * @param[in]   aViewColLen      View Columns String 의 길이
 * @param[in]   aViewQueryStart  View Query String 의 시작 위치
 * @param[in]   aViewQueryLen    View Query String 의 길이 
 * @return qlpViewDef node
 */
inline qlpViewDef * qlpMakeViewDef( stlParseParam      * aParam,
                                    stlInt32             aNodePos,
                                    stlBool              aOrReplace,
                                    stlBool              aForce,
                                    qlpList            * aViewName,
                                    qlpList            * aColumnList,
                                    qlpQueryExpr       * aViewQuery,
                                    stlInt32             aViewColStart,
                                    stlInt32             aViewColLen,
                                    stlInt32             aViewQueryStart,
                                    stlInt32             aViewQueryLen )
{
    qlpViewDef  * sView = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpViewDef ),
                            (void**) & sView,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sView = NULL;
        aParam->mErrStatus = STL_FAILURE;
        STL_THROW( RAMP_FINISH );
    }
    
    if( aParam->mAllocator( aParam->mContext,
                            aViewColLen + 1,
                            (void**) & sView->mViewColumnString,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sView = NULL;
        aParam->mErrStatus = STL_FAILURE;
        STL_THROW( RAMP_FINISH );
    }
    
    if( aParam->mAllocator( aParam->mContext,
                            aViewQueryLen + 1,
                            (void**) & sView->mViewQueryString,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sView = NULL;
        aParam->mErrStatus = STL_FAILURE;
        STL_THROW( RAMP_FINISH );
    }
        
    sView->mType        = QLL_STMT_CREATE_VIEW_TYPE;
    sView->mNodePos     = aNodePos;
    sView->mOrReplace   = aOrReplace;
    sView->mIsForce     = aForce;
    sView->mViewName    = qlpMakeObjectName( aParam, aViewName );
    sView->mColumnList  = aColumnList;
    sView->mViewQuery   = aViewQuery;

    stlMemcpy( sView->mViewColumnString,
               aParam->mBuffer + aViewColStart,
               aViewColLen );
    sView->mViewColumnString[aViewColLen] = '\0';

    stlMemcpy( sView->mViewQueryString,
               aParam->mBuffer + aViewQueryStart,
               aViewQueryLen );
    sView->mViewQueryString[aViewQueryLen] = '\0';

    STL_RAMP( RAMP_FINISH );
    
    return sView;
}



/**
 * @brief drop view 노드 생성
 * @param[in]   aParam        Parse Parameter
 * @param[in]   aNodePos      Node Position
 * @param[in]   aIfExists     IF EXISTS option
 * @param[in]   aViewName     View Name
 * 
 * @return qlpDropView node
 */
inline qlpDropView * qlpMakeDropView( stlParseParam    * aParam,
                                      stlInt32           aNodePos,
                                      stlBool            aIfExists,
                                      qlpList          * aViewName )
{
    qlpDropView    * sDropView = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDropView ),
                            (void**) & sDropView,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDropView = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDropView->mType        = QLL_STMT_DROP_VIEW_TYPE;
        sDropView->mNodePos     = aNodePos;
        sDropView->mIfExists    = aIfExists;
        sDropView->mViewName    = qlpMakeObjectName( aParam, aViewName );
    }

    return sDropView;
}


/**
 * @brief alter view 노드 생성
 * @param[in]   aParam        Parse Parameter
 * @param[in]   aNodePos      Node Position
 * @param[in]   aViewName     View Name
 * @param[in]   aAlterAction  Alter View Action
 * 
 * @return qlpDropView node
 */
inline qlpAlterView * qlpMakeAlterView( stlParseParam    * aParam,
                                        stlInt32           aNodePos,
                                        qlpList          * aViewName,
                                        qlpValue         * aAlterAction )
{
    qlpAlterView    * sAlterView = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterView ),
                            (void**) & sAlterView,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterView = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterView->mType        = QLL_STMT_ALTER_VIEW_TYPE;
        sAlterView->mNodePos     = aNodePos;
        sAlterView->mViewName    = qlpMakeObjectName( aParam, aViewName );
        sAlterView->mAlterAction = QLP_GET_INT_VALUE( aAlterAction );
    }

    return sAlterView;
}


/**
 * @brief sequence definition 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aName            Sequence Name
 * @param[in]   aIdentSeqOptions Sequence Option
 * 
 * @return qlpSequenceDef node
 */
inline qlpSequenceDef * qlpMakeSequenceDef( stlParseParam      * aParam,
                                            stlInt32             aNodePos,
                                            qlpList            * aName,
                                            qlpSequenceOption  * aIdentSeqOptions )
{
    qlpSequenceDef  * sSequence = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpSequenceDef ),
                            (void**) & sSequence,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSequence = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sSequence->mType    = QLL_STMT_CREATE_SEQUENCE_TYPE;
        sSequence->mNodePos = aNodePos;
        sSequence->mName    = qlpMakeObjectName( aParam, aName );
        sSequence->mOptions = aIdentSeqOptions;
    }

    return sSequence;
}


/**
 * @brief drop sequence 노드 생성
 * @param[in]   aParam        Parse Parameter
 * @param[in]   aNodePos      Node Position
 * @param[in]   aIfExists     IF EXISTS option
 * @param[in]   aName         Sequence Name
 * @return qlpDropSequence node
 */
inline qlpDropSequence * qlpMakeDropSequence( stlParseParam    * aParam,
                                              stlInt32           aNodePos,
                                              stlBool            aIfExists,
                                              qlpList          * aName )
{
    qlpDropSequence    * sDropSequence = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDropSequence ),
                            (void**) & sDropSequence,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDropSequence = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDropSequence->mType        = QLL_STMT_DROP_SEQUENCE_TYPE;
        sDropSequence->mNodePos     = aNodePos;
        sDropSequence->mIfExists    = aIfExists;
        sDropSequence->mName        = qlpMakeObjectName( aParam, aName );
    }

    return sDropSequence;
}



/**
 * @brief ALTER SEQUENCE 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aName            Sequence Name
 * @param[in]   aIdentSeqOptions Sequence Option
 * 
 * @return qlpAlterSequence node
 */
inline qlpAlterSequence * qlpMakeAlterSequence( stlParseParam      * aParam,
                                                stlInt32             aNodePos,
                                                qlpList            * aName,
                                                qlpSequenceOption  * aIdentSeqOptions )
{
    qlpAlterSequence * sAlterSeq = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterSequence ),
                            (void**) & sAlterSeq,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSeq = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSeq->mType    = QLL_STMT_ALTER_SEQUENCE_TYPE;
        sAlterSeq->mNodePos = aNodePos;
        sAlterSeq->mName    = qlpMakeObjectName( aParam, aName );
        sAlterSeq->mOptions = aIdentSeqOptions;
    }

    return sAlterSeq;
}



/**
 * @brief synonym definition 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aOrReplace       OR REPLACE 여부
 * @param[in]   aIsPublic        PUBLIC 여부 
 * @param[in]   aSynonymName     Synonym Name
 * @param[in]   aObjectName      Object Name
 * 
 * @return qlpSynonymDef node
 */
inline qlpSynonymDef * qlpMakeSynonymDef( stlParseParam      * aParam,
                                          stlInt32             aNodePos,
                                          stlBool              aOrReplace,
                                          stlBool              aIsPublic,  
                                          qlpList            * aSynonymName,
                                          qlpList            * aObjectName )
{
    qlpSynonymDef  * sSynonym = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpSynonymDef ),
                            (void**) & sSynonym,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSynonym = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sSynonym->mType           = QLL_STMT_CREATE_SYNONYM_TYPE;
        sSynonym->mNodePos        = aNodePos;
        sSynonym->mOrReplace      = aOrReplace;
        sSynonym->mIsPublic       = aIsPublic;
        sSynonym->mSynonymName    = qlpMakeObjectName( aParam, aSynonymName );
        sSynonym->mObjectName     = qlpMakeObjectName( aParam, aObjectName ); 
    }

    return sSynonym;
}



/**
 * @brief drop synonym 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aIsPublic        PUBLIC 여부
 * @param[in]   aIfExists        IF EXISTS option
 * @param[in]   aSynonymName     Synonym Name
 *
 * @return qlpDropSynonym node
 */
inline qlpDropSynonym * qlpMakeDropSynonym( stlParseParam      * aParam,
                                            stlInt32             aNodePos,
                                            stlBool              aIsPublic,
                                            stlBool              aIfExists,
                                            qlpList            * aSynonymName )
{
    qlpDropSynonym    * sDropSynonym = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDropSynonym ),
                            (void**) & sDropSynonym,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDropSynonym = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDropSynonym->mType        = QLL_STMT_DROP_SYNONYM_TYPE;
        sDropSynonym->mNodePos     = aNodePos;
        sDropSynonym->mIsPublic    = aIsPublic;
        sDropSynonym->mIfExists    = aIfExists;
        sDropSynonym->mSynonymName = qlpMakeObjectName( aParam, aSynonymName );
    }

    return sDropSynonym;
}



/**
 * @brief SELECT 노드 생성
 * @param[in]   aParam         Parse Parameter
 * @param[in]   aNodePos       Node Position
 * @param[in]   aQueryNode     Query Node
 * @param[in]   aUpdatability  FOR UPDATE
 *
 * @return qlpSelect 노드 
 */
inline qlpSelect * qlpMakeSelect( stlParseParam      * aParam,
                                  stlInt32             aNodePos,
                                  qllNode            * aQueryNode,
                                  qlpUpdatableClause * aUpdatability )
{
    qlpSelect     * sSelect = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpSelect ),
                            (void**) & sSelect,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSelect = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        if ( aUpdatability == NULL )
        {
            sSelect->mType = QLL_STMT_SELECT_TYPE;
        }
        else
        {
            if ( aUpdatability->mUpdatableMode == QLP_UPDATABILITY_FOR_UPDATE )
            {
                sSelect->mType = QLL_STMT_SELECT_FOR_UPDATE_TYPE;
            }
            else
            {
                sSelect->mType = QLL_STMT_SELECT_TYPE;
            }
        }
        
        sSelect->mNodePos       = aNodePos;

        sSelect->mHasHintErr    = aParam->mHasHintErr;

        sSelect->mQueryNode     = aQueryNode;
        sSelect->mInto          = NULL;
        sSelect->mUpdatability  = aUpdatability;
    }

    return sSelect;
}


/**
 * @brief FOR UPDATE 노드 생성
 * @param[in]   aParam         Parse Parameter
 * @param[in]   aNodePos       Node Position
 * @param[in]   aUpdateMode    FOR READ ONLY or FOR UPDATE
 * @param[in]   aUpdateColList FOR UPDATE OF column list
 * @param[in]   aLockWait      Lock Wait Mode
 * @return qlpUpdatableClause 노드 
 */
inline qlpUpdatableClause * qlpMakeUpdatableClause( stlParseParam        * aParam,
                                                    stlInt32               aNodePos,
                                                    qlpUpdatableMode       aUpdateMode,
                                                    qlpList              * aUpdateColList,
                                                    qlpForUpdateLockWait * aLockWait )
{
    qlpUpdatableClause * sUpdatable = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpUpdatableClause ),
                            (void**) & sUpdatable,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sUpdatable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sUpdatable->mType             = QLL_UPDATABILITY_TYPE;
        sUpdatable->mNodePos          = aNodePos;

        sUpdatable->mUpdatableMode    = aUpdateMode;
        sUpdatable->mUpdateColumnList = aUpdateColList;
        sUpdatable->mLockWait         = aLockWait;
    }

    return sUpdatable;
}

/**
 * @brief FOR UPDATE 노드 생성
 * @param[in]   aParam         Parse Parameter
 * @param[in]   aNodePos       Node Position
 * @param[in]   aLockWaitMode  Lock Wait Mode
 * @param[in]   aWaitSecond    WAIT second
 * @return qlpForUpdateLockWait 노드 
 */
inline qlpForUpdateLockWait * qlpMakeForUpdateLockWait( stlParseParam            * aParam,
                                                        stlInt32                   aNodePos,
                                                        qlpForUpdateLockWaitMode   aLockWaitMode,
                                                        qlpValue                 * aWaitSecond )
{
    qlpForUpdateLockWait * sLockWait = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpForUpdateLockWait ),
                            (void**) & sLockWait,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sLockWait = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sLockWait->mType             = QLL_FOR_UPDATE_LOCK_WAIT_TYPE;
        sLockWait->mNodePos          = aNodePos;

        sLockWait->mLockWaitMode     = aLockWaitMode;
        sLockWait->mWaitSecond       = aWaitSecond;
    }

    return sLockWait;
}


/**
 * @brief SELECT INTO 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aQueryNode      Query Node
 * @param[in]   aInto           Into Clause
 * @param[in]   aUpdatability   For Update
 *
 * @return qlpSelect 노드 
 */
inline qlpSelect * qlpMakeSelectInto( stlParseParam      * aParam,
                                      stlInt32             aNodePos,
                                      qllNode            * aQueryNode,
                                      qlpIntoClause      * aInto,
                                      qlpUpdatableClause * aUpdatability )
{
    qlpSelect   * sSelectInto = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpSelect ),
                            (void**) & sSelectInto,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSelectInto = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        if ( aUpdatability == NULL )
        {
            sSelectInto->mType          = QLL_STMT_SELECT_INTO_TYPE;
        }
        else
        {
            sSelectInto->mType          = QLL_STMT_SELECT_INTO_FOR_UPDATE_TYPE;
        }
        
        sSelectInto->mNodePos       = aNodePos;

        sSelectInto->mHasHintErr    = aParam->mHasHintErr;

        sSelectInto->mQueryNode     = aQueryNode;
        sSelectInto->mInto          = aInto;
        sSelectInto->mUpdatability  = aUpdatability;
    }

    return sSelectInto;
}


/**
 * @brief select구문의 into clause 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aParameters      Parameter List
 * 
 * @return qlpIntoClause node
 */
inline qlpIntoClause * qlpMakeIntoClause( stlParseParam  * aParam,
                                          stlInt32         aNodePos,
                                          qlpList        * aParameters )
{
    qlpIntoClause  * sIntoClause = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpIntoClause ),
                            (void**) & sIntoClause,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sIntoClause = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sIntoClause->mType       = QLL_INTO_CLAUSE_TYPE;
        sIntoClause->mNodePos    = aNodePos;
        sIntoClause->mParameters = aParameters;
    }

    return sIntoClause;
}

/**
 * @brief INSERT or DELETE 구문의 RETURNING INTO 구문을 위한 parse node 생성
 * @param[in]  aParam          Parse Parameter
 * @param[in]  aNodePos        Node Position
 * @param[in]  aReturnTarget   RETURNING target
 * @param[in]  aIntoTarget     INTO target
 * @remarks
 */
inline qlpInsDelReturn * qlpMakeInsDelReturn( stlParseParam  * aParam,
                                              stlInt32         aNodePos,
                                              qlpList        * aReturnTarget,
                                              qlpIntoClause  * aIntoTarget )
{
    qlpInsDelReturn * sInsDelReturn = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpInsDelReturn ),
                            (void**) & sInsDelReturn,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sInsDelReturn = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sInsDelReturn->mType         = QLL_INSDEL_RETURN_CLAUSE_TYPE;
        sInsDelReturn->mNodePos      = aNodePos;
        sInsDelReturn->mReturnTarget = aReturnTarget;
        sInsDelReturn->mIntoTarget   = aIntoTarget;
    }

    return sInsDelReturn;
}



/**
 * @brief UPDATE 구문의 RETURNING INTO 구문을 위한 parse node 생성
 * @param[in]  aParam          Parse Parameter
 * @param[in]  aNodePos        Node Position
 * @param[in]  aIsReturnNew    RETURNING NEW 여부 
 * @param[in]  aReturnTarget   RETURNING target
 * @param[in]  aIntoTarget     INTO target
 * @remarks
 */
inline qlpUpdateReturn * qlpMakeUpdateReturn( stlParseParam  * aParam,
                                              stlInt32         aNodePos,
                                              stlBool          aIsReturnNew,
                                              qlpList        * aReturnTarget,
                                              qlpIntoClause  * aIntoTarget )
{
    qlpUpdateReturn * sUpdateReturn = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpUpdateReturn ),
                            (void**) & sUpdateReturn,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sUpdateReturn = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sUpdateReturn->mType         = QLL_UPDATE_RETURN_CLAUSE_TYPE;
        sUpdateReturn->mNodePos      = aNodePos;
        sUpdateReturn->mIsReturnNew  = aIsReturnNew;
        sUpdateReturn->mReturnTarget = aReturnTarget;
        sUpdateReturn->mIntoTarget   = aIntoTarget;
    }

    return sUpdateReturn;
}

/**
 * @brief insert 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aRelation        Relation Info
 * @param[in]   aSource          Insert Values
 * @param[in]   aInsertReturn    RETURNING 정보 
 * @return qlpInsert node
 */
inline qlpInsert * qlpMakeInsert( stlParseParam    * aParam,
                                  stlInt32           aNodePos,
                                  qlpRelInfo       * aRelation,
                                  qlpInsertSource  * aSource,
                                  qlpInsDelReturn  * aInsertReturn )
{
    qlpInsert  * sInsert = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpInsert ),
                            (void**) & sInsert,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sInsert = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        if ( aSource->mSubquery == NULL )
        {
            sInsert->mType           = QLL_STMT_INSERT_TYPE;
            sInsert->mBaseInsertType = QLL_STMT_INSERT_TYPE;
        }
        else
        {
            sInsert->mType           = QLL_STMT_INSERT_SELECT_TYPE;
            sInsert->mBaseInsertType = QLL_STMT_INSERT_SELECT_TYPE;
        }

        sInsert->mHasHintErr = aParam->mHasHintErr;

        sInsert->mNodePos  = aNodePos;
        sInsert->mRelation = aRelation;
        sInsert->mSource   = aSource;
        
        /**
         * RETURNING 절이 존재한다면
         */
        
        if ( aInsertReturn == NULL )
        {
            sInsert->mReturnTarget = NULL;
            sInsert->mIntoTarget = NULL;
        }
        else
        {
            if ( aInsertReturn->mIntoTarget == NULL )
            {
                sInsert->mType = QLL_STMT_INSERT_RETURNING_QUERY_TYPE;
            }
            else
            {
                sInsert->mType = QLL_STMT_INSERT_RETURNING_INTO_TYPE;
            }

            sInsert->mReturnTarget = aInsertReturn->mReturnTarget;
            sInsert->mIntoTarget   = aInsertReturn->mIntoTarget;
        }
    }

    return sInsert;
}


/**
 * @brief delete 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aRelation        Relation Info
 * @param[in]   aHints           Hint List
 * @param[in]   aWhere           Where Clause
 * @param[in]   aResultSkip      Result Skip
 * @param[in]   aResultLimit     Result Limit
 * @param[in]   aDeleteReturn    DELETE RETURNING parse tree
 * @return qlpDelete node
 */
inline qlpDelete * qlpMakeDelete( stlParseParam    * aParam,
                                  stlInt32           aNodePos,
                                  qlpRelInfo       * aRelation,
                                  qlpList          * aHints,
                                  qllNode          * aWhere,
                                  qllNode          * aResultSkip,
                                  qllNode          * aResultLimit,
                                  qlpInsDelReturn  * aDeleteReturn )
{
    qlpDelete  * sDelete = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDelete ),
                            (void**) & sDelete,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDelete = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        if ( aDeleteReturn == NULL )
        {
            sDelete->mType = QLL_STMT_DELETE_TYPE;
        }
        else
        {
            if ( aDeleteReturn->mIntoTarget == NULL )
            {
                sDelete->mType = QLL_STMT_DELETE_RETURNING_QUERY_TYPE;
            }
            else
            {
                sDelete->mType = QLL_STMT_DELETE_RETURNING_INTO_TYPE;
            }
        }
        
        sDelete->mNodePos     = aNodePos;
        sDelete->mHasHintErr  = aParam->mHasHintErr;
        sDelete->mRelation    = aRelation;
        sDelete->mHints       = aHints;
        sDelete->mWhere       = aWhere;
        sDelete->mResultSkip  = aResultSkip;

        if( aResultLimit != NULL )
        {
            if( aResultLimit->mType == QLL_POINTER_LIST_TYPE )
            {
                STL_TRY_THROW( aResultSkip == NULL,
                               RAMP_ERR_MULTIPLE_OFFSET_CLAUSE );

                sDelete->mResultSkip =
                    (qllNode *)(((qlpList *) aResultLimit)->mHead->mData.mPtrValue );
                
                sDelete->mResultLimit =
                    (qllNode *)(((qlpList *) aResultLimit)->mTail->mData.mPtrValue );
            }
            else
            {
                sDelete->mResultLimit = aResultLimit;
            }
        }

        if ( aDeleteReturn == NULL )
        {
            sDelete->mReturnTarget = NULL;
            sDelete->mIntoTarget   = NULL;
        }
        else
        {
            sDelete->mReturnTarget = aDeleteReturn->mReturnTarget;
            sDelete->mIntoTarget   = aDeleteReturn->mIntoTarget;
        }

        sDelete->mCursorName = NULL;
    }

    return sDelete;

    STL_CATCH( RAMP_ERR_MULTIPLE_OFFSET_CLAUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MULTIPLE_OFFSET_CLAUSE,
                      qlgMakeErrPosString( aParam->mBuffer,
                                           aResultSkip->mNodePos,
                                           QLP_PARSE_GET_ENV( aParam ) ),
                      aParam->mErrorStack );
    }

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;

    return sDelete;
}


/**
 * @brief positioned delete 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aHints           Hint Clause
 * @param[in]   aRelation        Relation Info
 * @param[in]   aCursorName      Cursor Name 
 * @return qlpDelete node
 */
inline qlpDelete * qlpMakeDeletePositionedCursor( stlParseParam    * aParam,
                                                  stlInt32           aNodePos,
                                                  qlpList          * aHints,
                                                  qlpRelInfo       * aRelation,
                                                  qlpValue         * aCursorName )
{
    qlpDelete  * sDelete = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDelete ),
                            (void**) & sDelete,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDelete = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDelete->mType = QLL_STMT_DELETE_WHERE_CURRENT_OF_TYPE;
        
        sDelete->mNodePos     = aNodePos;
        sDelete->mHasHintErr  = aParam->mHasHintErr;
        sDelete->mRelation    = aRelation;
        sDelete->mHints       = aHints;
        sDelete->mWhere       = NULL;
        sDelete->mResultSkip  = NULL;
        sDelete->mResultLimit = NULL;

        sDelete->mReturnTarget = NULL;
        sDelete->mIntoTarget   = NULL;

        sDelete->mCursorName   = aCursorName;
    }

    return sDelete;
}


/**
 * @brief update 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aRelation        Relation Info
 * @param[in]   aHints           Hint List
 * @param[in]   aSet             Set Clause List
 * @param[in]   aWhere           Where Clause
 * @param[in]   aResultSkip      Result Skip
 * @param[in]   aResultLimit     Result Limit
 * @param[in]   aUpdateReturn    RETURNING 절 
 * @return qlpUpdate node
 */
inline qlpUpdate * qlpMakeUpdate( stlParseParam    * aParam,
                                  stlInt32           aNodePos,
                                  qlpRelInfo       * aRelation,
                                  qlpList          * aHints,
                                  qlpList          * aSet,
                                  qllNode          * aWhere,
                                  qllNode          * aResultSkip,
                                  qllNode          * aResultLimit,
                                  qlpUpdateReturn  * aUpdateReturn )
{
    qlpUpdate  * sUpdate = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpUpdate ),
                            (void**) & sUpdate,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sUpdate = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        if ( aUpdateReturn == NULL )
        {
            sUpdate->mType = QLL_STMT_UPDATE_TYPE;
        }
        else
        {
            if ( aUpdateReturn->mIntoTarget == NULL )
            {
                sUpdate->mType = QLL_STMT_UPDATE_RETURNING_QUERY_TYPE;
            }
            else
            {
                sUpdate->mType = QLL_STMT_UPDATE_RETURNING_INTO_TYPE;
            }
        }
        
        sUpdate->mNodePos     = aNodePos;
        sUpdate->mHasHintErr  = aParam->mHasHintErr;
        sUpdate->mRelation    = aRelation;
        sUpdate->mHints       = aHints;
        sUpdate->mSet         = aSet;
        sUpdate->mWhere       = aWhere;
        sUpdate->mResultSkip  = aResultSkip;

        if( aResultLimit != NULL )
        {
            if( aResultLimit->mType == QLL_POINTER_LIST_TYPE )
            {
                STL_TRY_THROW( aResultSkip == NULL,
                               RAMP_ERR_MULTIPLE_OFFSET_CLAUSE );

                sUpdate->mResultSkip =
                    (qllNode *)(((qlpList *) aResultLimit)->mHead->mData.mPtrValue );
                
                sUpdate->mResultLimit =
                    (qllNode *)(((qlpList *) aResultLimit)->mTail->mData.mPtrValue );
            }
            else
            {
                sUpdate->mResultLimit = aResultLimit;
            }
        }

        if ( aUpdateReturn == NULL )
        {
            sUpdate->mIsReturnNew  = STL_FALSE;
            sUpdate->mReturnTarget = NULL;
            sUpdate->mIntoTarget   = NULL;
        }
        else
        {
            sUpdate->mIsReturnNew  = aUpdateReturn->mIsReturnNew;
            sUpdate->mReturnTarget = aUpdateReturn->mReturnTarget;
            sUpdate->mIntoTarget   = aUpdateReturn->mIntoTarget;
        }
        
    }

    return sUpdate;

    STL_CATCH( RAMP_ERR_MULTIPLE_OFFSET_CLAUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MULTIPLE_OFFSET_CLAUSE,
                      qlgMakeErrPosString( aParam->mBuffer,
                                           aResultSkip->mNodePos,
                                           QLP_PARSE_GET_ENV( aParam ) ),
                      aParam->mErrorStack );
    }

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;

    return sUpdate;
}


/**
 * @brief positioned update 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aHints           Hint Clause
 * @param[in]   aRelation        Relation Info
 * @param[in]   aSet             Set Clause List
 * @param[in]   aCursorName      Cursor Name 
 * @return qlpUpdate node
 */
inline qlpUpdate * qlpMakeUpdatePositionedCursor( stlParseParam    * aParam,
                                                  stlInt32           aNodePos,
                                                  qlpList          * aHints,
                                                  qlpRelInfo       * aRelation,
                                                  qlpList          * aSet,
                                                  qlpValue         * aCursorName )
{
    qlpUpdate  * sUpdate = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpUpdate ),
                            (void**) & sUpdate,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sUpdate = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sUpdate->mType = QLL_STMT_UPDATE_WHERE_CURRENT_OF_TYPE;
        
        sUpdate->mNodePos     = aNodePos;
        sUpdate->mHasHintErr  = aParam->mHasHintErr;
        sUpdate->mRelation    = aRelation;
        sUpdate->mHints       = aHints;
        sUpdate->mSet         = aSet;
        sUpdate->mWhere       = NULL;
        sUpdate->mResultSkip  = NULL;
        sUpdate->mResultLimit = NULL;

        sUpdate->mReturnTarget = NULL;
        sUpdate->mIntoTarget   = NULL;

        sUpdate->mCursorName   = aCursorName;
    }

    return sUpdate;
}


/**
 * @brief savepoint 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aName            Savepoint Name
 * @param[in]   aNamePos         Savepoint Name Position
 * 
 * @return qlpSavepoint node
 */
inline qlpSavepoint * qlpMakeSavepoint( stlParseParam  * aParam,
                                        stlInt32         aNodePos,
                                        stlChar        * aName,
                                        stlInt32         aNamePos )
{
    qlpSavepoint  * sSavepoint = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpSavepoint ),
                            (void**) & sSavepoint,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSavepoint = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sSavepoint->mType    = QLL_STMT_SAVEPOINT_TYPE;
        sSavepoint->mNodePos = aNodePos;
        sSavepoint->mName    = aName;
        sSavepoint->mNamePos = aNamePos;
    }

    return sSavepoint;
}


/**
 * @brief savepoint 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aName            Savepoint Name
 * @param[in]   aNamePos         Savepoint Name Position
 * 
 * @return qlpSavepoint node
 */
inline qlpSavepoint * qlpMakeReleaseSavepoint( stlParseParam  * aParam,
                                               stlInt32         aNodePos,
                                               stlChar        * aName,
                                               stlInt32         aNamePos )
{
    qlpSavepoint  * sSavepoint = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpSavepoint ),
                            (void**) & sSavepoint,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSavepoint = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sSavepoint->mType    = QLL_STMT_RELEASE_SAVEPOINT_TYPE;
        sSavepoint->mNodePos = aNodePos;
        sSavepoint->mName    = aName;
        sSavepoint->mNamePos = aNamePos;
    }

    return sSavepoint;
}


/**
 * @brief commit 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aForce           Force option
 * @param[in]   aWriteMode       Write Mode
 * @param[in]   aComment         Comment option
 * 
 * @return qlpCommit node
 */
inline qlpCommit * qlpMakeCommit( stlParseParam  * aParam,
                                  stlInt32         aNodePos,
                                  qlpValue       * aForce,
                                  qlpValue       * aWriteMode,
                                  qlpValue       * aComment )
{
    qlpCommit  * sCommit = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpCommit ),
                            (void**) & sCommit,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sCommit = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        if( aForce == NULL )
        {
            sCommit->mType = QLL_STMT_COMMIT_TYPE;
        }
        else
        {
            sCommit->mType = QLL_STMT_COMMIT_FORCE_TYPE;
        }
        
        sCommit->mNodePos = aNodePos;
    }

    if( aWriteMode == NULL )
    {
        /**
         * depend on property
         */
        sCommit->mWriteMode = SML_TRANSACTION_CWM_NONE;
    }
    else
    {
        sCommit->mWriteMode = QLP_GET_INT_VALUE( aWriteMode );
    }
    
    sCommit->mForce     = aForce;
    sCommit->mComment   = aComment;

    return sCommit;
}


/**
 * @brief rollback 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aForce           Force option
 * @param[in]   aComment         Comment option
 * @param[in]   aName            Savepoint Name
 * @param[in]   aNamePos         position of Savepoint Name
 * 
 * @return qlpRollback node
 */
inline qlpRollback * qlpMakeRollback( stlParseParam  * aParam,
                                      stlInt32         aNodePos,
                                      qlpValue       * aForce,
                                      qlpValue       * aComment,
                                      stlChar        * aName,
                                      stlInt32         aNamePos )
{
    qlpRollback  * sRollback = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpRollback ),
                            (void**) & sRollback,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sRollback = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        if ( aName == NULL )
        {
            if( aForce == NULL )
            {
                sRollback->mType = QLL_STMT_ROLLBACK_TYPE;
            }
            else
            {
                sRollback->mType = QLL_STMT_ROLLBACK_FORCE_TYPE;
            }
        
            sRollback->mNodePos          = aNodePos;
            sRollback->mSavepointName    = NULL;
            sRollback->mSavepointNamePos = 0;
        }
        else
        {
            sRollback->mType             = QLL_STMT_ROLLBACK_TO_TYPE;
            sRollback->mNodePos          = aNodePos;
            sRollback->mSavepointName    = aName;
            sRollback->mSavepointNamePos = aNamePos;
        }

        sRollback->mForce = aForce;
        sRollback->mComment = aComment;
    }

    return sRollback;
}


/**
 * @brief lock table 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aRelations       Relation List
 * @param[in]   aLockMode        Lock Mode
 * @param[in]   aLockModePos     position of Lock Mode
 * @param[in]   aWaitTime        Wait Time
 * 
 * @return qlpLockTable node
 */
inline qlpLockTable * qlpMakeLockTable( stlParseParam  * aParam,
                                        stlInt32         aNodePos,
                                        qlpList        * aRelations,
                                        qlpValue       * aLockMode,
                                        stlInt32         aLockModePos,
                                        qlpValue       * aWaitTime )
{
    qlpLockTable  * sLockTable = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpLockTable ),
                            (void**) & sLockTable,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sLockTable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sLockTable->mType        = QLL_STMT_LOCK_TABLE_TYPE;
        sLockTable->mNodePos     = aNodePos;
        sLockTable->mRelations   = aRelations;
        sLockTable->mLockMode    = QLP_GET_INT_VALUE( aLockMode );
        sLockTable->mLockModePos = aLockModePos;
        sLockTable->mWaitTime    = aWaitTime;
    }

    return sLockTable;
}


/**
 * @brief QUERY NODE 노드에 option 추가
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aQueryNode      Query Node
 * @param[in]   aWith           With Clause
 * @param[in]   aOrderBy        Order By Clause
 * @param[in]   aOrderByPos     Order By Clause Position
 * @param[in]   aResultSkip     Offset Clause
 * @param[in]   aResultLimit    Fetch Clause
 *
 * @return qllNode 노드 
 */
inline qllNode * qlpAddQueryNodeOption( stlParseParam    * aParam,
                                        stlInt32           aNodePos,
                                        qllNode          * aQueryNode,
                                        void             * aWith,
                                        qlpList          * aOrderBy,
                                        stlInt32           aOrderByPos,
                                        qllNode          * aResultSkip,
                                        qllNode          * aResultLimit )
{
    qlpQuerySetNode     * sQuerySetNode     = NULL;
    qlpQuerySpecNode    * sQuerySpecNode    = NULL;
    qllNode             * sQueryNode        = NULL;
    
    sQueryNode = aQueryNode;
    while( sQueryNode->mType == QLL_SUB_TABLE_NODE_TYPE )
    {
        sQueryNode = ((qlpSubTableNode *)sQueryNode)->mQueryNode;
    }
    
    if( sQueryNode->mType == QLL_QUERY_SET_NODE_TYPE )
    {
        sQuerySetNode = (qlpQuerySetNode*)sQueryNode;

        if( aWith != NULL )
        {
            /* with절의 중복은 parser에서 걸러지므로 에러처리가 필요없다. */
            sQuerySetNode->mWith = aWith;
        }

        if( aOrderBy != NULL )
        {
            STL_TRY_THROW( sQuerySetNode->mOrderBy == NULL,
                           RAMP_ERR_MULTIPLE_ORDER_BY_CLAUSE );

            sQuerySetNode->mOrderBy = aOrderBy;
        }

        if( aResultSkip != NULL )
        {
            STL_TRY_THROW( sQuerySetNode->mResultSkip == NULL,
                           RAMP_ERR_MULTIPLE_OFFSET_CLAUSE );

            sQuerySetNode->mResultSkip = aResultSkip;
        }

        if( aResultLimit != NULL )
        {
            STL_TRY_THROW( sQuerySetNode->mResultLimit == NULL,
                           RAMP_ERR_MULTIPLE_LIMIT_CLAUSE );

            if( aResultLimit->mType == QLL_POINTER_LIST_TYPE )
            {
                STL_TRY_THROW( sQuerySetNode->mResultSkip == NULL,
                               RAMP_ERR_MULTIPLE_OFFSET_CLAUSE );

                sQuerySetNode->mResultSkip =
                    (qllNode *)(((qlpList *) aResultLimit)->mHead->mData.mPtrValue );
                
                sQuerySetNode->mResultLimit =
                    (qllNode *)(((qlpList *) aResultLimit)->mTail->mData.mPtrValue );
            }
            else
            {
                sQuerySetNode->mResultLimit = aResultLimit;
            }
        }
    }
    else
    {
        sQuerySpecNode = (qlpQuerySpecNode*)sQueryNode;

        if( aWith != NULL )
        {
            /* with절의 중복은 parser에서 걸러지므로 에러처리가 필요없다. */
            sQuerySpecNode->mWith = aWith;
        }

        if( aOrderBy != NULL )
        {
            STL_TRY_THROW( sQuerySpecNode->mOrderBy == NULL,
                           RAMP_ERR_MULTIPLE_ORDER_BY_CLAUSE );

            sQuerySpecNode->mOrderBy = aOrderBy;
        }

        if( aResultSkip != NULL )
        {
            STL_TRY_THROW( sQuerySpecNode->mResultSkip == NULL,
                           RAMP_ERR_MULTIPLE_OFFSET_CLAUSE );

            sQuerySpecNode->mResultSkip = aResultSkip;
        }

        if( aResultLimit != NULL )
        {
            STL_TRY_THROW( sQuerySpecNode->mResultLimit == NULL,
                           RAMP_ERR_MULTIPLE_LIMIT_CLAUSE );

            if( aResultLimit->mType == QLL_POINTER_LIST_TYPE )
            {
                STL_TRY_THROW( sQuerySpecNode->mResultSkip == NULL,
                               RAMP_ERR_MULTIPLE_OFFSET_CLAUSE );

                sQuerySpecNode->mResultSkip =
                    (qllNode *)(((qlpList *) aResultLimit)->mHead->mData.mPtrValue );
                
                sQuerySpecNode->mResultLimit =
                    (qllNode *)(((qlpList *) aResultLimit)->mTail->mData.mPtrValue );
            }
            else
            {
                sQuerySpecNode->mResultLimit = aResultLimit;
            }
        }
    }

    return aQueryNode;

    STL_CATCH( RAMP_ERR_MULTIPLE_ORDER_BY_CLAUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MULTIPLE_ORDER_BY_CLAUSE,
                      qlgMakeErrPosString( aParam->mBuffer,
                                           aOrderByPos,
                                           QLP_PARSE_GET_ENV( aParam ) ),
                      aParam->mErrorStack );
    }

    STL_CATCH( RAMP_ERR_MULTIPLE_OFFSET_CLAUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MULTIPLE_OFFSET_CLAUSE,
                      qlgMakeErrPosString( aParam->mBuffer,
                                           aResultSkip->mNodePos,
                                           QLP_PARSE_GET_ENV( aParam ) ),
                      aParam->mErrorStack );
    }

    STL_CATCH( RAMP_ERR_MULTIPLE_LIMIT_CLAUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MULTIPLE_LIMIT_CLAUSE,
                      qlgMakeErrPosString( aParam->mBuffer,
                                           aResultLimit->mNodePos,
                                           QLP_PARSE_GET_ENV( aParam ) ),
                      aParam->mErrorStack );
    }

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;

    return aQueryNode;
}

/**
 * @brief QUERY SET NODE 노드 생성
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aSetType            SET Operation Type
 * @param[in]   aSetQuantifier      SET Quantifier
 * @param[in]   aLeftQueryNode      Left Query Node
 * @param[in]   aRightQueryNode     Right Query Node
 * @param[in]   aCorrespondingSpec  Corresponding Specification
 *
 * @return qllNode 노드 
 */
inline qllNode * qlpMakeQuerySetNode( stlParseParam     * aParam,
                                      stlInt32            aNodePos,
                                      qlpSetType          aSetType,
                                      qlpSetQuantifier    aSetQuantifier,
                                      qllNode           * aLeftQueryNode,
                                      qllNode           * aRightQueryNode,
                                      void              * aCorrespondingSpec )
{
    qlpQuerySetNode * sQuerySetNode = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpQuerySetNode ),
                            (void**) & sQuerySetNode,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sQuerySetNode = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sQuerySetNode->mType                = QLL_QUERY_SET_NODE_TYPE;
        sQuerySetNode->mNodePos             = aNodePos;
        sQuerySetNode->mSetType             = aSetType;
        sQuerySetNode->mSetQuantifier       = aSetQuantifier;
        sQuerySetNode->mLeftQueryNode       = aLeftQueryNode;
        sQuerySetNode->mRightQueryNode      = aRightQueryNode;
        sQuerySetNode->mCorrespondingSpec   = aCorrespondingSpec;

        sQuerySetNode->mWith                = NULL;
        sQuerySetNode->mOrderBy             = NULL;
        sQuerySetNode->mResultSkip          = NULL;
        sQuerySetNode->mResultLimit         = NULL;
    }

    return (qllNode*)sQuerySetNode;
}

/**
 * @brief QUERY SPECIFICATION NODE 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aHints          Hint 
 * @param[in]   aSetQuantifier  Set Quantifier 
 * @param[in]   aTargets        Target List
 * @param[in]   aFrom           From Clause
 * @param[in]   aWhere          Where Clause
 * @param[in]   aGroupBy        Group By Clause
 * @param[in]   aHaving         Having Clause
 * @param[in]   aWindow         Window Clause
 *
 * @return qllNode 노드 
 */
inline qllNode * qlpMakeQuerySpecNode( stlParseParam  * aParam,
                                       stlInt32         aNodePos,
                                       qlpList        * aHints,
                                       qlpSetQuantifier aSetQuantifier,
                                       qlpList        * aTargets,
                                       qllNode        * aFrom,
                                       qllNode        * aWhere,
                                       qllNode        * aGroupBy,
                                       qllNode        * aHaving,
                                       qllNode        * aWindow )
{
    qlpQuerySpecNode     * sQuerySpecNode = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpQuerySpecNode ),
                            (void**) & sQuerySpecNode,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sQuerySpecNode = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sQuerySpecNode->mType           = QLL_QUERY_SPEC_NODE_TYPE;
        sQuerySpecNode->mNodePos        = aNodePos;

        sQuerySpecNode->mHints          = aHints;
        sQuerySpecNode->mSetQuantifier  = aSetQuantifier;
        sQuerySpecNode->mTargets        = aTargets;
        sQuerySpecNode->mFrom           = aFrom;
        sQuerySpecNode->mWhere          = aWhere;
        sQuerySpecNode->mGroupBy        = aGroupBy;
        sQuerySpecNode->mHaving         = aHaving;
        sQuerySpecNode->mWindow         = aWindow;

        sQuerySpecNode->mWith           = NULL;
        sQuerySpecNode->mOrderBy        = NULL;
        sQuerySpecNode->mResultSkip     = NULL;
        sQuerySpecNode->mResultLimit    = NULL;
    }

    return (qllNode*)sQuerySpecNode;
}

/**
 * @brief join type info node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aJoinType       Join Type
 *
 * @return qlpJoinTypeInfo node
 */
inline qlpJoinTypeInfo * qlpMakeJoinTypeInfo( stlParseParam         * aParam,
                                              stlInt32                aNodePos,
                                              qlpJoinType             aJoinType )
{
    qlpJoinTypeInfo    * sJoinTypeInfo = NULL;
    
    /**
     * Node 생성
     */
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpJoinTypeInfo ),
                            (void**) & sJoinTypeInfo,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sJoinTypeInfo = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        /**
         *  Node 정보 설정
         */
        
        sJoinTypeInfo->mType        = QLL_JOIN_TYPE_INFO_TYPE;
        sJoinTypeInfo->mNodePos     = aNodePos;

        sJoinTypeInfo->mJoinType        = aJoinType;
    }
    
    return sJoinTypeInfo;
}

/**
 * @brief join specification node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aJoinCondition  Join Condition
 * @param[in]   aNamedColumns   Named Columns
 *
 * @return qlpJoinSpecification node
 */
inline qlpJoinSpecification * qlpMakeJoinSpecification( stlParseParam         * aParam,
                                                        stlInt32                aNodePos,
                                                        qlpList               * aJoinCondition,
                                                        qlpList               * aNamedColumns )
{
    qlpJoinSpecification    * sJoinSpec = NULL;
    
    /**
     * Node 생성
     */
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpJoinSpecification ),
                            (void**) & sJoinSpec,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sJoinSpec = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        /**
         *  Node 정보 설정
         */
        
        sJoinSpec->mType        = QLL_JOIN_SPECIFICATION_TYPE;
        sJoinSpec->mNodePos     = aNodePos;

        sJoinSpec->mJoinCondition   = aJoinCondition;
        sJoinSpec->mNamedColumns    = aNamedColumns;
    }
    
    return sJoinSpec;
}

/**
 * @brief base table node 생성
 * @param[in]   aParam        Parse Parameter
 * @param[in]   aNodePos      Node Position
 * @param[in]   aRelName      Relation Name
 * @param[in]   aDumpOpt      Dump Option
 * @param[in]   aAlias        Alias
 *
 * @return qllNode node
 */
inline qllNode * qlpMakeBaseTableNode( stlParseParam  * aParam,
                                       stlInt32         aNodePos,
                                       qlpList        * aRelName,
                                       qlpValue       * aDumpOpt,
                                       qlpAlias       * aAlias )
{
    qlpBaseTableNode   * sBaseTableNode = NULL;
    
    /**
     * Node 생성
     */
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpBaseTableNode ),
                            (void**) & sBaseTableNode,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sBaseTableNode = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        /**
         *  Node 정보 설정
         */
        
        sBaseTableNode->mType       = QLL_BASE_TABLE_NODE_TYPE;
        sBaseTableNode->mNodePos    = aNodePos;
        sBaseTableNode->mName       = qlpMakeObjectName( aParam, aRelName ); 
        sBaseTableNode->mDumpOpt    = aDumpOpt;
        sBaseTableNode->mAlias      = aAlias;

        if( sBaseTableNode->mAlias != NULL )
        {
            sBaseTableNode->mAlias->mBaseObject = (qllNode*)sBaseTableNode;
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    return (qllNode*)sBaseTableNode;
}

/**
 * @brief sub table node 생성
 * @param[in]   aParam        Parse Parameter
 * @param[in]   aNodePos      Node Position
 * @param[in]   aQueryNode    Query Node
 * @param[in]   aAlias        Alias
 * @param[in]   aColAlias     Column Alias
 *
 * @return qllNode node
 */
inline qllNode * qlpMakeSubTableNode( stlParseParam  * aParam,
                                      stlInt32         aNodePos,
                                      qllNode        * aQueryNode,
                                      qlpAlias       * aAlias,
                                      qlpList        * aColAlias )
{
    qlpSubTableNode * sSubTableNode = NULL;
    
    /**
     * Node 생성
     */

    if( aQueryNode->mType == QLL_SUB_TABLE_NODE_TYPE )
    {
        sSubTableNode = (qlpSubTableNode *) aQueryNode;
        
        sSubTableNode->mAlias        = aAlias;
        sSubTableNode->mColAliasList = aColAlias;

        return aQueryNode;
    }
    else
    {
        if( aParam->mAllocator( aParam->mContext,
                                STL_SIZEOF( qlpSubTableNode ),
                                (void**) & sSubTableNode,
                                aParam->mErrorStack )
            != STL_SUCCESS )
        {
            sSubTableNode = NULL;
            aParam->mErrStatus = STL_FAILURE;
        }
        else
        {
            /**
             *  Node 정보 설정
             */
        
            sSubTableNode->mType         = QLL_SUB_TABLE_NODE_TYPE;
            sSubTableNode->mNodePos      = aNodePos;
            sSubTableNode->mQueryNode    = aQueryNode;
            sSubTableNode->mAlias        = aAlias;
            sSubTableNode->mColAliasList = aColAlias;

            if( sSubTableNode->mAlias != NULL )
            {
                sSubTableNode->mAlias->mBaseObject = (qllNode*)sSubTableNode;
            }
            else
            {
                /* Do Nothing */
            }
        }

        return (qllNode*)sSubTableNode;
    }
}

/**
 * @brief join table node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aIsNatural      Natural Join 여부
 * @param[in]   aJoinType       Join Type
 * @param[in]   aLeftTableNode  Left Table Node
 * @param[in]   aRightTableNode Right Table Node
 * @param[in]   aJoinSpec       Join Specification
 *
 * @return qllNode node
 */
inline qllNode * qlpMakeJoinTableNode( stlParseParam         * aParam,
                                       stlInt32                aNodePos,
                                       stlBool                 aIsNatural,
                                       qlpJoinType             aJoinType,
                                       qllNode               * aLeftTableNode,
                                       qllNode               * aRightTableNode,
                                       qlpJoinSpecification  * aJoinSpec )
{
    qlpJoinTableNode    * sJoinTableNode    = NULL;
    
    /**
     * Node 생성
     */
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpJoinTableNode ),
                            (void**) & sJoinTableNode,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sJoinTableNode = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        /**
         *  Node 정보 설정
         */
        
        sJoinTableNode->mType           = QLL_JOIN_TABLE_NODE_TYPE;
        sJoinTableNode->mNodePos        = aNodePos;
        sJoinTableNode->mIsNatural      = aIsNatural;
        sJoinTableNode->mJoinType       = aJoinType;
        sJoinTableNode->mLeftTableNode  = aLeftTableNode;
        sJoinTableNode->mRightTableNode = aRightTableNode;
        sJoinTableNode->mJoinSpec       = aJoinSpec;
    }
    
    return (qllNode*)sJoinTableNode;
}


/**
 * @brief relation info node 생성
 * @param[in]   aParam        Parse Parameter
 * @param[in]   aNodePos      Node Position
 * @param[in]   aRelName      Relation Name
 * @param[in]   aAlias        Alias Node
 *
 * @return qlpRelInfo node
 */
inline qlpRelInfo * qlpMakeRelInfo( stlParseParam  * aParam,
                                    stlInt32         aNodePos,
                                    qlpList        * aRelName,
                                    qlpAlias       * aAlias )
{
    qlpRelInfo      * sRelInfo = NULL;
    
    /**
     * Node 생성
     */
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpRelInfo ),
                            (void**) & sRelInfo,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sRelInfo = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        /**
         *  Node 정보 설정
         */
        
        sRelInfo->mName    = qlpMakeObjectName( aParam, aRelName ); 
        sRelInfo->mNodePos = aNodePos;
        sRelInfo->mType    = QLL_REL_INFO_TYPE;
        sRelInfo->mAlias   = aAlias;
        sRelInfo->mIsUsed  = STL_FALSE;

        if( sRelInfo->mAlias != NULL )
        {
            sRelInfo->mAlias->mBaseObject = (qllNode*) sRelInfo;
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    return sRelInfo;
}


/**
 * @brief alias node 생성 
 * @param[in]   aParam         Parse Parameter
 * @param[in]   aNodePos       Node Position
 * @param[in]   aAliasName     Alias Name
 * 
 * @return qlpAlias node
 */
inline qlpAlias * qlpMakeAlias( stlParseParam  * aParam,
                                stlInt32         aNodePos,
                                qlpValue       * aAliasName )
{
    qlpAlias  * sAlias = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlias ),
                            (void**) & sAlias,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlias = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlias->mType         = QLL_ALIAS_TYPE;
        sAlias->mNodePos      = aNodePos;
        sAlias->mAliasName    = aAliasName;
        sAlias->mColumnNames  = NULL;

        qlpAddAliasList( aParam, sAlias );
    }

    return sAlias;
}


/**
 * @brief parameter node 생성 
 * @param[in]   aParam         Parse Parameter
 * @param[in]   aNodePos       Node Position
 * @param[in]   aParamType     Parameter Type
 * @param[in]   aInOutType     Parameter 가 필요로 하는 Bind 유형 (IN/OUT/INOUT/INVALID)
 * @param[in]   aParamTypePos  position of Parameter Type
 * @param[in]   aParamName     Parameter name
 * 
 * @return qlpParameter node
 */
inline qlpParameter * qlpMakeParameter( stlParseParam  * aParam,
                                        stlInt32         aNodePos,
                                        qlpParamType     aParamType,
                                        knlBindType      aInOutType,
                                        stlInt32         aParamTypePos,
                                        qlpList        * aParamName )
{
    qlpParameter    * sParamNode = NULL;
    qlpListElement  * sListElem  = NULL;
        
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpParameter ),
                            (void**) & sParamNode,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sParamNode = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sParamNode->mType         = QLL_PARAM_TYPE;
        sParamNode->mNodePos      = aNodePos;
        sParamNode->mParamType    = aParamType;
        sParamNode->mInOutType    = aInOutType;
        sParamNode->mParamTypePos = aParamTypePos;

        switch( aParamType )
        {
            case QLP_PARAM_TYPE_HOST :
                {
                    STL_DASSERT( aParamName != NULL );
                    
                    sListElem = QLP_LIST_GET_FIRST( aParamName );
                    sParamNode->mHostName =
                        (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sListElem );

                    sListElem = QLP_LIST_GET_NEXT_ELEM( sListElem );
                    sParamNode->mIndicatorName =
                        (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sListElem );
                    break;
                }
            case QLP_PARAM_TYPE_DYNAMIC :
                {
                    sParamNode->mHostName      = NULL;
                    sParamNode->mIndicatorName = NULL;
                    break;
                }
            default :
                {
                    STL_DASSERT( 0 );
                    break;
                }
        }

        qlpAddParamList( aParam, sParamNode );
    }

    return sParamNode;
}


/**
 * @brief function node 생성 
 * @param[in]   aParam      Parse Parameter
 * @param[in]   aFuncId     Function ID
 * @param[in]   aNodePos    Node Position
 * @param[in]   aNodeLen    Node Length
 * @param[in]   aArgs       Function Argument List
 * 
 * @return qlpFunction node
 */
inline qlpFunction * qlpMakeFunction( stlParseParam   * aParam,
                                      knlBuiltInFunc    aFuncId,
                                      stlInt32          aNodePos,
                                      stlInt32          aNodeLen,
                                      qlpList         * aArgs )
{
    qlpFunction  * sFunction = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpFunction ),
                            (void**) & sFunction,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sFunction = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sFunction->mType    = QLL_FUNCTION_TYPE;
        sFunction->mFuncId  = aFuncId;
        sFunction->mNodePos = aNodePos;
        sFunction->mNodeLen = aNodeLen;
        sFunction->mArgs    = aArgs;
    }

    return sFunction;    
}


/**
 * @brief function node의 argument list 포인터를 반환
 * @param[in]   aFuncNode     Function Node
 * 
 * @return qlpList node
 */
inline qlpList * qlpGetFunctionArgList( qlpFunction * aFuncNode )
{
    return aFuncNode->mArgs;
}


/**
 * @brief column definition node 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aColumnName      Column Name
 * @param[in]   aColumnNamePos   position of Column Name
 * @param[in]   aTypeName        Column Type Name
 * @param[in]   aConstraints     Column Constraint
 * @param[in]   aColDefSecond    Column Second Definition 
 *
 * @return qlpColumnDef node
 */
inline qlpColumnDef * qlpMakeColumnDef( stlParseParam    * aParam,
                                        stlInt32           aNodePos,
                                        stlChar          * aColumnName,
                                        stlInt32           aColumnNamePos,
                                        qlpTypeName      * aTypeName,
                                        qlpList          * aConstraints,
                                        qlpColDefSecond  * aColDefSecond )
{
    qlpColumnDef  * sColDef = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpColumnDef ),
                            (void**) & sColDef,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sColDef = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sColDef->mType           = QLL_COLUMN_DEF_TYPE;
        sColDef->mNodePos        = aNodePos;
        sColDef->mColumnName     = aColumnName;
        sColDef->mColumnNamePos  = aColumnNamePos;
        sColDef->mTypeName       = aTypeName;
        sColDef->mConstraints    = aConstraints;
        sColDef->mDefSecond      = aColDefSecond;
    }

    return sColDef;
}


/**
 * @brief null constraint 생성 
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aConstTypePos   position of Constraint Type 
 *
 * @return qlpConstraint node
 */
inline qlpConstraint * qlpMakeNullConst( stlParseParam  * aParam,
                                         stlInt32         aNodePos,
                                         stlInt32         aConstTypePos )
{
    qlpConstraint  * sConst = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpConstraint ),
                            (void**) & sConst,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sConst = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sConst->mType               = QLL_CONSTRAINT_TYPE;
        sConst->mNodePos            = aNodePos;
        sConst->mConstType          = QLP_CONSTRAINT_NULL;
        sConst->mConstTypePos       = aConstTypePos;
        sConst->mName               = NULL;
        sConst->mUniqueFields       = NULL;
        sConst->mPKTable            = NULL;
        sConst->mPKFields           = NULL;
        sConst->mFKFields           = NULL;
        sConst->mFKMatchType        = QLP_FK_MATCH_INVALID;
        sConst->mFKMatchTypePos     = 0;  
        sConst->mFKUpdateAction     = QLP_FK_ACTION_INVALID;
        sConst->mFKUpdateActionPos  = 0; 
        sConst->mFKDeleteAction     = QLP_FK_ACTION_INVALID;
        sConst->mFKDeleteActionPos  = 0;
        sConst->mRawExpr            = NULL;

        sConst->mIndexName          = NULL;
        qlpSetIndexAttr( aParam, NULL, & sConst->mIndexAttr );
        sConst->mIndexSpace         = NULL;
    }

    return sConst;
}


/**
 * @brief not null constraint 생성 
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aConstTypePos   position of Constraint Type 
 *
 * @return qlpConstraint node
 */
inline qlpConstraint * qlpMakeNotNullConst( stlParseParam  * aParam,
                                            stlInt32         aNodePos,
                                            stlInt32         aConstTypePos )
{
    qlpConstraint  * sConst = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpConstraint ),
                            (void**) & sConst,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sConst = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sConst->mType               = QLL_CONSTRAINT_TYPE;
        sConst->mNodePos            = aNodePos;
        sConst->mConstType          = QLP_CONSTRAINT_NOTNULL;
        sConst->mConstTypePos       = aConstTypePos;
        sConst->mName               = NULL;
        sConst->mUniqueFields       = NULL;
        sConst->mPKTable            = NULL;
        sConst->mPKFields           = NULL;
        sConst->mFKFields           = NULL;
        sConst->mFKMatchType        = QLP_FK_MATCH_INVALID;
        sConst->mFKMatchTypePos     = 0;  
        sConst->mFKUpdateAction     = QLP_FK_ACTION_INVALID;
        sConst->mFKUpdateActionPos  = 0; 
        sConst->mFKDeleteAction     = QLP_FK_ACTION_INVALID;
        sConst->mFKDeleteActionPos  = 0;
        sConst->mRawExpr            = NULL;

        sConst->mIndexName          = NULL;
        qlpSetIndexAttr( aParam, NULL, & sConst->mIndexAttr );
        sConst->mIndexSpace         = NULL;
    }

    return sConst;
}


/**
 * @brief unique constraint 생성 
 * @param[in]   aParam        Parse Parameter
 * @param[in]   aNodePos      Node Position
 * @param[in]   aConstTypePos position of Constraint Type 
 * @param[in]   aUniqueFields Unique Constraint 대상 컬럼들
 * @param[in]   aIndexName      Key Constraint 를 위한 Index 이름 (Schema 를 지정할 수 없음)
 * @param[in]   aIndexAttrList  Index Attribute List
 * @param[in]   aIndexSpace     Index Tablespace 이름 
 *
 * @return qlpConstraint node
 */
inline qlpConstraint * qlpMakeUniqueConst( stlParseParam      * aParam,
                                           stlInt32             aNodePos,
                                           stlInt32             aConstTypePos,
                                           qlpList            * aUniqueFields,
                                           qlpValue           * aIndexName,
                                           qlpList            * aIndexAttrList,
                                           qlpObjectName      * aIndexSpace )
{
    qlpConstraint  * sConst = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpConstraint ),
                            (void**) & sConst,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sConst = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sConst->mType               = QLL_CONSTRAINT_TYPE;
        sConst->mNodePos            = aNodePos;
        sConst->mConstType          = QLP_CONSTRAINT_UNIQUE;
        sConst->mConstTypePos       = aConstTypePos;
        sConst->mName               = NULL;
        sConst->mUniqueFields       = aUniqueFields;
        sConst->mPKTable            = NULL;
        sConst->mPKFields           = NULL;
        sConst->mFKFields           = NULL;
        sConst->mFKMatchType        = QLP_FK_MATCH_INVALID;
        sConst->mFKMatchTypePos     = 0;  
        sConst->mFKUpdateAction     = QLP_FK_ACTION_INVALID;
        sConst->mFKUpdateActionPos  = 0; 
        sConst->mFKDeleteAction     = QLP_FK_ACTION_INVALID;
        sConst->mFKDeleteActionPos  = 0;
        sConst->mRawExpr            = NULL;

        sConst->mIndexName  = aIndexName;
        qlpSetIndexAttr( aParam, aIndexAttrList, & sConst->mIndexAttr );
        sConst->mIndexSpace = aIndexSpace;
    }

    return sConst;
}


/**
 * @brief primary key constraint 생성 
 * @param[in]   aParam        Parse Parameter
 * @param[in]   aNodePos      Node Position
 * @param[in]   aConstTypePos position of Constraint Type 
 * @param[in]   aPKFields     Primary Key Constraint 대상 컬럼들
 * @param[in]   aIndexName      Key Constraint 를 위한 Index 이름 (Schema 를 지정할 수 없음)
 * @param[in]   aIndexAttrList  Index Attribute List
 * @param[in]   aIndexSpace     Index Tablespace 이름 
 *
 * @return qlpConstraint node
 */
inline qlpConstraint * qlpMakePKConst( stlParseParam      * aParam,
                                       stlInt32             aNodePos,
                                       stlInt32             aConstTypePos,
                                       qlpList            * aPKFields,
                                       qlpValue           * aIndexName,
                                       qlpList            * aIndexAttrList,
                                       qlpObjectName      * aIndexSpace )
{
    qlpConstraint  * sConst = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpConstraint ),
                            (void**) & sConst,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sConst = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sConst->mType               = QLL_CONSTRAINT_TYPE;
        sConst->mNodePos            = aNodePos;
        sConst->mConstType          = QLP_CONSTRAINT_PRIMARY;
        sConst->mConstTypePos       = aConstTypePos;
        sConst->mName               = NULL;
        sConst->mUniqueFields       = aPKFields;
        sConst->mPKTable            = NULL;
        sConst->mPKFields           = NULL;
        sConst->mFKFields           = NULL;
        sConst->mFKMatchType        = QLP_FK_MATCH_INVALID;
        sConst->mFKMatchTypePos     = 0;  
        sConst->mFKUpdateAction     = QLP_FK_ACTION_INVALID;
        sConst->mFKUpdateActionPos  = 0; 
        sConst->mFKDeleteAction     = QLP_FK_ACTION_INVALID;
        sConst->mFKDeleteActionPos  = 0;
        sConst->mRawExpr            = NULL;

        sConst->mIndexName  = aIndexName;
        qlpSetIndexAttr( aParam, aIndexAttrList, & sConst->mIndexAttr );
        sConst->mIndexSpace = aIndexSpace;
    }

    return sConst;
}


/**
 * @brief foreign key constraint 생성 
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aConstTypePos       position of Constraint Type 
 * @param[in]   aPKTable            참조되는 테이블 ( Primary Key 대상 Table )
 * @param[in]   aPKFields           참조되는 테이블의 컬럼들 ( Primary Key 대상 Columns )
 * @param[in]   aFKFields           참조하는 컬럼들 ( Foreign Key 대상 Columns )
 * @param[in]   aFKMatchType        Match Type
 * @param[in]   aFKMatchTypePos     position of Match Type
 * @param[in]   aFKUpdateAction     Update Trigger Action
 * @param[in]   aFKUpdateActionPos  position of Update Trigger Action
 * @param[in]   aFKDeleteAction     Delete Trigger Action
 * @param[in]   aFKDeleteActionPos  position of Delete Trigger Action
 *
 * @return qlpConstraint node
 */
inline qlpConstraint * qlpMakeFKConst( stlParseParam      * aParam,
                                       stlInt32             aNodePos,
                                       stlInt32             aConstTypePos,
                                       qlpRelInfo         * aPKTable,
                                       qlpList            * aPKFields,
                                       qlpList            * aFKFields,
                                       qlpFKMatchType       aFKMatchType,
                                       stlInt32             aFKMatchTypePos,
                                       qlpFKAction          aFKUpdateAction,
                                       stlInt32             aFKUpdateActionPos,
                                       qlpFKAction          aFKDeleteAction,
                                       stlInt32             aFKDeleteActionPos )
{
    qlpConstraint  * sConst = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpConstraint ),
                            (void**) & sConst,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sConst = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sConst->mType               = QLL_CONSTRAINT_TYPE;
        sConst->mNodePos            = aNodePos;
        sConst->mConstType          = QLP_CONSTRAINT_FOREIGN;
        sConst->mConstTypePos       = aConstTypePos;
        sConst->mName               = NULL;
        sConst->mUniqueFields       = NULL;
        sConst->mPKTable            = aPKTable;
        sConst->mPKFields           = aPKFields;
        sConst->mFKFields           = aFKFields;
        sConst->mFKMatchType        = aFKMatchType;
        sConst->mFKMatchTypePos     = aFKMatchTypePos;  
        sConst->mFKUpdateAction     = aFKUpdateAction;
        sConst->mFKUpdateActionPos  = aFKUpdateActionPos; 
        sConst->mFKDeleteAction     = aFKDeleteAction;
        sConst->mFKDeleteActionPos  = aFKDeleteActionPos;
        sConst->mRawExpr            = NULL;

        sConst->mIndexName          = NULL;
        qlpSetIndexAttr( aParam, NULL, & sConst->mIndexAttr );
        sConst->mIndexSpace         = NULL;
    }

    return sConst;
}


/**
 * @brief CHECK constraint 생성 
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aConstTypePos       position of Constraint Type 
 * @param[in]   aExpr               CHECK Expression
 *
 * @return qlpConstraint node
 */
inline qlpConstraint * qlpMakeCheckConst( stlParseParam      * aParam,
                                          stlInt32             aNodePos,
                                          stlInt32             aConstTypePos,
                                          qllNode            * aExpr )
{
    qlpConstraint  * sConst = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpConstraint ),
                            (void**) & sConst,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sConst = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sConst->mType               = QLL_CONSTRAINT_TYPE;
        sConst->mNodePos            = aNodePos;
        sConst->mConstType          = QLP_CONSTRAINT_CHECK;
        sConst->mConstTypePos       = aConstTypePos;
        sConst->mName               = NULL;
        sConst->mUniqueFields       = NULL;
        sConst->mPKTable            = NULL;
        sConst->mPKFields           = NULL;
        sConst->mFKFields           = NULL;
        sConst->mFKMatchType        = QLP_FK_MATCH_INVALID;
        sConst->mFKMatchTypePos     = 0;  
        sConst->mFKUpdateAction     = QLP_FK_ACTION_INVALID;
        sConst->mFKUpdateActionPos  = 0; 
        sConst->mFKDeleteAction     = QLP_FK_ACTION_INVALID;
        sConst->mFKDeleteActionPos  = 0;
        sConst->mRawExpr            = aExpr;

        sConst->mIndexName          = NULL;
        qlpSetIndexAttr( aParam, NULL, & sConst->mIndexAttr );
        sConst->mIndexSpace         = NULL;
    }

    return sConst;
}


/**
 * @brief 고정 길이형 데이타 타입 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodeLen         Node Length
 * @param[in]   aNodePos         Node Position
 * @param[in]   aDBType          DB Type
 * @param[in]   aDBTypePos       position of DB Type
 *
 * @return qlpTypeName node
 */
inline qlpTypeName * qlpMakeStaticTypeName( stlParseParam  * aParam,
                                            stlInt32         aNodePos,
                                            stlInt32         aNodeLen,
                                            dtlDataType      aDBType,
                                            stlInt32         aDBTypePos )
{
    qlpTypeName  * sTypeName = NULL;
    stlBool        sIsStatic = STL_FALSE;

    dtlIsStaticType( aDBType,
                     & sIsStatic,
                     QLL_ERROR_STACK( QLP_PARSE_GET_ENV( aParam ) ) );

    if( sIsStatic == STL_TRUE )
    {
        if( aParam->mAllocator( aParam->mContext,
                                STL_SIZEOF( qlpTypeName ),
                                (void**) & sTypeName,
                                aParam->mErrorStack )
            != STL_SUCCESS )
        {
            sTypeName = NULL;
            aParam->mErrStatus = STL_FAILURE;
        }
        else
        {
            sTypeName->mType      = QLL_TYPE_NAME_TYPE;
            sTypeName->mNodePos   = aNodePos;
            sTypeName->mNodeLen   = aNodeLen;
            sTypeName->mDBType    = aDBType;
            sTypeName->mDBTypePos = aDBTypePos;
        }
    }
    else
    {
        sTypeName = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    
    return sTypeName;
}

/**
 * @brief CHARACTER형 데이타 타입 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeLen         Node Length
 * @param[in]   aDBType          DB Type
 * @param[in]   aDBTypePos       position of DB Type
 * @param[in]   aStrLen          String Length
 * @param[in]   aCharSet         Character Set Name
 * @param[in]   aCharSetPos      position of Character Set Name
 *
 * @return qlpTypeName node
 */
inline qlpTypeName * qlpMakeCharTypeName( stlParseParam        * aParam,
                                          stlInt32               aNodePos,
                                          stlInt32               aNodeLen,
                                          dtlDataType            aDBType,
                                          stlInt32               aDBTypePos,
                                          qlpCharLength        * aStrLen,
                                          stlChar              * aCharSet,
                                          stlInt32               aCharSetPos )
{
    qlpTypeName  * sTypeName = NULL;

    if( ( aDBType == DTL_TYPE_CHAR ) ||
        ( aDBType == DTL_TYPE_VARCHAR ) ||
        ( aDBType == DTL_TYPE_LONGVARCHAR ) )
    {
        if( aParam->mAllocator( aParam->mContext,
                                STL_SIZEOF( qlpTypeName ),
                                (void**) & sTypeName,
                                aParam->mErrorStack )
            != STL_SUCCESS )
        {
            sTypeName = NULL;
            aParam->mErrStatus = STL_FAILURE;
        }
        else
        {
            sTypeName->mType       = QLL_TYPE_NAME_TYPE;
            sTypeName->mNodePos    = aNodePos;
            sTypeName->mNodeLen    = aNodeLen;
            sTypeName->mDBType     = aDBType;
            sTypeName->mDBTypePos  = aDBTypePos;

            sTypeName->mStringLength    = aStrLen;
            sTypeName->mCharacterSet    = aCharSet;
            sTypeName->mCharacterSetPos = aCharSetPos;
        }
    }
    else
    {
        sTypeName = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    
    return sTypeName;
}


/**
 * @brief BINARY형 데이타 타입 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeLen         Node Length
 * @param[in]   aDBType          DB Type
 * @param[in]   aDBTypePos       position of DB Type
 * @param[in]   aBinaryLen       Binary String Length
 *
 * @return qlpTypeName node
 */
inline qlpTypeName * qlpMakeBinaryTypeName( stlParseParam  * aParam,
                                            stlInt32         aNodePos,
                                            stlInt32         aNodeLen,
                                            dtlDataType      aDBType,
                                            stlInt32         aDBTypePos,
                                            qlpValue       * aBinaryLen )
{
    qlpTypeName  * sTypeName = NULL;

    if( ( aDBType == DTL_TYPE_BINARY ) ||
        ( aDBType == DTL_TYPE_VARBINARY ) ||
        ( aDBType == DTL_TYPE_LONGVARBINARY ) )
    {
        if( aParam->mAllocator( aParam->mContext,
                                STL_SIZEOF( qlpTypeName ),
                                (void**) & sTypeName,
                                aParam->mErrorStack )
            != STL_SUCCESS )
        {
            sTypeName = NULL;
            aParam->mErrStatus = STL_FAILURE;
        }
        else
        {
            sTypeName->mType         = QLL_TYPE_NAME_TYPE;
            sTypeName->mNodePos      = aNodePos;
            sTypeName->mNodeLen      = aNodeLen;
            sTypeName->mDBType       = aDBType;
            sTypeName->mDBTypePos    = aDBTypePos;
            sTypeName->mBinaryLength = aBinaryLen;
        }
    }
    else
    {
        sTypeName = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    
    return sTypeName;
}


/**
 * @brief NUMERIC형 데이타 타입 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeLen         Node Length
 * @param[in]   aDBType          DB Type
 * @param[in]   aDBTypePos       position of DB Type
 * @param[in]   aRadix           Radix
 * @param[in]   aPrecision       Precision
 * @param[in]   aScale           Scale
 *
 * @return qlpTypeName node
 */
inline qlpTypeName * qlpMakeNumericTypeName( stlParseParam  * aParam,
                                             stlInt32         aNodePos,
                                             stlInt32         aNodeLen,
                                             dtlDataType      aDBType,
                                             stlInt32         aDBTypePos,
                                             stlInt32         aRadix,
                                             qlpValue       * aPrecision,
                                             qlpValue       * aScale )
{
    qlpTypeName  * sTypeName = NULL;

    if( ( aDBType == DTL_TYPE_NUMBER ) ||
        ( aDBType == DTL_TYPE_FLOAT ) )
    {
        if( aParam->mAllocator( aParam->mContext,
                                STL_SIZEOF( qlpTypeName ),
                                (void**) & sTypeName,
                                aParam->mErrorStack )
            != STL_SUCCESS )
        {
            sTypeName = NULL;
            aParam->mErrStatus = STL_FAILURE;
        }
        else
        {
            sTypeName->mType      = QLL_TYPE_NAME_TYPE;
            sTypeName->mNodePos   = aNodePos;
            sTypeName->mNodeLen   = aNodeLen;
            sTypeName->mDBType    = aDBType;
            sTypeName->mDBTypePos = aDBTypePos;

            sTypeName->mNumericPrecRadix = aRadix;
            sTypeName->mNumericPrecision = aPrecision;
            sTypeName->mNumericScale     = aScale;
        }
    }
    else
    {
        sTypeName = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    
    return sTypeName;    
}

/**
 * @brief DataTime형 데이타 타입 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeLen         Node Length
 * @param[in]   aDBType          DB Type
 * @param[in]   aDBTypePos       position of DB Type
 * @param[in]   aSecondPrec      Second Precision
 *
 * @return qlpTypeName node
 */
inline qlpTypeName * qlpMakeDateTimeTypeName( stlParseParam  * aParam,
                                              stlInt32         aNodePos,
                                              stlInt32         aNodeLen,
                                              dtlDataType      aDBType,
                                              stlInt32         aDBTypePos,
                                              qlpValue       * aSecondPrec )
{
    qlpTypeName  * sTypeName = NULL;

    if( ( aDBType == DTL_TYPE_DATE ) ||
        ( aDBType == DTL_TYPE_TIME ) ||
        ( aDBType == DTL_TYPE_TIMESTAMP ) ||
        ( aDBType == DTL_TYPE_TIME_WITH_TIME_ZONE ) ||
        ( aDBType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ) ||
        ( aDBType == DTL_TYPE_LONGVARBINARY ) )
    {
        if( aParam->mAllocator( aParam->mContext,
                                STL_SIZEOF( qlpTypeName ),
                                (void**) & sTypeName,
                                aParam->mErrorStack )
            != STL_SUCCESS )
        {
            sTypeName = NULL;
            aParam->mErrStatus = STL_FAILURE;
        }
        else
        {
            sTypeName->mType      = QLL_TYPE_NAME_TYPE;
            sTypeName->mNodePos   = aNodePos;
            sTypeName->mNodeLen   = aNodeLen;
            sTypeName->mDBType    = aDBType;
            sTypeName->mDBTypePos = aDBTypePos;

            sTypeName->mFractionalSecondPrec = aSecondPrec;
        }
    }
    else
    {
        sTypeName = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    
    return sTypeName;    
}


/**
 * @brief Interval형 데이타 타입 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeLen         Node Length
 * @param[in]   aDBTypePos       position of DB Type
 * @param[in]   aQualifier       Interval Qualifier
 *
 * @return qlpTypeName node
 * 
 * @todo code 완성하기
 */
inline qlpTypeName * qlpMakeIntervalTypeName( stlParseParam  * aParam,
                                              stlInt32         aNodePos,
                                              stlInt32         aNodeLen,
                                              stlInt32         aDBTypePos,
                                              qllNode        * aQualifier )
{
    qlpTypeName  * sTypeName = NULL;
    qlpQualifier * sQulifier = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpTypeName ),
                            (void**) & sTypeName,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sTypeName = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sTypeName->mType      = QLL_TYPE_NAME_TYPE;
        sTypeName->mNodePos   = aNodePos;
        sTypeName->mNodeLen   = aNodeLen;
        sTypeName->mDBTypePos = aDBTypePos;

        sTypeName->mQualifier = aQualifier;

        sQulifier = (qlpQualifier *)sTypeName->mQualifier;

        if( sQulifier->mSingleFieldDesc != NULL )
        {
            switch( (QLP_GET_INT_VALUE(sQulifier->mSingleFieldDesc->mPrimaryFieldType)) )
            {
                case DTL_INTERVAL_INDICATOR_YEAR:
                case DTL_INTERVAL_INDICATOR_MONTH:
                    sTypeName->mDBType = DTL_TYPE_INTERVAL_YEAR_TO_MONTH;
                    break;
                default:
                    sTypeName->mDBType = DTL_TYPE_INTERVAL_DAY_TO_SECOND;
                    break;
            }
        }
        else
        {
            if( sQulifier->mStartFieldDesc != NULL )
            {
                switch( (QLP_GET_INT_VALUE(sQulifier->mStartFieldDesc->mPrimaryFieldType)) )
                {
                    case DTL_INTERVAL_INDICATOR_YEAR:
                    case DTL_INTERVAL_INDICATOR_MONTH:
                        sTypeName->mDBType = DTL_TYPE_INTERVAL_YEAR_TO_MONTH;
                        break;
                    default:
                        sTypeName->mDBType = DTL_TYPE_INTERVAL_DAY_TO_SECOND;
                        break;
                }
            }
            else
            {
                sTypeName = NULL;
                aParam->mErrStatus = STL_FAILURE;
            }
        }
    }

    return sTypeName;    
}

/**
 * @brief date field string literal 노드 생성
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aStringBuf          Input buffer literal
 * @param[in]   aLowStringBuf       LowString  buffer literal
 * @param[out]  aResultBuf          Result buffer literal
 * 
 * @return qlpValue node
 */
inline qlpValue * qlpMakeFieldStringConstant( stlParseParam              * aParam,
                                              stlInt32                     aNodePos,
                                              stlChar                    * aStringBuf,
                                              stlChar                    * aLowStringBuf,
                                              stlChar                    * aResultBuf )
{
    qlpValue   * sValue = NULL;
    stlInt32     sStrLen = 0;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpValue ),
                            (void**) & sValue,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sValue = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sValue->mType           = QLL_BNF_NUMBER_CONSTANT_TYPE;
        sValue->mNodePos        = aNodePos;

        stlMemset( aResultBuf, 0x00, 2 );
        
        if( aStringBuf == NULL )
        {
            sStrLen = 0;
            aLowStringBuf = '\0';
        }
        else
        {
            sStrLen = stlStrlen( aStringBuf );
            stlTolowerString( aStringBuf, aLowStringBuf );
        }

        switch( sStrLen )
        {
            case 3 :
                if( stlStrncmp( aLowStringBuf, "day", sStrLen ) == 0 )
                {
                    aResultBuf[0] = DTL_DATETIME_FIELD_DAY + '0';
                    break;
                }

            case 4 :
                if( stlStrncmp( aLowStringBuf, "year", sStrLen ) == 0 )
                {
                    aResultBuf[0] = DTL_DATETIME_FIELD_YEAR + '0';
                    break;
                }
                else if( stlStrncmp( aLowStringBuf, "hour", sStrLen ) == 0 )
                {
                    aResultBuf[0] = DTL_DATETIME_FIELD_HOUR + '0';
                    break;
                }
            case 5 :
                if( stlStrncmp( aLowStringBuf, "month", sStrLen ) == 0 )
                {
                    aResultBuf[0] = DTL_DATETIME_FIELD_MONTH + '0';
                    break;
                }
            case 6 :
                if( stlStrncmp( aLowStringBuf, "minute", sStrLen ) == 0 )
                {
                    aResultBuf[0] = DTL_DATETIME_FIELD_MINUTE + '0';
                    break;
                }
                else if( stlStrncmp( aLowStringBuf, "second", sStrLen ) == 0 )
                {
                    aResultBuf[0] = DTL_DATETIME_FIELD_SECOND + '0';
                    break;
                }
                
            case 13 :
                if( stlStrncmp( aLowStringBuf, "timezone_hour", sStrLen ) == 0 )
                { 
                    aResultBuf[0] = DTL_DATETIME_FIELD_TIMEZONE_HOUR + '0';
                    break;
                }

            case 15 :
                if( stlStrncmp( aLowStringBuf, "timezone_minute", sStrLen ) == 0 )
                {
                    aResultBuf[0] = DTL_DATETIME_FIELD_TIMEZONE_MINUTE + '0';
                    break;
                }

            default :
                STL_THROW( RAMP_ERR_DATETIME_FIELD );
                break;
        }
        aResultBuf[1] = '\0';
        sValue->mValue.mString = aResultBuf;
    }
    return sValue;
    
    STL_CATCH( RAMP_ERR_DATETIME_FIELD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYNTAX_ERROR,
                      qlgMakeErrPosString( aParam->mBuffer,
                                           aNodePos,
                                           QLP_PARSE_GET_ENV( aParam ) ),
                      aParam->mErrorStack );
    }

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;

    return sValue;
}

/**
 * @brief interval qualifier 노드 생성
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aStartFieldDesc     Start Field
 * @param[in]   aEndFieldDesc       End Field
 * @param[in]   aSingleFieldDesc    Single Datetime Field
 *
 * @return qlpQualifier node
 */
inline qlpQualifier * qlpMakeQualifier( stlParseParam         * aParam,
                                        stlInt32                aNodePos,
                                        qlpIntvPriFieldDesc   * aStartFieldDesc,
                                        qlpIntvPriFieldDesc   * aEndFieldDesc,
                                        qlpIntvPriFieldDesc   * aSingleFieldDesc )
{
    qlpQualifier  * sQualifier = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpQualifier ),
                            (void**) & sQualifier,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sQualifier = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sQualifier->mType               = QLL_QUALIFIER_TYPE;
        sQualifier->mNodePos            = aNodePos;

        sQualifier->mStartFieldDesc     = aStartFieldDesc;
        sQualifier->mEndFieldDesc       = aEndFieldDesc;
        sQualifier->mSingleFieldDesc    = aSingleFieldDesc;
    }

    return sQualifier;
}



/**
 * @brief interval primary datetime field description 노드 생성
 * @param[in]   aParam                          Parse Parameter
 * @param[in]   aNodePos                        Node Position
 * @param[in]   aPrimaryFieldType               Primary Datetime Field Type (dtlIntervalPrimaryField 사용)
 * @param[in]   aLeadingFieldPrecision          Interval Leading Field Precision
 * @param[in]   aFractionalSecondsPrecision     Interval Fractional Seconds Precision
 *
 * @return qlpIntvPriFieldDesc node
 */
inline qlpIntvPriFieldDesc * qlpMakeIntvPriFieldDesc( stlParseParam   * aParam,
                                                      stlInt32          aNodePos,
                                                      qlpValue        * aPrimaryFieldType,
                                                      qlpValue        * aLeadingFieldPrecision,
                                                      qlpValue        * aFractionalSecondsPrecision )
{
    qlpIntvPriFieldDesc  * sIntvPriFieldDesc = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpIntvPriFieldDesc ),
                            (void**) & sIntvPriFieldDesc,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sIntvPriFieldDesc = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sIntvPriFieldDesc->mType                        = QLL_INTERVAL_DESC_TYPE;
        sIntvPriFieldDesc->mNodePos                     = aNodePos;

        sIntvPriFieldDesc->mPrimaryFieldType            = aPrimaryFieldType;
        sIntvPriFieldDesc->mLeadingFieldPrecision       = aLeadingFieldPrecision;
        sIntvPriFieldDesc->mFractionalSecondsPrecision  = aFractionalSecondsPrecision;
    }

    return sIntvPriFieldDesc;
}



/**
 * @brief constant value 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aValue           qlpValue node
 * 
 * @return qlpConstantValue node
 */
inline qlpConstantValue * qlpMakeConstant( stlParseParam  * aParam,
                                           stlInt32         aNodePos,
                                           qlpValue       * aValue )
{
    qlpConstantValue  * sConstant = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpConstantValue ),
                            (void**) & sConstant,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sConstant = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sConstant->mType    = QLL_BNF_CONSTANT_TYPE;
        sConstant->mNodePos = aNodePos;
        sConstant->mValue   = aValue;
    }

    return sConstant;
}


/**
 * @brief CAST expression 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aExpr            the expression being casted
 * @param[in]   aTypeName        type name
 * 
 * @return qlpTypeCast node
 */
inline qlpTypeCast * qlpMakeTypeCast( stlParseParam  * aParam,
                                      stlInt32         aNodePos,
                                      qllNode        * aExpr,
                                      qlpTypeName    * aTypeName )
{
    qlpTypeCast  * sCast = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpTypeCast ),
                            (void**) & sCast,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sCast = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sCast->mType     = QLL_TYPECAST_TYPE;
        sCast->mNodePos  = aNodePos;
        sCast->mExpr     = aExpr;
        sCast->mTypeName = aTypeName;
    }

    return sCast;
}


/**
 * @brief element definition 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aElementType     element 종류
 * @param[in]   aArgument        element의 argument
 * 
 * @return DefElem node
 */
inline qlpDefElem * qlpMakeDefElem( stlParseParam  * aParam,
                                    stlInt32         aNodePos,
                                    stlInt32         aElementType,
                                    qllNode        * aArgument )
{
    qlpDefElem  * sDefElem = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDefElem ),
                            (void**) & sDefElem,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDefElem = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDefElem->mType        = QLL_DEF_ELEM_TYPE;
        sDefElem->mNodePos     = aNodePos;
        sDefElem->mElementType = aElementType;
        sDefElem->mArgument    = aArgument;
    }

    return sDefElem;
}


/**
 * @brief element의 argument 얻기 
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aElement         element 
 * 
 * @return qllNode node
 */
inline qllNode * qlpGetElemArgument( stlParseParam  * aParam,
                                     qlpDefElem     * aElement )
{
    return (qllNode*) aElement->mArgument;
}


/**
 * @brief target 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aName            Name of target 
 * @param[in]   aNamePos         position of Name of target
 * @param[in]   aDestSubscripts  destination subscripts (for INSERT, UPDATE)
 * @param[in]   aExpr            expression of target
 * @param[in]   aUpdateColumnRef update set 절의 update column
 *
 * @return qlpTarget node
 */
inline qlpTarget * qlpMakeTarget( stlParseParam  * aParam,
                                  stlInt32         aNodePos,
                                  stlChar        * aName,
                                  stlInt32         aNamePos,
                                  qlpList        * aDestSubscripts,
                                  qllNode        * aExpr,
                                  qlpColumnRef   * aUpdateColumnRef )
{
    qlpTarget  * sTarget = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpTarget ),
                            (void**) & sTarget,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sTarget = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sTarget->mType           = QLL_TARGET_TYPE;
        sTarget->mNodePos        = aNodePos;
        sTarget->mName           = aName;
        sTarget->mNamePos        = aNamePos;
        sTarget->mDestSubscripts = aDestSubscripts;
        sTarget->mRelName        = NULL;
        sTarget->mColumnNameList = NULL;
        sTarget->mExpr           = aExpr;
        sTarget->mUpdateColumn   = aUpdateColumnRef;
    }

    return sTarget;
}


/**
 * @brief target asterisk 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aName            Name of target 
 * @param[in]   aNamePos         position of Name of target
 * @param[in]   aDestSubscripts  identifier chain (for SELECT), destination subscripts (for INSERT, UPDATE)
 * @param[in]   aRelName         relation name (for SELECT)
 * @param[in]   aColumnNameList  column name list (for SELECT)
 * @param[in]   aExpr            expression of target
 *
 * @return qlpTarget node
 */
inline qlpTarget * qlpMakeTargetAsterisk( stlParseParam  * aParam,
                                          stlInt32         aNodePos,
                                          stlChar        * aName,
                                          stlInt32         aNamePos,
                                          qlpList        * aDestSubscripts,
                                          qlpList        * aRelName,
                                          qlpList        * aColumnNameList,
                                          qllNode        * aExpr )
{
    qlpTarget  * sTarget = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpTarget ),
                            (void**) & sTarget,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sTarget = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sTarget->mType           = QLL_TARGET_ASTERISK_TYPE;
        sTarget->mNodePos        = aNodePos;
        sTarget->mName           = aName;
        sTarget->mNamePos        = aNamePos;
        sTarget->mDestSubscripts = aDestSubscripts;
        if( aRelName == NULL )
        {
            sTarget->mRelName = NULL;
        }
        else
        {
            sTarget->mRelName = qlpMakeObjectName( aParam, aRelName );
        }
        sTarget->mColumnNameList = aColumnNameList;
        sTarget->mExpr           = aExpr;
    }

    return sTarget;
}

    
/**
 * @brief column reference 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aFields          field name
 * @param[in]   aIsSetOuterMark  Outer Join Operator가 설정되었는지 여부
 *
 * @return qlpColumnRef node
 */
inline qlpColumnRef * qlpMakeColumnRef( stlParseParam  * aParam,
                                        stlInt32         aNodePos,
                                        qlpList        * aFields,
                                        stlBool          aIsSetOuterMark )
{
    qlpColumnRef  * sColRef = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpColumnRef ),
                            (void**) & sColRef,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sColRef = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sColRef->mType              = QLL_COLUMN_REF_TYPE;
        sColRef->mNodePos           = aNodePos;
        sColRef->mName              = qlpMakeColumnName( aParam, aFields );
        sColRef->mIsSetOuterMark    = aIsSetOuterMark;
    }

    return sColRef;
}


/**
 * @brief index scan 노드 생성
 * @param[in]   aParam       Parse Parameter
 * @param[in]   aNodePos     Node Position
 * @param[in]   aName        Name of Index
 *
 * @return qlpIndexScan node
 */
inline qlpIndexScan * qlpMakeIndexScan( stlParseParam  * aParam,
                                        stlInt32         aNodePos,
                                        qlpList        * aName )
{
    qlpIndexScan  * sIndexScan = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpIndexScan ),
                            (void**) & sIndexScan,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sIndexScan = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sIndexScan->mType        = QLL_INDEX_SCAN_TYPE;
        sIndexScan->mNodePos     = aNodePos;
        sIndexScan->mName        = qlpMakeObjectName( aParam, aName );
    }

    return sIndexScan;
}


/**
 * @brief index scan 노드 생성
 * @param[in]   aParam         Parse Parameter
 * @param[in]   aNodePos       Node Position
 * @param[in]   aName          Index Column Name 
 * @param[in]   aIsAsc         is ascending sort order ?
 * @param[in]   aIsNullsFirst  is nulls first ? 
 * @param[in]   aIsNullable    nullable ? 
 *
 * @return qlpIndexElement node
 */
inline qlpIndexElement * qlpMakeIndexElement( stlParseParam  * aParam,
                                              stlInt32         aNodePos,
                                              qlpValue       * aName,
                                              qlpValue       * aIsAsc,
                                              qlpValue       * aIsNullsFirst,
                                              qlpValue       * aIsNullable )
{
    qlpIndexElement  * sIndexElem = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpIndexElement ),
                            (void**) & sIndexElem,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sIndexElem = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sIndexElem->mType         = QLL_INDEX_ELEMENT_TYPE;
        sIndexElem->mNodePos      = aNodePos;
        sIndexElem->mColumnName   = aName;
        sIndexElem->mIsAsc        = aIsAsc;
        sIndexElem->mIsNullsFirst = aIsNullsFirst;
        sIndexElem->mIsNullable   = aIsNullable;
    }

    return sIndexElem;
}


/**
 * @brief parameter integer constant 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aInteger         integer value
 *
 * @return qlpValue node
 * @remark parser에서 integer로 구성되는 경우
 *    <BR> 1. Boolean Type Value (DB type에서는 유일)
 *    <BR> 2. Validation에 사용될 미리 정해지 값들
 *    <BR>
 *    <BR> cf. 정수형 literal을 integer value로 정하지 않는 이유
 *    <BR> => 값이 Integer 범위를 넘을 수 있기 때문에.
 */
inline qlpValue * qlpMakeIntParamConstant( stlParseParam  * aParam,
                                           stlInt32         aNodePos,
                                           stlInt64         aInteger )
{
    qlpValue  * sValue = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpValue ),
                            (void**) & sValue,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sValue = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sValue->mType           = QLL_BNF_INTPARAM_CONSTANT_TYPE;
        sValue->mNodePos        = aNodePos;
        sValue->mValue.mInteger = aInteger;
    }

    return sValue;
}


/**
 * @brief string constant 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aString          string value 
 *
 * @return qlpValue node
 */
inline qlpValue * qlpMakeStrConstant( stlParseParam  * aParam,
                                      stlInt32         aNodePos,
                                      stlChar        * aString )
{
    qlpValue  * sValue = NULL;

    STL_DASSERT( aString != NULL );
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpValue ),
                            (void**) & sValue,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sValue = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sValue->mType          = QLL_BNF_STRING_CONSTANT_TYPE;
        sValue->mNodePos       = aNodePos;
        sValue->mValue.mString = aString;
    }
    
    return sValue;
}


/**
 * @brief sensitive string constant 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aStringValue     string value node
 * @return qlpValue node
 */
inline qlpValue * qlpMakeSensitiveString( stlParseParam  * aParam,
                                          qlpValue       * aStringValue )
{
    qlpValue  * sValue = NULL;
    stlInt32    sStrLen = 0;

    STL_DASSERT( aStringValue->mValue.mString != NULL );

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpValue ),
                            (void**) & sValue,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sValue = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sStrLen = stlStrlen( aStringValue->mValue.mString );
        if ( aParam->mAllocator( aParam->mContext,
                                 sStrLen + 1,
                                 (void**) & sValue->mValue.mString,
                                 aParam->mErrorStack )
             != STL_SUCCESS )
        {
            sValue = NULL;
            aParam->mErrStatus = STL_FAILURE;
        }
        else
        {
            STL_ASSERT( aStringValue->mType == QLL_BNF_STRING_CONSTANT_TYPE );
            
            sValue->mType          = QLL_BNF_STRING_CONSTANT_TYPE;
            sValue->mNodePos       = aStringValue->mNodePos;
            stlMemcpy( sValue->mValue.mString,
                       aParam->mBuffer + aStringValue->mNodePos,
                       sStrLen );
            sValue->mValue.mString[sStrLen] = '\0';
        }
    }
    
    return sValue;
}

/**
 * @brief number leteral constant value 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNumericStr      number literal
 *
 * @return qlpValue node
 */
inline qlpValue * qlpMakeNumberConstant( stlParseParam  * aParam,
                                         stlInt32         aNodePos,
                                         stlChar        * aNumericStr )
{
    qlpValue  * sValue = NULL;

    STL_DASSERT( aNumericStr != NULL );

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpValue ),
                            (void**) & sValue,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sValue = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sValue->mType          = QLL_BNF_NUMBER_CONSTANT_TYPE;
        sValue->mNodePos       = aNodePos;
        sValue->mValue.mString = aNumericStr;
    }

    return sValue;
}


/**
 * @brief bit(or hex) string constant 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aBitStr          bit string value 
 *
 * @return qlpValue node
 */
inline qlpValue * qlpMakeBitStrConstant( stlParseParam  * aParam,
                                         stlInt32         aNodePos,
                                         stlChar        * aBitStr )
{
    qlpValue  * sValue = NULL;

    STL_DASSERT( aBitStr != NULL );

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpValue ),
                            (void**) & sValue,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sValue = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sValue->mType          = QLL_BNF_BITSTR_CONSTANT_TYPE;
        sValue->mNodePos       = aNodePos;
        sValue->mValue.mString = aBitStr;
    }
    
    return sValue;
}


/**
 * @brief NULL constant 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 *
 * @return qlpValue node
 */
inline qlpValue * qlpMakeNullConstant( stlParseParam  * aParam,
                                       stlInt32         aNodePos )
{
    qlpValue  * sValue = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpValue ),
                            (void**) & sValue,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sValue = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sValue->mType          = QLL_BNF_NULL_CONSTANT_TYPE;
        sValue->mNodePos       = aNodePos;
        sValue->mValue.mString = NULL;
    }

    return sValue;
}

/**
 * @brief datetime literal constant 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aConstantType    Constant Type
 * @param[in]   aString          string value 
 *
 * @return qlpValue node
 */
inline qlpValue * qlpMakeDateTimeConstant( stlParseParam  * aParam,
                                           stlInt32         aNodePos,
                                           qllNodeType      aConstantType,
                                           stlChar        * aString )
{
    qlpValue  * sValue = NULL;

    STL_DASSERT( aString != NULL );
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpValue ),
                            (void**) & sValue,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sValue = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        if( aString[0] == '\0' )
        {
            /*
             * ex) DATE'' => error
             */
            STL_THROW( RAMP_DATETIME_SYNTAX_ERROR );
        }
        else
        {
            sValue->mType          = aConstantType;
            sValue->mNodePos       = aNodePos;
            sValue->mValue.mString = aString;
        }
    }

    return sValue;
    
    STL_CATCH( RAMP_DATETIME_SYNTAX_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYNTAX_ERROR,
                      qlgMakeErrPosString( aParam->mBuffer,
                                           aNodePos,
                                           QLP_PARSE_GET_ENV( aParam ) ),
                      aParam->mErrorStack );
    }

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    
    return sValue;
}

/**
 * @brief interval literal empty string 검사
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aString          string value
 *
 * @return 없음
 */
inline void qlpCheckIntervalLiteralEmptyString( stlParseParam  * aParam,
                                                stlInt32         aNodePos,
                                                stlChar        * aString )
{
    STL_DASSERT( aString != NULL );
    
    if( aString[0] == '\0' )
    {
        /*
         * ex) INTERVAL'' => error
         */
        STL_THROW( RAMP_DATETIME_SYNTAX_ERROR );
    }
    else
    {
        /* Do Nothing */
    }
    
    return;
    
    STL_CATCH( RAMP_DATETIME_SYNTAX_ERROR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYNTAX_ERROR,
                      qlgMakeErrPosString( aParam->mBuffer,
                                           aNodePos,
                                           QLP_PARSE_GET_ENV( aParam ) ),
                      aParam->mErrorStack );
    }

    STL_FINISH;
    
    aParam->mErrStatus = STL_FAILURE;
    
    return;
}

/**
 * @brief column definition second info 노드 생성
 * @param[in]   aParam             Parse Parameter
 * @param[in]   aNodePos           Node Position
 * @param[in]   aRawDefault        default value : untransformed parse tree
 * @param[in]   aIdentType         type of identity generation
 * @param[in]   aIdentTypePos      position : type of identity generation
 * @param[in]   aIdentSeqOptions   sequence options for identity
 * @param[in]   aGenerationExpr    expression of generation
 *
 * @return qlpColDefSecond node
 */
inline qlpColDefSecond * qlpMakeColDefSecond( stlParseParam      * aParam,
                                              stlInt32             aNodePos,
                                              qllNode            * aRawDefault,
                                              qlpIdentityType      aIdentType,
                                              stlInt32             aIdentTypePos,
                                              qlpSequenceOption  * aIdentSeqOptions,
                                              qllNode            * aGenerationExpr )
{
    qlpColDefSecond  * sSecond = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpColDefSecond ),
                            (void**) & sSecond,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSecond = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sSecond->mType            = QLL_COL_DEF_SECOND_TYPE;
        sSecond->mNodePos         = aNodePos;
        sSecond->mRawDefault      = aRawDefault;
        sSecond->mIdentType       = aIdentType;
        sSecond->mIdentTypePos    = aIdentTypePos;
        sSecond->mIdentSeqOptions = aIdentSeqOptions;
        sSecond->mGenerationExpr  = aGenerationExpr;
    }

    return sSecond;
}


/**
 * @brief datafile 노드 생성
 * @param[in]   aParam        Parse Parameter
 * @param[in]   aNodePos      Node Position
 * @param[in]   aFilePath     file path
 * @param[in]   aSize         file size
 * @param[in]   aReuse        Reuse 여부
 * @param[in]   aAutoExtend   AutoExtend Clause
 *
 * @return qlpDatafile node
 */
inline qlpDatafile * qlpMakeDatafile( stlParseParam  * aParam,
                                      stlInt32         aNodePos,
                                      qlpValue       * aFilePath,
                                      qlpSize        * aSize,
                                      qlpValue       * aReuse,
                                      qlpAutoExtend  * aAutoExtend )
{
    qlpDatafile     * sDatafile = NULL;
 
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDatafile ),
                            (void**) & sDatafile,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDatafile = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDatafile->mType       = QLL_DATAFILE_TYPE;
        sDatafile->mNodePos    = aNodePos;
        sDatafile->mFilePath   = aFilePath;
        sDatafile->mInitSize   = aSize;
        sDatafile->mReuse      = aReuse;
        sDatafile->mAutoExtend = aAutoExtend;
    }
    
    return sDatafile;
}


/**
 * @brief Datafile의 autoextend 속성 노드 생성
 * @param[in]   aParam       Parse Parameter
 * @param[in]   aNodePos     Node Position
 * @param[in]   aIsAutoOn      Is AutoExtend
 * @param[in]   aNextSize    Next Size
 * @param[in]   aMaxSize     Max Size
 *
 * @return qlpAutoExtend
 */
inline qlpAutoExtend * qlpMakeAutoExtend( stlParseParam    * aParam,
                                          stlInt32           aNodePos,
                                          qlpValue         * aIsAutoOn,
                                          qlpSize          * aNextSize,
                                          qlpSize          * aMaxSize )
{
    qlpAutoExtend  * sAuto = NULL;
 
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAutoExtend ),
                            (void**) & sAuto,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAuto = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAuto->mType     = QLL_AUTO_EXTEND_TYPE;
        sAuto->mNodePos  = aNodePos;
        sAuto->mIsAutoOn = aIsAutoOn;
        sAuto->mNextSize = aNextSize;
        sAuto->mMaxSize  = aMaxSize;
    }

    return sAuto;
}

/**
 * @brief tablespace attribute 설정
 * @param[in]   aParam       Parse Parameter
 * @param[in]   aTbsAttr     tablespace attribute
 * @param[in]   aValue       속성 값
 *
 * @return 없음
 */
inline void qlpSetTablespaceAttr( stlParseParam      * aParam,
                                  qlpTablespaceAttr  * aTbsAttr,
                                  qlpList            * aValue )
{
    qlpListElement * sElem  = QLP_LIST_GET_FIRST( aValue );
    qlpValue       * sValue = QLP_LIST_GET_POINTER_VALUE( sElem );

    switch( QLP_GET_INT_VALUE( sValue ) )
    {
        case QLP_TABLESPACE_ATTR_LOGGING :
            {
                if( aTbsAttr->mIsSetting[QLP_TABLESPACE_ATTR_LOGGING] == STL_FALSE )
                {
                    aTbsAttr->mIsSetting[QLP_TABLESPACE_ATTR_LOGGING] = STL_TRUE;
                    aTbsAttr->mIsLogging =
                        QLP_LIST_GET_POINTER_VALUE( QLP_LIST_GET_NEXT_ELEM( sElem ) );
                }
                else
                {
                    aParam->mErrStatus = STL_FAILURE;
                }
                break;
            }
        case QLP_TABLESPACE_ATTR_ONLINE :
            {
                if( aTbsAttr->mIsSetting[QLP_TABLESPACE_ATTR_ONLINE] == STL_FALSE )
                {
                    aTbsAttr->mIsSetting[QLP_TABLESPACE_ATTR_ONLINE] = STL_TRUE;
                    aTbsAttr->mIsOnline =
                        QLP_LIST_GET_POINTER_VALUE( QLP_LIST_GET_NEXT_ELEM( sElem ) );
                }
                else
                {
                    aParam->mErrStatus = STL_FAILURE;
                }
                break;
            }
        case QLP_TABLESPACE_ATTR_EXTSIZE :
            {
                if( aTbsAttr->mIsSetting[QLP_TABLESPACE_ATTR_EXTSIZE] == STL_FALSE )
                {
                    aTbsAttr->mIsSetting[QLP_TABLESPACE_ATTR_EXTSIZE] = STL_TRUE;
                    aTbsAttr->mExtSize =
                        QLP_LIST_GET_POINTER_VALUE( QLP_LIST_GET_NEXT_ELEM( sElem ) );
                }
                else
                {
                    aParam->mErrStatus = STL_FAILURE;
                }
                break;
            }
        default:
            {
                aParam->mErrStatus = STL_FAILURE;
                break;
            }
    }
}


/**
 * @brief Sequence definition option 노드 구성 
 * @param[in]   aParam        Parse Parameter
 * @param[in]   aOptions      Sequence Option List
 *
 * @return qlpSequenceOption node
 */
inline qlpSequenceOption * qlpMakeSequenceOption( stlParseParam   * aParam,
                                                  qlpList         * aOptions )
{
    qlpSequenceOption      * sOptions  = NULL;
    qlpListElement         * sListElem = QLP_LIST_GET_FIRST( aOptions );
    qlpDefElem             * sElement  = NULL;
    qllNode                * sArgument = NULL;
    qlpSequenceOptionType    sSeqType;  
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpSequenceOption ),
                            (void**) & sOptions,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sOptions = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sOptions->mType = QLL_SEQUENCE_OPTION_TYPE;

        while( ( sListElem != NULL ) && ( aParam->mErrStatus == STL_SUCCESS ) )
        {
            sElement  = (qlpDefElem*) QLP_LIST_GET_POINTER_VALUE( sListElem );
            sSeqType  = sElement->mElementType;
            sArgument = qlpGetElemArgument( aParam, sElement );
            
            if( sOptions->mIsSetting[ sSeqType ] == STL_FALSE )
            {
                sOptions->mIsSetting[ sSeqType ] = STL_TRUE;
                
                switch( sSeqType )
                {
                    case QLP_SEQUENCE_OPTION_CYCLE :
                        {
                            sOptions->mCycle = (qlpValue*) sArgument;
                            break;
                        }
                    case QLP_SEQUENCE_OPTION_START_WITH :
                        {
                            sOptions->mStartWith = (qlpValue*) sArgument;
                            break;
                        }
                    case QLP_SEQUENCE_OPTION_INCREMENT :
                        {
                            sOptions->mIncrement = (qlpValue*) sArgument;
                            break;
                        }
                    case QLP_SEQUENCE_OPTION_MAXVALUE :
                        {
                            sOptions->mMaxValue = (qlpValue*) sArgument;
                            break;
                        }
                    case QLP_SEQUENCE_OPTION_MINVALUE :
                        {
                            sOptions->mMinValue = (qlpValue*) sArgument;
                            break;
                        }
                    case QLP_SEQUENCE_OPTION_CACHE :
                        {
                            if( sArgument->mType == QLL_BNF_INTPARAM_CONSTANT_TYPE )
                            {
                                /* No Cache */
                                sOptions->mCache    = NULL;
                                sOptions->mNoCache  = (qlpValue*) sArgument;
                            }
                            else
                            {
                                /* No Cache */
                                sOptions->mCache    = (qlpValue*) sArgument;
                                sOptions->mNoCache  = NULL;
                            }
                            break;
                        }
                    case QLP_SEQUENCE_OPTION_RESTART_WITH :
                        {
                            sOptions->mRestartNodePos = sElement->mNodePos;
                            sOptions->mRestartWith = (qlpValue*) sArgument;
                            break;
                        }
                    default :
                        {
                            aParam->mErrStatus = STL_FAILURE;
                            break;
                        }
                }
            
                sListElem = QLP_LIST_GET_NEXT_ELEM( sListElem );
            }
            else
            {
                sOptions = NULL;
                aParam->mErrStatus = STL_FAILURE;
                
                break;
            }
        }
    }

    return sOptions;
}


/**
 * @brief tablespace attribute 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 *
 * @return qlpTablespaceAttr 노드
 *
 * @remark 노드의 attribute 값은 qlpSetTablespaceAttr에 의해 설정됨
 */
inline qlpTablespaceAttr * qlpMakeTablespaceAttr( stlParseParam      * aParam,
                                                  stlInt32             aNodePos )
{
    qlpTablespaceAttr  * sAttr = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpTablespaceAttr ),
                            (void**) & sAttr,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAttr = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAttr->mType    = QLL_TABLESPACE_ATTR_TYPE;
        sAttr->mNodePos = aNodePos;
    }

    return sAttr;
}


/**
 * @brief Parameter 리스트 구성
 * @param[in]   aParam        Parse Parameter
 * @param[in]   aParameter    Parameter Node
 *
 * @return 없음
 */
inline void qlpAddParamList( stlParseParam    * aParam,
                             qlpParameter     * aParameter )
{
    qlpList  * sList = QLP_PARSE_GET_PARAM( aParam );

    if( sList == NULL )
    {
        sList = qlpMakeList( aParam, QLL_POINTER_LIST_TYPE );
    }
    else
    {
        /* Do Nothing */
    }

    aParameter->mParamId = sList->mCount;
    qlpAddPtrValueToList( aParam,
                          sList,
                          aParameter );
    
    QLP_PARSE_GET_PARAM( aParam ) = sList;
}


/**
 * @brief Object Name 얻기 
 * @param[in]   aParam       Parse Parameter
 * @param[in]   aName        Object Name 
 *
 * @return qlpObjectName
 */
inline qlpObjectName * qlpMakeObjectName( stlParseParam  * aParam,
                                          qlpList        * aName )
{
    qlpObjectName   * sObjectName = NULL;
    qlpListElement  * sElem       = NULL;
    qlpValue        * sValue      = NULL;
    qlpList         * sObjectList = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpObjectName ),
                            (void**) & sObjectName,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sObjectName = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sObjectName->mType    = QLL_OBJECT_NAME_TYPE;

        sElem = QLP_LIST_GET_FIRST( aName );

        switch( aName->mCount )
        {
            /* case 3 : */
            /*     { */
            /*         sValue = QLP_LIST_GET_POINTER_VALUE( sElem ); */
            /*         sObjectName->mCatalog    = QLP_GET_PTR_VALUE( sValue ); */
            /*         sObjectName->mCatalogPos = sValue->mNodePos; */

            /*         sObjectName->mNodePos = sValue->mNodePos; */

            /*         sElem = QLP_LIST_GET_NEXT_ELEM( sElem ); */
            /*     } */
            case 2 :
                {
                    sValue = QLP_LIST_GET_POINTER_VALUE( sElem );
                    sObjectName->mSchema    = QLP_GET_PTR_VALUE( sValue );
                    sObjectName->mSchemaPos = sValue->mNodePos;

                    sObjectName->mNodePos = ( sObjectName->mNodePos == 0 )
                        ? sValue->mNodePos : sObjectName->mNodePos;

                    sElem = QLP_LIST_GET_NEXT_ELEM( sElem );
                }
            case 1 :
                {
                    sValue = QLP_LIST_GET_POINTER_VALUE( sElem );
                    sObjectName->mObject    = QLP_GET_PTR_VALUE( sValue );
                    sObjectName->mObjectPos = sValue->mNodePos;
                    
                    sObjectName->mNodePos = ( sObjectName->mNodePos == 0 )
                        ? sValue->mNodePos : sObjectName->mNodePos;

                    break;
                }
            default :
                {
                    aParam->mErrStatus = STL_FAILURE;
                    break;
                }
        }
    }


    /**
     * Schema Object List 에 연결
     */
    
    if ( aParam->mErrStatus == STL_SUCCESS )
    {
        /**
         * List 객체 생성
         */
        
        if ( aParam->mSchemaObjectList == NULL )
        {
            aParam->mSchemaObjectList = qlpMakeList( aParam, QLL_POINTER_LIST_TYPE );
        }
        else
        {
            /* nothing to do */
        }

        /**
         * List 에 등록 
         */
        
        sObjectList = (qlpList *) aParam->mSchemaObjectList;
        
        if ( sObjectList != NULL )
        {
            qlpAddPtrValueToList( aParam, sObjectList, sObjectName );  
        }
        else
        {
            sObjectName = NULL;
            aParam->mErrStatus = STL_FAILURE;
        }
    }
    else
    {
        /* nothing to do */
    }
    
    return sObjectName;
}


/**
 * @brief Position을 갖는 Object Name 얻기 
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aObjectName         Object Name
 * @param[in]   aIsFixedPosition    Is Fixed Position
 * @param[in]   aIsLeft             Is Left
 *
 * @return qlpOrdering
 */
inline qlpOrdering * qlpMakeOrdering( stlParseParam * aParam,
                                      stlInt32        aNodePos,
                                      qlpObjectName * aObjectName,
                                      stlBool         aIsFixedPosition,
                                      stlBool         aIsLeft )
{
    qlpOrdering     * sOrdering     = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpOrdering ),
                            (void**) & sOrdering,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sOrdering = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sOrdering->mType              = QLL_ORDERING;
        sOrdering->mNodePos           = aNodePos;

        sOrdering->mObjectName        = aObjectName;
        sOrdering->mIsFixedPosition   = aIsFixedPosition;
        sOrdering->mIsLeft            = aIsLeft;
    }

    return sOrdering;
}

/**
 * @brief Column Name 얻기 
 * @param[in]   aParam       Parse Parameter
 * @param[in]   aName        Column Name 
 *
 * @return qlpColumnName
 */
inline qlpColumnName * qlpMakeColumnName( stlParseParam  * aParam,
                                          qlpList        * aName )
{
    qlpColumnName   * sName  = NULL;
    qlpListElement  * sElem  = NULL;
    qlpValue        * sValue = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpColumnName ),
                            (void**) & sName,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sName = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sName->mType = QLL_COLUMN_NAME_TYPE;

        sElem = QLP_LIST_GET_FIRST( aName );

        switch( aName->mCount )
        {
            /* case 4 : */
            /*     { */
            /*         sValue = QLP_LIST_GET_POINTER_VALUE( sElem ); */
            /*         sName->mCatalog    = QLP_GET_PTR_VALUE( sValue ); */
            /*         sName->mCatalogPos = sValue->mNodePos; */

            /*         sName->mNodePos = sValue->mNodePos; */

            /*         sElem = QLP_LIST_GET_NEXT_ELEM( sElem ); */
            /*     } */
            case 3 :
                {
                    sValue = QLP_LIST_GET_POINTER_VALUE( sElem );
                    sName->mSchema    = QLP_GET_PTR_VALUE( sValue );
                    sName->mSchemaPos = sValue->mNodePos;

                    sName->mNodePos = ( sName->mNodePos == 0 )
                        ? sValue->mNodePos : sName->mNodePos;

                    sElem = QLP_LIST_GET_NEXT_ELEM( sElem );
                }
            case 2 :
                {
                    sValue = QLP_LIST_GET_POINTER_VALUE( sElem );
                    sName->mTable    = QLP_GET_PTR_VALUE( sValue );
                    sName->mTablePos = sValue->mNodePos;

                    sName->mNodePos = ( sName->mNodePos == 0 )
                        ? sValue->mNodePos : sName->mNodePos;

                    sElem = QLP_LIST_GET_NEXT_ELEM( sElem );
                }
            case 1 :
                {
                    sValue = QLP_LIST_GET_POINTER_VALUE( sElem );
                    sName->mColumn    = QLP_GET_PTR_VALUE( sValue );
                    sName->mColumnPos = sValue->mNodePos;

                    sName->mNodePos = ( sName->mNodePos == 0 )
                        ? sValue->mNodePos : sName->mNodePos;

                    break;
                }
            default :
                {
                    aParam->mErrStatus = STL_FAILURE;
                    break;
                }
        }
    }

    return sName;
}

/**
 * @brief insert source 노드 생성
 * @param[in]   aParam       Parse Parameter
 * @param[in]   aNodePos     Node Position
 * @param[in]   aIsAllColumn Is All Column
 * @param[in]   aIsDefault   Is Default Value
 * @param[in]   aColumns     Target Columns
 * @param[in]   aValues      Value
 * @param[in]   aSubquery    SELECT Subquery
 * @return qlpInsertSource node
 */
inline qlpInsertSource * qlpMakeInsertSource( stlParseParam  * aParam,
                                              stlInt32         aNodePos,
                                              stlBool          aIsAllColumn,
                                              stlBool          aIsDefault,
                                              qlpList        * aColumns,
                                              qlpList        * aValues,
                                              qllNode        * aSubquery )
{
    qlpInsertSource  * sSource = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpInsertSource ),
                            (void**) & sSource,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSource = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sSource->mType        = QLL_INSERT_SOURCE_TYPE;
        sSource->mNodePos     = aNodePos;

        sSource->mIsAllColumn = aIsAllColumn;
        sSource->mIsDefault   = aIsDefault;
        sSource->mColumns     = aColumns;
        sSource->mValues      = aValues;
        sSource->mSubquery    = aSubquery;
    }
        
    return sSource;
}

/**
 * @brief UPDATE multiple set column 생성 SET ( .. ) = ( .. ) 
 * @param[in]   aParam       Parse Parameter
 * @param[in]   aNodePos     Node Position
 * @param[in]   aColumns     Target Columns
 * @param[in]   aValues      Value
 * @return qlpList node
 */
inline qlpList * qlpMakeMultipleSetColumn( stlParseParam    * aParam,
                                           stlInt32           aNodePos,
                                           qlpList          * aColumns,
                                           qlpList          * aValues )
{
    qlpListElement  * sColumnList       = NULL;    
    qlpColumnRef    * sUpdateColumnRef  = NULL;

    qlpListElement  * sValueList        = NULL;
    qllNode         * sValue            = NULL;

    qlpTarget       * sTarget           = NULL;
    qlpRowTarget    * sRowTarget        = NULL;

    qlpList * sList = qlpMakeList( aParam, QLL_POINTER_LIST_TYPE );


    if( aValues->mCount > 1 )
    {
        for ( sColumnList = aColumns->mHead, sValueList = aValues->mHead;
              (sColumnList != NULL) && (sValueList != NULL);
              sColumnList = sColumnList->mNext, sValueList = sValueList->mNext )
        {
            sUpdateColumnRef = (qlpColumnRef *) sColumnList->mData.mPtrValue;
            sValue  = (qllNode *) sValueList->mData.mPtrValue;

            sTarget = qlpMakeTarget( aParam,
                                     sUpdateColumnRef->mNodePos,
                                     NULL,
                                     sUpdateColumnRef->mNodePos,
                                     NULL,
                                     sValue,
                                     sUpdateColumnRef );

            qlpAddPtrValueToList( aParam, sList, sTarget );
        }

        if ( (sColumnList != NULL) || (sValueList != NULL) )
        {
            /**
             * 컬럼 개수와 Value 개수가 다른 경우
             * ex) SET (C1, C2) = (1, 2, 3)
             */
            sList = NULL;
            aParam->mErrStatus = STL_FAILURE;

            if ( sColumnList != NULL )
            {
                sUpdateColumnRef = (qlpColumnRef *) sColumnList->mData.mPtrValue;
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              QLL_ERRCODE_NOT_ENOUGH_VALUES,
                              qlgMakeErrPosString( aParam->mBuffer,
                                                   sUpdateColumnRef->mNodePos,
                                                   QLP_PARSE_GET_ENV( aParam ) ),
                              aParam->mErrorStack );
            }
            else
            {
                sValue  = (qllNode *) sValueList->mData.mPtrValue;
                stlPushError( STL_ERROR_LEVEL_ABORT,
                              QLL_ERRCODE_TOO_MANY_VALUES,
                              qlgMakeErrPosString( aParam->mBuffer,
                                                   sValue->mNodePos,
                                                   QLP_PARSE_GET_ENV( aParam ) ),
                              aParam->mErrorStack );
            }
        }
        else
        {
            /* 개수가 동일함 */
        }
    }
    else
    {
        if( aParam->mAllocator( aParam->mContext,
                                STL_SIZEOF( qlpRowTarget ),
                                (void**) & sRowTarget,
                                aParam->mErrorStack )
            != STL_SUCCESS )
        {
            sRowTarget = NULL;
            aParam->mErrStatus = STL_FAILURE;
        }
        else
        {
            sRowTarget->mType           = QLL_ROW_TARGET_TYPE;
            sRowTarget->mNodePos        = aNodePos;
            sRowTarget->mSetColumnList  = aColumns;
            sRowTarget->mValueExpr      = (qllNode*)(aValues->mHead->mData.mPtrValue);
        }

        qlpAddPtrValueToList( aParam, sList, sRowTarget );
    }

    return sList;
}

/**
 * @brief default 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aIsRowValue      Is Row Value
 * @param[in]   aIsRowValuePos   position of Is Row Value
 * 
 * @return qlpDefault node
 */
inline qlpDefault * qlpMakeDefault( stlParseParam  * aParam,
                                    stlInt32         aNodePos,
                                    stlBool          aIsRowValue,
                                    stlInt32         aIsRowValuePos )
{
    qlpDefault  * sDefault = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDefault ),
                            (void**) & sDefault,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDefault = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDefault->mType          = QLL_DEFAULT_TYPE;
        sDefault->mNodePos       = aNodePos;
        sDefault->mIsRowValue    = aIsRowValue;
        sDefault->mIsRowValuePos = aIsRowValuePos;
    }
        
    return sDefault;
}


/**
 * @brief Alias 리스트 구성
 * @param[in]   aParam    Parse Parameter
 * @param[in]   aAlias    Alias Node
 *
 * @return 없음
 */
inline void qlpAddAliasList( stlParseParam  * aParam,
                             qlpAlias       * aAlias )
{
    qlpList  * sList = QLP_PARSE_GET_ALIAS( aParam );

    if( sList == NULL )
    {
        sList = qlpMakeList( aParam, QLL_POINTER_LIST_TYPE );
    }
    else
    {
        /* Do Nothing */
    }
    
    qlpAddPtrValueToList( aParam,
                          sList,
                          aAlias );
    
    QLP_PARSE_GET_ALIAS( aParam ) = sList;
}


/**
 * @brief referential Trigger Action 노드 생성
 * @param[in]   aParam            Parse Parameter
 * @param[in]   aNodePos          Node Position
 * @param[in]   aUpdateAction     Update Action
 * @param[in]   aDeleteAction     Delete Action
 *
 * @return qlpRefTriggerAction 노드 
 */
inline qlpRefTriggerAction * qlpMakeRefTriggerAction( stlParseParam  * aParam,
                                                      stlInt32         aNodePos,
                                                      qlpValue       * aUpdateAction,
                                                      qlpValue       * aDeleteAction )
{
    qlpRefTriggerAction     * sAction = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpRefTriggerAction ),
                            (void**) & sAction,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAction = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAction->mType            = QLL_REF_TRIGGER_ACTION_TYPE;
        sAction->mNodePos         = aNodePos;
        sAction->mUpdateAction    = aUpdateAction;
        sAction->mDeleteAction    = aDeleteAction;
    }

    return sAction;
    
}


/**
 * @brief character length 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aLengthUnit     Length Unit 
 * @param[in]   aLengthUnitPos  Position of Length Unit 
 * @param[in]   aLength         Length
 *
 * @return qlpCharLength 노드 
 */
inline qlpCharLength * qlpMakeCharLength( stlParseParam        * aParam,
                                          stlInt32               aNodePos,
                                          dtlStringLengthUnit    aLengthUnit,
                                          stlInt32               aLengthUnitPos,
                                          qlpValue             * aLength )
{
    qlpCharLength  * sCharLen = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpCharLength ),
                            (void**) & sCharLen,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sCharLen = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sCharLen->mType          = QLL_CHAR_LENGTH_TYPE;
        sCharLen->mNodePos       = aNodePos;
        sCharLen->mLengthUnit    = aLengthUnit;
        sCharLen->mLengthUnitPos = aLengthUnitPos;
        sCharLen->mLength        = aLength;
    }

    return sCharLen;
}


/**
 * @brief constraint attribute 노드 생성
 * @param[in]   aParam             Parse Parameter
 * @param[in]   aNodePos           Node Position
 * @param[in]   aInitDeferred      Init Deferred Option
 * @param[in]   aDeferrable        Deferrable Option
 * @param[in]   aEnforce           ENFORCED | NOT ENFORCED Option
 * @return qlpConstraintAttr 노드 
 */
inline qlpConstraintAttr * qlpMakeConstraintAttr( stlParseParam        * aParam,
                                                  stlInt32               aNodePos,
                                                  qlpValue             * aInitDeferred,
                                                  qlpValue             * aDeferrable,
                                                  qlpValue             * aEnforce )
{
    qlpConstraintAttr  * sConstAttr = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpConstraintAttr ),
                            (void**) & sConstAttr,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sConstAttr = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sConstAttr->mType            = QLL_CONSTRAINT_ATTR__TYPE;
        sConstAttr->mNodePos         = aNodePos;
        sConstAttr->mInitDeferred    = aInitDeferred;
        sConstAttr->mDeferrable      = aDeferrable;
        sConstAttr->mEnforce         = aEnforce;
    }

    return sConstAttr;
}


/**
 * @brief constraint characteristic 을 누적
 * @param[in]      aParam          Parse Parameter
 * @param[in]      aFirstConstAttr First Constraint Characteristic
 * @param[in]      aNextConstAttr  Next Constraint Characteristic
 * @return qlpConstraintAttr 노드 
 */
inline qlpConstraintAttr * qlpAppendConstraintAttr( stlParseParam     * aParam,
                                                    qlpConstraintAttr * aFirstConstAttr,
                                                    qlpConstraintAttr * aNextConstAttr )
{
    stlBool  sIsValid = STL_TRUE;
    
    if( aNextConstAttr->mInitDeferred != NULL )
    {
        if( ( aFirstConstAttr->mInitDeferred != NULL ) ||
            ( aFirstConstAttr->mEnforce != NULL ) ) 
        {
            /**
             * BNF상 enforce는 옵션 중 마지막이어야 한다.
             */
            sIsValid = STL_FALSE;
        }
        else
        {
            aFirstConstAttr->mInitDeferred = aNextConstAttr->mInitDeferred;
        }
    }
    else if( aNextConstAttr->mDeferrable != NULL )
    {
        if( ( aFirstConstAttr->mDeferrable != NULL ) ||
            ( aFirstConstAttr->mEnforce != NULL ) )   
        {
            /**
             * BNF상 enforce는 옵션 중 마지막이어야 한다.
             */
            sIsValid = STL_FALSE;
        }
        else
        {
            aFirstConstAttr->mDeferrable = aNextConstAttr->mDeferrable;
        }
    }
    else if( aNextConstAttr->mEnforce != NULL )
    {
        if( (aFirstConstAttr->mEnforce != NULL) )
        {
            sIsValid = STL_FALSE;
        }
        else
        {
            aFirstConstAttr->mEnforce = aNextConstAttr->mEnforce;
        }
    }
    else
    {
        sIsValid = STL_FALSE;
    }

    if ( sIsValid == STL_TRUE )
    {
        /* success */
    }
    else
    {
        aParam->mErrStatus = STL_FAILURE;
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYNTAX_ERROR,
                      qlgMakeErrPosString( aParam->mBuffer,
                                           aNextConstAttr->mNodePos,
                                           QLP_PARSE_GET_ENV( aParam ) ),
                      aParam->mErrorStack );
    }

    return aFirstConstAttr;
}

/**
 * @brief qlpHintTable 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aTableHintType  Table Hint Type
 * @param[in]   aObjectName     Object Name
 *
 * @return qlpHintTable node
 */
inline qlpHintTable * qlpMakeHintTable( stlParseParam     * aParam,
                                        stlInt32            aNodePos,
                                        qlpTableHintType    aTableHintType,
                                        qlpObjectName     * aObjectName )
{
    qlpHintTable    * sHintTable    = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpHintTable ),
                            (void**) & sHintTable,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sHintTable = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sHintTable->mType           = QLL_HINT_TYPE;
        sHintTable->mNodePos        = aNodePos;

        sHintTable->mHintType       = QLP_HINT_TABLE;
        sHintTable->mIsUsed         = STL_FALSE;

        sHintTable->mTableHintType  = aTableHintType;
        sHintTable->mObjectName     = aObjectName;
    }

    return sHintTable;
}


/**
 * @brief qlpHintIndex 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aIndexHintType  Index Hint Type
 * @param[in]   aObjectName     Object Name
 * @param[in]   aIndexScanList  Index Scan Node List
 *
 * @return qlpHintIndex node
 */
inline qlpHintIndex * qlpMakeHintIndex( stlParseParam     * aParam,
                                        stlInt32            aNodePos,
                                        qlpIndexHintType    aIndexHintType,
                                        qlpObjectName     * aObjectName,
                                        qlpList           * aIndexScanList )
{
    qlpHintIndex    * sHintIndex    = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpHintIndex ),
                            (void**) & sHintIndex,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sHintIndex = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sHintIndex->mType           = QLL_HINT_TYPE;
        sHintIndex->mNodePos        = aNodePos;

        sHintIndex->mHintType       = QLP_HINT_INDEX;
        sHintIndex->mIsUsed         = STL_FALSE;

        sHintIndex->mIndexHintType  = aIndexHintType;
        sHintIndex->mObjectName     = aObjectName;
        sHintIndex->mIndexScanList  = aIndexScanList;
    }

    return sHintIndex;
}


/**
 * @brief qlpHintJoinOrder 노드 생성
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aJoinOrderHintType  Join Order Hint Type
 * @param[in]   aObjectNameList     Object Name List
 *
 * @return qlpHintJoinOrder node
 */
inline qlpHintJoinOrder * qlpMakeHintJoinOrder( stlParseParam         * aParam,
                                                stlInt32                aNodePos,
                                                qlpJoinOrderHintType    aJoinOrderHintType,
                                                qlpList               * aObjectNameList )
{
    qlpHintJoinOrder    * sHintJoinOrder    = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpHintJoinOrder ),
                            (void**) & sHintJoinOrder,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sHintJoinOrder = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sHintJoinOrder->mType               = QLL_HINT_TYPE;
        sHintJoinOrder->mNodePos            = aNodePos;

        sHintJoinOrder->mHintType           = QLP_HINT_JOIN_ORDER;
        sHintJoinOrder->mIsUsed             = STL_FALSE;

        sHintJoinOrder->mJoinOrderHintType  = aJoinOrderHintType;
        sHintJoinOrder->mObjectNameList     = aObjectNameList;
    }

    return sHintJoinOrder;
}


/**
 * @brief qlpHintJoinOper 노드 생성
 * @param[in]   aParam              Parse Parameter
 * @param[in]   aNodePos            Node Position
 * @param[in]   aJoinOperHintType   Join Operation Hint Type
 * @param[in]   aIsNoUse            No Use Flag
 * @param[in]   aObjectNameList        Object Name List
 *
 * @return qlpHintJoinOper node
 */
inline qlpHintJoinOper * qlpMakeHintJoinOper( stlParseParam       * aParam,
                                              stlInt32              aNodePos,
                                              qlpJoinOperHintType   aJoinOperHintType,
                                              stlBool               aIsNoUse,
                                              qlpList             * aObjectNameList )
{
    qlpHintJoinOper * sHintJoinOper = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpHintJoinOper ),
                            (void**) & sHintJoinOper,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sHintJoinOper = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sHintJoinOper->mType                = QLL_HINT_TYPE;
        sHintJoinOper->mNodePos             = aNodePos;

        sHintJoinOper->mHintType            = QLP_HINT_JOIN_OPER;
        sHintJoinOper->mIsUsed              = STL_FALSE;

        sHintJoinOper->mJoinOperHintType    = aJoinOperHintType;
        sHintJoinOper->mIsNoUse             = aIsNoUse;
        sHintJoinOper->mObjectNameList      = aObjectNameList;
    }

    return sHintJoinOper;
}


/**
 * @brief qlpHintQueryTransform 노드 생성
 * @param[in]   aParam                  Parse Parameter
 * @param[in]   aNodePos                Node Position
 * @param[in]   aQueryTransformHintType Query Transformation Hint Type
 *
 * @return qlpHintQueryTransform node
 */
inline qlpHintQueryTransform * qlpMakeHintQueryTransform( stlParseParam             * aParam,
                                                          stlInt32                    aNodePos,
                                                          qlpQueryTransformHintType   aQueryTransformHintType )
{
    qlpHintQueryTransform   * sHintQueryTransform   = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpHintQueryTransform ),
                            (void**) & sHintQueryTransform,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sHintQueryTransform = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sHintQueryTransform->mType                      = QLL_HINT_TYPE;
        sHintQueryTransform->mNodePos                   = aNodePos;

        sHintQueryTransform->mHintType                  = QLP_HINT_QUERY_TRANSFORM;
        sHintQueryTransform->mIsUsed                    = STL_FALSE;

        sHintQueryTransform->mQueryTransformHintType    = aQueryTransformHintType;
    }

    return sHintQueryTransform;
}


/**
 * @brief qlpHintOther 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aIsNoUse        No Use Flag
 * @param[in]   aObjectName     Object Name
 * @param[in]   aOtherHintType  Other Hint Type
 *
 * @return qlpHintOther node
 */
inline qlpHintOther * qlpMakeHintOther( stlParseParam       * aParam,
                                        stlInt32              aNodePos,
                                        stlBool               aIsNoUse,
                                        qlpObjectName       * aObjectName,
                                        qlpOtherHintType      aOtherHintType )
{
    qlpHintOther    * sHintOther    = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpHintOther ),
                            (void**) & sHintOther,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sHintOther = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sHintOther->mType           = QLL_HINT_TYPE;
        sHintOther->mNodePos        = aNodePos;

        sHintOther->mHintType       = QLP_HINT_OTHER;
        sHintOther->mIsUsed         = STL_FALSE;

        sHintOther->mIsNoUse        = aIsNoUse;
        sHintOther->mObjectName     = aObjectName;

        sHintOther->mOtherHintType  = aOtherHintType;
    }

    return sHintOther;
}


/**
 * @brief Value Expression 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aNodeLen        Node Length
 * @param[in]   aExpr           Expression
 *
 * @return qlpValueExpr node
 */
inline qlpValueExpr * qlpMakeValueExpr( stlParseParam  * aParam,
                                        stlInt32         aNodePos,
                                        stlInt32         aNodeLen,
                                        qllNode        * aExpr )
{
    qlpValueExpr  * sExpr = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpValueExpr ),
                            (void**) & sExpr,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sExpr = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sExpr->mType      = QLL_VALUE_EXPR_TYPE;
        sExpr->mNodePos   = aNodePos;
        sExpr->mNodeLen   = aNodeLen;
        sExpr->mExpr      = aExpr;
    }

    return sExpr;
}


/**
 * @brief Identifier의 길이 검사
 * @param[in]      aParam          Parse Parameter
 * @param[in,out]  aStrValue       Identifier String Value
 *
 * @return 없음
 */
inline void qlpCheckIdentifierLength( stlParseParam   * aParam,
                                      qlpValue        * aStrValue )
{
    stlChar  * sString = (stlChar *) QLP_GET_PTR_VALUE( aStrValue );

    if( sString == NULL )
    {
        aParam->mErrStatus = STL_FAILURE;

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aParam->mBuffer,
                                           aStrValue->mNodePos,
                                           QLP_PARSE_GET_ENV( aParam ) ),
                      aParam->mErrorStack,
                      "\"\"" );
    }
    else if( stlStrlen( sString ) < STL_MAX_SQL_IDENTIFIER_LENGTH )
    {
        /* Do Nothing */
    }
    else
    {
        aParam->mErrStatus = STL_FAILURE;

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_IDENTIFIER_LENGTH_EXCEEDED,
                      qlgMakeErrPosString( aParam->mBuffer,
                                           aStrValue->mNodePos,
                                           QLP_PARSE_GET_ENV( aParam ) ),
                      aParam->mErrorStack );
    }

    return;
}


/**
 * @brief 중복된 option이 설정되어 있는지 확인  
 * @param[in]      aParam          Parse Parameter
 * @param[in]      aList           Options List
 * @param[in]      aOption         Input Option
 *
 * @return 없음
 */
inline void qlpCheckDuplicateOptions( stlParseParam   * aParam,
                                      qlpList         * aList,
                                      qlpDefElem      * aOption )
{
    qlpListElement  * sListElem = NULL;
    qlpDefElem      * sOption  = NULL;
    
    QLP_LIST_FOR_EACH( aList, sListElem )
    {
        sOption = (qlpDefElem *) QLP_LIST_GET_POINTER_VALUE( sListElem );
        
        if( sOption->mElementType == aOption->mElementType )
        {
            aParam->mErrStatus = STL_FAILURE;
            
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          QLL_ERRCODE_DUPLICATE_OPTIONS,
                          qlgMakeErrPosString( aParam->mBuffer,
                                               aOption->mNodePos,
                                               QLP_PARSE_GET_ENV( aParam ) ),
                          aParam->mErrorStack );
            
            break;
        }
        else
        {
            continue;
        }
    }
    
    return;
}


/**
 * @brief 중복된 constraint가 설정되어 있는지 확인  
 * @param[in]      aParam          Parse Parameter
 * @param[in]      aList           Constraints List
 * @param[in]      aConstr         Input Constraint
 *
 * @return 없음
 */
inline void qlpCheckDuplicateConstraints( stlParseParam   * aParam,
                                          qlpList         * aList,
                                          qlpConstraint   * aConstr )
{
    qlpListElement  * sListElem = NULL;
    qlpConstraint   * sConstr   = NULL;
    
    if( (aConstr->mConstType == QLP_CONSTRAINT_NULL) ||
        (aConstr->mConstType == QLP_CONSTRAINT_NOTNULL) )
    {
        QLP_LIST_FOR_EACH( aList, sListElem )
        {
            sConstr = (qlpConstraint *) QLP_LIST_GET_POINTER_VALUE( sListElem );

            if( (sConstr->mConstType == QLP_CONSTRAINT_NULL) ||
                (sConstr->mConstType == QLP_CONSTRAINT_NOTNULL) )
            {
                aParam->mErrStatus = STL_FAILURE;

                stlPushError( STL_ERROR_LEVEL_ABORT,
                              QLL_ERRCODE_DUPLICATE_CONSTRAINTS_NULL_NOT_NULL,
                              qlgMakeErrPosString( aParam->mBuffer,
                                                   aConstr->mNodePos,
                                                   QLP_PARSE_GET_ENV( aParam ) ),
                              aParam->mErrorStack );

                break;
            }
            else
            {
                continue;
            }
        }
    }
    else if ( (aConstr->mConstType == QLP_CONSTRAINT_UNIQUE) ||
              (aConstr->mConstType == QLP_CONSTRAINT_PRIMARY) )
    {
        QLP_LIST_FOR_EACH( aList, sListElem )
        {
            sConstr = (qlpConstraint *) QLP_LIST_GET_POINTER_VALUE( sListElem );

            if( (sConstr->mConstType == QLP_CONSTRAINT_UNIQUE) ||
                (sConstr->mConstType == QLP_CONSTRAINT_PRIMARY) )
            {
                aParam->mErrStatus = STL_FAILURE;

                stlPushError( STL_ERROR_LEVEL_ABORT,
                              QLL_ERRCODE_DUPLICATE_CONSTRAINTS_UNIQUE_PRIMARY_KEY,
                              qlgMakeErrPosString( aParam->mBuffer,
                                                   aConstr->mNodePos,
                                                   QLP_PARSE_GET_ENV( aParam ) ),
                              aParam->mErrorStack );

                break;
            }
            else
            {
                continue;
            }
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    return;
}


/**
 * @brief 노드의 type을 확인
 * @param[in]      aParam          Parse Parameter
 * @param[in]      aNode           Node
 * @param[in]      aType           Check Type
 *
 * @return 없음
 */
inline void qlpCheckNodeType( stlParseParam   * aParam,
                              qllNode         * aNode,
                              stlInt32          aType )
{
    if( aNode->mType == aType )
    {
        /* Do Nothing */
    }
    else
    {
        aParam->mErrStatus = STL_FAILURE;

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYNTAX_ERROR,
                      qlgMakeErrPosString( aParam->mBuffer,
                                           aNode->mNodePos,
                                           QLP_PARSE_GET_ENV( aParam ) ),
                      aParam->mErrorStack );
    }

    return;
}


/**
 * @brief constraint에 constraint characteristic을 추가
 * @param[in]      aParam          Parse Parameter
 * @param[in]      aConstr         Constraint
 * @param[in]      aConstAttr      Constraint Characteristic
 *
 * @return 없음
 */
inline void qlpAddConstraintCharacteristic( stlParseParam     * aParam,
                                            qlpConstraint     * aConstr,
                                            qlpConstraintAttr * aConstAttr )
{
    stlBool  sIsValid = STL_FALSE;
    
    switch ( aConstr->mConstType )
    {
        case QLP_CONSTRAINT_INVALID:
        case QLP_CONSTRAINT_NULL:
            sIsValid = STL_FALSE;
            break;
        case QLP_CONSTRAINT_NOTNULL:
        case QLP_CONSTRAINT_CHECK:
        case QLP_CONSTRAINT_PRIMARY:
        case QLP_CONSTRAINT_UNIQUE:
        case QLP_CONSTRAINT_FOREIGN:
            sIsValid = STL_TRUE;
            break;
        case QLP_CONSTRAINT_EXCLUSION:
        default:
            sIsValid = STL_FALSE;
            break;
    }

    if ( sIsValid == STL_TRUE )
    {
        /* success */
        aConstr->mConstAttr = aConstAttr;
    }
    else
    {
        aParam->mErrStatus = STL_FAILURE;
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SYNTAX_ERROR,
                      qlgMakeErrPosString( aParam->mBuffer,
                                           aConstAttr->mNodePos,
                                           QLP_PARSE_GET_ENV( aParam ) ),
                      aParam->mErrorStack );
    }
}


/**
 * @brief size 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aIsUnlimited    Is Unlimited
 * @param[in]   aSizeUnit       Size Unit
 * @param[in]   aSize           Size 
 *
 * @return qlpSize 노드 
 */
inline qlpSize * qlpMakeSize( stlParseParam  * aParam,
                              stlInt32         aNodePos,
                              stlBool          aIsUnlimited,
                              qlpValue       * aSizeUnit,
                              qlpValue       * aSize )
{
    qlpSize  * sSize = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpSize ),
                            (void**) & sSize,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSize = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sSize->mType        = QLL_SIZE_TYPE;
        sSize->mNodePos     = aNodePos;
        sSize->mIsUnlimited = aIsUnlimited;
        sSize->mSizeUnit    = aSizeUnit;
        sSize->mSize        = aSize;
    }

    return sSize;
}


/**
 * @brief parameter의 DEFERRED, SCOPE 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aIsDeferred     DEFERRED
 * @param[in]   aScope          SCOPE
 *
 * @return qlpSetParamAttr 노드 
 */
inline qlpSetParamAttr * qlpMakeSetParamAttr(stlParseParam * aParam,
                                             stlInt32        aNodePos,
                                             stlBool         aIsDeferred,
                                             qlpValue      * aScope )
{
    qlpSetParamAttr * sParamAttr = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpSetParam ),
                            (void**) & sParamAttr,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sParamAttr = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sParamAttr->mType       = QLL_SET_PARAM_ATTR_TYPE;
        sParamAttr->mNodePos    = aNodePos;
        sParamAttr->mIsDeferred = aIsDeferred;
        sParamAttr->mScope      = QLP_GET_INT_VALUE( aScope );
    }

    return sParamAttr;
}

/**
 * @brief parameter, value pair 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aPropertyName   Property Name
 * @param[in]   aSetValue       Set Value
 * @param[in]   aAtrr           Parameter Attribute
 *
 * @return qlpSetParam 노드 
 */
inline qlpSetParam * qlpMakeSetParameter( stlParseParam   * aParam,
                                          stlInt32          aNodePos,
                                          qlpValue        * aPropertyName,
                                          qlpValue        * aSetValue,
                                          qlpSetParamAttr * aAtrr )
{
    qlpSetParam * sSetParam = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpSetParam ),
                            (void**) & sSetParam,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSetParam = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sSetParam->mType         = QLL_SET_PARAM_TYPE;
        sSetParam->mNodePos      = aNodePos;
        sSetParam->mPropertyName = aPropertyName;
        sSetParam->mSetValue     = aSetValue;
        sSetParam->mIsDeferred   = aAtrr->mIsDeferred;
        sSetParam->mScope        = aAtrr->mScope;
    }

    return sSetParam;
}



/**
 * @brief Alter System SET 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodeType       Node Type
 * @param[in]   aNodePos        Node Position
 * @param[in]   aSetStruct      parameter name & value
 *
 * @return qlpAlterSystem 노드 
 */
inline qlpAlterSystem * qlpMakeAlterSystemSet( stlParseParam  * aParam,
                                               qllNodeType      aNodeType,
                                               stlInt32         aNodePos,
                                               qlpSetParam    * aSetStruct )
{
    qlpAlterSystem * sAlterSystem = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterSystem ),
                            (void**) & sAlterSystem,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSystem = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSystem->mType         = aNodeType;
        sAlterSystem->mNodePos      = aNodePos;
        sAlterSystem->mPropertyName = aSetStruct->mPropertyName;
        sAlterSystem->mSetValue     = aSetStruct->mSetValue;
        sAlterSystem->mIsDeferred   = aSetStruct->mIsDeferred;
        sAlterSystem->mScope        = aSetStruct->mScope;
    }

    return sAlterSystem;
}

/**
 * @brief Alter System RESET 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodeType       Node Type
 * @param[in]   aNodePos        Node Position
 * @param[in]   aPropertyName   parameter name
 *
 * @return qlpAlterSystem 노드 
 */
inline qlpAlterSystem * qlpMakeAlterSystemReset( stlParseParam  * aParam,
                                                 qllNodeType      aNodeType,
                                                 stlInt32         aNodePos,
                                                 qlpValue       * aPropertyName )
{
    qlpAlterSystem * sAlterSystem = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterSystem ),
                            (void**) & sAlterSystem,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSystem = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSystem->mType         = aNodeType;
        sAlterSystem->mNodePos      = aNodePos;
        sAlterSystem->mPropertyName = aPropertyName;
        sAlterSystem->mSetValue     = NULL;
        sAlterSystem->mIsDeferred   = STL_FALSE;
        sAlterSystem->mScope        = KNL_PROPERTY_SCOPE_FILE;
    }

    return sAlterSystem;
}

/**
 * @brief BREAKPOINT ACTION 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aActionType     action type
 * @param[in]   aPostActionType post action type
 * @param[in]   aArgument       action argument
 *
 * @return qlpBreakpointAction 노드 
 */
qlpBreakpointAction * qlpMakeBreakpointAction( stlParseParam           * aParam,
                                               stlInt32                  aNodePos,
                                               knlBreakPointAction       aActionType,
                                               knlBreakPointPostAction   aPostActionType,
                                               qlpValue                * aArgument )
{
    qlpBreakpointAction * sBpAction = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpBreakpointAction ),
                            (void**) & sBpAction,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sBpAction = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sBpAction->mType             = QLL_BREAKPOINT_ACTION_TYPE;
        sBpAction->mNodePos          = aNodePos;
        sBpAction->mActionType       = aActionType;
        sBpAction->mPostActionType   = aPostActionType;
        sBpAction->mArgument         = aArgument;
    }

    return sBpAction;

}

/**
 * @brief BREAKPOINT OPTION 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aSkipCount      skip count
 * @param[in]   aAction         action info
 *
 * @return qlpBreakpointOption 노드 
 */
qlpBreakpointOption * qlpMakeBreakpointOption( stlParseParam       * aParam,
                                               stlInt32              aNodePos,
                                               qlpValue            * aSkipCount,
                                               qlpBreakpointAction * aAction )
{
    qlpBreakpointOption * sBpOption = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpBreakpointOption ),
                            (void**) & sBpOption,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sBpOption = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sBpOption->mType         = QLL_BREAKPOINT_OPTION_TYPE;
        sBpOption->mNodePos      = aNodePos;
        sBpOption->mSkipCount    = aSkipCount;
        sBpOption->mAction       = aAction;
    }

    return sBpOption;

}


/**
 * @brief ALTER SYSTEM BREAKPOINT 노드 생성
 * @param[in]   aParam             Parse Parameter
 * @param[in]   aNodePos           Node Position
 * @param[in]   aSessType          breakpoint session type
 * @param[in]   aBreakpointName    breakpoint name
 * @param[in]   aOwnerSessName     owner session name
 * @param[in]   aBreakpointOption  breakpoint option
 *
 * @return qlpAlterSystemBreakpoint 노드 
 */
qlpAlterSystemBreakpoint * qlpMakeAlterSystemBreakpoint( stlParseParam         * aParam,
                                                         stlInt32                aNodePos,
                                                         knlBreakPointSessType   aSessType,
                                                         qlpValue              * aBreakpointName,
                                                         qlpValue              * aOwnerSessName,
                                                         qlpBreakpointOption   * aBreakpointOption )
{
    qlpAlterSystemBreakpoint  * sAlterSystemBreakpoint = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterSystemBreakpoint ),
                            (void**) & sAlterSystemBreakpoint,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSystemBreakpoint = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSystemBreakpoint->mType             = QLL_STMT_ALTER_SYSTEM_BREAKPOINT_TYPE;
        sAlterSystemBreakpoint->mNodePos          = aNodePos;
        sAlterSystemBreakpoint->mSessType         = aSessType;
        sAlterSystemBreakpoint->mBreakpointName   = aBreakpointName;
        sAlterSystemBreakpoint->mOwnerSessName    = aOwnerSessName;
        sAlterSystemBreakpoint->mOption           = aBreakpointOption;
    }

    return sAlterSystemBreakpoint;
}

/**
 * @brief ALTER SYSTEM SESSION NAME 노드 생성
 * @param[in]   aParam        Parse Parameter
 * @param[in]   aNodePos      Node Position
 * @param[in]   aSessName     session name
 *
 * @return qlpAlterSystemSessName 노드 
 */
qlpAlterSystemSessName * qlpMakeAlterSystemSessName( stlParseParam       * aParam,
                                                     stlInt32              aNodePos,
                                                     qlpValue            * aSessName )
{
    qlpAlterSystemSessName  * sAlterSystemSessName = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterSystemSessName ),
                            (void**) & sAlterSystemSessName,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSystemSessName = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSystemSessName->mType       = QLL_STMT_ALTER_SYSTEM_SESS_NAME_TYPE;
        sAlterSystemSessName->mNodePos    = aNodePos;
        sAlterSystemSessName->mSessName   = aSessName;
    }

    return sAlterSystemSessName;
}

/**
 * @brief Alter System AGER 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aBehavior       START/STOP/LOOPBACK
 *
 * @return qlpAlterSystemAger 노드 
 */
inline qlpAlterSystemAger * qlpMakeAlterSystemAger( stlParseParam  * aParam,
                                                    stlInt32         aNodePos,
                                                    qlpAgerBehavior  aBehavior )
{
    qlpAlterSystemAger  * sAlterSystemAger = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterSystemAger ),
                            (void**) & sAlterSystemAger,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSystemAger = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSystemAger->mType      = QLL_STMT_ALTER_SYSTEM_AGER_TYPE;
        sAlterSystemAger->mNodePos   = aNodePos;
        sAlterSystemAger->mBehavior  = aBehavior;
    }

    return sAlterSystemAger;
}


/**
 * @brief Alter System {MOUNT|OPEN} DATABASE 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aStartupPhase   MOUNT/RESTRICT/SERVICE
 * @param[in]   aOpenOption     NULL/READ_ONLY/READ_WRITE
 * @param[in]   aLogOption      NORESETLOGS/RESETLOGS
 *
 * @return qlpAlterSystemStartupDatabase 노드 
 */
inline qlpAlterSystemStartupDatabase * qlpMakeAlterSystemStartupDatabase( stlParseParam   * aParam,
                                                                          stlInt32          aNodePos,
                                                                          knlStartupPhase   aStartupPhase,
                                                                          qlpValue        * aOpenOption,
                                                                          qlpValue        * aLogOption )
{
    qlpAlterSystemStartupDatabase  * sAlterSystemStartupDatabase = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( sAlterSystemStartupDatabase ),
                            (void**) & sAlterSystemStartupDatabase,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSystemStartupDatabase = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSystemStartupDatabase->mType         = QLL_STMT_ALTER_SYSTEM_STARTUP_DATABASE_TYPE;
        sAlterSystemStartupDatabase->mNodePos      = aNodePos;
        sAlterSystemStartupDatabase->mStartupPhase = aStartupPhase;
        sAlterSystemStartupDatabase->mOpenOption   = aOpenOption;
        sAlterSystemStartupDatabase->mLogOption    = aLogOption;
    }

    return sAlterSystemStartupDatabase;
}


/**
 * @brief Alter System CLOSE DATABASE 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aShutdownPhase  CLOSE
 * @param[in]   aCloseOption NULL/NORMAL/TRANSACTIONAL/IMMEDIATE/ABORT
 *
 * @return qlpAlterSystemShutdownDatabase 노드 
 */
inline qlpAlterSystemShutdownDatabase * qlpMakeAlterSystemShutdownDatabase( stlParseParam    * aParam,
                                                                            stlInt32           aNodePos,
                                                                            knlShutdownPhase   aShutdownPhase,
                                                                            qlpValue         * aCloseOption )
{
    qlpAlterSystemShutdownDatabase  * sAlterSystemShutdownDatabase = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( sAlterSystemShutdownDatabase ),
                            (void**) & sAlterSystemShutdownDatabase,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSystemShutdownDatabase = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSystemShutdownDatabase->mType = QLL_STMT_ALTER_SYSTEM_SHUTDOWN_DATABASE_TYPE;
        
        if( aCloseOption != NULL )
        {
            if( QLP_GET_INT_VALUE( aCloseOption ) == KNL_SHUTDOWN_MODE_ABORT )
            {
                /**
                 * ABORT는 QLL_STMT_ATTR_AUTOCOMMIT_NO를 가져야 한다.
                 */
                sAlterSystemShutdownDatabase->mType = QLL_STMT_ALTER_SYSTEM_SHUTDOWN_DATABASE_ABORT_TYPE;
            }
        }
        
        sAlterSystemShutdownDatabase->mNodePos       = aNodePos;
        sAlterSystemShutdownDatabase->mShutdownPhase = aShutdownPhase;
        sAlterSystemShutdownDatabase->mCloseOption   = aCloseOption;
    }

    return sAlterSystemShutdownDatabase;
}


/**
 * @brief ALTER SYSTEM SWITCH LOGFILE 을 위한 Parse Node 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aNodeType       Node Type
 * 
 * @return qlpAlterSystemSwitchLogfile 노드
 */
inline qlpAlterSystemSwitchLogfile * qlpMakeAlterSystemSwitchLogfile( stlParseParam    * aParam,
                                                                      stlInt32           aNodePos,
                                                                      qllNodeType        aNodeType )
{
    qlpAlterSystemSwitchLogfile  * sAlterDbLog = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpAlterSystemSwitchLogfile),
                            (void**) & sAlterDbLog,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterDbLog = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterDbLog->mType         = aNodeType;
        sAlterDbLog->mNodePos      = aNodePos;
    }

    return sAlterDbLog;
}


/**
 * @brief Alter System CHECKPOINT 노드 생성
 * @param[in]   aParam     Parse Parameter
 * @param[in]   aNodePos   Node Position
 *
 * @return qlpAlterSystemChkpt 노드 
 */
inline qlpAlterSystemChkpt * qlpMakeAlterSystemChkpt( stlParseParam  * aParam,
                                                      stlInt32         aNodePos )
{
    qlpAlterSystemChkpt  * sAlterSystemChkpt = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterSystemChkpt ),
                            (void**) & sAlterSystemChkpt,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSystemChkpt = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSystemChkpt->mType    = QLL_STMT_ALTER_SYSTEM_CHECKPOINT_TYPE;
        sAlterSystemChkpt->mNodePos = aNodePos;
    }

    return sAlterSystemChkpt;
}


/**
 * @brief Alter System FLUSH LOG 노드 생성
 * @param[in]   aParam     Parse Parameter
 * @param[in]   aNodePos   Node Position
 * @param[in]   aBehavior  START FLUSH LOGS, STOP FLUSH LOGS, FLUSH LOGS
 *
 * @return qlpAlterSystemFlushLog 노드 
 */
inline qlpAlterSystemFlushLog * qlpMakeAlterSystemFlushLog( stlParseParam       * aParam,
                                                            stlInt32              aNodePos,
                                                            qlpFlushLogBehavior   aBehavior )
{
    qlpAlterSystemFlushLog  * sAlterSystemFlushLog = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterSystemFlushLog ),
                            (void**) & sAlterSystemFlushLog,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSystemFlushLog = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSystemFlushLog->mType    = QLL_STMT_ALTER_SYSTEM_FLUSH_LOG_TYPE;
        sAlterSystemFlushLog->mNodePos = aNodePos;
        sAlterSystemFlushLog->mBehavior = aBehavior;
    }

    return sAlterSystemFlushLog;
}


/**
 * @brief ALTER SYSTEM CLEANUP PLAN parse tree 생성
 * @param[in]   aParam     Parse Parameter
 * @param[in]   aNodePos   Node Position
 *
 * @return qlpAlterSystemCleanupPlan 노드 
 */
inline qlpAlterSystemCleanupPlan * qlpMakeAlterSystemCleanupPlan( stlParseParam  * aParam,
                                                                  stlInt32         aNodePos )
{
    qlpAlterSystemCleanupPlan  * sAlterSystem = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterSystemCleanupPlan ),
                            (void**) & sAlterSystem,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSystem = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSystem->mType    = QLL_STMT_ALTER_SYSTEM_CLEANUP_PLAN_TYPE;
        sAlterSystem->mNodePos = aNodePos;
    }

    return sAlterSystem;
}

/**
 * @brief ALTER SYSTEM CLEANUP SESSION parse tree 생성
 * @param[in]   aParam     Parse Parameter
 * @param[in]   aNodePos   Node Position
 *
 * @return qlpAlterSystemCleanupSession 노드 
 */
inline qlpAlterSystemCleanupSession * qlpMakeAlterSystemCleanupSession( stlParseParam  * aParam,
                                                                        stlInt32         aNodePos )
{
    qlpAlterSystemCleanupSession  * sAlterSystem = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterSystemCleanupSession ),
                            (void**) & sAlterSystem,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSystem = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSystem->mType    = QLL_STMT_ALTER_SYSTEM_CLEANUP_SESSION_TYPE;
        sAlterSystem->mNodePos = aNodePos;
    }

    return sAlterSystem;
}

/**
 * @brief Alter System DISCONNECT SESSION 노드 생성
 * @param[in]   aParam             Parse Parameter
 * @param[in]   aNodePos           Node Position
 * @param[in]   aSessionID         Session ID
 * @param[in]   aSerialNum         Serial#
 * @param[in]   aIsPostTransaction POST_TRANSACTION 여부
 * @param[in]   aOption            Option
 *
 * @return qlpAlterSystemDisconnSession 노드 
 */
inline qlpAlterSystemDisconnSession * qlpMakeAlterSystemDisconnectSession( stlParseParam  * aParam,
                                                                           stlInt32         aNodePos,
                                                                           qlpValue       * aSessionID,
                                                                           qlpValue       * aSerialNum,
                                                                           stlBool          aIsPostTransaction,
                                                                           qlpValue       * aOption )
{
    qlpAlterSystemDisconnSession * sAlterSystemDisconnSession = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterSystemDisconnSession ),
                            (void**) & sAlterSystemDisconnSession,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSystemDisconnSession = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSystemDisconnSession->mType              = QLL_STMT_ALTER_SYSTEM_DISCONNECT_SESSION_TYPE;
        sAlterSystemDisconnSession->mNodePos           = aNodePos;
        sAlterSystemDisconnSession->mSessionID         = aSessionID;
        sAlterSystemDisconnSession->mSerialNum         = aSerialNum;
        sAlterSystemDisconnSession->mIsPostTransaction = aIsPostTransaction;
        sAlterSystemDisconnSession->mOption            = aOption;
    }

    return sAlterSystemDisconnSession;
}


/**
 * @brief Alter System KILL SESSION 노드 생성
 * @param[in]   aParam     Parse Parameter
 * @param[in]   aNodePos   Node Position
 * @param[in]   aSessionID Session ID
 * @param[in]   aSerialNum Serial#
 *
 * @return qlpAlterSystemKillSession 노드 
 */
inline qlpAlterSystemKillSession * qlpMakeAlterSystemKillSession( stlParseParam  * aParam,
                                                                  stlInt32         aNodePos,
                                                                  qlpValue       * aSessionID,
                                                                  qlpValue       * aSerialNum )
{
    qlpAlterSystemKillSession * sAlterSystemKillSession = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterSystemKillSession ),
                            (void**) & sAlterSystemKillSession,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSystemKillSession = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSystemKillSession->mType      = QLL_STMT_ALTER_SYSTEM_KILL_SESSION_TYPE;
        sAlterSystemKillSession->mNodePos   = aNodePos;
        sAlterSystemKillSession->mSessionID = aSessionID;
        sAlterSystemKillSession->mSerialNum = aSerialNum;
    }

    return sAlterSystemKillSession;
}


/**
 * @brief Alter Session (SET/RESET) 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodeType       Node Type
 * @param[in]   aNodePos        Node Position
 * @param[in]   aPropertyName   Property Name
 * @param[in]   aSetValue       Set Value
 *
 * @return qlpAlterSession 노드 
 */
inline qlpAlterSession * qlpMakeAlterSession( stlParseParam  * aParam,
                                              qllNodeType      aNodeType,
                                              stlInt32         aNodePos,
                                              qlpValue       * aPropertyName,
                                              qlpValue       * aSetValue )
{
    qlpAlterSession  * sAlterSession = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAlterSession ),
                            (void**) & sAlterSession,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAlterSession = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAlterSession->mType         = aNodeType;
        sAlterSession->mNodePos      = aNodePos;
        sAlterSession->mPropertyName = aPropertyName;
        sAlterSession->mSetValue     = aSetValue;
    }

    return sAlterSession;
}


/**
 * @brief pseudo column node 생성 
 * @param[in]   aParam       Parse Parameter
 * @param[in]   aPseudoColId Pseudo Column ID
 * @param[in]   aNodePos     Node Position
 * @param[in]   aNodeLen     Node Length
 * @param[in]   aArgs        Pseudo Column Argument List
 * 
 * @return qlpPseudoCol node
 */
inline qlpPseudoCol * qlpMakePseudoColumn( stlParseParam   * aParam,
                                           knlPseudoCol      aPseudoColId,
                                           stlInt32          aNodePos,
                                           stlInt32          aNodeLen,
                                           qlpList         * aArgs )
{
    qlpPseudoCol  * sColumn = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpPseudoCol ),
                            (void**) & sColumn,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sColumn = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sColumn->mType     = QLL_PSEUDO_COLUMN_TYPE;
        sColumn->mPseudoId = aPseudoColId;
        sColumn->mNodePos  = aNodePos;
        sColumn->mNodeLen  = aNodeLen;
        sColumn->mArgs     = aArgs;
    }

    return sColumn;
}


/**
 * @brief aggregation node 생성 
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aAggrId         Aggregation ID
 * @param[in]   aSetQuantifier  Set Quantifier
 * @param[in]   aArgs           Aggregation Argument List
 * @param[in]   aFilter         Filter
 * 
 * @return qlpAggregation node
 */
inline qlpAggregation * qlpMakeAggregation( stlParseParam       * aParam,
                                            stlInt32              aNodePos,
                                            knlBuiltInAggrFunc    aAggrId,
                                            qlpSetQuantifier      aSetQuantifier,
                                            qlpList             * aArgs,
                                            qllNode             * aFilter )
{
    qlpAggregation  * sAggregation  = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpAggregation ),
                            (void**) & sAggregation,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sAggregation = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sAggregation->mType             = QLL_AGGREGATION_TYPE;
        sAggregation->mNodePos          = aNodePos;

        sAggregation->mAggrId           = aAggrId;
        sAggregation->mSetQuantifier    = aSetQuantifier;
        sAggregation->mArgs             = aArgs;
        sAggregation->mFilter           = aFilter;
    }

    return sAggregation;
}


/**
 * @brief comment object node 생성 (COMMENT ON object)
 * @param[in]   aParam       Parse Parameter
 * @param[in]   aObjectType  Object Type
 * @param[in]   aNodePos     Node Position
 * @param[in]   aNonSchemaObjectName  Non-Schema Object Name
 * @param[in]   aSchemaObjectName     Schema Object Name
 * @param[in]   aColumnObjectName     Column Object Name
 * @return qlpCommentObject
 * @remarks
 */
inline qlpCommentObject * qlpMakeCommentObject( stlParseParam * aParam,
                                                ellObjectType   aObjectType,
                                                stlInt32        aNodePos,
                                                qlpValue      * aNonSchemaObjectName,
                                                qlpList       * aSchemaObjectName,
                                                qlpList       * aColumnObjectName )
{
    qlpCommentObject * sCommentObject = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpCommentObject ),
                            (void**) & sCommentObject,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sCommentObject = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sCommentObject->mType    = QLL_COMMENT_OBJECT_TYPE;
        sCommentObject->mNodePos = aNodePos;
        
        sCommentObject->mObjectType = aObjectType;

        switch (aObjectType)
        {
            case ELL_OBJECT_COLUMN:
                {
                    sCommentObject->mNonSchemaObjectName = NULL;
                    sCommentObject->mSchemaObjectName    = NULL;
                    sCommentObject->mColumnObjectName    =
                        qlpMakeColumnName( aParam, aColumnObjectName );
                    break;
                }
            case ELL_OBJECT_TABLE:
            case ELL_OBJECT_CONSTRAINT:
            case ELL_OBJECT_INDEX:
            case ELL_OBJECT_SEQUENCE:
                {
                    sCommentObject->mNonSchemaObjectName = NULL;
                    sCommentObject->mSchemaObjectName    =
                        qlpMakeObjectName( aParam, aSchemaObjectName );
                    sCommentObject->mColumnObjectName    = NULL;

                    break;
                }
            case ELL_OBJECT_SCHEMA:
            case ELL_OBJECT_TABLESPACE:
            case ELL_OBJECT_AUTHORIZATION:
            case ELL_OBJECT_PROFILE:
                {
                    sCommentObject->mNonSchemaObjectName = aNonSchemaObjectName;
                    sCommentObject->mSchemaObjectName    = NULL;
                    sCommentObject->mColumnObjectName    = NULL;
                    
                    break;
                }
            case ELL_OBJECT_CATALOG:
                {
                    sCommentObject->mNonSchemaObjectName = NULL;
                    sCommentObject->mSchemaObjectName    = NULL;
                    sCommentObject->mColumnObjectName    = NULL;
                    break;
                }
            default:
                {
                    STL_ASSERT(0);
                    break;
                }
        }
    }

    return sCommentObject;
}


/**
 * @brief comment statement 생성 
 * @param[in]   aParam       Parse Parameter
 * @param[in]   aNodePos     Node Position
 * @param[in]   aCommentObject Comment Object
 * @param[in]   aCommentString Comment String
 * @return qlpCommentOnStmt
 * @remarks
 */
inline qlpCommentOnStmt * qlpMakeCommentOnStmt( stlParseParam    * aParam,
                                                stlInt32           aNodePos,
                                                qlpCommentObject * aCommentObject,
                                                qlpValue         * aCommentString )
{
    qlpCommentOnStmt * sCommentStmt = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpCommentOnStmt ),
                            (void**) & sCommentStmt,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sCommentStmt = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sCommentStmt->mType = QLL_STMT_COMMENT_ON_TYPE;
        sCommentStmt->mNodePos = aNodePos;

        sCommentStmt->mCommentObject = aCommentObject;
        sCommentStmt->mCommentString = aCommentString;
    }

    return sCommentStmt;
}



/**
 * @brief Set Constraint 구문 생성 
 * @param[in]   aParam         Parse Parameter
 * @param[in]   aNodePos       Node Position
 * @param[in]   aConstMode     Constraint Mode(DEFERRED/IMMEDIATE)
 * @param[in]   aConstNameList Constraint Name List (nullable)
 * @return qlpCommentOnStmt
 * @remarks
 */
inline qlpSetConstraint * qlpMakeSetConstraint( stlParseParam        * aParam,
                                                stlInt32               aNodePos,
                                                qllSetConstraintMode   aConstMode,
                                                qlpList              * aConstNameList )
{
    qlpSetConstraint * sSetConst = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( sSetConst ),
                            (void**) & sSetConst,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSetConst = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sSetConst->mType    = QLL_STMT_SET_CONSTRAINTS_TYPE;
        sSetConst->mNodePos = aNodePos;

        sSetConst->mConstMode     = aConstMode;
        sSetConst->mConstNameList = aConstNameList;
    }

    return sSetConst;
}

/**
 * @brief transaction mode 노드 생성
 * @param[in]   aParam          Parse Parameter
 * @param[in]   aNodePos        Node Position
 * @param[in]   aAttr           Transaction Attribute
 * @param[in]   aValue          Transaction Attribute Value
 *
 * @return qlpTransactionMode 노드 
 */
inline qlpTransactionMode * qlpMakeSetTransactionMode( stlParseParam      * aParam,
                                                       stlInt32             aNodePos,
                                                       qlpTransactionAttr   aAttr,   
                                                       qlpValue           * aValue )
{
    qlpTransactionMode * sTransactionMode = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpTransactionMode ),
                            (void**) & sTransactionMode,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sTransactionMode    = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sTransactionMode->mType    = QLL_SET_TRANSACTION_MODE;
        sTransactionMode->mNodePos = aNodePos;
        sTransactionMode->mAttr    = aAttr;
        sTransactionMode->mValue   = aValue;
    }

    return sTransactionMode;
}


/**
 * @brief SET TRANSACTION 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aTransactionMode Transaction Mode
 *
 * @return qlpSetTransaction 노드 
 */
inline qlpSetTransaction * qlpMakeSetTransaction( stlParseParam      * aParam,
                                                  stlInt32             aNodePos,
                                                  qlpTransactionMode * aTransactionMode )
{
    qlpSetTransaction * sSetTransaction = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpSetTransaction ),
                            (void**) & sSetTransaction,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSetTransaction    = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sSetTransaction->mType    = QLL_STMT_SET_TRANSACTION_TYPE;
        sSetTransaction->mNodePos = aNodePos;
        sSetTransaction->mAttr    = aTransactionMode->mAttr;
        sSetTransaction->mValue   = aTransactionMode->mValue;
    }

    return sSetTransaction;
}


/**
 * @brief SET SESSION CHARACTERISTICS 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aTransactionMode Transaction Mode
 *
 * @return qlpSetSessionCharacteristics 노드 
 */
inline qlpSetSessionCharacteristics *
qlpMakeSetSessionCharacteristics( stlParseParam      * aParam,
                                  stlInt32             aNodePos,
                                  qlpTransactionMode * aTransactionMode )
{
    qlpSetSessionCharacteristics * sSessionCharacteristics = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpSetSessionCharacteristics ),
                            (void**) & sSessionCharacteristics,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSessionCharacteristics = NULL;
        aParam->mErrStatus      = STL_FAILURE;
    }
    else
    {
        sSessionCharacteristics->mType    = QLL_STMT_SET_SESSION_CHARACTERISTICS_TYPE;
        sSessionCharacteristics->mNodePos = aNodePos;
        sSessionCharacteristics->mAttr    = aTransactionMode->mAttr;
        sSessionCharacteristics->mValue   = aTransactionMode->mValue;
    }

    return sSessionCharacteristics;
}


/**
 * @brief SET SESSION AUTHORIZATION 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aUserName        User Name
 *
 * @return qlpSetSessionCharacteristics 노드 
 */
inline qlpSetSessionAuth * qlpMakeSetSessionAuth( stlParseParam * aParam,
                                                  stlInt32        aNodePos,
                                                  qlpValue      * aUserName )
{
    qlpSetSessionAuth * sSetSession = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpSetSessionAuth ),
                            (void**) & sSetSession,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sSetSession          = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sSetSession->mType     = QLL_STMT_SET_SESSION_AUTHORIZATION_TYPE;
        sSetSession->mNodePos  = aNodePos;
        sSetSession->mUserName = aUserName;
    }

    return sSetSession;
    
}

/**
 * @brief SET TIME ZONE 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aValue           Transaction Mode
 *
 * @return qlpSetSessionCharacteristics 노드 
 */
inline qlpSetTimeZone * qlpMakeSetTimeZone( stlParseParam * aParam,
                                            stlInt32        aNodePos,
                                            qlpValue      * aValue )
{
    qlpSetTimeZone * sTimeZone = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpSetTimeZone ),
                            (void**) & sTimeZone,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sTimeZone          = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sTimeZone->mType    = QLL_STMT_SET_TIMEZONE_TYPE;
        sTimeZone->mNodePos = aNodePos;
        sTimeZone->mValue   = aValue;
    }

    return sTimeZone;
}


/**
 * @brief DECLARE CURSOR (ISO Standard) parse tree 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aCursorName      Cursor Name 
 * @param[in]   aSensitivity     Cursor Sensitivity
 * @param[in]   aScrollability   Cursor Scrollability
 * @param[in]   aHoldability     Cursor Holdability
 * @param[in]   aCursorQuery     Cursor Query
 * @return qlpDeclareCursor Node
 * @remarks
 */
inline qlpDeclareCursor * qlpMakeDeclareCursorISO( stlParseParam * aParam,
                                                   stlInt32        aNodePos,
                                                   qlpValue      * aCursorName,
                                                   qlpValue      * aSensitivity,
                                                   qlpValue      * aScrollability,
                                                   qlpValue      * aHoldability,
                                                   qllNode       * aCursorQuery )
{
    qlpDeclareCursor * sDeclCursor = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDeclareCursor ),
                            (void**) & sDeclCursor,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDeclCursor = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDeclCursor->mType    = QLL_STMT_DECLARE_CURSOR_TYPE;
        sDeclCursor->mNodePos = aNodePos;

        sDeclCursor->mCursorName = aCursorName;

        sDeclCursor->mStandardType  = NULL;
        sDeclCursor->mSensitivity   = aSensitivity;
        sDeclCursor->mScrollability = aScrollability;
        sDeclCursor->mHoldability   = aHoldability;

        sDeclCursor->mCursorQuery = aCursorQuery;
    }
    
    return sDeclCursor;
}

/**
 * @brief DECLARE CURSOR (ODBC Standard) parse tree 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aStandardType    Cursor Standard Type
 * @param[in]   aCursorName      Cursor Name 
 * @param[in]   aHoldability     Cursor Holdability
 * @param[in]   aCursorQuery     Cursor Query
 * @return qlpDeclareCursor Node
 * @remarks
 */
inline qlpDeclareCursor * qlpMakeDeclareCursorODBC( stlParseParam * aParam,
                                                    stlInt32        aNodePos,
                                                    qlpValue      * aCursorName,
                                                    qlpValue      * aStandardType,
                                                    qlpValue      * aHoldability,
                                                    qllNode       * aCursorQuery )
{
    qlpDeclareCursor * sDeclCursor = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpDeclareCursor ),
                            (void**) & sDeclCursor,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDeclCursor = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDeclCursor->mType    = QLL_STMT_DECLARE_CURSOR_TYPE;
        sDeclCursor->mNodePos = aNodePos;

        sDeclCursor->mCursorName = aCursorName;

        sDeclCursor->mStandardType  = aStandardType;
        sDeclCursor->mSensitivity   = NULL;
        sDeclCursor->mScrollability = NULL;
        sDeclCursor->mHoldability   = aHoldability;

        sDeclCursor->mCursorQuery = aCursorQuery;
    }
    
    return sDeclCursor;
}


/**
 * @brief OPEN CURSOR parse tree 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aCursorName      Cursor Name 
 * @return qlpOpenCursor Node
 * @remarks
 */
inline qlpOpenCursor * qlpMakeOpenCursor( stlParseParam * aParam,
                                          stlInt32        aNodePos,
                                          qlpValue      * aCursorName )
{
    qlpOpenCursor * sOpenCursor = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpOpenCursor ),
                            (void**) & sOpenCursor,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sOpenCursor = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sOpenCursor->mType    = QLL_STMT_OPEN_CURSOR_TYPE;
        sOpenCursor->mNodePos = aNodePos;

        sOpenCursor->mCursorName = aCursorName;
    }
    
    return sOpenCursor;
}


/**
 * @brief CLOSE CURSOR parse tree 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aCursorName      Cursor Name 
 * @return qlpCloseCursor Node
 * @remarks
 */
inline qlpCloseCursor * qlpMakeCloseCursor( stlParseParam * aParam,
                                            stlInt32        aNodePos,
                                            qlpValue      * aCursorName )
{
    qlpCloseCursor * sCloseCursor = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpCloseCursor ),
                            (void**) & sCloseCursor,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sCloseCursor = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sCloseCursor->mType    = QLL_STMT_CLOSE_CURSOR_TYPE;
        sCloseCursor->mNodePos = aNodePos;

        sCloseCursor->mCursorName = aCursorName;
    }
    
    return sCloseCursor;
}


/**
 * @brief FETCH CURSOR parse tree 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aCursorName      Cursor Name
 * @param[in]   aFetchOrient     Fetch Orientation
 * @param[in]   aIntoList        Into List
 * @return qlpCloseCursor Node
 * @remarks
 */
inline qlpFetchCursor * qlpMakeFetchCursor( stlParseParam       * aParam,
                                            stlInt32              aNodePos,
                                            qlpValue            * aCursorName,
                                            qlpFetchOrientation * aFetchOrient,
                                            qlpIntoClause       * aIntoList )
{
    qlpFetchCursor * sFetchCursor = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpFetchCursor ),
                            (void**) & sFetchCursor,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sFetchCursor = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sFetchCursor->mType    = QLL_STMT_FETCH_CURSOR_TYPE;
        sFetchCursor->mNodePos = aNodePos;

        sFetchCursor->mCursorName    = aCursorName;
        sFetchCursor->mFetchOrient   = aFetchOrient->mFetchOrient;
        sFetchCursor->mFetchPosition = aFetchOrient->mFetchPosition;
        sFetchCursor->mIntoList      = aIntoList;
    }
    
    return sFetchCursor;
}


/**
 * @brief Fetch Orientation parse tree 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aFetchOrient     Fetch Orientation
 * @param[in]   aFetchPosition   Fetch Position
 * @return qlpFetchOrientation Node
 * @remarks
 */
inline qlpFetchOrientation * qlpMakeFetchOrientation( stlParseParam       * aParam,
                                                      stlInt32              aNodePos,
                                                      qllFetchOrientation   aFetchOrient,
                                                      qllNode             * aFetchPosition )
{
    qlpFetchOrientation * sFetchOrient = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpFetchOrientation ),
                            (void**) & sFetchOrient,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sFetchOrient = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sFetchOrient->mType    = QLL_STMT_CLOSE_CURSOR_TYPE;
        sFetchOrient->mNodePos = aNodePos;

        sFetchOrient->mFetchOrient = aFetchOrient;
        sFetchOrient->mFetchPosition = aFetchPosition;
    }
    
    return sFetchOrient;
}

/**
 * @brief Between parse tree 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aExpr1           Expr1 Node Value
 * @param[in]   aExpr2           Expr2 Node Value
 * @param[in]   aType            Between Func Type
 * @return qlpBetween Node
 * @remarks
 */
inline qlpBetween * qlpMakeBetween( stlParseParam   * aParam,
                                    qlpListElement  * aExpr1,
                                    qlpListElement  * aExpr2,
                                    qlpBetweenType    aType )
{
    qlpBetween     * sBetween = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpBetween ),
                            (void**) & sBetween,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sBetween = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sBetween->mElem1 = aExpr1;
        sBetween->mElem2 = aExpr2;
        sBetween->mType  = aType;
        
        switch( aType )
        {
            case QLP_BETWEEN_TYPE_BETWEEN_ASYMMETRIC :
                sBetween->mFuncID = KNL_BUILTIN_FUNC_BETWEEN;
                break;
                
            case QLP_BETWEEN_TYPE_BETWEEN_SYMMETRIC :
                sBetween->mFuncID = KNL_BUILTIN_FUNC_BETWEEN_SYMMETRIC;
                break;
                
            case QLP_BETWEEN_TYPE_NOT_BETWEEN_ASYMMETRIC :
                sBetween->mFuncID = KNL_BUILTIN_FUNC_NOT_BETWEEN;
                break;

            case QLP_BETWEEN_TYPE_NOT_BETWEEN_SYMMETRIC :
                sBetween->mFuncID = KNL_BUILTIN_FUNC_NOT_BETWEEN_SYMMETRIC;
                break;

            default :
                sBetween->mFuncID = KNL_BUILTIN_FUNC_INVALID;
                break;
        }
    }
    
    return sBetween;
}

/**
 * @brief order by parse tree 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aSortKey         sort key
 * @param[in]   aSortOrder       Sort Ordering
 * @param[in]   aNullOrder       Null Ordering
 * @return qllNode Node
 * @remarks
 */
inline qllNode * qlpMakeOrderBy( stlParseParam   * aParam,
                                 stlInt32          aNodePos,
                                 qllNode         * aSortKey,
                                 qlpValue        * aSortOrder,
                                 qlpValue        * aNullOrder )
{
    qlpOrderBy  * sOrderBy  = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpOrderBy ),
                            (void**) & sOrderBy,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sOrderBy = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sOrderBy->mType         = QLL_ORDER_BY_TYPE;
        sOrderBy->mNodePos      = aNodePos;
        sOrderBy->mSortKey      = aSortKey;
        sOrderBy->mSortOrder    = aSortOrder;
        sOrderBy->mNullOrder    = aNullOrder;
    }
    
    return (qllNode*)sOrderBy;
}

/**
 * @brief searched case에 대한 qlpCaseExpr node 생성.
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeLen         Node Length
 * @param[in]   aNodeType        Node Type
 * @param[in]   aFuncId          Function Id
 * @param[in]   aWhenClause      When Clauses List
 * @param[in]   aDefResult       Else Clause 의 default result   
 * @return qllNode Node
 * @remarks
 */
inline qllNode * qlpMakeCaseExprForSearchedCase( stlParseParam   * aParam,
                                                 stlInt32          aNodePos,
                                                 stlInt32          aNodeLen,
                                                 qllNodeType       aNodeType,
                                                 knlBuiltInFunc    aFuncId,
                                                 qlpList         * aWhenClause,
                                                 qllNode         * aDefResult )
{
    qlpCaseExpr   * sCaseExpr = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpCaseExpr ),
                            (void**) & sCaseExpr,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sCaseExpr = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sCaseExpr->mType       = aNodeType;
        sCaseExpr->mNodePos    = aNodePos;
        sCaseExpr->mNodeLen    = aNodeLen;

        sCaseExpr->mFuncId     = aFuncId;
        sCaseExpr->mExpr       = NULL;
        sCaseExpr->mWhenClause = aWhenClause;
        sCaseExpr->mDefResult  = aDefResult;
    }

    return (qllNode*)(sCaseExpr);
}

/**
 * @brief simple case, nullif, coalese 등을 searched case로 rewrite하는 과정에서 <BR>
 *        when 조건에 대한 argument가 1개인 function을 생성한다.
 * @param[in]  aParam          Parse Parameter
 * @param[in]  aArgument       function의 argument 정보
 * @param[in]  aNodePos        node position
 * @param[in]  aFunctionId     function ID
 */
inline qlpFunction * qlpMakeFunctionArgs1ForCaseWhenClause( stlParseParam   * aParam,
                                                            qllNode         * aArgument,
                                                            stlInt32          aNodePos,
                                                            knlBuiltInFunc    aFunctionId )
{
    qlpList            * sList     = NULL;
    qlpFunction        * sFunction = NULL;
    
    STL_DASSERT( aArgument != NULL );
    
    /* Function Argument List 구성 */
    sList = qlpMakeList( aParam,
                         QLL_POINTER_LIST_TYPE );
    
    /* Function Argument List Argument 구성 */
    qlpAddPtrValueToList( aParam,
                          sList,
                          aArgument );
    
    /* Function 구성 */
    sFunction = qlpMakeFunction( aParam,
                                 aFunctionId,
                                 aNodePos,
                                 0,
                                 sList );
    
    return sFunction;
}

/**
 * @brief simple case, nullif, coalese 등을 searched case로 rewrite하는 과정에서 <BR>
 *        when 조건에 대한 argument가 2개인 function을 생성한다.
 * @param[in]  aParam          Parse Parameter
 * @param[in]  aLeftArgument   function의 left argument 정보
 * @param[in]  aRightArgument  function의 right argument 정보
 * @param[in]  aNodePos        node position
 * @param[in]  aFunctionId     function ID
 */
inline qlpFunction * qlpMakeFunctionArgs2ForCaseWhenClause( stlParseParam   * aParam,
                                                            qllNode         * aLeftArgument,
                                                            qllNode         * aRightArgument,
                                                            stlInt32          aNodePos,
                                                            knlBuiltInFunc    aFunctionId )
{
    qlpList            * sList     = NULL;
    qlpFunction        * sFunction = NULL;

    STL_DASSERT( aLeftArgument != NULL );
    STL_DASSERT( aRightArgument != NULL );
    
    /* Function Argument List 구성 */
    sList = qlpMakeList( aParam,
                         QLL_POINTER_LIST_TYPE );
    
    /* Function Argument List Left Argument 구성 */
    qlpAddPtrValueToList( aParam,
                          sList,
                          aLeftArgument );
    
    /* Function Argument List Right Argument 구성 */
    qlpAddPtrValueToList( aParam,
                          sList,
                          aRightArgument );

    /* Function 구성 */
    sFunction = qlpMakeFunction( aParam,
                                 aFunctionId,
                                 aNodePos,
                                 0,
                                 sList );
    
    return sFunction;
}

/**
 * @brief simple case, nullif, coalese 등을 searched case로 rewrite하는 과정에서 <BR>
 *        when 조건에 대한 list function을 생성한다.
 * @param[in]  aParam         Parse Parameter
 * @param[in]  aLeftList      function의 left list 정보
 * @param[in]  aRightList     function의 right list 정보
 * @param[in]  aFunctionId    function ID
 */
inline qlpListFunc * qlpMakeListFunctionForCaseWhenCaluse( stlParseParam   * aParam,
                                                           qlpList         * aLeftList,
                                                           qlpList         * aRightList,
                                                           knlBuiltInFunc    aFunctionId )
{
    qlpRowExpr         * sLeftRow      = NULL;    
    qlpList            * sLeftList     = NULL;
    qlpListCol         * sLeftListCol  = NULL;
    qlpRowExpr         * sRightRow     = NULL;    
    qlpList            * sRightList    = NULL;
    qlpListCol         * sRightListCol = NULL;
    qlpListFunc        * sListFunc     = NULL;
    
    qlpList            * sList         = NULL;
    
    /******************
     * left list 
     ******************/
    
    /* left list row expr */
    sLeftRow = qlpMakeRowExpr( aParam,
                               ((qllNode*)aLeftList)->mNodePos,
                               aLeftList );
    
    /* List 구성 */    
    sLeftList = qlpMakeList( aParam,
                             QLL_POINTER_LIST_TYPE );
    
    /* ListElement 설정 */
    qlpAddPtrValueToList( aParam,
                          sLeftList,
                          sLeftRow );
    
    /* ListCol 생성 */
    sLeftListCol = qlpMakeListColumn( aParam,
                                      ((qllNode*)aLeftList)->mNodePos,
                                      aFunctionId,
                                      DTL_LIST_PREDICATE_LEFT,
                                      sLeftList );
    
    /******************
     * right list
     ******************/
    
    /* right list row expr */
    sRightRow = qlpMakeRowExpr( aParam,
                                ((qllNode*)aRightList)->mNodePos,
                                aRightList );
    
    /* List 구성 */    
    sRightList = qlpMakeList( aParam,
                              QLL_POINTER_LIST_TYPE );
    
    /* ListElement 설정 */
    qlpAddPtrValueToList( aParam,
                          sRightList,
                          sRightRow );
    
    /* ListCol 생성 */
    sRightListCol = qlpMakeListColumn( aParam,
                                       ((qllNode*)aRightList)->mNodePos,
                                       aFunctionId,
                                       DTL_LIST_PREDICATE_RIGHT,
                                       sRightList );

    /******************
     * list function argument  
     ******************/
    
    /* List 구성 */
    sList = qlpMakeList( aParam,
                         QLL_POINTER_LIST_TYPE );
    
    /* ListElement 설정 */
    qlpAddPtrValueToList( aParam,
                          sList,
                          sRightListCol );

    /* ListElement 설정 */
    qlpAddPtrValueToList( aParam,
                          sList,
                          sLeftListCol );
    
    /******************
     * list function 구성
     ******************/
    sListFunc = qlpMakeListFunction( aParam,
                                     ((qllNode*)(aRightList->mHead->mData.mPtrValue))->mNodePos,
                                     0,
                                     ((qllNode*)(aRightList->mHead->mData.mPtrValue))->mNodePos,
                                     aFunctionId,
                                     sList );
    
    /******************
     * function 완료
     ******************/
    
    return sListFunc;
}

/**
 * @brief simple case에 대한 qlpCaseExpr node 생성(searched case로 rewrite한다).
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeLen         Node Length
 * @param[in]   aNodeType        Node Type
 * @param[in]   aFuncId          Function Id
 * @param[in]   aExpr            Expr List
 * @param[in]   aWhenClause      When Clauses List
 * @param[in]   aDefResult       Else Clause 의 default result   
 * @return qllNode Node
 * @remarks
 */
inline qllNode * qlpMakeCaseExprForSimpleCase( stlParseParam   * aParam,
                                               stlInt32          aNodePos,
                                               stlInt32          aNodeLen,
                                               qllNodeType       aNodeType,
                                               knlBuiltInFunc    aFuncId,
                                               qlpList         * aExpr,
                                               qlpList         * aWhenClause,
                                               qllNode         * aDefResult )
{
    qlpCaseExpr        * sCaseExpr = NULL;

    qlpCaseWhenClause  * sCaseWhenClause     = NULL;
    qlpListElement     * sWhenClauseElement  = NULL;
    qlpListFunc        * sListFunc           = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpCaseExpr ),
                            (void**) & sCaseExpr,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sCaseExpr = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        /***********************************************
         * SIMPLE CASE 을 아래와 같이 변경.
         * 
         * CASE I1 WHEN 1 THEN 'A'
         *         WHEN 2 THEN 'B'
         *         WHEN 3 THEN 'C'
         *         ELSE 999
         *      END
         * ===>
         * CASE WHEN I1 = 1 THEN 'A'
         *      WHEN I1 = 2 THEN 'B'
         *      WHEN I1 = 3 THEN 'C'
         *      ELSE 999
         *      
         ***********************************************/
        
        sCaseExpr->mType       = aNodeType;
        sCaseExpr->mNodePos    = aNodePos;
        sCaseExpr->mNodeLen    = aNodeLen;
        
        sCaseExpr->mFuncId     = aFuncId;
        sCaseExpr->mExpr       = NULL;
        sCaseExpr->mWhenClause = aWhenClause;
        sCaseExpr->mDefResult  = aDefResult;
        
        QLP_LIST_FOR_EACH( aWhenClause, sWhenClauseElement )
        {
            /**
             * 아래와 같이 변경
             *
             * CASE I1 WHEN 1 THEN 'A'
             *     -----------
             * =>
             * CASE WHEN I1 = 1 THEN 'A'
             *     -------------  
             *
             *     
             * CASE (I1,I2,I3) WHEN (1,2,3) THEN 'A'
             *     -------------------------
             * =>
             * CASE WHEN (I1,I2,I3) = (1,2,3) THEN 'A'
             *     ---------------------------  
             */

            sCaseWhenClause = (qlpCaseWhenClause *)QLP_LIST_GET_POINTER_VALUE( sWhenClauseElement );        
                
            sListFunc = qlpMakeListFunctionForCaseWhenCaluse( aParam,
                                                              aExpr,
                                                              (qlpList *)(sCaseWhenClause->mWhen),
                                                              KNL_BUILTIN_FUNC_IS_EQUAL );
                
            /* qlpCaseWhenClause->mWhen의 포인터를 변경한다. */
            sCaseWhenClause->mWhen = (qllNode *)sListFunc;
        }
    }
    
    return (qllNode*)(sCaseExpr);
}

/**
 * @brief nullif expr에 대한 qlpCaseExpr node 생성(searched case로 rewrite한다).
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeLen         Node Length
 * @param[in]   aNodeType        Node Type
 * @param[in]   aFuncId          Function Id
 * @param[in]   aExpr            Expr List
 * @return qllNode Node
 * @remarks
 */
inline qllNode * qlpMakeCaseExprForNullif( stlParseParam   * aParam,
                                           stlInt32          aNodePos,
                                           stlInt32          aNodeLen,
                                           qllNodeType       aNodeType,
                                           knlBuiltInFunc    aFuncId,
                                           qlpList         * aExpr )
{
    qlpCaseExpr        * sCaseExpr = NULL;
    
    qlpCaseWhenClause  * sCaseWhenClause = NULL;    
    qlpFunction        * sFunction       = NULL;
    qlpList            * sCaseWhenClauseList = NULL;
    qlpListElement     * sListElement    = NULL;
    qllNode            * sExpr1          = NULL;
    qlpValue           * sNullConstValue = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpCaseExpr ),
                            (void**) & sCaseExpr,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sCaseExpr = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        /***********************************************
         * NULLIF( expr1, expr2 )를 아래와 같이 변경
         * ==>
         *      CASE WHEN expr1 = expr2 THEN NULL
         *           ELSE expr1
         *      END     
         ***********************************************/

        sCaseExpr->mType       = aNodeType;
        sCaseExpr->mNodePos    = aNodePos;
        sCaseExpr->mNodeLen    = aNodeLen;
        
        sCaseExpr->mFuncId     = aFuncId;
        sCaseExpr->mExpr       = aExpr;
        sCaseExpr->mWhenClause = NULL;
        sCaseExpr->mDefResult  = NULL;
        
        /**
         *  CaseWhenCaluase 구성.
         *  WHEN expr1 = expr2 THEN NULL 구성.
         */
    
        /**
         * expr1 = expr2 
         */
        
        sFunction = qlpMakeFunction( aParam,
                                     KNL_BUILTIN_FUNC_IS_EQUAL,
                                     aNodePos,
                                     0,
                                     aExpr );
        
        /*
         * NULL constant value
         */
        
        sNullConstValue = qlpMakeNullConstant( aParam,
                                               0 );
        
        /**
         * CaseWhen Clause
         */

        sCaseWhenClause = (qlpCaseWhenClause*)qlpMakeCaseWhenClause( aParam,
                                                                     (qllNode*)sFunction,
                                                                     (qllNode*)sNullConstValue );
        
        /**
         * sCaseWhenClausesList 구성.
         */
        
        sCaseWhenClauseList = qlpMakeList( aParam,
                                           QLL_POINTER_LIST_TYPE );

        qlpAddPtrValueToList( aParam,
                              sCaseWhenClauseList,
                              sCaseWhenClause );
        
        /**
         *  ELSE expr1 
         */    
        
        sListElement = QLP_LIST_GET_FIRST( aExpr );
        sExpr1       = (qllNode*)QLP_LIST_GET_POINTER_VALUE(sListElement);
        
        /**
         * qlpCaseExpr 완료.
         */
        
        sCaseExpr->mWhenClause = sCaseWhenClauseList;
        sCaseExpr->mDefResult  = sExpr1;
        
    }
    
    return (qllNode *)sCaseExpr;
}

/**
 * @brief coalesce expr에 대한 qlpCaseExpr node 생성(searched case로 rewrite한다).
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeLen         Node Length
 * @param[in]   aNodeType        Node Type
 * @param[in]   aFuncId          Function Id
 * @param[in]   aExpr            Expr List
 * @return qllNode Node
 * @remarks
 */
inline qllNode * qlpMakeCaseExprForCoalesce( stlParseParam   * aParam,
                                             stlInt32          aNodePos,
                                             stlInt32          aNodeLen,
                                             qllNodeType       aNodeType,
                                             knlBuiltInFunc    aFuncId,
                                             qlpList         * aExpr )
{
    qlpCaseExpr        * sCaseExpr = NULL;
    
    qlpCaseWhenClause  * sCaseWhenClause = NULL;    
    qlpFunction        * sFunction       = NULL;
    qlpList            * sCaseWhenClauseList = NULL;
    qlpListElement     * sExprListElement    = NULL;    
    qllNode            * sExpr           = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpCaseExpr ),
                            (void**) & sCaseExpr,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sCaseExpr = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        /***********************************************
         * COALESCE( expr1, expr2, expr3, ....., exprN )를 아래와 같이 변경
         * ==>
         *      CASE WHEN expr1 IS NOT NULL THEN expr1
         *           WHEN expr2 IS NOT NULL THEN expr2
         *           WHEN expr3 IS NOT NULL THEN expr3
         *           ...
         *           ELSE exprN
         *      END     
         ***********************************************/

        sCaseExpr->mType       = aNodeType;
        sCaseExpr->mNodePos    = aNodePos;
        sCaseExpr->mNodeLen    = aNodeLen;
        
        sCaseExpr->mFuncId     = aFuncId;
        sCaseExpr->mExpr       = aExpr;
        sCaseExpr->mWhenClause = NULL;
        sCaseExpr->mDefResult  = NULL;
        
        /**
         * CaseWhenClause List
         */
        
        sCaseWhenClauseList = qlpMakeList( aParam,
                                           QLL_POINTER_LIST_TYPE );
        
        /**
         *  CaseWhenCaluase 구성.
         *  WHEN expr1 is not null THEN expr1 구성.
         */
        
        QLP_LIST_FOR_EACH( aExpr, sExprListElement )
        {
            if( sExprListElement == QLP_LIST_GET_LAST(aExpr) )
            {
                break;
            }
            
            /**
             * expr1 IS NOT NULL
             */
            
            sExpr = QLP_LIST_GET_POINTER_VALUE( sExprListElement );

            sFunction = qlpMakeFunctionArgs1ForCaseWhenClause( aParam,
                                                               sExpr,
                                                               aNodePos,
                                                               KNL_BUILTIN_FUNC_IS_NOT_NULL );
            
            /**
             * CaseWhen Clause
             *
             * WHEN expr1 is not null THEN expr1
             */

            sCaseWhenClause = (qlpCaseWhenClause*)qlpMakeCaseWhenClause( aParam,
                                                                         (qllNode*)sFunction,
                                                                         (qllNode *)sExpr );            
            
            /**
             * sCaseWhenClausesList 구성.
             */

            /* List Element 설정 */
            qlpAddPtrValueToList( aParam,
                                  sCaseWhenClauseList,
                                  sCaseWhenClause );
        }
        
        STL_DASSERT( sCaseWhenClauseList->mCount == ( aExpr->mCount - 1 ) );
        
        /**
         *  ELSE expr1 
         */    
        
        sExpr  = (qllNode*)QLP_LIST_GET_POINTER_VALUE(sExprListElement);
        
        /**
         * qlpCaseExpr 완료.
         */
        
        sCaseExpr->mWhenClause = sCaseWhenClauseList;
        sCaseExpr->mDefResult  = sExpr;
    }
    
    return (qllNode*)(sCaseExpr);
}

/**
 * @brief decode expr에 대한 qlpCaseExpr node 생성(searched case로 rewrite한다).
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeLen         Node Length
 * @param[in]   aNodeType        Node Type
 * @param[in]   aFuncId          Function Id
 * @param[in]   aExpr            Expr List
 * @return qllNode Node
 * @remarks
 */
inline qllNode * qlpMakeCaseExprForDecode( stlParseParam   * aParam,
                                           stlInt32          aNodePos,
                                           stlInt32          aNodeLen,
                                           qllNodeType       aNodeType,
                                           knlBuiltInFunc    aFuncId,
                                           qlpList         * aExpr )
{
    qlpCaseExpr        * sCaseExpr = NULL;
    
    qlpCaseWhenClause  * sCaseWhenClause     = NULL;    
    qlpFunction        * sOrFunction         = NULL;    
    qlpFunction        * sEqualFunction      = NULL;
    qlpFunction        * sAndFunction        = NULL;
    qlpFunction        * sLeftIsNullFunction = NULL;
    qlpFunction        * sRightIsNullFunction = NULL;
    qlpList            * sCaseWhenClauseList = NULL;
    qlpListElement     * sExprListElement    = NULL;    
    qllNode            * sExpr               = NULL;
    qllNode            * sSearch             = NULL;
    qllNode            * sResult             = NULL;
    qlpValue           * sDefaultNullValue   = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpCaseExpr ),
                            (void**) & sCaseExpr,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sCaseExpr = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        /***********************************************
         * DECODE( expr, search1, result1, search2, result2 ..., default) 를 아래와 같이 변경
         * ==>
         *      CASE WHEN (expr = search1) or (expr is null and search1 is null) THEN result1
         *           WHEN (expr = search2) or (expr is null and search2 is null) THEN result2
         *           ...
         *           ELSE default
         *      END     
         ***********************************************/

        sCaseExpr->mType       = aNodeType;
        sCaseExpr->mNodePos    = aNodePos;
        sCaseExpr->mNodeLen    = aNodeLen;
        
        sCaseExpr->mFuncId     = aFuncId;
        sCaseExpr->mExpr       = aExpr;
        sCaseExpr->mWhenClause = NULL;
        sCaseExpr->mDefResult  = NULL;
        
        /**
         * CaseWhenClause List
         */

        sCaseWhenClauseList = qlpMakeList( aParam,
                                           QLL_POINTER_LIST_TYPE );        
        
        /**
         *  CaseWhenCaluase 구성.
         *  WHEN절의 expr1 = search1 구성.
         */

        /**
         * DECODE( expr, search1, result1, search2, result2 ..., default) 의
         * expr 을 구한다. 
         */

        sExprListElement = QLP_LIST_GET_FIRST( aExpr );
        sExpr = QLP_LIST_GET_POINTER_VALUE( sExprListElement );
        
        /**
         * DECODE( expr, search1, result1, search2, result2 ..., default) 의
         * search, result를 찾아 when ... then 구문을 만든다.
         */
     
        sExprListElement = QLP_LIST_GET_NEXT_ELEM( sExprListElement );
     
        QLP_LIST_FOR_EACH_ELEM( sExprListElement, sExprListElement )
        {
            if( QLP_LIST_GET_NEXT_ELEM( sExprListElement ) == NULL )
            {
                break;
            }
         
            /**
             * expr = search
             */
         
            sSearch = QLP_LIST_GET_POINTER_VALUE( sExprListElement );

            sEqualFunction = qlpMakeFunctionArgs2ForCaseWhenClause( aParam,
                                                                    sExpr,
                                                                    sSearch,
                                                                    aNodePos,
                                                                    KNL_BUILTIN_FUNC_IS_EQUAL );
            
            /**
             * expr is null and search is null
             */
            
            /* expr IS NULL Function */
            sLeftIsNullFunction = qlpMakeFunctionArgs1ForCaseWhenClause( aParam,
                                                                         sExpr,
                                                                         aNodePos,
                                                                         KNL_BUILTIN_FUNC_IS_NULL );
            
            /* search IS NULL Function */
            sRightIsNullFunction = qlpMakeFunctionArgs1ForCaseWhenClause( aParam,
                                                                          sSearch,
                                                                          aNodePos,
                                                                          KNL_BUILTIN_FUNC_IS_NULL );
            
            /* AND Function ( expr is null and search is null ) */
            sAndFunction = qlpMakeFunctionArgs2ForCaseWhenClause( aParam,
                                                                  (qllNode*)sLeftIsNullFunction,
                                                                  (qllNode*)sRightIsNullFunction,
                                                                  aNodePos,
                                                                  KNL_BUILTIN_FUNC_AND );
            
            /**
             * (expr = search) OR (expr is null and search is null)
             */
            
            /* OR Function ( expr is null and search is null ) */
            sOrFunction = qlpMakeFunctionArgs2ForCaseWhenClause( aParam,
                                                                 (qllNode*)sEqualFunction,
                                                                 (qllNode*)sAndFunction,
                                                                 aNodePos,
                                                                 KNL_BUILTIN_FUNC_OR );
            
            /**
             * CaseWhen Clause
             *
             * WHEN (expr = search) or (expr is null and expr is null) THEN result
             */
            
            sExprListElement = QLP_LIST_GET_NEXT_ELEM( sExprListElement );
            sResult = QLP_LIST_GET_POINTER_VALUE( sExprListElement );
            
            sCaseWhenClause = (qlpCaseWhenClause*)qlpMakeCaseWhenClause( aParam,
                                                                         (qllNode*)sOrFunction,
                                                                         (qllNode*)sResult );
            
            /**
             * sCaseWhenClausesList 구성.
             */
            
            /* List Element 설정 */
            qlpAddPtrValueToList( aParam,
                                  sCaseWhenClauseList,
                                  sCaseWhenClause );
        }
        
        if( sExprListElement != NULL )
        {
            /**
             *  ELSE default
             */    
        
            sResult  = (qllNode*)QLP_LIST_GET_POINTER_VALUE(sExprListElement);        
        }
        else
        {
            /**
             * ELSE NULL
             */
            sDefaultNullValue = qlpMakeNullConstant( aParam,
                                                     0 );
        
            sResult = (qllNode *)sDefaultNullValue;
        }
        
        /**
         * qlpCaseExpr 완료.
         */
        
        sCaseExpr->mWhenClause = sCaseWhenClauseList;
        sCaseExpr->mDefResult  = sResult;
    }
    
    return (qllNode*)sCaseExpr;
}

/**
 * @brief Nvl, Nvl2 에 대한 qlpCaseExpr node 생성(searched case로 rewrite한다).
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeLen         Node Length
 * @param[in]   aNodeType        Node Type
 * @param[in]   aFuncId          Function Id
 * @param[in]   aExpr            Expr List
 * @return qllNode Node
 * @remarks
 */
inline qllNode * qlpMakeCaseExprForNvl( stlParseParam   * aParam,
                                        stlInt32          aNodePos,
                                        stlInt32          aNodeLen,
                                        qllNodeType       aNodeType,
                                        knlBuiltInFunc    aFuncId,
                                        qlpList         * aExpr )
{
    qlpCaseExpr        * sCaseExpr = NULL;
    
    qlpCaseWhenClause  * sCaseWhenClause = NULL;    
    qlpFunction        * sFunction       = NULL;
    qlpList            * sCaseWhenClauseList = NULL;
    qlpListElement     * sExprListElement    = NULL;    
    qllNode            * sExpr           = NULL;
    qllNode            * sThenExpr       = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpCaseExpr ),
                            (void**) & sCaseExpr,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sCaseExpr = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        /***********************************************
         * NVL( expr1, expr2 )를 아래와 같이 변경
         * ==>
         *      CASE WHEN expr1 IS NOT NULL THEN expr1
         *           ELSE expr2
         *      END
         *
         * NVL2( expr1, expr2, expr3 )를 아래와 같이 변경
         * ==>
         *      CASE WHEN expr1 IS NOT NULL THEN expr2
         *           ELSE expr3
         *      END
         ***********************************************/
        
        sCaseExpr->mType       = aNodeType;
        sCaseExpr->mNodePos    = aNodePos;
        sCaseExpr->mNodeLen    = aNodeLen;
        
        sCaseExpr->mFuncId     = aFuncId;
        sCaseExpr->mExpr       = aExpr;
        sCaseExpr->mWhenClause = NULL;
        sCaseExpr->mDefResult  = NULL;
        
        /**
         *  CaseWhenCaluase 구성.
         *  WHEN expr1 is not null THEN expr1 구성.
         */
        
        /**
         * expr1 IS NOT NULL
         */
        
        sExprListElement = QLP_LIST_GET_FIRST( aExpr );
        sExpr = QLP_LIST_GET_POINTER_VALUE( sExprListElement );
        
        sFunction = qlpMakeFunctionArgs1ForCaseWhenClause( aParam,
                                                           sExpr,
                                                           aNodePos,
                                                           KNL_BUILTIN_FUNC_IS_NOT_NULL );
        
        /**
         * CaseWhen Clause
         *
         * WHEN expr1 is not null THEN expr1
         */
        
        if( aFuncId == KNL_BUILTIN_FUNC_NVL2 )
        {
            sExprListElement = QLP_LIST_GET_NEXT_ELEM( sExprListElement );
            sThenExpr = (qllNode *)QLP_LIST_GET_POINTER_VALUE( sExprListElement );
        }
        else
        {
            /*
             * KNL_BUILTIN_FUNC_NVL
             */
            sThenExpr = (qllNode *)sExpr;
        }
        
        sCaseWhenClause = (qlpCaseWhenClause*)qlpMakeCaseWhenClause( aParam,
                                                                     (qllNode*)sFunction,
                                                                     (qllNode*)sThenExpr );
        
        /**
         * sCaseWhenClausesList 구성.
         */
        
        sCaseWhenClauseList = qlpMakeList( aParam,
                                           QLL_POINTER_LIST_TYPE );
        
        /* List Element 설정 */
        qlpAddPtrValueToList( aParam,
                              sCaseWhenClauseList,
                              sCaseWhenClause );
        /**
         *  ELSE expr1 
         */    
        
        sExprListElement = QLP_LIST_GET_NEXT_ELEM( sExprListElement );
        sExpr  = (qllNode*)QLP_LIST_GET_POINTER_VALUE(sExprListElement);

        /**
         * qlpCaseExpr 완료.
         */

        sCaseExpr->mWhenClause = sCaseWhenClauseList;
        sCaseExpr->mDefResult  = sExpr;
    }
    
    return (qllNode*)(sCaseExpr);
}

/**
 * @brief case2 expr에 대한 qlpCaseExpr node 생성(searched case로 rewrite한다).
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeLen         Node Length
 * @param[in]   aNodeType        Node Type
 * @param[in]   aFuncId          Function Id
 * @param[in]   aExpr            Expr List
 * @return qllNode Node
 * @remarks
 */
inline qllNode * qlpMakeCaseExprForCase2( stlParseParam   * aParam,
                                          stlInt32          aNodePos,
                                          stlInt32          aNodeLen,
                                          qllNodeType       aNodeType,
                                          knlBuiltInFunc    aFuncId,
                                          qlpList         * aExpr )
{
    qlpCaseExpr        * sCaseExpr = NULL;
    
    qlpCaseWhenClause  * sCaseWhenClause     = NULL;    
    qlpList            * sCaseWhenClauseList = NULL;
    qlpListElement     * sExprListElement    = NULL;    
    qllNode            * sExpr               = NULL;
    qllNode            * sResult             = NULL;
    qlpValue           * sDefaultNullValue   = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpCaseExpr ),
                            (void**) & sCaseExpr,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sCaseExpr = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        /***********************************************
         * CASE2( expr1, result1, expr2, result2 ..., default) 를 아래와 같이 변경
         * ==>
         *      CASE WHEN expr1 THEN result1
         *           WHEN expr2 THEN result2
         *           ...
         *           ELSE default
         *      END     
         ***********************************************/

        sCaseExpr->mType       = aNodeType;
        sCaseExpr->mNodePos    = aNodePos;
        sCaseExpr->mNodeLen    = aNodeLen;
        
        sCaseExpr->mFuncId     = aFuncId;
        sCaseExpr->mExpr       = aExpr;
        sCaseExpr->mWhenClause = NULL;
        sCaseExpr->mDefResult  = NULL;        
        
        /**
         * CaseWhenClause List
         */
        
        sCaseWhenClauseList = qlpMakeList( aParam,
                                           QLL_POINTER_LIST_TYPE );
        
        /**
         * CASE2( expr1, result1, expr2, result2 ..., default) 의
         * expr, result를 찾아 when ... then 구문을 만든다.
         */
        
        QLP_LIST_FOR_EACH( aExpr, sExprListElement )
        {
            if( QLP_LIST_GET_NEXT_ELEM( sExprListElement ) == NULL )
            {
                break;
            }
            
            sExpr = QLP_LIST_GET_POINTER_VALUE( sExprListElement );
            
            sExprListElement = QLP_LIST_GET_NEXT_ELEM( sExprListElement );
            sResult = QLP_LIST_GET_POINTER_VALUE( sExprListElement );
            
            /**
             * CaseWhen Clause
             *
             * WHEN expr THEN result
             */
            
            sCaseWhenClause = (qlpCaseWhenClause*)qlpMakeCaseWhenClause( aParam,
                                                                         (qllNode*)sExpr,
                                                                         (qllNode*)sResult );
            /**
             * sCaseWhenClauseList 구성.
             */
            
            /* List Element 설정 */
            qlpAddPtrValueToList( aParam,
                                  sCaseWhenClauseList,
                                  sCaseWhenClause );
        }
        
        if( sExprListElement != NULL )
        {
            /**
             *  ELSE default
             */    
        
            sResult  = (qllNode*)QLP_LIST_GET_POINTER_VALUE(sExprListElement);        
        }
        else
        {
            /**
             * ELSE NULL
             */
            sDefaultNullValue = qlpMakeNullConstant( aParam,
                                                     0 );
            
            sResult = (qllNode *)sDefaultNullValue;
        }
        
        /**
         * qlpCaseExpr 완료.
         */
        
        sCaseExpr->mWhenClause = sCaseWhenClauseList;
        sCaseExpr->mDefResult  = sResult;
    }
    
    return (qllNode*)(sCaseExpr);
}

/**
 * @brief qlpCaseWhenClause node 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aWhen            When condition expression
 * @param[in]   aThen            Then result 
 * @return qllNode Node
 * @remarks
 */
inline qllNode * qlpMakeCaseWhenClause( stlParseParam   * aParam,
                                        qllNode         * aWhen,
                                        qllNode         * aThen )
{
    qlpCaseWhenClause  * sCaseWhenClause = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpCaseWhenClause ),
                            (void**) & sCaseWhenClause,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sCaseWhenClause = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sCaseWhenClause->mWhen  = aWhen;
        sCaseWhenClause->mThen  = aThen;
    }

    return (qllNode*)(sCaseWhenClause);
}



/**
 * @brief group by parse tree 생성
 * @param[in]   aParam             Parse Parameter
 * @param[in]   aNodePos           Node Position
 * @param[in]   aGroupingElemList  Grouping Element List
 * @return qllNode Node
 * @remarks
 */
inline qllNode * qlpMakeGroupBy( stlParseParam   * aParam,
                                 stlInt32          aNodePos,
                                 qlpList         * aGroupingElemList )
{
    qlpGroupBy  * sGroupBy  = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpGroupBy ),
                            (void**) & sGroupBy,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sGroupBy = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sGroupBy->mType             = QLL_GROUP_BY_TYPE;
        sGroupBy->mNodePos          = aNodePos;
        sGroupBy->mGroupingElemList = aGroupingElemList;
    }
    
    return (qllNode*)sGroupBy;
}


/**
 * @brief grouping element parse tree 생성
 * @param[in]   aParam             Parse Parameter
 * @param[in]   aNodePos           Node Position
 * @param[in]   aGroupingType      Grouping Element Type
 * @param[in]   aGroupingElem      Grouping Element
 * @return qllNode Node
 * @remarks
 */
inline qllNode * qlpMakeGroupingElem( stlParseParam   * aParam,
                                      stlInt32          aNodePos,
                                      qlpGroupingType   aGroupingType,
                                      qlpList         * aGroupingElem )
{
    qlpGroupingElem  * sGroupingElem  = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpGroupingElem ),
                            (void**) & sGroupingElem,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sGroupingElem = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sGroupingElem->mType         = QLL_GROUPING_ELEM_TYPE;
        sGroupingElem->mNodePos      = aNodePos;
        sGroupingElem->mGroupingType = aGroupingType;
        sGroupingElem->mGroupingElem = aGroupingElem;
    }
    
    return (qllNode*)sGroupingElem;
}


/**
 * @brief having parse tree 생성
 * @param[in]   aParam             Parse Parameter
 * @param[in]   aNodePos           Node Position
 * @param[in]   aCondition         Having Condition
 * @return qllNode Node
 * @remarks
 */
inline qllNode * qlpMakeHaving( stlParseParam   * aParam,
                                stlInt32          aNodePos,
                                qllNode         * aCondition )
{
    qlpHaving  * sHaving  = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpHaving ),
                            (void**) & sHaving,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sHaving = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sHaving->mType      = QLL_GROUP_BY_TYPE;
        sHaving->mNodePos   = aNodePos;
        sHaving->mCondition = aCondition;
    }
    
    return (qllNode*)sHaving;
}


/**
 * @brief qlpMakeListFunc 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aNodeLen         Node Length 
 * @param[in]   aOperatorPos     Operator Position
 * @param[in]   aFunc            Built In Func
 * @param[in]   aList            Member value identifier List
 * 
 * @return qlpListFunc node
 */
inline qlpListFunc * qlpMakeListFunction( stlParseParam       * aParam,
                                          stlInt32              aNodePos,
                                          stlInt32              aNodeLen,
                                          stlInt32              aOperatorPos,
                                          knlBuiltInFunc        aFunc,
                                          qlpList             * aList )
{
    
    qlpListFunc   * sListFunc = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpListFunc),
                            (void**) & sListFunc,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sListFunc = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sListFunc->mType        = QLL_LIST_FUNCTION_TYPE;
        sListFunc->mNodePos     = aNodePos;
        sListFunc->mNodeLen     = aNodeLen;

        sListFunc->mOperatorPos = aOperatorPos;
        sListFunc->mFuncId      = aFunc;
        
        sListFunc->mArgs        = aList;
    }

    return sListFunc;
}


/**
 * @brief qlpMakeListCol 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aFunc            Built In Func
 * @param[in]   aPredicate       set predicate
 * @param[in]   aList            Member value identifier List
 * 
 * @return qlpListCol node
 */
inline qlpListCol * qlpMakeListColumn( stlParseParam       * aParam,
                                       stlInt32              aNodePos,
                                       knlBuiltInFunc        aFunc,
                                       dtlListPredicate      aPredicate,
                                       qlpList             * aList  )
{
    
    qlpListCol   * sListCol = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpListCol),
                            (void**) & sListCol,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sListCol = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sListCol->mType    = QLL_LIST_COLUMN_TYPE;

        sListCol->mFuncId       = aFunc;
        sListCol->mNodePos      = aNodePos;
        sListCol->mPredicate    = aPredicate;
        sListCol->mArgCount     = aList->mCount;
        
        sListCol->mArgs         = aList;
    }

    return sListCol;
}


/**
 * @brief qlpMakeRowExpr 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aList            Member value identifier List
 * 
 * @return qlpRowExpr node
 */
inline qlpRowExpr * qlpMakeRowExpr( stlParseParam       * aParam,
                                    stlInt32              aNodePos,
                                    qlpList             * aList  )
{
    
    qlpRowExpr   * sRow = NULL;

    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF(qlpRowExpr),
                            (void**) & sRow,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sRow = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sRow->mType         = QLL_ROW_EXPR_TYPE;
        sRow->mNodePos      = aNodePos;
        sRow->mArgCount     = aList->mCount;       
        sRow->mArgs         = aList;
    }

    return sRow;
}


/**
 * @brief Quntifier의 qlpValue 노드 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aFuncId          Function Id
 * @param[in]   aQuantifier      qlpQuantifier Id
 * 
 * @return qlpValue node
 */
inline qlpValue * qlpMakeQuantifier( stlParseParam       * aParam,
                                     stlInt32              aNodePos,
                                     knlBuiltInFunc        aFuncId,
                                     qlpQuantifier         aQuantifier  )
{
    qlpValue   * sValue    = NULL;
   
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpValue ),
                            (void**) & sValue,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sValue = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sValue->mType           = QLL_BNF_INTPARAM_CONSTANT_TYPE;
        sValue->mNodePos        = aNodePos;

        if( aQuantifier == QLP_QUANTIFIER_ALL )
        {
            switch( aFuncId )
            {
                case KNL_BUILTIN_FUNC_IS_EQUAL :
                    sValue->mValue.mInteger = KNL_BUILTIN_FUNC_EQUAL_ALL;
                    break;
                case KNL_BUILTIN_FUNC_IS_NOT_EQUAL :
                    sValue->mValue.mInteger = KNL_BUILTIN_FUNC_NOT_EQUAL_ALL;
                    break;
                case KNL_BUILTIN_FUNC_IS_LESS_THAN :
                    sValue->mValue.mInteger = KNL_BUILTIN_FUNC_LESS_THAN_ALL;
                    break;
                case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :
                    sValue->mValue.mInteger = KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ALL;
                    break;
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN :
                    sValue->mValue.mInteger = KNL_BUILTIN_FUNC_GREATER_THAN_ALL;
                    break;
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :
                    sValue->mValue.mInteger = KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ALL;
                    break;
                default :
                     STL_ASSERT(0);
                     break;
            }           
        }
        else if( aQuantifier == QLP_QUANTIFIER_ANY )
        {
            switch( aFuncId )
            {
                case KNL_BUILTIN_FUNC_IS_EQUAL :
                    sValue->mValue.mInteger = KNL_BUILTIN_FUNC_EQUAL_ANY;
                    break;
                case KNL_BUILTIN_FUNC_IS_NOT_EQUAL :
                    sValue->mValue.mInteger = KNL_BUILTIN_FUNC_NOT_EQUAL_ANY;
                    break;
                case KNL_BUILTIN_FUNC_IS_LESS_THAN :
                    sValue->mValue.mInteger = KNL_BUILTIN_FUNC_LESS_THAN_ANY;
                    break;
                case KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL :
                    sValue->mValue.mInteger = KNL_BUILTIN_FUNC_LESS_THAN_EQUAL_ANY;
                    break;
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN :
                    sValue->mValue.mInteger = KNL_BUILTIN_FUNC_GREATER_THAN_ANY;
                    break;
                case KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL :
                    sValue->mValue.mInteger = KNL_BUILTIN_FUNC_GREATER_THAN_EQUAL_ANY;
                    break;
                default :
                     STL_ASSERT(0);
                     break;
            }  
        }
        else
        {
            STL_ASSERT(0);
        }
    }
   
    return sValue;
}





/********************************************************************************
 * PHRASE - for syntax replacement
 ********************************************************************************/


/**
 * @brief PHRASE viewed table 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aViewName        View Name
 * @param[in]   aColumnList      aColumnList
 * @return qlpPhraseViewedTable phrase node
 */
inline qlpPhraseViewedTable * qlpMakePharseViewedTable( stlParseParam      * aParam,
                                                        stlInt32             aNodePos,
                                                        qlpList            * aViewName,
                                                        qlpList            * aColumnList )
{
    qlpPhraseViewedTable  * sView = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpPhraseViewedTable ),
                            (void**) & sView,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sView = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sView->mType        = QLL_PHRASE_VIEWED_TABLE_TYPE;
        sView->mNodePos     = aNodePos;
        sView->mViewName    = qlpMakeObjectName( aParam, aViewName );
        sView->mColumnList  = aColumnList;
    }
    
    return sView;
}


/**
 * @brief PHRASE default expression 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aNodePos         Node Position
 * @param[in]   aDefaultExpr     Default Expression
 * @return qlpPhraseDefaultExpr phrase node
 */
inline qlpPhraseDefaultExpr * qlpMakePhraseDefaultExpr( stlParseParam      * aParam,
                                                        stlInt32             aNodePos,
                                                        qlpValueExpr       * aDefaultExpr )
{
    qlpPhraseDefaultExpr  * sDefault = NULL;
    
    if( aParam->mAllocator( aParam->mContext,
                            STL_SIZEOF( qlpPhraseDefaultExpr ),
                            (void**) & sDefault,
                            aParam->mErrorStack )
        != STL_SUCCESS )
    {
        sDefault = NULL;
        aParam->mErrStatus = STL_FAILURE;
    }
    else
    {
        sDefault->mType        = QLL_PHRASE_DEFAULT_EXPR_TYPE;
        sDefault->mNodePos     = aNodePos;
        sDefault->mDefaultExpr = aDefaultExpr;
    }
    
    return sDefault;
}


/**
 * @brief Hint Syntax Validation
 * @param[in]   aParam           Parse Parameter
 * @return void
 */
inline void qlpCheckSyntaxForHint( stlParseParam  * aParam )
{
    stlBool  sEnableHintErr = STL_FALSE;

    /**
     * Hint Error Property에 따라 수행
     */

    STL_TRY( knlGetPropertyValueByID( KNL_PROPERTY_HINT_ERROR,
                                      &sEnableHintErr,
                                      KNL_ENV( QLP_PARSE_GET_ENV( aParam ) ) ) == STL_SUCCESS );

    if( sEnableHintErr == STL_TRUE )
    {
        /* Hint Error 발생시킴 */
        /* Do Nothing */
    }
    else
    {
        /* Hint Error 발생시키지 않음 */
        aParam->mErrStatus = STL_SUCCESS;
    }
    
    return;
    
    STL_FINISH;

    return;
}


/**
 * @brief node가 EXISTS function 인지 여부
 * @param[in]   aNode       Node
 * 
 * @return boolean
 */
inline stlBool qlpIsExistsFunc( void  * aNode )
{
    STL_DASSERT( aNode != NULL );
    
    if( *((qllNodeType *) aNode) == QLL_LIST_FUNCTION_TYPE )
    {
        return ( ((qlpListFunc *) aNode)->mFuncId == KNL_BUILTIN_FUNC_EXISTS );
    }
    else
    {
        return STL_FALSE;
    }
}


/** @} qlpParser */

