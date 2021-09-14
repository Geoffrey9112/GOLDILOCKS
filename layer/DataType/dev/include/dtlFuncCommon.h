/*******************************************************************************
 * dtlFuncCommon.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlFuncCommon.h 1841 2011-09-07 07:14:06Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/
#ifndef _DTL_FUNC_COMMON_H_
#define _DTL_FUNC_COMMON_H_ 1


/**
 * @file dtlFuncCommon.h
 * @brief DataType Layer Function 함수 
 */


#define DTL_MAX_FUNC_ARG_CNT        5

/*
 * 대표 func의 정보를 관리하는 구조체.
 */
typedef struct dtlFuncInfo
{
    stlBool    mValidArgList[DTL_MAX_FUNC_ARG_CNT];       /* 함수를 결정할 유효 argument index list */
} dtlFuncInfo;


/*
 * 각 arg type의 func별로 arg및 result type을 관리하는 구조체.
 */
typedef struct dtlFuncArgInfo
{
    dtlBuiltInFuncPtr   mFuncPtr;
    dtlDataType         mArgType[DTL_MAX_FUNC_ARG_CNT];
    dtlDataType         mResultType;
} dtlFuncArgInfo;

/*
 * dtlGroup간의 cast가 가능 여부를 관리하는 table
 * mSourceList 는 마지막을 알기위해 마지막에 DTL_GROUP_MAX를
 *  넣기 때문에 [DTL_GROUP_MAX + 1] 이다.
 */
typedef struct dtlGroupCast
{
    dtlGroup mTarget;
    dtlGroup mSourceList[DTL_GROUP_MAX + 1];
} dtlGroupCast;

extern const dtlGroupCast dtlGroupArgCast[];

stlStatus dtlGetFuncInfoCommon( const dtlFuncInfo     * aFuncInfo,
                                const dtlFuncArgInfo  * aFuncList,
                                stlUInt16               aDataTypeArrayCount,
                                stlBool               * aIsConstantData,
                                dtlDataType           * aDataTypeArray,
                                dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                stlUInt16               aFuncArgDataTypeArrayCount,
                                dtlDataType           * aFuncArgDataTypeArray,
                                dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                                dtlDataType           * aFuncResultDataType,
                                dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                                stlUInt32             * aFuncPtrIdx,
                                stlErrorStack         * aErrorStack );

/*******************************************************************************
 * ETC
 ******************************************************************************/

stlStatus dtlGetDataTypesConversionInfo( dtlDataType   * aDataTypeArray,
                                         stlUInt16       aFuncArgDataTypeArrayCount,
                                         dtlGroup      * aReturnFuncGroupType,
                                         stlErrorStack * aErrorStack );

stlStatus dtlSetFuncArgDataTypeInfo( dtlDataType   * aDataTypeArray,
                                     stlUInt16       aFuncArgDataTypeArrayCount,
                                     dtlGroup        aReturnGroupType,
                                     dtlDataType   * aFuncArgDataTypeArray,
                                     stlErrorStack * aErrorStack );

stlStatus dtlSetIntervalTypeIndicator( dtlDataTypeDefInfo  * aDataTypeDefInfoArray,
                                       stlUInt16             aFuncArgDataTypeArrayCount,
                                       dtlDataTypeDefInfo  * aFuncArgDataTypeDefInfoArray,
                                       dtlDataType         * aFuncArgDataTypeArray,
                                       stlErrorStack       * aErrorStack );

stlStatus dtlGetIntervalTypeIndicator( dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                                       stlUInt16               aDataTypeArrayCount,
                                       dtlIntervalIndicator  * aIntervalIndicator,
                                       stlErrorStack         * aErrorStack );


#endif /* _DTL_FUNC_COMMON_H_ */
