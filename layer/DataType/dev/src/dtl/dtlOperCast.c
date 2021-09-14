/*******************************************************************************
 * dtlOperCast.c
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
 * @file dtlOperCast.c
 * @brief Data Type Layer Casting 함수 
 */

#include <dtl.h>
#include <dtDef.h>

#include <dtcCast.h>
#include <dtcFromBoolean.h>
#include <dtcFromSmallInt.h>
#include <dtcFromInteger.h>
#include <dtcFromBigInt.h>
#include <dtcFromReal.h>
#include <dtcFromDouble.h>
#include <dtcFromNumeric.h>
#include <dtcFromChar.h>
#include <dtcFromVarchar.h>
#include <dtcFromLongvarchar.h>
#include <dtcFromBinary.h>
#include <dtcFromVarbinary.h>
#include <dtcFromLongvarbinary.h>
#include <dtcFromDate.h>
#include <dtcFromTime.h>
#include <dtcFromTimestamp.h>
#include <dtcFromTimeTz.h>
#include <dtcFromTimestampTz.h>
#include <dtcFromIntervalYearToMonth.h>
#include <dtcFromIntervalDayToSecond.h>
#include <dtcFromRowId.h>


/**
 * @addtogroup dtlOperCast
 * @{
 */

/**
 * @todo
 *  qlvCodeTypeCast , qlvCodeTypeDef 정보와 dest type 정보를 보고
 *  argument를 설정해줄 수 있는 interface가 필요함.
 *
 * @todo dtlOperCast.h의 cast함수의 input argument 인자를 하나 더 추가해야 함.
 *       qlvCodeTypeDef.mStringLengthUnit ( char precision이 byte or character 단위인지 )
 * 
 * @remark
 * <BR><BR>
 * <BR> cast(data type conversion) 연산에 대한 기본 구조. 
 * <BR>
 * <BR>
 * <BR>
 * <BR> 1. type cast
 * <BR>        
 * <table>
 *   <tr>
 *     <td> from / to     </td>
 *     <td> BOOLEAN       </td>
 *     <td> SMALLINT      </td>
 *     <td> INTEGER       </td>
 *     <td> BIGINT        </td>
 *     <td> REAL          </td>
 *     <td> DOUBLE        </td>
 *     <td> FLOAT         </td>
 *     <td> NUMBER        </td>
 *     <td> DECIMAL       </td>
 *     <td> CHAR          </td>
 *     <td> VARCHAR       </td>
 *     <td> LONGVARCHAR   </td>
 *     <td> CLOB          </td>
 *     <td> BINARY        </td>
 *     <td> VARBINARY     </td>
 *     <td> LONGVARBINARY </td>
 *     <td> BLOB          </td>
 *     <td> DATE          </td>
 *     <td> TIME          </td>
 *     <td> TIMESTAMP     </td>
 *     <td> TIMETZ        </td>
 *     <td> TIMESTAMPTZ   </td>
 *     <td> DTL_TYPE_INTERVAL_YEAR_TO_MONTH </td>
 *     <td> DTL_TYPE_INTERVAL_DAY_TO_SECOND </td>
 *     <td> ROWID         </td>
 *   </tr>
 *   <tr>
 *     <td> BOOLEAN       </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> SMALLINT      </td>
 *     <td>  N/A          </td> 
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>     
 *   </tr>
 *   <tr>
 *     <td> INTEGER       </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>     
 *   </tr>
 *   <tr>    
 *     <td> BIGINT        </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> REAL          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> DOUBLE        </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> FLOAT         </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> NUMBER       </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> DECIMAL       </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> CHAR          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *   </tr>
 *   <tr>
 *     <td> VARCHAR       </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *   </tr>
 *   <tr>
 *     <td> LONGVARCHAR   </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *   </tr>
 *   <tr>
 *     <td> CLOB          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> BINARY        </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> VARBINARY     </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> LONGVARBINARY </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> BLOB          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>     
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> DATE          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> TIME          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> TIMESTAMP     </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> TIMETZ        </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> TIMESTAMPTZ   </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> DTL_TYPE_INTERVAL_YEAR_TO_MONTH     </td>  
 *     <td>  N/A          </td>  
 *     <td>  O            </td>  
 *     <td>  O            </td>  
 *     <td>  O            </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>  
 *     <td>  O            </td>  
 *     <td>  N/A          </td>  
 *     <td>  O            </td>  
 *     <td>  O            </td>  
 *     <td>  O            </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> DTL_TYPE_INTERVAL_DAY_TO_SECOND     </td>  
 *     <td>  N/A          </td>  
 *     <td>  O            </td>  
 *     <td>  O            </td>  
 *     <td>  O            </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>  
 *     <td>  O            </td>  
 *     <td>  N/A          </td>  
 *     <td>  O            </td>  
 *     <td>  O            </td>  
 *     <td>  O            </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *     <td>  N/A          </td>
 *   </tr>
 *   <tr>
 *     <td> ROWID         </td>  
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  O            </td>  
 *     <td>  O            </td>  
 *     <td>  O            </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>  
 *     <td>  N/A          </td>
 *     <td>  N/A          </td>
 *     <td>  O            </td>
 *   </tr>
 * </table>
 * 
 * @{
 */

dtlBuiltInFuncPtr dtlCastFunctionList[DTL_TYPE_MAX][DTL_TYPE_MAX] = 
{
    /* DTL_TYPE_BOOLEAN */
    {
        dtcCastBooleanToBoolean,             /* DTL_TYPE_BOOLEAN  */
        NULL,                                /* DTL_TYPE_SMALLINT  */
        NULL,                                /* DTL_TYPE_INTEGER  */
        NULL,                                /* DTL_TYPE_BIGINT  */
        NULL,                                /* DTL_TYPE_REAL  */
        NULL,                                /* DTL_TYPE_DOUBLE  */
        NULL,                                /* DTL_TYPE_FLOAT */
        NULL,                                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        dtcCastBooleanToChar,                /* DTL_TYPE_CHAR */
        dtcCastBooleanToVarchar,             /* DTL_TYPE_VARCHAR  */
        dtcCastBooleanToLongvarchar,         /* DTL_TYPE_LONGVARCHAR  */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE  */
        NULL,                                /* DTL_TYPE_TIME  */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE  */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE  */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_SMALLINT */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */
        dtcCastSmallIntToSmallInt,           /* DTL_TYPE_SMALLINT */
        dtcCastSmallIntToInteger,            /* DTL_TYPE_INTEGER */
        dtcCastSmallIntToBigInt,             /* DTL_TYPE_BIGINT */
        dtcCastSmallIntToReal,               /* DTL_TYPE_REAL */
        dtcCastSmallIntToDouble,             /* DTL_TYPE_DOUBLE */
        dtcCastSmallIntToNumeric,            /* DTL_TYPE_FLOAT */
        dtcCastSmallIntToNumeric,            /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        dtcCastSmallIntToChar,               /* DTL_TYPE_CHAR */
        dtcCastSmallIntToVarchar,            /* DTL_TYPE_VARCHAR */
        dtcCastSmallIntToLongvarchar,        /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        dtcCastSmallIntToIntervalYearToMonth, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        dtcCastSmallIntToIntervalDayToSecond, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_INTEGER */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */
        dtcCastIntegerToSmallInt,            /* DTL_TYPE_SMALLINT */
        dtcCastIntegerToInteger,             /* DTL_TYPE_INTEGER */
        dtcCastIntegerToBigInt,              /* DTL_TYPE_BIGINT */
        dtcCastIntegerToReal,                /* DTL_TYPE_REAL */
        dtcCastIntegerToDouble,              /* DTL_TYPE_DOUBLE */
        dtcCastIntegerToNumeric,             /* DTL_TYPE_FLOAT */
        dtcCastIntegerToNumeric,             /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        dtcCastIntegerToChar,                /* DTL_TYPE_CHAR */
        dtcCastIntegerToVarchar,             /* DTL_TYPE_VARCHAR */
        dtcCastIntegerToLongvarchar,         /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        dtcCastIntegerToIntervalYearToMonth, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        dtcCastIntegerToIntervalDayToSecond, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },
    
    /* DTL_TYPE_BIGINT */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */
        dtcCastBigIntToSmallInt,             /* DTL_TYPE_SMALLINT */
        dtcCastBigIntToInteger,              /* DTL_TYPE_INTEGER */
        dtcCastBigIntToBigInt,               /* DTL_TYPE_BIGINT */
        dtcCastBigIntToReal,                 /* DTL_TYPE_REAL */
        dtcCastBigIntToDouble,               /* DTL_TYPE_DOUBLE */
        dtcCastBigIntToNumeric,              /* DTL_TYPE_FLOAT */
        dtcCastBigIntToNumeric,              /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        dtcCastBigIntToChar,                 /* DTL_TYPE_CHAR */
        dtcCastBigIntToVarchar,              /* DTL_TYPE_VARCHAR */
        dtcCastBigIntToLongvarchar,          /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        dtcCastBigIntToIntervalYearToMonth,  /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        dtcCastBigIntToIntervalDayToSecond,  /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_REAL */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */
        dtcCastRealToSmallInt,               /* DTL_TYPE_SMALLINT */
        dtcCastRealToInteger,                /* DTL_TYPE_INTEGER */
        dtcCastRealToBigInt,                 /* DTL_TYPE_BIGINT */
        dtcCastRealToReal,                   /* DTL_TYPE_REAL */
        dtcCastRealToDouble,                 /* DTL_TYPE_DOUBLE */
        dtcCastRealToNumeric,                /* DTL_TYPE_FLOAT */        
        dtcCastRealToNumeric,                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        dtcCastRealToChar,                   /* DTL_TYPE_CHAR */
        dtcCastRealToVarchar,                /* DTL_TYPE_VARCHAR */
        dtcCastRealToLongvarchar,            /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_DOUBLE */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */
        dtcCastDoubleToSmallInt,             /* DTL_TYPE_SMALLINT */
        dtcCastDoubleToInteger,              /* DTL_TYPE_INTEGER */
        dtcCastDoubleToBigInt,               /* DTL_TYPE_BIGINT */
        dtcCastDoubleToReal,                 /* DTL_TYPE_REAL */
        dtcCastDoubleToDouble,               /* DTL_TYPE_DOUBLE */
        dtcCastDoubleToNumeric,              /* DTL_TYPE_FLOAT */        
        dtcCastDoubleToNumeric,              /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        dtcCastDoubleToChar,                 /* DTL_TYPE_CHAR */
        dtcCastDoubleToVarchar,              /* DTL_TYPE_VARCHAR */
        dtcCastDoubleToLongvarchar,          /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_FLOAT */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */ 
        dtcCastNumericToSmallInt,            /* DTL_TYPE_SMALLINT */
        dtcCastNumericToInteger,             /* DTL_TYPE_INTEGER */
        dtcCastNumericToBigInt,              /* DTL_TYPE_BIGINT */
        dtcCastNumericToReal,                /* DTL_TYPE_REAL */
        dtcCastNumericToDouble,              /* DTL_TYPE_DOUBLE */
        dtcCastNumericToNumeric,             /* DTL_TYPE_FLOAT */
        dtcCastNumericToNumeric,             /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        dtcCastNumericToChar,                /* DTL_TYPE_CHAR */
        dtcCastNumericToVarchar,             /* DTL_TYPE_VARCHAR */
        dtcCastNumericToLongvarchar,         /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        dtcCastNumericToIntervalYearToMonth, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        dtcCastNumericToIntervalDayToSecond, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },
    
    /* DTL_TYPE_NUMBER */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */ 
        dtcCastNumericToSmallInt,            /* DTL_TYPE_SMALLINT */
        dtcCastNumericToInteger,             /* DTL_TYPE_INTEGER */
        dtcCastNumericToBigInt,              /* DTL_TYPE_BIGINT */
        dtcCastNumericToReal,                /* DTL_TYPE_REAL */
        dtcCastNumericToDouble,              /* DTL_TYPE_DOUBLE */
        dtcCastNumericToNumeric,             /* DTL_TYPE_FLOATE */
        dtcCastNumericToNumeric,             /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        dtcCastNumericToChar,                /* DTL_TYPE_CHAR */
        dtcCastNumericToVarchar,             /* DTL_TYPE_VARCHAR */
        dtcCastNumericToLongvarchar,         /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        dtcCastNumericToIntervalYearToMonth, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        dtcCastNumericToIntervalDayToSecond, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },
      
    /* DTL_TYPE_DECIMAL */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */
        NULL,                                /* DTL_TYPE_SMALLINT */
        NULL,                                /* DTL_TYPE_INTEGER */
        NULL,                                /* DTL_TYPE_BIGINT */
        NULL,                                /* DTL_TYPE_REAL */
        NULL,                                /* DTL_TYPE_DOUBLE */
        NULL,                                /* DTL_TYPE_FLOAT */
        NULL,                                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        NULL,                                /* DTL_TYPE_CHAR */
        NULL,                                /* DTL_TYPE_VARCHAR */
        NULL,                                /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY */
        NULL,                                /* DTL_TYPE_VARBINARY */
        NULL,                                /* DTL_TYPE_LONGVARBINARY */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_CHAR */
    {
        dtcCastCharToBoolean,                /* DTL_TYPE_BOOLEAN */
        dtcCastCharToSmallInt,               /* DTL_TYPE_SMALLINT */
        dtcCastCharToInteger,                /* DTL_TYPE_INTEGER */
        dtcCastCharToBigInt,                 /* DTL_TYPE_BIGINT */
        dtcCastCharToReal,                   /* DTL_TYPE_REAL */
        dtcCastCharToDouble,                 /* DTL_TYPE_DOUBLE */
        dtcCastCharToNumeric,                /* DTL_TYPE_FLOAT */
        dtcCastCharToNumeric,                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        dtcCastCharToChar,                   /* DTL_TYPE_CHAR */
        dtcCastCharToVarchar,                /* DTL_TYPE_VARCHAR */
        dtcCastCharToLongvarchar,            /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        dtcCastCharToDate,                   /* DTL_TYPE_DATE */
        dtcCastCharToTime,                   /* DTL_TYPE_TIME */
        dtcCastCharToTimestamp,              /* DTL_TYPE_TIMESTAMP */
        dtcCastCharToTimeTz,                 /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        dtcCastCharToTimestampTz,            /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        dtcCastCharToIntervalYearToMonth,    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        dtcCastCharToIntervalDayToSecond,    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        dtcCastCharToRowId                   /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_VARCHAR */
    {
        dtcCastVarcharToBoolean,             /* DTL_TYPE_BOOLEAN */
        dtcCastVarcharToSmallInt,            /* DTL_TYPE_SMALLINT */
        dtcCastVarcharToInteger,             /* DTL_TYPE_INTEGER */
        dtcCastVarcharToBigInt,              /* DTL_TYPE_BIGINT */
        dtcCastVarcharToReal,                /* DTL_TYPE_REAL */
        dtcCastVarcharToDouble,              /* DTL_TYPE_DOUBLE */
        dtcCastVarcharToNumeric,             /* DTL_TYPE_FLOAT */
        dtcCastVarcharToNumeric,             /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        dtcCastVarcharToChar,                /* DTL_TYPE_CHAR */
        dtcCastVarcharToVarchar,             /* DTL_TYPE_VARCHAR */
        dtcCastVarcharToLongvarchar,         /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        dtcCastVarcharToBinary,              /* DTL_TYPE_BINARY  */
        dtcCastVarcharToVarbinary,           /* DTL_TYPE_VARBINARY  */
        dtcCastVarcharToLongvarbinary,       /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        dtcCastVarcharToDate,                /* DTL_TYPE_DATE */
        dtcCastVarcharToTime,                /* DTL_TYPE_TIME */
        dtcCastVarcharToTimestamp,           /* DTL_TYPE_TIMESTAMP */
        dtcCastVarcharToTimeTz,              /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        dtcCastVarcharToTimestampTz,         /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        dtcCastVarcharToIntervalYearToMonth, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        dtcCastVarcharToIntervalDayToSecond, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        dtcCastVarcharToRowId                /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_LONGVARCHAR */
    {
        dtcCastLongvarcharToBoolean,         /* DTL_TYPE_BOOLEAN */
        dtcCastLongvarcharToSmallInt,        /* DTL_TYPE_SMALLINT */
        dtcCastLongvarcharToInteger,         /* DTL_TYPE_INTEGER */
        dtcCastLongvarcharToBigInt,          /* DTL_TYPE_BIGINT */
        dtcCastLongvarcharToReal,            /* DTL_TYPE_REAL */
        dtcCastLongvarcharToDouble,          /* DTL_TYPE_DOUBLE */
        dtcCastLongvarcharToNumeric,         /* DTL_TYPE_FLOAT */
        dtcCastLongvarcharToNumeric,         /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        dtcCastLongvarcharToChar,            /* DTL_TYPE_CHAR */
        dtcCastLongvarcharToVarchar,         /* DTL_TYPE_VARCHAR */
        dtcCastLongvarcharToLongvarchar,     /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        dtcCastLongvarcharToDate,            /* DTL_TYPE_DATE */
        dtcCastLongvarcharToTime,            /* DTL_TYPE_TIME */
        dtcCastLongvarcharToTimestamp,       /* DTL_TYPE_TIMESTAMP */
        dtcCastLongvarcharToTimeTz,          /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        dtcCastLongvarcharToTimestampTz,     /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        dtcCastLongvarcharToIntervalYearToMonth, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        dtcCastLongvarcharToIntervalDayToSecond, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        dtcCastLongvarcharToRowId                /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_CLOB */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */
        NULL,                                /* DTL_TYPE_SMALLINT */
        NULL,                                /* DTL_TYPE_INTEGER */
        NULL,                                /* DTL_TYPE_BIGINT */
        NULL,                                /* DTL_TYPE_REAL */
        NULL,                                /* DTL_TYPE_DOUBLE */
        NULL,                                /* DTL_TYPE_FLOAT */
        NULL,                                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        NULL,                                /* DTL_TYPE_CHAR */
        NULL,                                /* DTL_TYPE_VARCHAR */
        NULL,                                /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY */
        NULL,                                /* DTL_TYPE_VARBINARY */
        NULL,                                /* DTL_TYPE_LONGVARBINARY */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_BINARY */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */
        NULL,                                /* DTL_TYPE_SMALLINT */
        NULL,                                /* DTL_TYPE_INTEGER */
        NULL,                                /* DTL_TYPE_BIGINT */
        NULL,                                /* DTL_TYPE_REAL */
        NULL,                                /* DTL_TYPE_DOUBLE */
        NULL,                                /* DTL_TYPE_FLOAT */
        NULL,                                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        NULL,                                /* DTL_TYPE_CHAR */
        NULL,                                /* DTL_TYPE_VARCHAR */
        NULL,                                /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        dtcCastBinaryToBinary,               /* DTL_TYPE_BINARY */
        dtcCastBinaryToVarbinary,            /* DTL_TYPE_VARBINARY */
        dtcCastBinaryToLongvarbinary,        /* DTL_TYPE_LONGVARBINARY */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_VARBINARY */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */
        NULL,                                /* DTL_TYPE_SMALLINT */
        NULL,                                /* DTL_TYPE_INTEGER */
        NULL,                                /* DTL_TYPE_BIGINT */
        NULL,                                /* DTL_TYPE_REAL */
        NULL,                                /* DTL_TYPE_DOUBLE */
        NULL,                                /* DTL_TYPE_FLOAT */
        NULL,                                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        NULL,                                /* DTL_TYPE_CHAR */
        NULL,                                /* DTL_TYPE_VARCHAR */
        NULL,                                /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        dtcCastVarbinaryToBinary,            /* DTL_TYPE_BINARY */
        dtcCastVarbinaryToVarbinary,         /* DTL_TYPE_VARBINARY */
        dtcCastVarbinaryToLongvarbinary,     /* DTL_TYPE_LONGVARBINARY */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_LONGVARBINARY */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */
        NULL,                                /* DTL_TYPE_SMALLINT */
        NULL,                                /* DTL_TYPE_INTEGER */
        NULL,                                /* DTL_TYPE_BIGINT */
        NULL,                                /* DTL_TYPE_REAL */
        NULL,                                /* DTL_TYPE_DOUBLE */
        NULL,                                /* DTL_TYPE_FLOAT */
        NULL,                                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        NULL,                                /* DTL_TYPE_CHAR */
        NULL,                                /* DTL_TYPE_VARCHAR */
        NULL,                                /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        dtcCastLongvarbinaryToBinary,        /* DTL_TYPE_BINARY */
        dtcCastLongvarbinaryToVarbinary,     /* DTL_TYPE_VARBINARY */
        dtcCastLongvarbinaryToLongvarbinary, /* DTL_TYPE_LONGVARBINARY */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_BLOB */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */
        NULL,                                /* DTL_TYPE_SMALLINT */
        NULL,                                /* DTL_TYPE_INTEGER */
        NULL,                                /* DTL_TYPE_BIGINT */
        NULL,                                /* DTL_TYPE_REAL */
        NULL,                                /* DTL_TYPE_DOUBLE */
        NULL,                                /* DTL_TYPE_FLOAT */
        NULL,                                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */ 
        NULL,                                /* DTL_TYPE_CHAR */
        NULL,                                /* DTL_TYPE_VARCHAR */
        NULL,                                /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY */
        NULL,                                /* DTL_TYPE_VARBINARY */
        NULL,                                /* DTL_TYPE_LONGVARBINARY */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_DATE */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */  
        NULL,                                /* DTL_TYPE_SMALLINT */
        NULL,                                /* DTL_TYPE_INTEGER */
        NULL,                                /* DTL_TYPE_BIGINT */
        NULL,                                /* DTL_TYPE_REAL */
        NULL,                                /* DTL_TYPE_DOUBLE */
        NULL,                                /* DTL_TYPE_FLOAT */
        NULL,                                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */
        dtcCastDateToChar,                   /* DTL_TYPE_CHAR */
        dtcCastDateToVarchar,                /* DTL_TYPE_VARCHAR */
        dtcCastDateToLongvarchar,            /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        dtcCastDateToDate,                   /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        dtcCastDateToTimestamp,              /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        dtcCastDateToTimestampTz,            /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_TIME */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */  
        NULL,                                /* DTL_TYPE_SMALLINT */
        NULL,                                /* DTL_TYPE_INTEGER */
        NULL,                                /* DTL_TYPE_BIGINT */
        NULL,                                /* DTL_TYPE_REAL */
        NULL,                                /* DTL_TYPE_DOUBLE */
        NULL,                                /* DTL_TYPE_FLOAT */
        NULL,                                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */
        dtcCastTimeToChar,                   /* DTL_TYPE_CHAR */
        dtcCastTimeToVarchar,                /* DTL_TYPE_VARCHAR */
        dtcCastTimeToLongvarchar,            /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        dtcCastTimeToTime,                   /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        dtcCastTimeToTimeTz,                 /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */  
    },

    /* DTL_TYPE_TIMESTAMP */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */  
        NULL,                                /* DTL_TYPE_SMALLINT */
        NULL,                                /* DTL_TYPE_INTEGER */
        NULL,                                /* DTL_TYPE_BIGINT */
        NULL,                                /* DTL_TYPE_REAL */
        NULL,                                /* DTL_TYPE_DOUBLE */
        NULL,                                /* DTL_TYPE_FLOAT */
        NULL,                                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */
        dtcCastTimestampToChar,              /* DTL_TYPE_CHAR */
        dtcCastTimestampToVarchar,           /* DTL_TYPE_VARCHAR */
        dtcCastTimestampToLongvarchar,       /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        dtcCastTimestampToDate,              /* DTL_TYPE_DATE */
        dtcCastTimestampToTime,              /* DTL_TYPE_TIME */
        dtcCastTimestampToTimestamp,         /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        dtcCastTimestampToTimestampTz,       /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */  
        NULL,                                /* DTL_TYPE_SMALLINT */
        NULL,                                /* DTL_TYPE_INTEGER */
        NULL,                                /* DTL_TYPE_BIGINT */
        NULL,                                /* DTL_TYPE_REAL */
        NULL,                                /* DTL_TYPE_DOUBLE */
        NULL,                                /* DTL_TYPE_FLOAT */
        NULL,                                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */
        dtcCastTimeTzToChar,                 /* DTL_TYPE_CHAR */
        dtcCastTimeTzToVarchar,              /* DTL_TYPE_VARCHAR */
        dtcCastTimeTzToLongvarchar,          /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        dtcCastTimeTzToTime,                 /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        dtcCastTimeTzToTimeTz,               /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE  */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */  
        NULL,                                /* DTL_TYPE_SMALLINT */
        NULL,                                /* DTL_TYPE_INTEGER */
        NULL,                                /* DTL_TYPE_BIGINT */
        NULL,                                /* DTL_TYPE_REAL */
        NULL,                                /* DTL_TYPE_DOUBLE */
        NULL,                                /* DTL_TYPE_FLOAT */
        NULL,                                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */
        dtcCastTimestampTzToChar,            /* DTL_TYPE_CHAR */
        dtcCastTimestampTzToVarchar,         /* DTL_TYPE_VARCHAR */
        dtcCastTimestampTzToLongvarchar,     /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        dtcCastTimestampTzToDate,            /* DTL_TYPE_DATE */
        dtcCastTimestampTzToTime,            /* DTL_TYPE_TIME */
        dtcCastTimestampTzToTimestamp,       /* DTL_TYPE_TIMESTAMP */
        dtcCastTimestampTzToTimeTz,          /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        dtcCastTimestampTzToTimestampTz,     /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
    {
        NULL,                                    /* DTL_TYPE_BOOLEAN */  
        dtcCastIntervalYearToMonthToSmallInt,    /* DTL_TYPE_SMALLINT */
        dtcCastIntervalYearToMonthToInteger,     /* DTL_TYPE_INTEGER */
        dtcCastIntervalYearToMonthToBigInt,      /* DTL_TYPE_BIGINT */
        NULL,                                    /* DTL_TYPE_REAL */
        NULL,                                    /* DTL_TYPE_DOUBLE */
        dtcCastIntervalYearToMonthToNumeric,     /* DTL_TYPE_FLOAT */        
        dtcCastIntervalYearToMonthToNumeric,     /* DTL_TYPE_NUMBER */
        NULL,                                    /* DTL_TYPE_DECIMAL */
        dtcCastIntervalYearToMonthToChar,        /* DTL_TYPE_CHAR */
        dtcCastIntervalYearToMonthToVarchar,     /* DTL_TYPE_VARCHAR */
        dtcCastIntervalYearToMonthToLongvarchar, /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        dtcCastIntervalYearToMonthToIntervalYearToMonth, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
    {
        NULL,                                    /* DTL_TYPE_BOOLEAN */  
        dtcCastIntervalDayToSecondToSmallInt,    /* DTL_TYPE_SMALLINT */
        dtcCastIntervalDayToSecondToInteger,     /* DTL_TYPE_INTEGER */
        dtcCastIntervalDayToSecondToBigInt,      /* DTL_TYPE_BIGINT */
        NULL,                                    /* DTL_TYPE_REAL */
        NULL,                                    /* DTL_TYPE_DOUBLE */
        dtcCastIntervalDayToSecondToNumeric,     /* DTL_TYPE_FLOAT */
        dtcCastIntervalDayToSecondToNumeric,     /* DTL_TYPE_NUMBER */
        NULL,                                    /* DTL_TYPE_DECIMAL */
        dtcCastIntervalDayToSecondToChar,        /* DTL_TYPE_CHAR */
        dtcCastIntervalDayToSecondToVarchar,     /* DTL_TYPE_VARCHAR */
        dtcCastIntervalDayToSecondToLongvarchar, /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        dtcCastIntervalDayToSecondToIntervalDayToSecond, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL                                 /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_ROWID */
    {
        NULL,                                /* DTL_TYPE_BOOLEAN */  
        NULL,                                /* DTL_TYPE_SMALLINT */
        NULL,                                /* DTL_TYPE_INTEGER */
        NULL,                                /* DTL_TYPE_BIGINT */
        NULL,                                /* DTL_TYPE_REAL */
        NULL,                                /* DTL_TYPE_DOUBLE */
        NULL,                                /* DTL_TYPE_FLOAT */
        NULL,                                /* DTL_TYPE_NUMBER */
        NULL,                                /* DTL_TYPE_DECIMAL */
        dtcCastRowIdToChar,                  /* DTL_TYPE_CHAR */
        dtcCastRowIdToVarchar,               /* DTL_TYPE_VARCHAR */
        dtcCastRowIdToLongvarchar,           /* DTL_TYPE_LONGVARCHAR */
        NULL,                                /* DTL_TYPE_CLOB */
        NULL,                                /* DTL_TYPE_BINARY  */
        NULL,                                /* DTL_TYPE_VARBINARY  */
        NULL,                                /* DTL_TYPE_LONGVARBINARY  */
        NULL,                                /* DTL_TYPE_BLOB */
        NULL,                                /* DTL_TYPE_DATE */
        NULL,                                /* DTL_TYPE_TIME */
        NULL,                                /* DTL_TYPE_TIMESTAMP */
        NULL,                                /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        NULL,                                /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        NULL,                                /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        dtcCastRowIdToRowId                  /* DTL_TYPE_ROWID */ 
    }
    
};


/**
 * Cast 시 Data Loss 가 발생할 수 있는지 여부
 */
stlBool dtlIsNoDataLossForCast[DTL_TYPE_MAX][DTL_TYPE_MAX] = 
{
    /* DTL_TYPE_BOOLEAN */
    {
        STL_TRUE,                     /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */        
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_SMALLINT */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_TRUE,                     /* DTL_TYPE_SMALLINT */
        STL_TRUE,                     /* DTL_TYPE_INTEGER */
        STL_TRUE,                     /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_TRUE,                     /* DTL_TYPE_FLOAT */        
        STL_TRUE,                     /* DTL_TYPE_NUMBER */
        STL_TRUE,                     /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_TRUE,                     /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_TRUE,                     /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_INTEGER */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_TRUE,                     /* DTL_TYPE_INTEGER */
        STL_TRUE,                     /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_TRUE,                     /* DTL_TYPE_FLOAT */
        STL_TRUE,                     /* DTL_TYPE_NUMBER */
        STL_TRUE,                     /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },
    
    /* DTL_TYPE_BIGINT */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_TRUE,                     /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_TRUE,                     /* DTL_TYPE_FLOAT */
        STL_TRUE,                     /* DTL_TYPE_NUMBER */
        STL_TRUE,                     /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_REAL */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_TRUE,                     /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_TRUE,                     /* DTL_TYPE_FLOAT */
        STL_TRUE,                     /* DTL_TYPE_NUMBER */
        STL_TRUE,                     /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_DOUBLE */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_TRUE,                     /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_FLOAT */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_TRUE,                     /* DTL_TYPE_FLOAT */
        STL_TRUE,                     /* DTL_TYPE_NUMBER */
        STL_TRUE,                     /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },
    
    /* DTL_TYPE_NUMBER */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_TRUE,                     /* DTL_TYPE_FLOAT */
        STL_TRUE,                     /* DTL_TYPE_NUMBER */
        STL_TRUE,                     /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },
      
    /* DTL_TYPE_DECIMAL */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_TRUE,                     /* DTL_TYPE_FLOAT */
        STL_TRUE,                     /* DTL_TYPE_NUMBER */
        STL_TRUE,                     /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_CHAR */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_TRUE,                     /* DTL_TYPE_DOUBLE */
        STL_TRUE,                     /* DTL_TYPE_FLOAT */
        STL_TRUE,                     /* DTL_TYPE_NUMBER */
        STL_TRUE,                     /* DTL_TYPE_DECIMAL */ 
        STL_TRUE,                     /* DTL_TYPE_CHAR */
        STL_TRUE,                     /* DTL_TYPE_VARCHAR */
        STL_TRUE,                     /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_VARCHAR */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_TRUE,                     /* DTL_TYPE_DOUBLE */
        STL_TRUE,                     /* DTL_TYPE_FLOAT */
        STL_TRUE,                     /* DTL_TYPE_NUMBER */
        STL_TRUE,                     /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_TRUE,                     /* DTL_TYPE_VARCHAR */
        STL_TRUE,                     /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_LONGVARCHAR */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_TRUE,                     /* DTL_TYPE_DOUBLE */
        STL_TRUE,                     /* DTL_TYPE_FLOAT */
        STL_TRUE,                     /* DTL_TYPE_NUMBER */
        STL_TRUE,                     /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_TRUE,                     /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_CLOB */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_BINARY */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_TRUE,                     /* DTL_TYPE_BINARY */
        STL_TRUE,                     /* DTL_TYPE_VARBINARY */
        STL_TRUE,                     /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_VARBINARY */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_TRUE,                     /* DTL_TYPE_VARBINARY */
        STL_TRUE,                     /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_LONGVARBINARY */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_TRUE,                     /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_BLOB */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_DATE */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_TRUE,                     /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_TRUE,                     /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_TRUE,                     /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_TIME */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_TRUE,                     /* DTL_TYPE_TIME */
        STL_TRUE,                     /* DTL_TYPE_TIMESTAMP */
        STL_TRUE,                     /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_TRUE,                     /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_TIMESTAMP */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_TRUE,                     /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_TRUE,                     /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_TRUE,                     /* DTL_TYPE_TIME */
        STL_TRUE,                     /* DTL_TYPE_TIMESTAMP */
        STL_TRUE,                     /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_TRUE,                     /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_TRUE,                     /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_TRUE,                     /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_TRUE,                     /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_TRUE,                     /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_FALSE                     /* DTL_TYPE_ROWID */ 
    },

    /* DTL_TYPE_ROWID */
    {
        STL_FALSE,                    /* DTL_TYPE_BOOLEAN */
        STL_FALSE,                    /* DTL_TYPE_SMALLINT */
        STL_FALSE,                    /* DTL_TYPE_INTEGER */
        STL_FALSE,                    /* DTL_TYPE_BIGINT */
        STL_FALSE,                    /* DTL_TYPE_REAL */
        STL_FALSE,                    /* DTL_TYPE_DOUBLE */
        STL_FALSE,                    /* DTL_TYPE_FLOAT */
        STL_FALSE,                    /* DTL_TYPE_NUMBER */
        STL_FALSE,                    /* DTL_TYPE_DECIMAL */ 
        STL_FALSE,                    /* DTL_TYPE_CHAR */
        STL_FALSE,                    /* DTL_TYPE_VARCHAR */
        STL_FALSE,                    /* DTL_TYPE_LONGVARCHAR */
        STL_FALSE,                    /* DTL_TYPE_CLOB */
        STL_FALSE,                    /* DTL_TYPE_BINARY */
        STL_FALSE,                    /* DTL_TYPE_VARBINARY */
        STL_FALSE,                    /* DTL_TYPE_LONGVARBINARY */
        STL_FALSE,                    /* DTL_TYPE_BLOB */
        STL_FALSE,                    /* DTL_TYPE_DATE */
        STL_FALSE,                    /* DTL_TYPE_TIME */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP */
        STL_FALSE,                    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        STL_FALSE,                    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        STL_TRUE                      /* DTL_TYPE_ROWID */ 
    }
};


/** @} dtcCast */

/** @} dtlOperCast */

