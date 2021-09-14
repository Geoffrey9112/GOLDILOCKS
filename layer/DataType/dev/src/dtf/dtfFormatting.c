/*******************************************************************************
 * dtfFormatting.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfFormatting.c 1395 2011-07-12 09:14:31Z ehpark $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file dtfFormatting.c
 * @brief Formatting Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfFormatting.h>

/**
 * @addtogroup dtfFormatting  Formatting
 * @ingroup dtf
 * @internal
 * @{
 */

/***************
 * DATETIME 의 format element의 index
 ***************/

const stlInt32 DTF_DATETIME_FORMAT_INDEX[256] =
{
    -1, -1, -1, -1, -1,  /*  0 ~ 4  */
    -1, -1, -1, -1, -1,  /*  5 ~ 9  */    
    -1, -1, -1, -1, -1,  /* 10 ~ 14 */
    -1, -1, -1, -1, -1,  /* 15 ~ 19 */    
    -1, -1, -1, -1, -1,  /* 20 ~ 24 */
    -1, -1, -1, -1, -1,  /* 25 ~ 29 */    
    -1, -1, -1, -1, -1,  /* 30 ~ 34 */
    -1, -1, -1, -1, -1,  /* 35 ~ 39 */
    -1, -1, -1, -1, -1,  /* 40 ~ 44 */
    -1, -1, -1, -1, -1,  /* 45 ~ 49 */    
    -1, -1, -1, -1, -1,  /* 50 ~ 54 */
    -1, -1, -1, -1, -1,  /* 55 ~ 59 */    
    -1, -1, -1, -1, -1,  /* 60 ~ 64 */
    /* A, B, C, D, E */ 
    DTF_DATETIME_FMT_A_D, DTF_DATETIME_FMT_B_C, DTF_DATETIME_FMT_CC, DTF_DATETIME_FMT_DAY, -1,  /* 65 ~ 69 */
    /* F, G, H, I, J */
    DTF_DATETIME_FMT_FF1, -1, DTF_DATETIME_FMT_HH24, -1, DTF_DATETIME_FMT_J,  /* 70 ~ 74 */    
    /* K, L, M, N, O */
    -1, -1, DTF_DATETIME_FMT_MI, -1, -1,      /* 75 ~ 79 */
    /* P, Q, R, S, T */
    DTF_DATETIME_FMT_P_M, DTF_DATETIME_FMT_Q, DTF_DATETIME_FMT_RM, DTF_DATETIME_FMT_SSSSS, DTF_DATETIME_FMT_TZH,      /* 80 ~ 84 */
    /* U, V, W, X, Y */
    -1, -1, DTF_DATETIME_FMT_WW, -1, DTF_DATETIME_FMT_Y_YYY,    /* 85 ~ 89 */
    /* Z, -1, -1, -1, -1, */
    -1, -1, -1, -1, -1,     /* 90 ~ 94 */
    /* -1, -1, a, b, c */
    -1, -1, DTF_DATETIME_FMT_a_d, DTF_DATETIME_FMT_b_c, DTF_DATETIME_FMT_cc,     /* 95 ~ 99 */
    /* d, e, f, g, h */
    DTF_DATETIME_FMT_day, -1, DTF_DATETIME_FMT_ff1, -1, DTF_DATETIME_FMT_hh24,     /* 100 ~ 104 */
    /* i, j, k, l, m */
    -1, DTF_DATETIME_FMT_j, -1, -1, DTF_DATETIME_FMT_mi,     /* 105 ~ 109 */
    /* n, o, p, q, r */
    -1, -1, DTF_DATETIME_FMT_p_m, DTF_DATETIME_FMT_q, DTF_DATETIME_FMT_rm,     /* 110 ~ 114 */
    /* s, t, u, v, w */
    DTF_DATETIME_FMT_sssss, DTF_DATETIME_FMT_tzh, -1, -1, DTF_DATETIME_FMT_ww,     /* 115 ~ 119 */
    /* x, y, z, -1, -1 */
    -1, DTF_DATETIME_FMT_y_yyy, -1, -1, -1,     /* 120 ~ 124 */
    -1, -1, -1, -1, -1,     /* 125 ~ 129 */
    -1, -1, -1, -1, -1,     /* 130 ~ 134 */
    -1, -1, -1, -1, -1,     /* 135 ~ 139 */
    -1, -1, -1, -1, -1,     /* 140 ~ 144 */
    -1, -1, -1, -1, -1,     /* 145 ~ 149 */
    -1, -1, -1, -1, -1,     /* 150 ~ 154 */
    -1, -1, -1, -1, -1,     /* 155 ~ 159 */
    -1, -1, -1, -1, -1,     /* 160 ~ 164 */
    -1, -1, -1, -1, -1,     /* 165 ~ 169 */
    -1, -1, -1, -1, -1,     /* 170 ~ 174 */
    -1, -1, -1, -1, -1,     /* 175 ~ 179 */
    -1, -1, -1, -1, -1,     /* 180 ~ 184 */
    -1, -1, -1, -1, -1,     /* 185 ~ 189 */
    -1, -1, -1, -1, -1,     /* 190 ~ 194 */
    -1, -1, -1, -1, -1,     /* 195 ~ 199 */
    -1, -1, -1, -1, -1,     /* 200 ~ 204 */
    -1, -1, -1, -1, -1,     /* 205 ~ 209 */
    -1, -1, -1, -1, -1,     /* 210 ~ 214 */
    -1, -1, -1, -1, -1,     /* 215 ~ 219 */
    -1, -1, -1, -1, -1,     /* 220 ~ 224 */
    -1, -1, -1, -1, -1,     /* 225 ~ 229 */
    -1, -1, -1, -1, -1,     /* 230 ~ 234 */
    -1, -1, -1, -1, -1,     /* 235 ~ 239 */
    -1, -1, -1, -1, -1,     /* 240 ~ 244 */
    -1, -1, -1, -1, -1,     /* 245 ~ 249 */
    -1, -1, -1, -1, -1, -1  /* 250 ~ 225 */
    
};

/* ", &, ', 0 ~ 9, A ~ Z, a ~ z 는 제외된 특수문자들의 배열 */
const stlInt32 DTF_DATETIME_FORMAT_SPECIAL_CHAR_INDEX[256] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 0 ~ 9 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 10 ~ 19 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 20 ~ 29 */
    /* -1, -1, space, !, -1, #, $, %, -1, ' */    
    -1, -1,  0,  0, -1,  0,  0,  0, -1,  0,   /* 30 ~ 39 */
    /* (, ), *, +, ,, -, ., /, -1, -1, */
     0,  0,  0,  0,  0,  0,  0,  0, -1, -1,   /* 40 ~ 49 */
    /* -1, -1, -1, -1, -1, -1, -1, -1, :, ;, */
    -1, -1, -1, -1, -1, -1, -1, -1,  0,  0,   /* 50 ~ 59 */
    /* <,  =,  >,  ?,  @, -1, -1, -1, -1, -1, */
     0,  0,  0,  0,  0, -1, -1, -1, -1, -1,   /* 60 ~ 69 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 70 ~ 79 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 80 ~ 89 */
    /* -1, [, \, ], ^, _, `, -1, -1, -1, */
    -1,  0,  0,  0,  0,  0,  0, -1, -1, -1,   /* 90 ~ 99 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 100 ~ 109 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 110 ~ 119 */
    /* -1, -1, -1, {, |, }, ~, -1, -1, -1, */
    -1, -1, -1,  0,  0,  0,  0, -1, -1, -1,   /* 120 ~ 129 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 130 ~ 139 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 140 ~ 149 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 150 ~ 159 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 160 ~ 169 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 170 ~ 179 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 180 ~ 189 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 190 ~ 199 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 200 ~ 209 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 210 ~ 219 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 220 ~ 229 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 230 ~ 239 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   /* 240 ~ 249 */
    -1, -1, -1, -1, -1, -1                    /* 250 ~ 255 */
};


/************************************
 * DATETIME 을 위한 keywords
 ************************************/

const dtlDateTimeFormatKeyWord dtfDateTimeFormattingKeyword[] =
{
    /**    
     * { NAME, NAME_LEN, VALUE_MAX_LEN, ID, ISDIGIT, DATEMODE, TYPEMODE, USABLE_TO_DATE }
     */

    /**
     * 아래 배열은 NAME의 첫번째 문자가 알파벳 순서로 정렬되어 추가해야 함.
     */
    
    { "A.D.",   DTF_DATETIME_FMT_A_D_STR_LEN, DTF_DATETIME_FMT_A_D_STR_LEN,          DTF_DATETIME_FMT_A_D,    STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "A.M.",   DTF_DATETIME_FMT_A_M_STR_LEN, DTF_DATETIME_FMT_A_M_STR_LEN,          DTF_DATETIME_FMT_A_M,    STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "AD",     DTF_DATETIME_FMT_AD_STR_LEN,  DTF_DATETIME_FMT_AD_STR_LEN,           DTF_DATETIME_FMT_AD,     STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "AM",     DTF_DATETIME_FMT_AM_STR_LEN,  DTF_DATETIME_FMT_AM_STR_LEN,           DTF_DATETIME_FMT_AM,     STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "Am",     DTF_DATETIME_FMT_AM_STR_LEN,  DTF_DATETIME_FMT_AM_STR_LEN,           DTF_DATETIME_FMT_Am,     STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "B.C.",   DTF_DATETIME_FMT_B_C_STR_LEN, DTF_DATETIME_FMT_B_C_STR_LEN,          DTF_DATETIME_FMT_B_C,    STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "BC",     DTF_DATETIME_FMT_BC_STR_LEN,  DTF_DATETIME_FMT_BC_STR_LEN,           DTF_DATETIME_FMT_BC,     STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "CC",     2,                            3,                                     DTF_DATETIME_FMT_CC,     STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_FALSE },
    { "DAY",    3,                            DTF_DATETIME_FMT_DAY_MAX_STR_LEN,      DTF_DATETIME_FMT_DAY,    STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "Day",    3,                            DTF_DATETIME_FMT_DAY_MAX_STR_LEN,      DTF_DATETIME_FMT_Day,    STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "DDD",    3,                            3,                                     DTF_DATETIME_FMT_DDD,    STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "DD",     2,                            2,                                     DTF_DATETIME_FMT_DD,     STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },    
    { "DY",     2,                            DTF_DATETIME_FMT_DY_MAX_STR_LEN,       DTF_DATETIME_FMT_DY,     STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "Dy",     2,                            DTF_DATETIME_FMT_DY_MAX_STR_LEN,       DTF_DATETIME_FMT_Dy,     STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },    
    { "D",      1,                            1,                                     DTF_DATETIME_FMT_D,      STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "FF1",    3,                            1,                                     DTF_DATETIME_FMT_FF1,    STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "FF2",    3,                            2,                                     DTF_DATETIME_FMT_FF2,    STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "FF3",    3,                            3,                                     DTF_DATETIME_FMT_FF3,    STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "FF4",    3,                            4,                                     DTF_DATETIME_FMT_FF4,    STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "FF5",    3,                            5,                                     DTF_DATETIME_FMT_FF5,    STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "FF6",    3,                            6,                                     DTF_DATETIME_FMT_FF6,    STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "FF",     2,                            6,                                     DTF_DATETIME_FMT_FF,     STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "HH24",   4,                            2,                                     DTF_DATETIME_FMT_HH24,   STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "HH12",   4,                            2,                                     DTF_DATETIME_FMT_HH12,   STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "HH",     2,                            2,                                     DTF_DATETIME_FMT_HH,     STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
 // { "IW",     2, DTF_DATETIME_FMT_IW,     STL_TRUE,  DTL_FORMAT_DATE_ISOWEEK },
 // { "IYYY",   4, DTF_DATETIME_FMT_IYYY,   STL_TRUE,  DTL_FORMAT_DATE_ISOWEEK },
 // { "IYY",    3, DTF_DATETIME_FMT_IYY,    STL_TRUE,  DTL_FORMAT_DATE_ISOWEEK },
 // { "IY",     2, DTF_DATETIME_FMT_IY,     STL_TRUE,  DTL_FORMAT_DATE_ISOWEEK },
 // { "I",      1, DTF_DATETIME_FMT_I,      STL_TRUE,  DTL_FORMAT_DATE_ISOWEEK },
    { "J",      1,                            7,                                     DTF_DATETIME_FMT_J,      STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "MI",     2,                            2,                                     DTF_DATETIME_FMT_MI,     STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "MM",     2,                            2,                                     DTF_DATETIME_FMT_MM ,    STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "MONTH",  5,                            DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,    DTF_DATETIME_FMT_MONTH,  STL_FALSE, DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "Month",  5,                            DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,    DTF_DATETIME_FMT_Month,  STL_FALSE, DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "MON",    3,                            DTF_DATETIME_FMT_MON_MAX_STR_LEN,      DTF_DATETIME_FMT_MON,    STL_FALSE, DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "Mon",    3,                            DTF_DATETIME_FMT_MON_MAX_STR_LEN,      DTF_DATETIME_FMT_Mon,    STL_FALSE, DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "P.M.",   4,                            DTF_DATETIME_FMT_P_M_STR_LEN,          DTF_DATETIME_FMT_P_M,    STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "PM",     2,                            DTF_DATETIME_FMT_PM_STR_LEN,           DTF_DATETIME_FMT_PM,     STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "Pm",     2,                            DTF_DATETIME_FMT_PM_STR_LEN,           DTF_DATETIME_FMT_Pm,     STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "Q",      1,                            1,                                     DTF_DATETIME_FMT_Q,      STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_FALSE },
    { "RM",     2,                            DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN, DTF_DATETIME_FMT_RM,     STL_FALSE, DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "Rm",     2,                            DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN, DTF_DATETIME_FMT_Rm,     STL_FALSE, DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "RRRR",   4,                            4,                                     DTF_DATETIME_FMT_RRRR,   STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "RR",     2,                            2,                                     DTF_DATETIME_FMT_RR,     STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },        
    { "SSSSS",  5,                            5,                                     DTF_DATETIME_FMT_SSSSS,  STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "SYYYY",  5,                            4,                                     DTF_DATETIME_FMT_SYYYY,  STL_FALSE, DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "SS",     2,                            2,                                     DTF_DATETIME_FMT_SS,     STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "TZH",    3,                            2,                                     DTF_DATETIME_FMT_TZH,    STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_WITH_TIME_ZONE, STL_TRUE },
    { "TZM",    3,                            2,                                     DTF_DATETIME_FMT_TZM,    STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_WITH_TIME_ZONE, STL_TRUE },
    { "WW",     2,                            2,                                     DTF_DATETIME_FMT_WW,     STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_FALSE },
    { "W",      1,                            1,                                     DTF_DATETIME_FMT_W,      STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_FALSE },
    { "Y,YYY",  5,                            5,                                     DTF_DATETIME_FMT_Y_YYY,  STL_FALSE, DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },    
    { "YYYY",   4,                            4,                                     DTF_DATETIME_FMT_YYYY,   STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "YYY",    3,                            3,                                     DTF_DATETIME_FMT_YYY,    STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "YY",     2,                            2,                                     DTF_DATETIME_FMT_YY,     STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "Y",      1,                            1,                                     DTF_DATETIME_FMT_Y,      STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },

    
    
    { "a.d.",   DTF_DATETIME_FMT_A_D_STR_LEN, DTF_DATETIME_FMT_A_D_STR_LEN,          DTF_DATETIME_FMT_a_d,    STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "a.m.",   DTF_DATETIME_FMT_A_M_STR_LEN, DTF_DATETIME_FMT_A_M_STR_LEN,          DTF_DATETIME_FMT_a_m,    STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "ad",     DTF_DATETIME_FMT_AD_STR_LEN,  DTF_DATETIME_FMT_AD_STR_LEN,           DTF_DATETIME_FMT_ad,     STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "am",     DTF_DATETIME_FMT_AM_STR_LEN,  DTF_DATETIME_FMT_AM_STR_LEN,           DTF_DATETIME_FMT_am,     STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "b.c.",   DTF_DATETIME_FMT_B_C_STR_LEN, DTF_DATETIME_FMT_B_C_STR_LEN,          DTF_DATETIME_FMT_b_c,    STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "bc",     DTF_DATETIME_FMT_BC_STR_LEN,  DTF_DATETIME_FMT_BC_STR_LEN,           DTF_DATETIME_FMT_bc,     STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "cc",     2,                            3,                                     DTF_DATETIME_FMT_CC,     STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_FALSE },
    { "day",    3,                            DTF_DATETIME_FMT_DAY_MAX_STR_LEN,      DTF_DATETIME_FMT_day,    STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "ddd",    3,                            3,                                     DTF_DATETIME_FMT_DDD,    STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },    
    { "dd",     2,                            2,                                     DTF_DATETIME_FMT_DD,     STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },    
    { "dy",     2,                            DTF_DATETIME_FMT_DY_MAX_STR_LEN,       DTF_DATETIME_FMT_dy,     STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "d",      1,                            1,                                     DTF_DATETIME_FMT_D,      STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "ff1",    3,                            1,                                     DTF_DATETIME_FMT_FF1,    STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "ff2",    3,                            2,                                     DTF_DATETIME_FMT_FF2,    STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "ff3",    3,                            3,                                     DTF_DATETIME_FMT_FF3,    STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "ff4",    3,                            4,                                     DTF_DATETIME_FMT_FF4,    STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "ff5",    3,                            5,                                     DTF_DATETIME_FMT_FF5,    STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "ff6",    3,                            6,                                     DTF_DATETIME_FMT_FF6,    STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "ff",     2,                            6,                                     DTF_DATETIME_FMT_FF,     STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },            
    { "hh24",   4,                            2,                                     DTF_DATETIME_FMT_HH24,   STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "hh12",   4,                            2,                                     DTF_DATETIME_FMT_HH12,   STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "hh",     2,                            2,                                     DTF_DATETIME_FMT_HH,     STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
 // { "iw",     2, DTF_DATETIME_FMT_IW,     STL_TRUE,  DTL_FORMAT_DATE_ISOWEEK },
 // { "iyyy",   4, DTF_DATETIME_FMT_IYYY,   STL_TRUE,  DTL_FORMAT_DATE_ISOWEEK },
 // { "iyy",    3, DTF_DATETIME_FMT_IYY,    STL_TRUE,  DTL_FORMAT_DATE_ISOWEEK },
 // { "iy",     2, DTF_DATETIME_FMT_IY,     STL_TRUE,  DTL_FORMAT_DATE_ISOWEEK },
 // { "i",      1, DTF_DATETIME_FMT_I,      STL_TRUE,  DTL_FORMAT_DATE_ISOWEEK },
    { "j",      1,                            7,                                     DTF_DATETIME_FMT_J,      STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "mi",     2,                            2,                                     DTF_DATETIME_FMT_MI,     STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "mm",     2,                            2,                                     DTF_DATETIME_FMT_MM ,    STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "month",  5,                            DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,    DTF_DATETIME_FMT_month,  STL_FALSE, DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "mon",    3,                            DTF_DATETIME_FMT_MON_MAX_STR_LEN,      DTF_DATETIME_FMT_mon,    STL_FALSE, DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "p.m.",   4,                            DTF_DATETIME_FMT_P_M_STR_LEN,          DTF_DATETIME_FMT_p_m,    STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "pm",     2,                            DTF_DATETIME_FMT_PM_STR_LEN,           DTF_DATETIME_FMT_pm,     STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "q",      1,                            1,                                     DTF_DATETIME_FMT_Q,      STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_FALSE },
    { "rm",     2,                            DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN, DTF_DATETIME_FMT_rm,     STL_FALSE, DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "rrrr",   4,                            4,                                     DTF_DATETIME_FMT_RRRR,   STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "rr",     2,                            2,                                     DTF_DATETIME_FMT_RR,     STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "sssss",  5,                            5,                                     DTF_DATETIME_FMT_SSSSS,  STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "syyyy",  5,                            4,                                     DTF_DATETIME_FMT_SYYYY,  STL_FALSE, DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "ss",     2,                            2,                                     DTF_DATETIME_FMT_SS,     STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_TIME, STL_TRUE },
    { "tzh",    3,                            2,                                     DTF_DATETIME_FMT_TZH,    STL_FALSE, DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_WITH_TIME_ZONE, STL_TRUE },
    { "tzm",    3,                            2,                                     DTF_DATETIME_FMT_TZM,    STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_WITH_TIME_ZONE, STL_TRUE },
    { "ww",     2,                            2,                                     DTF_DATETIME_FMT_WW,     STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_FALSE },
    { "w",      1,                            1,                                     DTF_DATETIME_FMT_W,      STL_TRUE,  DTL_FORMAT_DATE_NONE, DTL_FORMAT_TYPE_DATE, STL_FALSE },
    { "y,yyy",  5,                            5,                                     DTF_DATETIME_FMT_Y_YYY,  STL_FALSE, DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "yyyy",   4,                            4,                                     DTF_DATETIME_FMT_YYYY,   STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "yyy",    3,                            3,                                     DTF_DATETIME_FMT_YYY,    STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "yy",     2,                            2,                                     DTF_DATETIME_FMT_YY,     STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    { "y",      1,                            1,                                     DTF_DATETIME_FMT_Y,      STL_TRUE,  DTL_FORMAT_DATE_GREGORIAN, DTL_FORMAT_TYPE_DATE, STL_TRUE },
    
    /* last */
    { NULL, 0, 0, 0, 0, 0, 0, STL_FALSE }
};


/*******************************
 * MON, Mon, mon 표시
 *******************************/

dtfDateTimeFmtStr const DTF_DATETIME_FMT_MON_UPPER_STR[] =
{
    { "JAN", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  1 },
    { "FEB", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  2 },
    { "MAR", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  3 },
    { "APR", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  4 },
    { "MAY", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  5 },
    { "JUN", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  6 },
    { "JUL", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  7 },
    { "AUG", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  8 },
    { "SEP", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  9 },
    { "OCT", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN, 10 },
    { "NOV", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN, 11 },
    { "DEC", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN, 12 },
    
    { NULL , 0,  0,  0 }
};

dtfDateTimeFmtStr const DTF_DATETIME_FMT_MON_FIRST_UPPER_STR[] =
{
    { "Jan", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  1 },
    { "Feb", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  2 },
    { "Mar", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  3 },
    { "Apr", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  4 },
    { "May", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  5 },
    { "Jun", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  6 },
    { "Jul", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  7 },
    { "Aug", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  8 },
    { "Sep", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  9 },
    { "Oct", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN, 10 },
    { "Nov", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN, 11 },
    { "Dec", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN, 12 },
    
    { NULL,  0,  0,  0 } 
};

dtfDateTimeFmtStr const DTF_DATETIME_FMT_MON_LOWER_STR[] =
{
    { "jan", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  1 },
    { "feb", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  2 },
    { "mar", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  3 },
    { "apr", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  4 },
    { "may", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  5 },
    { "jun", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  6 },
    { "jul", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  7 },
    { "aug", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  8 },
    { "sep", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN,  9 },
    { "oct", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN, 10 },
    { "nov", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN, 11 },
    { "dec", DTF_DATETIME_FMT_MON_STR_LEN, DTF_DATETIME_FMT_MON_MAX_STR_LEN, 12 },

    { NULL,  0,  0,  0 } 
};


/*******************************
 * MONTH, Month, month 표시
 *******************************/

dtfDateTimeFmtStr const DTF_DATETIME_FMT_MONTH_UPPER_STR[] =
{
    { "JANUARY  ", 7, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  1 },
    { "FEBRUARY ", 8, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  2 },
    { "MARCH    ", 5, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  3 },
    { "APRIL    ", 5, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  4 },
    { "MAY      ", 3, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  5 },
    { "JUNE     ", 4, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  6 },
    { "JULY     ", 4, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  7 },
    { "AUGUST   ", 6, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  8 },
    { "SEPTEMBER", 9, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  9 },
    { "OCTOBER  ", 7, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN, 10 },
    { "NOVEMBER ", 8, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN, 11 },
    { "DECEMBER ", 8, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN, 12 },
    
    { NULL,        0, 0,  0 }
};

dtfDateTimeFmtStr const DTF_DATETIME_FMT_MONTH_FIRST_UPPER_STR[] =
{
    { "January  ", 7, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  1 },
    { "February ", 8, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  2 },
    { "March    ", 5, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  3 },
    { "April    ", 5, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  4 },
    { "May      ", 3, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  5 },
    { "June     ", 4, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  6 },
    { "July     ", 4, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  7 },
    { "August   ", 6, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  8 },
    { "September", 9, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  9 },
    { "October  ", 7, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN, 10 },
    { "November ", 8, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN, 11 },
    { "December ", 8, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN, 12 },
    
    { NULL,        0, 0,  0 }
};

dtfDateTimeFmtStr const DTF_DATETIME_FMT_MONTH_LOWER_STR[] =
{
    { "january  ", 7, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  1 },
    { "february ", 8, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  2 },
    { "march    ", 5, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  3 },
    { "april    ", 5, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  4 },
    { "may      ", 3, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  5 },
    { "june     ", 4, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  6 },
    { "july     ", 4, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  7 },
    { "august   ", 6, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  8 },
    { "september", 9, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN,  9 },
    { "october  ", 7, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN, 10 },
    { "november ", 8, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN, 11 },
    { "december ", 8, DTF_DATETIME_FMT_MONTH_MAX_STR_LEN, 12 },
    
    { NULL,        0, 0,  0 }
};

/*******************************
 * 월(month)의 로마 숫자 표시
 *******************************/

dtfDateTimeFmtStr const DTF_DATETIME_FMT_RM_MONTH_UPPER_STR[] =
{
    { "I   ", 1, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  1 },
    { "II  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  2 },
    { "III ", 3, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  3 },
    { "IV  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  4 },
    { "V   ", 1, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  5 },    
    { "VI  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  6 },
    { "VII ", 3, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  7 },
    { "VIII", 4, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  8 },
    { "IX  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  9 },
    { "X   ", 1, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN, 10 },    
    { "XI  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN, 11 },
    { "XII ", 3, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN, 12 },
    
    { NULL,   0, 0,  0 }
};

dtfDateTimeFmtStr const DTF_DATETIME_FMT_RM_MONTH_FIRST_UPPER_STR[] =
{
    { "I   ", 1, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  1 },
    { "Ii  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  2 },
    { "Iii ", 3, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  3 },
    { "Iv  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  4 },
    { "V   ", 1, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  5 },    
    { "Vi  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  6 },
    { "Vii ", 3, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  7 },
    { "Viii", 4, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  8 },
    { "Ix  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  9 },
    { "X   ", 1, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN, 10 },    
    { "Xi  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN, 11 },
    { "Xii ", 3, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN, 12 },
    
    { NULL,   0, 0,  0 }
};

dtfDateTimeFmtStr const DTF_DATETIME_FMT_RM_MONTH_LOWER_STR[] =
{
    { "i   ", 1, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  1 },
    { "ii  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  2 },
    { "iii ", 3, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  3 },
    { "iv  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  4 },
    { "v   ", 1, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  5 },    
    { "vi  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  6 },
    { "vii ", 3, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  7 },
    { "viii", 4, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  8 },
    { "ix  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN,  9 },
    { "x   ", 1, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN, 10 },
    { "xi  ", 2, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN, 11 },
    { "xii ", 3, DTF_DATETIME_FMT_RM_MONTH_MAX_STR_LEN, 12 },

    { NULL,   0, 0,  0 }
};

/*******************************
 * 일(day)의 dayname 표시
 *******************************/

dtfDateTimeFmtStr const DTF_DATETIME_FMT_DAY_UPPER_STR[] =
{
    { "SUNDAY   ", 6, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  0 },
    { "MONDAY   ", 6, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  1 },
    { "TUESDAY  ", 7, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  2 },
    { "WEDNESDAY", 9, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  3 },
    { "THURSDAY ", 8, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  4 },
    { "FRIDAY   ", 6, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  5 },
    { "SATURDAY ", 8, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  6 },
    
    { NULL,        0, 0,  0 }
};

dtfDateTimeFmtStr const DTF_DATETIME_FMT_DAY_FIRST_UPPER_STR[] =
{
    { "Sunday   ", 6, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  0 },
    { "Monday   ", 6, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  1 },
    { "Tuesday  ", 7, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  2 },
    { "Wednesday", 9, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  3 },
    { "Thursday ", 8, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  4 },
    { "Friday   ", 6, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  5 },
    { "Saturday ", 8, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  6 },
    
    { NULL,        0, 0,  0 }    
};

dtfDateTimeFmtStr const DTF_DATETIME_FMT_DAY_LOWER_STR[] =
{
    { "sunday   ", 6, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  0 },
    { "monday   ", 6, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  1 },
    { "tuesday  ", 7, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  2 },
    { "wednesday", 9, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  3 },
    { "thursday ", 8, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  4 },
    { "friday   ", 6, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  5 },
    { "saturday ", 8, DTF_DATETIME_FMT_DAY_MAX_STR_LEN,  6 },
    
    { NULL,        0, 0,  0 }    
};

/*******************************
 * 일(dy)의 Abbreviated name of day. 표시
 *******************************/

dtfDateTimeFmtStr const DTF_DATETIME_FMT_DY_UPPER_STR[] =
{
    { "SUN",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 0 },
    { "MON",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 1 },
    { "TUE",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 2 },
    { "WED",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 3 },
    { "THU",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 4 },
    { "FRI",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 5 },
    { "SAT",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 6 },
    
    { NULL,   0,  0,  0 }
};

dtfDateTimeFmtStr const DTF_DATETIME_FMT_DY_FIRST_UPPER_STR[] =
{
    { "Sun",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 0 },
    { "Mon",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 1 },
    { "Tue",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 2 },
    { "Wed",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 3 },
    { "Thu",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 4 },
    { "Fri",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 5 },
    { "Sat",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 6 },
    
    { NULL,   0,  0,  0 }    
};

dtfDateTimeFmtStr const DTF_DATETIME_FMT_DY_LOWER_STR[] =
{
    { "sun",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 0 },
    { "mon",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 1 },
    { "tue",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 2 },
    { "wed",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 3 },
    { "thu",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 4 },
    { "fri",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 5 },
    { "sat",  DTF_DATETIME_FMT_DY_STR_LEN, DTF_DATETIME_FMT_DY_MAX_STR_LEN, 6 },
    
    { NULL,   0,  0,  0 }    
};


/*******************************
 * 6자리 fractional second를 생성하기 위한 adjust scale
 *******************************/

static const stlInt64 dtfFractionalSecondAdjustScale[DTL_TIME_MAX_PRECISION + 1] =
{
    STL_INT64_C(1000000),
    STL_INT64_C(100000),
    STL_INT64_C(10000),
    STL_INT64_C(1000),
    STL_INT64_C(100),
    STL_INT64_C(10),
    STL_INT64_C(1)
};



/***************
 * NUMBER 의 format element의 index
 ***************/

const stlInt32 DTF_NUMBER_FORMAT_INDEX[256] =
{
    -1, -1, -1, -1, -1,     /*  0 ~ 4  */
    -1, -1, -1, -1, -1,     /*  5 ~ 9  */    
    -1, -1, -1, -1, -1,     /* 10 ~ 14 */
    -1, -1, -1, -1, -1,     /* 15 ~ 19 */    
    -1, -1, -1, -1, -1,     /* 20 ~ 24 */
    -1, -1, -1, -1, -1,     /* 25 ~ 29 */    
    -1, -1, -1, -1, -1,     /* 30 ~ 34 */
    -1, DTF_NUMBER_FMT_DOLLAR, -1, -1, -1,     /* 35 ~ 39 */
    -1, -1, -1, -1, DTF_NUMBER_FMT_COMMA,      /* 40 ~ 44 */
    -1, DTF_NUMBER_FMT_PERIOD, -1, DTF_NUMBER_FMT_0, -1,     /* 45 ~ 49 */    
    -1, -1, -1, -1, -1,     /* 50 ~ 54 */
    /* -1, -1, 9, -1, -1*/
    -1, -1, DTF_NUMBER_FMT_9, -1, -1,     /* 55 ~ 59 */    
    -1, -1, -1, -1, -1,     /* 60 ~ 64 */
    /* A, B, C, D, E */ 
    -1, DTF_NUMBER_FMT_B, -1, -1, DTF_NUMBER_FMT_EEEE,     /* 65 ~ 69 */
    /* F, G, H, I, J */
    DTF_NUMBER_FMT_FM, -1, -1, -1, -1,    /* 70 ~ 74 */    
    /* K, L, M, N, O */
    -1, -1, DTF_NUMBER_FMT_MI, -1, -1,     /* 75 ~ 79 */
    /* P, Q, R, S, T */
    DTF_NUMBER_FMT_PR, -1, DTF_NUMBER_FMT_RN, DTF_NUMBER_FMT_S, -1,     /* 80 ~ 84 */
    /* U, V, W, X, Y */
    -1, DTF_NUMBER_FMT_V, -1, DTF_NUMBER_FMT_X, -1,     /* 85 ~ 89 */
    /* Z, -1, -1, -1, -1, */
    -1, -1, -1, -1, -1,     /* 90 ~ 94 */
    /* -1, -1, a, b, c */
    -1, -1, -1, DTF_NUMBER_FMT_b, -1,     /* 95 ~ 99 */
    /* d, e, f, g, h */
    -1, DTF_NUMBER_FMT_eeee, DTF_NUMBER_FMT_FM, -1, -1,     /* 100 ~ 104 */
    /* i, j, k, l, m */
    -1, -1, -1, -1, DTF_NUMBER_FMT_mi,     /* 105 ~ 109 */
    /* n, o, p, q, r */
    -1, -1, DTF_NUMBER_FMT_pr, -1, DTF_NUMBER_FMT_rn,     /* 110 ~ 114 */
    /* s, t, u, v, w */
    DTF_NUMBER_FMT_s, -1, -1, DTF_NUMBER_FMT_v, -1,     /* 115 ~ 119 */
    /* x, y, z, -1, -1 */
    DTF_NUMBER_FMT_x, -1, -1, -1, -1,     /* 120 ~ 124 */
    -1, -1, -1, -1, -1,     /* 125 ~ 129 */
    -1, -1, -1, -1, -1,     /* 130 ~ 134 */
    -1, -1, -1, -1, -1,     /* 135 ~ 139 */
    -1, -1, -1, -1, -1,     /* 140 ~ 144 */
    -1, -1, -1, -1, -1,     /* 145 ~ 149 */
    -1, -1, -1, -1, -1,     /* 150 ~ 154 */
    -1, -1, -1, -1, -1,     /* 155 ~ 159 */
    -1, -1, -1, -1, -1,     /* 160 ~ 164 */
    -1, -1, -1, -1, -1,     /* 165 ~ 169 */
    -1, -1, -1, -1, -1,     /* 170 ~ 174 */
    -1, -1, -1, -1, -1,     /* 175 ~ 179 */
    -1, -1, -1, -1, -1,     /* 180 ~ 184 */
    -1, -1, -1, -1, -1,     /* 185 ~ 189 */
    -1, -1, -1, -1, -1,     /* 190 ~ 194 */
    -1, -1, -1, -1, -1,     /* 195 ~ 199 */
    -1, -1, -1, -1, -1,     /* 200 ~ 204 */
    -1, -1, -1, -1, -1,     /* 205 ~ 209 */
    -1, -1, -1, -1, -1,     /* 210 ~ 214 */
    -1, -1, -1, -1, -1,     /* 215 ~ 219 */
    -1, -1, -1, -1, -1,     /* 220 ~ 224 */
    -1, -1, -1, -1, -1,     /* 225 ~ 229 */
    -1, -1, -1, -1, -1,     /* 230 ~ 234 */
    -1, -1, -1, -1, -1,     /* 235 ~ 239 */
    -1, -1, -1, -1, -1,     /* 240 ~ 244 */
    -1, -1, -1, -1, -1,     /* 245 ~ 249 */
    -1, -1, -1, -1, -1, -1  /* 250 ~ 225 */
    
};


/************************************
 * NUMBER 을 위한 keywords
 ************************************/

const dtfNumberFormatKeyWord dtfNumberFormattingKeyword[] =
{
    /**    
     * { NAME, NAME_LEN, ID }
     */
    
    /**
     * 아래 배열은 NAME의 첫번째 문자가 알파벳 순서로 정렬되어 추가해야 함.
     */

    { "$" ,   1, DTF_NUMBER_FMT_DOLLAR },
    { "," ,   1, DTF_NUMBER_FMT_COMMA },
    { "." ,   1, DTF_NUMBER_FMT_PERIOD },
    { "0" ,   1, DTF_NUMBER_FMT_0 },
    { "9" ,   1, DTF_NUMBER_FMT_9 },
    { "B" ,   1, DTF_NUMBER_FMT_B },
    { "EEEE", 4, DTF_NUMBER_FMT_EEEE },
    { "FM",   2, DTF_NUMBER_FMT_FM },    
    { "MI",   2, DTF_NUMBER_FMT_MI },
    { "PR",   2, DTF_NUMBER_FMT_PR },
    { "RN",   2, DTF_NUMBER_FMT_RN },
    { "S" ,   1, DTF_NUMBER_FMT_S },
    { "V" ,   1, DTF_NUMBER_FMT_V },
    { "X" ,   1, DTF_NUMBER_FMT_X },
    

    { "b" ,   1, DTF_NUMBER_FMT_b },
    { "eeee", 4, DTF_NUMBER_FMT_eeee },
    { "fm",   2, DTF_NUMBER_FMT_fm },        
    { "mi",   2, DTF_NUMBER_FMT_mi },
    { "pr",   2, DTF_NUMBER_FMT_pr },
    { "rn",   2, DTF_NUMBER_FMT_rn },
    { "s" ,   1, DTF_NUMBER_FMT_s },
    { "v" ,   1, DTF_NUMBER_FMT_v },
    { "x" ,   1, DTF_NUMBER_FMT_x },

    
    /* last */
    { NULL, 0, 0 }
};

/************************************
 * NUMBER 의 로마숫자 문자열 정보 구조체 
 ************************************/

dtfRomanNumeral dtfRomanNumeralStringList[2][4][10] =
{
    {
        {
            { NULL, 0, 0 },
            { "i", 1, 1 },
            { "ii", 2, 2 },
            { "iii", 3, 3 },
            { "iv", 2, 4 },
            { "v", 1, 5 },
            { "vi", 2, 6 },
            { "vii", 3, 7 },
            { "viii", 4, 8 },
            { "ix", 2, 9 },
        },
        {
            { NULL, 0, 0 },
            { "x", 1, 10 },
            { "xx", 2, 20 },
            { "xxx", 3, 30 },
            { "xl", 2, 40 },
            { "l", 1, 50 },
            { "lx", 2, 60 },
            { "lxx", 3, 70 },
            { "lxxx", 4, 80 },
            { "xc", 2, 90 },
        },
        {
            { NULL, 0, 0 },
            { "c", 1, 10 },
            { "cc", 2, 20 },
            { "ccc", 3, 30 },
            { "cd", 2, 40 },
            { "d", 1, 50 },
            { "dc", 2, 60 },
            { "dcc", 3, 70 },
            { "dccc", 4, 80 },
            { "cm", 2, 90 },
        },
        {
            { NULL, 0, 0 },
            { "m", 1, 1000 },
            { "mm", 2, 2000 },
            { "mmm", 3, 3000 },
            { NULL, 0, 0 },
            { NULL, 0, 0 },
            { NULL, 0, 0 },
            { NULL, 0, 0 },
            { NULL, 0, 0 },
            { NULL, 0, 0 },
        }
    },

    {
        {
            { NULL, 0, 0 },
            { "I", 1, 1 },
            { "II", 2, 2 },
            { "III", 3, 3 },
            { "IV", 2, 4 },
            { "V", 1, 5 },
            { "VI", 2, 6 },
            { "VII", 3, 7 },
            { "VIII", 4, 8 },
            { "IX", 2, 9 },
        }, 
        {
            { NULL, 0, 0 },
            { "X", 1, 10 },
            { "XX", 2, 20 },
            { "XXX", 3, 30 },
            { "XL", 2, 40 },
            { "L", 1, 50 },
            { "LX", 2, 60 },
            { "LXX", 3, 70 },
            { "LXXX", 4, 80 },
            { "XC", 2, 90 },
        },
        {
            { NULL, 0, 0 },
            { "C", 1, 10 },
            { "CC", 2, 20 },
            { "CCC", 3, 30 },
            { "CD", 2, 40 },
            { "D", 1, 50 },
            { "DC", 2, 60 },
            { "DCC", 3, 70 },
            { "DCCC", 4, 80 },
            { "CM", 2, 90 },
        },
        {
            { NULL, 0, 0 },
            { "M", 1, 1000 },
            { "MM", 2, 2000 },
            { "MMM", 3, 3000 },
            { NULL, 0, 0 },
            { NULL, 0, 0 },
            { NULL, 0, 0 },
            { NULL, 0, 0 },
            { NULL, 0, 0 },
            { NULL, 0, 0 },
        }
    }
};


/************************************
 * NUMBER 의 16진수 문자열 정보 구조체 
 ************************************/

stlChar dtfHexaToString[2][16] =
{
    { '0', '1', '2', '3',
      '4', '5', '6', '7',
      '8', '9', 'a', 'b',
      'c', 'd', 'e', 'f' },
    
    { '0', '1', '2', '3',
      '4', '5', '6', '7',
      '8', '9', 'A', 'B',
      'C', 'D', 'E', 'F' }
};


/************************************
 * MM, DD, MI, HH 등을 위한 2 digit 상수 
 ************************************/

static const stlChar gDtfTwoDigitString[100][3] =
{ "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
  "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
  "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
  "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
  "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
  "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
  "60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
  "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
  "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
  "90", "91", "92", "93", "94", "95", "96", "97", "98", "99"
};



/************************************
 * Format func pointer list
 ************************************/
dtfNumberFormatFuncPtr dtfToCharFormatFuncList[ DTL_TYPE_MAX ] =
{
    NULL, NULL, NULL, NULL,
    dtfToCharRealToFormatString,    /**< DTL_TYPE_NATIVE_REAL */
    dtfToCharDoubleToFormatString,  /**< DTL_TYPE_NATIVE_DOUBLE */
    NULL,
    dtfToCharNumberToFormatString,  /**< DTL_TYPE_NUMBER */  
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL
};

dtfNumberFormatFuncPtr dtfToCharEEEEFormatFuncList[ DTL_TYPE_MAX ] =
{
    NULL, NULL, NULL, NULL,
    dtfToCharRealToEEEEFormatString,    /**< DTL_TYPE_NATIVE_REAL */
    dtfToCharDoubleToEEEEFormatString,  /**< DTL_TYPE_NATIVE_DOUBLE */
    NULL,
    dtfToCharNumberToEEEEFormatString,  /**< DTL_TYPE_NUMBER */  
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL
};

dtfNumberFormatFuncPtr dtfToCharRNFormatFuncList[ DTL_TYPE_MAX ] =
{
    NULL, NULL, NULL, NULL,
    dtfToCharRealToRNFormatString,    /**< DTL_TYPE_NATIVE_REAL */
    dtfToCharDoubleToRNFormatString,  /**< DTL_TYPE_NATIVE_DOUBLE */
    NULL,
    dtfToCharNumberToRNFormatString,  /**< DTL_TYPE_NUMBER */  
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL
};

dtfNumberFormatFuncPtr dtfToCharVFormatFuncList[ DTL_TYPE_MAX ] =
{
    NULL, NULL, NULL, NULL,
    dtfToCharRealToVFormatString,    /**< DTL_TYPE_NATIVE_REAL */
    dtfToCharDoubleToVFormatString,  /**< DTL_TYPE_NATIVE_DOUBLE */
    NULL,
    dtfToCharNumberToVFormatString,  /**< DTL_TYPE_NUMBER */  
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL
};

dtfNumberFormatFuncPtr dtfToCharXFormatFuncList[ DTL_TYPE_MAX ] =
{
    NULL, NULL, NULL, NULL,
    dtfToCharRealToXFormatString,    /**< DTL_TYPE_NATIVE_REAL */
    dtfToCharDoubleToXFormatString,  /**< DTL_TYPE_NATIVE_DOUBLE */
    NULL,
    dtfToCharNumberToXFormatString,  /**< DTL_TYPE_NUMBER */  
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL
};

/**
 * @brief 입력받은 format string으로 해당되는 keyword 정보를 얻는다.
 * @param[in]  aFormatString
 * @return format string과 일치하는 dtlDateTimeFormatKeyWord 반환
 */

const dtlDateTimeFormatKeyWord * dtfGetDateTimeFormatKeyWord( stlChar  * aFormatString )
{
    stlInt32                           sKeyIdx;
    const dtlDateTimeFormatKeyWord   * sKeyWord = NULL;
    stlBool                            sIsGetDateTimeKeyWord = STL_FALSE;

    sKeyIdx = DTF_DATETIME_FORMAT_INDEX[(stlUInt8)*aFormatString];
        
    if( sKeyIdx > -1 )
    {
        sKeyWord = dtfDateTimeFormattingKeyword + sKeyIdx;

        do
        {
            if( stlStrncasecmp( aFormatString,
                                sKeyWord->mName,
                                sKeyWord->mLen ) == 0 )
            {
                /**
                 * @todo MON Mon  MONTH Month 구분
                 */
                if( ( sKeyWord->mId == DTF_DATETIME_FMT_MON )   ||
                    ( sKeyWord->mId == DTF_DATETIME_FMT_MONTH ) ||
                    ( sKeyWord->mId == DTF_DATETIME_FMT_RM )    ||
                    ( sKeyWord->mId == DTF_DATETIME_FMT_DAY )   ||
                    ( sKeyWord->mId == DTF_DATETIME_FMT_AM )    ||
                    ( sKeyWord->mId == DTF_DATETIME_FMT_PM )    ||
                    ( sKeyWord->mId == DTF_DATETIME_FMT_DY ) )
                {
                    if( *(sKeyWord->mName + 1) == *(aFormatString + 1) )
                    {
                        sIsGetDateTimeKeyWord = STL_TRUE;
                        break;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    sIsGetDateTimeKeyWord = STL_TRUE;
                    break;
                }
            }

            sKeyWord++;

            if( sKeyWord->mName == NULL )
            {
                sKeyWord = NULL;
                break;
            }
            
        } while( *aFormatString == *(sKeyWord->mName) );
    }
    else
    {
        sKeyWord = NULL;
    }

    if( sIsGetDateTimeKeyWord == STL_FALSE )
    {
        sKeyWord = NULL;
    }
    else
    {
        /* Do Nothing */
    }

    return sKeyWord;
}


/**
 * @brief datetime 정보를 format string에 맞게 string으로 변환한다.
 * @param[in]  aFormatInfo         format string에 대한 분석 정보
 * @param[in]  aDtlExpTime         datetime 정보
 * @param[in]  aFractionalSecond   fractional second
 * @param[in]  aTimeZone           time zone
 * @param[in]  aAvailableSize      aAvailableSize 
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfDateTimeToChar( dtlDateTimeFormatInfo * aFormatInfo,
                             dtlExpTime            * aDtlExpTime,
                             dtlFractionalSecond     aFractionalSecond,
                             stlInt32              * aTimeZone,
                             stlInt64                aAvailableSize,
                             stlChar               * aResult,
                             stlInt64              * aLength,
                             dtlFuncVector         * aVectorFunc,
                             void                  * aVectorArg,
                             stlErrorStack         * aErrorStack )
{
    dtlDateTimeFormatNode  * sFormatNode;
    stlChar                * sFormatStr  = aFormatInfo->mFormatStrInfo->mStr;
    stlChar                * sResult     = aResult;
    stlChar                * sDigitStr = NULL;
    stlInt64                 sSize       = 0;
    stlInt32                 sUintFormatStrSize = 0;
    stlInt32                 sValue      = 0;
    stlInt32                 sFirstDigit = 0;
    stlInt32                 sJulianDate = 0;
    stlSize                  sAlignSize  = STL_ALIGN_DEFAULT( STL_SIZEOF(dtlDateTimeFormatNode) );

#define DTF_TWO_DIGIT_NUMBER_TO_STRING( aNumber, aString )  \
    {                                                       \
        STL_DASSERT( (aNumber >= 0) && (aNumber < 100) );   \
        aString[0] =  gDtfTwoDigitString[aNumber][0];       \
        aString[1] =  gDtfTwoDigitString[aNumber][1];       \
    }                                                                    

#define DTF_FOUR_DIGIT_NUMBER_TO_STRING( aNumber, aString )     \
    {                                                           \
        stlUInt32 sUValue = (aNumber);                          \
        stlUInt32 sUNewValue = 0;                               \
                                                                \
        STL_DASSERT( ((aNumber) >= 1000) && ((aNumber) < 10000) );      \
                                                                \
        sUNewValue = sUValue / 10;                              \
        (aString)[3] = ( sUValue - (sUNewValue * 10) ) + '0';   \
        sUValue = sUNewValue;                                   \
                                                                \
        sUNewValue = sUValue / 10;                              \
        (aString)[2] = ( sUValue - (sUNewValue * 10) ) + '0';   \
        sUValue = sUNewValue;                                   \
                                                                \
        sUNewValue = sUValue / 10;                              \
        (aString)[1] = ( sUValue - (sUNewValue * 10) ) + '0';   \
        sUValue = sUNewValue;                                   \
                                                                \
        sUNewValue = sUValue / 10;                              \
        (aString)[0] = ( sUValue - (sUNewValue * 10) ) + '0';   \
    }                                                                    
    
    
    STL_TRY_THROW( aFormatInfo->mToCharMaxResultLen <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER );
    
    for( sFormatNode = aFormatInfo->mFormatStrInfo->mFormat;
         sFormatNode->mType != DTF_FORMAT_NODE_TYPE_END;
         sFormatNode = (dtlDateTimeFormatNode*)((stlChar*)sFormatNode + sAlignSize) )
    {
        if( sFormatNode->mType == DTF_FORMAT_NODE_TYPE_CHAR )
        {
            /* STL_TRY_THROW( (sSize + sFormatNode->mPosLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
            
            stlMemcpy( sResult + sSize,
                       sFormatStr + sFormatNode->mPos,
                       sFormatNode->mPosLen );
            
            sSize += sFormatNode->mPosLen;
            
            continue;
        }

        switch( sFormatNode->mKey->mId )
        {
            case DTF_DATETIME_FMT_CC:
                {
                    sValue = DTF_DATETIME_FMT_GET_YEAR(aDtlExpTime->mYear, STL_FALSE);
                    sValue = (sValue - 1) / 100 + 1;

                    if( (sValue >= 1) && (sValue <= 99) )
                    {
                        /* STL_TRY_THROW( (sSize + 2) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                        
                        STL_TRY( dtdIntegerToLeadingZeroFormatString(
                                     sValue,
                                     2,
                                     & sUintFormatStrSize,
                                     sResult + sSize,
                                     aErrorStack )
                                 == STL_SUCCESS );
                        sSize += sUintFormatStrSize;
                    }
                    else
                    {
                        /* STL_TRY_THROW( (sSize + 3) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                        
                        STL_TRY( dtdIntegerToLeadingZeroFormatString(
                                     sValue,
                                     3,
                                     & sUintFormatStrSize,
                                     sResult + sSize,
                                     aErrorStack )
                                 == STL_SUCCESS );
                        sSize += sUintFormatStrSize;
                    }
                }
                break;
            case DTF_DATETIME_FMT_MONTH:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_MONTH_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen) */
                    /*                <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_MONTH_UPPER_STR[aDtlExpTime->mMonth -1].mStr,
                               DTF_DATETIME_FMT_MONTH_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_MONTH_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_Month:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_MONTH_FIRST_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen) */
                    /*                <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */

                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_MONTH_FIRST_UPPER_STR[aDtlExpTime->mMonth -1].mStr,
                               DTF_DATETIME_FMT_MONTH_FIRST_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_MONTH_FIRST_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_month:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_MONTH_LOWER_STR[aDtlExpTime->mMonth -1].mMaxLen) */
                    /*                <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_MONTH_LOWER_STR[aDtlExpTime->mMonth -1].mStr,
                               DTF_DATETIME_FMT_MONTH_LOWER_STR[aDtlExpTime->mMonth -1].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_MONTH_LOWER_STR[aDtlExpTime->mMonth -1].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_MON:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_MON_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen) */
                    /*                <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_MON_UPPER_STR[aDtlExpTime->mMonth -1].mStr,
                               DTF_DATETIME_FMT_MON_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_MON_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_Mon:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_MON_FIRST_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen) */
                    /*                <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_MON_FIRST_UPPER_STR[aDtlExpTime->mMonth -1].mStr,
                               DTF_DATETIME_FMT_MON_FIRST_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_MON_FIRST_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_mon:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_MON_LOWER_STR[aDtlExpTime->mMonth -1].mMaxLen) */
                    /*                <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_MON_LOWER_STR[aDtlExpTime->mMonth -1].mStr,
                               DTF_DATETIME_FMT_MON_LOWER_STR[aDtlExpTime->mMonth -1].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_MON_LOWER_STR[aDtlExpTime->mMonth -1].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_DDD:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 3 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sJulianDate = dtdDate2JulianDate( aDtlExpTime->mYear,
                                                      aDtlExpTime->mMonth,
                                                      aDtlExpTime->mDay );
                    
                    sValue = dtdDate2JulianDate( aDtlExpTime->mYear,
                                                 1,
                                                 1 );
                    
                    aDtlExpTime->mYearDay = sJulianDate - sValue + 1;

                    STL_DASSERT( (aDtlExpTime->mYearDay >= DTF_FIRST_DAY_OF_YEAR) &&
                                 (aDtlExpTime->mYearDay <=
                                  ((DTL_IS_LEAP(aDtlExpTime->mYear) == STL_FALSE) ?
                                   DTF_LAST_DAYS_OF_YEAR : DTF_LAST_DAYS_OF_LEAP_YEAR)) );

                    STL_TRY( dtdIntegerToLeadingZeroFormatString( aDtlExpTime->mYearDay,
                                                                  sFormatNode->mKey->mValueMaxLen,
                                                                  & sUintFormatStrSize,
                                                                  sResult + sSize,
                                                                  aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_DD:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 2 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sDigitStr = sResult + sSize;
                    DTF_TWO_DIGIT_NUMBER_TO_STRING( aDtlExpTime->mDay, sDigitStr );
                    
                    sSize += 2;
                }
                break;
            case DTF_DATETIME_FMT_D:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 1 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sJulianDate = dtdDate2JulianDate( aDtlExpTime->mYear,
                                                      aDtlExpTime->mMonth,
                                                      aDtlExpTime->mDay );
                    
                    aDtlExpTime->mWeekDay = ( sJulianDate + 1 ) % 7;

                    *(sResult + sSize) = (aDtlExpTime->mWeekDay + 1) + '0';
                    sSize += sFormatNode->mKey->mValueMaxLen;                    
                }
                break;
            case DTF_DATETIME_FMT_MM:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 2 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sDigitStr = sResult + sSize;
                    DTF_TWO_DIGIT_NUMBER_TO_STRING( aDtlExpTime->mMonth, sDigitStr );
                    
                    sSize += 2;
                }
                break;
            case DTF_DATETIME_FMT_SYYYY:
                {
                    if( aDtlExpTime->mYear > 0 )
                    {
                        /* Do Nothing */
                    }
                    else
                    {
                        /* STL_TRY_THROW( (sSize + 1) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                        
                        *(sResult + sSize) = '-';
                        sSize++;
                    }

                    /* STL_TRY_THROW( (sSize + 4) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */

                    STL_TRY( dtdIntegerToLeadingZeroFormatString(
                                 DTF_DATETIME_FMT_GET_YEAR(aDtlExpTime->mYear, STL_FALSE),
                                 4,
                                 & sUintFormatStrSize,
                                 sResult + sSize,
                                 aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_Y_YYY:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 5 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = DTF_DATETIME_FMT_GET_YEAR(aDtlExpTime->mYear, STL_FALSE);
                    sFirstDigit = sValue / 1000;

                    *(sResult + sSize) = sFirstDigit + '0';
                    sSize += 1;

                    *(sResult + sSize) = ',';
                    sSize += 1;                    
                        
                    STL_TRY( dtdIntegerToLeadingZeroFormatString(
                                 (sValue - sFirstDigit * 1000 ),
                                 3,
                                 & sUintFormatStrSize,
                                 sResult + sSize,
                                 aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_RRRR:                
            case DTF_DATETIME_FMT_YYYY:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 4 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */

                    sValue = DTF_DATETIME_FMT_GET_YEAR(aDtlExpTime->mYear, STL_FALSE);
                    sDigitStr = sResult + sSize;

                    if ( (sValue >= 1000) && (sValue < 10000))
                    {
                        DTF_FOUR_DIGIT_NUMBER_TO_STRING( sValue, sDigitStr );
                        sSize += 4;
                    }
                    else
                    {
                        STL_TRY( dtdIntegerToLeadingZeroFormatString(
                                     sValue,
                                     sFormatNode->mKey->mValueMaxLen,
                                     & sUintFormatStrSize,
                                     sResult + sSize,
                                     aErrorStack )
                                 == STL_SUCCESS );
                        sSize += sUintFormatStrSize;
                    }
                }
                break;
            case DTF_DATETIME_FMT_YYY:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 3 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = DTF_DATETIME_FMT_GET_YEAR(aDtlExpTime->mYear, STL_FALSE);
                    sValue -= (sValue / 1000) * 1000;

                    STL_TRY( dtdIntegerToLeadingZeroFormatString(
                                 sValue,
                                 sFormatNode->mKey->mValueMaxLen,
                                 & sUintFormatStrSize,
                                 sResult + sSize,
                                 aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_RR:
            case DTF_DATETIME_FMT_YY:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 2 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = DTF_DATETIME_FMT_GET_YEAR(aDtlExpTime->mYear, STL_FALSE);
                    sValue -= (sValue / 100) * 100;

                    STL_TRY( dtdIntegerToLeadingZeroFormatString(
                                 sValue,
                                 sFormatNode->mKey->mValueMaxLen,
                                 & sUintFormatStrSize,
                                 sResult + sSize,
                                 aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_Y:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 1 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = DTF_DATETIME_FMT_GET_YEAR(aDtlExpTime->mYear, STL_FALSE);
                    sValue -= (sValue / 10) * 10;

                    *(sResult + sSize) = sValue + '0';
                    sSize += sFormatNode->mKey->mValueMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_A_D:
            case DTF_DATETIME_FMT_B_C:
                {
                    /* sFormatNode->mKey->mValueMaxLen = DTF_DATETIME_FMT_A_D_STR_LEN or DTF_DATETIME_FMT_B_C_STR_LEN */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    if( aDtlExpTime->mYear > 0 )
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_A_D_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }
                    else                        
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_B_C_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }

                    sSize += sFormatNode->mKey->mValueMaxLen;                    
                }
                break;
            case DTF_DATETIME_FMT_a_d:
            case DTF_DATETIME_FMT_b_c:
                {
                    /* sFormatNode->mKey->mValueMaxLen = DTF_DATETIME_FMT_A_D_STR_LEN or DTF_DATETIME_FMT_B_C_STR_LEN */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    if( aDtlExpTime->mYear > 0 )
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_a_d_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }
                    else                        
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_b_c_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }
                    
                    sSize += sFormatNode->mKey->mValueMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_AD:
            case DTF_DATETIME_FMT_BC:
                {
                    /* sFormatNode->mKey->mValueMaxLen = DTF_DATETIME_FMT_AD_STR_LEN or DTF_DATETIME_FMT_BC_STR_LEN */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    if( aDtlExpTime->mYear > 0 )
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_AD_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }
                    else                        
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_BC_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }

                    sSize += sFormatNode->mKey->mValueMaxLen ;                    
                }
                break;
            case DTF_DATETIME_FMT_ad:
            case DTF_DATETIME_FMT_bc:
                {
                    /* sFormatNode->mKey->mValueMaxLen = DTF_DATETIME_FMT_AD_STR_LEN or DTF_DATETIME_FMT_BC_STR_LEN */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    if( aDtlExpTime->mYear > 0 )
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_ad_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }
                    else                        
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_bc_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }

                    sSize += sFormatNode->mKey->mValueMaxLen;                    
                }
                break;
            case DTF_DATETIME_FMT_RM:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_RM_MONTH_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen) <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_RM_MONTH_UPPER_STR[aDtlExpTime->mMonth -1].mStr,
                               DTF_DATETIME_FMT_RM_MONTH_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_RM_MONTH_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_Rm:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_RM_MONTH_FIRST_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen) <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_RM_MONTH_FIRST_UPPER_STR[aDtlExpTime->mMonth -1].mStr,
                               DTF_DATETIME_FMT_RM_MONTH_FIRST_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_RM_MONTH_FIRST_UPPER_STR[aDtlExpTime->mMonth -1].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_rm:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_RM_MONTH_LOWER_STR[aDtlExpTime->mMonth -1].mMaxLen) <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_RM_MONTH_LOWER_STR[aDtlExpTime->mMonth -1].mStr,
                               DTF_DATETIME_FMT_RM_MONTH_LOWER_STR[aDtlExpTime->mMonth -1].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_RM_MONTH_LOWER_STR[aDtlExpTime->mMonth -1].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_DAY:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_DAY_UPPER_STR[aDtlExpTime->mWeekDay].mMaxLen) <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sJulianDate = dtdDate2JulianDate( aDtlExpTime->mYear,
                                                      aDtlExpTime->mMonth,
                                                      aDtlExpTime->mDay );
                    
                    aDtlExpTime->mWeekDay = ( sJulianDate + 1 ) % 7;

                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_DAY_UPPER_STR[aDtlExpTime->mWeekDay].mStr,
                               DTF_DATETIME_FMT_DAY_UPPER_STR[aDtlExpTime->mWeekDay].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_DAY_UPPER_STR[aDtlExpTime->mWeekDay].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_Day:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_DAY_FIRST_UPPER_STR[aDtlExpTime->mWeekDay].mMaxLen) <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sJulianDate = dtdDate2JulianDate( aDtlExpTime->mYear,
                                                      aDtlExpTime->mMonth,
                                                      aDtlExpTime->mDay );
                    
                    aDtlExpTime->mWeekDay = ( sJulianDate + 1 ) % 7;

                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_DAY_FIRST_UPPER_STR[aDtlExpTime->mWeekDay].mStr,
                               DTF_DATETIME_FMT_DAY_FIRST_UPPER_STR[aDtlExpTime->mWeekDay].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_DAY_FIRST_UPPER_STR[aDtlExpTime->mWeekDay].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_day:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_DAY_LOWER_STR[aDtlExpTime->mWeekDay].mMaxLen) <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sJulianDate = dtdDate2JulianDate( aDtlExpTime->mYear,
                                                      aDtlExpTime->mMonth,
                                                      aDtlExpTime->mDay );
                    
                    aDtlExpTime->mWeekDay = ( sJulianDate + 1 ) % 7;

                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_DAY_LOWER_STR[aDtlExpTime->mWeekDay].mStr,
                               DTF_DATETIME_FMT_DAY_LOWER_STR[aDtlExpTime->mWeekDay].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_DAY_LOWER_STR[aDtlExpTime->mWeekDay].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_DY:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_DY_UPPER_STR[aDtlExpTime->mWeekDay].mMaxLen) <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */

                    
                    sJulianDate = dtdDate2JulianDate( aDtlExpTime->mYear,
                                                      aDtlExpTime->mMonth,
                                                      aDtlExpTime->mDay );
                    
                    aDtlExpTime->mWeekDay = ( sJulianDate + 1 ) % 7;

                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_DY_UPPER_STR[aDtlExpTime->mWeekDay].mStr,
                               DTF_DATETIME_FMT_DY_UPPER_STR[aDtlExpTime->mWeekDay].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_DY_UPPER_STR[aDtlExpTime->mWeekDay].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_Dy:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_DY_FIRST_UPPER_STR[aDtlExpTime->mWeekDay].mMaxLen) <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sJulianDate = dtdDate2JulianDate( aDtlExpTime->mYear,
                                                      aDtlExpTime->mMonth,
                                                      aDtlExpTime->mDay );
                    
                    aDtlExpTime->mWeekDay = ( sJulianDate + 1 ) % 7;

                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_DY_FIRST_UPPER_STR[aDtlExpTime->mWeekDay].mStr,
                               DTF_DATETIME_FMT_DY_FIRST_UPPER_STR[aDtlExpTime->mWeekDay].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_DY_FIRST_UPPER_STR[aDtlExpTime->mWeekDay].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_dy:
                {
                    /* STL_TRY_THROW( (sSize + DTF_DATETIME_FMT_DY_LOWER_STR[aDtlExpTime->mWeekDay].mMaxLen) <= aAvailableSize, */
                    /*                ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sJulianDate = dtdDate2JulianDate( aDtlExpTime->mYear,
                                                      aDtlExpTime->mMonth,
                                                      aDtlExpTime->mDay );
                    
                    aDtlExpTime->mWeekDay = ( sJulianDate + 1 ) % 7;

                    stlMemcpy( sResult + sSize,
                               DTF_DATETIME_FMT_DY_LOWER_STR[aDtlExpTime->mWeekDay].mStr,
                               DTF_DATETIME_FMT_DY_LOWER_STR[aDtlExpTime->mWeekDay].mMaxLen );
                    
                    sSize += DTF_DATETIME_FMT_DY_LOWER_STR[aDtlExpTime->mWeekDay].mMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_HH:
            case DTF_DATETIME_FMT_HH12:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 2 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = aDtlExpTime->mHour % ( DTL_HOURS_PER_DAY / 2 );                    
                    sValue = ( sValue == 0 ) ? 12 : sValue;

                    sDigitStr = sResult + sSize;
                    DTF_TWO_DIGIT_NUMBER_TO_STRING( sValue, sDigitStr );
                    
                    sSize += 2;
                }
                break;
            case DTF_DATETIME_FMT_HH24:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 2 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sDigitStr = sResult + sSize;
                    DTF_TWO_DIGIT_NUMBER_TO_STRING( aDtlExpTime->mHour, sDigitStr );
                    
                    sSize += 2;
                }
                break;
            case DTF_DATETIME_FMT_MI:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 2 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sDigitStr = sResult + sSize;
                    DTF_TWO_DIGIT_NUMBER_TO_STRING( aDtlExpTime->mMinute, sDigitStr );
                    
                    sSize += 2;
                }
                break;
            case DTF_DATETIME_FMT_SS:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 2 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sDigitStr = sResult + sSize;
                    DTF_TWO_DIGIT_NUMBER_TO_STRING( aDtlExpTime->mSecond, sDigitStr );
                    
                    sSize += 2;
                }
                break;
            case DTF_DATETIME_FMT_FF:
            case DTF_DATETIME_FMT_FF6:                
                {
                    /* sFormatNode->mKey->mValueMaxLen = 6 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    STL_TRY( dtdIntegerToLeadingZeroFormatString( aFractionalSecond,
                                                                  sFormatNode->mKey->mValueMaxLen,
                                                                  & sUintFormatStrSize,
                                                                  sResult + sSize,
                                                                  aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_FF1:                
                {
                    /* sFormatNode->mKey->mValueMaxLen = 1 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = aFractionalSecond / 100000;
                    *(sResult + sSize) = sValue + '0';
                    sSize += sFormatNode->mKey->mValueMaxLen;                    
                }
                break;
            case DTF_DATETIME_FMT_FF2:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 2 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = aFractionalSecond / 10000;
                    STL_TRY( dtdIntegerToLeadingZeroFormatString( sValue,
                                                                  sFormatNode->mKey->mValueMaxLen,
                                                                  & sUintFormatStrSize,
                                                                  sResult + sSize,
                                                                  aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_FF3:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 3 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = aFractionalSecond / 1000;                    
                    STL_TRY( dtdIntegerToLeadingZeroFormatString( sValue,
                                                                  sFormatNode->mKey->mValueMaxLen,
                                                                  & sUintFormatStrSize,
                                                                  sResult + sSize,
                                                                  aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_FF4:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 4 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = aFractionalSecond / 100;
                    STL_TRY( dtdIntegerToLeadingZeroFormatString( sValue,
                                                                  sFormatNode->mKey->mValueMaxLen,
                                                                  & sUintFormatStrSize,
                                                                  sResult + sSize,
                                                                  aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_FF5:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 5 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = aFractionalSecond / 10;
                    STL_TRY( dtdIntegerToLeadingZeroFormatString( sValue,
                                                                  sFormatNode->mKey->mValueMaxLen,
                                                                  & sUintFormatStrSize,
                                                                  sResult + sSize,
                                                                  aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_TZH:
                {
                    /* STL_TRY_THROW( (sSize + 3) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    STL_DASSERT( aTimeZone != NULL );

                    sValue = ( *aTimeZone < 0 ) ? -(*aTimeZone) : *aTimeZone;
                    sValue = sValue / DTL_SECS_PER_HOUR;

                    if( *aTimeZone <= 0 )
                    {
                        *(sResult + sSize) = '+';
                        sSize += 1;                    
                    }
                    else
                    {
                        *(sResult + sSize) = '-';
                        sSize += 1;                    
                    }
                    
                    STL_TRY( dtdIntegerToLeadingZeroFormatString( sValue,
                                                                  2,
                                                                  & sUintFormatStrSize,
                                                                  sResult + sSize,
                                                                  aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_TZM:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 2 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    STL_DASSERT( aTimeZone != NULL );                    
                    
                    sValue = ( *aTimeZone < 0 ) ? -(*aTimeZone) : *aTimeZone;
                    sValue -= (sValue / DTL_SECS_PER_HOUR) * DTL_SECS_PER_HOUR;
                    sValue = sValue / DTL_SECS_PER_MINUTE;
                    
                    STL_TRY( dtdIntegerToLeadingZeroFormatString( sValue,
                                                                  sFormatNode->mKey->mValueMaxLen,
                                                                  & sUintFormatStrSize,
                                                                  sResult + sSize,
                                                                  aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_SSSSS:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 5 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = ( aDtlExpTime->mHour * DTL_SECS_PER_HOUR ) +
                        ( aDtlExpTime->mMinute * DTL_SECS_PER_MINUTE ) +
                        aDtlExpTime->mSecond;

                    STL_DASSERT( sValue < DTL_SECS_PER_DAY );

                    STL_TRY( dtdIntegerToLeadingZeroFormatString( sValue,
                                                                  sFormatNode->mKey->mValueMaxLen,
                                                                  & sUintFormatStrSize,
                                                                  sResult + sSize,
                                                                  aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_A_M:
            case DTF_DATETIME_FMT_P_M:
                {
                    /* sFormatNode->mKey->mValueMaxLen = DTF_DATETIME_FMT_P_M_STR_LEN  or  DTF_DATETIME_FMT_A_M_STR_LEN */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = aDtlExpTime->mHour % DTL_HOURS_PER_DAY;
                    
                    if( sValue >= (DTL_HOURS_PER_DAY / 2) )
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_P_M_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }
                    else
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_A_M_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }
                    
                    sSize += sFormatNode->mKey->mValueMaxLen;                    
                }
                break;
            case DTF_DATETIME_FMT_AM:
            case DTF_DATETIME_FMT_PM:
                {
                    /* sFormatNode->mKey->mValueMaxLen = DTF_DATETIME_FMT_PM_STR_LEN  or  DTF_DATETIME_FMT_AM_STR_LEN */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = aDtlExpTime->mHour % DTL_HOURS_PER_DAY;
                    
                    if( sValue >= (DTL_HOURS_PER_DAY / 2) )
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_PM_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }
                    else
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_AM_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }

                    sSize += sFormatNode->mKey->mValueMaxLen;                    
                }
                break;
            case DTF_DATETIME_FMT_Am:
            case DTF_DATETIME_FMT_Pm:
                {
                    /* sFormatNode->mKey->mValueMaxLen = DTF_DATETIME_FMT_PM_STR_LEN  or  DTF_DATETIME_FMT_AM_STR_LEN */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = aDtlExpTime->mHour % DTL_HOURS_PER_DAY;
                    
                    if( sValue >= (DTL_HOURS_PER_DAY / 2) )
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_Pm_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }
                    else
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_Am_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }

                    sSize += sFormatNode->mKey->mValueMaxLen;                    
                }
                break;                
            case DTF_DATETIME_FMT_a_m:
            case DTF_DATETIME_FMT_p_m:
                {
                    /* sFormatNode->mKey->mValueMaxLen = DTF_DATETIME_FMT_P_M_STR_LEN  or  DTF_DATETIME_FMT_A_M_STR_LEN */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = aDtlExpTime->mHour % DTL_HOURS_PER_DAY;
                    
                    if( sValue >= (DTL_HOURS_PER_DAY / 2) )
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_p_m_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }
                    else
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_a_m_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }
                    
                    sSize += sFormatNode->mKey->mValueMaxLen;                    
                }
                break;
            case DTF_DATETIME_FMT_am:
            case DTF_DATETIME_FMT_pm:
                {
                    /* sFormatNode->mKey->mValueMaxLen = DTF_DATETIME_FMT_PM_STR_LEN  or  DTF_DATETIME_FMT_AM_STR_LEN */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = aDtlExpTime->mHour % DTL_HOURS_PER_DAY;
                    
                    if( sValue >= (DTL_HOURS_PER_DAY / 2) )
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_pm_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }
                    else
                    {
                        stlMemcpy( sResult + sSize,
                                   DTF_DATETIME_FMT_am_STR,
                                   sFormatNode->mKey->mValueMaxLen );
                    }

                    sSize += sFormatNode->mKey->mValueMaxLen;                    
                }
                break;
            case DTF_DATETIME_FMT_WW:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 2 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sJulianDate = dtdDate2JulianDate( aDtlExpTime->mYear,
                                                      aDtlExpTime->mMonth,
                                                      aDtlExpTime->mDay );
                    
                    sValue = dtdDate2JulianDate( aDtlExpTime->mYear,
                                                 1,
                                                 1 );
                    
                    aDtlExpTime->mYearDay = sJulianDate - sValue + 1;

                    sValue = ( aDtlExpTime->mYearDay - 1 ) / 7 + 1;

                    STL_DASSERT( (sValue >= 1) && (sValue <= 53) );

                    STL_TRY( dtdIntegerToLeadingZeroFormatString( sValue,
                                                                  sFormatNode->mKey->mValueMaxLen,
                                                                  & sUintFormatStrSize,
                                                                  sResult + sSize,
                                                                  aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_W:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 1 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = (aDtlExpTime->mDay - 1) / 7 + 1;

                    STL_DASSERT( (sValue >= 1) && (sValue <= 5) );
                    
                    *(sResult + sSize) = sValue + '0';
                    sSize += sFormatNode->mKey->mValueMaxLen;
                }
                break;
            case DTF_DATETIME_FMT_J:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 7 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sJulianDate = dtdDate2JulianDate( aDtlExpTime->mYear,
                                                      aDtlExpTime->mMonth,
                                                      aDtlExpTime->mDay );

                    STL_DASSERT( (sValue >= DTL_JULIAN_MINJULDAY) && (sValue <= DTL_JULIAN_MAXJULDAY) );

                    STL_TRY( dtdIntegerToLeadingZeroFormatString( sJulianDate,
                                                                  sFormatNode->mKey->mValueMaxLen,
                                                                  & sUintFormatStrSize,
                                                                  sResult + sSize,
                                                                  aErrorStack )
                             == STL_SUCCESS );
                    sSize += sUintFormatStrSize;
                }
                break;
            case DTF_DATETIME_FMT_Q:
                {
                    /* sFormatNode->mKey->mValueMaxLen = 1 */
                    /* STL_TRY_THROW( (sSize + sFormatNode->mKey->mValueMaxLen) <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER ); */
                    
                    sValue = (aDtlExpTime->mMonth - 1) / 3 + 1;
                    
                    STL_DASSERT( (sValue >= 1) && (sValue <= 4) );

                    *(sResult + sSize) = sValue + '0';
                    sSize += sFormatNode->mKey->mValueMaxLen;
                }
                break;
            default:
                /* Do Nothing */
                break;
        }
    }

    *aLength = sSize;

    STL_DASSERT( sSize <= aFormatInfo->mToCharMaxResultLen );

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_DATE_BYTE_LENGTH_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief 입력받은 string을 format string에 맞게 datetime으로 변환한다.
 * @param[in]  aDateTimeType           datetime type
 * @param[in]  aDateTimeString         datetime으로 변경할 string
 * @param[in]  aDateTimeStringLength   datetime으로 변경할 string length
 * @param[in]  aToDateTimeFormatInfo   format string에 대한 분석 정보
 * @param[out] aDtlExpTime             dtlExpTime
 * @param[out] aFractionalSecond       fractional second
 * @param[out] aTimeZone               time zone
 * @param[in]  aVectorFunc             aVectorArg
 * @param[in]  aVectorArg              aVectorArg
 * @param[in]  aErrorStack             에러 스택
 */
stlStatus dtfToDateTimeFromString( dtlDataType                aDateTimeType,
                                   stlChar                  * aDateTimeString,
                                   stlInt64                   aDateTimeStringLength,
                                   dtlDateTimeFormatInfo    * aToDateTimeFormatInfo,
                                   dtlExpTime               * aDtlExpTime,
                                   dtlFractionalSecond      * aFractionalSecond,
                                   stlInt32                 * aTimeZone,
                                   dtlFuncVector            * aVectorFunc,
                                   void                     * aVectorArg,
                                   stlErrorStack            * aErrorStack )
{
    stlChar   * sDateTimeStringPtr = aDateTimeString;
    stlChar   * sDateTimeStringPtrEnd = aDateTimeString + aDateTimeStringLength;

    dtlDateTimeFormatNode     * sFormatNode;        
    dtlDateTimeFormatStrInfo  * sFormatStrInfo;
    dtlToDateTimeFormatElementSetInfo * sFormatElementSetInfo;

    stlSize             sAlignSize  = STL_ALIGN_DEFAULT( STL_SIZEOF(dtlDateTimeFormatNode) );

    dtlDataValue        sDateDataValue;
    dtlDateType         sDateTypeValue;
    dtlFractionalSecond sFractionalSecond = 0;

    stlInt8             i                    = 0;
    stlInt8             sRemainStrLen        = 0;
    stlInt32            sIntValue            = 0;
    stlInt32            sJulianDate          = 0;
    stlInt32            sMonthForFormatDDD   = 0;
    stlInt32            sDayForFormatDDD     = 0;
    stlInt32            sIntValueDigitCnt    = 0;
    stlInt32            sLeadingZeroDigitCnt = 0;
    stlInt32            sElementStrLen       = 0;
    stlInt32            sIntValueForWeekDay  = STL_INT32_MIN;
    stlInt32            sIntValueForYearDay  = STL_INT32_MIN;
    stlInt32            sYY00Year            = 0;    
    stlInt32            sLastTwoDigitYear    = 0;
    stlInt64            sIntValueBeforeComma = 0;
    stlInt32            sIntValueBeforeCommaStrLen = 0;    
    stlInt32            sIntYear             = 0;
    stlInt32            sIntMonth            = 0;
    stlInt32            sIntDay              = 0;
    stlInt32            sIntHour             = 0;
    stlInt32            sIntMinute           = 0;
    stlInt32            sIntSecond           = 0;
    stlInt32            sTimeZone            = 0;
    stlInt32            sTimeZoneHour        = 0;
    stlInt32            sTimeZoneMinute      = 0;
    stlInt32            sCurrentTimeZone     = 0;
    stlInt32            sSSSSS               = 0;
    stlBool             sIsNegative          = STL_FALSE;
    stlBool             sIsNegativeTimeZoneHour = STL_FALSE;    
    stlBool             sExistFormatElementD = STL_FALSE;
    stlBool             sIsBc                = STL_FALSE;
    stlBool             sIsPm                = STL_FALSE;
    stlBool             sNeedCurrentTimeZoneHour = STL_FALSE;
    stlBool             sNeedCurrentTimeZoneMinute = STL_FALSE;
    stlChar           * sTmpPtr              = NULL;

    stlBool             sNotSpecifiedAmPmDateTimeString = STL_FALSE;
    stlBool             sNotSpecifiedSSSSSDateTimeString = STL_FALSE;
    stlBool             sNotSpecifiedCurrentTimeZoneHour = STL_FALSE;
    stlBool             sNotSpecifiedCurrentTimeZoneMinute = STL_FALSE;    

    sFormatStrInfo        = aToDateTimeFormatInfo->mFormatStrInfo;
    sFormatElementSetInfo = &(aToDateTimeFormatInfo->mFormatElementSetInfo);
    
    if( aToDateTimeFormatInfo->mNeedCurrentDate == STL_FALSE )
    {
        /* Do Nothing */
    }
    else
    {
        /*
         * 현재 년 월 일에 대한 기본값이 필요한 경우, 년 월 일에 대한 기본값을 생성한다.
         * 년 월 의 기본값 : 현재 날짜의 년 월
         * 일    의 기본값 : 1
         * 시분초의 기본값 : 0
         *
         * 예) TO_DATE( '1999', 'YYYY' )
         *  => 1999-05-01  ( 현재 날짜가 1999-05 인 경우 )
         */
        
        sDateDataValue.mValue = &sDateTypeValue;
        STL_TRY( dtlInitDataValue( DTL_TYPE_DATE,
                                   DTL_DATE_SIZE,
                                   &sDateDataValue,
                                   aErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY( dtlDateSetValueFromStlTime( stlNow(),
                                             DTL_PRECISION_NA,
                                             DTL_SCALE_NA,
                                             DTL_DATE_SIZE,
                                             &sDateDataValue,
                                             aVectorFunc,
                                             aVectorArg,
                                             aErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY( dtdDateTime2dtlExpTime( sDateTypeValue,
                                         aDtlExpTime,
                                         & sFractionalSecond,
                                         aErrorStack )
                 == STL_SUCCESS );
        
        aDtlExpTime->mDay = DTF_FIRST_DAY_OF_MONTH;

        aDtlExpTime->mHour = 0;
        aDtlExpTime->mMinute = 0;
        aDtlExpTime->mSecond = 0;

        if( sFormatElementSetInfo->mYearDay == STL_TRUE )
        {
            aDtlExpTime->mMonth = DTF_FIRST_MONTH_OF_YEAR;  /* 1 */
            aDtlExpTime->mDay   = DTF_FIRST_DAY_OF_MONTH;   /* 1 */
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    /* field간의 공백 무시 */
    while( sDateTimeStringPtr < sDateTimeStringPtrEnd )
    {
        if( stlIsspace(*sDateTimeStringPtr) == STL_TRUE )
        {
            sDateTimeStringPtr++;
        }
        else
        {
            break;
        }
    }

    for( sFormatNode = sFormatStrInfo->mFormat;
         (sFormatNode->mType != DTF_FORMAT_NODE_TYPE_END) &&
             (sDateTimeStringPtr < sDateTimeStringPtrEnd) ;
         sFormatNode = (dtlDateTimeFormatNode*)((stlChar*)sFormatNode + sAlignSize) )
    {       
        if( sFormatNode->mType == DTF_FORMAT_NODE_TYPE_CHAR )
        {
            if( sFormatNode->mPosLen == 1 )
            {
                if( stlIsspace(*(sFormatStrInfo->mStr + sFormatNode->mPos)) == STL_TRUE )
                {
                    /* Do Nothing */
                }
                else if( ( DTF_DATETIME_FORMAT_SPECIAL_CHAR_INDEX[
                               (stlUInt8)*(sDateTimeStringPtr)] == 0 ) &&
                         ( DTF_DATETIME_FORMAT_SPECIAL_CHAR_INDEX[
                             (stlUInt8)*(sFormatStrInfo->mStr + sFormatNode->mPos)] == 0 ) )
                {
                    sDateTimeStringPtr++;
                }
                else
                {
                    if( *(sDateTimeStringPtr) == *(sFormatStrInfo->mStr + sFormatNode->mPos) )
                    {
                        sDateTimeStringPtr++;
                    }
                    else
                    {
                        STL_THROW( ERROR_NOT_MATCH_FORMAT_STR );
                    }
                }
            }
            else
            {
                if( (sDateTimeStringPtrEnd - sDateTimeStringPtr) >= sFormatNode->mPosLen )
                {
                    if( stlMemcmp( sDateTimeStringPtr,
                                   (sFormatStrInfo->mStr + sFormatNode->mPos),
                                   sFormatNode->mPosLen ) == 0 )
                    {
                        sDateTimeStringPtr += sFormatNode->mPosLen;
                    }
                    else
                    {
                        STL_THROW( ERROR_NOT_MATCH_FORMAT_STR );
                    }
                }
                else
                {
                    STL_THROW( ERROR_NOT_MATCH_FORMAT_STR );
                }
            }
            
            /* field간의 공백 무시 */
            while( sDateTimeStringPtr < sDateTimeStringPtrEnd )
            {
                if( stlIsspace(*sDateTimeStringPtr) == STL_TRUE )
                {
                    sDateTimeStringPtr++;
                }
                else
                {
                    break;
                }
            }
            
            continue;
        }
        else
        {
            // Do Nothing
        }
        
        /* field간의 공백 무시 */
        while( sDateTimeStringPtr < sDateTimeStringPtrEnd )
        {
            if( stlIsspace(*sDateTimeStringPtr) == STL_TRUE )
            {
                sDateTimeStringPtr++;
            }
            else
            {
                break;
            }
        }

        STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd,
                       RAMP_NODE_CHECK );
        
        STL_TRY_THROW( sFormatNode->mKey->mDateMode != DTL_FORMAT_DATE_ISOWEEK,
                       ERROR_INVALID_DATE_FORMAT );

        /**
         * 시간날짜를 구할 수 있는 element들을 먼저 처리한다.
         */
        
        if( sFormatNode->mKey->mIsDigit == STL_TRUE )
        {
            /*
             * string을 int value로 변환한다.
             */
            
            if( dtfToDateTimeGetIntValueFromParseString(
                    sFormatStrInfo,
                    sDateTimeStringPtr,
                    sDateTimeStringPtrEnd,
                    sFormatNode,
                    & sIntValue,
                    & sIntValueDigitCnt,
                    & sLeadingZeroDigitCnt,
                    & sElementStrLen,
                    aVectorFunc,
                    aVectorArg,
                    aErrorStack )
                == STL_SUCCESS )
            {
                /* Do Nothing */
            }
            else
            {
                if( ( aDateTimeType == DTL_TYPE_TIME_WITH_TIME_ZONE ) ||
                    ( aDateTimeType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ) )
                {
                    switch( sFormatNode->mKey->mId )
                    {
                        case DTF_DATETIME_FMT_FF:
                        case DTF_DATETIME_FMT_FF1:
                        case DTF_DATETIME_FMT_FF2:
                        case DTF_DATETIME_FMT_FF3:
                        case DTF_DATETIME_FMT_FF4:
                        case DTF_DATETIME_FMT_FF5:
                        case DTF_DATETIME_FMT_FF6:
                            stlPopError( aErrorStack );
                            continue;
                            break;
                        default:
                            STL_TRY( STL_FALSE );
                            break;
                    }
                }
                else
                {
                    STL_TRY( STL_FALSE );
                }
            }
            
            /*
             * format element에 맞게 처리.
             */
            switch( sFormatNode->mKey->mId )
            {
                case DTF_DATETIME_FMT_YYYY:
                    {
                        STL_TRY_THROW( sIntValue != 0, ERROR_DATETIME_YEAR_VALUE );
                        
                        aDtlExpTime->mYear = sIntValue;                    
                    }
                    break;
                case DTF_DATETIME_FMT_YYY:
                    {
                        aDtlExpTime->mYear =
                            (aDtlExpTime->mYear / DTF_ONE_THOUSAND_YEAR * DTF_ONE_THOUSAND_YEAR)
                            + sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_YY:
                    {
                        aDtlExpTime->mYear =
                            (aDtlExpTime->mYear / DTF_ONE_HUNDRED_YEAR * DTF_ONE_HUNDRED_YEAR)
                            + sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_Y:
                    {
                        aDtlExpTime->mYear =
                            (aDtlExpTime->mYear / DTF_TEN_YEAR * DTF_TEN_YEAR ) + sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_RR:                    
                case DTF_DATETIME_FMT_RRRR:
                    {
                        /***********************************************
                         * Last two digits of the current year | Two digit of input string | result
                         * 00 - 50 | 00 - 49 | First two digits of the current year
                         * 51 - 99 | 00 - 49 | First two digits of the current year + 1
                         * 00 - 50 | 50 - 99 | First two digits of the current year - 1 
                         * 51 - 99 | 50 - 99 | First two digits of the current year
                         ***********************************************/

                        sYY00Year =
                            (aDtlExpTime->mYear / DTF_ONE_HUNDRED_YEAR * DTF_ONE_HUNDRED_YEAR);
                        
                        sLastTwoDigitYear = aDtlExpTime->mYear - sYY00Year;
                        
                        if( sElementStrLen <= DTF_TWO_DIGIT )
                        {
                            if( (sIntValue >= 0) && (sIntValue <= 49) )
                            {
                                if( (sLastTwoDigitYear >= 0) &&
                                    (sLastTwoDigitYear <= 50) )
                                {
                                    aDtlExpTime->mYear = sYY00Year + sIntValue;
                                }
                                else
                                {
                                    /* 51 - 99 */                                
                                    aDtlExpTime->mYear =
                                        sYY00Year + DTF_ONE_HUNDRED_YEAR + sIntValue;
                                }
                            }
                            else // ( (sIntValue >= 50) && (sIntValue <= 99) )
                            {
                                if( (sLastTwoDigitYear >= 0) &&
                                    (sLastTwoDigitYear <= 50) )
                                {
                                    aDtlExpTime->mYear =
                                        sYY00Year - DTF_ONE_HUNDRED_YEAR + sIntValue;
                                }
                                else
                                {
                                    /* 51 - 99 */                                
                                    aDtlExpTime->mYear = sYY00Year + sIntValue;
                                }
                            }
                        }
                        else
                        {
                            aDtlExpTime->mYear = sIntValue;
                        }
                    }
                    break;
                case DTF_DATETIME_FMT_MM:
                    {
                        STL_TRY_THROW( (sIntValue >= 1) && (sIntValue <= DTL_MONTHS_PER_YEAR),
                                       ERROR_MM_FIELD_OVERFLOW );
                        
                        aDtlExpTime->mMonth = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_DD:
                    {
                        STL_TRY_THROW( (sIntValue >= DTF_FIRST_DAY_OF_MONTH) &&
                                       (sIntValue <= DTF_MAX_LAST_DAYS_PER_MONTH),
                                       ERROR_DD_FIELD_OVERFLOW );
                        
                        aDtlExpTime->mDay = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_HH24:
                case DTF_DATETIME_FMT_hh24:
                    {
                        STL_TRY_THROW( (sIntValue >= DTF_HH24_MIN) && (sIntValue <= DTF_HH24_MAX),
                                       ERROR_HH24_FILED_OVERFLOW );

                        aDtlExpTime->mHour = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_HH12:
                case DTF_DATETIME_FMT_hh12:
                case DTF_DATETIME_FMT_HH:
                case DTF_DATETIME_FMT_hh:
                    {
                        STL_TRY_THROW( (sIntValue >= DTF_HH12_MIN) && (sIntValue <= DTF_HH12_MAX),
                                       ERROR_HH12_FILED_OVERFLOW );

                        aDtlExpTime->mHour = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_MI:
                case DTF_DATETIME_FMT_mi:
                    {
                        STL_TRY_THROW( (sIntValue >= DTF_MINUTE_MIN) && (sIntValue <= DTF_MINUTE_MAX),
                                       ERROR_MINUTE_FILED_OVERFLOW );

                        aDtlExpTime->mMinute = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_SS:
                case DTF_DATETIME_FMT_ss:
                    {
                        STL_TRY_THROW( (sIntValue >= DTF_SECOND_MIN) && (sIntValue <= DTF_SECOND_MAX),
                                       ERROR_SECOND_FIELD_OVERFLOW );

                        aDtlExpTime->mSecond = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_FF1:
                    {
                        STL_TRY_THROW( (sIntValue >= DTF_FRACTIONAL_SECOND_MIN) &&
                                       (sIntValue <= DTF_FRACTIONAL_SECOND_FF1_MAX),
                                       ERROR_FRACTIONAL_SECOND_FIELD_OVERFLOW );
                        
                        *aFractionalSecond =
                            sIntValue * dtfFractionalSecondAdjustScale[sIntValueDigitCnt];
                    }
                    break;
                case DTF_DATETIME_FMT_FF2:
                    {
                        STL_TRY_THROW( (sIntValue >= DTF_FRACTIONAL_SECOND_MIN) &&
                                       (sIntValue <= DTF_FRACTIONAL_SECOND_FF2_MAX),
                                       ERROR_FRACTIONAL_SECOND_FIELD_OVERFLOW );

                        *aFractionalSecond =
                            sIntValue * dtfFractionalSecondAdjustScale[sIntValueDigitCnt + sLeadingZeroDigitCnt];
                    }
                    break;
                case DTF_DATETIME_FMT_FF3:
                    {
                        STL_TRY_THROW( (sIntValue >= DTF_FRACTIONAL_SECOND_MIN) &&
                                       (sIntValue <= DTF_FRACTIONAL_SECOND_FF3_MAX),
                                       ERROR_FRACTIONAL_SECOND_FIELD_OVERFLOW );

                        *aFractionalSecond =
                            sIntValue * dtfFractionalSecondAdjustScale[sIntValueDigitCnt + sLeadingZeroDigitCnt];
                    }                    
                    break;
                case DTF_DATETIME_FMT_FF4:
                    {
                        STL_TRY_THROW( (sIntValue >= DTF_FRACTIONAL_SECOND_MIN) &&
                                       (sIntValue <= DTF_FRACTIONAL_SECOND_FF4_MAX),
                                       ERROR_FRACTIONAL_SECOND_FIELD_OVERFLOW );

                        *aFractionalSecond =
                            sIntValue * dtfFractionalSecondAdjustScale[sIntValueDigitCnt + sLeadingZeroDigitCnt];
                    }
                    break;
                case DTF_DATETIME_FMT_FF5:
                    {
                        STL_TRY_THROW( (sIntValue >= DTF_FRACTIONAL_SECOND_MIN) &&
                                       (sIntValue <= DTF_FRACTIONAL_SECOND_FF5_MAX),
                                       ERROR_FRACTIONAL_SECOND_FIELD_OVERFLOW );

                        *aFractionalSecond =
                            sIntValue * dtfFractionalSecondAdjustScale[sIntValueDigitCnt + sLeadingZeroDigitCnt];
                    }
                    break;
                case DTF_DATETIME_FMT_FF6:
                case DTF_DATETIME_FMT_FF:
                    {
                        STL_TRY_THROW( (sIntValue >= DTF_FRACTIONAL_SECOND_MIN) &&
                                       (sIntValue <= DTF_FRACTIONAL_SECOND_FF6_MAX),
                                       ERROR_FRACTIONAL_SECOND_FIELD_OVERFLOW );

                        *aFractionalSecond =
                            sIntValue * dtfFractionalSecondAdjustScale[sIntValueDigitCnt + sLeadingZeroDigitCnt];
                    }
                    break;
                case DTF_DATETIME_FMT_J:
                    {
                        STL_TRY_THROW( (sIntValue >= DTL_JULIAN_MINJULDAY) &&
                                       (sIntValue <= DTL_JULIAN_MAXJULDAY),
                                       ERROR_INVALID_JULIAN_DAY_VALUE );
                        
                        sJulianDate = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_D:
                case DTF_DATETIME_FMT_d:
                    {
                        STL_TRY_THROW( (sIntValue >= 1) && (sIntValue <= 7),
                                       ERROR_INVALIDE_DAY_OF_WEEK );
                        
                        sIntValueForWeekDay  = sIntValue;
                        sExistFormatElementD = STL_TRUE;
                    }
                    break;
                case DTF_DATETIME_FMT_DDD:
                case DTF_DATETIME_FMT_ddd:
                    {
                        sIntValueForYearDay = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_SSSSS:
                    {
                        STL_TRY_THROW( (sIntValue >= 0) && (sIntValue < DTL_SECS_PER_DAY),
                                       ERROR_INVALID_SECONDS_IN_DAY );

                        sSSSSS = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_TZM:
                    {
                        STL_TRY_THROW( (sIntValue >= DTF_MINUTE_MIN) &&
                                       (sIntValue <= DTF_MINUTE_MAX),
                                       ERROR_TIMEZONE_MINUTE_OVERFLOW );
                        
                        sTimeZoneMinute = sIntValue * DTL_SECS_PER_MINUTE;
                    }
                    break;
                default:
                    STL_THROW( ERROR_INVALID_DATE_FORMAT );
                    break;
            }
        }
        else
        {
            switch( sFormatNode->mKey->mId )
            {
                case DTF_DATETIME_FMT_MON:
                case DTF_DATETIME_FMT_Mon:
                case DTF_DATETIME_FMT_mon:
                    {
                        sRemainStrLen = (sDateTimeStringPtrEnd - sDateTimeStringPtr);
                        
                        STL_TRY_THROW( sRemainStrLen >= DTF_DATETIME_FMT_MON_MIN_STR_LEN,
                                       ERROR_MM_FIELD_OVERFLOW );
                        
                        for( i = 0; DTF_DATETIME_FMT_MON_UPPER_STR[i].mStr != NULL; i++ )
                        {
                            if( stlStrncasecmp(
                                    sDateTimeStringPtr,
                                    DTF_DATETIME_FMT_MON_UPPER_STR[i].mStr,
                                    DTF_DATETIME_FMT_MON_UPPER_STR[i].mStrLen )
                                == 0 )
                            {
                                sIntValue = DTF_DATETIME_FMT_MON_UPPER_STR[i].mDateInfo;
                                sElementStrLen = DTF_DATETIME_FMT_MON_UPPER_STR[i].mStrLen;

                                if( sRemainStrLen >= DTF_DATETIME_FMT_MONTH_UPPER_STR[i].mStrLen )
                                {
                                    if( stlStrncasecmp(
                                            sDateTimeStringPtr,
                                            DTF_DATETIME_FMT_MONTH_UPPER_STR[i].mStr,
                                            DTF_DATETIME_FMT_MONTH_UPPER_STR[i].mStrLen )
                                        == 0 )
                                    {
                                        sElementStrLen = DTF_DATETIME_FMT_MONTH_UPPER_STR[i].mStrLen;
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
                                
                                break;                                
                            }
                            else
                            {
                                /* Do Nothing */
                            }
                        }
                        
                        STL_TRY_THROW( DTF_DATETIME_FMT_MON_UPPER_STR[i].mStr != NULL,
                                       ERROR_MM_FIELD_OVERFLOW );

                        aDtlExpTime->mMonth = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_MONTH:
                case DTF_DATETIME_FMT_Month:
                case DTF_DATETIME_FMT_month:
                    {
                        sRemainStrLen = (sDateTimeStringPtrEnd - sDateTimeStringPtr);
                        
                        for( i = 0; DTF_DATETIME_FMT_MONTH_UPPER_STR[i].mStr != NULL; i++ )
                        {
                            if( sRemainStrLen >= DTF_DATETIME_FMT_MONTH_UPPER_STR[i].mStrLen )
                            {
                                if( stlStrncasecmp(
                                        sDateTimeStringPtr,
                                        DTF_DATETIME_FMT_MONTH_UPPER_STR[i].mStr,
                                        DTF_DATETIME_FMT_MONTH_UPPER_STR[i].mStrLen )
                                    == 0 )
                                {
                                    sIntValue = DTF_DATETIME_FMT_MONTH_UPPER_STR[i].mDateInfo;
                                    sElementStrLen = DTF_DATETIME_FMT_MONTH_UPPER_STR[i].mStrLen;
                                    
                                    break;
                                }
                                else
                                {
                                    if( stlStrncasecmp(
                                            sDateTimeStringPtr,
                                            DTF_DATETIME_FMT_MON_UPPER_STR[i].mStr,
                                            DTF_DATETIME_FMT_MON_UPPER_STR[i].mStrLen )
                                        == 0 )
                                    {
                                        sIntValue = DTF_DATETIME_FMT_MON_UPPER_STR[i].mDateInfo;
                                        sElementStrLen = DTF_DATETIME_FMT_MON_UPPER_STR[i].mStrLen;
                                        
                                        break;
                                    }
                                    else
                                    {
                                        /* Do Nothing */
                                    }
                                }                                
                            }
                            else
                            {
                                STL_TRY_THROW( sRemainStrLen >= DTF_DATETIME_FMT_MON_MIN_STR_LEN,
                                               ERROR_MM_FIELD_OVERFLOW );
                                
                                if( stlStrncasecmp(
                                        sDateTimeStringPtr,
                                        DTF_DATETIME_FMT_MON_UPPER_STR[i].mStr,
                                        DTF_DATETIME_FMT_MON_UPPER_STR[i].mStrLen )
                                    == 0 )
                                {
                                    sIntValue = DTF_DATETIME_FMT_MON_UPPER_STR[i].mDateInfo;
                                    sElementStrLen = DTF_DATETIME_FMT_MON_UPPER_STR[i].mStrLen;
                                    
                                    break;
                                }
                                else
                                {
                                    /* Do Nothing */
                                }
                            }
                        }
                        
                        STL_TRY_THROW( DTF_DATETIME_FMT_MONTH_UPPER_STR[i].mStr != NULL,
                                       ERROR_MM_FIELD_OVERFLOW );

                        aDtlExpTime->mMonth = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_RM:
                case DTF_DATETIME_FMT_Rm:
                case DTF_DATETIME_FMT_rm:
                    {
                        sRemainStrLen = (sDateTimeStringPtrEnd - sDateTimeStringPtr);
                        
                        STL_TRY_THROW( sRemainStrLen >= DTF_DATETIME_FMT_RM_MONTH_MIN_STR_LEN,
                                       ERROR_MM_FIELD_OVERFLOW );
                        
                        for( i = (DTF_DATETIME_FMT_RM_MONTH_MAX - 1); i >= 0; i-- )
                        {
                            if( sRemainStrLen >= DTF_DATETIME_FMT_RM_MONTH_UPPER_STR[i].mStrLen )
                            {
                                if( stlStrncasecmp(
                                        sDateTimeStringPtr,
                                        DTF_DATETIME_FMT_RM_MONTH_UPPER_STR[i].mStr,
                                        DTF_DATETIME_FMT_RM_MONTH_UPPER_STR[i].mStrLen )
                                    == 0 )
                                {
                                    sIntValue = DTF_DATETIME_FMT_RM_MONTH_UPPER_STR[i].mDateInfo;
                                    sElementStrLen = DTF_DATETIME_FMT_RM_MONTH_UPPER_STR[i].mStrLen;
                                    
                                    break;
                                }
                                else
                                {
                                    /* Do Nothing */
                                }
                            }
                            else
                            {
                                /* Do Noting */
                            }
                        }
                        
                        STL_TRY_THROW( i >= 0,
                                       ERROR_MM_FIELD_OVERFLOW );

                        aDtlExpTime->mMonth = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_DY:
                case DTF_DATETIME_FMT_Dy:
                case DTF_DATETIME_FMT_dy:                
                    {
                        sRemainStrLen = (sDateTimeStringPtrEnd - sDateTimeStringPtr);
                    
                        STL_TRY_THROW( sRemainStrLen >= DTF_DATETIME_FMT_DY_STR_LEN,
                                       ERROR_INVALIDE_DAY_OF_WEEK );

                        for( i = 0; DTF_DATETIME_FMT_DY_UPPER_STR[i].mStr != NULL; i++ )
                        {
                            if( stlStrncasecmp(
                                    sDateTimeStringPtr,
                                    DTF_DATETIME_FMT_DY_UPPER_STR[i].mStr,
                                    DTF_DATETIME_FMT_DY_UPPER_STR[i].mStrLen ) == 0 )
                            {
                                sIntValue = DTF_DATETIME_FMT_DY_UPPER_STR[i].mDateInfo;
                                sElementStrLen = DTF_DATETIME_FMT_DY_UPPER_STR[i].mStrLen;

                                break;
                            }
                            else
                            {
                                /* Do Nothing */
                            }
                        }

                        STL_TRY_THROW( DTF_DATETIME_FMT_DY_UPPER_STR[i].mStr != NULL,
                                       ERROR_INVALIDE_DAY_OF_WEEK );

                        sIntValueForWeekDay = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_DAY:
                case DTF_DATETIME_FMT_Day:
                case DTF_DATETIME_FMT_day:
                    {
                        sRemainStrLen = (sDateTimeStringPtrEnd - sDateTimeStringPtr);
                    
                        STL_TRY_THROW( sRemainStrLen >= DTF_DATETIME_FMT_DAY_MIN_STR_LEN,
                                       ERROR_INVALIDE_DAY_OF_WEEK );

                        for( i = 0; DTF_DATETIME_FMT_DAY_UPPER_STR[i].mStr != NULL; i++ )
                        {
                            if( sRemainStrLen >= DTF_DATETIME_FMT_DAY_UPPER_STR[i].mStrLen )
                            {                        
                                if( stlStrncasecmp(
                                        sDateTimeStringPtr,
                                        DTF_DATETIME_FMT_DAY_UPPER_STR[i].mStr,
                                        DTF_DATETIME_FMT_DAY_UPPER_STR[i].mStrLen ) == 0 )
                                {
                                    sIntValue = DTF_DATETIME_FMT_DAY_UPPER_STR[i].mDateInfo;
                                    sElementStrLen = DTF_DATETIME_FMT_DAY_UPPER_STR[i].mStrLen;
                                
                                    break;
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
                        }

                        STL_TRY_THROW( DTF_DATETIME_FMT_DAY_UPPER_STR[i].mStr != NULL,
                                       ERROR_INVALIDE_DAY_OF_WEEK );
                        
                        sIntValueForWeekDay = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_AD:
                case DTF_DATETIME_FMT_ad:                    
                case DTF_DATETIME_FMT_BC:
                case DTF_DATETIME_FMT_bc:                    
                    {
                        sRemainStrLen = (sDateTimeStringPtrEnd - sDateTimeStringPtr);
                        
                        STL_TRY_THROW( sRemainStrLen >= DTF_DATETIME_FMT_AD_STR_LEN,
                                       ERROR_REQUIRED_AD_BC_VALUE );

                        if( stlStrncasecmp(
                                sDateTimeStringPtr,
                                DTF_DATETIME_FMT_BC_STR,
                                DTF_DATETIME_FMT_BC_STR_LEN ) == 0 )
                        {
                            sIsBc = STL_TRUE;
                        }
                        else
                        {
                            if( stlStrncasecmp(
                                    sDateTimeStringPtr,
                                    DTF_DATETIME_FMT_AD_STR,
                                    DTF_DATETIME_FMT_AD_STR_LEN ) == 0 )
                            {
                                sIsBc = STL_FALSE;
                            }
                            else
                            {
                                STL_THROW( ERROR_REQUIRED_AD_BC_VALUE );
                            }
                        }

                        sElementStrLen = DTF_DATETIME_FMT_BC_STR_LEN;
                    }
                    break;
                case DTF_DATETIME_FMT_A_D:
                case DTF_DATETIME_FMT_a_d:                    
                case DTF_DATETIME_FMT_B_C:
                case DTF_DATETIME_FMT_b_c:                    
                    {
                        sRemainStrLen = (sDateTimeStringPtrEnd - sDateTimeStringPtr);
                        
                        STL_TRY_THROW( sRemainStrLen >= DTF_DATETIME_FMT_A_D_STR_LEN,
                                       ERROR_REQUIRED_AD_BC_VALUE );

                        if( stlStrncasecmp(
                                sDateTimeStringPtr,
                                DTF_DATETIME_FMT_B_C_STR,
                                DTF_DATETIME_FMT_B_C_STR_LEN ) == 0 )
                        {
                            sIsBc = STL_TRUE;
                        }
                        else
                        {
                            if( stlStrncasecmp(
                                    sDateTimeStringPtr,
                                    DTF_DATETIME_FMT_A_D_STR,
                                    DTF_DATETIME_FMT_A_D_STR_LEN ) == 0 )
                            {
                                sIsBc = STL_FALSE;
                            }
                            else
                            {
                                STL_THROW( ERROR_REQUIRED_AD_BC_VALUE );
                            }
                        }

                        sElementStrLen = DTF_DATETIME_FMT_B_C_STR_LEN;
                    }
                    break;
                case DTF_DATETIME_FMT_A_M:
                case DTF_DATETIME_FMT_a_m:                    
                case DTF_DATETIME_FMT_P_M:
                case DTF_DATETIME_FMT_p_m:                    
                    {
                        sRemainStrLen = (sDateTimeStringPtrEnd - sDateTimeStringPtr);
                        
                        STL_TRY_THROW( sRemainStrLen >= DTF_DATETIME_FMT_A_M_STR_LEN,
                                       ERROR_REQUIRED_AM_PM_VALUE );
                        
                        if( stlStrncasecmp(
                                sDateTimeStringPtr,
                                DTF_DATETIME_FMT_P_M_STR,
                                DTF_DATETIME_FMT_P_M_STR_LEN ) == 0 )
                        {
                            sIsPm = STL_TRUE;
                        }
                        else
                        {
                            if( stlStrncasecmp(
                                    sDateTimeStringPtr,
                                    DTF_DATETIME_FMT_A_M_STR,
                                    DTF_DATETIME_FMT_A_M_STR_LEN ) == 0 )
                            {
                                sIsPm = STL_FALSE;
                            }
                            else
                            {
                                STL_THROW( ERROR_REQUIRED_AM_PM_VALUE );
                            }
                        }

                        sElementStrLen = DTF_DATETIME_FMT_P_M_STR_LEN;
                    }
                    break;
                case DTF_DATETIME_FMT_AM:
                case DTF_DATETIME_FMT_am:                    
                case DTF_DATETIME_FMT_PM:
                case DTF_DATETIME_FMT_pm:                    
                    {
                        sRemainStrLen = (sDateTimeStringPtrEnd - sDateTimeStringPtr);
                        
                        STL_TRY_THROW( sRemainStrLen >= DTF_DATETIME_FMT_AM_STR_LEN,
                                       ERROR_REQUIRED_AM_PM_VALUE );
                        
                        if( stlStrncasecmp(
                                sDateTimeStringPtr,
                                DTF_DATETIME_FMT_PM_STR,
                                DTF_DATETIME_FMT_PM_STR_LEN ) == 0 )
                        {
                            sIsPm = STL_TRUE;
                        }
                        else
                        {
                            if( stlStrncasecmp(
                                    sDateTimeStringPtr,
                                    DTF_DATETIME_FMT_AM_STR,
                                    DTF_DATETIME_FMT_AM_STR_LEN ) == 0 )
                            {
                                sIsPm = STL_FALSE;
                            }
                            else
                            {
                                STL_THROW( ERROR_REQUIRED_AM_PM_VALUE );
                            }
                        }

                        sElementStrLen = DTF_DATETIME_FMT_PM_STR_LEN;
                    }
                    break;
                case DTF_DATETIME_FMT_Am:
                case DTF_DATETIME_FMT_Pm:
                    {
                        sRemainStrLen = (sDateTimeStringPtrEnd - sDateTimeStringPtr);
                        
                        STL_TRY_THROW( sRemainStrLen >= DTF_DATETIME_FMT_AM_STR_LEN,
                                       ERROR_REQUIRED_AM_PM_VALUE );
                        
                        if( stlStrncasecmp(
                                sDateTimeStringPtr,
                                DTF_DATETIME_FMT_Pm_STR,
                                DTF_DATETIME_FMT_PM_STR_LEN ) == 0 )
                        {
                            sIsPm = STL_TRUE;
                        }
                        else
                        {
                            if( stlStrncasecmp(
                                    sDateTimeStringPtr,
                                    DTF_DATETIME_FMT_Am_STR,
                                    DTF_DATETIME_FMT_AM_STR_LEN ) == 0 )
                            {
                                sIsPm = STL_FALSE;
                            }
                            else
                            {
                                STL_THROW( ERROR_REQUIRED_AM_PM_VALUE );
                            }
                        }

                        sElementStrLen = DTF_DATETIME_FMT_PM_STR_LEN;
                    }
                    break;
                case DTF_DATETIME_FMT_SYYYY:
                    {
                        if( *sDateTimeStringPtr == '+' )
                        {
                            sIsNegative = STL_FALSE;
                            sDateTimeStringPtr++;
                        }
                        else if( *sDateTimeStringPtr == '-' )
                        {
                            sIsNegative = STL_TRUE;
                            sDateTimeStringPtr++;
                        }
                        else
                        {
                            sIsNegative = STL_FALSE;
                        }

                        STL_TRY( dtfToDateTimeGetIntValueFromParseString(
                                     sFormatStrInfo,
                                     sDateTimeStringPtr,
                                     sDateTimeStringPtrEnd,
                                     sFormatNode,
                                     & sIntValue,
                                     & sIntValueDigitCnt,
                                     & sLeadingZeroDigitCnt,
                                     & sElementStrLen,
                                     aVectorFunc,
                                     aVectorArg,
                                     aErrorStack )
                                 == STL_SUCCESS );

                        STL_TRY_THROW( sIntValue != 0, ERROR_DATETIME_YEAR_VALUE );                        
                        
                        if( sIsNegative == STL_TRUE )
                        {
                            aDtlExpTime->mYear = -(sIntValue - 1);
                        }
                        else
                        {
                            aDtlExpTime->mYear = sIntValue;
                        }
                    }
                    break;
                case DTF_DATETIME_FMT_Y_YYY:
                    {
                        sTmpPtr = sDateTimeStringPtr;
                        
                        while( sTmpPtr < sDateTimeStringPtrEnd )
                        {
                            if( (*(sTmpPtr)) == ',' )
                            {
                                break;
                            }
                            else
                            {
                                sTmpPtr++;
                            }
                        }

                        if( *(sTmpPtr) == ',' )
                        {
                            sIntValueBeforeCommaStrLen = sTmpPtr - sDateTimeStringPtr;
                            
                            STL_TRY( dtdToInt64FromString(
                                         sDateTimeStringPtr,
                                         sIntValueBeforeCommaStrLen,
                                         & sIntValueBeforeComma,
                                         aErrorStack )
                                     == STL_SUCCESS );
                            
                            /* comma 이전의 string 이후로 포인터 이동 */
                            sDateTimeStringPtr = sTmpPtr;

                            STL_DASSERT( *sDateTimeStringPtr == ',' );
                            
                            /* comma 이후로 포인터 이동 */
                            sDateTimeStringPtr++;
                            
                            STL_TRY( dtfToDateTimeGetIntValueFromParseString(
                                         sFormatStrInfo,
                                         sDateTimeStringPtr,
                                         sDateTimeStringPtrEnd,
                                         sFormatNode,
                                         & sIntValue,
                                         & sIntValueDigitCnt,
                                         & sLeadingZeroDigitCnt,
                                         & sElementStrLen,
                                         aVectorFunc,
                                         aVectorArg,
                                         aErrorStack )
                                     == STL_SUCCESS );
                            
                            /* comma 이전의 년도 계산 */
                            switch( sIntValueBeforeCommaStrLen )
                            {
                                case 3:
                                    STL_TRY_THROW( sElementStrLen == 1, ERROR_DATETIME_YEAR_VALUE );
                                    sIntValueBeforeComma = sIntValueBeforeComma * DTF_TEN_YEAR;
                                    break;
                                case 2:
                                    STL_TRY_THROW( sElementStrLen == 2, ERROR_DATETIME_YEAR_VALUE );
                                    sIntValueBeforeComma = sIntValueBeforeComma * DTF_ONE_HUNDRED_YEAR;
                                    break;
                                case 1:
                                    STL_TRY_THROW( sElementStrLen == 3, ERROR_DATETIME_YEAR_VALUE );
                                    sIntValueBeforeComma = sIntValueBeforeComma * DTF_ONE_THOUSAND_YEAR;
                                    break;
                                default:
                                    STL_THROW( ERROR_DATETIME_YEAR_VALUE );
                                    break;
                            }

                            sIntValue = sIntValue + sIntValueBeforeComma;
                        }
                        else
                        {
                            STL_TRY( dtfToDateTimeGetIntValueFromParseString(
                                         sFormatStrInfo,
                                         sDateTimeStringPtr,
                                         sTmpPtr, 
                                         sFormatNode,
                                         & sIntValue,
                                         & sIntValueDigitCnt,
                                         & sLeadingZeroDigitCnt,
                                         & sElementStrLen,
                                         aVectorFunc,
                                         aVectorArg,
                                         aErrorStack )
                                     == STL_SUCCESS );
                        }

                        STL_TRY_THROW( (sIntValue >= 1) && (sIntValue <= DTL_JULIAN_MAXYEAR),
                                       ERROR_DATETIME_YEAR_VALUE );
                        
                        aDtlExpTime->mYear = sIntValue;
                    }
                    break;
                case DTF_DATETIME_FMT_TZH:
                    {
                        if( *sDateTimeStringPtr == '+' )
                        {
                            sIsNegativeTimeZoneHour = STL_FALSE;
                            sDateTimeStringPtr++;
                        }
                        else if( *sDateTimeStringPtr == '-' )
                        {
                            sIsNegativeTimeZoneHour = STL_TRUE;
                            sDateTimeStringPtr++;
                        }
                        else
                        {
                            sIsNegativeTimeZoneHour = STL_FALSE;
                        }
                        
                        /* 공백 무시 */
                        while( sDateTimeStringPtr < sDateTimeStringPtrEnd )
                        {
                            if( stlIsspace(*sDateTimeStringPtr) == STL_TRUE )
                            {
                                sDateTimeStringPtr++;
                            }
                            else
                            {
                                break;
                            }
                        }

                        STL_TRY( dtfToDateTimeGetIntValueFromParseString(
                                     sFormatStrInfo,
                                     sDateTimeStringPtr,
                                     sDateTimeStringPtrEnd,
                                     sFormatNode,
                                     & sIntValue,
                                     & sIntValueDigitCnt,
                                     & sLeadingZeroDigitCnt,
                                     & sElementStrLen,
                                     aVectorFunc,
                                     aVectorArg,
                                     aErrorStack )
                                 == STL_SUCCESS );
                        
                        STL_TRY_THROW( (sIntValue >= DTL_TIMEZONE_MIN_HOUR) &&
                                       (sIntValue <= DTL_TIMEZONE_MAX_HOUR),
                                       ERROR_TIMEZONE_HOUR_OVERFLOW );
                        
                        sTimeZoneHour = sIntValue * DTL_SECS_PER_HOUR;
                    }
                    break;
                default:
                    /** @todo 코드 구현후 주석풀기 */
                    /* STL_DASSERT( STL_FALSE ); */
                    STL_THROW( ERROR_INVALID_DATE_FORMAT );
                    break;
            }
        }

        sDateTimeStringPtr += sElementStrLen;

        /* field간의 공백 무시 */
        while( sDateTimeStringPtr < sDateTimeStringPtrEnd )
        {
            if( stlIsspace(*sDateTimeStringPtr) == STL_TRUE )
            {
                sDateTimeStringPtr++;
            }
            else
            {
                break;
            }
        }
    }

    /**
     * string 과 format node의 마지막 공백 제거.
     */

    /* field간의 공백 무시 */
    while( sDateTimeStringPtr < sDateTimeStringPtrEnd )
    {
        if( stlIsspace(*sDateTimeStringPtr) == STL_TRUE )
        {
            sDateTimeStringPtr++;
        }
        else
        {
            break;
        }
    }

    STL_RAMP( RAMP_NODE_CHECK );        
    
    for( ;
         sFormatNode->mType != DTF_FORMAT_NODE_TYPE_END;
         sFormatNode = (dtlDateTimeFormatNode*)((stlChar*)sFormatNode + sAlignSize) )
    {
        if( sFormatNode->mType == DTF_FORMAT_NODE_TYPE_CHAR )
        {
            for( i = 0; i < sFormatNode->mPosLen; i++ )
            {
                if( stlIsspace(*(sFormatStrInfo->mStr + sFormatNode->mPos + i)) == STL_TRUE )
                {
                    /* Do Nothing */
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            break;
        }
    }
    
    if( (sFormatNode->mType != DTF_FORMAT_NODE_TYPE_END) &&
        (sDateTimeStringPtr == sDateTimeStringPtrEnd ) )
    {
        for( ;
             sFormatNode->mType != DTF_FORMAT_NODE_TYPE_END;
             sFormatNode = (dtlDateTimeFormatNode*)((stlChar*)sFormatNode + sAlignSize) )
        {
            if( sFormatNode->mType == DTF_FORMAT_NODE_TYPE_CHAR )
            {
                /* Do Nothing */
            }
            else
            {
                /* DTF_FORMAT_NODE_TYPE_KEYWORD
                 *
                 * 아래와 같은 부분이 처리될 수 있도록.
                 * 예) to_time_with_time_zone( '23:59:59', 'HH24:MI:SS.FF6 TZH TZM' )
                 *     to_timestamp( '1999-01-01', 'YYYY-MM-DD HH24:MI:SS.FF6' )
                 *     to_timestamp_with_time_zone( '1999-01-01', 'YYYY-MM-DD HH24:MI:SS.FF6 TZH:TZM' )
                 *
                 * etc) format string 체크 부분에서 data type 마다 처리할 수 있는 format element들이 정리되므로,
                 *      여기서 type 마다 처리될 수 있는 element들을 체크하지 않아도 됨.
                 */

                if( ( sFormatNode->mKey->mTypeMode == DTL_FORMAT_TYPE_TIME ) ||
                    ( sFormatNode->mKey->mTypeMode == DTL_FORMAT_TYPE_WITH_TIME_ZONE ) )
                {
                    switch( sFormatNode->mKey->mId )
                    {
                        case DTF_DATETIME_FMT_A_M:
                        case DTF_DATETIME_FMT_a_m:                    
                        case DTF_DATETIME_FMT_P_M:
                        case DTF_DATETIME_FMT_p_m:
                        case DTF_DATETIME_FMT_AM:
                        case DTF_DATETIME_FMT_am:                    
                        case DTF_DATETIME_FMT_PM:
                        case DTF_DATETIME_FMT_pm:                    
                        case DTF_DATETIME_FMT_Am:
                        case DTF_DATETIME_FMT_Pm:                            
                            sNotSpecifiedAmPmDateTimeString = STL_TRUE;
                            break;
                        case DTF_DATETIME_FMT_SSSSS:
                            sNotSpecifiedSSSSSDateTimeString = STL_TRUE;
                            break;
                        case DTF_DATETIME_FMT_TZH:
                            sNotSpecifiedCurrentTimeZoneHour = STL_TRUE;
                            break;
                        case DTF_DATETIME_FMT_TZM:
                            sNotSpecifiedCurrentTimeZoneMinute = STL_TRUE;
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    STL_THROW( RAMP_FINISH_NODE_CHECK );
                }
            }
        }
    }
    else
    {
        /* Do Nothing */
    }

    STL_RAMP( RAMP_FINISH_NODE_CHECK );
    
    if( (sFormatNode->mType == DTF_FORMAT_NODE_TYPE_END) &&
        (sDateTimeStringPtr == sDateTimeStringPtrEnd ) )
    {
        /* Do Nothing */
    }
    else
    {
        if( (sFormatNode->mType == DTF_FORMAT_NODE_TYPE_END) &&
            (sDateTimeStringPtr < sDateTimeStringPtrEnd) )
        {
            STL_THROW( ERROR_DATETIME_NOT_LONG_ENOUGH_FORMAT_PICTURE );            
        }
        else if( (sFormatNode->mType != DTF_FORMAT_NODE_TYPE_END) &&
                 (sDateTimeStringPtr == sDateTimeStringPtrEnd ) )
        {
            STL_THROW( ERROR_DATETIME_NOT_LONG_ENOUGH_INPUT_VALUE );            
        }
        else
        {
            STL_DASSERT( STL_FALSE );
        }
    }
    
    /**
     * 시간날짜 구성요소를 구하고 난 후 체크 할 수 있는 element들을 처리한다.
     */
    
    if( sFormatElementSetInfo->mAmPm == STL_TRUE )
    {
        if( sNotSpecifiedAmPmDateTimeString == STL_FALSE )
        {
            if( sFormatElementSetInfo->mHour == STL_TRUE )
            {
                if( sIsPm == STL_TRUE )
                {
                    if( aDtlExpTime->mHour < DTF_HH12_MAX )
                    {
                        aDtlExpTime->mHour = aDtlExpTime->mHour + DTF_HH12_MAX;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    if( aDtlExpTime->mHour == DTF_HH12_MAX )
                    {
                        aDtlExpTime->mHour = 0;
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

    if( sFormatElementSetInfo->mSssss == STL_TRUE )
    {
        if( sNotSpecifiedSSSSSDateTimeString == STL_FALSE )
        {
            sIntHour = sSSSSS / DTL_SECS_PER_HOUR;
            sSSSSS = sSSSSS - (sIntHour * DTL_SECS_PER_HOUR);

            sIntMinute = sSSSSS / DTL_SECS_PER_MINUTE;
            sSSSSS = sSSSSS - (sIntMinute * DTL_SECS_PER_MINUTE);
        
            sIntSecond = sSSSSS;
        
            if( sFormatElementSetInfo->mHour == STL_TRUE )
            {
                STL_TRY_THROW( aDtlExpTime->mHour == sIntHour,
                               ERROR_HOUR_CONFLICTS_WITH_SECONDS_IN_DAY );
            }
            else
            {
                aDtlExpTime->mHour = sIntHour;
            }
        
            if( sFormatElementSetInfo->mMinute == STL_TRUE )
            {
                STL_TRY_THROW( aDtlExpTime->mMinute == sIntMinute,
                               ERROR_MINUTES_OF_HOUR_CONFLICTS_WITH_SECONDS_IN_DAY );
            }
            else
            {
                aDtlExpTime->mMinute = sIntMinute;
            }
        
            if( sFormatElementSetInfo->mSecond == STL_TRUE )
            {
                STL_TRY_THROW( aDtlExpTime->mSecond == sIntSecond,
                               ERROR_SECONDS_OF_MINUTES_CONFLICTS_WITH_SECONDS_IN_DAY );
            }
            else
            {
                aDtlExpTime->mSecond = sIntSecond;
            }
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
    
    if( sFormatElementSetInfo->mWeekDay == STL_TRUE )
    {
        STL_DASSERT( sIntValueForWeekDay >= 0 );
        
        sJulianDate = dtdDate2JulianDate( aDtlExpTime->mYear,
                                          aDtlExpTime->mMonth,
                                          aDtlExpTime->mDay );
        
        aDtlExpTime->mWeekDay = ( sJulianDate + 1 ) % 7;
        
        if( sExistFormatElementD == STL_TRUE )
        {
            STL_TRY_THROW( ( (aDtlExpTime->mWeekDay + 1) == sIntValueForWeekDay ),
                           ERROR_DAY_OF_WEEK_CONFLICTS_WITH_JULIAN_DATE );
        }
        else
        {
            STL_TRY_THROW( ( aDtlExpTime->mWeekDay == sIntValueForWeekDay ),
                           ERROR_DAY_OF_WEEK_CONFLICTS_WITH_JULIAN_DATE );
        }
    }
    else
    {
        /* Do Nothing */
    }

    if( sFormatElementSetInfo->mYearDay == STL_TRUE )
    {
        /* day of year =>  1 ~ 365( or 366 ) */
        STL_TRY_THROW( ( sIntValueForYearDay >= DTF_FIRST_DAY_OF_YEAR ) &&
                       ( sIntValueForYearDay <=
                         ((DTL_IS_LEAP(aDtlExpTime->mYear) == STL_FALSE) ?
                          DTF_LAST_DAYS_OF_YEAR : DTF_LAST_DAYS_OF_LEAP_YEAR) ),
                       ERROR_INVALID_DAY_OF_YEAR );
        
        /* 평년인지 윤년인지의 정보를 얻는다. */
        sIntValue = ( DTL_IS_LEAP(aDtlExpTime->mYear) == STL_FALSE ) ? 0 : 1;
        
        for( i = 1; i <= DTL_MONTHS_PER_YEAR; i++ )
        {
            if( sIntValueForYearDay <= dtlSumDayOfYear[sIntValue][i] )
            {
                break;
            }
            else
            {
                /* Do Nothing */
            }
        }
        
        sMonthForFormatDDD = i;
        sDayForFormatDDD   = sIntValueForYearDay - dtlSumDayOfYear[sIntValue][i-1];
        
        if( ( sFormatElementSetInfo->mMonth == STL_TRUE ) &&
            ( sFormatElementSetInfo->mDay   == STL_TRUE ) )
        {
            STL_TRY_THROW( aDtlExpTime->mMonth == sMonthForFormatDDD,
                           ERROR_MONTH_CONFLICTS_WITH_JULIAN_DATE );
            
            STL_TRY_THROW( aDtlExpTime->mDay == sDayForFormatDDD,
                           ERROR_DAY_OF_MONTH_CONFLICTS_WITH_JULIAN_DATE );
        }
        else
        {
            if( sFormatElementSetInfo->mMonth == STL_TRUE )
            {
                STL_TRY_THROW( aDtlExpTime->mMonth == sMonthForFormatDDD,
                               ERROR_MONTH_CONFLICTS_WITH_JULIAN_DATE );
            }
            else
            {
                /* Do Nothing */
            }

            if( sFormatElementSetInfo->mDay == STL_TRUE )
            {
                STL_TRY_THROW( aDtlExpTime->mDay == sDayForFormatDDD,
                               ERROR_DAY_OF_MONTH_CONFLICTS_WITH_JULIAN_DATE );
            }
            else
            {
                /* Do Nothing */                
            }

            aDtlExpTime->mMonth = sMonthForFormatDDD;
            aDtlExpTime->mDay = sDayForFormatDDD;
        }
    }
    else
    {
        /* Do Nothing */
    }

    if( sFormatElementSetInfo->mJulianDay == STL_TRUE )
    {
        STL_TRY( dtdJulianDate2Date( sJulianDate,
                                     & sIntYear,
                                     & sIntMonth,
                                     & sIntDay,
                                     aErrorStack )
                 == STL_SUCCESS );
        
        if( sFormatElementSetInfo->mYear == STL_TRUE )
        {
            STL_TRY_THROW( aDtlExpTime->mYear == sIntYear,
                           ERROR_DATE_FORMAT_YEAR_CONFLICTS_JULIANDAY );
        }
        else
        {
            aDtlExpTime->mYear = sIntYear;
        }
        
        if( sFormatElementSetInfo->mMonth == STL_TRUE )
        {
            STL_TRY_THROW( aDtlExpTime->mMonth == sIntMonth,
                           ERROR_MONTH_CONFLICTS_WITH_JULIAN_DATE );
        }
        else
        {
            aDtlExpTime->mMonth = sIntMonth;
        }       
       
        if( sFormatElementSetInfo->mDay == STL_TRUE )
        {
            STL_TRY_THROW( aDtlExpTime->mDay == sIntDay,
                           ERROR_DAY_OF_MONTH_CONFLICTS_WITH_JULIAN_DATE );
        }
        else
        {
            aDtlExpTime->mDay = sIntDay;
        }
    }
    else
    {
        /* Do Nothing */        
    }
    
    if( sFormatElementSetInfo->mBc == STL_TRUE )
    {
        if( sFormatElementSetInfo->mYear == STL_TRUE )
        {
            if( sIsBc == STL_TRUE )
            {
                aDtlExpTime->mYear = -(aDtlExpTime->mYear - 1);
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
    }
    else
    {
        /* Do Nothing */
    }
    
    if( ( aToDateTimeFormatInfo->mNeedCurrentTimeZoneOffset == STL_TRUE ) ||
        ( sNotSpecifiedCurrentTimeZoneHour == STL_TRUE ) ||
        ( sNotSpecifiedCurrentTimeZoneMinute == STL_TRUE ) )
    {
        sCurrentTimeZone = aVectorFunc->mGetGMTOffsetFunc(aVectorArg);

        if( sCurrentTimeZone >= 0 )
        {
            sIsNegative = STL_FALSE;
        }
        else
        {
            sIsNegative = STL_TRUE;
            sCurrentTimeZone = -(sCurrentTimeZone);
        }
        
        if( (sFormatElementSetInfo->mTimeZoneHour == STL_FALSE) ||
            (sNotSpecifiedCurrentTimeZoneHour == STL_TRUE) )
        {
            sNeedCurrentTimeZoneHour = STL_TRUE;
        }

        if( (sFormatElementSetInfo->mTimeZoneMinute == STL_FALSE) ||
            (sNotSpecifiedCurrentTimeZoneMinute == STL_TRUE) )
        {
            sNeedCurrentTimeZoneMinute = STL_TRUE;
        }

        if( (sNeedCurrentTimeZoneHour == STL_TRUE) &&
            (sNeedCurrentTimeZoneMinute == STL_TRUE) )
        {
            sTimeZone = sCurrentTimeZone;

            sIsNegativeTimeZoneHour = sIsNegative;
            sTimeZone = (sIsNegativeTimeZoneHour == STL_FALSE) ? sTimeZone : -sTimeZone;
        }
        else
        {
            if( sNeedCurrentTimeZoneHour == STL_TRUE )
            {
                sTimeZoneHour = sCurrentTimeZone - (sCurrentTimeZone % DTL_SECS_PER_HOUR);
                sIsNegativeTimeZoneHour = sIsNegative;
            }
            else
            {
                /* (sNeedCurrentTimeZoneMinute == STL_TRUE) */
                
                sTimeZoneMinute = sCurrentTimeZone - (sCurrentTimeZone / DTL_SECS_PER_HOUR) * DTL_SECS_PER_HOUR;
            }

            sTimeZone = sTimeZoneHour + sTimeZoneMinute;

            sTimeZone = (sIsNegativeTimeZoneHour == STL_FALSE) ? sTimeZone : -sTimeZone;
        }
        
        STL_TRY_THROW( (sTimeZone >= DTL_TIMEZONE_MIN_OFFSET) &&
                       (sTimeZone <= DTL_TIMEZONE_MAX_OFFSET),
                       ERROR_TZDISP_OVERFLOW );
    }
    else
    {
        if( (sFormatElementSetInfo->mTimeZoneHour == STL_TRUE) &&
            (sFormatElementSetInfo->mTimeZoneMinute == STL_TRUE) )
        {
            sTimeZone = sTimeZoneHour + sTimeZoneMinute;

            sTimeZone = (sIsNegativeTimeZoneHour == STL_FALSE) ? sTimeZone : -sTimeZone;            
            
            STL_TRY_THROW( (sTimeZone >= DTL_TIMEZONE_MIN_OFFSET) &&
                           (sTimeZone <= DTL_TIMEZONE_MAX_OFFSET),
                           ERROR_TZDISP_OVERFLOW );
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    /* 일 validate */
    if( ( aDateTimeType == DTL_TYPE_DATE ) ||
        ( aDateTimeType == DTL_TYPE_TIMESTAMP ) ||
        ( aDateTimeType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ) )
    {
        STL_TRY_THROW(
            (aDtlExpTime->mDay <=
             dtlDayTab[DTL_IS_LEAP(aDtlExpTime->mYear)][aDtlExpTime->mMonth - 1]),
            ERROR_DATE_FOR_MONTH_SPECIFIED );
    }
    else
    {
        /* Do Nothing */
    }
    
    *aTimeZone = -sTimeZone;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_DATE_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_NOT_RECOGNIZED,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_NOT_MATCH_FORMAT_STR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_NOT_MATCH_FORMAT_STRING,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( ERROR_DATETIME_NOT_LONG_ENOUGH_INPUT_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_NOT_LONG_ENOUGH_INPUT_VALUE,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( ERROR_DATETIME_NOT_LONG_ENOUGH_FORMAT_PICTURE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_NOT_LONG_ENOUGH_FORMAT_PICTURE,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( ERROR_DATETIME_YEAR_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_INVALID_YEAR_VALUE,
                      NULL,
                      aErrorStack,
                      DTF_DATE_MIN_YEAR_STRING,
                      DTF_DATE_MAX_YEAR_STRING );
    }

    STL_CATCH( ERROR_MM_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_INVALID_MONTH_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_DD_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_INVALID_LAST_DAY_OF_MONTH_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_DAY_OF_WEEK_CONFLICTS_WITH_JULIAN_DATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_CODE_CONFLICT,
                      NULL,
                      aErrorStack,
                      DTF_DATE_DAY_OF_WEEK_STRING,
                      DTF_DATE_JULIAN_DATE_STRING );        
    }
    
    STL_CATCH( ERROR_INVALIDE_DAY_OF_WEEK )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_INVALID_DAY_OF_WEEK,
                      NULL,
                      aErrorStack );        
    }

    STL_CATCH( ERROR_DAY_OF_MONTH_CONFLICTS_WITH_JULIAN_DATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_CODE_CONFLICT,
                      NULL,
                      aErrorStack,
                      DTF_DATE_DAY_OF_MONTH_STRING,
                      DTF_DATE_JULIAN_DATE_STRING );        
    }

    STL_CATCH( ERROR_INVALID_DAY_OF_YEAR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_INVALID_DAY_OF_YEAR,
                      NULL,
                      aErrorStack );        
    }

    STL_CATCH( ERROR_MONTH_CONFLICTS_WITH_JULIAN_DATE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_CODE_CONFLICT,
                      NULL,
                      aErrorStack,
                      DTF_DATE_MONTH_STRING,
                      DTF_DATE_JULIAN_DATE_STRING );        
    }
    
    STL_CATCH( ERROR_REQUIRED_AD_BC_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_REQUIRED_AD_BC_VALUE,
                      NULL,
                      aErrorStack );        
    }

    STL_CATCH( ERROR_INVALID_JULIAN_DAY_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_INVALID_JULIAN_DATE_VALUE,
                      NULL,
                      aErrorStack );        
    }

    STL_CATCH( ERROR_DATE_FORMAT_YEAR_CONFLICTS_JULIANDAY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_CODE_CONFLICT,
                      NULL,
                      aErrorStack,
                      DTF_DATE_YEAR_STRING,
                      DTF_DATE_JULIAN_DATE_STRING );
    }
    
    STL_CATCH( ERROR_HH24_FILED_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_INVALID_HOUR_VALUE,
                      NULL,
                      aErrorStack,
                      DTF_HH24_MIN,
                      DTF_HH24_MAX );
    }

    STL_CATCH( ERROR_HH12_FILED_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_INVALID_HOUR_VALUE,
                      NULL,
                      aErrorStack,
                      DTF_HH12_MIN,
                      DTF_HH12_MAX );
    }

    STL_CATCH( ERROR_MINUTE_FILED_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_INVALID_MINUTE_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_SECOND_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_INVALID_SECOND_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_FRACTIONAL_SECOND_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_INVALID_FRACTIONAL_SECOND_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_TIMEZONE_HOUR_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_INVALID_TIMEZONE_HOUR_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_TIMEZONE_MINUTE_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_INVALID_TIMEZONE_MINUTE_VALUE,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( ERROR_TZDISP_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_TIME_ZONE_DISPLACEMENT_VALUE,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( ERROR_INVALID_SECONDS_IN_DAY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_INVALID_SECONDS_IN_DAY_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_REQUIRED_AM_PM_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_REQUIRED_AM_PM_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_HOUR_CONFLICTS_WITH_SECONDS_IN_DAY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_CODE_CONFLICT,
                      NULL,
                      aErrorStack,
                      DTF_DATE_HOUR_STRING,
                      DTF_DATE_SECONDS_IN_DAY_STRING );
    }

    STL_CATCH( ERROR_MINUTES_OF_HOUR_CONFLICTS_WITH_SECONDS_IN_DAY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_CODE_CONFLICT,
                      NULL,
                      aErrorStack,
                      DTF_DATE_MINUTES_OF_HOUR_STRING,
                      DTF_DATE_SECONDS_IN_DAY_STRING );
    }

    STL_CATCH( ERROR_SECONDS_OF_MINUTES_CONFLICTS_WITH_SECONDS_IN_DAY )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_CODE_CONFLICT,
                      NULL,
                      aErrorStack,
                      DTF_DATE_SECONDS_OF_MINUTE_STRING,
                      DTF_DATE_SECONDS_IN_DAY_STRING );
    }

    STL_CATCH( ERROR_DATE_FOR_MONTH_SPECIFIED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_NOT_VALID_FOR_MONTH_SPECIFIED,
                      NULL,
                      aErrorStack );
        
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief to_datetime 함수에서 formatNode에 맞는 datetime string을 intvalue로 변경한다.
 * @param[in]  aFormatStrInfo       format str info
 * @param[in]  aDateTimeStr         datetime string
 * @param[in]  aDateTimeStrEnd      datetime string end
 * @param[in]  aFormatNode          formatNode
 * @param[out] aIntValue            formatNode에 맞는 datetime string을 int value로 변환
 * @param[out] aIntValueDigitCnt    변환된 int value의 digit count
 * @param[out] aLeadingZeroDigitCnt 제거된 leading zero digit count ( fractional second 값을 구하기 위해 사용됨.)
 * @param[out] aElementStrLen       formatNode에 맞게 해석된 datetime string의 길이
 * @param[in]  aVectorFunc          aVectorArg
 * @param[in]  aVectorArg           aVectorArg
 * @param[in]  aErrorStack          에러 스택
 */
stlStatus dtfToDateTimeGetIntValueFromParseString( dtlDateTimeFormatStrInfo  * aFormatStrInfo,
                                                   stlChar                   * aDateTimeStr,
                                                   stlChar                   * aDateTimeStrEnd,
                                                   dtlDateTimeFormatNode     * aFormatNode,
                                                   stlInt32                  * aIntValue,
                                                   stlInt32                  * aIntValueDigitCnt,
                                                   stlInt32                  * aLeadingZeroDigitCnt,
                                                   stlInt32                  * aElementStrLen,
                                                   dtlFuncVector             * aVectorFunc,
                                                   void                      * aVectorArg,
                                                   stlErrorStack             * aErrorStack )
{
    stlChar  * sDateTimeStr = aDateTimeStr;
    stlChar    sValue[DTL_DECIMAL_INTEGER_DEFAULT_PRECISION + 1] = {0,};
    stlInt32   sValueLength = 0;
    stlInt32   sLeadingZeroDigitCnt = 0;
//    stlChar * sValueEnd  = sValue + (DTL_DECIMAL_INTEGER_DEFAULT_PRECISION + 1);
    stlInt32   sIntValue = 0;
    stlInt32   sElementStrLen = 0;
    stlInt32   i;
    stlInt32   sCheckLen;
    stlSize    sAlignSize = STL_ALIGN_DEFAULT( STL_SIZEOF(dtlDateTimeFormatNode) );

    sCheckLen = ( (aDateTimeStr + aFormatNode->mKey->mValueMaxLen) <= aDateTimeStrEnd ) ?
        aFormatNode->mKey->mValueMaxLen : (aDateTimeStrEnd - aDateTimeStr);
    
    for( i = 0; i < sCheckLen; i++, sDateTimeStr++ )
    {
        /* leading zero 무시 */
        if( *sDateTimeStr == '0' )
        {
            sLeadingZeroDigitCnt++;
        }
        else
        {
            break;
        }
        
        sElementStrLen++;
    }
    
    if( ((dtlDateTimeFormatNode*)((stlChar*)aFormatNode + sAlignSize))->mType == DTF_FORMAT_NODE_TYPE_CHAR )
    {
        /**
         * 다음 format node가 있는 경우
         */

        for( ; i < sCheckLen; i++, sDateTimeStr++ )
        {
            if( DTF_IS_DIGIT( *sDateTimeStr ) == STL_TRUE )
            {
                sValue[sValueLength] = *sDateTimeStr;
                //DTD_APPEND_CHAR( sValue, sValueEnd, *sDateTimeStr );
                sValueLength++;
            }
            else if( ( DTF_DATETIME_FORMAT_SPECIAL_CHAR_INDEX[(stlUInt8)*(sDateTimeStr)] == 0 ) ||
                     ( stlIsspace( *sDateTimeStr ) == STL_TRUE ) )
            {
                STL_TRY_THROW( sElementStrLen > 0, ERROR_NON_NUMERIC_CHARACTER );

                STL_THROW( RAMP_FINISH_PARSE );
            }
            else
            {
                STL_THROW( ERROR_NON_NUMERIC_CHARACTER );
            }
            
            sElementStrLen++;
        }

        if( sDateTimeStr < aDateTimeStrEnd )
        {
            STL_TRY_THROW( ( DTF_DATETIME_FORMAT_SPECIAL_CHAR_INDEX[(stlUInt8)*(sDateTimeStr)] == 0 ) ||
                           ( *(sDateTimeStr) == *(aFormatStrInfo->mStr + aFormatNode[1].mPos) ),
                           ERROR_NOT_MATCH_FORMAT_STRING );
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        /**
         * 다음 format node가 keyword 이거나 끝인 경우,
         */

        for( ; i < sCheckLen; i++, sDateTimeStr++ )
        {
            if( stlIsdigit( *sDateTimeStr ) == STL_TRUE )
            {
                sValue[sValueLength] = *sDateTimeStr;
                //DTD_APPEND_CHAR( sValue, sValueEnd, *sDateTimeStr );
                sValueLength++;
            }
            else if( stlIsspace( *sDateTimeStr ) == STL_TRUE )
            {
                break;
            }
            else
            {
                STL_THROW( ERROR_NON_NUMERIC_CHARACTER );
            }
            
            sElementStrLen++;
        }
    }

    STL_RAMP( RAMP_FINISH_PARSE );
    
    STL_DASSERT( sElementStrLen <= sCheckLen );
    STL_DASSERT( sValueLength <= sCheckLen );

    if( sValueLength > 0 )
    {
        for( i = 0; i < sValueLength; i++ )
        {
            sIntValue = sIntValue * 10;
            sIntValue = sIntValue + (sValue[i] - '0');
        }
    }
    else
    {
        STL_DASSERT( sValueLength == 0 );
        sIntValue = 0;
    }
    
    *aIntValue            = sIntValue;
    *aIntValueDigitCnt    = sValueLength;
    *aLeadingZeroDigitCnt = sLeadingZeroDigitCnt;
    *aElementStrLen       = sElementStrLen;
    
    STL_DASSERT( (*aIntValue >= 0) &&
                 (*aIntValueDigitCnt >= 0) &&
                 (*aLeadingZeroDigitCnt >= 0) &&
                 (*aElementStrLen >= 0) );
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_NON_NUMERIC_CHARACTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_NON_NUMERIC_CHARACTER,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_NOT_MATCH_FORMAT_STRING )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_NOT_MATCH_FORMAT_STRING,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 입력받은 format string으로 해당되는 keyword 정보를 얻는다.
 * @param[in]  aFormatString
 * @return format string과 일치하는 dtlNumberFormatKeyWord 반환
 */

const dtfNumberFormatKeyWord * dtfGetNumberFormatKeyWord( stlChar  * aFormatString )
{
    stlInt32                         sKeyIdx;
    const dtfNumberFormatKeyWord   * sKeyWord = NULL;

    sKeyIdx = DTF_NUMBER_FORMAT_INDEX[(stlUInt8)*aFormatString];
        
    if( sKeyIdx > -1 )
    {
        sKeyWord = dtfNumberFormattingKeyword + sKeyIdx;
        
        do
        {
            if( stlStrncasecmp( aFormatString,
                                sKeyWord->mName,
                                sKeyWord->mLen ) == 0 )
            {
                break;
            }
            
            if( sKeyWord->mName == NULL )
            {
                sKeyWord = NULL;
                break;
            }

            if( *aFormatString != *(sKeyWord->mName) )
            {
                sKeyWord = NULL;
                break;
            }
            
            sKeyWord++;

        } while( STL_TRUE );
            //} while( *aFormatString == *(sKeyWord->mName) );
    }
    else
    {
        sKeyWord = NULL;
    }

    return sKeyWord;
}


/**
 * @brief format string 으로 formatNode (format info)를 구성한다.
 * @param[in]   aDataType                        dtlDataType
 * @param[in]   aFormatString                    format string
 * @param[in]   aFormatStringLen                 format string length
 * @param[in,out]  aToCharFormatInfoBuffer       to_char format info를 위한 공간
 * @param[in]   aToCharFormatInfoBufferSize      to_char format info를 위한 공간 사이즈
 * @param[in]   aVectorFunc                      aVectorArg
 * @param[in]   aVectorArg                       aVectorArg
 * @param[in]   aErrorStack                      에러 스택
 */

stlStatus dtfGetNumberFormatInfoForToChar( dtlDataType      aDataType,
                                           stlChar        * aFormatString,
                                           stlInt64         aFormatStringLen,
                                           void           * aToCharFormatInfoBuffer,
                                           stlInt32         aToCharFormatInfoBufferSize,
                                           dtlFuncVector  * aVectorFunc,
                                           void           * aVectorArg,
                                           stlErrorStack  * aErrorStack )
{
    dtfNumberFormatInfo           * sToCharFormatInfo = NULL;
    const dtfNumberFormatKeyWord  * sKeyWord          = NULL;
    stlChar                       * sFormatString     = NULL;
    stlChar                       * sFormatStringEnd  = NULL;
    
    /**
     * format string 길이 검사 & 초기화 
     */

    /* format string의 길이 검사 */
    STL_TRY_THROW( aFormatStringLen <= DTF_NLS_NUMBER_FORMAT_STRING_MAX_SIZE,
                   ERROR_NUMBER_FORMAT_LENGTH );
    
    /**
     *  dtfNumberFormatInfo 공간 확보
     */
    
    STL_DASSERT( aToCharFormatInfoBufferSize >= STL_SIZEOF( dtfNumberFormatInfo ) );
    
    sToCharFormatInfo = (dtfNumberFormatInfo *)aToCharFormatInfoBuffer;
    
    DTF_INIT_NUMBER_FORMAT_INFO( sToCharFormatInfo );
    
    /**
     * format string 정보 복사
     */
    
    stlMemcpy( sToCharFormatInfo->mStr, aFormatString, aFormatStringLen );
    sToCharFormatInfo->mStrLen = aFormatStringLen;
    
    sFormatString     = sToCharFormatInfo->mStr;
    sFormatStringEnd  = sToCharFormatInfo->mStr + sToCharFormatInfo->mStrLen;
    
    /**
     * format key word 를 찾아 정보를 구축한다.
     */

    while( sFormatString < sFormatStringEnd )
    {
        sKeyWord = dtfGetNumberFormatKeyWord( sFormatString );
        
        if( sKeyWord != NULL )
        {
            STL_TRY( dtfSetNumberFormatInfo( sFormatString,
                                             sKeyWord,
                                             sToCharFormatInfo,
                                             aVectorFunc,
                                             aVectorArg,
                                             aErrorStack )
                     == STL_SUCCESS );

            sFormatString = sFormatString + sKeyWord->mLen;
        }
        else
        {
            STL_THROW( ERROR_INVALID_NUMBER_FORMAT );
        }
    }

    /**
     * 분석된 format info로 부가적인 정보 설정.
     */

    /* mDigitCntAfterDecimal(소수점이후의 숫자갯수) 의 홀수/짝수 여부 */
    
    if( (sToCharFormatInfo->mDigitCntAfterDecimal -( sToCharFormatInfo->mDigitCntAfterDecimal / 2 * 2 )) == 0 )
    {
        sToCharFormatInfo->mIsOddDigitCntAfterDecimal = STL_FALSE;
    }
    else
    {
        sToCharFormatInfo->mIsOddDigitCntAfterDecimal = STL_TRUE;
    }
    
    /* mDigitCntAfterV(V 이후의 숫자갯수) 의 홀수/짝수 여부 */
    if( DTF_NUMBER_FMT_FLAG_IS_V( sToCharFormatInfo ) == STL_TRUE )
    {
        if( sToCharFormatInfo->mDigitCntAfterV > 0 )
        {
            if( (sToCharFormatInfo->mDigitCntAfterV - ( sToCharFormatInfo->mDigitCntAfterV / 2 * 2 )) == 0 )
            {
                sToCharFormatInfo->mIsOddDigitCntAfterV = STL_FALSE;
            }
            else
            {
                sToCharFormatInfo->mIsOddDigitCntAfterV = STL_TRUE;
            }
        }
        else
        {
            /* V flag를 지운다.
             * 예) TO_CHAR( 0, '9V' ) */
            sToCharFormatInfo->mFlag &= ~DTF_NUMBER_FMT_FLAG_V;
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    
    /**
     * 분석된 format info로 to_char의 최종 result 결과 길이를 구한다.
     *
     * 부호길이 + 소수점 이전 숫자 길이 + 소수점 이후 숫자 길이 + comma 갯수 + decimal point  + '$' 
     */
    
    if( DTF_NUMBER_FMT_FLAG_IS_EEEE( sToCharFormatInfo ) == STL_TRUE )
    {
        DTF_SET_NUMBER_EEEE_TO_CHAR_RESULT_LEN( sToCharFormatInfo );
    }
    else if( DTF_NUMBER_FMT_FLAG_IS_RN( sToCharFormatInfo ) == STL_TRUE )
    {
        /* RN, rn 은 변환후 남은 왼쪽 공간은 공백으로 채운다.
         * 고정길이 사용 ( 15 ) */
        sToCharFormatInfo->mToCharResultLen = DTF_NUMBER_TO_CHAR_RESULT_ROMAN_NUMERAL_STRING_SIZE;
    }
    else if( DTF_NUMBER_FMT_FLAG_IS_X( sToCharFormatInfo ) == STL_TRUE )
    {
        sToCharFormatInfo->mToCharResultLen =
            1 + sToCharFormatInfo->mDigitCntBeforeDecimal + sToCharFormatInfo->mXCnt;

        STL_TRY_THROW( sToCharFormatInfo->mToCharResultLen <= DTF_NLS_NUMBER_FORMAT_STRING_MAX_SIZE,
                       ERROR_NUMBER_X_FORMAT_LENGTH );
    }
    else
    {
        DTF_SET_NUMBER_TO_CHAR_RESULT_LEN( sToCharFormatInfo );
    }
    
    /**
     * 분석된 format info로 to_char의 실행 함수 정보를 구한다.
     */
    
    if( DTF_NUMBER_FMT_FLAG_IS_EEEE( sToCharFormatInfo ) == STL_TRUE )
    {
        sToCharFormatInfo->mFormatFunc = dtfToCharEEEEFormatFuncList[ aDataType ];
    }
    else if( DTF_NUMBER_FMT_FLAG_IS_V( sToCharFormatInfo ) == STL_TRUE )
    {
        sToCharFormatInfo->mFormatFunc = dtfToCharVFormatFuncList[ aDataType ];
    }
    else if( DTF_NUMBER_FMT_FLAG_IS_RN( sToCharFormatInfo ) == STL_TRUE )
    {
        sToCharFormatInfo->mFormatFunc = dtfToCharRNFormatFuncList[ aDataType ];
    }
    else if( DTF_NUMBER_FMT_FLAG_IS_X( sToCharFormatInfo ) == STL_TRUE )
    {
        sToCharFormatInfo->mFormatFunc = dtfToCharXFormatFuncList[ aDataType ];
    }
    else
    {
        sToCharFormatInfo->mFormatFunc = dtfToCharFormatFuncList[ aDataType ];
    }
    
    STL_DASSERT( sToCharFormatInfo->mFormatFunc != NULL );
    
   
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NUMBER_FORMAT_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMBER_FORMAT_LENGTH,
                      NULL,
                      aErrorStack,
                      DTF_NLS_NUMBER_FORMAT_STRING_MAX_SIZE );
    }

    STL_CATCH( ERROR_NUMBER_X_FORMAT_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMBER_FORMAT_LENGTH,
                      NULL,
                      aErrorStack,
                      DTF_NLS_NUMBER_FORMAT_STRING_MAX_SIZE - 1 );
    }    
    
    STL_CATCH( ERROR_INVALID_NUMBER_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_NUMBER_FORMAT_MODEL,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;      
}

/**
 * @brief Number format에 대한 dtfNumberFormatInfo 정보를 구축한다.
 * @param[in]  aFormatStr          format string에서 현재 format element의 위치 
 * @param[in]  aKeyWord            format string에 대한 단위 keyword 정보
 * @param[out] aNumberFormatInfo   dtfNumberFormatInfo
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfSetNumberFormatInfo( stlChar                       * aFormatStr,
                                  const dtfNumberFormatKeyWord  * aKeyWord,
                                  dtfNumberFormatInfo           * aNumberFormatInfo,
                                  dtlFuncVector                 * aVectorFunc,
                                  void                          * aVectorArg,
                                  stlErrorStack                 * aErrorStack )
{
    stlChar sErrStr[10] = {0};

    switch( aKeyWord->mId )
    {
        case DTF_NUMBER_FMT_9:
            {
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_X( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                if( ( aNumberFormatInfo->mFirstZeroOrNine == NULL ) &&
                    ( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aNumberFormatInfo ) == STL_FALSE )
                    &&
                    ( DTF_NUMBER_FMT_FLAG_IS_V( aNumberFormatInfo ) == STL_FALSE ) )
                {
                    aNumberFormatInfo->mFirstZeroOrNine = aFormatStr;
                }
                else
                {
                    /* Do Nothing */
                }
                
                if( DTF_NUMBER_FMT_FLAG_IS_V( aNumberFormatInfo ) == STL_TRUE )
                {
                    (aNumberFormatInfo->mDigitCntAfterV)++;
                    (aNumberFormatInfo->mDigitCntBeforeDecimal)++;                    
                }
                else
                {                
                    if( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aNumberFormatInfo ) == STL_TRUE )
                    {
                        (aNumberFormatInfo->mDigitCntAfterDecimal)++;
                    }
                    else
                    {
                        (aNumberFormatInfo->mDigitCntBeforeDecimal)++;
                    }
                }
                
                break;
            }
        case DTF_NUMBER_FMT_0:
            {
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_X( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );
                
                if( ( aNumberFormatInfo->mFirstZeroOrNine == NULL ) &&
                    ( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aNumberFormatInfo ) == STL_FALSE ) &&
                    ( DTF_NUMBER_FMT_FLAG_IS_V( aNumberFormatInfo ) == STL_FALSE ) )
                {
                    aNumberFormatInfo->mFirstZeroOrNine = aFormatStr;
                }
                else
                {
                    /* Do Nothing */
                }
                
                if( ( DTF_NUMBER_FMT_FLAG_IS_ZERO( aNumberFormatInfo ) == STL_FALSE ) &&
                    ( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aNumberFormatInfo ) == STL_FALSE ) )
                {
                    aNumberFormatInfo->mFlag |= DTF_NUMBER_FMT_FLAG_ZERO;
                    aNumberFormatInfo->mZeroStart = aNumberFormatInfo->mDigitCntBeforeDecimal + 1;
                }
                else
                {
                    /* Do Nothing */
                }
                
                if( DTF_NUMBER_FMT_FLAG_IS_V( aNumberFormatInfo ) == STL_TRUE )
                {
                    (aNumberFormatInfo->mDigitCntAfterV)++;
                    (aNumberFormatInfo->mDigitCntBeforeDecimal)++;                    
                }
                else
                {                                
                    if( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aNumberFormatInfo ) == STL_TRUE )
                    {
                        (aNumberFormatInfo->mDigitCntAfterDecimal)++;
                        (aNumberFormatInfo->mZeroEndAfterDecimal) = (aNumberFormatInfo->mDigitCntAfterDecimal);
                    }
                    else
                    {
                        (aNumberFormatInfo->mDigitCntBeforeDecimal)++;
                    }
                }
                
                break;
            }
        case DTF_NUMBER_FMT_PERIOD:
            {
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_NUMBER_FORMAT_APPEAR_TWICE );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_V( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT_V_AND_PERIOD );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_X( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                aNumberFormatInfo->mFlag |= DTF_NUMBER_FMT_FLAG_PERIOD;
                
                break;
            }
        case DTF_NUMBER_FMT_COMMA:
            {
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_V( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_X( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( aNumberFormatInfo->mDigitCntBeforeDecimal > 0,
                               ERROR_INVALID_NUMBER_FORMAT );

                (aNumberFormatInfo->mCommaCnt)++;
                
                break;
            }
        case DTF_NUMBER_FMT_B:
        case DTF_NUMBER_FMT_b:
            {
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_BLANK( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_X( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                aNumberFormatInfo->mFlag |= DTF_NUMBER_FMT_FLAG_BLANK;
                
                break;
            }
        case DTF_NUMBER_FMT_S:
        case DTF_NUMBER_FMT_s:
            {
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_SIGN( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_NUMBER_FORMAT_APPEAR_TWICE );
                
                STL_TRY_THROW( ( DTF_NUMBER_FMT_FLAG_IS_MINUS( aNumberFormatInfo ) == STL_FALSE ) &&
                               ( DTF_NUMBER_FMT_FLAG_IS_BRACKET( aNumberFormatInfo ) == STL_FALSE ),
                               ERROR_INVALID_NUMBER_FORMAT_S_AND_MI_PR );
                
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_X( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );
                
                if( ( ( DTF_NUMBER_FMT_FLAG_IS_FM( aNumberFormatInfo ) == STL_FALSE ) &&
                      ( aFormatStr == aNumberFormatInfo->mStr ) ) ||
                    ( ( DTF_NUMBER_FMT_FLAG_IS_FM( aNumberFormatInfo ) == STL_TRUE ) &&
                      ( aFormatStr == ( aNumberFormatInfo->mStr + 2 ) ) ) )
                {
                    aNumberFormatInfo->mSignLocate = DTF_NUMBER_SIGN_LOCATE_PRE;
                }
                else if( aFormatStr == (aNumberFormatInfo->mStr + aNumberFormatInfo->mStrLen - 1) )
                {
                    aNumberFormatInfo->mSignLocate = DTF_NUMBER_SIGN_LOCATE_POST;
                }
                else
                {
                    STL_THROW( ERROR_INVALID_NUMBER_FORMAT );
                }
                
                aNumberFormatInfo->mFlag |= DTF_NUMBER_FMT_FLAG_SIGN;
                
                break;
            }
        case DTF_NUMBER_FMT_MI:
        case DTF_NUMBER_FMT_mi:
            {
                STL_TRY_THROW( aFormatStr == (aNumberFormatInfo->mStr + aNumberFormatInfo->mStrLen - 2),
                               ERROR_INVALID_NUMBER_FORMAT );
                
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_MINUS( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_NUMBER_FORMAT_APPEAR_TWICE );

                STL_TRY_THROW( ( DTF_NUMBER_FMT_FLAG_IS_SIGN( aNumberFormatInfo ) == STL_FALSE ) &&
                               ( DTF_NUMBER_FMT_FLAG_IS_BRACKET( aNumberFormatInfo ) == STL_FALSE ),
                               ERROR_INVALID_NUMBER_FORMAT_MI_AND_S_PR );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_X( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                aNumberFormatInfo->mFlag |= DTF_NUMBER_FMT_FLAG_MINUS;
                aNumberFormatInfo->mSignLocate = DTF_NUMBER_SIGN_LOCATE_POST;
                    
                break;
            }
        case DTF_NUMBER_FMT_PR:
        case DTF_NUMBER_FMT_pr:
            {
                STL_TRY_THROW( aFormatStr == (aNumberFormatInfo->mStr + aNumberFormatInfo->mStrLen - 2),
                               ERROR_INVALID_NUMBER_FORMAT );
                
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_BRACKET( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_NUMBER_FORMAT_APPEAR_TWICE );

                STL_TRY_THROW( ( DTF_NUMBER_FMT_FLAG_IS_SIGN( aNumberFormatInfo ) == STL_FALSE ) &&
                               ( DTF_NUMBER_FMT_FLAG_IS_MINUS( aNumberFormatInfo ) == STL_FALSE ),
                               ERROR_INVALID_NUMBER_FORMAT_PR_AND_S_MI );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_X( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                aNumberFormatInfo->mFlag |= DTF_NUMBER_FMT_FLAG_BRACKET;

                break;
            }
        case DTF_NUMBER_FMT_DOLLAR:
            {
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_DOLLAR( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_NUMBER_FORMAT_APPEAR_TWICE );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_X( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                aNumberFormatInfo->mFlag |= DTF_NUMBER_FMT_FLAG_DOLLAR;

                break;
            }
        case DTF_NUMBER_FMT_EEEE:
        case DTF_NUMBER_FMT_eeee:
            {
                STL_TRY_THROW( aNumberFormatInfo->mCommaCnt == 0, ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( ( aNumberFormatInfo->mDigitCntBeforeDecimal - aNumberFormatInfo->mDigitCntAfterV ) > 0,
                               ERROR_INVALID_NUMBER_FORMAT );
                
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_EEEE( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_NUMBER_FORMAT_APPEAR_TWICE );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_X( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                /* EEEE는 format의 맨 마지막에 오거나
                 * S, MI, PR 앞에 올 수 있다.
                 * (S, MI, PR은 format의 맨 마지막에 와야 한다.) */
                if( ( aFormatStr == (aNumberFormatInfo->mStr + aNumberFormatInfo->mStrLen - 4) )
                    ||
                    ( ( aFormatStr == (aNumberFormatInfo->mStr + aNumberFormatInfo->mStrLen - 5) ) &&
                      ( stlStrncasecmp( (aNumberFormatInfo->mStr + aNumberFormatInfo->mStrLen - 1),
                                        "S",
                                        1 ) == 0 ) )
                    ||
                    ( ( aFormatStr == (aNumberFormatInfo->mStr + aNumberFormatInfo->mStrLen - 6) ) &&
                      ( stlStrncasecmp( (aNumberFormatInfo->mStr + aNumberFormatInfo->mStrLen - 2),
                                        "MI",
                                        2 ) == 0 ) )
                    ||
                    ( ( aFormatStr == (aNumberFormatInfo->mStr + aNumberFormatInfo->mStrLen - 6) ) &&
                      ( stlStrncasecmp( (aNumberFormatInfo->mStr + aNumberFormatInfo->mStrLen - 2),
                                        "PR",
                                        2 ) == 0 ) ) )
                {
                    /* Do Nothing */
                }
                else
                {
                    STL_THROW( ERROR_INVALID_NUMBER_FORMAT );
                }
                
                aNumberFormatInfo->mFlag |= DTF_NUMBER_FMT_FLAG_EEEE;
                
                break;
            }
        case DTF_NUMBER_FMT_V:
        case DTF_NUMBER_FMT_v:
            {
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_V( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_NUMBER_FORMAT_APPEAR_TWICE );
                
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT_V_AND_PERIOD );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_X( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                aNumberFormatInfo->mFlag |= DTF_NUMBER_FMT_FLAG_V;
                
                break;
            }
        case DTF_NUMBER_FMT_RN:
        case DTF_NUMBER_FMT_rn:
            {
                /* RN, rn 은 FM 포맷과만 쓰일 수 있다. */
                
                if( DTF_NUMBER_FMT_FLAG_IS_FM( aNumberFormatInfo ) == STL_FALSE )
                {
                    STL_TRY_THROW( aKeyWord->mLen == aNumberFormatInfo->mStrLen,
                                   ERROR_INVALID_NUMBER_FORMAT );

                    if( aNumberFormatInfo->mStr[0] == 'R' )
                    {
                        aNumberFormatInfo->mLowerUpperIdx = DTF_NUMBER_FMT_UPPER_IDX;
                    }
                    else
                    {
                        aNumberFormatInfo->mLowerUpperIdx = DTF_NUMBER_FMT_LOWER_IDX;
                    }
                }
                else
                {
                    STL_DASSERT( DTF_NUMBER_FMT_FLAG_IS_FM( aNumberFormatInfo ) == STL_TRUE );
                                 
                    STL_TRY_THROW( ( aNumberFormatInfo->mStrLen == 4 ) &&
                                   ( stlStrncasecmp( (aNumberFormatInfo->mStr + aNumberFormatInfo->mStrLen - 2),
                                                     aKeyWord->mName,
                                                     2 ) == 0 ),
                                   ERROR_INVALID_NUMBER_FORMAT );
                    
                    if( *( aNumberFormatInfo->mStr + aNumberFormatInfo->mStrLen - 2 ) == 'R' )
                    {
                        aNumberFormatInfo->mLowerUpperIdx = DTF_NUMBER_FMT_UPPER_IDX;
                    }
                    else
                    {
                        aNumberFormatInfo->mLowerUpperIdx = DTF_NUMBER_FMT_LOWER_IDX;
                    }
                }
                
                aNumberFormatInfo->mFlag |= DTF_NUMBER_FMT_FLAG_RN;

                break;
            }
        case DTF_NUMBER_FMT_x:
            {
                /* 대문자는 이미 초기화 되어 있어, 소문자만 표시한다. */
                aNumberFormatInfo->mLowerUpperIdx = DTF_NUMBER_FMT_LOWER_IDX;
            }
        case DTF_NUMBER_FMT_X:
            {
                if( aNumberFormatInfo->mZeroStart == 0 )
                {
                    STL_TRY_THROW( aNumberFormatInfo->mDigitCntBeforeDecimal == 0,
                                   ERROR_INVALID_NUMBER_FORMAT );
                }
                
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );
                
                STL_TRY_THROW( aNumberFormatInfo->mCommaCnt == 0,
                               ERROR_INVALID_NUMBER_FORMAT );
                
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_BLANK( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );
                
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_SIGN( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );
                
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_MINUS( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_BRACKET( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_DOLLAR( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_EEEE( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_V( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_RN( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                if( DTF_NUMBER_FMT_FLAG_IS_X( aNumberFormatInfo ) == STL_FALSE )
                {
                    aNumberFormatInfo->mFlag |= DTF_NUMBER_FMT_FLAG_X;
                }

                aNumberFormatInfo->mXCnt++;

                break;
            }
        case DTF_NUMBER_FMT_FM:
        case DTF_NUMBER_FMT_fm:
            {
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_FM( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );
                
                STL_TRY_THROW( aNumberFormatInfo->mFirstZeroOrNine == NULL,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( aNumberFormatInfo->mCommaCnt == 0,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_BLANK( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_MINUS( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_BRACKET( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_DOLLAR( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_EEEE( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_V( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );

                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_RN( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );
                
                STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_X( aNumberFormatInfo ) == STL_FALSE,
                               ERROR_INVALID_NUMBER_FORMAT );
                
                aNumberFormatInfo->mFlag |= DTF_NUMBER_FMT_FLAG_FM;
                
                break;
            }
        default:
            STL_DASSERT( STL_FALSE );
            break;
    }
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_NUMBER_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_NUMBER_FORMAT_MODEL,
                      NULL,
                      aErrorStack );
    }    
    
    STL_CATCH( ERROR_NUMBER_FORMAT_APPEAR_TWICE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_FORMAT_CODE_APPEARS_TWICE,
                      NULL,
                      aErrorStack );
    }
   
    STL_CATCH( ERROR_INVALID_NUMBER_FORMAT_S_AND_MI_PR )
    {
        stlSnprintf( sErrStr,
                     STL_SIZEOF(sErrStr),
                     "%s ",
                     DTF_NUMBER_FMT_MI_STR );
        stlSnprintf( sErrStr + stlStrlen(sErrStr),
                     STL_SIZEOF(sErrStr) - stlStrlen(sErrStr),
                     "%s",
                     DTF_NUMBER_FMT_PR_STR );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMBER_FORMAT_CANNOT_USE_TOGETHER,
                      NULL,
                      aErrorStack,
                      (stlChar *)DTF_NUMBER_FMT_SIGN_STR,
                      sErrStr );
    }

    STL_CATCH( ERROR_INVALID_NUMBER_FORMAT_MI_AND_S_PR )
    {
        stlSnprintf( sErrStr,
                     STL_SIZEOF(sErrStr),
                     "%s ",
                     DTF_NUMBER_FMT_SIGN_STR );
        stlSnprintf( sErrStr + stlStrlen(sErrStr),
                     STL_SIZEOF(sErrStr) - stlStrlen(sErrStr),
                     "%s",
                     DTF_NUMBER_FMT_PR_STR );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMBER_FORMAT_CANNOT_USE_TOGETHER,
                      NULL,
                      aErrorStack,
                      (stlChar *)DTF_NUMBER_FMT_MI_STR,
                      sErrStr );
    }

    STL_CATCH( ERROR_INVALID_NUMBER_FORMAT_PR_AND_S_MI )
    {
        stlSnprintf( sErrStr,
                     STL_SIZEOF(sErrStr),
                     "%s ",
                     DTF_NUMBER_FMT_SIGN_STR );
        stlSnprintf( sErrStr + stlStrlen(sErrStr),
                     STL_SIZEOF(sErrStr) - stlStrlen(sErrStr),
                     "%s",
                     DTF_NUMBER_FMT_MI_STR );
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMBER_FORMAT_CANNOT_USE_TOGETHER,
                      NULL,
                      aErrorStack,
                      (stlChar *)DTF_NUMBER_FMT_PR_STR,
                      sErrStr );
    }


    STL_CATCH( ERROR_INVALID_NUMBER_FORMAT_V_AND_PERIOD )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMBER_FORMAT_CANNOT_USE_TOGETHER,
                      NULL,
                      aErrorStack,
                      (stlChar *)DTF_NUMBER_FMT_V_STR,
                      (stlChar *)DTF_NUMBER_FMT_PERIOD_STR );
    }           
    
    
    STL_FINISH;
    
    return STL_FAILURE;
}
    

/**
 * @brief number type을 V, EEEE, RN, rn이 아닌 format string에 맞게 string으로 변환한다.
 * @param[in]  aDataValue          dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharNumberToFormatString( dtlDataValue         * aDataValue,
                                         dtfNumberFormatInfo  * aFormatInfo,
                                         stlInt64               aAvailableSize,
                                         stlChar              * aResult,
                                         stlInt64             * aLength,
                                         dtlFuncVector        * aVectorFunc,
                                         void                 * aVectorArg,
                                         stlErrorStack        * aErrorStack )
{
    stlInt32           i = 0;
    stlInt32           j = 0;
    stlInt32           sExponent      = 0;
    stlInt32           sDigitLen      = 0; 
    stlInt32           sZeroLen       = 0;
    stlInt32           sDigitCntBeforeDecimal   = 0;
    stlInt32           sDigitCntAfterDecimal    = 0;
    stlInt32           sTotalDigitCnt = 0;
    stlInt32           sOrgDigitCntAfterDecimal = 0;
    stlInt32           sIsPositive    = STL_FALSE;
    stlInt32           sResultLen     = aFormatInfo->mToCharResultLen;
    stlInt32           sTempLen       = 0;
    
    dtlNumericType   * sNumericType   = NULL;
    stlUInt8         * sDigit         = NULL;
    stlChar          * sDigitString   = NULL;
    stlChar          * sResult        = aResult;

    stlBool            sNumericIsZero             = STL_FALSE;
    stlBool            sPlusOneDigitForRound      = STL_FALSE;
    
    stlBool            sIsSetBlank    = STL_FALSE;
    stlInt32           sExtraDigitLen = 0;
    stlInt32           sExtraCommaLen = 0;
    stlInt32           sExtraZeroLen  = 0;
    stlInt32           sExtraSpaceLen = 0;    
    stlInt32           sRoundDigit    = 0;
    stlInt32           sTempDigitCntAfterDecimal = 0;
    stlChar          * sTmpStr        = NULL;
    stlChar          * sFormatStrEnd  = aFormatInfo->mStr + aFormatInfo->mStrLen;
    
    stlUInt8           sCarry         = 0;
    stlBool            sIsOdd         = STL_FALSE;
    stlBool            sIsAfterDecimalPos  = STL_TRUE;
    stlInt32           sTrimDigit     = 0;    
    
    
    sNumericType = DTF_NUMERIC( aDataValue );
    sIsPositive = DTL_NUMERIC_IS_POSITIVE( sNumericType );
    
    if( DTL_NUMERIC_IS_ZERO( sNumericType, aDataValue->mLength ) == STL_TRUE )
    {
        sNumericIsZero         = STL_TRUE;
        sDigitCntAfterDecimal  = aFormatInfo->mDigitCntAfterDecimal;

        if( ( (aFormatInfo->mDigitCntBeforeDecimal > 0) && (sDigitCntAfterDecimal == 0 ) ) ||
            ( aFormatInfo->mZeroStart > 0 ) )
        {
            /*
             * 예) TO_CHAR( 0, '9' )     => ' 0'
             *     TO_CHAR( 0, '0' )     => ' 0'
             *     TO_CHAR( 0, '0.999' ) => ' 0.000'
             */     
            sDigitCntBeforeDecimal = 1;
        }
        else
        {
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                /*
                 * 예) TO_CHAR( 0, '.999' )  => ' .000'
                 *     TO_CHAR( 0, '9.9' )   => '  .0'
                 */
                sDigitCntBeforeDecimal = 0;
            }
            else
            {
                if( sDigitCntAfterDecimal == 0 )
                {
                    /*
                     * 예) TO_CHAR( 0, 'FM' ) => null
                     */
                    sDigitCntBeforeDecimal = 0;
                }
                else
                {
                    if( aFormatInfo->mDigitCntBeforeDecimal > 0 )
                    {
                        if( aFormatInfo->mZeroEndAfterDecimal > 0 )
                        {
                            /* 예) TO_CHAR( 0, 'FM9.0' ) => '.0' */
                            sDigitCntBeforeDecimal = 0;
                        }
                        else
                        {
                            /* 예) TO_CHAR( 0, 'FM9.9' ) => '0.' */
                            sDigitCntBeforeDecimal = 1;
                        }
                    }
                    else
                    {
                        /* aFormatInfo->mDigitCntBeforeDecimal == 0 */

                        /*
                         * 예) TO_CHAR( 0, 'FM.0' ) => '.0'
                         *     TO_CHAR( 0, 'FM.9' ) => '.'
                         */ 
                        sDigitCntBeforeDecimal = 0;
                    }
                }
            }
        }
    }
    else
    {
        sDigitLen = DTL_NUMERIC_GET_DIGIT_COUNT( aDataValue->mLength );
        sExponent = DTL_NUMERIC_GET_EXPONENT( sNumericType );
        sDigit    = & sNumericType->mData[1];

        sDigitCntAfterDecimal = aFormatInfo->mDigitCntAfterDecimal;

        if( sExponent < 0 )
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : -2
             * String : .0001234
             */
            
            sOrgDigitCntAfterDecimal =
                ( ( ( sDigitLen - sExponent - 1 ) << 1 ) -
                  ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen - 1 ] )[ 1 ] == '0' ) );

            if( ( aFormatInfo->mDigitCntBeforeDecimal > 0 ) &&
                ( sDigitCntAfterDecimal == 0 ) )
            {
                sDigitCntBeforeDecimal = 1;
            }
            else
            {
                sDigitCntBeforeDecimal = 0;
            }

            sZeroLen = -((sExponent + 1) << 1);

            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                /* Do Nothing */
            }
            else
            {
                if( sZeroLen > 0 )
                {
                    if( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) )
                    {
                        sZeroLen++;
                    }
                    else
                    {
                        /* Do Nothing */
                    }

                    sTrimDigit = ( sDigitCntAfterDecimal - aFormatInfo->mZeroEndAfterDecimal );
                    if( sZeroLen >= sTrimDigit )
                    {
                        if( sZeroLen == sTrimDigit )
                        {
                            if( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) )
                            {
                                if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ 0 ] )[ 1 ] > '4' )
                                {
                                    sCarry = 1;
                                }
                                else
                                {
                                    sCarry = 0;
                                }
                            }
                            else
                            {
                                if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ 0 ] )[ 0 ] > '4' )
                                {
                                    sCarry = 1;                        
                                }
                                else
                                {
                                    sCarry = 0;
                                }
                            }
                        }
                        
                        if( ( aFormatInfo->mDigitCntBeforeDecimal > 0 ) &&
                            ( aFormatInfo->mZeroEndAfterDecimal == 0 ) )
                        {
                            if( sCarry == 0 )
                            {
                                /*
                                 * 예) TO_CHAR( 0.0000000123, 'FM9999999999.9999999' ) => '0.'
                                 *     TO_CHAR( 0.0000000123, 'FM0.9999999' ) => '0.'
                                 */
                                sDigitCntBeforeDecimal = 1;
                            }
                            else
                            {
                                /*
                                 * 예) TO_CHAR( 0.000099, 'FM99.9999' ) => .0001
                                 */
                                
                                /* Do Nothing */
                            }
                        }
                        else
                        {
                            /*
                             * 예) TO_CHAR( 0.0000000123, 'FM9999999999.0000000' ) => '.0000000'
                             *     TO_CHAR( 0.0000000123, 'FM.9999999' ) => '.'
                             *     TO_CHAR( 0.0000000123, 'FM.0000000' ) => '.0000000'
                             */
                            
                            /* Do Nothing */
                        }
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
            }
            
            if( sOrgDigitCntAfterDecimal > sDigitCntAfterDecimal )
            {
                /* 반올림되어 정수부자리가 늘어나는지 여부 체크 */
                
                /* 반올림할 자리를 찾는다. */
                STL_TRY_THROW( sZeroLen == 0, RAMP_MINUS_EXP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );

                i = sDigitCntAfterDecimal >> 1;
                STL_DASSERT( ( i >= 0 ) && ( i < sDigitLen ) );
                    
                j = ( aFormatInfo->mIsOddDigitCntAfterDecimal == STL_TRUE ) ? 1 : 0;
                
                if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[j] > '4' )
                {
                    if( aFormatInfo->mIsOddDigitCntAfterDecimal == STL_TRUE )
                    {
                        if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '9' )
                        {
                            i--;
                        }
                        else
                        {
                            STL_THROW( RAMP_MINUS_EXP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
                        }
                    }
                    else
                    {
                        i--;
                    }

                    for( ; i >= 0; i-- )
                    {
                        if( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '9' ) &&
                            ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[1] == '9' ) )
                        {
                            continue;
                        }
                        else
                        {
                            STL_THROW( RAMP_MINUS_EXP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
                        }                            
                    }
                    
                    STL_DASSERT( i < 0 );
                    
                    sDigitCntBeforeDecimal = 1;
                    sPlusOneDigitForRound = STL_TRUE;
                }
                else
                {
                    sPlusOneDigitForRound = STL_FALSE;
                }

                STL_RAMP( RAMP_MINUS_EXP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
            }
            else
            {
                /* Do Nothing */
            }
        }
        else if( sDigitLen > (sExponent + 1) )
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : 1
             * String : 123.4
             */

            sTotalDigitCnt =
                ( ( sDigitLen << 1 ) -
                  ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[0] == '0' ? 1 : 0 ) -
                  ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[sDigitLen - 1] )[1] == '0' ? 1 : 0 ) );
            
            sDigitCntBeforeDecimal = ( sExponent << 1 ) +
                ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[0] == '0' ? 1 : 2 );
            
            sOrgDigitCntAfterDecimal = sTotalDigitCnt - sDigitCntBeforeDecimal;
            
            if( sOrgDigitCntAfterDecimal > sDigitCntAfterDecimal )
            {
                /* 반올림되어 정수부자리가 늘어나는지 여부 체크 */
                
                /*
                 * 예) TO_CHAR(  99.999, '999.99' )
                 *     sExponent = 0
                 *     sDigitLen = 3  ( 99 | 99 | 90 )
                 *     sDigit[2][0]에서 반올림되어야 함.
                 *
                 * 예) TO_CHAR(  99.9999, '999.999' )
                 *     sExponent = 0
                 *     sDigitLen = 3  ( 99 | 99 | 99 )
                 *     sDigit[2][1]에서 반올림되어야 함.
                 */ 
                
                i = sExponent + 1 + ( sDigitCntAfterDecimal >> 1 );
                STL_DASSERT( ( i >= 0 ) && ( i < sDigitLen ) );
                
                j = ( aFormatInfo->mIsOddDigitCntAfterDecimal == STL_TRUE ) ? 1 : 0;

                if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[j] > '4' )
                {
                    if( aFormatInfo->mIsOddDigitCntAfterDecimal == STL_TRUE )
                    {
                        if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '9' )
                        {
                            i--;
                        }
                        else
                        {
                            STL_THROW( RAMP_PLUS_EXP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
                        }
                    }
                    else
                    {
                        i--;
                    }
                    
                    for( ; i > 0; i-- )
                    {
                        if( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '9' ) &&
                            ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[1] == '9' ) )
                        {
                            continue;
                        }
                        else
                        {
                            STL_THROW( RAMP_PLUS_EXP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
                        }
                    }
                    
                    STL_DASSERT( i == 0 );
                    
                    if( ( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '0' ) &&
                          ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[1] == '9' ) )
                        ||
                        ( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '9' ) &&
                          ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[1] == '9' ) ) )
                    {
                        sDigitCntBeforeDecimal++;
                        sPlusOneDigitForRound = STL_TRUE;
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

                STL_RAMP( RAMP_PLUS_EXP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
            }
            else
            {
                /* Do Nothing */
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
            sDigitCntBeforeDecimal = ( ( sZeroLen + ( sDigitLen << 1 ) ) -
                                       ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) );
        }
    }
    
    
    /**
     * 'B' format 처리
     */
    
    if( DTF_NUMBER_FMT_FLAG_IS_BLANK( aFormatInfo ) == STL_TRUE )
    {
        if( sNumericIsZero == STL_TRUE )
        {
            /* 예) TO_CHAR( 0, 'B' ) */
            sIsSetBlank = STL_TRUE;
        }
        else if( sExponent < 0 )
        {
            /* 예) TO_CHAR( 0.1, 'B' )
             *     TO_CHAR( 0.00001, 'B.99' ) */
            
            sZeroLen = -((sExponent + 1) << 1);
            
            if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' )
            {
                sZeroLen++;
            }
            else
            {
                /* Do Nothing */
            }
            
            if( ( sPlusOneDigitForRound == STL_FALSE ) &&
                ( sDigitCntAfterDecimal <= sZeroLen ) )
            {
                if( sDigitCntAfterDecimal < sZeroLen )
                {
                    sIsSetBlank = STL_TRUE;
                }
                else
                {
                    /* ( sDigitCntAfterDecimal == sZeroLen ) */
                    if( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) )
                    {
                        if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ 0 ] )[ 1 ] < '5' )
                        {
                            sIsSetBlank = STL_TRUE;
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    }
                    else
                    {
                        if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ 0 ] )[ 0 ] < '5' )
                        {
                            sIsSetBlank = STL_TRUE;
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    }                    
                }
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
        
        if( sIsSetBlank == STL_TRUE )
        {
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                sTempLen = sResultLen;
                while( sTempLen > 0 )
                {
                    *sResult = ' ';
                    sResult++;
                    sTempLen--;
                }
            }
            else
            {
                sResultLen = 0;
            }
            
            STL_THROW( RAMP_FUNCTION_END );
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
    

    /**
     * 정수부분의 digit format 숫자보다 실제 value의 유효숫자 갯수가 많은 경우 '#' 표기.
     */
    
    if( sDigitCntBeforeDecimal > aFormatInfo->mDigitCntBeforeDecimal )
    {
        /* 예: TO_CHAR( 12345, '999' ) => '####'  */

        sTempLen = sResultLen;
        while( sTempLen > 0 )
        {
            *sResult = '#';
            sResult++;
            sTempLen--;
        }
        
        STL_THROW( RAMP_FUNCTION_END );
    }
    else
    {
        /* Do Nothing */
    }
    

    /**
     * 공백 & 부호 표기 & 0인 숫자에 대한 '0' 표기
     */
    
    /**
     * 공백 표기
     */
    
    if( aFormatInfo->mZeroStart == 0 )
    {
        /*
         * '0' format 이 없는 경우
         */
        
        /* 예) TO_CHAR( 12345, '9999999' ) => 'sign__12345' => '   12345' */
        sExtraDigitLen = (aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal);
        sExtraZeroLen = 0;
    }
    else
    {
        /*
         * '0' format 이 있는 경우
         */
        
        if( aFormatInfo->mZeroStart == 1 )
        {
            /* '0' format 이 첫번째에 오는 경우  */
        
            /* 예) TO_CHAR( 12345, '0999999' ) => 'sign0012345' => ' 0012345' */
            sExtraDigitLen = 0;
            sExtraZeroLen = (aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal );
        }
        else // ( aFormatInfo->mZeroStart > 1 )
        {
            /* '0' format 이 두번째 이후에 오는 경우  */
        
            sExtraDigitLen = aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal;
        
            if( sExtraDigitLen >= aFormatInfo->mZeroStart )
            {
                /* 예) TO_CHAR( 123, '9909999' ) => 'sign__00123' => '   00123' */
                sExtraDigitLen = (aFormatInfo->mZeroStart - 1);
                sExtraZeroLen =
                    (aFormatInfo->mDigitCntBeforeDecimal - sExtraDigitLen - sDigitCntBeforeDecimal);
            }
            else
            {
                /* 예) TO_CHAR( 123456, '9909999' ) => 'sign_123456' => '  123456' */
                sExtraDigitLen = (aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal);
                sExtraZeroLen = 0;
            }
        }
    }
    
    if( aFormatInfo->mFirstZeroOrNine != NULL )
    {
        sTmpStr = aFormatInfo->mFirstZeroOrNine;
    }
    else
    {
        sTmpStr = aFormatInfo->mStr;
        
        /* format string 중 digit 앞에 나올 수 있는 FM, S, $, B 를 제외한 포인터를 저장한다. */        
        
        /*
         * FM , S  skip 
         * ex) to_char( 1, 'fms9' ) or to_char( 1, 'sfm9' )
         */
        DTF_NUMBER_FMT_SKIP_FM_SIGN_NEXT_FORMAT_STR( sTmpStr );
        
        /* B, $ skip*/
        DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
    }

    i = sExtraDigitLen;
    sExtraCommaLen = 0;
    
    while( i > 0 )
    {
        if( (*sTmpStr == '9') || (*sTmpStr == '0' ) )
        {
            i--;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
        else if( *sTmpStr == ',' )
        {
            while( *sTmpStr == ',' )
            {
                sExtraCommaLen++;
                sTmpStr++;
                DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            }
        }
        else
        {
            STL_DASSERT( STL_FALSE );
        }
    }

    if( sExtraDigitLen > 0 )
    {
        while( *sTmpStr == ',' )
        {
            sExtraCommaLen++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    sExtraSpaceLen = sExtraDigitLen + sExtraCommaLen;
    
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        while( sExtraSpaceLen > 0 )
        {
            *sResult = ' ';
            sResult++;
            sExtraSpaceLen--;
        }
    }
    else
    {
        sResultLen -= sExtraSpaceLen;
    }
    
    
    /**
     * 부호 표기
     */
    
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        dtfNumberFmtSetSignForNormal( aFormatInfo, aResult, sResultLen, sIsPositive, sResult, &sResult );
    }
    else
    {
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  sResultLen,
                                  sIsPositive,
                                  STL_TRUE, // aIsSignLocatePre
                                  &sResultLen,
                                  sResult,
                                  &sResult );            
    }
    
    
    /**
     * DOLLAR 표기
     */

    DTF_NUMBER_FMT_SET_DOLLAR( aFormatInfo, sResult );
    
    
    /**
     * 0인 숫자에 대한 '0' 표기
     */
    
    while( sExtraZeroLen > 0 )
    {
        if( ( *sTmpStr == '9' ) || ( *sTmpStr == '0' ) )
        {
            *sResult = '0';

            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            sExtraZeroLen--;
        }
        else if( *sTmpStr == ',' )
        {
            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
        }
        else
        {
            STL_DASSERT( STL_FALSE );
        }        
    }
    
    DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
    
    /**
     * 숫자 표기 ( 0 인 경우 )
     */
    
    if( sNumericIsZero == STL_TRUE )
    {
        if( sDigitCntBeforeDecimal == 1 )
        {
            if( ( *sTmpStr == '0' ) || ( *sTmpStr == '9' ) )
            {
                *sResult = '0';
                sTmpStr++;
            }
            else
            {
                /* Do Nothing */
            }
                
            while( sTmpStr < sFormatStrEnd )
            {
                if( (*sTmpStr == ',') || (*sTmpStr == '.') )
                {
                    sResult++;
                    *sResult = *sTmpStr;
                    sTmpStr++;
                }
                else if( *sTmpStr == '$' )
                {
                    sTmpStr++;
                    continue;
                }
                else
                {
                    STL_DASSERT( ( *sTmpStr != 'B' ) && ( *sTmpStr != 'b' ) );
                    break;
                }
            }
        }
        else
        {
            /* Do Nothing */
        }
        
        if( sDigitCntAfterDecimal > 0 )
        {
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                sZeroLen = sDigitCntAfterDecimal;
            }
            else
            {
                sZeroLen = aFormatInfo->mZeroEndAfterDecimal;
                sResultLen -= ( sDigitCntAfterDecimal - aFormatInfo->mZeroEndAfterDecimal );
            }
            
            STL_DASSERT( ( ( sDigitCntBeforeDecimal == 0 ) && ( *sTmpStr == '.' ) ) ||
                         ( ( sDigitCntBeforeDecimal == 1 ) && ( *sTmpStr != '.' ) ) );
            
            *sResult = '.';
            sResult++;
            
            while( sZeroLen > 0 )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }
        else
        {
            if( sTmpStr < sFormatStrEnd )
            {
                if( *sTmpStr == '.' )
                {
                    *sResult = *sTmpStr;
                    sResult++;
                }
                else
                {
                    sResult++;
                }
            }
            else
            {
                /* Do Nothing */
            }
        }
        
        STL_THROW( RAMP_NUMBER_TO_CHAR_FINISH );
    }
    else
    {
        /* Do Nothing */
    }
    
    
    /**
     * 숫자 표기 ( 0 이 아닌 경우 )
     */
    
    if( sExponent < 0 )
    {
        sRoundDigit = sDigitCntAfterDecimal;
        
        if( sDigitCntBeforeDecimal == 1 )
        {
            if( sPlusOneDigitForRound == STL_TRUE )
            {
                /* 반올림된 숫자가 저장될 장소 skip */
                sResult++;
                sTmpStr++;
                DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            }
            else
            {
                /*
                 * 예) TO_CHAR( 0.1, '9' ) => 0
                 *     TO_CHAR( 0.0000000123, 'FM9999999999.9999999' ) => 0.
                 */
                STL_DASSERT( ( ( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE ) &&
                               ( sDigitCntAfterDecimal == 0 ) ) ||
                             ( ( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE ) &&
                               ( sDigitCntAfterDecimal >= 0 ) ) );
                
                if( ( *sTmpStr == '9' ) || ( *sTmpStr == '0' ) )
                {
                    *sResult = '0';
                    sResult++;
                    sTmpStr++;
                    DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
                }
                else
                {
                    STL_DASSERT( STL_FALSE );
                }
            }
        }
        else
        {
            /* Do Nothing */
        }
        
        DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );

        if( sDigitCntAfterDecimal > 0 )
        {
//            STL_DASSERT( *sTmpStr == '.' );
            *sResult = '.';
            sResult++;
        }
        else
        {
            if( sTmpStr < sFormatStrEnd )
            {
                if( *sTmpStr == '.' )
                {
                    *sResult = *sTmpStr;
                    sResult++;
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
        }
        
        sZeroLen = -((sExponent + 1) << 1);
        
        /*
         * 예) TO_CHAR( 0.0000000123, '999999999.999' ) => '.000'
         *     TO_CHAR( 0.0000000123, 'FM9999999999.9999999' ) => '0.'
         *     TO_CHAR( 0.0000000123, 'FM9999999999.9999990' ) => '.0000000'
         */
        
        if( sZeroLen > 0 )
        {
            sTempLen =
                sZeroLen + ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ 0 ] )[ 0 ] == '0' ? 1 : 0 );

            sCarry = 0;
            if( sTempLen >= sDigitCntAfterDecimal )
            {
                if( sTempLen == sDigitCntAfterDecimal )
                {
                    if( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) )
                    {
                        if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ 0 ] )[ 1 ] > '4' )
                        {
                            sCarry = 1;
                        }
                        else
                        {
                            sCarry = 0;
                        }
                    }
                    else
                    {
                        if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ 0 ] )[ 0 ] > '4' )
                        {
                            sCarry = 1;                        
                        }
                        else
                        {
                            sCarry = 0;
                        }
                    }
                }
                
                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
                {
                    sZeroLen = sDigitCntAfterDecimal;
                }
                else
                {
                    if( sCarry == 0 )
                    {
                        sResultLen -= ( sDigitCntAfterDecimal - aFormatInfo->mZeroEndAfterDecimal );
                        sZeroLen = aFormatInfo->mZeroEndAfterDecimal;
                    }
                    else
                    {
                        sZeroLen = sTempLen;
                    }
                }

                while( sZeroLen > 0 )
                {
                    *sResult = '0';
                    sResult++;
                    sZeroLen--;
                }
                
                if( sCarry == 1 )
                {
                    sResult--;
                    *sResult = '1';
                }
                
                STL_THROW( RAMP_NUMBER_TO_CHAR_FINISH );
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
        
        while( sZeroLen > 0 )
        {
            *sResult = '0';
            sResult++;
            sZeroLen--;
            sRoundDigit--;
        }
        
        while( (sRoundDigit > 0) && (sDigitLen > 0) )
        {
            if( sIsOdd == STL_TRUE )
            {
                *sResult = sDigitString[1];
                sDigit++;
                sDigitLen--;
            }
            else
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[0];
            }

            sIsOdd = ( !sIsOdd );
            sRoundDigit--;
            sResult++;                    
        }

        if( sDigitLen > 0 )
        {
            if( sIsOdd == STL_TRUE )
            {
                sCarry = sDigitString[1] > '4' ? 1 : 0;
            }
            else
            {
                sCarry = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[0] > '4' ? 1 : 0;
            }
        }
        else
        {
            sCarry = 0;
        }
        
        if( sCarry == 1 )
        {
            sResult--;
            
            sRoundDigit = sDigitCntAfterDecimal;
            sTrimDigit = ( sDigitCntAfterDecimal - aFormatInfo->mZeroEndAfterDecimal );
            
            while( ( sCarry == 1 ) && ( sRoundDigit > 0 ) )
            {
                *sResult += sCarry;
                
                if( *sResult > '9' )
                {
                    *sResult = '0';
                    sCarry = 1;
                    
                    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
                    {
                        /* DoNothing */
                    }
                    else
                    {
                        if( sTrimDigit > 0 )
                        {
                            sResultLen--; 
                            sTrimDigit--;
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    }
                }
                else
                {
                    sCarry = 0;
                }
                
                sRoundDigit--;
                sResult--;
            }
            
            if( (sRoundDigit == 0) && (sCarry == 1) )
            {
                if( *sResult == '.' )
                {
                    sResult--;
                    
                    while( *sResult == ',' )
                    {
                        sResult--;
                    }
                }
                else
                {
                    /* Do Nothing */
                }

                while( *sResult == ',' )
                {
                    sResult--;
                }
                
                *sResult = '1';
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            /* set trailing zero */
            
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                while( sRoundDigit > 0 )
                {
                    *sResult = '0';
                    sResult++;
                    sRoundDigit--;
                }
            }
            else
            {
                /* sTempDigitCntAfterDecimal : 현재까지 만들어진 소수점 이후 숫자 갯수 */
                sTempDigitCntAfterDecimal = (sDigitCntAfterDecimal - sRoundDigit);
                
                if( sTempDigitCntAfterDecimal < aFormatInfo->mZeroEndAfterDecimal )
                {
                    /*
                     * ex) to_char( 0.45, 'fm999.9900999' ) => .4500
                     */
                    
                    sRoundDigit = ( aFormatInfo->mZeroEndAfterDecimal - sTempDigitCntAfterDecimal );
                    
                    while( sRoundDigit > 0 )
                    {
                        *sResult = '0';
                        sResult++;
                        sRoundDigit--;
                    }

                    sResultLen -= ( sDigitCntAfterDecimal - aFormatInfo->mZeroEndAfterDecimal );
                }
                else
                {
                    /*
                     * ex) to_char( 0.45678, 'fm999.9900999' ) => .45678
                     */
                    
                    sResultLen -= ( sDigitCntAfterDecimal - sTempDigitCntAfterDecimal );
                    
                    if( sTempDigitCntAfterDecimal > aFormatInfo->mZeroEndAfterDecimal )
                    {
                        sTrimDigit = ( sTempDigitCntAfterDecimal - aFormatInfo->mZeroEndAfterDecimal );
                        sResult--;
                        
                        while( ( *sResult == '0' ) && ( sTrimDigit > 0 ) )
                        {
                            sResultLen--;
                            sResult--;
                            sTrimDigit--;
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
            }
        }
    }
    else if( sDigitLen > ( sExponent + 1 ) )
    {
        sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
        
        sRoundDigit = sDigitCntBeforeDecimal + sDigitCntAfterDecimal;
        
        if( sPlusOneDigitForRound == STL_TRUE )
        {
            /* 반올림 된 숫자가 저장될 자리로 skip */
            sRoundDigit--;
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );

            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
        }
        else
        {
            /* Do Nothing */
        }
                
        if( sDigitString[ 0 ] == '0' )
        {
            /* Do Nothing */
        }
        else
        {
            *sResult = sDigitString[ 0 ];
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            sRoundDigit--;
        }

        DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );

        *sResult = sDigitString[ 1 ];
        sResult++;
        sTmpStr++;
        DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        
        sDigit++;
        sDigitLen--;
        sRoundDigit--;                

        /* set digits : before decimal point */
        while( sExponent > 0 )
        {
            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
            
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
            
            *sResult = sDigitString[ 0 ];
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );

            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
            
            *sResult = sDigitString[ 1 ];
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            
            sDigit++;
            sDigitLen--;
            sExponent--;
            sRoundDigit -= 2;
        }
        
        DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );

        if( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aFormatInfo ) == STL_TRUE )
        {
            STL_DASSERT( *sTmpStr == '.' );
            
            *sResult = '.';
            sResult++;
        }
        else
        {
            /* Do Nothing */
        }
        
        /* set digits : after decimal point */
        while( (sRoundDigit > 0) && (sDigitLen > 0) )
        {
            if( sIsOdd == STL_TRUE )
            {
                *sResult = sDigitString[1];
                sDigit++;
                sDigitLen--;
            }
            else
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[0];
            }

            sIsOdd = ( !sIsOdd );
            sResult++;
            sRoundDigit--;                    
        }

        /* check carry after decimal point */
        if( sDigitLen > 0 )
        {
            if( sIsOdd == STL_TRUE )
            {
                sCarry = sDigitString[1] > '4' ? 1 : 0;
            }
            else
            {
                sCarry = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[0] > '4' ? 1 : 0;
            }
        }
        else
        {
            sCarry = 0;
        }

        if( sCarry == 1 )
        {
            sResult--;
                    
            sRoundDigit =
                sDigitCntBeforeDecimal +
                sDigitCntAfterDecimal -
                ( sPlusOneDigitForRound == STL_FALSE ? 0 : 1 );

            sTrimDigit = ( sDigitCntAfterDecimal - aFormatInfo->mZeroEndAfterDecimal );
            
            while( (sCarry == 1) && (sRoundDigit > 0) )
            {
                if( *sResult == '.' )
                {
                    sResult--;
                    sIsAfterDecimalPos = STL_FALSE;                    

                    while( *sResult == ',' )
                    {
                        sResult--;
                    }
                }

                while( *sResult == ',' )
                {
                    sResult--;
                }

                *sResult += sCarry;

                if( *sResult > '9' )
                {
                    *sResult = '0';
                    sCarry = 1;
                    
                    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
                    {
                        /* DoNothing */
                    }
                    else
                    {
                        if( ( sTrimDigit > 0 ) && ( sIsAfterDecimalPos == STL_TRUE ) )
                        {
                            sResultLen--;
                            sTrimDigit--;
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    }
                }
                else
                {
                    sCarry = 0;
                }

                sResult--;
                sRoundDigit--;
                
                while( *sResult == ',' )
                {
                    sResult--;
                }                
            }

            if( (sRoundDigit == 0) && (sCarry == 1) )
            {
                *sResult = '1';
            }
            else
            {
                /* Do Nothing */
            }                    
        }
        else
        {
            /* set trailing zero */

            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                while( sRoundDigit > 0 )
                {
                    *sResult = '0';
                    sResult++;
                    sRoundDigit--;
                }
            }
            else
            {
                /* sTempDigitCntAfterDecimal : 현재까지 만들어진 소수점 이후 숫자 갯수 */
                sTempDigitCntAfterDecimal = (sDigitCntAfterDecimal - sRoundDigit);
                
                if( sTempDigitCntAfterDecimal < aFormatInfo->mZeroEndAfterDecimal )
                {
                    /*
                     * ex) to_char( 3.45, 'fm999.9900999' ) => 3.4500
                     */
                    
                    sRoundDigit = ( aFormatInfo->mZeroEndAfterDecimal - sTempDigitCntAfterDecimal );
                    
                    while( sRoundDigit > 0 )
                    {
                        *sResult = '0';
                        sResult++;
                        sRoundDigit--;
                    }
                    
                    sResultLen -= ( sDigitCntAfterDecimal - aFormatInfo->mZeroEndAfterDecimal );
                }
                else
                {
                    /*
                     * ex) to_char( 3.45678, 'fm999.9900999' ) => 3.45678
                     */
                    
                    sResultLen -= ( sDigitCntAfterDecimal - sTempDigitCntAfterDecimal );

                    if( sTempDigitCntAfterDecimal > aFormatInfo->mZeroEndAfterDecimal )
                    {
                        sTrimDigit = ( sTempDigitCntAfterDecimal - aFormatInfo->mZeroEndAfterDecimal );
                        sResult--;
                        
                        while( ( *sResult == '0' ) && ( sTrimDigit > 0 ) )
                        {
                            sResultLen--;
                            sResult--;
                            sTrimDigit--;
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
            }
        }
    }
    else
    {
        sZeroLen = ( sExponent - sDigitLen + 1 ) << 1;
        sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );        
        
        if( sDigitString[ 0 ] == '0' )
        {
            /* Do Nothing */
        }
        else
        {
            *sResult = sDigitString[ 0 ];
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
        
        DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
        
        *sResult = sDigitString[ 1 ];
        sResult++;
        sTmpStr++;
        DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        sDigit++;
        sDigitLen--;

        /* set digits (n) */
        while( sDigitLen > 0 )
        {
            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
            
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );

            *sResult = sDigitString[ 0 ];
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );

            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );            
            
            *sResult = sDigitString[ 1 ];
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            
            sDigit++;
            sDigitLen--;
        }

        while( sZeroLen > 0 )
        {
            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
            
            *sResult = '0';
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            sZeroLen--;
        }

        DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
        
        if( sDigitCntAfterDecimal > 0 )
        {
            /*
             * 소수점 이하 trailing zero
             * 예) TO_CHAR( 1234, '9999.99' ) => 1234.00
             */
            
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                sZeroLen = sDigitCntAfterDecimal;
            }
            else
            {
                sZeroLen = aFormatInfo->mZeroEndAfterDecimal;
                sResultLen -= ( sDigitCntAfterDecimal - aFormatInfo->mZeroEndAfterDecimal );
            }
            
            *sResult = '.';
            sResult++;
            
            while( sZeroLen > 0 )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }
        else
        {
            while( sTmpStr < sFormatStrEnd )
            {
                if( (*sTmpStr == ',') || (*sTmpStr == '.') )
                {
                    *sResult = *sTmpStr;
                    sTmpStr++;
                    DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
                    sResult++;
                }
                else
                {
                    break;
                }
            }                        
        }
    }
    
    STL_RAMP( RAMP_NUMBER_TO_CHAR_FINISH );
    
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        /* Do Nothing */
    }
    else
    {
        /*
         * FM format인 경우, 부호 표기
         * 예) TO_CHAR( 0, 'FM9.99999S' ) => '0.+'
         *     TO_CHAR( -1, '9PR' ) => '<1>'
         */
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  sResultLen,
                                  sIsPositive,
                                  STL_FALSE, // aIsSignLocatePre
                                  &sResultLen,
                                  sResult,
                                  &sResult );            
    }
    
    STL_RAMP( RAMP_FUNCTION_END );

    STL_DASSERT( sResultLen <= aFormatInfo->mToCharResultLen );
    
    *aLength = sResultLen;
    
    return STL_SUCCESS;
    
//    STL_FINISH;
    
//    return STL_FAILURE;       
}


/**
 * @brief number type을 RN format string에 맞게 string으로 변환한다.
 * @param[in]  aDataValue          dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharNumberToRNFormatString( dtlDataValue         * aDataValue,
                                           dtfNumberFormatInfo  * aFormatInfo,
                                           stlInt64               aAvailableSize,
                                           stlChar              * aResult,
                                           stlInt64             * aLength,
                                           dtlFuncVector        * aVectorFunc,
                                           void                 * aVectorArg,
                                           stlErrorStack        * aErrorStack )
{
    dtlNumericType     * sNumeric;
    stlChar            * sResult        = aResult;
    stlChar            * sDigitString   = NULL;
    stlInt64             sLength        = 0;
    stlInt32             sExponent      = 0;
    stlInt32             sDigitLen      = 0;
    stlUInt8           * sDigit         = NULL;
    stlInt32             sIsPositive    = STL_FALSE;
    stlInt32             sValue         = 0;
    stlInt32             sLeftSpace     = 0;
    stlInt32             sCarry         = 0;
    stlInt32             sIsOdd         = 0;
    stlInt32             sIndex         = 0;
    stlInt32             sIntDigitLen   = 0;
    stlInt32             sZeroLen       = 0;
    stlInt32             sResultLen     = aFormatInfo->mToCharResultLen;
    stlInt32             sLowerUpperIdx = aFormatInfo->mLowerUpperIdx;

#define DTF_TO_CHAR_RN_VALUE_DIGIT_MAX ( 4 )    
    stlUInt8             sValueDigit[ DTF_TO_CHAR_RN_VALUE_DIGIT_MAX ] = { 0, 0, 0, 0 };
    stlInt8              sValueIdx      = DTF_TO_CHAR_RN_VALUE_DIGIT_MAX;

    
    sNumeric = DTF_NUMERIC( aDataValue );

    if( DTL_NUMERIC_IS_ZERO( sNumeric , aDataValue->mLength ) == STL_TRUE )
    {
        /* zero 는 로마숫자로 표현하지 않음 */
        STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
    }
    else
    {
        sDigitLen   = DTL_NUMERIC_GET_DIGIT_COUNT( aDataValue->mLength );
        sExponent   = DTL_NUMERIC_GET_EXPONENT( sNumeric );
        sDigit      = & sNumeric->mData[1];
        sIsPositive = DTL_NUMERIC_IS_POSITIVE( sNumeric );
        sLeftSpace  = aFormatInfo->mToCharResultLen;


        if( sIsPositive )
        {
            /* Do Nothing */
        }
        else
        {
            /* 음수는 로마숫자로 표현하지 않는다. */
            STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
        }

        
        if( sExponent < 0 )
        {
            if( ( sExponent < -1 ) )
            {
                /* 0.00xx... */
                STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
            }
            else 
            {
                if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] < '5' )
                {
                    /* sExponent == -1 , sDigitString[ 0 ] < 5  인 상황 ( 0.4, 0.3... 0.0 */
                    STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
                }
                
                else
                {
                    /* sExponent == -1 , sDigitString[ 0 ] > 4  인 상황 ( 0.5, 0.6... 0.9 */
                    
                    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
                    {
                        stlMemset( sResult,
                                   ' ',
                                   sResultLen - 1 );
                    }
                    else
                    {
                        sResultLen = 1;
                    }
                    
                    aResult[ sResultLen - 1 ] = dtfRomanNumeralStringList[ sLowerUpperIdx ][ 0 ][ 1 ].mStr[0];
                }
            }
            
        }
        else if( sDigitLen > sExponent + 1 )
        {
            sCarry       = ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sExponent + 1 ] )[ 0 ] > '4' );
            sIsOdd       = ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' );
            sIntDigitLen = ( ( sExponent + 1 ) << 1 ) - sIsOdd;

            sDigit += sExponent;

            sIndex = 0;
            sIsOdd = 1;
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
            while( sIntDigitLen > 0 )
            {
                /* rounded value = 4000 */
                if( sIndex == 3 )
                {
                    if( ( sDigitString[ 0 ] == '3' ) && ( sDigitString[ 1 ] == '9' ) && ( sCarry == 1 ) )
                    {
                        STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
                    }
                    else if( sDigitString[ 0 ] >= '4' )
                    {
                        STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
                else if( sIndex > 3 )
                {
                    STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
                }
                else
                {
                    /* do nothing */
                }
                
                if( sIsOdd )
                {
                    sValue = sDigitString[ 1 ] - '0';
                }
                else
                {
                    sValue = sDigitString[ 0 ] - '0';

                    if( sIntDigitLen -1 > 0 )
                    {
                        sDigit--;
                        sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
                sIsOdd = !sIsOdd;
                
                sValue += sCarry;

                if( sValue > 9 )
                {
                    sValue = 0;
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                    
                    /* copy roman numeral */
                    sLength = dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex][ sValue ].mStrLen;
                    sLeftSpace -= sLength;
                    
                    sValueIdx--;
                    sValueDigit[ sValueIdx ] = sValue;
                }
                sIndex++;
                sIntDigitLen--;
            }

            if( sCarry == 1 )
            {
                sLength = dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ 1 ].mStrLen;
                sLeftSpace -= sLength;

                sValueIdx--;
                sValueDigit[ sValueIdx ] = 1;
            }
            else
            {
                sIndex--;
            }
            
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                stlMemset( sResult,
                           ' ',
                           sLeftSpace );
                
                sResult += sLeftSpace;
            }
            else
            {
                sResultLen -= sLeftSpace;
            }

            STL_DASSERT( ( sValueIdx >= 0 ) && ( sValueIdx < DTF_TO_CHAR_RN_VALUE_DIGIT_MAX ) );

            for( ; sValueIdx < DTF_TO_CHAR_RN_VALUE_DIGIT_MAX; sValueIdx++, sIndex-- )
            {
                stlMemcpy(
                    sResult,
                    dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sValueDigit[sValueIdx] ].mStr,
                    dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sValueDigit[sValueIdx] ].mStrLen );

                sResult += dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sValueDigit[sValueIdx] ].mStrLen;
            }
        }
        else
        {
            sIsOdd        = ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' );
            sZeroLen      = ( sExponent - sDigitLen + 1 ) << 1;
            sIntDigitLen += ( ( sZeroLen + ( sDigitLen << 1 ) ) - sIsOdd );

            
            /* sZeroLen    += ( sZeroLen > 0 ? */
            /*                ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 1 ] == '0' ) : 0 ); */

            /* 전체 값이 4자리를 넘는 경우 ex) 10000 */
            if( sIntDigitLen > 4 )
            {
                STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
            }
            else
            {
                
                if( sZeroLen > 0  )
                {
                    sIntDigitLen -= ( sExponent << 1 );
                }
                else
                {
                    sDigit += sExponent;
                }

                sIndex = sZeroLen;
                sIsOdd = 1;
            }
            
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
            while( sIntDigitLen > 0 )
            {
                /* value >= 4000 */
                if( sIndex == 3 )
                {
                    if( sDigitString[ 0 ] > '3' )
                    {
                        STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
                else
                {
                    STL_DASSERT( sIndex < 3 );
                    /* do nothing */
                }

                if( sIsOdd )
                {
                    sValue = sDigitString[ 1 ] - '0';
                }
                else
                {
                    sValue = sDigitString[ 0 ] - '0';

                    /* get before digits */
                    if( ( sIntDigitLen - 1 ) > 0 )
                    {
                        sDigit--;
                        sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
                sIsOdd = !sIsOdd;              
                                   
                /* copy roman numeral */
                sLength = dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sValue ].mStrLen;
                sLeftSpace -= sLength;

                sValueIdx--;
                sValueDigit[ sValueIdx ] = sValue;
                
                sIndex++;
                sIntDigitLen--;
            }

            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                /* fill to left space*/
                stlMemset( sResult,
                           ' ',
                           sLeftSpace );

                sResult += sLeftSpace;
            }
            else
            {
                sResultLen -= sLeftSpace;
            }

            STL_DASSERT( ( sValueIdx >= 0 ) && ( sValueIdx < DTF_TO_CHAR_RN_VALUE_DIGIT_MAX ) );
            
            sIndex--;            
            for( ; sValueIdx < DTF_TO_CHAR_RN_VALUE_DIGIT_MAX; sValueIdx++, sIndex-- )
            {
                stlMemcpy(
                    sResult,
                    dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sValueDigit[sValueIdx] ].mStr,
                    dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sValueDigit[sValueIdx] ].mStrLen );

                sResult += dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sValueDigit[sValueIdx] ].mStrLen;
            }
        }
    }

    STL_THROW( RAMP_EXIT );

    STL_RAMP( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
    
    stlMemset( sResult , '#' , aFormatInfo->mToCharResultLen );

    STL_RAMP( RAMP_EXIT );

    *aLength = sResultLen;
    
    STL_DASSERT( sResultLen <= aFormatInfo->mToCharResultLen );
    
    return STL_SUCCESS;

}


/**
 * @brief number type을 V format string에 맞게 string으로 변환한다.
 * @param[in]  aDataValue          dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharNumberToVFormatString( dtlDataValue         * aDataValue,
                                          dtfNumberFormatInfo  * aFormatInfo,
                                          stlInt64               aAvailableSize,
                                          stlChar              * aResult,
                                          stlInt64             * aLength,
                                          dtlFuncVector        * aVectorFunc,
                                          void                 * aVectorArg,
                                          stlErrorStack        * aErrorStack )
{
    stlInt32           i = 0;
    stlInt32           j = 0;
    stlInt32           sExponent      = 0;
    stlInt32           sDigitLen      = 0;
    stlInt32           sZeroLen       = 0;
    stlInt32           sRealZeroLen   = 0;
    stlInt32           sDigitCntBeforeDecimal   = 0;
    stlInt32           sOrgDigitCntAfterDecimal = 0;
    stlInt32           sTotalDigitCnt = 0;    
    stlInt32           sIsPositive    = STL_FALSE;
    stlInt32           sResultLen     = aFormatInfo->mToCharResultLen;
    stlInt32           sTempLen       = 0;

    dtlNumericType   * sNumericType   = NULL;    
    stlUInt8         * sDigit         = NULL;
    stlChar          * sDigitString   = NULL;
    stlChar          * sResult        = aResult;

    stlBool            sNumericIsZero        = STL_FALSE;
    stlBool            sPlusOneDigitForRound = STL_FALSE;

    stlInt32           sExtraDigitLen = 0;
    stlInt32           sExtraCommaLen = 0;
    stlInt32           sExtraZeroLen  = 0;
    stlInt32           sExtraSpaceLen = 0;
    stlInt32           sRoundDigit    = 0;    
    stlChar          * sTmpStr        = NULL;    

    stlUInt8           sCarry         = 0;
    stlBool            sIsOdd         = STL_FALSE;    
    
    
    sNumericType = DTF_NUMERIC( aDataValue );
    sIsPositive = DTL_NUMERIC_IS_POSITIVE( sNumericType );

    if( DTL_NUMERIC_IS_ZERO( sNumericType, aDataValue->mLength ) == STL_TRUE )
    {
        sNumericIsZero         = STL_TRUE;
        sDigitCntBeforeDecimal = aFormatInfo->mDigitCntAfterV;
    }
    else
    {
        sDigitLen = DTL_NUMERIC_GET_DIGIT_COUNT( aDataValue->mLength );
        sExponent = DTL_NUMERIC_GET_EXPONENT( sNumericType );
        sDigit    = & sNumericType->mData[1];
        
        if( sExponent < 0 )
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : -2
             * String : .0001234
             */

            sDigitCntBeforeDecimal = aFormatInfo->mDigitCntAfterV;
            
            sZeroLen = -((sExponent + 1) << 1);
            sRealZeroLen = sZeroLen + ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[0] )[0] == '0' );
            
            if( sRealZeroLen > aFormatInfo->mDigitCntAfterV )
            {
                /* ex: TO_CHAR( 0.00012, 'V99' ) */
                sNumericIsZero = STL_TRUE;
                STL_THROW( RAMP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
            }
            else
            {
                /* Do Nothing */
            }
            
            sOrgDigitCntAfterDecimal =
                ( ( ( sDigitLen - sExponent - 1 ) << 1 ) -
                  ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen - 1 ] )[ 1 ] == '0' ) );
            
            if( ( sOrgDigitCntAfterDecimal <= aFormatInfo->mDigitCntAfterV ) ||
                ( sRealZeroLen > 0 ) )
            {
                /* 
                 * ex: TO_CHAR( 0.9999, 'V9999' ) => ' 9999'
                 *     TO_CHAR( 0.09999, 'V999' ) => ' 100' 
                 */
                STL_THROW( RAMP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
            }
            else
            {
                /* Do Nothing */
            }
            
            /*
             * 반올림되어 정수부자리가 늘어나는지 여부 체크
             */

            STL_DASSERT( sRealZeroLen == 0 );

            i = ( aFormatInfo->mDigitCntAfterV  >> 1 );
            j = ( aFormatInfo->mIsOddDigitCntAfterV  == STL_TRUE ) ? 1 : 0;

            STL_DASSERT( ( i >= 0 ) && ( i < sDigitLen ) );

            if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[j] > '4' )
            {
                if( aFormatInfo->mIsOddDigitCntAfterV == STL_TRUE )
                {
                    if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '9' )
                    {
                        i--;
                    }
                    else
                    {
                        STL_THROW( RAMP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
                    }
                }
                else
                {
                    i--;
                }

                for( ; i >= 0; i-- )
                {
                    if( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '9' ) &&
                        ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[1] == '9' ) )
                    {
                        continue;
                    }
                    else
                    {
                        STL_THROW( RAMP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
                    }                            
                }
                
                STL_DASSERT( i < 0 );
                
                sPlusOneDigitForRound = STL_TRUE;
                sDigitCntBeforeDecimal++;
            }
            else
            {
                sPlusOneDigitForRound = STL_FALSE;
            }
        }
        else if( sDigitLen > (sExponent + 1) )
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : 1
             * String : 123.4
             */

            sTotalDigitCnt =
                ( ( sDigitLen << 1 ) -
                  ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[0] == '0' ? 1 : 0 ) -
                  ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[sDigitLen - 1] )[1] == '0' ? 1 : 0 ) );
            
            sDigitCntBeforeDecimal = ( sExponent << 1 ) +
                ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[0] == '0' ? 1 : 2 );

            sOrgDigitCntAfterDecimal = sTotalDigitCnt - sDigitCntBeforeDecimal;
            
            sDigitCntBeforeDecimal += aFormatInfo->mDigitCntAfterV;
            
            if( sOrgDigitCntAfterDecimal <= aFormatInfo->mDigitCntAfterV )
            {
                STL_THROW( RAMP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
            }
            else
            {
                /* Do Nothing */
            }

            /*
             * 반올림되어 정수부자리가 늘어나는지 여부 체크
             */
            
            /*
             * 예) TO_CHAR(  99.999, '999.99' )
             *     sExponent = 0
             *     sDigitLen = 3  ( 99 | 99 | 90 )
             *     sDigit[2][0]에서 반올림되어야 함.
             * 
             * 예) TO_CHAR(  99.9999, '999.999' )
             *     sExponent = 0
             *     sDigitLen = 3  ( 99 | 99 | 99 )
             *     sDigit[2][1]에서 반올림되어야 함.
             */ 
            
            i = sExponent + 1 + ( aFormatInfo->mDigitCntAfterV >> 1 );
            j = ( aFormatInfo->mIsOddDigitCntAfterV == STL_TRUE ) ? 1 : 0;
            
            STL_DASSERT( ( i >= 0 ) && ( i < sDigitLen ) );
            
            if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[j] > '4' )
            {
                if( aFormatInfo->mIsOddDigitCntAfterV == STL_TRUE )
                {
                    if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '9' )
                    {
                        i--;
                    }
                    else
                    {
                        STL_THROW( RAMP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
                    }
                }
                else
                {
                    i--;
                }

                for( ; i > 0; i-- )
                {
                    if( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '9' ) &&
                        ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[1] == '9' ) )
                    {
                        continue;
                    }
                    else
                    {
                        STL_THROW( RAMP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
                    }                            
                }
                    
                STL_DASSERT( i == 0 );

                if( ( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '0' ) &&
                      ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[1] == '9' ) )
                    ||
                    ( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '9' ) &&
                      ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[1] == '9' ) ) )
                {
                    sDigitCntBeforeDecimal++;
                    sPlusOneDigitForRound = STL_TRUE;
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
            sDigitCntBeforeDecimal = ( ( sZeroLen + ( sDigitLen << 1 ) ) -
                                       ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) );

            sDigitCntBeforeDecimal += aFormatInfo->mDigitCntAfterV;
        }
        
        STL_RAMP( RAMP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
    }

    
    /**
     * 'B' format 처리
     */

    if( DTF_NUMBER_FMT_FLAG_IS_BLANK( aFormatInfo ) == STL_TRUE )
    {
        if( sNumericIsZero == STL_TRUE )
        {
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                
                /* 
                 * 예) TO_CHAR( 0, 'VB' )
                 *     TO_CHAR( 0.00001, 'BV99' )
                 */
                
                sTempLen = sResultLen;
                while( sTempLen > 0 )
                {
                    *sResult = ' ';
                    sResult++;
                    sTempLen--;
                }
            }
            else
            {
                sResultLen = 0;
            }
            
            STL_THROW( RAMP_FUNCTION_END );
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
    

    /**
     * 정수부분의 digit format 숫자보다 실제 value의 유효숫자 갯수가 많은 경우 '#' 표기.
     */

    if( sDigitCntBeforeDecimal > aFormatInfo->mDigitCntBeforeDecimal )
    {
        /* 예: TO_CHAR( 0.999, 'V99' ) => '####'  */
        
        sTempLen = sResultLen;
        while( sTempLen > 0 )
        {
            *sResult = '#';
            sResult++;
            sTempLen--;
        }
        
        STL_THROW( RAMP_FUNCTION_END );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * 공백 & 부호 표기 & 0인 숫자에 대한 '0' 표기
     */
    
    /**
     * 공백 표기
     */
    
    if( aFormatInfo->mZeroStart == 0 )
    {
        /*
         * '0' format 이 없는 경우
         */
        
        /* 예) TO_CHAR( 12345, '9999999' ) => 'sign__12345' => '   12345' */
        sExtraDigitLen = (aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal);
        sExtraZeroLen = 0;
    }
    else
    {
        /*
         * '0' format 이 있는 경우
         */
        
        if( aFormatInfo->mZeroStart == 1 )
        {
            /* '0' format 이 첫번째에 오는 경우  */
        
            /* 예) TO_CHAR( 12345, '0999999' ) => 'sign0012345' => ' 0012345' */
            sExtraDigitLen = 0;
            sExtraZeroLen = (aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal );
        }
        else // ( aFormatInfo->mZeroStart > 1 )
        {
            /* '0' format 이 두번째 이후에 오는 경우  */
        
            sExtraDigitLen = aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal;
        
            if( sExtraDigitLen >= aFormatInfo->mZeroStart )
            {
                /* 예) TO_CHAR( 123, '9909999' ) => 'sign__00123' => '   00123' */
                sExtraDigitLen = (aFormatInfo->mZeroStart - 1);
                sExtraZeroLen =
                    (aFormatInfo->mDigitCntBeforeDecimal - sExtraDigitLen - sDigitCntBeforeDecimal);
            }
            else
            {
                /* 예) TO_CHAR( 123456, '9909999' ) => 'sign_123456' => '  123456' */
                sExtraDigitLen = (aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal);
                sExtraZeroLen = 0;
            }
        }
    }

    if( aFormatInfo->mFirstZeroOrNine != NULL )
    {
        sTmpStr = aFormatInfo->mFirstZeroOrNine;
    }
    else
    {
        sTmpStr = aFormatInfo->mStr;

        /* format string 중 digit 앞에 나올 수 있는 FM, S, $, B 를 제외한 포인터를 저장한다. */        
        
        /*
         * FM , S  skip 
         * ex) to_char( 1, 'fms9' ) or to_char( 1, 'sfm9' )
         */
        DTF_NUMBER_FMT_SKIP_FM_SIGN_NEXT_FORMAT_STR( sTmpStr );
        
        /* B, $ skip*/
        DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
    }

    i = sExtraDigitLen;
    sExtraCommaLen = 0;
    
    while( i > 0 )
    {
        if( (*sTmpStr == '9') || (*sTmpStr == '0' ) )
        {
            i--;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
        else if( *sTmpStr == ',' )
        {
            while( *sTmpStr == ',' )
            {
                sExtraCommaLen++;
                sTmpStr++;
                DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            }
        }
        else
        {
            STL_DASSERT( STL_FALSE );
        }
    }

    if( sExtraDigitLen > 0 )
    {
        while( *sTmpStr == ',' )
        {
            sExtraCommaLen++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    sExtraSpaceLen = sExtraDigitLen + sExtraCommaLen;

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {    
        while( sExtraSpaceLen > 0 )
        {
            *sResult = ' ';
            sResult++;
            sExtraSpaceLen--;
        }
    }
    else
    {
        sResultLen -= sExtraSpaceLen;
    }
    
    
    /**
     * 부호 표기
     */

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        dtfNumberFmtSetSignForNormal( aFormatInfo, aResult, sResultLen, sIsPositive, sResult, &sResult );
    }
    else
    {
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  sResultLen,
                                  sIsPositive,
                                  STL_TRUE, // aIsSignLocatePre
                                  &sResultLen,
                                  sResult,
                                  &sResult );            
    }
    

    /**
     * DOLLAR 표기
     */

    DTF_NUMBER_FMT_SET_DOLLAR( aFormatInfo, sResult );
    

    /**
     * 0인 숫자에 대한 '0' 표기
     */
    
    while( sExtraZeroLen > 0 )
    {
        if( ( *sTmpStr == '9' ) || ( *sTmpStr == '0' ) )
        {
            *sResult = '0';

            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            sExtraZeroLen--;
        }
        else if( *sTmpStr == ',' )
        {
            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
        }
        else
        {
            STL_DASSERT( STL_FALSE );
        }        
    }
    
    DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );


    /**
     * 숫자 표기 ( 0 인 경우 )
     */
    
    if( sNumericIsZero == STL_TRUE )
    {
        while( sDigitCntBeforeDecimal > 0 )
        {
            *sResult = '0';
            sResult++;
            sDigitCntBeforeDecimal--;
        }
        
        STL_THROW( RAMP_NUMBER_TO_CHAR_FINISH );
    }
    else
    {
        /* Do Nothing */
    }


    /**
     * 숫자 표기 ( 0 이 아닌 경우 )
     */
    
    if( sExponent < 0 )
    {
        sRoundDigit = aFormatInfo->mDigitCntAfterV;
        
        if( sPlusOneDigitForRound == STL_TRUE )
        {
            /* 반올림된 숫자가 저장될 장소 skip */
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
        else
        {
            /* Do Nothing */
        }
        
        DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
        
        while( sZeroLen > 0 )
        {
            *sResult = '0';
            sResult++;
            sZeroLen--;
            sRoundDigit--;
        }

        while( (sRoundDigit > 0) && (sDigitLen > 0) )
        {
            if( sIsOdd == STL_TRUE )
            {
                *sResult = sDigitString[1];
                sDigit++;
                sDigitLen--;
            }
            else
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[0];
            }

            sIsOdd = ( !sIsOdd );
            sRoundDigit--;
            sResult++;                    
        }
        
        if( sDigitLen > 0 )
        {
            if( sIsOdd == STL_TRUE )
            {
                sCarry = sDigitString[1] > '4' ? 1 : 0;
            }
            else
            {
                sCarry = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[0] > '4' ? 1 : 0;
            }
        }
        else
        {
            sCarry = 0;
        }

        if( sCarry == 1 )
        {
            sResult--;
                    
            sRoundDigit = aFormatInfo->mDigitCntAfterV;

            while( ( sCarry == 1 ) && ( sRoundDigit > 0 ) )
            {
                *sResult += sCarry;

                if( *sResult > '9' )
                {
                    *sResult = '0';
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }

                sRoundDigit--;
                sResult--;
            }
            
            if( (sRoundDigit == 0) && (sCarry == 1) )
            {
                while( *sResult == ',' )
                {
                    sResult--;
                }
                
                *sResult = '1';
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            while( sRoundDigit > 0 )
            {
                *sResult = '0';
                sResult++;
                sRoundDigit--;
            }
        }
    }
    else if( sDigitLen > ( sExponent + 1 ) )
    {
        sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );

        sRoundDigit = sDigitCntBeforeDecimal;
        
        if( sPlusOneDigitForRound == STL_TRUE )
        {
            /* 반올림 된 숫자가 저장될 자리로 skip */
            sRoundDigit--;
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            
            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
        }
        else
        {
            /* Do Nothing */
        }
        
        if( sDigitString[ 0 ] == '0' )
        {
            /* Do Nothing */
        }
        else
        {
            *sResult = sDigitString[ 0 ];
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            sRoundDigit--;
        }

        DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );

        *sResult = sDigitString[ 1 ];
        sResult++;
        sTmpStr++;
        DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        
        sDigit++;
        sDigitLen--;
        sRoundDigit--;
        
        /* set digits : before decimal point */
        while( sExponent > 0 )
        {
            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
            
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
            
            *sResult = sDigitString[ 0 ];
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );

            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
            
            *sResult = sDigitString[ 1 ];
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            
            sDigit++;
            sDigitLen--;
            sExponent--;
            sRoundDigit -= 2;
        }

        DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
        
        /* set digits : after decimal point */
        while( (sRoundDigit > 0) && (sDigitLen > 0) )
        {
            if( sIsOdd == STL_TRUE )
            {
                *sResult = sDigitString[1];
                sDigit++;
                sDigitLen--;
            }
            else
            {
                sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
                *sResult = sDigitString[0];
            }
            
            sIsOdd = ( !sIsOdd );
            sResult++;
            sRoundDigit--;                    
        }
        
        /* check carry */
        if( sDigitLen > 0 )
        {
            if( sIsOdd == STL_TRUE )
            {
                sCarry = sDigitString[1] > '4' ? 1 : 0;
            }
            else
            {
                sCarry = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[0] > '4' ? 1 : 0;
            }
        }
        else
        {
            sCarry = 0;
        }

        if( sCarry == 1 )
        {
            sResult--;
                    
            sRoundDigit =
                sDigitCntBeforeDecimal -
                ( sPlusOneDigitForRound == STL_FALSE ? 0 : 1 );

            while( (sCarry == 1) && (sRoundDigit > 0) )
            {
                while( *sResult == ',' )
                {
                    sResult--;
                }
                
                *sResult += sCarry;

                if( *sResult > '9' )
                {
                    *sResult = '0';
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }

                sResult--;
                sRoundDigit--;
                
                while( *sResult == ',' )
                {
                    sResult--;
                }
            }

            if( (sRoundDigit == 0) && (sCarry == 1) )
            {
                *sResult = '1';
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            while( sRoundDigit > 0 )
            {
                *sResult = '0';
                sResult++;
                sRoundDigit--;
            }
        }
    }
    else
    {
        sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );        
        
        if( sDigitString[ 0 ] == '0' )
        {
            /* Do Nothing */
        }
        else
        {
            *sResult = sDigitString[ 0 ];
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
        
        DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
        
        *sResult = sDigitString[ 1 ];
        sResult++;
        sTmpStr++;
        DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        sDigit++;
        sDigitLen--;
        
        /* set digits (n) */
        while( sDigitLen > 0 )
        {
            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
            
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );

            *sResult = sDigitString[ 0 ];
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );

            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );            
            
            *sResult = sDigitString[ 1 ];
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            
            sDigit++;
            sDigitLen--;
        }

        while( sZeroLen > 0 )
        {
            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
            
            *sResult = '0';
            sResult++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            sZeroLen--;
        }

        DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );

        sZeroLen = aFormatInfo->mDigitCntAfterV;
        while( sZeroLen > 0 )
        {
            *sResult = '0';
            sResult++;
            sZeroLen--;
        }
    }

    STL_RAMP( RAMP_NUMBER_TO_CHAR_FINISH );
    
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        /* Do Nothing */
    }
    else
    {
        /*
         * FM format인 경우, 부호 표기
         * 예) TO_CHAR( 0, 'FM9.99999S' ) => '0.+'
         *     TO_CHAR( -1, '9PR' ) => '<1>'
         */
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  sResultLen,
                                  sIsPositive,
                                  STL_FALSE, // aIsSignLocatePre
                                  &sResultLen,
                                  sResult,
                                  &sResult );            
    }

    STL_RAMP( RAMP_FUNCTION_END );

    *aLength = sResultLen;

    STL_DASSERT( sResultLen <= aFormatInfo->mToCharResultLen );
    
    return STL_SUCCESS;
    
//    STL_FINISH;
    
//    return STL_FAILURE;    
}


static inline stlBool _dtfAddUInt64ToUInt256( stlUInt64 * aUInt256,
                                              stlUInt64   aUInt64 )
{
    stlUInt64     sSum;


    sSum = aUInt256[3] + aUInt64;

    if( (sSum < aUInt256[3]) || (sSum < aUInt64) )
    {
        aUInt256[2]++;
        if( aUInt256[2] == 0 )
        {
            aUInt256[1]++;
            if( aUInt256[1] == 0 )
            {
                aUInt256[0]++;
                STL_TRY( aUInt256[0] > 0 );
            }
        }
    }

    aUInt256[3] = sSum;


    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}

static inline stlBool _dtfMulUInt64ToUInt256( stlUInt64 * aUInt256,
                                              stlUInt64   aUInt64 )
{
    stlUInt128    sUInt128[4];
    stlBool       sHasCarry;


    STL_TRY( stlMulUInt64( aUInt256[3], aUInt64, &sUInt128[3] ) == STL_TRUE );
    STL_TRY( stlMulUInt64( aUInt256[2], aUInt64, &sUInt128[2] ) == STL_TRUE );
    STL_TRY( stlMulUInt64( aUInt256[1], aUInt64, &sUInt128[1] ) == STL_TRUE );
    STL_TRY( stlMulUInt64( aUInt256[0], aUInt64, &sUInt128[0] ) == STL_TRUE );

    /* 4번째 자리 */
    aUInt256[3] = sUInt128[3].mLowDigit;

    /* 3번째 자리 */
    aUInt256[2] = sUInt128[3].mHighDigit + sUInt128[2].mLowDigit;
    if( (aUInt256[2] < sUInt128[3].mHighDigit) ||
        (aUInt256[2] < sUInt128[2].mLowDigit) )
    {
        sHasCarry = STL_TRUE;
    }
    else
    {
        sHasCarry = STL_FALSE;
    }

    /* 2번째 자리 */
    aUInt256[1] = sUInt128[2].mHighDigit + sUInt128[1].mLowDigit;
    if( (aUInt256[1] < sUInt128[2].mHighDigit) ||
        (aUInt256[1] < sUInt128[1].mLowDigit) )
    {
        if( sHasCarry == STL_TRUE )
        {
            aUInt256[1]++;
        }
        sHasCarry = STL_TRUE;
    }
    else
    {
        if( sHasCarry == STL_TRUE )
        {
            aUInt256[1]++;

            if( aUInt256[1] == 0 )
            {
                sHasCarry = STL_TRUE;
            }
            else
            {
                sHasCarry = STL_FALSE;
            }
        }
        else
        {
            sHasCarry = STL_FALSE;
        }
    }

    /* 1번째 자리 */
    aUInt256[0] = sUInt128[1].mHighDigit + sUInt128[0].mLowDigit;
    STL_TRY( (aUInt256[0] >= sUInt128[1].mHighDigit) &&
             (aUInt256[0] >= sUInt128[0].mLowDigit) );

    if( sHasCarry == STL_TRUE )
    {
        aUInt256[0]++;
        STL_TRY( aUInt256[1] > 0 );
    }


    return STL_TRUE;

    STL_FINISH;

    return STL_FALSE;
}


static inline stlBool _dtfDivisionUInt256By64( stlUInt64  * aUInt256,
                                               stlUInt64    aDivisor,
                                               stlUInt64  * aQuotient256,
                                               stlUInt64  * aRemainder )
{
    stlUInt128  s256HighDigits;
    stlUInt128  s256LowDigits;
    stlUInt128  sDivisor;
    stlUInt128  sQuotientHigh;
    stlUInt128  sQuotientLow;
    stlUInt128  sRemainder;
    stlBool     sIsSuccess = STL_TRUE;

    s256HighDigits.mHighDigit = aUInt256[0];
    s256HighDigits.mLowDigit  = aUInt256[1];
    s256LowDigits.mHighDigit  = aUInt256[2];
    s256LowDigits.mLowDigit   = aUInt256[3];
    sDivisor.mHighDigit       = 0;
    sDivisor.mLowDigit        = aDivisor;
    
    sIsSuccess = stlDivisionUInt256By128( & s256HighDigits,
                                          & s256LowDigits,
                                          sDivisor,
                                          & sQuotientHigh,
                                          & sQuotientLow,
                                          & sRemainder );
    
    aQuotient256[0] = sQuotientHigh.mHighDigit;
    aQuotient256[1] = sQuotientHigh.mLowDigit;
    aQuotient256[2] = sQuotientLow.mHighDigit;
    aQuotient256[3] = sQuotientLow.mLowDigit;
    
    *aRemainder = sRemainder.mLowDigit;
    STL_DASSERT( sRemainder.mHighDigit == 0 );
    
    return sIsSuccess;
}


static inline stlUInt64 _dtfGetPositiveUInt64Value( stlUInt8    * aDigit,
                                                    stlInt32      aLen )
{
    stlInt32      i;
    stlUInt64     sResult;


    sResult = 0;
    for( i = 0; i < aLen; i++ )
    {
        sResult = ( ( sResult * (stlUInt64)100 )
                    + (stlUInt64)DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( aDigit[i] ) );
    }


    return sResult;
}


/**
 * @brief number type을 X format string에 맞게 string으로 변환한다.
 * @param[in]  aDataValue          dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharNumberToXFormatString( dtlDataValue         * aDataValue,
                                          dtfNumberFormatInfo  * aFormatInfo,
                                          stlInt64               aAvailableSize,
                                          stlChar              * aResult,
                                          stlInt64             * aLength,
                                          dtlFuncVector        * aVectorFunc,
                                          void                 * aVectorArg,
                                          stlErrorStack        * aErrorStack )
{
    dtlNumericType  * sNumeric      = NULL;
    stlUInt8        * sDigit        = NULL;
    stlChar         * sResult       = aResult;

    stlUInt64         sResultValue[4];
    stlUInt64         sTmpValue;

    stlBool           sHasCarry;
    stlInt32          sDigitCount;
    stlInt32          sExponent;
    stlInt32          sRoundPos;
    stlUInt8          sToCharResultLen = aFormatInfo->mToCharResultLen;
    stlUInt8          sResultLen = aFormatInfo->mToCharResultLen;
    stlUInt8          sSpaceLen = 0;
    stlUInt8          sLeadingZeroCnt = 0;
    stlUInt8          sLowerUpperIdx = aFormatInfo->mLowerUpperIdx;


#ifndef STL_IS_BIGENDIAN     
    stlInt32          i;
#endif    

    /**
     * = 규칙 =
     * 1. 에러인 상황에서는 모든 자리를 '#'으로 출력한다.
     * 2. 소수자리가 존재하면 반올림한다.
     * 3. 음수는 에러로 처리한다.
     * 4. number type을 16진수로 표현할 수 있는 최대범위는 다음과 같다.
     * 
     *                   1         2         3         4         5         6         7
     *          12345678901234567890123456789012345678901234567890123456789012345678901234567890
     * 10진수 : 7237005577332262213973186563042994240829374041602535252466099000494570602495
     * 16진수 : fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
     */

    /* Numeric Data 얻기 */
    sNumeric = DTF_NUMERIC( aDataValue );
    
    /* Zero인 경우 0으로 Formating 해준다. */
    if( DTL_NUMERIC_IS_ZERO( sNumeric, aDataValue->mLength ) == STL_TRUE )
    {
        if( aFormatInfo->mDigitCntBeforeDecimal == 0 )
        {
            /* 공백 계산 */
            sSpaceLen = sToCharResultLen - 1;
            sLeadingZeroCnt = 0;
        }
        else
        {
            /* 공백 계산 */
            sSpaceLen = sToCharResultLen - aFormatInfo->mDigitCntBeforeDecimal - aFormatInfo->mXCnt;
            
            /* leading 0 format string 추가 */
            /* 예: to_char( 0, '00XXXXX' ) =>  0000000 */
            sLeadingZeroCnt = aFormatInfo->mDigitCntBeforeDecimal + ( aFormatInfo->mXCnt - 1 );
        }

        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
        {
            /*  String의 앞에 공백 추가 */
            DTF_ASSIGN_CHAR( sResult, ' ', sSpaceLen );
        }
        else
        {
            sResultLen -= sSpaceLen;
        }
        
        /* leading 0 format string 추가 */
        DTF_ASSIGN_CHAR( sResult, '0', sLeadingZeroCnt );
        
        /* 0 value string 추가 */
        *sResult = '0';
        
        STL_THROW( RAMP_FINISH );
    }
    
    /* 소수 자리가 존재하는지 파악 */
    sDigit = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( aDataValue->mLength );
    sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );

    /* 반올림할 위치 찾기 */
    sRoundPos = sExponent + 1;
    
    if( sRoundPos == 0 )
    {
        /* 예: to_char( 0.01, 'x' ) => 0 인 경우로 미리 걸러낸다. */
        sRoundPos =
            ( DTD_NUMERIC_GET_DIGIT_STRING( DTL_NUMERIC_IS_POSITIVE( sNumeric ),
                                            sDigit[sRoundPos] )[0] < '5' ) ? -1 : sRoundPos ;
    }
    else
    {
        /* Do Nothing */
    }
    
    /* 반올림해도 0인 경우 */
    if( sRoundPos < 0 )
    {
        if( aFormatInfo->mDigitCntBeforeDecimal == 0 )
        {
            /* 공백 계산 */
            sSpaceLen = sToCharResultLen - 1;
            sLeadingZeroCnt = 0;
        }
        else
        {
            /* 공백 계산 */
            sSpaceLen = sToCharResultLen - aFormatInfo->mDigitCntBeforeDecimal - aFormatInfo->mXCnt;
            
            /* leading 0 format string 추가 */
            /* 예: to_char( 0, '00XXXXX' ) =>  0000000 */
            sLeadingZeroCnt = aFormatInfo->mDigitCntBeforeDecimal + ( aFormatInfo->mXCnt - 1 );
        }

        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
        {
            /*  String의 앞에 공백 추가 */
            DTF_ASSIGN_CHAR( sResult, ' ', sSpaceLen );
        }
        else
        {
            sResultLen -= sSpaceLen;
        }
        
        /* leading 0 format string 추가 */
        DTF_ASSIGN_CHAR( sResult, '0', sLeadingZeroCnt );
        
        /* 0 value string 추가 */
        *sResult = '0';
        
        STL_THROW( RAMP_FINISH );
    }

    /* 음수는 변환할 수 없다. */
    STL_TRY_THROW( DTL_NUMERIC_IS_POSITIVE( sNumeric ) == STL_TRUE,
                   RAMP_CANNOT_DISPLAY );
    
    if( sRoundPos < sDigitCount )
    {
        /* 반올림이 필요한 경우 */
        sHasCarry = ( DTD_NUMERIC_GET_POS_DIGIT_STRING( sDigit[sRoundPos] )[0] > '4'
                      ? STL_TRUE : STL_FALSE );
        sDigitCount = sRoundPos;
        sExponent = 0;
    }
    else
    {
        /* 반올림이 불필요한 경우 */
        sHasCarry = STL_FALSE;
        sExponent = sExponent - sDigitCount + 1;
    }

    STL_TRY_THROW( ( ( ( sDigitCount * 2 )
                       - ( DTD_NUMERIC_GET_POS_DIGIT_STRING( sDigit[0] )[0] == '0' ? 1 : 0  ) )
                     + ( sExponent * 2 ) )
                   <= DTF_NUMBER_DIGIT_COUNT_FOR_X_FORMAT,
                   RAMP_CANNOT_DISPLAY );
    
    /**
     * Digit를 stlUInt64형태로 변환
     */

    /* Result Value 초기화 */
    sResultValue[0] = (stlUInt64)0;
    sResultValue[1] = (stlUInt64)0;
    sResultValue[2] = (stlUInt64)0;
    sResultValue[3] = (stlUInt64)0;

    /* 상위 자리 */
    if( sDigitCount > 18 )
    {
        sTmpValue = _dtfGetPositiveUInt64Value( sDigit, sDigitCount - 18 );
        STL_TRY_THROW( _dtfAddUInt64ToUInt256( sResultValue, sTmpValue ) == STL_TRUE,
                       RAMP_CANNOT_DISPLAY );
        STL_TRY_THROW( _dtfMulUInt64ToUInt256( sResultValue, gPreDefinedPow[18] ) == STL_TRUE,
                       RAMP_CANNOT_DISPLAY );
        sDigit += (sDigitCount - 18);
        sDigitCount = 18;
    }

    /* 중간 자리 */
    if( sDigitCount > 9 )
    {
        sTmpValue = _dtfGetPositiveUInt64Value( sDigit, sDigitCount - 9 );
        STL_TRY_THROW( _dtfAddUInt64ToUInt256( sResultValue, sTmpValue ) == STL_TRUE,
                       RAMP_CANNOT_DISPLAY );
        STL_TRY_THROW( _dtfMulUInt64ToUInt256( sResultValue, gPreDefinedPow[18] ) == STL_TRUE,
                       RAMP_CANNOT_DISPLAY );
        sDigit += (sDigitCount - 9);
        sDigitCount = 9;
    }

    /* 하위 자리 */
    sTmpValue = _dtfGetPositiveUInt64Value( sDigit, sDigitCount );
    if( sHasCarry == STL_TRUE )
    {
        sTmpValue++;
        STL_DASSERT( sTmpValue > 0 );
    }

    STL_TRY_THROW( _dtfAddUInt64ToUInt256( sResultValue, sTmpValue ) == STL_TRUE,
                   RAMP_CANNOT_DISPLAY );

    /* Exponent에 대한 처리 */
    while( sExponent > 9 )
    {
        STL_TRY_THROW( _dtfMulUInt64ToUInt256( sResultValue, gPreDefinedPow[18] ) == STL_TRUE,
                       RAMP_CANNOT_DISPLAY );

        sExponent -= 9;
    }

    if( sExponent > 0 )
    {
        STL_TRY_THROW( _dtfMulUInt64ToUInt256( sResultValue, gPreDefinedPow[sExponent*2] ) == STL_TRUE,
                       RAMP_CANNOT_DISPLAY );
    }


    /**
     * 결과를 String으로 변환
     */

#ifndef STL_IS_BIGENDIAN   /* This platform has little endian */
    /* Bigendian형태로 바꾸어 stlUInt8의 Array로 접근 가능하도록 한다. */
    for( i = 0; i < 4; i++ )
    {
        if( sResultValue[i] > (stlUInt64)0 )
        {
            sTmpValue = sResultValue[i];
            ((stlUInt8*)(&sResultValue[i]))[0] = ((stlUInt8*)(&sTmpValue))[7];
            ((stlUInt8*)(&sResultValue[i]))[1] = ((stlUInt8*)(&sTmpValue))[6];
            ((stlUInt8*)(&sResultValue[i]))[2] = ((stlUInt8*)(&sTmpValue))[5];
            ((stlUInt8*)(&sResultValue[i]))[3] = ((stlUInt8*)(&sTmpValue))[4];
            ((stlUInt8*)(&sResultValue[i]))[4] = ((stlUInt8*)(&sTmpValue))[3];
            ((stlUInt8*)(&sResultValue[i]))[5] = ((stlUInt8*)(&sTmpValue))[2];
            ((stlUInt8*)(&sResultValue[i]))[6] = ((stlUInt8*)(&sTmpValue))[1];
            ((stlUInt8*)(&sResultValue[i]))[7] = ((stlUInt8*)(&sTmpValue))[0];
        }
    }
#endif

    /* 결과 Sting을 만들기 위해 변환할 Digit 및 변환된 String Size 계산 */
    sDigit = (stlUInt8*)sResultValue;
    sDigitCount = 64;
    while( *sDigit == (stlUInt8)0 )
    {
        sDigit++;
        sDigitCount -= 2;
    }

    if( *sDigit < (stlUInt8)0x10 )
    {
        sHasCarry = STL_TRUE;
        sDigitCount--;
    }
    else
    {
        sHasCarry = STL_FALSE;
    }
    
    /* 변환된 수가 Result Length에 포함되는지 판단 */
    STL_TRY_THROW( sDigitCount < sToCharResultLen,
                   RAMP_CANNOT_DISPLAY );
    
    /* 공백 계산 */
    if( aFormatInfo->mDigitCntBeforeDecimal == 0 )
    {
        sSpaceLen = sToCharResultLen - sDigitCount;
        sLeadingZeroCnt = 0;
    }
    else
    {
        sSpaceLen = sToCharResultLen - aFormatInfo->mDigitCntBeforeDecimal - aFormatInfo->mXCnt;
        sLeadingZeroCnt = aFormatInfo->mDigitCntBeforeDecimal + ( aFormatInfo->mXCnt - sDigitCount );        
    }

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        /*  String의 앞에 공백 추가 */
        DTF_ASSIGN_CHAR( sResult, ' ', sSpaceLen );
    }
    else
    {
        sResultLen -= sSpaceLen;
    }
    
    /* leading 0 format string 추가 */
    DTF_ASSIGN_CHAR( sResult, '0', sLeadingZeroCnt );
    
    /* String의 실제 값 추가 */
    if( sHasCarry == STL_TRUE )
    {
        *sResult = dtfHexaToString[sLowerUpperIdx][*sDigit];
        sResult++;
        sDigit++;
        sDigitCount--;
    }

    while( sDigitCount > 0 )
    {
        *sResult = dtfHexaToString[sLowerUpperIdx][( (*sDigit) & (stlUInt8)0xF0 ) >> 4];
        sResult++;
        *sResult = dtfHexaToString[sLowerUpperIdx][(*sDigit) & (stlUInt8)0x0F];
        sResult++;
        sDigit++;
        sDigitCount -= 2;
    }
    
    STL_THROW( RAMP_FINISH );
    
    
    STL_RAMP( RAMP_CANNOT_DISPLAY );
    
    
    /* Result의 모든 자리에 #으로 표시한다. */
    DTF_ASSIGN_CHAR( sResult, '#', sToCharResultLen );    


    STL_RAMP( RAMP_FINISH );


    /* Result Length 셋팅 */
    *aLength = sResultLen;

    STL_DASSERT( sResultLen <= aFormatInfo->mToCharResultLen );

    return STL_SUCCESS;

//    STL_FINISH;

//    return STL_FAILURE;
}

/**
 * @brief number type을 EEEE format string에 맞게 string으로 변환한다.
 *
 * @param[in]  aDataValue          dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharNumberToEEEEFormatString( dtlDataValue         * aDataValue,
                                             dtfNumberFormatInfo  * aFormatInfo,
                                             stlInt64               aAvailableSize,
                                             stlChar              * aResult,
                                             stlInt64             * aLength,
                                             dtlFuncVector        * aVectorFunc,
                                             void                 * aVectorArg,
                                             stlErrorStack        * aErrorStack )
{
    stlInt32           i = 0;
    stlInt32           j = 0;
    
    dtlNumericType   * sNumericType     = NULL;
    stlBool            sNumericIsZero   = STL_FALSE;            
    
    stlUInt8         * sDigit           = NULL;
    stlInt32           sDigitLen        = 0;
    stlChar          * sDigitString     = NULL;
    stlChar          * sResult          = aResult;
    stlInt32           sResultLen       = aFormatInfo->mToCharResultLen;
    stlInt32           sTempLen         = 0;
    stlInt32           sExponent        = 0;
    stlInt32           sDisplayExponent = 0;    
    stlInt32           sZeroLen         = 0;
    stlInt32           sRoundDigit      = 0;
    stlInt32           sIsPositive      = STL_FALSE;
    stlInt32           sTotalDigitCnt   = 0;    
    stlUInt8           sLen             = 0;
    stlUInt8           sCarry           = 0;
    stlBool            sIsOdd           = STL_FALSE;
    stlInt32           sDigitCntBeforeDecimal = 0;    
    stlInt32           sDigitCntAfterDecimal  = 0;
    stlInt32           sDigitCntExceptZero    = 0;
    stlInt32           sTempDigitCntAfterDecimal = 0;
    stlInt32           sTrimDigit       = 0;        
    

    sNumericType = DTF_NUMERIC( aDataValue );
    sIsPositive = DTL_NUMERIC_IS_POSITIVE( sNumericType );

    STL_DASSERT( aFormatInfo->mDigitCntBeforeDecimal > 0 );
    
    if( DTL_NUMERIC_IS_ZERO( sNumericType, aDataValue->mLength ) == STL_TRUE )
    {
        sNumericIsZero         = STL_TRUE;
        sDisplayExponent       = 0;
        sDigitCntAfterDecimal  = aFormatInfo->mDigitCntAfterDecimal;
        
        if( ( sDigitCntAfterDecimal == 0 ) ||
            ( aFormatInfo->mZeroStart > 0 ) )
        {
            /*
             * 예) TO_CHAR( 0, '9EEEE' ) => '  0E+00'
             *     TO_CHAR( 0, '0EEEE' ) => '  0E+00'
             *     TO_CHAR( 0, '0.999EEEE' ) => '  0.000E+00'
             */     
            sDigitCntBeforeDecimal = 1;
        }
        else
        {
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                /* 예) TO_CHAR( 0, '9.999EEEE' ) => '   .000E+00' */
                sDigitCntBeforeDecimal = 0;
            }
            else
            {
                if( aFormatInfo->mZeroEndAfterDecimal > 0 )
                {
                    /* 예) TO_CHAR( 0, 'FM9.0EEEE' ) => '.0E+00' */
                    sDigitCntBeforeDecimal = 0;
                }
                else
                {
                    /* 예) TO_CHAR( 0, 'FM9.9EEEE' ) => '0.E+00' */
                    sDigitCntBeforeDecimal = 1;
                }
            }
        }
    }
    else
    {
        sDigitLen = DTL_NUMERIC_GET_DIGIT_COUNT( aDataValue->mLength );
        sExponent = DTL_NUMERIC_GET_EXPONENT( sNumericType );
        sDigit    = & sNumericType->mData[1];

        sDigitCntAfterDecimal  = aFormatInfo->mDigitCntAfterDecimal;
        sDigitCntBeforeDecimal = 1 + aFormatInfo->mDigitCntAfterV;
        
        if( sExponent < 0 )
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : -2
             * String : .0001234
             */
            
            /**
             *  display 될 exponent와 total digit count를 구한다.
             */

            /* display 될 exponent를 구한다. */            
            sZeroLen = -((sExponent + 1) << 1);
            
            /* 앞뒤 zero를 제외한 digit count를 계산한다. */
            
            if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' )
            {
                sZeroLen++;
                sDigitCntExceptZero = 1;
            }
            else
            {
                sDigitCntExceptZero = 2;                
            }
            

            if( sDigitLen > 1 )
            {
                sDigitCntExceptZero +=
                    ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[sDigitLen - 1] )[1] == '0') ? 1 : 2;
                sDigitCntExceptZero += (sDigitLen - 2) << 1;
            }
            else
            {
                sDigitCntExceptZero -=
                    ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[0] )[1] == '0') ? 1 : 0;
            }

            sDisplayExponent = -(sZeroLen + 1);
        }
        else if( sDigitLen > (sExponent + 1) )
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : 1
             * String : 123.4
             */
            if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[0] == '0' )
            {
                sDisplayExponent = ( sExponent << 1 );
                sDigitCntExceptZero = 1;
            }
            else
            {
                sDisplayExponent = ( sExponent << 1 ) + 1;
                sDigitCntExceptZero = 2;
            }

            STL_DASSERT( sDigitLen >= 2 );
            
            sDigitCntExceptZero +=
                ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[sDigitLen - 1] )[1] == '0') ? 1 : 2;
            sDigitCntExceptZero += (sDigitLen - 2) << 1;
        }
        else
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : 3
             * String : 1234000
             */

            if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[0] == '0' )
            {
                sDisplayExponent = ( sExponent << 1 );
                sDigitCntExceptZero = 1;
            }
            else
            {
                sDisplayExponent = ( sExponent << 1 ) + 1;
                sDigitCntExceptZero = 2;
            }

            if( sDigitLen > 1 )
            {            
                sDigitCntExceptZero +=
                    ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[sDigitLen - 1] )[1] == '0') ? 1 : 2;
                sDigitCntExceptZero += (sDigitLen - 2) << 1;
            }
            else
            {
                /* Do Nothing */
            }
        }
        
        /**
         * 반올림되어 자릿수가 늘어나는 경우 display exponent 재조정
         */
        
        sTotalDigitCnt = sDigitCntBeforeDecimal + sDigitCntAfterDecimal;
        
        if( sDigitCntExceptZero > sTotalDigitCnt )
        {
            /* 반올림되어 정수부자리가 늘어나는지 여부 체크 */
            
            if( aFormatInfo->mIsOddDigitCntAfterDecimal == STL_TRUE  )
            {
                if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[0] == '0' )
                {
                    sIsOdd = STL_TRUE;
                    i = 2;
                }
                else
                {
                    sIsOdd = STL_FALSE;
                    i = 0;
                }
            }
            else
            {
                if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[0] == '0' )
                {
                    sIsOdd = STL_FALSE;
                }
                else
                {
                    sIsOdd = STL_TRUE;
                }
                
                i = 1;
            }
            
            /* 반올림할 자리를 찾는다. */

            i = ( ( sTotalDigitCnt + i ) >> 1 );

            if( sIsOdd == STL_TRUE )
            {
                i--;
            }
            else
            {
                /* Do Nothing */
            }

            STL_DASSERT( ( i >= 0 ) && ( i < sDigitLen ) );
                
            j = ( sIsOdd == STL_TRUE ) ? 1 : 0;

            if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[j] > '4' )
            {
                if( sIsOdd == STL_TRUE )
                {
                    if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '9' )
                    {
                        if( i == 0 )
                        {
                            sDisplayExponent++;
                            STL_THROW( RAMP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
                        }
                        else
                        {
                            i--;
                        }
                    }
                    else
                    {
                        STL_THROW( RAMP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
                    }
                }
                else
                {
                    i--;
                }

                for( ; i > 0; i-- )
                {
                    if( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '9' ) &&
                        ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[1] == '9' ) )
                    {
                        continue;
                    }
                    else
                    {
                        STL_THROW( RAMP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
                    }                            
                }
                    
                STL_DASSERT( i == 0 );
                    
                if( ( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '0' ) &&
                      ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[1] == '9' ) )
                    ||
                    ( ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[0] == '9' ) &&
                      ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[i] )[1] == '9' ) ) )
                {
                    sDisplayExponent++;
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
                
            STL_RAMP( RAMP_CHECK_PLUS_ONE_DIGIT_FOR_ROUND );
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    
    /**
     * 'B' format 처리
     */

    if( DTF_NUMBER_FMT_FLAG_IS_BLANK( aFormatInfo ) == STL_TRUE )
    {
        if( sNumericIsZero == STL_TRUE )    
        {
            /* 예) TO_CHAR( 0, 'B9EEEE' ) */

            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                sTempLen = sResultLen;
                while( sTempLen > 0 )
                {
                    *sResult = ' ';
                    sResult++;
                    sTempLen--;
                }
            }
            else
            {
                sResultLen = 0;
            }

            STL_THROW( RAMP_FUNCTION_END );
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
    

    /**
     * '#' 표기.
     */
    
    /* Do Nothing */
    

    /**
     * 공백 & 부호  & DOLLAR 표기
     */

    
    /**
     * 공백 표기
     */
    
    if( aFormatInfo->mDigitCntAfterV == 0 )
    {
        if( ( sNumericIsZero == STL_TRUE ) && ( sDigitCntBeforeDecimal == 0 ) )
        {
            /*
             * 예) to_char( 0, '9.999999EEEE' )  =>  [][][][].000000E+00
             */
            
            *sResult = ' ';
            sResult++;
        }
        else
        {
            /*
             * 예) to_char( 0, '0.999999EEEE' )  =>  [][][]0.000000E+00
             */
            
            /* Do Nothing */
        }
    }
    else
    {
        
        if( ( sNumericIsZero == STL_TRUE ) && ( aFormatInfo->mZeroStart == 0 ) )
        {
            /*
             * 예) to_char( 0, '9v9eeee' )  =>  [][][]0E+00
             */
            
            *sResult = ' ';
            sResult++;
        }
        else
        {
            /*
             * 예) to_char( 0, '0v9eeee' )  =>  [][]00E+00
             */
            
            /* Do Nothing */
        }
    }
    
    if( ( sDisplayExponent > -100 ) && ( sDisplayExponent < 100 ) )
    {
        *sResult = ' ';
        sResult++;
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * FM format element가 있는 경우, 공백 제거.
     */
    
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        /* Do Nothing */
    }
    else
    {
        /* FM format element가 있는 경우, 공백 제거. */

        sResultLen -= ( sResult - aResult );
        sResult = aResult;
    }
    

    /**
     * 부호 표기
     */
    
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        dtfNumberFmtSetSignForNormal( aFormatInfo, aResult, sResultLen, sIsPositive, sResult, &sResult );
    }
    else
    {
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  sResultLen,
                                  sIsPositive,
                                  STL_TRUE, // aIsSignLocatePre
                                  &sResultLen,
                                  sResult,
                                  &sResult );            
    }
    
    
    /**
     * DOLLAR 표기
     */

    DTF_NUMBER_FMT_SET_DOLLAR( aFormatInfo, sResult );


    /**
     * EEEE 표기 ( 0 인 경우 )
     */
    
    if( sNumericIsZero == STL_TRUE )
    {
        if( aFormatInfo->mDigitCntAfterV == 0 )
        {
            if( sDigitCntBeforeDecimal == 1 )
            {
                *sResult = '0';
                sResult++;                
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            if( aFormatInfo->mZeroStart == 0 )
            {
                sZeroLen = aFormatInfo->mDigitCntAfterV;
            }
            else
            {
                sZeroLen = 1 + aFormatInfo->mDigitCntAfterV;
            }

            while( sZeroLen > 0 )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }
        
        if( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aFormatInfo ) == STL_TRUE )
        {
            *sResult = '.';
            sResult++;
        }
        else
        {
            /* Do Nothing */
        }

        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
        {
            sZeroLen = sDigitCntAfterDecimal;
        }
        else
        {
            sZeroLen = aFormatInfo->mZeroEndAfterDecimal;
            sResultLen -= ( sDigitCntAfterDecimal - sZeroLen );
        }
        
        while( sZeroLen > 0 )
        {
            *sResult = '0';
            sResult++;
            sZeroLen--;
        }
        
        STL_THROW( RAMP_NUMBER_TO_CHAR_FINISH );
    }
    else
    {
        /* Do Nothing */
    }

    
    /**
     * EEEE 표기 ( 0 이 아닌 경우 )
     */
    
    sIsOdd = STL_FALSE;
    
    sRoundDigit = sDigitCntBeforeDecimal + sDigitCntAfterDecimal;
        
    sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
        
    if( sDigitString[0] == '0' )
    {
        *sResult = sDigitString[1];
        sResult++;
        sDigit++;
        sDigitLen--;
        sRoundDigit--;
    }
    else
    {
        *sResult = sDigitString[0];
        sResult++;
        sRoundDigit--;

        sIsOdd = ( !sIsOdd );
    }
    
    if( aFormatInfo->mDigitCntAfterV == 0 )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aFormatInfo ) == STL_TRUE )
        {
            *sResult = '.';
            sResult++;
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
    
    while( (sRoundDigit > 0) && (sDigitLen > 0) )
    {
        if( sIsOdd == STL_TRUE )
        {
            *sResult = sDigitString[1];
            sDigit++;
            sDigitLen--;
        }
        else
        {
            sDigitString = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit );
            *sResult = sDigitString[0];
        }
        
        sIsOdd = ( !sIsOdd );
        sRoundDigit--;
        sResult++;
    }

    if( sDigitLen > 0 )
    {
        if( sIsOdd == STL_TRUE )
        {
            sCarry = sDigitString[1] > '4' ? 1 : 0;
        }
        else
        {
            sCarry = DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[0] > '4' ? 1 : 0;
        }
    }
    else
    {
        sCarry = 0;
    }

    if( sCarry == 1 )
    {
        sResult--;
        
        if( aFormatInfo->mDigitCntAfterV == 0 )
        {
            sRoundDigit = sDigitCntAfterDecimal;
            sTrimDigit = ( sDigitCntAfterDecimal - aFormatInfo->mZeroEndAfterDecimal );
        }
        else
        {
            sRoundDigit = aFormatInfo->mDigitCntAfterV;
            sTrimDigit = 0;
        }

        while( ( sCarry == 1 ) && ( sRoundDigit > 0 ) )
        {
            *sResult += sCarry;

            if( *sResult > '9' )
            {
                *sResult = '0';
                sCarry = 1;

                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
                {
                    /* Do Nothing */
                }
                else
                {
                    if( sTrimDigit > 0 )
                    {
                        sResultLen--;
                        sTrimDigit--;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
            }
            else
            {
                sCarry = 0;
            }

            sRoundDigit--;
            sResult--;
        }
        
        if( (sRoundDigit == 0) && (sCarry == 1) )
        {
            if( *sResult == '.' )
            {
                sResult--;
            }
            else
            {
                /* Do Nothing */
            }
            
            *sResult = '1';
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        /* set trailing zero */

        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
        {
            sZeroLen = sRoundDigit;
            
            while( sZeroLen > 0 )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }
        else
        {
            if( aFormatInfo->mDigitCntAfterV > 0 )
            {
                sZeroLen = sRoundDigit;
            
                while( sZeroLen > 0 )
                {
                    *sResult = '0';
                    sResult++;
                    sZeroLen--;
                }
            }
            else
            {
                /* sTempDigitCntAfterDecimal : 현재까지 만들어진 소수점 이후 숫자 갯수 */
                sTempDigitCntAfterDecimal = (sDigitCntAfterDecimal - sRoundDigit);
            
                if( sTempDigitCntAfterDecimal < aFormatInfo->mZeroEndAfterDecimal )
                {
                    /*
                     * ex) TO_CHAR( 123, 'FM9.9900999EEEE' ) => 1.2300E+02
                     */
                
                    sZeroLen = ( aFormatInfo->mZeroEndAfterDecimal - sTempDigitCntAfterDecimal );
                
                    while( sZeroLen > 0 )
                    {
                        *sResult = '0';
                        sResult++;
                        sZeroLen--;
                    }

                    sResultLen -= ( sDigitCntAfterDecimal - aFormatInfo->mZeroEndAfterDecimal );
                }
                else
                {
                    /*
                     * ex) TO_CHAR( 456789, 'FM9.9900999EEEE' ) => 4.56789E+05
                     */
                    
                    sResultLen -= ( sDigitCntAfterDecimal - sTempDigitCntAfterDecimal );

                    if( sTempDigitCntAfterDecimal > aFormatInfo->mZeroEndAfterDecimal )
                    {
                        sTrimDigit = ( sTempDigitCntAfterDecimal - aFormatInfo->mZeroEndAfterDecimal );
                        sResult--;
                        
                        while( ( *sResult == '0' ) && ( sTrimDigit > 0 ) )
                        {
                            sResultLen--;
                            sResult--;
                            sTrimDigit--;
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
            }
        }
    }
    
    STL_RAMP( RAMP_NUMBER_TO_CHAR_FINISH );
    
    /* 지수표현출력 */
    dtfNumberFmtSetEEEEExpForTempNumber( aFormatInfo,
                                         aResult,
                                         sResultLen,
                                         sIsPositive,
                                         sDisplayExponent,
                                         &sLen );    
    
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        /* Do Nothing */
    }
    else
    {
        /*
         * FM format인 경우, 부호 표기
         * 예) TO_CHAR( 0, 'FM9.99999S' ) => '0.+'
         *     TO_CHAR( -1, '9PR' ) => '<1>'
         */
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  sResultLen,
                                  sIsPositive,
                                  STL_FALSE, // aIsSignLocatePre
                                  &sResultLen,
                                  sResult,
                                  &sResult );            
    }
    
    STL_RAMP( RAMP_FUNCTION_END );
    
    *aLength = sResultLen;

    STL_DASSERT( sResultLen <= aFormatInfo->mToCharResultLen );
    
    return STL_SUCCESS;
    
//    STL_FINISH;
    
//    return STL_FAILURE;
}


/**
 * @brief format string을 변환하는 과정 중 부호를 표기한다.
 * @param[in]  aFormatInfo       format info
 * @param[in]  aResult           result 결과를 저장할 버퍼
 * @param[in]  aResultLen        result 결과 길이
 * @param[in]  aIsPositive       부호가 positive인지 여부
 * @param[out] aCurResultPtr     현재 처리할 result 버퍼 포인터
 * @param[out] aCurResultPtrAdd  현재 처리할 result 버퍼 포인터의 주소
 */
void dtfNumberFmtSetSignForNormal( dtfNumberFormatInfo   * aFormatInfo,
                                   stlChar               * aResult,
                                   stlInt32                aResultLen,
                                   stlInt32                aIsPositive,
                                   stlChar               * aCurResultPtr,
                                   stlChar              ** aCurResultPtrAdd )
{
    stlChar sSign = {0};

    if( DTF_NUMBER_FMT_FLAG_IS_BRACKET( (aFormatInfo) ) == STL_FALSE )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_SIGN( (aFormatInfo) ) == STL_TRUE )
        {
            /*
             * 예) TO_CHAR( 123, 'S999' ) or TO_CHAR( 123, '999S' )
             */
            if( aIsPositive )
            {
                sSign = '+';
            }
            else
            {
                sSign = '-';
            }
        }
        else
        {
            /*
             * 예) TO_CHAR( -123, '999MI' )
             *     TO_CHAR( -123, '999' )
             */
            if( aIsPositive )
            {
                sSign = ' ';
            }
            else
            {
                sSign = '-';
            }
        }

        if( ( (aFormatInfo)->mSignLocate == DTF_NUMBER_SIGN_LOCATE_NONE ) ||
            ( (aFormatInfo)->mSignLocate == DTF_NUMBER_SIGN_LOCATE_PRE ) )
        {
            *(aCurResultPtr) = sSign;
            (aCurResultPtr)++;
        }
        else
        {
            *((aResult) + (aResultLen) - 1) = sSign;
        }
    }
    else
    {
        /* PR 에 대한 부호 표기 */

        if( aIsPositive )
        {
            *(aCurResultPtr) = ' ';
            (aCurResultPtr)++;
            *((aResult) + (aResultLen) - 1) = ' ';
        }
        else
        {
            *(aCurResultPtr) = '<';
            (aCurResultPtr)++;
            *((aResult) + (aResultLen) - 1) = '>';
        }
    }
    
    *aCurResultPtrAdd = aCurResultPtr;
}


/**
 * @brief FM format string을 변환하는 과정 중 부호를 표기한다.
 * @param[in]  aFormatInfo       format info
 * @param[in]  aResult           result 결과를 저장할 버퍼
 * @param[in]  aResultLen        result 결과 길이
 * @param[in]  aIsPositive       부호가 positive인지 여부
 * @param[in]  aIsSignLocatePre  부호가 쓰여질 위치. ( +123, 123+, <123> )
 * @param[in]  aSetSignResultLen 부호 처리후, result 결과 길이 
 * @param[out] aCurResultPtr     현재 처리할 result 버퍼 포인터
 * @param[out] aCurResultPtrAdd  현재 처리할 result 버퍼 포인터의 주소
 */
void dtfNumberFmtSetSignForFM( dtfNumberFormatInfo   * aFormatInfo,
                               stlChar               * aResult,
                               stlInt32                aResultLen,
                               stlInt32                aIsPositive,
                               stlBool                 aIsSignLocatePre,
                               stlInt32              * aSetSignResultLen,                          
                               stlChar               * aCurResultPtr,
                               stlChar              ** aCurResultPtrAdd )
{
    stlChar sSign = {0};
    
    if( DTF_NUMBER_FMT_FLAG_IS_BRACKET( (aFormatInfo) ) == STL_FALSE )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_SIGN( (aFormatInfo) ) == STL_TRUE )
        {
            /*
             * 예) TO_CHAR( 123, 'S999' ) or TO_CHAR( 123, '999S' )
             */
            if( aIsPositive )
            {
                sSign = '+';
            }
            else
            {
                sSign = '-';
            }
        }
        else
        {
            /*
             * 예) TO_CHAR( -123, '999MI' )
             *     TO_CHAR( -123, '999' )
             */
            if( aIsPositive )
            {
                sSign = ' ';
            }
            else
            {
                sSign = '-';
            }
        }
        
        if( aIsSignLocatePre == STL_TRUE )
        {
            if( (aFormatInfo)->mSignLocate == DTF_NUMBER_SIGN_LOCATE_POST )
            {
                /* Do Nothing */
            }
            else
            {
                STL_DASSERT( ( (aFormatInfo)->mSignLocate == DTF_NUMBER_SIGN_LOCATE_NONE ) ||
                             ( (aFormatInfo)->mSignLocate == DTF_NUMBER_SIGN_LOCATE_PRE ) );

                if( sSign == ' ' )
                {
                    STL_DASSERT( ( DTF_NUMBER_FMT_FLAG_IS_SIGN( (aFormatInfo) ) == STL_FALSE ) &&
                                 ( aIsPositive ) );
                    
                    *aSetSignResultLen = aResultLen - 1;                        
                }
                else
                {
                    *(aCurResultPtr) = sSign;
                    (aCurResultPtr)++;
                }
            }
        }
        else
        {
            if( (aFormatInfo)->mSignLocate == DTF_NUMBER_SIGN_LOCATE_POST )
            {
                if( sSign == ' ' )
                {
                    STL_DASSERT( ( DTF_NUMBER_FMT_FLAG_IS_SIGN( (aFormatInfo) ) == STL_FALSE ) &&
                                 ( aIsPositive ) );
                    
                    *aSetSignResultLen = aResultLen - 1;                        
                }
                else
                {
                    *(aResult + aResultLen - 1) = sSign;
                    (aCurResultPtr)++;
                }
            }
            else
            {
                STL_DASSERT( ( (aFormatInfo)->mSignLocate == DTF_NUMBER_SIGN_LOCATE_NONE ) ||
                             ( (aFormatInfo)->mSignLocate == DTF_NUMBER_SIGN_LOCATE_PRE ) );
                
                /* Do Nothing */                
            }
        }
    }
    else
    {
        /* PR 에 대한 부호 표기 */
        
        if( aIsSignLocatePre == STL_TRUE )
        {
            if( aIsPositive )
            {
                *aSetSignResultLen = aResultLen - 1;
            }
            else
            {
                *(aCurResultPtr) = '<';
                (aCurResultPtr)++;
            }
        }
        else
        {
            if( aIsPositive )
            {
                *aSetSignResultLen = aResultLen - 1;
            }
            else
            {
                *(aResult + aResultLen - 1) =  '>';
                (aCurResultPtr)++;
            }            
        }
    }
    
    *aCurResultPtrAdd = aCurResultPtr;
}


/**
 * @brief format string을 변환하는 과정 중 지수형태 exponent를 표기한다
 * @param[in]  aFormatInfo       format info
 * @param[in]  aResult           result 결과를 저장할 버퍼
 * @param[in]  aResultLen        result 결과 길이
 * @param[in]  aIsPositive       부호가 positive인지 여부
 * @param[in]  aExp              표현할 exponent
 * @param[out] aSetEEEEExpLen    현재 처리한 EEEE Exp의 길이 반환
 */
void dtfNumberFmtSetEEEEExpForTempNumber( dtfNumberFormatInfo   * aFormatInfo,
                                          stlChar               * aResult,
                                          stlInt32                aResultLen,
                                          stlInt32                aIsPositive,
                                          stlInt32                aExp,
                                          stlUInt8              * aSetEEEEExpLen )
{
    /*
     * @todo binary_real, binary_double 에 대한 구현상태를 보고 함수 통합
     */
    
    stlUInt8   sExpLen         = 4;
    stlInt32   sExp            = aExp;
    stlChar    sNumber[11]     = "0123456789";
    stlChar  * sCurResultPtr   = NULL;
    stlChar  * sResultLastCharPtr = ( aResult + aResultLen - 1 );

    if( (sExp >= 100 ) || (sExp <= -100) )
    {
        sExpLen = DTF_NUMBER_FMT_EEEE_STR_LEN;        
    }
    else
    {
        /* Do Nothing */
    }

    if( ( ( DTF_NUMBER_FMT_FLAG_IS_SIGN( (aFormatInfo) ) == STL_TRUE ) &&
          ( aFormatInfo->mSignLocate == DTF_NUMBER_SIGN_LOCATE_POST ) )
        ||
        ( DTF_NUMBER_FMT_FLAG_IS_MINUS( (aFormatInfo) ) == STL_TRUE )
        ||  
        ( DTF_NUMBER_FMT_FLAG_IS_BRACKET( aFormatInfo ) == STL_TRUE ) )
    {
        /* ex) TO_CHAR( 1, '9.9EEEES' ) => ' 1.0E+00+'
         *     TO_CHAR( 1, '9.9EEEEMI' ) => ' 1.0E+00 '
         *     TO_CHAR( 1, '9.9EEEEPR' ) => '  1.0E+00 '
         *     TO_CHAR( -1, '9.9EEEES' ) => ' 1.0E+00-'
         *     TO_CHAR( -1, '9.9EEEEMI' ) => ' 1.0E+00-'
         *     TO_CHAR( -1, '9.9EEEEPR' ) => ' <1.0E+00>'
         *
         *     TO_CHAR( 1, 'FM9.9EEEES' ) => '1.E+00+'
         *     TO_CHAR( 1, 'FM9.9EEEEMI' ) => '1.E+00'
         *     TO_CHAR( 1, 'FM9.9EEEEPR' ) => '1.E+00'
         *     TO_CHAR( -1, 'FM9.9EEEES' ) => '1.E+00-'
         *     TO_CHAR( -1, 'FM9.9EEEEMI' ) => '1.E+00-'
         *     TO_CHAR( -1, 'FM9.9EEEEPR' ) => '<1.E+00>'
         */
        sCurResultPtr = sResultLastCharPtr - sExpLen;
    }
    else
    {
        /*
         * ex) TO_CHAR( 1, '9.9EEEE' ) => '  1.0E+00'
         *     TO_CHAR( 1, 'S9.9EEEE' ) => ' +1.0E+00'
         *     TO_CHAR( -1, '9.9EEEE' ) => ' -1.0E+00'
         *     TO_CHAR( -1, 'S9.9EEEE' ) => ' -1.0E+00'
         *
         *     TO_CHAR( 1, 'FM9.9EEEE' ) => '1.E+00'
         *     TO_CHAR( 1, 'FMS9.9EEEE' ) => '+1.E+00'
         *     TO_CHAR( -1, 'FM9.9EEEE' ) => '-1.E+00'
         *     TO_CHAR( -1, 'FMS9.9EEEE' ) => '-1.E+00'
         */
        sCurResultPtr = ( sResultLastCharPtr + 1 - sExpLen );
    }
    
    *(sCurResultPtr) = 'E';
    (sCurResultPtr)++;
    
    if( sExp < 0 )
    {
        *(sCurResultPtr) = '-';
        sExp = -sExp;
    }
    else
    {
        *(sCurResultPtr) = '+';
    }
    
    (sCurResultPtr)++;
        
    if( sExp == 0 )
    {
        *(sCurResultPtr) = '0';
        (sCurResultPtr)++;
        *(sCurResultPtr) = '0';
    }        
    else if( (sExp > 0) && (sExp < 10) )
    {
        *(sCurResultPtr) = '0';
        (sCurResultPtr)++;
        *(sCurResultPtr) = sNumber[sExp];
    }
    else if( (sExp >= 10) && (sExp < 100) )
    {
        *(sCurResultPtr) = sNumber[sExp/10];
        sExp = sExp - (sExp/10*10);
        (sCurResultPtr)++;
        *(sCurResultPtr) = sNumber[sExp];
    }
    else
    {
        *(sCurResultPtr) = sNumber[sExp/100];
        sExp = sExp - (sExp/100*100);
        (sCurResultPtr)++;
        *(sCurResultPtr) = sNumber[sExp/10];
        sExp = sExp - (sExp/10*10);
        (sCurResultPtr)++;
        *(sCurResultPtr) = sNumber[sExp];
    }
    
    *aSetEEEEExpLen = sExpLen;
    
    STL_DASSERT( sCurResultPtr < ( aResult + aResultLen ) );
}


/**
 * @brief format string을 변환하는 과정 중 지수형태 exponent를 표기한다
 * @param[in]  aFormatInfo       format info
 * @param[in]  aExp              표현할 exponent
 * @param[out] aCurResultPtr     현재 처리할 result 버퍼 포인터
 * @param[out] aSetEEEEExpLen    현재 처리한 EEEE Exp의 길이 반환
 */
void dtfNumberFmtSetEEEEExp( dtfNumberFormatInfo   * aFormatInfo,
                             stlInt32                aExp,
                             stlChar               * aCurResultPtr,
                             stlUInt8              * aSetEEEEExpLen )
{
    stlUInt8   sExpLen     = 4;
    stlInt32   sExp        = aExp;
    stlChar    sNumber[11] = "0123456789";
        
    *(aCurResultPtr) = 'E';
    (aCurResultPtr)++;
    
    if( sExp < 0 )
    {
        *(aCurResultPtr) = '-';
        sExp = -sExp;
    }
    else
    {
        *(aCurResultPtr) = '+';
    }
    
    (aCurResultPtr)++;
        
    if( sExp == 0 )
    {
        *(aCurResultPtr) = '0';
        (aCurResultPtr)++;
        *(aCurResultPtr) = '0';
        (aCurResultPtr)++;
    }        
    else if( (sExp > 0) && (sExp < 10) )
    {
        *(aCurResultPtr) = '0';
        (aCurResultPtr)++;
        *(aCurResultPtr) = sNumber[sExp];
        (aCurResultPtr)++;
    }
    else if( (sExp >= 10) && (sExp < 100) )
    {
        *(aCurResultPtr) = sNumber[sExp/10];
        sExp = sExp - (sExp/10*10);
        (aCurResultPtr)++;
        *(aCurResultPtr) = sNumber[sExp];
        (aCurResultPtr)++;
    }
    else
    {
        *(aCurResultPtr) = sNumber[sExp/100];
        sExp = sExp - (sExp/100*100);
        (aCurResultPtr)++;
        *(aCurResultPtr) = sNumber[sExp/10];
        sExp = sExp - (sExp/10*10);
        (aCurResultPtr)++;
        *(aCurResultPtr) = sNumber[sExp];
        (aCurResultPtr)++;

        sExpLen = DTF_NUMBER_FMT_EEEE_STR_LEN;
    }

    *aSetEEEEExpLen = sExpLen;
}


    
/**
 * @brief double 을 EEEE, RN, rn이 아닌 format string에 맞게 string으로 변환한다.
 * @param[in]  aDataValue   dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharDoubleToFormatString( dtlDataValue         * aDataValue,
                                         dtfNumberFormatInfo  * aFormatInfo,
                                         stlInt64               aAvailableSize,
                                         stlChar              * aResult,
                                         stlInt64             * aLength,
                                         dtlFuncVector        * aVectorFunc,
                                         void                 * aVectorArg,
                                         stlErrorStack        * aErrorStack )
{
    dtlDoubleType   sValue;
    stlFloat64      sPow;
    stlBool         sIsNegative              = STL_FALSE;
    stlInt64        sInteger                 = 0;
    stlBool         sIsNegativeExp           = STL_FALSE;
    stlInt32        sExponent                = 0;
    stlInt64        sTotDigitCount           = 0;
    stlInt64        sIntDigitCount           = 0;
    stlUInt8        sCarry                   = 0;
    stlInt32        i                        = 0;
    stlInt32        sTmpValue                = 0;
    stlInt32        sRemainder               = 0;
    stlInt32        sTrailingZeroLen         = 0;
    stlInt32        sZeroLen                 = 0;
    stlInt32        sOrgDigitCntAfterDecimal = 0;
    stlInt32        sDigitCntAfterDecimal    = 0;
    stlInt32        sDigitCntBeforeDecimal   = 0;
    stlInt32        sResultLen               = aFormatInfo->mToCharResultLen;
    stlInt32        sReduceLen               = 0;
    stlInt32        sExtraDigitLen           = 0;
    stlInt32        sExtraCommaLen           = 0;
    stlInt32        sExtraZeroLen            = 0;
    stlInt32        sExtraSpaceLen           = 0;
    stlInt32        sRoundDigit              = 0;
    stlChar       * sResult                  = aResult;
    stlChar       * sTmpStr                  = NULL;
    stlChar       * sFormatStrEnd            = aFormatInfo->mStr + aFormatInfo->mStrLen -1;
    stlBool         sPlusOneDigitForRound    = STL_FALSE;
    stlBool         sIsSetBlank              = STL_FALSE;
    stlBool         sNumberIsZero            = STL_FALSE;
    stlBool         sLastSpaceUsed           = STL_FALSE;
    stlBool         sIsPrintZero             = STL_FALSE;
 

    sValue = *((dtlDoubleType*) aDataValue->mValue);

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        STL_TRY_THROW( aAvailableSize >= DTL_BINARY_REAL_INFINITY_SIZE, ERROR_NOT_ENOUGH_BUFFER );
        stlMemcpy( (stlChar*)aResult, DTL_BINARY_REAL_INFINITY, DTL_BINARY_REAL_INFINITY_SIZE );
        *aLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }

    if( sValue == 0.0 )
    {       
        sIsNegative            = STL_FALSE;
        sNumberIsZero          = STL_TRUE;
        sDigitCntAfterDecimal  = aFormatInfo->mDigitCntAfterDecimal;

        if( ( (aFormatInfo->mDigitCntBeforeDecimal > 0) && (sDigitCntAfterDecimal == 0 ) ) ||
            ( aFormatInfo->mZeroStart > 0 ) )
        {
            /*
             * 예) TO_CHAR( 0, '9' ) => ' 0'
             *     TO_CHAR( 0, '0' ) => ' 0'
             *     TO_CHAR( 0, '0.999' ) => ' 0.000'
             */     
            sDigitCntBeforeDecimal = 1;
        }
        else
        {
            /* 예) TO_CHAR( 0, '.999' ) => ' .000' */
            sDigitCntBeforeDecimal = 0;
        }
        STL_THROW( RAMP_VALUE_IS_ZERO );
        
    }

    /* STL_DBL_DIG만큼의 유효숫자(trailing zero는 제거한 상태)와 
     * exponent의 형태로 만든다. */
    if( sValue < 0.0 )
    {
        sIsNegative = STL_TRUE;
        sValue = -sValue;
    }
    else
    {
        sIsNegative = STL_FALSE;
    }

    sTotDigitCount = stlLog10( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }


    sExponent = sTotDigitCount - STL_DBL_DIG;
    if( sExponent > STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent - STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else if( sExponent < -STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( -STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent + STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else
    {
        DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }

    DTL_IS_VALID( stlGet10Pow( STL_DBL_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_DBL_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sIsNegative      sValue의 음수여부
     * sInteger         sValue의 STL_DBL_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값
     * */

    if( sIsNegativeExp == STL_TRUE )
    {
        sDigitCntAfterDecimal = aFormatInfo->mDigitCntAfterDecimal;
        if( sExponent <= sIntDigitCount )
        {
            /* XXX.XXX */

            sZeroLen                 = 0;
            sDigitCntBeforeDecimal   = sIntDigitCount - sExponent;
            sOrgDigitCntAfterDecimal = sIntDigitCount - sDigitCntBeforeDecimal;
            
            if( sOrgDigitCntAfterDecimal > sDigitCntAfterDecimal )
            {
                /*
                 * round 되는 부분의 값이 4 보다 큰지 ( 크다면 )
                 * round 되는 자리부터 .x 까지 계속 9값이어서
                 * 최후에 정수 값 + 1 이 되는지 확인
                 */
                i = sOrgDigitCntAfterDecimal - sDigitCntAfterDecimal;
                if( i > 1 ) /* 1 자리 이상 차이 나는 경우 */
                {
                    sTmpValue = i > STL_DBL_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                }
                else 
                {
                    sTmpValue = sInteger;
                }
                
                sRemainder = sTmpValue % 10;
                sTmpValue  = sTmpValue / 10;

                if( ( sExponent == sIntDigitCount ) && ( sTmpValue == 0 ) )
                {
                    if( sRemainder < 5 )
                    {
                        sNumberIsZero          = STL_TRUE;
                        sDigitCntAfterDecimal  = aFormatInfo->mDigitCntAfterDecimal;

                        if( ( (aFormatInfo->mDigitCntBeforeDecimal > 0) && (sDigitCntAfterDecimal == 0 ) ) ||
                            ( aFormatInfo->mZeroStart > 0 ) )
                        {
                            /*
                             * 예) TO_CHAR( 0, '9' ) => ' 0'
                             *     TO_CHAR( 0, '0' ) => ' 0'
                             *     TO_CHAR( 0, '0.999' ) => ' 0.000'
                             */     
                            sDigitCntBeforeDecimal = 1;
                        }
                        else
                        {
                            /* 예) TO_CHAR( 0, '.999' ) => ' .000' */
                            sDigitCntBeforeDecimal = 0;
                        }
                        STL_THROW( RAMP_VALUE_IS_ZERO );
                    }
                    else
                    {
                        sDigitCntBeforeDecimal++;
                        sPlusOneDigitForRound  = STL_TRUE;
                    }
                }
                else
                {

                    if( sRemainder > 4 )
                    {
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }               

                    /* sTmpValue /= 10; */
                    while( ( sCarry == 1 ) && ( sTmpValue > 9 ) )
                    {
                        if( ( sTmpValue % 10 ) == 9 )
                        {
                            sTmpValue /= 10;
                            sCarry = 1;
                        }
                        else
                        {
                            sCarry = 0;
                        }
                    }

                    /* 정수가 한자리 늘어나는 경우
                     * ex) 999.999 -> 1000 */
                    if( ( sCarry == 1 ) && ( sTmpValue == 9 ) )
                    {
                        sDigitCntBeforeDecimal++;
                        sPlusOneDigitForRound  = STL_TRUE;
                    }
                    else
                    {
                        sPlusOneDigitForRound = STL_FALSE;
                    }
                }
            }
        }
        else
        {
            /* .0XXX */
            
            sZeroLen                 = sDigitCntAfterDecimal;
            sOrgDigitCntAfterDecimal = sExponent;

            if( ( aFormatInfo->mDigitCntBeforeDecimal > 0 ) &&
                ( sDigitCntAfterDecimal == 0 ) )
            {
                sDigitCntBeforeDecimal = 1;
            }
            else
            {
                sDigitCntBeforeDecimal = 0;
            }

            if( (sOrgDigitCntAfterDecimal > sDigitCntAfterDecimal) )
            {
                /*
                 * round 되는 부분의 값이 4 보다 큰지 ( 크다면 )
                 * round 되는 자리부터 .x 까지 계속 9값이어서
                 * 최후에 정수 값 + 1 이 되는지 확인
                 */
                i = sOrgDigitCntAfterDecimal - sDigitCntAfterDecimal;
                if( i > 1 ) /* 1 자리 이상 차이 나는 경우 */
                {
                    sTmpValue = i > STL_DBL_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                }
                else 
                {
                    sTmpValue = sInteger;
                }

                sRemainder = sTmpValue % 10;
                sTmpValue  = sTmpValue / 10;

                if( (sRemainder < 5)  && (sTmpValue == 0) && (aFormatInfo->mZeroEndAfterDecimal == 0) )
                {
                    sNumberIsZero          = STL_TRUE;
                    sDigitCntAfterDecimal  = aFormatInfo->mDigitCntAfterDecimal;

                    if( ( (aFormatInfo->mDigitCntBeforeDecimal > 0) && (sDigitCntAfterDecimal == 0 ) ) ||
                        ( aFormatInfo->mZeroStart > 0 ) )
                    {
                        /*
                         * 예) TO_CHAR( 0, '9' ) => ' 0'
                         *     TO_CHAR( 0, '0' ) => ' 0'
                         *     TO_CHAR( 0, '0.999' ) => ' 0.000'
                         */     
                        sDigitCntBeforeDecimal = 1;
                    }
                    else
                    {
                        /* 예) TO_CHAR( 0, '.999' ) => ' .000' */
                        sDigitCntBeforeDecimal = 0;
                    }
                    STL_THROW( RAMP_VALUE_IS_ZERO );
                }

                if( sRemainder > 4 )
                {
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }               

                /* sTmpValue /= 10; */
                while( ( sCarry == 1 ) && ( sTmpValue > 9 ) )
                {
                    if( ( sTmpValue % 10 ) == 9 )
                    {
                        sTmpValue /= 10;
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }
                }

                /* 정수자리 값이 증가 하는 경우 */
                if( ( sCarry == 1 ) && ( sTmpValue == 9 ) )
                {
                    sPlusOneDigitForRound  = STL_TRUE;
                }
                else
                {
                    sPlusOneDigitForRound = STL_FALSE;
                }
            }
            else
            {
                /* Do Nothing */
            }

        }
    }
    else
    {
        /* XXX000 */
        sDigitCntBeforeDecimal = sIntDigitCount + sExponent;
    }

    STL_RAMP( RAMP_VALUE_IS_ZERO );

    /* B Format */
    if( DTF_NUMBER_FMT_FLAG_IS_BLANK( aFormatInfo ) == STL_TRUE )
    {
        if( sNumberIsZero == STL_TRUE )
        {
            /* to_char( 0 , 'B' ) */
            sIsSetBlank = STL_TRUE;
        }
        else
        {
           /* do nothing */
        }

        if( sIsSetBlank == STL_TRUE )
        {
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
            {
                sReduceLen = -aFormatInfo->mToCharResultLen;
            }
            else
            {
                stlMemset( sResult , ' ' , sResultLen );
            }
            
            *aLength = aFormatInfo->mToCharResultLen + sReduceLen;
            STL_THROW( RAMP_END );
        }
        else
        {
            /* do nothing */
        }
        
    }

    /*
     * 정수 부분의 digit format 숫자 보다 실제 value 의 유효숫자 갯수가 많은 경우 '#' 표기.
     */
    if( sDigitCntBeforeDecimal > aFormatInfo->mDigitCntBeforeDecimal )
    {
        /* to_char( 12345 , '999' ) -> '#####' */
        stlMemset( sResult , '#' , aFormatInfo->mToCharResultLen );

        *aLength = aFormatInfo->mToCharResultLen;
        STL_THROW( RAMP_END );
    }
    else
    {
        /* do nothing */
    }


    /*
     * 공백 & 부호 표기 & 0인 숫자에 대한 '0' 표기
     */

    /* 공백 */

    /* sExtraDigitLen -> 공백으로 표현되는 길이
     * sExtraZeroLen  -> 공백부분에서 zero 로 표현되는 길이 */
    if( aFormatInfo->mZeroStart == 0 )
    {
        /* '0' format 이 없는 경우 */

        /* 예) TO_CHAR( 12345, '9999999' ) => 'sign__12345' => '   12345' */
        sExtraDigitLen = ( aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal );
        sExtraZeroLen = 0;
    }
    else
    {
        /*
         * '0' format 이 있는 경우
         */
        
        if( aFormatInfo->mZeroStart == 1 )
        {
            /* '0' format 이 첫번째에 오는 경우  */
        
            /* 예) TO_CHAR( 12345, '0999999' ) => 'sign0012345' => ' 0012345' */
            sExtraDigitLen = 0;
            sExtraZeroLen = (aFormatInfo->mDigitCntBeforeDecimal - sExtraDigitLen - sDigitCntBeforeDecimal );
        }
        else
        {
            /* '0' format 이 두번째 이후에 오는 경우  */
        
            sExtraDigitLen = aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal;
        
            if( sExtraDigitLen >= aFormatInfo->mZeroStart )
            {
                /* 예) TO_CHAR( 123, '9909999' ) => 'sign__00123' => '   00123' */
                sExtraDigitLen = (aFormatInfo->mZeroStart - 1);
                sExtraZeroLen =
                    (aFormatInfo->mDigitCntBeforeDecimal - sExtraDigitLen - sDigitCntBeforeDecimal);
            }
            else
            {
                /* 예) TO_CHAR( 123456, '9909999' ) => 'sign_123456' => '  123456' */
                sExtraDigitLen = (aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal);
                sExtraZeroLen = 0;
            }
        }
    }

    if( aFormatInfo->mFirstZeroOrNine != NULL )
    {
        sTmpStr = aFormatInfo->mFirstZeroOrNine;
    }
    else
    {
        sTmpStr = aFormatInfo->mStr;

        while( STL_TRUE )
        {
        
            if( ( *sTmpStr == 'S' ) || ( *sTmpStr == 's' ) || (*sTmpStr == '$') )
            {
                sTmpStr++;
                continue;
            }
        
            if( ( *sTmpStr == 'F' ) || ( *sTmpStr == 'f' ) )
            {
                if( ( *(sTmpStr+1) == 'M' ) || ( *(sTmpStr+1) == 'm' ) )
                {
                    sTmpStr += 2;
                    continue;
                }
                else
                {
                    STL_DASSERT( ( ( *sTmpStr == 'M' ) || ( *sTmpStr == 'm' ) ) );
                }
            }
            else
            {
                /* do nothing */
            }
            break;
        }
    }

    i = sExtraDigitLen;
    sExtraCommaLen = 0;

    /**
     * reading blank 가 있는 경우 decimal point 이전 '9' '0' 을 skip
     */
    while( i > 0 )
    {
        if( (*sTmpStr == '9') || (*sTmpStr == '0' ) )
        {
            i--;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
        else if( *sTmpStr == ',' )
        {
            while( *sTmpStr == ',' )
            {
                sExtraCommaLen++;
                sTmpStr++;
                DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            }
        }
        else
        {
            STL_DASSERT( STL_FALSE );
        }
    }

    if( sExtraDigitLen > 0 )
    {
        while( *sTmpStr == ',' )
        {
            sExtraCommaLen++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    sExtraSpaceLen = sExtraDigitLen + sExtraCommaLen;

    /**
     * FM 포맷에 의한 reading blank 제거
     */
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        sResultLen -= sExtraSpaceLen;
        sReduceLen -= sExtraSpaceLen;
        sExtraSpaceLen = 0;

        /**
         * 부호 길이 적용
         * SIGN 과 같이 쓰이는 경우 '+' 값도 자리수가 유효하다
         */
        
        if( DTF_NUMBER_FMT_FLAG_IS_SIGN( aFormatInfo ) == STL_FALSE )
        {
            sReduceLen -= sIsNegative == STL_TRUE ? 0 : 1;
        }
        else
        {
            /* do nothing */
        }

        if( DTF_NUMBER_FMT_FLAG_IS_BRACKET( aFormatInfo ) == STL_TRUE )
        {
            sReduceLen -= sIsNegative == STL_TRUE ? 0 : 1;
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* do nothing */
    }
    
    stlMemset( sResult, ' ', sExtraSpaceLen );
    sResult += sExtraSpaceLen;
    
    /**
     * 부호 표기 ( +, -, PR, MI )
     */

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        dtfNumberFmtSetSignForNormal( aFormatInfo, aResult, sResultLen, !sIsNegative, sResult, &sResult );
    }
    else
    {
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  sResultLen,
                                  !sIsNegative,
                                  STL_TRUE, // aIsSignLocatePre
                                  &sResultLen,
                                  sResult,
                                  &sResult );            
    }

    /**
     * DOLLAR 표기
     */

    DTF_NUMBER_FMT_SET_DOLLAR( aFormatInfo, sResult );

    /**
     * 0인 숫자에 대한 '0' 표기
     */

    if( sExtraZeroLen > 0 )
    {
        while( sExtraZeroLen > 0 )
        {
            if( ( *sTmpStr == '0' ) || ( *sTmpStr == '9' ) )
            {
                *sResult = '0';

                sResult++;
                sTmpStr++;
                DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
                sExtraZeroLen--;
            }
            else if( *sTmpStr == ',' )
            {
                DTF_NUMBER_FMT_SET_COMMA( sResult , sTmpStr );
            }
            else
            {
                STL_DASSERT( STL_FALSE );
            }
        }

        DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
    }
    else
    {
        /* do nothing */
    }

    /**
     * 숫자 표기 ( 0 인 경우 )
     */
    
    if( sNumberIsZero == STL_TRUE )
    {
        if( sDigitCntBeforeDecimal == 1 )
        {
            if( ( *sTmpStr == '0' ) || ( *sTmpStr == '9' ) )
            {
                *sResult = '0';
                sTmpStr++;
            }
            else
            {
                /* Do Nothing */
            }
            
            while( sTmpStr <= sFormatStrEnd )
            {
                if( (*sTmpStr == ',') || (*sTmpStr == '.') )
                {
                    sResult++;
                    *sResult = *sTmpStr;
                    sTmpStr++;
                }
                else if( *sTmpStr == '$' )
                {
                    sTmpStr++;
                    continue;
                }
                else
                {
                    STL_DASSERT( ( *sTmpStr != 'B' ) && ( *sTmpStr != 'b' ) );
                    break;
                }
            }
        }
        else
        {
            if( (aFormatInfo->mDigitCntBeforeDecimal > 0) &&
                (DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE) )
            {
                *sResult = '0';
                sResult++;
                sReduceLen++;
            }
        }
        
        if( (DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE) && (sDigitCntAfterDecimal > 0) )
        {        
            if( aFormatInfo->mToCharResultLen + sReduceLen > 0 )
            {
                *sResult = '.';
                sResult++;
            }

            sZeroLen = aFormatInfo->mZeroEndAfterDecimal > sDigitCntAfterDecimal ?
                sDigitCntAfterDecimal : aFormatInfo->mZeroEndAfterDecimal;
            sReduceLen -= (sDigitCntAfterDecimal - sZeroLen );
            
            while( sZeroLen > 0 )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }
        else if( sDigitCntAfterDecimal > 0 )
        {
            sZeroLen = sDigitCntAfterDecimal;
            
            STL_DASSERT( ( ( sDigitCntBeforeDecimal == 0 ) && ( *sTmpStr == '.' ) ) ||
                         ( ( sDigitCntBeforeDecimal == 1 ) && ( *sTmpStr != '.' ) ) );
            
            *sResult = '.';
            sResult++;
            
            while( sZeroLen > 0 )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }
        else
        {
            if( sTmpStr <= sFormatStrEnd )
            {
                if( *sTmpStr == '.' )
                {
                    *sResult = *sTmpStr;
                    sResult++;
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
        }
        
        STL_THROW( RAMP_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }


     /**
     * 숫자 표기 ( 0 이 아닌 경우 )
     */

    DTF_NUMBER_FMT_LAST_SPACE_USED( aFormatInfo, sLastSpaceUsed );

    /* 포맷에 의해 마지막 공간에 부호 혹은 ' ' Blank 가 set 되는 경우 */
    if( sLastSpaceUsed == STL_TRUE )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_SIGN( aFormatInfo ) == STL_TRUE )
        {
            sResultLen--;
            sFormatStrEnd--;
        }
        else
        {
            /* MI , PR */
            sResultLen--;
            sFormatStrEnd -= 2;
        }
    }
    else
    {
        /* do nothing */
    }

    sResult = aResult + sResultLen -1;
    sTmpStr = sFormatStrEnd;

    DTF_NUMBER_FMT_REWIND_V_FORMAT_STR( sTmpStr );

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent <= sIntDigitCount )
        {
            sRoundDigit = sDigitCntBeforeDecimal + sDigitCntAfterDecimal;
        
            /*  round 가 발생하는 경우
             *  round 되는 값을 미리 keep */
            if( sDigitCntAfterDecimal < sOrgDigitCntAfterDecimal )
            {
                i = sIntDigitCount - sRoundDigit;
                if( i > 1 ) /* 1 자리 이상 차이 나는 경우 */
                {
                    sInteger = i > STL_DBL_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                }
                else 
                {
                    /* do nothing */
                }

                sRemainder = sInteger % 10;
                sInteger   = sInteger / 10;
                

                if( sRemainder > 4 )
                {
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }
            }
            else
            {
                sCarry = 0;
            }

            
            sExponent  = sDigitCntAfterDecimal;

            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

            sZeroLen = sDigitCntAfterDecimal - sOrgDigitCntAfterDecimal;
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
            {
                /**
                 * FM format : '.' 이후 제로는 '0' 이전에 '9' 가 있더라도
                 * 계속 제로를 출력 한다.
                 */
                if( *sFormatStrEnd == '0' )
                {
                    sIsPrintZero = STL_TRUE;
                }
                else
                {
                    /* do nothing */
                }

                /* print trailing zero */
                while( sZeroLen > 0 )
                {
                    DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

                    if( sIsPrintZero == STL_FALSE )
                    {
                        if( *sTmpStr == '0' )
                        {
                            sIsPrintZero = STL_TRUE;
                        }
                        else
                        {
                            /* do nothing */
                        }
                    }
                    else
                    {
                        /* do nothing */
                    }

                    if( sIsPrintZero == STL_TRUE )
                    {
                        *sResult = '0';
                        sIsPrintZero = STL_TRUE;
                    }
                    else
                    {
                        sReduceLen--;
                    }

                    sRoundDigit--;
                    sExponent--;
                    sTmpStr--;
                    sResult--;
                    sZeroLen--;
                }

                /* 소수점 digit 출력 */
                while( (sExponent > 0) && (sIntDigitCount > 0 ) )
                {
                    DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

                    if( sIsPrintZero == STL_FALSE )
                    {
                        if( *sTmpStr == '0' )
                        {
                            sIsPrintZero = STL_TRUE;
                        }
                        else
                        {
                            /* do nothing */
                        }
                    }
                    else
                    {
                        /* do nothing */
                    }
                
                    sTmpValue = sInteger % 10;
                    if( (sIsPrintZero == STL_FALSE ) && (sTmpValue == 0) )
                    {
                        sReduceLen--;
                    }

                    if( (sIsPrintZero == STL_TRUE) || sTmpValue != 0 )
                    {
                        *sResult = sTmpValue + '0';
                        sIsPrintZero = STL_TRUE;
                    }
                    else
                    {
                        /* do nothing */
                    }

                    sInteger = sInteger / 10;

                    sTmpStr--;
                    sExponent--;
                    sRoundDigit--;
                    sIntDigitCount--;
                    sResult--;
                }
            }
            else
            {
                /* trailing zero 출력 */
                while( sZeroLen > 0 )
                {
                    DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
                
                    *sResult = '0';
                
                    sTmpStr--;
                    sExponent--;
                    sRoundDigit--;
                    sResult--;
                    sZeroLen--;

                }

                /* 소수점 digit 출력 */
                while( (sExponent > 0) && (sIntDigitCount > 0 ) )
                {
                    DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
                
                    sTmpValue = sInteger % 10;
                    *sResult = sTmpValue + '0';

                    sInteger = sInteger / 10;

                    sTmpStr--;
                    sExponent--;
                    sRoundDigit--;
                    sIntDigitCount--;
                    sResult--;

                }
            }

            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

            if( sDigitCntAfterDecimal > 0 )
            {
                STL_DASSERT( *sTmpStr == '.' );
                
                *sResult = '.';
                sResult--;
                sTmpStr--;
            }
            else
            {
                if( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aFormatInfo ) == STL_TRUE )
                {
                    STL_DASSERT( *sTmpStr == '.' );

                    *sResult = '.';
                    sResult--;
                    sTmpStr--;
                }
                else
                {
                    // ?????
                    /* Do Nothing */
                }
            }           

            /* 정수부 출력 */
            while( ( sRoundDigit > 0 ) && ( sIntDigitCount > 0 ) )
            {
                DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
                
                DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( sResult , sTmpStr );
                
                sTmpValue = sInteger % 10;
                *sResult = sTmpValue + '0';               

                sInteger = sInteger / 10;
                sTmpStr--;
                sExponent--;
                sRoundDigit--;
                sIntDigitCount--;
                sResult--;
            }

            if( sCarry == 1 )
            {                   
                sRoundDigit =
                    sDigitCntBeforeDecimal + sDigitCntAfterDecimal -
                    ( sPlusOneDigitForRound == STL_FALSE ? 0 : 1 ); 

              
                sResult = aResult + sResultLen -1;


                while( ( sCarry == 1 ) && ( sRoundDigit > 0 ) )
                {
                    if( ( *sResult == '.' ) || ( *sResult == ',' ) )
                    {
                        sResult--;

                        while( *sResult == ',' )
                        {
                            sResult--;
                        }

                        continue;
                    }

                    *sResult += sCarry;

                    if( *sResult > '9' )
                    {
                        *sResult = '0';
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }

                    sRoundDigit--;
                    sResult--;
                }
                    
                if( (sRoundDigit == 0) && (sCarry == 1) )
                {
                    if( ( *sResult == '.' ) || ( *sResult == ',' ) )
                    {
                        sResult--;

                        while( *sResult == ',' )
                        {
                            sResult--;
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }

                    *sResult = '1';
                }
                else
                {
                    /* Do Nothing */
                }

                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                {
                    /**
                     * Carry 에 의해 생성된 '0' 을 제거
                     */
                    sResult = aResult + sResultLen -1;
                    while( (sDigitCntAfterDecimal > 0) && (*sFormatStrEnd != '0') && (*sResult == '0') )
                    {
                        sResult--;
                        sReduceLen--;
                        sFormatStrEnd--;
                    }
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            /**
             * 0.0XXXX
             */
            sRoundDigit = sDigitCntAfterDecimal;
        
            /**
             * sCarry : round trucated 로 반올림이 발생하는 경우
             *          sZeroLen 가 줄어든다.
             */
            sZeroLen = sExponent - sIntDigitCount - sCarry;

            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
            {
                if( *sFormatStrEnd == '0' )
                {
                    sIsPrintZero = STL_TRUE;
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* do nothing */
            }

            /**
             * 제로길이가 sDigitCntAfterDecimal 보다
             * 크다면 0만 출력
             */
            if( sZeroLen >= sDigitCntAfterDecimal )
            {
                sZeroLen = sDigitCntAfterDecimal;
                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                {
                    while( sZeroLen > 0 )
                    {
                        if( sIsPrintZero == STL_FALSE )
                        {
                            if( *sTmpStr == '0' )
                            {
                                sIsPrintZero = STL_TRUE;
                            }
                            else
                            {
                                /* do nothing */
                            }
                        }
                        
                        if( sIsPrintZero == STL_TRUE )
                        {
                            *sResult = '0'; 
                        }
                        else
                        {
                            sReduceLen--;
                        }
                        
                        sResult--;
                        sZeroLen--;
                        sTmpStr--;
                    }
                    
                }
                else
                {
                    while( sZeroLen > 0 )
                    {
                        *sResult = '0';
                        sResult--;
                        sZeroLen--;
                    }
                }
            }
            else
            {
                sZeroLen = sExponent - sIntDigitCount;

                /* write trailing zero */
                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                {
                    sTrailingZeroLen = sDigitCntAfterDecimal - sOrgDigitCntAfterDecimal;
                    while( sTrailingZeroLen > 0 )
                    {
                        if( sIsPrintZero == STL_FALSE )
                        {
                            if( *sTmpStr == '0' )
                            {
                                sIsPrintZero = STL_TRUE;
                            }
                            else
                            {
                                /* do nothing */
                            }
                        }
                        else
                        {
                            /* do nothing */
                        }

                        if( sIsPrintZero == STL_TRUE )
                        {
                            *sResult = '0';
                            sIsPrintZero = STL_TRUE;
                        }
                        else
                        {
                            sReduceLen--;
                        }
                        
                        sResult--;
                        sTrailingZeroLen--;
                        sTmpStr--;
                    }
                }
                else
                {
                    sTrailingZeroLen = sDigitCntAfterDecimal - sOrgDigitCntAfterDecimal;
                    while( sTrailingZeroLen > 0 )
                    {
                        *sResult = '0';
                        sResult--;
                        sTrailingZeroLen--;
                    }          
                }

                /*  round 가 발생하는 경우
                 *  round 되는 값을 미리 keep */
                if( sDigitCntAfterDecimal < sOrgDigitCntAfterDecimal )
                {
                    i = sOrgDigitCntAfterDecimal - sDigitCntAfterDecimal;
                    if( i > 1 ) /* 1 자리 이상 차이 나는 경우 */
                    {
                        sTmpValue = i > STL_DBL_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                    }
                    else 
                    {
                        sTmpValue = sInteger;
                    }

                    sRemainder = sTmpValue % 10;
                    sInteger   = sTmpValue / 10;
                    sIntDigitCount -= i;
                

                    if( sRemainder > 4 )
                    {
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }
                }
                else
                {
                    sCarry = 0;
                }               

                /* 소수 출력 */
                while( (sRoundDigit > 0) && (sIntDigitCount > 0 ) )
                {
                    sTmpValue = sInteger % 10;
                    *sResult = sTmpValue + '0';

                    sInteger = sInteger / 10;
                    sRoundDigit--;
                    sIntDigitCount--;
                    sResult--;
                }

                /* write reading zero */
                while( sZeroLen > 0 )
                {
                    *sResult = '0';
                    sResult--;
                    sZeroLen--;
                    sRoundDigit--;
                }

            }

            if( sDigitCntAfterDecimal > 0 )
            {
                *sResult = '.';
                sResult--;
            }
            else
            {
                /* Do Nothing */
            }

            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );

            if( sCarry == 1 )
            {                    
                sRoundDigit = sDigitCntAfterDecimal;

                sResult = aResult + sResultLen -1;
                while( ( sCarry == 1 ) && ( sRoundDigit > 0 ) )
                {
                    *sResult += sCarry;

                    if( *sResult > '9' )
                    {
                        *sResult = '0';
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }

                    sRoundDigit--;
                    sResult--;
                }
                                    
                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                {
                    sResult = aResult + sResultLen -1;
                    while( ( *sFormatStrEnd != '0' ) && ( *sResult == '0' ) )
                    {
                        sResult--;
                        sReduceLen--;
                        sFormatStrEnd--;
                    }
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* do nothing */
            }
        }
    }
    else
    {
        sRoundDigit = sDigitCntBeforeDecimal;
        
        sTmpStr = sFormatStrEnd;
        sExponent  = sDigitCntAfterDecimal;

        DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

        if( aFormatInfo->mDigitCntAfterDecimal > 0 )
        {
            sZeroLen = aFormatInfo->mDigitCntAfterDecimal;
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
            {
                if( *sTmpStr == '0' )
                {
                    while( sZeroLen > 0 )
                    {
                        *sResult = '0';
                
                        sTmpStr--;
                        sExponent--;
                        sResult--;
                        sZeroLen--;

                        DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
                    }
                }
                else
                {
                    while( sZeroLen > 0 )
                    {
                        DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

                        sReduceLen--;
                        sTmpStr--;
                        sExponent--;
                        sResult--;
                        sZeroLen--;
                    }
                }
            }
            else
            {
                while( sZeroLen > 0 )
                {
                    *sResult = '0';
                
                    sTmpStr--;
                    sExponent--;
                    sResult--;
                    sZeroLen--;

                    DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
                }
            }
        }
        else
        {
            /* do nothing */
        }

        DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
        
        while( STL_TRUE )
        {
            if( (*sTmpStr == '.') || (*sTmpStr == ',') )
            {
                if( sRoundDigit > 0 )
                {
                    *sResult = *sTmpStr;
                    sResult--;
                    sTmpStr--;
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                break;
            }
        }

        DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

        /* print trailing zero */
        sZeroLen = sDigitCntBeforeDecimal - sIntDigitCount;
        while( sZeroLen > 0 )
        {
            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

            DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( sResult , sTmpStr );
            
            *sResult = '0';
                
            sTmpStr--;
            sExponent--;
            sRoundDigit--;
            sResult--;
            sZeroLen--;
        }


        
        
        /* 정수부 출력 */
        while( ( sRoundDigit > 0 ) && ( sIntDigitCount > 0 ) )
        {
            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
            
            DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( sResult , sTmpStr );
            
            sTmpValue = sInteger % 10;
            *sResult = sTmpValue + '0';              

            sInteger = sInteger / 10;
            sTmpStr--;
            sExponent--;
            sRoundDigit--;
            sIntDigitCount--;
            sResult--;
        }       
    }

    
    STL_RAMP( RAMP_SUCCESS );

    *aLength = aFormatInfo->mToCharResultLen + sReduceLen;
    
    /**
     * FM format 시 decimal point 이후 '9' format 에 해당하는
     * '0' 문자의 위치를 선계산 할 수 없으므로
     * 문자열 변환이 끝나고 'S,MI,PR' 등을 재 출력 한다.
     */
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        sResult = aResult + ( *aLength - 1 < 0 ? 0 : *aLength -1 );

        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  *aLength,
                                  !sIsNegative,
                                  STL_FALSE,
                                  &sResultLen,
                                  sResult,
                                  &sResult );
    }
    else
    {
        /* do nothing */
    }

    STL_RAMP( RAMP_END );

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    } 

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief native_double type을 V format string에 맞게 string으로 변환한다.
 * @param[in]  aDataValue          dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharDoubleToVFormatString( dtlDataValue         * aDataValue,
                                          dtfNumberFormatInfo  * aFormatInfo,
                                          stlInt64               aAvailableSize,
                                          stlChar              * aResult,
                                          stlInt64             * aLength,
                                          dtlFuncVector        * aVectorFunc,
                                          void                 * aVectorArg,
                                          stlErrorStack        * aErrorStack )
{
    dtlDoubleType   sValue;
    stlFloat64      sPow;
    stlBool         sIsNegative              = STL_FALSE;
    stlInt64        sInteger                 = 0;
    stlBool         sIsNegativeExp           = STL_FALSE;
    stlInt32        sExponent                = 0;
    stlInt64        sTotDigitCount           = 0;
    stlInt64        sIntDigitCount           = 0;
    stlUInt8        sCarry                   = 0;
    stlInt32        i                        = 0;
    stlInt32        sTmpValue                = 0;
    stlInt32        sRemainder               = 0;
    stlInt32        sTrailingZeroLen         = 0;
    stlInt32        sZeroLen                 = 0;
    stlInt32        sOrgDigitCntAfterDecimal = 0;
    stlInt32        sDigitCntAfterV          = 0;
    stlInt32        sDigitCntAfterDecimal    = 0;
    stlInt32        sDigitCntBeforeDecimal   = 0;
    stlInt32        sResultLen               = aFormatInfo->mToCharResultLen;
    stlInt32        sReduceLen               = 0;
    stlInt32        sExtraDigitLen           = 0;
    stlInt32        sExtraCommaLen           = 0;
    stlInt32        sExtraZeroLen            = 0;
    stlInt32        sExtraSpaceLen           = 0;
    stlInt32        sRoundDigit              = 0;
    stlInt32        sReadingZeroLen          = 0;
    stlChar       * sResult                  = aResult;
    stlChar       * sTmpStr                  = NULL;
    stlChar       * sFormatStrEnd            = aFormatInfo->mStr + aFormatInfo->mStrLen -1;
    stlBool         sPlusOneDigitForRound    = STL_FALSE;
    stlBool         sIsSetBlank              = STL_FALSE;
    stlBool         sNumberIsZero            = STL_FALSE;
    stlBool         sLastSpaceUsed           = STL_FALSE;
 

    sValue = *((dtlDoubleType*) aDataValue->mValue);

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        STL_TRY_THROW( aAvailableSize >= DTL_BINARY_REAL_INFINITY_SIZE, ERROR_NOT_ENOUGH_BUFFER );
        stlMemcpy( (stlChar*)aResult, DTL_BINARY_REAL_INFINITY, DTL_BINARY_REAL_INFINITY_SIZE );
        *aLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }

    if( sValue == 0.0 )
    {       
        sIsNegative            = STL_FALSE;
        sNumberIsZero          = STL_TRUE;
        sDigitCntBeforeDecimal = aFormatInfo->mDigitCntAfterV;

        STL_THROW( RAMP_VALUE_IS_ZERO );
        
    }

    /* STL_DBL_DIG만큼의 유효숫자(trailing zero는 제거한 상태)와 
     * exponent의 형태로 만든다. */
    if( sValue < 0.0 )
    {
        sIsNegative = STL_TRUE;
        sValue = -sValue;
    }
    else
    {
        sIsNegative = STL_FALSE;
    }

    sTotDigitCount = stlLog10( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }


    sExponent = sTotDigitCount - STL_DBL_DIG;
    if( sExponent > STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent - STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else if( sExponent < -STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( -STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent + STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else
    {
        DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }

    DTL_IS_VALID( stlGet10Pow( STL_DBL_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_DBL_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sIsNegative      sValue의 음수여부
     * sInteger         sValue의 STL_DBL_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값
     * */

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent <= sIntDigitCount )
        {
            /* XXX.XXX */

            sTrailingZeroLen = ( aFormatInfo->mDigitCntAfterV - sExponent ) > 0 ?
                               aFormatInfo->mDigitCntAfterV - sExponent : 0;
            sDigitCntBeforeDecimal   = sIntDigitCount - sExponent + aFormatInfo->mDigitCntAfterV;
            sOrgDigitCntAfterDecimal = sExponent;

            if( sOrgDigitCntAfterDecimal > aFormatInfo->mDigitCntAfterV )
            {
                i = sOrgDigitCntAfterDecimal - aFormatInfo->mDigitCntAfterV;
                if( i > 1 )
                {
                    sTmpValue = i > STL_DBL_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                }
                else
                {
                    sTmpValue = sInteger;
                }
                                
                sRemainder = sTmpValue % 10;

                if( sRemainder > 4 )
                {
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }               

                sTmpValue /= 10;
                while( ( sCarry == 1 ) && ( sTmpValue > 9 ) )
                {
                    if( ( sTmpValue % 10 ) == 9 )
                    {
                        sTmpValue /= 10;
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }
                }

                /* 정수가 한자리 늘어나는 경우
                 * ex) 999.999 -> 1000 */
                if( ( sCarry == 1 ) && ( sTmpValue == 9 ) )
                {
                    sPlusOneDigitForRound  = STL_TRUE;
                    sDigitCntBeforeDecimal++;
                }
                else
                {
                    sPlusOneDigitForRound = STL_FALSE;
                }
            }

            /*
             * Round 로 인한 DigitCnt overflow
             */
            if( (sIntDigitCount - ( sExponent - aFormatInfo->mDigitCntAfterV ) +
                 (sPlusOneDigitForRound == STL_TRUE ? 1 : 0 ) ) > aFormatInfo->mDigitCntBeforeDecimal )
            {
                stlMemset( sResult , '#' , aFormatInfo->mToCharResultLen );

                *aLength = aFormatInfo->mToCharResultLen;
                STL_THROW( RAMP_END );
            }
        }
        else
        {
            /* .0XXXXX */

            sReadingZeroLen = sExponent - sIntDigitCount;
            
            sDigitCntBeforeDecimal   = aFormatInfo->mDigitCntAfterV;
            sOrgDigitCntAfterDecimal = sExponent - aFormatInfo->mDigitCntAfterV;
            
            if( sReadingZeroLen > aFormatInfo->mDigitCntAfterV )
            {
                sNumberIsZero = STL_TRUE;
                STL_THROW( RAMP_VALUE_IS_ZERO );
            }
            else
            {
                /* do nothing */
            }

            if( sOrgDigitCntAfterDecimal > 0 )
            {
                /*
                 * round 되는 부분의 값이 4 보다 큰지 ( 크다면 )
                 * round 되는 자리부터 .x 까지 계속 9값이어서
                 * 최후에 정수 값 + 1 이 되는지 확인
                 */
                i = sOrgDigitCntAfterDecimal;
                if( i > 1) /* 1 자리 이상 차이 나는 경우 */
                {
                    sTmpValue = i > STL_DBL_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                }
                else 
                {
                    sTmpValue = sInteger;
                }
                
                sRemainder = sTmpValue % 10;

                if( sRemainder > 4 )
                {
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }               

                while( ( sCarry == 1 ) && ( sTmpValue > 9 ) )
                {
                    if( ( sTmpValue % 10 ) == 9 )
                    {
                        sTmpValue /= 10;
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }
                }

                /* 정수자리 값이 증가 하는 경우 */
                if( ( sCarry == 1 ) && ( sTmpValue > 4 ) )
                {
                    sPlusOneDigitForRound  = STL_TRUE;
                }
                else
                {
                    sPlusOneDigitForRound = STL_FALSE;
                }
            }
            else
            {
                /* Do Nothing */
            }           
        }
    }
    else
    {
        /* XXX000 */
        sZeroLen               = aFormatInfo->mDigitCntAfterV;
        sTrailingZeroLen       = sExponent;
        sDigitCntBeforeDecimal = sIntDigitCount + sExponent + aFormatInfo->mDigitCntAfterV;;
    }

    STL_RAMP( RAMP_VALUE_IS_ZERO );

    /* B Format */
    if( DTF_NUMBER_FMT_FLAG_IS_BLANK( aFormatInfo ) == STL_TRUE )
    {
        if( sNumberIsZero == STL_TRUE )
        {
            /* to_char( 0 , 'B' ) */
            sIsSetBlank = STL_TRUE;
        }
        else
        {
            if( sIsNegativeExp == STL_TRUE )
            {
                if( sExponent >= sIntDigitCount + aFormatInfo->mDigitCntAfterV )
                {
                    /* .XXXXXX */
                    if( sPlusOneDigitForRound == STL_FALSE) 
                    {
                        sIsSetBlank = STL_TRUE;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* XXX.XXX
                     * do nothing */
                }
            }
            else
            {
                /* XXX000
                 * do nothing */
            }
        }

        if( sIsSetBlank == STL_TRUE )
        {
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
            {
                sReduceLen = -aFormatInfo->mToCharResultLen;
            }
            else
            {
                stlMemset( sResult , ' ' , sResultLen );
            }

            *aLength = aFormatInfo->mToCharResultLen + sReduceLen;
            STL_THROW( RAMP_END );
        }
        else
        {
            /* do nothing */
        }
        
    }

    /*
     * Round 로 인한 DigitCnt overflow
     */
    if( sDigitCntBeforeDecimal > aFormatInfo->mDigitCntBeforeDecimal )
    {
        /* to_char( 12345 , '999' ) -> '#####' */
        stlMemset( sResult , '#' , aFormatInfo->mToCharResultLen );

        *aLength = aFormatInfo->mToCharResultLen;
        STL_THROW( RAMP_END );
    }
    else
    {
        /* do nothing */
    }


    /*
     * 공백 & 부호 표기 & 0인 숫자에 대한 '0' 표기
     */

    /* 공백 */

    if( aFormatInfo->mZeroStart == 0 )
    {
        /* '0' format 이 없는 경우 */

        /* 예) TO_CHAR( 12345, '9999999' ) => 'sign__12345' => '   12345' */
        sExtraDigitLen = ( aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal );
        sExtraZeroLen = 0;
    }
    else
    {
        /*
         * '0' format 이 있는 경우
         */
        
        if( aFormatInfo->mZeroStart == 1 )
        {
            /* '0' format 이 첫번째에 오는 경우  */
        
            /* 예) TO_CHAR( 12345, '0999999' ) => 'sign0012345' => ' 0012345' */
            sExtraDigitLen = 0;
            sExtraZeroLen = (aFormatInfo->mDigitCntBeforeDecimal - sExtraDigitLen - sDigitCntBeforeDecimal );
        }
        else
        {
            /* '0' format 이 두번째 이후에 오는 경우  */
        
            sExtraDigitLen = aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal;
        
            if( sExtraDigitLen >= aFormatInfo->mZeroStart )
            {
                /* 예) TO_CHAR( 123, '9909999' ) => 'sign__00123' => '   00123' */
                sExtraDigitLen = (aFormatInfo->mZeroStart - 1);
                sExtraZeroLen =
                    (aFormatInfo->mDigitCntBeforeDecimal - sExtraDigitLen - sDigitCntBeforeDecimal);
            }
            else
            {
                /* 예) TO_CHAR( 123456, '9909999' ) => 'sign_123456' => '  123456' */
                sExtraDigitLen = (aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal);
                sExtraZeroLen = 0;
            }
        }
    }

    if( aFormatInfo->mFirstZeroOrNine != NULL )
    {
        sTmpStr = aFormatInfo->mFirstZeroOrNine;
    }
    else
    {
        sTmpStr = aFormatInfo->mStr;

        while( STL_TRUE )
        {
            if( ( *sTmpStr == 'S' ) || ( *sTmpStr == 's' ) || (*sTmpStr == '$') )
            {
                sTmpStr++;
                continue;
            }
        
            if( ( *sTmpStr == 'F' ) || ( *sTmpStr == 'f' ) )
            {
                if( ( *(sTmpStr+1) == 'M' ) || ( *(sTmpStr+1) == 'm' ) )
                {
                    sTmpStr += 2;
                    continue;
                }
                else
                {
                    STL_DASSERT( ( ( *sTmpStr == 'M' ) || ( *sTmpStr == 'm' ) ) );
                }
            }
            else
            {
                /* do nothing */
            }

            break;
        }
    }

    i = sExtraDigitLen;
    sExtraCommaLen = 0;   
    
    while( i > 0 )
    {
        if( (*sTmpStr == '9') || (*sTmpStr == '0' ) )
        {
            i--;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
        else if( *sTmpStr == ',' )
        {
            while( *sTmpStr == ',' )
            {
                sExtraCommaLen++;
                sTmpStr++;
                DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            }
        }
        else
        {
            STL_DASSERT( STL_FALSE );
        }
    }

    if( sExtraDigitLen > 0 )
    {
        while( *sTmpStr == ',' )
        {
            sExtraCommaLen++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    sExtraSpaceLen = sExtraDigitLen + sExtraCommaLen;

    /**
     * FM 포맷에 의한 reading blank 제거
     */
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        sResultLen -= sExtraSpaceLen;
        sReduceLen -= sExtraSpaceLen;
        sExtraSpaceLen = 0;

        /**
         * 부호 길이 적용
         * SIGN 과 같이 쓰이는 경우 '+' 값도 자리수가 유효하다
         */
        
        if( DTF_NUMBER_FMT_FLAG_IS_SIGN( aFormatInfo ) == STL_FALSE )
        {
            sReduceLen -= sIsNegative == STL_TRUE ? 0 : 1;
        }
        else
        {
            /* do nothing */
        }

        if( DTF_NUMBER_FMT_FLAG_IS_BRACKET( aFormatInfo ) == STL_TRUE )
        {
            sReduceLen -= sIsNegative == STL_TRUE ? 0 : 1;
        }
        else
        {
            /* do nothing */
        }

        
    }
    else
    {
        /* do nothing */
    }
   
    stlMemset( sResult, ' ', sExtraSpaceLen );
    sResult += sExtraSpaceLen;
    
    /**
     * 부호 표기 ( +, -, PR, MI )
     */

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        dtfNumberFmtSetSignForNormal( aFormatInfo, aResult, sResultLen, !sIsNegative, sResult, &sResult );
    }
    else
    {
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  sResultLen,
                                  !sIsNegative,
                                  STL_TRUE, // aIsSignLocatePre
                                  &sResultLen,
                                  sResult,
                                  &sResult );            
    }

    /**
     * DOLLAR 표기
     */

    DTF_NUMBER_FMT_SET_DOLLAR( aFormatInfo, sResult );

    /**
     * 0인 숫자에 대한 '0' 표기
     */

    if( sExtraZeroLen > 0 )
    {
        while( sExtraZeroLen > 0 )
        {
            if( ( *sTmpStr == '0' ) || ( *sTmpStr == '9' ) )
            {
                *sResult = '0';

                sResult++;
                sTmpStr++;
                DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
                sExtraZeroLen--;
            }
            else if( *sTmpStr == ',' )
            {
                DTF_NUMBER_FMT_SET_COMMA( sResult , sTmpStr );
            }
            else
            {
                STL_DASSERT( STL_FALSE );
            }
        }

        DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
    }
    else
    {
        /* do nothing */
    }

    /**
     * 숫자 표기 ( 0 인 경우 )
     */

    if( sNumberIsZero == STL_TRUE )
    {
        while( sDigitCntBeforeDecimal > 0 )
        {
            *sResult = '0';
            sResult++;
            sDigitCntBeforeDecimal--;
        }
        
        STL_THROW( RAMP_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }
    
     /**
     * 숫자 표기 ( 0 이 아닌 경우 )
     */

    DTF_NUMBER_FMT_LAST_SPACE_USED( aFormatInfo, sLastSpaceUsed );

    /* 포맷에 의해 마지막 공간에 부호 혹은 ' ' Blank 가 set 되는 경우 */
    if( sLastSpaceUsed == STL_TRUE )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_SIGN( aFormatInfo ) == STL_TRUE )
        {
            sResultLen--;
            sFormatStrEnd--;
        }
        else
        {
            /* MI , PR */
            sResultLen--;
            sFormatStrEnd -= 2;
        }
    }
    else
    {
        /* do nothing */
    }

    sResult = aResult + sResultLen -1;
    sTmpStr = sFormatStrEnd;

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent <= sIntDigitCount )
        {
            sRoundDigit = sDigitCntBeforeDecimal;
        
            if( sOrgDigitCntAfterDecimal > aFormatInfo->mDigitCntAfterV )
            {
                i = sOrgDigitCntAfterDecimal - aFormatInfo->mDigitCntAfterV;
                if( i > 1 ) /* 1 자리 이상 차이 나는 경우 */
                {
                    sInteger = i > STL_DBL_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                }
                else 
                {
                    /* do nothing */
                }

                sRemainder = sInteger % 10;
                sInteger   = sInteger / 10;
                

                if( sRemainder > 4 )
                {
                    sInteger++;
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* do nothing */
            }

            sDigitCntAfterV = aFormatInfo->mDigitCntAfterV;

            /* print trailing zero */
            while( sTrailingZeroLen > 0 )
            {
                DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
                
                *sResult = '0';
                
                sTmpStr--;
                sRoundDigit--;
                sResult--;
                sTrailingZeroLen--;
                sDigitCntAfterV--;

            }

            /* V 포맷 정수 출력 */
            while( sDigitCntAfterV > 0 )
            {
                DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

                sTmpValue = sInteger % 10;
                *sResult = sTmpValue + '0';               

                sInteger = sInteger / 10;

                sTmpStr--;
                sRoundDigit--;
                sResult--;
                sDigitCntAfterV--;
            }

            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

            if( ( *sTmpStr == 'V' ) || ( *sTmpStr == 'v' ) )
            {
                sTmpStr--;
            }
            else
            {
                /* do nothing */
            }

            /* 정수부 출력 */
            while( sRoundDigit > 0 )
            {
                DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
                
                DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( sResult , sTmpStr );
                
                sTmpValue = sInteger % 10;
                *sResult = sTmpValue + '0';               

                sInteger = sInteger / 10;
                sTmpStr--;
                sRoundDigit--;
                sResult--;
            }
        }
        else
        {
            /* .0XXXXX */

            sReadingZeroLen          = sExponent - sIntDigitCount;
            sTrailingZeroLen         = ( aFormatInfo->mDigitCntAfterV - sExponent > 0 ?
                                         aFormatInfo->mDigitCntAfterV - sExponent : 0 );
            sDigitCntBeforeDecimal   = aFormatInfo->mDigitCntAfterV;
            sOrgDigitCntAfterDecimal = sExponent - aFormatInfo->mDigitCntAfterV;
            
            sRoundDigit = aFormatInfo->mDigitCntAfterV - sReadingZeroLen - sTrailingZeroLen +
                          ( sPlusOneDigitForRound == STL_TRUE ? 1 : 0 );
            sReadingZeroLen -= ( sPlusOneDigitForRound == STL_TRUE ? 1 : 0 );

            if( sOrgDigitCntAfterDecimal > 0 )
            {
                i = sOrgDigitCntAfterDecimal;
                if( i > 1 )
                {
                    sInteger = i > STL_DBL_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                }
                else
                {
                    /* do nothing */
                }

                sTmpValue = sInteger % 10;
                sInteger  /= 10;
                sIntDigitCount -= i;
                
                if( sTmpValue > 4 )
                {
                    sInteger++;
                }
                else
                {
                    /* do nothing */
                }

            }

            /* trailing zero */
            while( sTrailingZeroLen > 0 )
            {
                *sResult = '0';
                sResult--;
                sTrailingZeroLen--;
            }
            
            /* 소수 출력 */
            while( sRoundDigit > 0 )
            {
                sTmpValue = sInteger % 10;
                *sResult = sTmpValue + '0';

                sInteger = sInteger / 10;
                sRoundDigit--;
                sResult--;
            }

            /* reading zero */
            while( sReadingZeroLen > 0 )
            {
                *sResult = '0';
                sResult--;
                sReadingZeroLen--;
            }

            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
        }
    }
    else
    {
        sRoundDigit = sDigitCntBeforeDecimal;
        
        sTmpStr = sFormatStrEnd;
        sExponent  = sDigitCntAfterDecimal;

        DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

        /* print trailing zero */
        sZeroLen = sDigitCntBeforeDecimal - sIntDigitCount;
        while( sZeroLen > 0 )
        {
            *sResult = '0';
                
            sTmpStr--;
            sExponent--;
            sRoundDigit--;
            sResult--;
            sZeroLen--;

            DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( sResult , sTmpStr );
            
            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
        }


        DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( sResult , sTmpStr );
        
        /* 정수부 출력 */
        while( ( sRoundDigit > 0 ) && ( sIntDigitCount > 0 ) )
        {
            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

            DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( sResult , sTmpStr );
            
            sTmpValue = sInteger % 10;
            *sResult = sTmpValue + '0';              

            sInteger = sInteger / 10;
            sTmpStr--;
            sExponent--;
            sRoundDigit--;
            sIntDigitCount--;
            sResult--;
        }       
    }

    
    STL_RAMP( RAMP_SUCCESS );

    *aLength = aFormatInfo->mToCharResultLen + sReduceLen;

    /**
     * FM format 시 decimal point 이후 '9' format 에 해당하는
     * '0' 문자의 위치를 선계산 할 수 없으므로
     * 문자열 변환이 끝나고 'S,MI,PR' 등을 재 출력 한다.
     */
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        sResult = aResult + ( *aLength - 1 < 0 ? 0 : *aLength -1 );
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  *aLength,
                                  !sIsNegative,
                                  STL_FALSE,
                                  &sResultLen,
                                  sResult,
                                  &sResult );

    }
    else
    {
        /* do nothing */
    }

    STL_RAMP( RAMP_END );

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    } 

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief native_double type을 X format string에 맞게 string으로 변환한다.
 * @param[in]  aDataValue          dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharDoubleToXFormatString( dtlDataValue         * aDataValue,
                                          dtfNumberFormatInfo  * aFormatInfo,
                                          stlInt64               aAvailableSize,
                                          stlChar              * aResult,
                                          stlInt64             * aLength,
                                          dtlFuncVector        * aVectorFunc,
                                          void                 * aVectorArg,
                                          stlErrorStack        * aErrorStack )
{
    dtlDoubleType   sValue;
    stlFloat64      sPow;
    stlInt64        sInteger                 = 0;
    stlUInt64       sResultValue[4];
    stlBool         sIsNegativeExp           = STL_FALSE;
    stlBool         sCarry                   = STL_FALSE;
    stlInt32        sTmpExponent             = 0;
    stlInt32        sExponent                = 0;
    stlInt64        sTotDigitCount           = 0;
    stlInt64        sIntDigitCount           = 0;
    stlInt32        sDigitCount              = 0;
    stlUInt64       sUInt64Value             = 0;
    stlInt32        sResultLen               = aFormatInfo->mToCharResultLen;
    stlInt32        sReduceLen               = 0;
    stlInt32        sTmpValue                = 0;
    stlInt32        sZeroLen                 = 0;
    stlUInt8      * sDigit                   = NULL;
    stlChar       * sResult                  = aResult;
    stlBool         sNumberIsZero            = STL_FALSE;
    stlBool         sZeroFormatFlag          = STL_FALSE;
    stlUInt8        sLowerUpperIdx           = aFormatInfo->mLowerUpperIdx;    

#ifndef STL_IS_BIGENDIAN     
    stlInt32        i = 0;
#endif    
    

    sValue = *((dtlDoubleType*) aDataValue->mValue);

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        STL_TRY_THROW( aAvailableSize >= DTL_BINARY_REAL_INFINITY_SIZE, ERROR_NOT_ENOUGH_BUFFER );
        stlMemcpy( (stlChar*)aResult, DTL_BINARY_REAL_INFINITY, DTL_BINARY_REAL_INFINITY_SIZE );
        *aLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }

    if( sValue == 0.0 )
    {
        sNumberIsZero            = STL_TRUE;
        STL_THROW( RAMP_VALUE_IS_ZERO );       
    }

    /* STL_DBL_DIG만큼의 유효숫자(trailing zero는 제거한 상태)와 
     * exponent의 형태로 만든다. */
    if( sValue < 0.0 )
    {
        STL_THROW( RAMP_CANNOT_DISPLAY );
    }
    else
    {
        /* do nothing */
    }

    sTotDigitCount = stlLog10( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }


    sExponent = sTotDigitCount - STL_DBL_DIG;
    if( sExponent > STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent - STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else if( sExponent < -STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( -STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent + STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else
    {
        DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }

    DTL_IS_VALID( stlGet10Pow( STL_DBL_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_DBL_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sIsNegative      sValue의 음수여부
     * sInteger         sValue의 STL_DBL_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값
     * */

    STL_RAMP( RAMP_VALUE_IS_ZERO );

    /**
     * '0' 포맷으로 인한 Zero 길이
     */
    if( aFormatInfo->mZeroStart > 0 )
    {
        sZeroFormatFlag = STL_TRUE;
    }
    else
    {
        sZeroFormatFlag = STL_FALSE;
    }
   
    /**
     * 숫자 표기 ( 0 인 경우 )
     */
    
    if( sNumberIsZero == STL_TRUE )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
        {
            sReduceLen--;
        }
        else
        {
            *sResult = ' ';
            sResult++;
        }
        
        sResultLen = aFormatInfo->mToCharResultLen;

        sResultLen = sResultLen - 2 /* blank , zero  */;
        while( sResultLen > 0 )
        {
            if( sZeroFormatFlag == STL_FALSE )
            {
                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                {
                    sReduceLen--;
                }
                else
                {
                    *sResult = ' ';
                    sResult++;
                }
            }
            else
            {
                *sResult = '0';
                sResult++;
            }
            sResultLen--;
        }
       
        *sResult = '0';
        sResult++;
                      
        STL_THROW( RAMP_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }


     /**
     * 숫자 표기 ( 0 이 아닌 경우 )
     */

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent < sIntDigitCount )
        {
            /* XXX.XXX */
            sInteger   = sInteger / gPreDefinedPow[ sExponent - 1 ];
            sTmpValue  = sInteger % 10;
            sInteger  /= 10;
            if( sTmpValue > 4 )
            {
                sInteger++;
            }
            else
            {
                /* do nothing */
            }

            sExponent = 0;
        }
        else if( sExponent == sIntDigitCount )
        {

            /* 0.XXXXXX */

            /* X format 은
             * 소수점을 무조건 round 처리 한다. */
            sInteger = sInteger / gPreDefinedPow[ sExponent -1 ];
            if( sInteger > 4 )
            {
                sInteger  = 1;
                sExponent = 0;
            }
            else
            {
                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                {
                    sReduceLen--;
                }
                else
                {
                    *sResult = ' ';
                    sResult++;
                }

                sResultLen = aFormatInfo->mToCharResultLen;

                sResultLen = sResultLen - 2 /* blank , zero  */;
                while( sResultLen > 0 )
                {
                    if( sZeroFormatFlag == STL_FALSE )
                    {
                        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                        {
                            sReduceLen--;
                        }
                        else
                        {
                            *sResult = ' ';
                            sResult++;
                        }
                    }
                    else
                    {
                        *sResult = '0';
                        sResult++;
                    }
                    sResultLen--;
                }
       
                *sResult = '0';
                sResult++;
                      
                STL_THROW( RAMP_SUCCESS );
            }
        }
        else
        {
            /* .0XXX */

            /* equal to zero value */
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
            {
                sReduceLen--;
            }
            else
            {
                *sResult = ' ';
                sResult++;
            }
            
            sResultLen = aFormatInfo->mToCharResultLen;

            sResultLen = sResultLen - 2 /* blank , zero  */;
            while( sResultLen > 0 )
            {
                if( sZeroFormatFlag == STL_FALSE )
                {
                    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                    {
                        sReduceLen--;
                    }
                    else
                    {
                        *sResult = ' ';
                        sResult++;
                    }
                }
                else
                {
                    *sResult = '0';
                    sResult++;
                }
                sResultLen--;
            }
       
            *sResult = '0';
            sResult++;
                      
            STL_THROW( RAMP_SUCCESS );
            sInteger  = 0;
            sExponent = 0;
        }
    }
    else
    {
        /* XXX000 */

        /* do nothing */
    }

    /**
     * Digit를 stlUInt64형태로 변환
     */

    /* Result Value 초기화 */
    sResultValue[0] = (stlUInt64)0;
    sResultValue[1] = (stlUInt64)0;
    sResultValue[2] = (stlUInt64)0;
    sResultValue[3] = (stlUInt64)0;

    sUInt64Value = (stlUInt64)sInteger;

    STL_TRY_THROW( _dtfAddUInt64ToUInt256( sResultValue ,sUInt64Value  ) == STL_TRUE,
                   RAMP_CANNOT_DISPLAY );

    while( sExponent > 0 )
    {        
        sTmpExponent = sExponent / 19 ;
        if( sTmpExponent == 0 )
        {
            sTmpExponent = sExponent % 19;
            STL_TRY_THROW( _dtfMulUInt64ToUInt256( sResultValue , gPreDefinedPow[ sTmpExponent ] ) == STL_TRUE,
                       RAMP_CANNOT_DISPLAY );

            sExponent -= sTmpExponent;
        }
        else
        {
            STL_TRY_THROW( _dtfMulUInt64ToUInt256( sResultValue , gPreDefinedPow[ 19 ] ) == STL_TRUE,
                       RAMP_CANNOT_DISPLAY );

            sExponent -= 19;
        }       
    }

#ifndef STL_IS_BIGENDIAN   /* This platform has little endian */
    /* Bigendian형태로 바꾸어 stlUInt8의 Array로 접근 가능하도록 한다. */
    for( i = 0; i < 4; i++ )
    {
        if( sResultValue[i] > (stlUInt64)0 )
        {
            sUInt64Value = sResultValue[i];
            ((stlUInt8*)(&sResultValue[i]))[0] = ((stlUInt8*)(&sUInt64Value))[7];
            ((stlUInt8*)(&sResultValue[i]))[1] = ((stlUInt8*)(&sUInt64Value))[6];
            ((stlUInt8*)(&sResultValue[i]))[2] = ((stlUInt8*)(&sUInt64Value))[5];
            ((stlUInt8*)(&sResultValue[i]))[3] = ((stlUInt8*)(&sUInt64Value))[4];
            ((stlUInt8*)(&sResultValue[i]))[4] = ((stlUInt8*)(&sUInt64Value))[3];
            ((stlUInt8*)(&sResultValue[i]))[5] = ((stlUInt8*)(&sUInt64Value))[2];
            ((stlUInt8*)(&sResultValue[i]))[6] = ((stlUInt8*)(&sUInt64Value))[1];
            ((stlUInt8*)(&sResultValue[i]))[7] = ((stlUInt8*)(&sUInt64Value))[0];
        }
    }
#endif

    /* 결과 Sting을 만들기 위해 변환할 Digit 및 변환된 String Size 계산 */
    sDigit = (stlUInt8*)sResultValue;
    sDigitCount = 64;
    while( *sDigit == (stlUInt8)0 )
    {
        sDigit++;
        sDigitCount -= 2;
    }

    if( *sDigit < (stlUInt8)0x10 )
    {
        sCarry = STL_TRUE;
        sDigitCount--;
    }
    else
    {
        sCarry = STL_FALSE;
    }
    

    /* 변환된 수가 Result Length에 포함되는지 판단 */
    STL_TRY_THROW( sDigitCount < aFormatInfo->mToCharResultLen,
                   RAMP_CANNOT_DISPLAY );

    /* String의 앞에 공백 추가 */
    sResult = aResult;
    
    /* '0' 길이 계산 */
    if( sZeroFormatFlag == STL_TRUE )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
        {
            sReduceLen--;
        }
        else
        {
            *sResult = ' ';
            sResult++;
        }
        
        sZeroLen = aFormatInfo->mToCharResultLen - sDigitCount -1;
        while( sZeroLen > 0 )
        {
            *sResult = '0';
            sResult++;
            sZeroLen--;
        }   
    }
    else
    {
        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
        {   
            sReduceLen -= (aFormatInfo->mToCharResultLen - sDigitCount);
        }
        else
        {
            stlMemset( sResult, ' ', STL_SIZEOF( stlChar ) * (aFormatInfo->mToCharResultLen - sDigitCount) );
            sResult += (aFormatInfo->mToCharResultLen - sDigitCount);
        }
    }   

    /* String의 실제 값 추가 */
    if( sCarry == STL_TRUE )
    {
        *sResult = dtfHexaToString[sLowerUpperIdx][*sDigit];
        sResult++;
        sDigit++;
        sDigitCount--;
    }

    while( sDigitCount > 0 )
    {
        *sResult = dtfHexaToString[sLowerUpperIdx][( (*sDigit) & (stlUInt8)0xF0 ) >> 4];
        sResult++;
        *sResult = dtfHexaToString[sLowerUpperIdx][(*sDigit) & (stlUInt8)0x0F];
        sResult++;
        sDigit++;
        sDigitCount -= 2;
    }

    STL_THROW( RAMP_SUCCESS );

    STL_RAMP( RAMP_CANNOT_DISPLAY );

    /* Result의 모든 자리에 #으로 표시한다. */
    stlMemset( aResult, '#', STL_SIZEOF( stlChar ) * aFormatInfo->mToCharResultLen );
    
    STL_RAMP( RAMP_SUCCESS );

    *aLength = aFormatInfo->mToCharResultLen + sReduceLen;

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    } 

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief native_double type을 EEEE format string에 맞게 string으로 변환한다.
 *
 * @param[in]  aDataValue          dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharDoubleToEEEEFormatString( dtlDataValue         * aDataValue,
                                             dtfNumberFormatInfo  * aFormatInfo,
                                             stlInt64               aAvailableSize,
                                             stlChar              * aResult,
                                             stlInt64             * aLength,
                                             dtlFuncVector        * aVectorFunc,
                                             void                 * aVectorArg,
                                             stlErrorStack        * aErrorStack )
{
    dtlDoubleType   sValue;
    stlFloat64      sPow;
    stlBool         sIsNegative              = STL_FALSE;
    stlInt64        sInteger                 = 0;
    stlBool         sIsNegativeExp           = STL_FALSE;
    stlInt32        sExponent                = 0;
    stlInt64        sTotDigitCount           = 0;
    stlInt64        sIntDigitCount           = 0;
    stlUInt8        sCarry                   = 0;
    stlInt32        i                        = 0;
    stlInt32        sTmpValue                = 0;
    stlInt32        sRemainder               = 0;
    stlInt32        sZeroLen                 = 0;
    stlInt32        sOrgDigitCntAfterDecimal = 0;
    stlInt32        sDigitCntAfterDecimal    = 0;
    stlInt32        sDigitCntBeforeDecimal   = 0;
    stlInt32        sResultLen               = aFormatInfo->mToCharResultLen;
    stlInt32        sReduceLen               = 0;
    stlInt32        sRoundDigit              = 0;
    stlInt32        sDisplayExponent         = 0;
    stlUInt8        sLen                     = 0;
    stlChar       * sTmpStr                  = NULL;
    stlChar       * sFormatStrEnd            = aFormatInfo->mStr + aFormatInfo->mStrLen -1;
    stlChar       * sResult                  = aResult;
    stlBool         sPlusOneDigitForRound    = STL_FALSE;
    stlBool         sNumberIsZero            = STL_FALSE;
    stlBool         sIsPrintZero             = STL_FALSE;

    sValue = *((dtlDoubleType*) aDataValue->mValue);

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        STL_TRY_THROW( aAvailableSize >= DTL_BINARY_REAL_INFINITY_SIZE, ERROR_NOT_ENOUGH_BUFFER );
        stlMemcpy( (stlChar*)aResult, DTL_BINARY_REAL_INFINITY, DTL_BINARY_REAL_INFINITY_SIZE );
        *aLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }

    if( sValue == 0.0 )
    {
        sNumberIsZero            = STL_TRUE;
        sIsNegative              = STL_FALSE;
        sDisplayExponent         = 0;
        sDigitCntAfterDecimal = aFormatInfo->mDigitCntAfterDecimal;
        if( ( (aFormatInfo->mDigitCntBeforeDecimal > 0) && (sDigitCntAfterDecimal == 0 ) ) ||
            ( aFormatInfo->mZeroStart > 0 ) )
        {
            /*
             * 예) TO_CHAR( 0, '9EEEE' ) => '  0E+00'
             *     TO_CHAR( 0, '0EEEE' ) => '  0E+00'
             *     TO_CHAR( 0, '0.999EEEE' ) => '  0.000E+00'
             */     
            sDigitCntBeforeDecimal = 1;
        }
        else
        {
            /* 예) TO_CHAR( 0, '9.999EEEE' ) => '   .000E+00' */
            sDigitCntBeforeDecimal = 0;
        }
        STL_THROW( RAMP_VALUE_IS_ZERO );
    }
    else
    {
        /* Do Nothing */
    }

    /* STL_DBL_DIG만큼의 유효숫자(trailing zero는 제거한 상태)와 
     * exponent의 형태로 만든다. */
    if( sValue < 0.0 )
    {
        sIsNegative = STL_TRUE;
        sValue = -sValue;
    }
    else
    {
        sIsNegative = STL_FALSE;
    }

    sTotDigitCount = stlLog10( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }


    sExponent = sTotDigitCount - STL_DBL_DIG;
    if( sExponent > STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent - STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else if( sExponent < -STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( -STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent + STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else
    {
        DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }

    DTL_IS_VALID( stlGet10Pow( STL_DBL_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_DBL_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sIsNegative      sValue의 음수여부
     * sInteger         sValue의 STL_DBL_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값
     * */

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent <= sIntDigitCount )
        {
            sDisplayExponent = (sIntDigitCount == sExponent ? 1 : sIntDigitCount - sExponent - 1);
        }
        else
        {
            sDisplayExponent = sExponent - sIntDigitCount + 1;

        }
    }
    else
    {
        sDisplayExponent = sExponent + sIntDigitCount - 1;
    }

    sDigitCntAfterDecimal = aFormatInfo->mDigitCntAfterDecimal;
    sDigitCntBeforeDecimal = 1 + aFormatInfo->mDigitCntAfterV;
    sOrgDigitCntAfterDecimal = sIntDigitCount -1;

    

    /* 포맷에 의한 round 발생 */
    if( sOrgDigitCntAfterDecimal > ( sDigitCntAfterDecimal + aFormatInfo->mDigitCntAfterV ) )
    {
        i = sOrgDigitCntAfterDecimal - ( sDigitCntAfterDecimal + aFormatInfo->mDigitCntAfterV );
        if( i > 1 ) /* 1 자리 이상 차이 나는 경우 */
        {
            sTmpValue = i > STL_DBL_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
        }
        else 
        {
            sTmpValue = sInteger;
        }
                
        sRemainder = sTmpValue % 10;

        if( sRemainder > 4 )
        {
            sCarry = 1;
        }
        else
        {
            sCarry = 0;
        }               

        sTmpValue /= 10; 
        while( ( sCarry == 1 ) && ( sTmpValue > 9 ) )
        {
            if( ( sTmpValue % 10 ) == 9 )
            {
                sTmpValue /= 10;
                sCarry = 1;
            }
            else
            {
                sCarry = 0;
            }
        }

        /*
         * 정수가 한자리 늘어나는 경우
         * ex) 9999 =  9.999E...
         *  -> 2000 = 10.000E...
         */
        if( ( sCarry == 1 ) && ( sTmpValue == 9 ) )
        {          
            sPlusOneDigitForRound  = STL_TRUE;
        }
        else
        {
            sPlusOneDigitForRound = STL_FALSE;
        }
    }

    STL_RAMP( RAMP_VALUE_IS_ZERO );

    /* B Format */
    if( sNumberIsZero == STL_TRUE )    
    {
        if( DTF_NUMBER_FMT_FLAG_IS_BLANK( aFormatInfo ) == STL_TRUE )
        {
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                /* 예) TO_CHAR( 0, 'B9EEEE' ) */
                stlMemset( sResult, ' ', sResultLen );
            }
            else
            {
                sReduceLen = -aFormatInfo->mToCharResultLen;
            }

            *aLength = aFormatInfo->mToCharResultLen + sReduceLen;
            STL_THROW( RAMP_END );
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

    /*
     * 공백 & 부호 표기 & 0인 숫자에 대한 '0' 표기
     */
    if( aFormatInfo->mDigitCntAfterV == 0 )
    {
        if( ( sNumberIsZero == STL_TRUE ) && ( sDigitCntBeforeDecimal == 0 ) )
        {
            /*
             * 예) to_char( 0, '9.999999EEEE' )  =>  [][][][].000000E+00
             */

            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                *sResult = ' ';
                sResult++;
            }
            else
            {
                sReduceLen -= 1;
            }
        }
        else
        {
            /*
             * 예) to_char( 0, '0.999999EEEE' )  =>  [][][]0.000000E+00
             */
            
            /* Do Nothing */
        }
    }
    else
    {
        
        if( ( sNumberIsZero == STL_TRUE ) && ( aFormatInfo->mZeroStart == 0 ) )
        {
            /*
             * 예) to_char( 0, '9v9eeee' )  =>  [][][]0E+00
             */

            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                *sResult = ' ';
                sResult++;
            }
            else
            {
                sReduceLen -= 1;
            }
        }
        else
        {
            /*
             * 예) to_char( 0, '0v9eeee' )  =>  [][]00E+00
             */
            
            /* Do Nothing */
        }
    }

    /* 공백 */

    /* DisplayExponent */
    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent >= sIntDigitCount )
        {
            sDisplayExponent = (-1) * sDisplayExponent;
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* Do Nothing */
    }

    if( sPlusOneDigitForRound == STL_TRUE )
    {
        sInteger++;
        sOrgDigitCntAfterDecimal++;
        sDisplayExponent++;

    }
    else
    {
        /* do nothing */
    }

    if( ( sDisplayExponent > -100 ) && ( sDisplayExponent < 100 ) )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
        {
            *sResult = ' ';
            sResult++;
        }
        else
        {
            sReduceLen -= 1;
        }
    }
    else
    {
        /* Do Nothing */
    }

    sTmpStr = sFormatStrEnd;


    /**
     * 'EEEE, S, MI, PR' skip
     */
    while( STL_TRUE )
    {
        if( (*sTmpStr == 'E') || (*sTmpStr == 'e' ) )
        {
            sTmpStr -= 4;
            continue;
        }

        if( (*sTmpStr == 'S') || (*sTmpStr == 's') )
        {
            sTmpStr -= 1;
            continue;
        }

        if( (*sTmpStr == 'I') || (*sTmpStr == 'i') )
        {
            sTmpStr -= 2;
            continue;
        }

        if( (*sTmpStr == 'R') || (*sTmpStr == 'r') )
        {
            sTmpStr -= 2;
            continue;
        }

        break;
    }

    /**
     * 부호 표기 ( +, -, PR, MI )
     */
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        dtfNumberFmtSetSignForNormal( aFormatInfo, aResult, sResultLen, !sIsNegative, sResult, &sResult );
    }
    else
    {
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  sResultLen,
                                  !sIsNegative,
                                  STL_TRUE, // aIsSignLocatePre
                                  &sResultLen,
                                  sResult,
                                  &sResult );            
    }

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_BRACKET( aFormatInfo ) == STL_TRUE )
        {
            sReduceLen -= (sIsNegative == STL_TRUE ? 0 : 2);
        }
        else if( DTF_NUMBER_FMT_FLAG_IS_SIGN( aFormatInfo ) == STL_FALSE )
        {
            sReduceLen -= (sIsNegative == STL_TRUE ? 0 : 1);
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* do nothing */
    }


    /**
     * DOLLAR 표기
     */

    DTF_NUMBER_FMT_SET_DOLLAR( aFormatInfo, sResult );


    

    
    /**
     * 숫자 표기 ( 0 인 경우 )
     */
    if( sNumberIsZero == STL_TRUE )
    {
        if( aFormatInfo->mDigitCntAfterV == 0 )
        {
            if( sDigitCntBeforeDecimal >= 1 )
            {
                *sResult = '0';
                sResult++;
            }
            else 
            {
                if( (aFormatInfo->mDigitCntBeforeDecimal > 0) &&
                    (aFormatInfo->mZeroEndAfterDecimal == 0 ) &&
                    (DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE) )
                {
                    *sResult = '0';
                    sResult++;
                    sReduceLen++;
                }
                /* do nothing */
            }

        }
        else
        {
            if( aFormatInfo->mZeroStart == 0 )
            {
                sZeroLen = aFormatInfo->mDigitCntAfterV;
            }
            else
            {
                sZeroLen = 1 + aFormatInfo->mDigitCntAfterV;
            }

            while( sZeroLen > 0 )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }
        
        if( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aFormatInfo ) == STL_TRUE )
        {
            *sResult = '.';
            sResult++;
        }
        else
        {
            /* Do Nothing */
        }

        sZeroLen = sDigitCntAfterDecimal;
        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
        {
            sZeroLen = aFormatInfo->mZeroEndAfterDecimal;
            sReduceLen -= ( sDigitCntAfterDecimal - sZeroLen );

            while( sZeroLen > 0 )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }
        else
        {
            /* 정방향 출력 */
            while( sZeroLen > 0 )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }

        dtfNumberFmtSetEEEEExp( aFormatInfo, sDisplayExponent, sResult, &sLen );

        STL_THROW( RAMP_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }
  

     /**
     * 숫자 표기 ( 0 이 아닌 경우 )
     */
    
    sRoundDigit = sDigitCntBeforeDecimal + sDigitCntAfterDecimal;
        
    /*  round 가 발생하는 경우
     *  round 되는 값을 미리 keep */
    if( sOrgDigitCntAfterDecimal > ( sDigitCntAfterDecimal + aFormatInfo->mDigitCntAfterV ) )
    {
        i = sOrgDigitCntAfterDecimal - ( sDigitCntAfterDecimal + aFormatInfo->mDigitCntAfterV );
        if( i > 1 ) /* 1 자리 이상 차이 나는 경우 */
        {
            sInteger = i > STL_DBL_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
        }
        else 
        {
            /* do nothing */
        }

        sRemainder = sInteger % 10;
        sInteger   = sInteger / 10;
                

        if( sRemainder > 4 )
        {
            sCarry = 1;
        }
        else
        {
            sCarry = 0;
        }
    }
    else
    {
        sCarry = 0;
    }

    sResult--;
    sResult   += sRoundDigit +
                 ( sDigitCntAfterDecimal > 0 ? 1 :
                 ( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aFormatInfo ) == STL_TRUE ? 1 : 0 ) ) /* decimal point */;
    sExponent  = sDigitCntAfterDecimal;

    /**
     * V format trailing zero
     */
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        sZeroLen = aFormatInfo->mDigitCntAfterV - sExponent - sOrgDigitCntAfterDecimal;
    }
    else
    {
        sZeroLen = aFormatInfo->mDigitCntAfterV + sExponent - sOrgDigitCntAfterDecimal;
    }
    while( sZeroLen > 0 )
    {
        *sResult = '0';
        sExponent--;
        sResult--;
        sZeroLen--;
    }

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        if( *sTmpStr == '0' )
        {
            sIsPrintZero = STL_TRUE;
        }
        else
        {
            /* do nothing */
        }

        /**
         * not V format trailing zero
         */
        sZeroLen = sDigitCntAfterDecimal - sOrgDigitCntAfterDecimal;

        
        /**
         * FM format : '.' 이후 제로는 '0' 이전에 '9' 가 있더라도
         * 계속 제로를 출력 한다.
         */
        while( sZeroLen > 0 )
        {
            if( (sIsPrintZero == STL_TRUE ) || (*sTmpStr == '0') )
            {
                *sResult = '0';
            }
            else
            {
                sReduceLen--;
            }

            sExponent--;
            sResult--;
            sZeroLen--;
                
            sTmpStr--;
                
        }
        
        /* 소수점 digit 출력 */
        while( ( sExponent > 0) && (sIntDigitCount > 0 ) )
        {
            sTmpValue = sInteger % 10;

            sIsPrintZero = sTmpValue > 0 ? STL_TRUE : sIsPrintZero;

            if( (sIsPrintZero == STL_TRUE ) || (*sTmpStr == '0') )
            {
                *sResult = sTmpValue + '0';
            }
            else
            {
                sReduceLen--;
            }

            sInteger = sInteger / 10;

            sExponent--;
            sResult--;
            sRoundDigit--;
            sIntDigitCount--;
        
        }
    }
    else
    {
        /* 소수점 digit 출력 */
        while( ( sExponent > 0) && (sIntDigitCount > 0 ) )
        {
            sTmpValue = sInteger % 10;
            *sResult = sTmpValue + '0';

            sInteger = sInteger / 10;

            sExponent--;
            sResult--;
            sRoundDigit--;
            sIntDigitCount--;
        
        }
    }

    if( (sDigitCntAfterDecimal > 0) || (DTF_NUMBER_FMT_FLAG_IS_PERIOD( aFormatInfo ) == STL_TRUE) )
    {
        *sResult = '.';
        sResult--;
    }
    else
    {
        /* Do Nothing */
    }

    /* 정수부 출력 */
    while( ( sRoundDigit > 0 ) && ( sIntDigitCount > 0 ) )
    {
        sTmpValue = sInteger % 10;
        *sResult = sTmpValue + '0';               

        sInteger = sInteger / 10;
        sRoundDigit--;
        sIntDigitCount--;
        sResult--;
    }

    if( sCarry == 1 )
    {
        sRoundDigit = 1 + sDigitCntAfterDecimal + aFormatInfo->mDigitCntAfterV +
                      ( sDigitCntAfterDecimal > 0 ? 1: 0 ); /* decimal point */
            
        sResult += sRoundDigit;

        while( ( sCarry == 1 ) && ( sRoundDigit > 0 ) )
        {                   
            *sResult += sCarry;

            if( *sResult > '9' )
            {
                *sResult = '0';
                sCarry = 1;
            }
            else
            {
                sCarry = 0;
            }

            sRoundDigit--;
            sResult--;
        }
    }
    else
    {
        /* Do Nothing */
    }


    
    sRoundDigit = ( aFormatInfo->mSignLocate == DTF_NUMBER_SIGN_LOCATE_POST ? 0 : 1 ) +
        1 + sDigitCntAfterDecimal +
        aFormatInfo->mDigitCntAfterV +
        ( DTF_NUMBER_FMT_FLAG_IS_DOLLAR( aFormatInfo ) == STL_TRUE ? 1 : 0 ) +
        ( sDigitCntAfterDecimal > 0 ? 1 :
          ( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aFormatInfo ) == STL_TRUE ? 1 : 0 ) ) /* decimal point */ +
        ( sDisplayExponent > -100  ? ( sDisplayExponent < 100 ? 1 : 0 ) : 0 );

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        sRoundDigit += sReduceLen;
        if( ((DTF_NUMBER_FMT_FLAG_IS_MINUS( aFormatInfo ) == STL_TRUE) ||
             (DTF_NUMBER_FMT_FLAG_IS_BRACKET( aFormatInfo ) == STL_TRUE) )&&
            (sIsNegative == STL_FALSE ) )
        {
            sRoundDigit++;
        }
        
    
    }
    else
    {
        /* do nothing */
    }
                  
    sResult = aResult + sRoundDigit;

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        /**
         * Carry 에 의해 생성된 '0' 을 제거
         */
        sResult--;
        sZeroLen = sDigitCntAfterDecimal;

        while( sZeroLen > aFormatInfo->mZeroEndAfterDecimal )
        {
            if( *sResult == '0' )
            {
                sResult--;
                sReduceLen--;
            }

            sZeroLen--;
        }
        sResult++;
        
    }
    else
    {
        /* do nothing */
    }
    
    dtfNumberFmtSetEEEEExp( aFormatInfo, sDisplayExponent, sResult, &sLen );

    STL_RAMP( RAMP_SUCCESS );
    
    *aLength = aFormatInfo->mToCharResultLen + sReduceLen;

    /**
     * FM format 시 decimal point 이후 '9' format 에 해당하는
     * '0' 문자의 위치를 선계산 할 수 없으므로
     * 문자열 변환이 끝나고 'S,MI,PR' 등을 재 출력 한다.
     */
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        sResult = aResult + (*aLength -1 < 0 ? 0 : *aLength -1 );
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  *aLength,
                                  !sIsNegative,
                                  STL_FALSE,
                                  &sResultLen,
                                  sResult,
                                  &sResult );

    }
    else
    {
        /* do nothing */
    }

    STL_RAMP( RAMP_END );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    } 

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief native_double type을 RN, rn format string에 맞게 string으로 변환한다.
 * @param[in]  aDataValue          dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharDoubleToRNFormatString( dtlDataValue         * aDataValue,
                                           dtfNumberFormatInfo  * aFormatInfo,
                                           stlInt64               aAvailableSize,
                                           stlChar              * aResult,
                                           stlInt64             * aLength,
                                           dtlFuncVector        * aVectorFunc,
                                           void                 * aVectorArg,
                                           stlErrorStack        * aErrorStack )
{

    dtlDoubleType     sValue;
    stlFloat64        sPow;
    stlInt64          sInteger;
    stlBool           sIsNegativeExp;
    stlInt32          sExponent;
    stlInt64          sTotDigitCount;
    stlInt64          sIntDigitCount;
    stlUInt8          sCarry                            = 0;
    stlInt32          sTmpValue                         = 0;
    stlChar         * sResult                           = aResult;
    stlInt32          sLeftSpace                        = 0;
    stlInt32          sIndex                            = 0;
    stlInt32          sReduceLen                        = 0;
    stlInt32          sLength                           = 0;
    const stlInt32    RESULT_STRING_SIZE                = DTF_NUMBER_TO_CHAR_RESULT_ROMAN_NUMERAL_STRING_SIZE;
    stlInt32          sLowerUpperIdx                    = aFormatInfo->mLowerUpperIdx;
    stlInt64          ROMAN_NUMERAL_MAX_VALUE           = 3999;

    sValue = *((dtlDoubleType*) aDataValue->mValue);

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        STL_TRY_THROW( aAvailableSize >= DTL_BINARY_REAL_INFINITY_SIZE, ERROR_NOT_ENOUGH_BUFFER );
        stlMemcpy( (stlChar*)aResult, DTL_BINARY_REAL_INFINITY, DTL_BINARY_REAL_INFINITY_SIZE );
        *aLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }

    if( sValue == 0 )
    {
        /* zero 는 로마숫자로 표현하지 않음 */
        STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
    }
    else
    {
        /* Do Nothing */
    }

    /* STL_DBL_DIG만큼의 유효숫자(trailing zero는 제거한 상태)와 
     * exponent의 형태로 만든다. */
    if( sValue < 0.0 )
    {
        /* 음수는 로마숫자로 표현하지 않는다. */
        STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
    }
    else
    {
        /* do nothing */
    }

    sTotDigitCount = stlLog10( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }

    sExponent = sTotDigitCount - STL_DBL_DIG;
    if( sExponent > STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent - STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else if( sExponent < -STL_DBL_MAX_10_EXP )
    {
        DTL_IS_VALID( stlGet10Pow( -STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sValue = sValue / sPow;
        DTL_IS_VALID( stlGet10Pow( sExponent + STL_DBL_MAX_10_EXP, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }
    else
    {
        DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
        sInteger = (stlInt64)(sValue / sPow + 0.5);
    }

    DTL_IS_VALID( stlGet10Pow( STL_DBL_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_DBL_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sInteger         sValue의 STL_DBL_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값
     * */

      
    sLeftSpace  = RESULT_STRING_SIZE;

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent > sIntDigitCount )
        {
            /* .000XXX */
            STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
        }
        else if( sExponent == sIntDigitCount )
        {
            /* .XXXXXX
             * 소수점 첫째자리만 고려한다. */
            sInteger = sInteger / gPreDefinedPow[ sExponent -1 ];
            if( sInteger < 5 )
            {
                /* 0.1, 0.2, 0.3, 0.4 */
                STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
            }
            else
            {
                /* 0.5, 0.6, 0.7, 0.8, 0.9 */
                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                {
                    sReduceLen   = RESULT_STRING_SIZE - 1;
                    aResult[ 0 ] = dtfRomanNumeralStringList[ sLowerUpperIdx ][ 0 ][ 1 ].mStr[0];
                }
                else
                {
                    stlMemset( sResult,
                               ' ',
                               RESULT_STRING_SIZE - 1 );
                    aResult[ 14 ] = dtfRomanNumeralStringList[ sLowerUpperIdx ][ 0 ][ 1 ].mStr[0];
                }
            }
        }
        else
        {
            /* 소수점 반올림 */
            while( sExponent > 0 )
            {
                sTmpValue = sInteger % 10;
                if( sTmpValue > 4 )
                {
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }
                 
                sInteger = sInteger / 10;
                sInteger += sCarry;

                sExponent--;
            }

            /* 최대값에 대한 예외 처리 */
            if( sInteger > ROMAN_NUMERAL_MAX_VALUE )
            {
                STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
            }
            else
            {
                /* do nothing */
            }

            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                sIndex = 0;
                while( sInteger > 0 )
                {
                    sTmpValue = sInteger % 10;
                    sInteger = sInteger / 10;
                                    
                    /* copy roman numeral */
                    sLength = dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStrLen;
                    stlMemcpy(
                        sResult + ( sLeftSpace - sLength ),
                        dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStr ,
                        sLength );

                    sLeftSpace -= sLength;

                    sIndex++;
                }
                        
                stlMemset( sResult,
                           ' ',
                           sLeftSpace );
            }
            else
            {
                sLength = 0;

                if( (sInteger / gPreDefinedPow[ sTotDigitCount ]) > 0 )
                {
                    sTotDigitCount++;
                    sIndex = sTotDigitCount -1;
                }
                else
                {
                    sIndex = sTotDigitCount -1;
                }
                while( sTotDigitCount > 0 )
                {
                    if( (sTotDigitCount - 1) > 0 )
                    {
                        sTmpValue = sInteger / gPreDefinedPow[ sTotDigitCount -1 ];
                        sInteger  = sInteger % gPreDefinedPow[ sTotDigitCount -1 ];
                    }
                    else
                    {
                        sTmpValue = sInteger;
                        sInteger  = 0;
                    }

                    if( sTmpValue == 0 )
                    {
                        sIndex--;
                        sTotDigitCount--;
                        continue;
                    }

                    /* copy roman numeral */
                    stlMemcpy(
                        sResult + sLength,
                        dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStr ,
                        dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStrLen );

                    sLength += dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStrLen;
                 
                    sIndex--;
                    sTotDigitCount--;
                }
                sReduceLen = RESULT_STRING_SIZE - sLength;
            }
        }
    }
    else
    {
        /* XXXXXX000 */

        /* 4자리 이상의 수  */
        if( sExponent + 1 > 4 )
        {
            STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
        }
        else
        {
            /* do nothing */
        }
         
        sIndex = 0;
        if( sExponent > 0 )
        {
            sInteger *= gPreDefinedPow[ sExponent ];
        }
        else
        {
            /* do nothing */
        }

        /* 최대값 예외처리 */
        if( sInteger > ROMAN_NUMERAL_MAX_VALUE )
        {
            STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
        }
        else
        {
            /* do nothing */
        }

        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
        {
            sIndex = 0;
            while( sInteger > 0 )
            {
                sTmpValue = sInteger % 10;
                sInteger = sInteger / 10;
                                    
                /* copy roman numeral */
                sLength = dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStrLen;
                stlMemcpy(
                    sResult + ( sLeftSpace - sLength ),
                    dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStr ,
                    sLength );

                sLeftSpace -= sLength;

                sIndex++;
            }
                        
            stlMemset( sResult,
                       ' ',
                       sLeftSpace );
        }
        else
        {
            sLength = 0;
            sIndex = sTotDigitCount -1;
            while( sTotDigitCount > 0 )
            {
                if( (sTotDigitCount - 1) > 0 )
                {
                    sTmpValue = sInteger / gPreDefinedPow[ sTotDigitCount -1 ];
                    sInteger  = sInteger % gPreDefinedPow[ sTotDigitCount -1 ];
                }
                else
                {
                    sTmpValue = sInteger;
                    sInteger  = 0;
                }

                if( sTmpValue == 0 )
                {
                    sIndex--;
                    sTotDigitCount--;
                    continue;
                }

                /* copy roman numeral */
                stlMemcpy(
                    sResult + sLength,
                    dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStr ,
                    dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStrLen );

                sLength += dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStrLen;

                sIndex--;
                sTotDigitCount--;
            }
            sReduceLen = RESULT_STRING_SIZE - sLength;
        }
    }  

    STL_THROW( RAMP_SUCCESS );

    STL_RAMP( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
    
    stlMemset( sResult , '#' , RESULT_STRING_SIZE );
    
    STL_RAMP( RAMP_SUCCESS );

    *aLength = RESULT_STRING_SIZE - sReduceLen;

    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    } 

    STL_FINISH;

    return STL_FAILURE;
}

   
/**
 * @brief double 을 EEEE, RN, rn이 아닌 format string에 맞게 string으로 변환한다.
 * @param[in]  aDataValue          dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharRealToFormatString( dtlDataValue         * aDataValue,
                                       dtfNumberFormatInfo  * aFormatInfo,
                                       stlInt64               aAvailableSize,
                                       stlChar              * aResult,
                                       stlInt64             * aLength,
                                       dtlFuncVector        * aVectorFunc,
                                       void                 * aVectorArg,
                                       stlErrorStack        * aErrorStack )
{
    dtlRealType     sValue;
    stlFloat64      sPow;
    stlBool         sIsNegative              = STL_FALSE;
    stlInt64        sInteger                 = 0;
    stlBool         sIsNegativeExp           = STL_FALSE;
    stlInt32        sExponent                = 0;
    stlInt64        sTotDigitCount           = 0;
    stlInt64        sIntDigitCount           = 0;
    stlUInt8        sCarry                   = 0;
    stlInt32        i                        = 0;
    stlInt32        sTmpValue                = 0;
    stlInt32        sRemainder               = 0;
    stlInt32        sTrailingZeroLen         = 0;
    stlInt32        sZeroLen                 = 0;
    stlInt32        sOrgDigitCntAfterDecimal = 0;
    stlInt32        sDigitCntAfterDecimal    = 0;
    stlInt32        sDigitCntBeforeDecimal   = 0;
    stlInt32        sResultLen               = aFormatInfo->mToCharResultLen;
    stlInt32        sReduceLen               = 0;
    stlInt32        sExtraDigitLen           = 0;
    stlInt32        sExtraCommaLen           = 0;
    stlInt32        sExtraZeroLen            = 0;
    stlInt32        sExtraSpaceLen           = 0;
    stlInt32        sRoundDigit              = 0;
    stlChar       * sResult                  = aResult;
    stlChar       * sTmpStr                  = NULL;
    stlChar       * sFormatStrEnd            = aFormatInfo->mStr + aFormatInfo->mStrLen -1;
    stlBool         sPlusOneDigitForRound    = STL_FALSE;
    stlBool         sIsSetBlank              = STL_FALSE;
    stlBool         sNumberIsZero            = STL_FALSE;
    stlBool         sLastSpaceUsed           = STL_FALSE;
    stlBool         sIsPrintZero             = STL_FALSE;
 

    sValue = *((dtlRealType*) aDataValue->mValue);

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        STL_TRY_THROW( aAvailableSize >= DTL_BINARY_REAL_INFINITY_SIZE, ERROR_NOT_ENOUGH_BUFFER );
        stlMemcpy( (stlChar*)aResult, DTL_BINARY_REAL_INFINITY, DTL_BINARY_REAL_INFINITY_SIZE );
        *aLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }

    if( sValue == 0 )
    {       
        sIsNegative            = STL_FALSE;
        sNumberIsZero          = STL_TRUE;
        sDigitCntAfterDecimal  = aFormatInfo->mDigitCntAfterDecimal;

        if( ( (aFormatInfo->mDigitCntBeforeDecimal > 0) && (sDigitCntAfterDecimal == 0 ) ) ||
            (aFormatInfo->mZeroStart > 0) )
        {
            /*
             * 예) TO_CHAR( 0, '9' ) => ' 0'
             *     TO_CHAR( 0, '0' ) => ' 0'
             *     TO_CHAR( 0, '0.999' ) => ' 0.000'
             */     
            sDigitCntBeforeDecimal = 1;
        }
        else
        {
            /* 예) TO_CHAR( 0, '.999' ) => ' .000' */
            sDigitCntBeforeDecimal = 0;
        }
        STL_THROW( RAMP_VALUE_IS_ZERO );
        
    }

    if( sValue < 0.0 )
    {
        sIsNegative = STL_TRUE;
        sValue = -sValue;
    }
    else
    {
        sIsNegative = STL_FALSE;
    }

    sTotDigitCount = stlLog10f( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }

    sExponent = sTotDigitCount - STL_FLT_DIG;
    DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
    sInteger = (stlInt64)(sValue / sPow + 0.5);

    DTL_IS_VALID( stlGet10Pow( STL_FLT_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_FLT_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sIsNegative      sValue의 음수여부
     * sInteger         sValue의 STL_DBL_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값
     * */

    if( sIsNegativeExp == STL_TRUE )
    {
        sDigitCntAfterDecimal = aFormatInfo->mDigitCntAfterDecimal;
        if( sExponent <= sIntDigitCount )
        {
            /* XXX.XXX */

            sZeroLen                 = 0;
            sDigitCntBeforeDecimal   = sIntDigitCount - sExponent;
            sOrgDigitCntAfterDecimal = sIntDigitCount - sDigitCntBeforeDecimal;
            
            if( sOrgDigitCntAfterDecimal > sDigitCntAfterDecimal )
            {
                /*
                 * round 되는 부분의 값이 4 보다 큰지 ( 크다면 )
                 * round 되는 자리부터 .x 까지 계속 9값이어서
                 * 최후에 정수 값 + 1 이 되는지 확인
                 */
                i = sOrgDigitCntAfterDecimal - sDigitCntAfterDecimal;
                if( i > 1 ) /* 1 자리 이상 차이 나는 경우 */
                {
                    sTmpValue = i > STL_FLT_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                }
                else 
                {
                    sTmpValue = sInteger;
                }
                
                sRemainder = sTmpValue % 10;
                sTmpValue  = sTmpValue / 10;

                if( ( sExponent == sIntDigitCount ) && ( sTmpValue == 0 ) )
                {
                    if( sRemainder < 5 )
                    {
                        sNumberIsZero          = STL_TRUE;
                        sDigitCntAfterDecimal  = aFormatInfo->mDigitCntAfterDecimal;

                        if( ( (aFormatInfo->mDigitCntBeforeDecimal > 0) && (sDigitCntAfterDecimal == 0 ) ) ||
                            ( aFormatInfo->mZeroStart > 0 ) )
                        {
                            /*
                             * 예) TO_CHAR( 0, '9' ) => ' 0'
                             *     TO_CHAR( 0, '0' ) => ' 0'
                             *     TO_CHAR( 0, '0.999' ) => ' 0.000'
                             */     
                            sDigitCntBeforeDecimal = 1;
                        }
                        else
                        {
                            /* 예) TO_CHAR( 0, '.999' ) => ' .000' */
                            sDigitCntBeforeDecimal = 0;
                        }
                        STL_THROW( RAMP_VALUE_IS_ZERO );
                    }
                    else
                    {
                        sDigitCntBeforeDecimal++;
                        sPlusOneDigitForRound  = STL_TRUE;
                    }
                }
                else
                {

                    if( sRemainder > 4 )
                    {
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }               

                    /* sTmpValue /= 10; */
                    while( ( sCarry == 1 ) && ( sTmpValue > 9 ) )
                    {
                        if( ( sTmpValue % 10 ) == 9 )
                        {
                            sTmpValue /= 10;
                            sCarry = 1;
                        }
                        else
                        {
                            sCarry = 0;
                        }
                    }

                    /* 정수가 한자리 늘어나는 경우
                     * ex) 999.999 -> 1000 */
                    if( ( sCarry == 1 ) && ( sTmpValue == 9 ) )
                    {
                        sDigitCntBeforeDecimal++;
                        sPlusOneDigitForRound  = STL_TRUE;
                    }
                    else
                    {
                        sPlusOneDigitForRound = STL_FALSE;
                    }
                }
            }
        }
        else
        {
            /* .0XXX */
            
            sZeroLen                 = sDigitCntAfterDecimal;
            sOrgDigitCntAfterDecimal = sExponent;

            if( ( aFormatInfo->mDigitCntBeforeDecimal > 0 ) &&
                ( sDigitCntAfterDecimal == 0 ) )
            {
                sDigitCntBeforeDecimal = 1;
            }
            else
            {
                sDigitCntBeforeDecimal = 0;
            }

            if( (sOrgDigitCntAfterDecimal > sDigitCntAfterDecimal) )
            {
                /*
                 * round 되는 부분의 값이 4 보다 큰지 ( 크다면 )
                 * round 되는 자리부터 .x 까지 계속 9값이어서
                 * 최후에 정수 값 + 1 이 되는지 확인
                 */
                i = sOrgDigitCntAfterDecimal - sDigitCntAfterDecimal;
                if( i > 1 ) /* 1 자리 이상 차이 나는 경우 */
                {
                    sTmpValue = i > STL_FLT_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                }
                else 
                {
                    sTmpValue = sInteger;
                }

                sRemainder = sTmpValue % 10;
                sTmpValue  = sTmpValue / 10;
                
                if( (sRemainder < 5)  && (sTmpValue == 0) && (aFormatInfo->mZeroEndAfterDecimal == 0) )
                {
                    sNumberIsZero          = STL_TRUE;
                    sDigitCntAfterDecimal  = aFormatInfo->mDigitCntAfterDecimal;

                    if( ( (aFormatInfo->mDigitCntBeforeDecimal > 0) && (sDigitCntAfterDecimal == 0 ) ) ||
                        ( aFormatInfo->mZeroStart > 0 ) )
                    {
                        /*
                         * 예) TO_CHAR( 0, '9' ) => ' 0'
                         *     TO_CHAR( 0, '0' ) => ' 0'
                         *     TO_CHAR( 0, '0.999' ) => ' 0.000'
                         */     
                        sDigitCntBeforeDecimal = 1;
                    }
                    else
                    {
                        /* 예) TO_CHAR( 0, '.999' ) => ' .000' */
                        sDigitCntBeforeDecimal = 0;
                    }
                    STL_THROW( RAMP_VALUE_IS_ZERO );
                }

                if( sRemainder > 4 )
                {
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }               

                /* sTmpValue /= 10; */
                while( ( sCarry == 1 ) && ( sTmpValue > 9 ) )
                {
                    if( ( sTmpValue % 10 ) == 9 )
                    {
                        sTmpValue /= 10;
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }
                }

                /* 정수자리 값이 증가 하는 경우 */
                if( ( sCarry == 1 ) && ( sTmpValue == 9 ) )
                {
                    sPlusOneDigitForRound  = STL_TRUE;
                }
                else
                {
                    sPlusOneDigitForRound = STL_FALSE;
                }
            }
            else
            {
                /* Do Nothing */
            }

        }
    }
    else
    {
        /* XXX000 */
        sDigitCntBeforeDecimal = sIntDigitCount + sExponent;
    }

    STL_RAMP( RAMP_VALUE_IS_ZERO );

    /* B Format */
    if( DTF_NUMBER_FMT_FLAG_IS_BLANK( aFormatInfo ) == STL_TRUE )
    {
        if( sNumberIsZero == STL_TRUE )
        {
            /* to_char( 0 , 'B' ) */
            sIsSetBlank = STL_TRUE;
        }
        else
        {
            /* do nothing */
        }

        if( sIsSetBlank == STL_TRUE )
        {
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
            {
                sReduceLen = -aFormatInfo->mToCharResultLen;
            }
            else
            {
                stlMemset( sResult , ' ' , sResultLen );
            }
            
            *aLength = aFormatInfo->mToCharResultLen + sReduceLen;
            STL_THROW( RAMP_END );
        }
        else
        {
            /* do nothing */
        }
        
    }

    /*
     * 정수 부분의 digit format 숫자 보다 실제 value 의 유효숫자 갯수가 많은 경우 '#' 표기.
     */
    if( sDigitCntBeforeDecimal > aFormatInfo->mDigitCntBeforeDecimal )
    {
        /* to_char( 12345 , '999' ) -> '#####' */
        stlMemset( sResult , '#' , aFormatInfo->mToCharResultLen );

        *aLength = aFormatInfo->mToCharResultLen;
        STL_THROW( RAMP_END );
    }
    else
    {
        /* do nothing */
    }


    /*
     * 공백 & 부호 표기 & 0인 숫자에 대한 '0' 표기
     */

    /* 공백 */

    /* sExtraDigitLen -> 공백으로 표현되는 길이
     * sExtraZeroLen  -> 공백부분에서 zero 로 표현되는 길이 */
    if( aFormatInfo->mZeroStart == 0 )
    {
        /* '0' format 이 없는 경우 */

        /* 예) TO_CHAR( 12345, '9999999' ) => 'sign__12345' => '   12345' */
        sExtraDigitLen = ( aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal );
        sExtraZeroLen = 0;
    }
    else
    {
        /*
         * '0' format 이 있는 경우
         */
        
        if( aFormatInfo->mZeroStart == 1 )
        {
            /* '0' format 이 첫번째에 오는 경우  */
        
            /* 예) TO_CHAR( 12345, '0999999' ) => 'sign0012345' => ' 0012345' */
            sExtraDigitLen = 0;
            sExtraZeroLen = (aFormatInfo->mDigitCntBeforeDecimal - sExtraDigitLen - sDigitCntBeforeDecimal );
        }
        else
        {
            /* '0' format 이 두번째 이후에 오는 경우  */
        
            sExtraDigitLen = aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal;
        
            if( sExtraDigitLen >= aFormatInfo->mZeroStart )
            {
                /* 예) TO_CHAR( 123, '9909999' ) => 'sign__00123' => '   00123' */
                sExtraDigitLen = (aFormatInfo->mZeroStart - 1);
                sExtraZeroLen =
                    (aFormatInfo->mDigitCntBeforeDecimal - sExtraDigitLen - sDigitCntBeforeDecimal);
            }
            else
            {
                /* 예) TO_CHAR( 123456, '9909999' ) => 'sign_123456' => '  123456' */
                sExtraDigitLen = (aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal);
                sExtraZeroLen = 0;
            }
        }
    }

    if( aFormatInfo->mFirstZeroOrNine != NULL )
    {
        sTmpStr = aFormatInfo->mFirstZeroOrNine;
    }
    else
    {
        sTmpStr = aFormatInfo->mStr;

        while( STL_TRUE )
        {
        
            if( ( *sTmpStr == 'S' ) || ( *sTmpStr == 's' ) || (*sTmpStr == '$') )
            {
                sTmpStr++;
                continue;
            }
        
            if( ( *sTmpStr == 'F' ) || ( *sTmpStr == 'f' ) )
            {
                if( ( *(sTmpStr+1) == 'M' ) || ( *(sTmpStr+1) == 'm' ) )
                {
                    sTmpStr += 2;
                    continue;
                }
                else
                {
                    STL_DASSERT( ( ( *sTmpStr == 'M' ) || ( *sTmpStr == 'm' ) ) );
                }
            }
            else
            {
                /* do nothing */
            }
            break;
        }
    }

    i = sExtraDigitLen;
    sExtraCommaLen = 0;

    /**
     * reading blank 가 있는 경우 decimal point 이전 '9' '0' 을 skip
     */
    while( i > 0 )
    {
        if( (*sTmpStr == '9') || (*sTmpStr == '0' ) )
        {
            i--;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
        else if( *sTmpStr == ',' )
        {
            while( *sTmpStr == ',' )
            {
                sExtraCommaLen++;
                sTmpStr++;
                DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            }
        }
        else
        {
            STL_DASSERT( STL_FALSE );
        }
    }

    if( sExtraDigitLen > 0 )
    {
        while( *sTmpStr == ',' )
        {
            sExtraCommaLen++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    sExtraSpaceLen = sExtraDigitLen + sExtraCommaLen;

    /**
     * FM 포맷에 의한 reading blank 제거
     */
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        sResultLen -= sExtraSpaceLen;
        sReduceLen -= sExtraSpaceLen;
        sExtraSpaceLen = 0;

        /**
         * 부호 길이 적용
         * SIGN 과 같이 쓰이는 경우 '+' 값도 자리수가 유효하다
         */
        
        if( DTF_NUMBER_FMT_FLAG_IS_SIGN( aFormatInfo ) == STL_FALSE )
        {
            sReduceLen -= sIsNegative == STL_TRUE ? 0 : 1;
        }
        else
        {
            /* do nothing */
        }

        if( DTF_NUMBER_FMT_FLAG_IS_BRACKET( aFormatInfo ) == STL_TRUE )
        {
            sReduceLen -= sIsNegative == STL_TRUE ? 0 : 1;
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* do nothing */
    }
    
    stlMemset( sResult, ' ', sExtraSpaceLen );
    sResult += sExtraSpaceLen;
    
    /**
     * 부호 표기 ( +, -, PR, MI )
     */

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        dtfNumberFmtSetSignForNormal( aFormatInfo, aResult, sResultLen, !sIsNegative, sResult, &sResult );
    }
    else
    {
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  sResultLen,
                                  !sIsNegative,
                                  STL_TRUE, // aIsSignLocatePre
                                  &sResultLen,
                                  sResult,
                                  &sResult );            
    }

    /**
     * DOLLAR 표기
     */

    DTF_NUMBER_FMT_SET_DOLLAR( aFormatInfo, sResult );

    /**
     * 0인 숫자에 대한 '0' 표기
     */

    if( sExtraZeroLen > 0 )
    {
        while( sExtraZeroLen > 0 )
        {
            if( ( *sTmpStr == '0' ) || ( *sTmpStr == '9' ) )
            {
                *sResult = '0';

                sResult++;
                sTmpStr++;
                DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
                sExtraZeroLen--;
            }
            else if( *sTmpStr == ',' )
            {
                DTF_NUMBER_FMT_SET_COMMA( sResult , sTmpStr );
            }
            else
            {
                STL_DASSERT( STL_FALSE );
            }
        }

        DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
    }
    else
    {
        /* do nothing */
    }

    /**
     * 숫자 표기 ( 0 인 경우 )
     */
    
    if( sNumberIsZero == STL_TRUE )
    {
        if( sDigitCntBeforeDecimal == 1 )
        {
            if( ( *sTmpStr == '0' ) || ( *sTmpStr == '9' ) )
            {
                *sResult = '0';
                sTmpStr++;
            }
            else
            {
                /* Do Nothing */
            }
            
            while( sTmpStr <= sFormatStrEnd )
            {
                if( (*sTmpStr == ',') || (*sTmpStr == '.') )
                {
                    sResult++;
                    *sResult = *sTmpStr;
                    sTmpStr++;
                }
                else if( *sTmpStr == '$' )
                {
                    sTmpStr++;
                    continue;
                }
                else
                {
                    STL_DASSERT( ( *sTmpStr != 'B' ) && ( *sTmpStr != 'b' ) );
                    break;
                }
            }
        }
        else
        {
            if( (aFormatInfo->mDigitCntBeforeDecimal > 0) &&
                (DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE) )
            {
                *sResult = '0';
                sResult++;
                sReduceLen++;
            }
        }
        
        if( (DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE) && (sDigitCntAfterDecimal > 0) )
        {        
            if( aFormatInfo->mToCharResultLen + sReduceLen > 0 )
            {
                *sResult = '.';
                sResult++;
            }

            sZeroLen = aFormatInfo->mZeroEndAfterDecimal > sDigitCntAfterDecimal ?
                sDigitCntAfterDecimal : aFormatInfo->mZeroEndAfterDecimal;
            sReduceLen -= (sDigitCntAfterDecimal - sZeroLen );
            
            while( sZeroLen > 0 )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }
        else if( sDigitCntAfterDecimal > 0 )
        {
            sZeroLen = sDigitCntAfterDecimal;
            
            STL_DASSERT( ( ( sDigitCntBeforeDecimal == 0 ) && ( *sTmpStr == '.' ) ) ||
                         ( ( sDigitCntBeforeDecimal == 1 ) && ( *sTmpStr != '.' ) ) );
            
            *sResult = '.';
            sResult++;
            
            while( sZeroLen > 0 )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }
        else
        {
            if( sTmpStr <= sFormatStrEnd )
            {
                if( *sTmpStr == '.' )
                {
                    *sResult = *sTmpStr;
                    sResult++;
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
        }
        
        STL_THROW( RAMP_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }


     /**
     * 숫자 표기 ( 0 이 아닌 경우 )
     */

    DTF_NUMBER_FMT_LAST_SPACE_USED( aFormatInfo, sLastSpaceUsed );

    /* 포맷에 의해 마지막 공간에 부호 혹은 ' ' Blank 가 set 되는 경우 */
    if( sLastSpaceUsed == STL_TRUE )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_SIGN( aFormatInfo ) == STL_TRUE )
        {
            sResultLen--;
            sFormatStrEnd--;
        }
        else
        {
            /* MI , PR */
            sResultLen--;
            sFormatStrEnd -= 2;
        }
    }
    else
    {
        /* do nothing */
    }

    sResult = aResult + sResultLen -1;
    sTmpStr = sFormatStrEnd;

    DTF_NUMBER_FMT_REWIND_V_FORMAT_STR( sTmpStr );

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent <= sIntDigitCount )
        {
            sRoundDigit = sDigitCntBeforeDecimal + sDigitCntAfterDecimal;
        
            /*  round 가 발생하는 경우
             *  round 되는 값을 미리 keep */
            if( sDigitCntAfterDecimal < sOrgDigitCntAfterDecimal )
            {
                i = sIntDigitCount - sRoundDigit;
                if( i > 1 ) /* 1 자리 이상 차이 나는 경우 */
                {
                    sInteger = i > STL_FLT_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                }
                else 
                {
                    /* do nothing */
                }

                sRemainder = sInteger % 10;
                sInteger   = sInteger / 10;
                

                if( sRemainder > 4 )
                {
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }
            }
            else
            {
                sCarry = 0;
            }

            
            sExponent  = sDigitCntAfterDecimal;

            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

            sZeroLen = sDigitCntAfterDecimal - sOrgDigitCntAfterDecimal;
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
            {
                /**
                 * FM format : '.' 이후 제로는 '0' 이전에 '9' 가 있더라도
                 * 계속 제로를 출력 한다.
                 */
                if( *sFormatStrEnd == '0' )
                {
                    sIsPrintZero = STL_TRUE;
                }
                else
                {
                    /* do nothing */
                }

                /* print trailing zero */
                while( sZeroLen > 0 )
                {
                    DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

                    if( sIsPrintZero == STL_FALSE )
                    {
                        if( *sTmpStr == '0' )
                        {
                            sIsPrintZero = STL_TRUE;
                        }
                        else
                        {
                            /* do nothing */
                        }
                    }
                    else
                    {
                        /* do nothing */
                    }

                    if( sIsPrintZero == STL_TRUE )
                    {
                        *sResult = '0';
                        sIsPrintZero = STL_TRUE;
                    }
                    else
                    {
                        sReduceLen--;
                    }

                    sRoundDigit--;
                    sExponent--;
                    sTmpStr--;
                    sResult--;
                    sZeroLen--;
                }

                /* 소수점 digit 출력 */
                while( (sExponent > 0) && (sIntDigitCount > 0 ) )
                {
                    DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

                    if( sIsPrintZero == STL_FALSE )
                    {
                        if( *sTmpStr == '0' )
                        {
                            sIsPrintZero = STL_TRUE;
                        }
                        else
                        {
                            /* do nothing */
                        }
                    }
                    else
                    {
                        /* do nothing */
                    }
                
                    sTmpValue = sInteger % 10;
                    if( (sIsPrintZero == STL_FALSE ) && (sTmpValue == 0) )
                    {
                        sReduceLen--;
                    }

                    if( (sIsPrintZero == STL_TRUE) || sTmpValue != 0 )
                    {
                        *sResult = sTmpValue + '0';
                        sIsPrintZero = STL_TRUE;
                    }
                    else
                    {
                        /* do nothing */
                    }

                    sInteger = sInteger / 10;

                    sTmpStr--;
                    sExponent--;
                    sRoundDigit--;
                    sIntDigitCount--;
                    sResult--;
                }
            }
            else
            {
                /* trailing zero 출력 */
                while( sZeroLen > 0 )
                {
                    DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
                
                    *sResult = '0';
                
                    sTmpStr--;
                    sExponent--;
                    sRoundDigit--;
                    sResult--;
                    sZeroLen--;

                }

                /* 소수점 digit 출력 */
                while( (sExponent > 0) && (sIntDigitCount > 0 ) )
                {
                    DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
                
                    sTmpValue = sInteger % 10;
                    *sResult = sTmpValue + '0';

                    sInteger = sInteger / 10;

                    sTmpStr--;
                    sExponent--;
                    sRoundDigit--;
                    sIntDigitCount--;
                    sResult--;

                }
            }

            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

            if( sDigitCntAfterDecimal > 0 )
            {
                STL_DASSERT( *sTmpStr == '.' );
                
                *sResult = '.';
                sResult--;
                sTmpStr--;
            }
            else
            {
                if( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aFormatInfo ) == STL_TRUE )
                {
                    STL_DASSERT( *sTmpStr == '.' );

                    *sResult = '.';
                    sResult--;
                    sTmpStr--;
                }
                else
                {
                    // ?????
                    /* Do Nothing */
                }
            }           

            /* 정수부 출력 */
            while( ( sRoundDigit > 0 ) && ( sIntDigitCount > 0 ) )
            {
                DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
                
                DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( sResult , sTmpStr );
                
                sTmpValue = sInteger % 10;
                *sResult = sTmpValue + '0';               

                sInteger = sInteger / 10;
                sTmpStr--;
                sExponent--;
                sRoundDigit--;
                sIntDigitCount--;
                sResult--;
            }

            if( sCarry == 1 )
            {                   
                sRoundDigit =
                    sDigitCntBeforeDecimal + sDigitCntAfterDecimal -
                    ( sPlusOneDigitForRound == STL_FALSE ? 0 : 1 ); 

              
                sResult = aResult + sResultLen -1;


                while( ( sCarry == 1 ) && ( sRoundDigit > 0 ) )
                {
                    if( ( *sResult == '.' ) || ( *sResult == ',' ) )
                    {
                        sResult--;

                        while( *sResult == ',' )
                        {
                            sResult--;
                        }

                        continue;
                    }

                    *sResult += sCarry;

                    if( *sResult > '9' )
                    {
                        *sResult = '0';
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }

                    sRoundDigit--;
                    sResult--;
                }
                    
                if( (sRoundDigit == 0) && (sCarry == 1) )
                {
                    if( ( *sResult == '.' ) || ( *sResult == ',' ) )
                    {
                        sResult--;

                        while( *sResult == ',' )
                        {
                            sResult--;
                        }
                    }
                    else
                    {
                        /* Do Nothing */
                    }

                    *sResult = '1';
                }
                else
                {
                    /* Do Nothing */
                }

                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                {
                    /**
                     * Carry 에 의해 생성된 '0' 을 제거
                     */
                    sResult = aResult + sResultLen -1;
                    while( (sDigitCntAfterDecimal > 0) && (*sFormatStrEnd != '0') && (*sResult == '0') )
                    {
                        sResult--;
                        sReduceLen--;
                        sFormatStrEnd--;
                    }
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            /**
             * 0.0XXXX
             */
            sRoundDigit = sDigitCntAfterDecimal;

            /**
             * sCarry : round trucated 로 반올림이 발생하는 경우
             *          sZeroLen 가 줄어든다.
             */
            sZeroLen = sExponent - sIntDigitCount - sCarry;

            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
            {
                if( *sFormatStrEnd == '0' )
                {
                    sIsPrintZero = STL_TRUE;
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* do nothing */
            }

            /**
             * 제로길이가 sDigitCntAfterDecimal 보다
             * 크다면 0만 출력
             */
            if( sZeroLen >= sDigitCntAfterDecimal )
            {
                sZeroLen = sDigitCntAfterDecimal;
                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                {
                    while( sZeroLen > 0 )
                    {
                        if( sIsPrintZero == STL_FALSE )
                        {
                            if( *sTmpStr == '0' )
                            {
                                sIsPrintZero = STL_TRUE;
                            }
                            else
                            {
                                /* do nothing */
                            }
                        }
                        
                        if( sIsPrintZero == STL_TRUE )
                        {
                            *sResult = '0'; 
                        }
                        else
                        {
                            sReduceLen--;
                        }
                        
                        sResult--;
                        sZeroLen--;
                        sTmpStr--;
                    }
                    
                }
                else
                {
                    while( sZeroLen > 0 )
                    {
                        *sResult = '0';
                        sResult--;
                        sZeroLen--;
                    }
                }
            }
            else
            {
                sZeroLen = sExponent - sIntDigitCount;

                /* write trailing zero */
                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                {
                    sTrailingZeroLen = sDigitCntAfterDecimal - sOrgDigitCntAfterDecimal;
                    while( sTrailingZeroLen > 0 )
                    {
                        if( sIsPrintZero == STL_FALSE )
                        {
                            if( *sTmpStr == '0' )
                            {
                                sIsPrintZero = STL_TRUE;
                            }
                            else
                            {
                                /* do nothing */
                            }
                        }
                        else
                        {
                            /* do nothing */
                        }

                        if( sIsPrintZero == STL_TRUE )
                        {
                            *sResult = '0';
                            sIsPrintZero = STL_TRUE;
                        }
                        else
                        {
                            sReduceLen--;
                        }
                        
                        sResult--;
                        sTrailingZeroLen--;
                        sTmpStr--;
                    }
                }
                else
                {
                    sTrailingZeroLen = sDigitCntAfterDecimal - sOrgDigitCntAfterDecimal;
                    while( sTrailingZeroLen > 0 )
                    {
                        *sResult = '0';
                        sResult--;
                        sTrailingZeroLen--;
                    }          
                }

                /*  round 가 발생하는 경우
                 *  round 되는 값을 미리 keep */
                if( sDigitCntAfterDecimal < sOrgDigitCntAfterDecimal )
                {
                    i = sOrgDigitCntAfterDecimal - sDigitCntAfterDecimal;
                    if( i > 1 ) /* 1 자리 이상 차이 나는 경우 */
                    {
                        sTmpValue = i > STL_FLT_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                    }
                    else 
                    {
                        sTmpValue = sInteger;
                    }

                    sRemainder = sTmpValue % 10;
                    sInteger   = sTmpValue / 10;
                    sIntDigitCount -= i;
                

                    if( sRemainder > 4 )
                    {
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }
                }
                else
                {
                    sCarry = 0;
                }               

                /* 소수 출력 */
                while( (sRoundDigit > 0) && (sIntDigitCount > 0 ) )
                {
                    sTmpValue = sInteger % 10;
                    *sResult = sTmpValue + '0';

                    sInteger = sInteger / 10;
                    sRoundDigit--;
                    sIntDigitCount--;
                    sResult--;
                }

                /* write reading zero */
                while( sZeroLen > 0 )
                {
                    *sResult = '0';
                    sResult--;
                    sZeroLen--;
                    sRoundDigit--;
                }

            }

            if( sDigitCntAfterDecimal > 0 )
            {
                *sResult = '.';
                sResult--;
            }
            else
            {
                /* Do Nothing */
            }

            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );

            if( sCarry == 1 )
            {                    
                sRoundDigit = sDigitCntAfterDecimal;

                sResult = aResult + sResultLen -1;
                while( ( sCarry == 1 ) && ( sRoundDigit > 0 ) )
                {
                    *sResult += sCarry;

                    if( *sResult > '9' )
                    {
                        *sResult = '0';
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }

                    sRoundDigit--;
                    sResult--;
                }
                                    
                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                {
                    sResult = aResult + sResultLen -1;
                    while( ( *sFormatStrEnd != '0' ) && ( *sResult == '0' ) )
                    {
                        sResult--;
                        sReduceLen--;
                        sFormatStrEnd--;
                    }
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* do nothing */
            }
        }
    }
    else
    {
        sRoundDigit = sDigitCntBeforeDecimal;
        
        sTmpStr = sFormatStrEnd;
        sExponent  = sDigitCntAfterDecimal;

        DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

        if( aFormatInfo->mDigitCntAfterDecimal > 0 )
        {
            sZeroLen = aFormatInfo->mDigitCntAfterDecimal;
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
            {
                if( *sTmpStr == '0' )
                {
                    while( sZeroLen > 0 )
                    {
                        *sResult = '0';
                
                        sTmpStr--;
                        sExponent--;
                        sResult--;
                        sZeroLen--;

                        DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
                    }
                }
                else
                {
                    while( sZeroLen > 0 )
                    {
                        DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

                        sReduceLen--;
                        sTmpStr--;
                        sExponent--;
                        sResult--;
                        sZeroLen--;
                    }
                }
            }
            else
            {
                while( sZeroLen > 0 )
                {
                    *sResult = '0';
                
                    sTmpStr--;
                    sExponent--;
                    sResult--;
                    sZeroLen--;

                    DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
                }
            }
        }
        else
        {
            /* do nothing */
        }

        DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
        
        while( STL_TRUE )
        {
            if( (*sTmpStr == '.') || (*sTmpStr == ',') )
            {
                if( sRoundDigit > 0 )
                {
                    *sResult = *sTmpStr;
                    sResult--;
                    sTmpStr--;
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                break;
            }
        }

        DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

        /* print trailing zero */
        sZeroLen = sDigitCntBeforeDecimal - sIntDigitCount;
        while( sZeroLen > 0 )
        {
            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

            DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( sResult , sTmpStr );
            
            *sResult = '0';
                
            sTmpStr--;
            sExponent--;
            sRoundDigit--;
            sResult--;
            sZeroLen--;
        }


        
        
        /* 정수부 출력 */
        while( ( sRoundDigit > 0 ) && ( sIntDigitCount > 0 ) )
        {
            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
            
            DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( sResult , sTmpStr );
            
            sTmpValue = sInteger % 10;
            *sResult = sTmpValue + '0';              

            sInteger = sInteger / 10;
            sTmpStr--;
            sExponent--;
            sRoundDigit--;
            sIntDigitCount--;
            sResult--;
        }       
    }

    
    STL_RAMP( RAMP_SUCCESS );

    *aLength = aFormatInfo->mToCharResultLen + sReduceLen;
    
    /**
     * FM format 시 decimal point 이후 '9' format 에 해당하는
     * '0' 문자의 위치를 선계산 할 수 없으므로
     * 문자열 변환이 끝나고 'S,MI,PR' 등을 재 출력 한다.
     */
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        sResult = aResult + ( *aLength - 1 < 0 ? 0 : *aLength -1 );

        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  *aLength,
                                  !sIsNegative,
                                  STL_FALSE,
                                  &sResultLen,
                                  sResult,
                                  &sResult );
    }
    else
    {
        /* do nothing */
    }

    STL_RAMP( RAMP_END );

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    } 

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief native_real type을 V format string에 맞게 string으로 변환한다.
 * @param[in]  aDataValue          dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharRealToVFormatString( dtlDataValue         * aDataValue,
                                        dtfNumberFormatInfo  * aFormatInfo,
                                        stlInt64               aAvailableSize,
                                        stlChar              * aResult,
                                        stlInt64             * aLength,
                                        dtlFuncVector        * aVectorFunc,
                                        void                 * aVectorArg,
                                        stlErrorStack        * aErrorStack )
{

    dtlRealType     sValue;
    stlFloat64      sPow;
    stlBool         sIsNegative              = STL_FALSE;
    stlInt64        sInteger                 = 0;
    stlBool         sIsNegativeExp           = STL_FALSE;
    stlInt32        sExponent                = 0;
    stlInt64        sTotDigitCount           = 0;
    stlInt64        sIntDigitCount           = 0;
    stlUInt8        sCarry                   = 0;
    stlInt32        i                        = 0;
    stlInt32        sTmpValue                = 0;
    stlInt32        sRemainder               = 0;
    stlInt32        sTrailingZeroLen         = 0;
    stlInt32        sZeroLen                 = 0;
    stlInt32        sOrgDigitCntAfterDecimal = 0;
    stlInt32        sDigitCntAfterV          = 0;
    stlInt32        sDigitCntAfterDecimal    = 0;
    stlInt32        sDigitCntBeforeDecimal   = 0;
    stlInt32        sResultLen               = aFormatInfo->mToCharResultLen;
    stlInt32        sReduceLen               = 0;
    stlInt32        sExtraDigitLen           = 0;
    stlInt32        sExtraCommaLen           = 0;
    stlInt32        sExtraZeroLen            = 0;
    stlInt32        sExtraSpaceLen           = 0;
    stlInt32        sRoundDigit              = 0;
    stlInt32        sReadingZeroLen          = 0;
    stlChar       * sResult                  = aResult;
    stlChar       * sTmpStr                  = NULL;
    stlChar       * sFormatStrEnd            = aFormatInfo->mStr + aFormatInfo->mStrLen -1;
    stlBool         sPlusOneDigitForRound    = STL_FALSE;
    stlBool         sIsSetBlank              = STL_FALSE;
    stlBool         sNumberIsZero            = STL_FALSE;
    stlBool         sLastSpaceUsed           = STL_FALSE;
 

    sValue = *((dtlRealType*) aDataValue->mValue);

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        STL_TRY_THROW( aAvailableSize >= DTL_BINARY_REAL_INFINITY_SIZE, ERROR_NOT_ENOUGH_BUFFER );
        stlMemcpy( (stlChar*)aResult, DTL_BINARY_REAL_INFINITY, DTL_BINARY_REAL_INFINITY_SIZE );
        *aLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }

    if( sValue == 0.0 )
    {       
        sIsNegative            = STL_FALSE;
        sNumberIsZero          = STL_TRUE;
        sDigitCntBeforeDecimal = aFormatInfo->mDigitCntAfterV;

        STL_THROW( RAMP_VALUE_IS_ZERO );
        
    }

    if( sValue < 0.0 )
    {
        sIsNegative = STL_TRUE;
        sValue = -sValue;
    }
    else
    {
        sIsNegative = STL_FALSE;
    }

    sTotDigitCount = stlLog10f( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }

    sExponent = sTotDigitCount - STL_FLT_DIG;
    DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
    sInteger = (stlInt64)(sValue / sPow + 0.5);

    DTL_IS_VALID( stlGet10Pow( STL_FLT_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_FLT_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sIsNegative      sValue의 음수여부
     * sInteger         sValue의 STL_FLT_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값
     * */


    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent <= sIntDigitCount )
        {
            /* XXX.XXX */

            sTrailingZeroLen = ( aFormatInfo->mDigitCntAfterV - sExponent ) > 0 ?
                               aFormatInfo->mDigitCntAfterV - sExponent : 0;
            sDigitCntBeforeDecimal   = sIntDigitCount - sExponent + aFormatInfo->mDigitCntAfterV;
            sOrgDigitCntAfterDecimal = sExponent;

            if( sOrgDigitCntAfterDecimal > aFormatInfo->mDigitCntAfterV )
            {
                i = sOrgDigitCntAfterDecimal - aFormatInfo->mDigitCntAfterV;
                if( i > 1 )
                {
                    sTmpValue = i > STL_FLT_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                }
                else
                {
                    sTmpValue = sInteger;
                }
                                
                sRemainder = sTmpValue % 10;

                if( sRemainder > 4 )
                {
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }               

                sTmpValue /= 10;
                while( ( sCarry == 1 ) && ( sTmpValue > 9 ) )
                {
                    if( ( sTmpValue % 10 ) == 9 )
                    {
                        sTmpValue /= 10;
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }
                }

                /* 정수가 한자리 늘어나는 경우
                 * ex) 999.999 -> 1000 */
                if( ( sCarry == 1 ) && ( sTmpValue == 9 ) )
                {
                    sPlusOneDigitForRound  = STL_TRUE;
                    sDigitCntBeforeDecimal++;
                }
                else
                {
                    sPlusOneDigitForRound = STL_FALSE;
                }
            }

            /*
             * Round 로 인한 DigitCnt overflow
             */
            if( (sIntDigitCount - ( sExponent - aFormatInfo->mDigitCntAfterV ) +
                 (sPlusOneDigitForRound == STL_TRUE ? 1 : 0 ) ) > aFormatInfo->mDigitCntBeforeDecimal )
            {
                stlMemset( sResult , '#' , aFormatInfo->mToCharResultLen );

                *aLength = aFormatInfo->mToCharResultLen;
                STL_THROW( RAMP_END );
            }
        }
        else
        {
            /* .0XXXXX */

            sReadingZeroLen = sExponent - sIntDigitCount;
            
            sDigitCntBeforeDecimal   = aFormatInfo->mDigitCntAfterV;
            sOrgDigitCntAfterDecimal = sExponent - aFormatInfo->mDigitCntAfterV;
            
            if( sReadingZeroLen > aFormatInfo->mDigitCntAfterV )
            {
                sNumberIsZero = STL_TRUE;
                STL_THROW( RAMP_VALUE_IS_ZERO );
            }
            else
            {
                /* do nothing */
            }

            if( sOrgDigitCntAfterDecimal > 0 )
            {
                /*
                 * round 되는 부분의 값이 4 보다 큰지 ( 크다면 )
                 * round 되는 자리부터 .x 까지 계속 9값이어서
                 * 최후에 정수 값 + 1 이 되는지 확인
                 */
                i = sOrgDigitCntAfterDecimal;
                if( i > 1) /* 1 자리 이상 차이 나는 경우 */
                {
                    sTmpValue = i > STL_FLT_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                }
                else 
                {
                    sTmpValue = sInteger;
                }
                
                sRemainder = sTmpValue % 10;

                if( sRemainder > 4 )
                {
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }               

                while( ( sCarry == 1 ) && ( sTmpValue > 9 ) )
                {
                    if( ( sTmpValue % 10 ) == 9 )
                    {
                        sTmpValue /= 10;
                        sCarry = 1;
                    }
                    else
                    {
                        sCarry = 0;
                    }
                }

                /* 정수자리 값이 증가 하는 경우 */
                if( ( sCarry == 1 ) && ( sTmpValue > 4 ) )
                {
                    sPlusOneDigitForRound  = STL_TRUE;
                }
                else
                {
                    sPlusOneDigitForRound = STL_FALSE;
                }
            }
            else
            {
                /* Do Nothing */
            }           
        }
    }
    else
    {
        /* XXX000 */
        sZeroLen               = aFormatInfo->mDigitCntAfterV;
        sTrailingZeroLen       = sExponent;
        sDigitCntBeforeDecimal = sIntDigitCount + sExponent + aFormatInfo->mDigitCntAfterV;;
    }

    STL_RAMP( RAMP_VALUE_IS_ZERO );

    /* B Format */
    if( DTF_NUMBER_FMT_FLAG_IS_BLANK( aFormatInfo ) == STL_TRUE )
    {
        if( sNumberIsZero == STL_TRUE )
        {
            /* to_char( 0 , 'B' ) */
            sIsSetBlank = STL_TRUE;
        }
        else
        {
            if( sIsNegativeExp == STL_TRUE )
            {
                if( sExponent >= sIntDigitCount + aFormatInfo->mDigitCntAfterV )
                {
                    /* .XXXXXX */
                    if( sPlusOneDigitForRound == STL_FALSE) 
                    {
                        sIsSetBlank = STL_TRUE;
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* XXX.XXX
                     * do nothing */
                }
            }
            else
            {
                /* XXX000
                 * do nothing */
            }
        }

        if( sIsSetBlank == STL_TRUE )
        {
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
            {
                sReduceLen = -aFormatInfo->mToCharResultLen;
            }
            else
            {
                stlMemset( sResult , ' ' , sResultLen );
            }

            *aLength = aFormatInfo->mToCharResultLen + sReduceLen;
            STL_THROW( RAMP_END );
        }
        else
        {
            /* do nothing */
        }
        
    }

    /*
     * Round 로 인한 DigitCnt overflow
     */
    if( sDigitCntBeforeDecimal > aFormatInfo->mDigitCntBeforeDecimal )
    {
        /* to_char( 12345 , '999' ) -> '#####' */
        stlMemset( sResult , '#' , aFormatInfo->mToCharResultLen );

        *aLength = aFormatInfo->mToCharResultLen;
        STL_THROW( RAMP_END );
    }
    else
    {
        /* do nothing */
    }


    /*
     * 공백 & 부호 표기 & 0인 숫자에 대한 '0' 표기
     */

    /* 공백 */

    if( aFormatInfo->mZeroStart == 0 )
    {
        /* '0' format 이 없는 경우 */

        /* 예) TO_CHAR( 12345, '9999999' ) => 'sign__12345' => '   12345' */
        sExtraDigitLen = ( aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal );
        sExtraZeroLen = 0;
    }
    else
    {
        /*
         * '0' format 이 있는 경우
         */
        
        if( aFormatInfo->mZeroStart == 1 )
        {
            /* '0' format 이 첫번째에 오는 경우  */
        
            /* 예) TO_CHAR( 12345, '0999999' ) => 'sign0012345' => ' 0012345' */
            sExtraDigitLen = 0;
            sExtraZeroLen = (aFormatInfo->mDigitCntBeforeDecimal - sExtraDigitLen - sDigitCntBeforeDecimal );
        }
        else
        {
            /* '0' format 이 두번째 이후에 오는 경우  */
        
            sExtraDigitLen = aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal;
        
            if( sExtraDigitLen >= aFormatInfo->mZeroStart )
            {
                /* 예) TO_CHAR( 123, '9909999' ) => 'sign__00123' => '   00123' */
                sExtraDigitLen = (aFormatInfo->mZeroStart - 1);
                sExtraZeroLen =
                    (aFormatInfo->mDigitCntBeforeDecimal - sExtraDigitLen - sDigitCntBeforeDecimal);
            }
            else
            {
                /* 예) TO_CHAR( 123456, '9909999' ) => 'sign_123456' => '  123456' */
                sExtraDigitLen = (aFormatInfo->mDigitCntBeforeDecimal - sDigitCntBeforeDecimal);
                sExtraZeroLen = 0;
            }
        }
    }

    if( aFormatInfo->mFirstZeroOrNine != NULL )
    {
        sTmpStr = aFormatInfo->mFirstZeroOrNine;
    }
    else
    {
        sTmpStr = aFormatInfo->mStr;

        while( STL_TRUE )
        {
            if( ( *sTmpStr == 'S' ) || ( *sTmpStr == 's' ) || (*sTmpStr == '$') )
            {
                sTmpStr++;
                continue;
            }
        
            if( ( *sTmpStr == 'F' ) || ( *sTmpStr == 'f' ) )
            {
                if( ( *(sTmpStr+1) == 'M' ) || ( *(sTmpStr+1) == 'm' ) )
                {
                    sTmpStr += 2;
                    continue;
                }
                else
                {
                    STL_DASSERT( ( ( *sTmpStr == 'M' ) || ( *sTmpStr == 'm' ) ) );
                }
            }
            else
            {
                /* do nothing */
            }

            break;
        }
    }

    i = sExtraDigitLen;
    sExtraCommaLen = 0;   
    
    while( i > 0 )
    {
        if( (*sTmpStr == '9') || (*sTmpStr == '0' ) )
        {
            i--;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
        else if( *sTmpStr == ',' )
        {
            while( *sTmpStr == ',' )
            {
                sExtraCommaLen++;
                sTmpStr++;
                DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
            }
        }
        else
        {
            STL_DASSERT( STL_FALSE );
        }
    }

    if( sExtraDigitLen > 0 )
    {
        while( *sTmpStr == ',' )
        {
            sExtraCommaLen++;
            sTmpStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    sExtraSpaceLen = sExtraDigitLen + sExtraCommaLen;

    /**
     * FM 포맷에 의한 reading blank 제거
     */
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        sResultLen -= sExtraSpaceLen;
        sReduceLen -= sExtraSpaceLen;
        sExtraSpaceLen = 0;

        /**
         * 부호 길이 적용
         * SIGN 과 같이 쓰이는 경우 '+' 값도 자리수가 유효하다
         */
        
        if( DTF_NUMBER_FMT_FLAG_IS_SIGN( aFormatInfo ) == STL_FALSE )
        {
            sReduceLen -= sIsNegative == STL_TRUE ? 0 : 1;
        }
        else
        {
            /* do nothing */
        }

        if( DTF_NUMBER_FMT_FLAG_IS_BRACKET( aFormatInfo ) == STL_TRUE )
        {
            sReduceLen -= sIsNegative == STL_TRUE ? 0 : 1;
        }
        else
        {
            /* do nothing */
        }

        
    }
    else
    {
        /* do nothing */
    }
   
    stlMemset( sResult, ' ', sExtraSpaceLen );
    sResult += sExtraSpaceLen;
    
    /**
     * 부호 표기 ( +, -, PR, MI )
     */

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        dtfNumberFmtSetSignForNormal( aFormatInfo, aResult, sResultLen, !sIsNegative, sResult, &sResult );
    }
    else
    {
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  sResultLen,
                                  !sIsNegative,
                                  STL_TRUE, // aIsSignLocatePre
                                  &sResultLen,
                                  sResult,
                                  &sResult );            
    }

    /**
     * DOLLAR 표기
     */

    DTF_NUMBER_FMT_SET_DOLLAR( aFormatInfo, sResult );

    /**
     * 0인 숫자에 대한 '0' 표기
     */

    if( sExtraZeroLen > 0 )
    {
        while( sExtraZeroLen > 0 )
        {
            if( ( *sTmpStr == '0' ) || ( *sTmpStr == '9' ) )
            {
                *sResult = '0';

                sResult++;
                sTmpStr++;
                DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpStr );
                sExtraZeroLen--;
            }
            else if( *sTmpStr == ',' )
            {
                DTF_NUMBER_FMT_SET_COMMA( sResult , sTmpStr );
            }
            else
            {
                STL_DASSERT( STL_FALSE );
            }
        }

        DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
    }
    else
    {
        /* do nothing */
    }

    /**
     * 숫자 표기 ( 0 인 경우 )
     */

    if( sNumberIsZero == STL_TRUE )
    {
        while( sDigitCntBeforeDecimal > 0 )
        {
            *sResult = '0';
            sResult++;
            sDigitCntBeforeDecimal--;
        }
        
        STL_THROW( RAMP_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }
    
     /**
     * 숫자 표기 ( 0 이 아닌 경우 )
     */

    DTF_NUMBER_FMT_LAST_SPACE_USED( aFormatInfo, sLastSpaceUsed );

    /* 포맷에 의해 마지막 공간에 부호 혹은 ' ' Blank 가 set 되는 경우 */
    if( sLastSpaceUsed == STL_TRUE )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_SIGN( aFormatInfo ) == STL_TRUE )
        {
            sResultLen--;
            sFormatStrEnd--;
        }
        else
        {
            /* MI , PR */
            sResultLen--;
            sFormatStrEnd -= 2;
        }
    }
    else
    {
        /* do nothing */
    }

    sResult = aResult + sResultLen -1;
    sTmpStr = sFormatStrEnd;

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent <= sIntDigitCount )
        {
            sRoundDigit = sDigitCntBeforeDecimal;
        
            if( sOrgDigitCntAfterDecimal > aFormatInfo->mDigitCntAfterV )
            {
                i = sOrgDigitCntAfterDecimal - aFormatInfo->mDigitCntAfterV;
                if( i > 1 ) /* 1 자리 이상 차이 나는 경우 */
                {
                    sInteger = i > STL_FLT_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                }
                else 
                {
                    /* do nothing */
                }

                sRemainder = sInteger % 10;
                sInteger   = sInteger / 10;
                

                if( sRemainder > 4 )
                {
                    sInteger++;
                }
                else
                {
                    /* do nothing */
                }
            }
            else
            {
                /* do nothing */
            }

            sDigitCntAfterV = aFormatInfo->mDigitCntAfterV;

            /* print trailing zero */
            while( sTrailingZeroLen > 0 )
            {
                DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
                
                *sResult = '0';
                
                sTmpStr--;
                sRoundDigit--;
                sResult--;
                sTrailingZeroLen--;
                sDigitCntAfterV--;

            }

            /* V 포맷 정수 출력 */
            while( sDigitCntAfterV > 0 )
            {
                DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

                sTmpValue = sInteger % 10;
                *sResult = sTmpValue + '0';               

                sInteger = sInteger / 10;

                sTmpStr--;
                sRoundDigit--;
                sResult--;
                sDigitCntAfterV--;
            }

            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

            if( ( *sTmpStr == 'V' ) || ( *sTmpStr == 'v' ) )
            {
                sTmpStr--;
            }
            else
            {
                /* do nothing */
            }

            /* 정수부 출력 */
            while( sRoundDigit > 0 )
            {
                DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
                
                DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( sResult , sTmpStr );
                
                sTmpValue = sInteger % 10;
                *sResult = sTmpValue + '0';               

                sInteger = sInteger / 10;
                sTmpStr--;
                sRoundDigit--;
                sResult--;
            }
        }
        else
        {
            /* .0XXXXX */

            sReadingZeroLen          = sExponent - sIntDigitCount;
            sTrailingZeroLen         = ( aFormatInfo->mDigitCntAfterV - sExponent > 0 ?
                                         aFormatInfo->mDigitCntAfterV - sExponent : 0 );
            sDigitCntBeforeDecimal   = aFormatInfo->mDigitCntAfterV;
            sOrgDigitCntAfterDecimal = sExponent - aFormatInfo->mDigitCntAfterV;
            
            sRoundDigit = aFormatInfo->mDigitCntAfterV - sReadingZeroLen - sTrailingZeroLen +
                          ( sPlusOneDigitForRound == STL_TRUE ? 1 : 0 );
            sReadingZeroLen -= ( sPlusOneDigitForRound == STL_TRUE ? 1 : 0 );

            if( sOrgDigitCntAfterDecimal > 0 )
            {
                i = sOrgDigitCntAfterDecimal;
                if( i > 1 )
                {
                    sInteger = i > STL_FLT_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
                }
                else
                {
                    /* do nothing */
                }

                sTmpValue = sInteger % 10;
                sInteger  /= 10;
                sIntDigitCount -= i;
                
                if( sTmpValue > 4 )
                {
                    sInteger++;
                }
                else
                {
                    /* do nothing */
                }

            }

            /* trailing zero */
            while( sTrailingZeroLen > 0 )
            {
                *sResult = '0';
                sResult--;
                sTrailingZeroLen--;
            }
            
            /* 소수 출력 */
            while( sRoundDigit > 0 )
            {
                sTmpValue = sInteger % 10;
                *sResult = sTmpValue + '0';

                sInteger = sInteger / 10;
                sRoundDigit--;
                sResult--;
            }

            /* reading zero */
            while( sReadingZeroLen > 0 )
            {
                *sResult = '0';
                sResult--;
                sReadingZeroLen--;
            }

            DTF_NUMBER_FMT_SET_COMMA( sResult, sTmpStr );
        }
    }
    else
    {
        sRoundDigit = sDigitCntBeforeDecimal;
        
        sTmpStr = sFormatStrEnd;
        sExponent  = sDigitCntAfterDecimal;

        DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

        /* print trailing zero */
        sZeroLen = sDigitCntBeforeDecimal - sIntDigitCount;
        while( sZeroLen > 0 )
        {
            *sResult = '0';
                
            sTmpStr--;
            sExponent--;
            sRoundDigit--;
            sResult--;
            sZeroLen--;

            DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( sResult , sTmpStr );
            
            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );
        }


        DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( sResult , sTmpStr );
        
        /* 정수부 출력 */
        while( ( sRoundDigit > 0 ) && ( sIntDigitCount > 0 ) )
        {
            DTF_NUMBER_FMT_REWIND_BLANK_DOLLAR_PREVIOUS_FORMAT_STR( sTmpStr );

            DTF_NUMBER_FMT_SET_COMMA_AND_REWIND( sResult , sTmpStr );
            
            sTmpValue = sInteger % 10;
            *sResult = sTmpValue + '0';              

            sInteger = sInteger / 10;
            sTmpStr--;
            sExponent--;
            sRoundDigit--;
            sIntDigitCount--;
            sResult--;
        }       
    }

    
    STL_RAMP( RAMP_SUCCESS );

    *aLength = aFormatInfo->mToCharResultLen + sReduceLen;

    /**
     * FM format 시 decimal point 이후 '9' format 에 해당하는
     * '0' 문자의 위치를 선계산 할 수 없으므로
     * 문자열 변환이 끝나고 'S,MI,PR' 등을 재 출력 한다.
     */
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        sResult = aResult + ( *aLength - 1 < 0 ? 0 : *aLength -1 );
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  *aLength,
                                  !sIsNegative,
                                  STL_FALSE,
                                  &sResultLen,
                                  sResult,
                                  &sResult );

    }
    else
    {
        /* do nothing */
    }

    STL_RAMP( RAMP_END );

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    } 

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief native_real type을 X format string에 맞게 string으로 변환한다.
 * @param[in]  aDataValue          dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharRealToXFormatString( dtlDataValue         * aDataValue,
                                        dtfNumberFormatInfo  * aFormatInfo,
                                        stlInt64               aAvailableSize,
                                        stlChar              * aResult,
                                        stlInt64             * aLength,
                                        dtlFuncVector        * aVectorFunc,
                                        void                 * aVectorArg,
                                        stlErrorStack        * aErrorStack )
{
    dtlRealType     sValue;
    stlFloat64      sPow;
    stlInt64        sInteger                 = 0;
    stlUInt64       sResultValue[4];
    stlBool         sIsNegativeExp           = STL_FALSE;
    stlBool         sCarry                   = STL_FALSE;
    stlInt32        sTmpExponent             = 0;
    stlInt32        sExponent                = 0;
    stlInt64        sTotDigitCount           = 0;
    stlInt64        sIntDigitCount           = 0;
    stlInt32        sDigitCount              = 0;
    stlUInt64       sUInt64Value             = 0;
    stlInt32        sResultLen               = aFormatInfo->mToCharResultLen;
    stlInt32        sReduceLen               = 0;
    stlInt32        sTmpValue                = 0;
    stlInt32        sZeroLen                 = 0;
    stlUInt8      * sDigit                   = NULL;
    stlChar       * sResult                  = aResult;
    stlBool         sNumberIsZero            = STL_FALSE;
    stlBool         sZeroFormatFlag          = STL_FALSE;
    stlUInt8        sLowerUpperIdx           = aFormatInfo->mLowerUpperIdx;    
 
#ifndef STL_IS_BIGENDIAN     
    stlInt32        i = 0;
#endif    

    sValue = *((dtlRealType*) aDataValue->mValue);

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        STL_TRY_THROW( aAvailableSize >= DTL_BINARY_REAL_INFINITY_SIZE, ERROR_NOT_ENOUGH_BUFFER );
        stlMemcpy( (stlChar*)aResult, DTL_BINARY_REAL_INFINITY, DTL_BINARY_REAL_INFINITY_SIZE );
        *aLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }

    if( sValue == 0.0 )
    {
        sNumberIsZero            = STL_TRUE;
        STL_THROW( RAMP_VALUE_IS_ZERO );       
    }

    /* STL_FLT_DIG만큼의 유효숫자(trailing zero는 제거한 상태)와 
     * exponent의 형태로 만든다. */
    if( sValue < 0.0 )
    {
        STL_THROW( RAMP_CANNOT_DISPLAY );
    }
    else
    {
        /* do nothing */
    }

    sTotDigitCount = stlLog10( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }


    sExponent = sTotDigitCount - STL_FLT_DIG;
    DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
    sInteger = (stlInt64)(sValue / sPow + 0.5);

    DTL_IS_VALID( stlGet10Pow( STL_FLT_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_FLT_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sIsNegative      sValue의 음수여부
     * sInteger         sValue의 STL_FLT_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값
     * */

    STL_RAMP( RAMP_VALUE_IS_ZERO );

    /**
     * '0' 포맷으로 인한 Zero 길이
     */
    if( aFormatInfo->mZeroStart > 0 )
    {
        sZeroFormatFlag = STL_TRUE;
    }
    else
    {
        sZeroFormatFlag = STL_FALSE;
    }
   
    /**
     * 숫자 표기 ( 0 인 경우 )
     */
    
    if( sNumberIsZero == STL_TRUE )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
        {
            sReduceLen--;
        }
        else
        {
            *sResult = ' ';
            sResult++;
        }
        
        sResultLen = aFormatInfo->mToCharResultLen;

        sResultLen = sResultLen - 2 /* blank , zero  */;
        while( sResultLen > 0 )
        {
            if( sZeroFormatFlag == STL_FALSE )
            {
                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                {
                    sReduceLen--;
                }
                else
                {
                    *sResult = ' ';
                    sResult++;
                }
            }
            else
            {
                *sResult = '0';
                sResult++;
            }
            sResultLen--;
        }
       
        *sResult = '0';
        sResult++;
                      
        STL_THROW( RAMP_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }


     /**
     * 숫자 표기 ( 0 이 아닌 경우 )
     */

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent < sIntDigitCount )
        {
            /* XXX.XXX */
            sInteger   = sInteger / gPreDefinedPow[ sExponent - 1 ];
            sTmpValue  = sInteger % 10;
            sInteger  /= 10;
            if( sTmpValue > 4 )
            {
                sInteger++;
            }
            else
            {
                /* do nothing */
            }

            sExponent = 0;
        }
        else if( sExponent == sIntDigitCount )
        {

            /* 0.XXXXXX */

            /* X format 은
             * 소수점을 무조건 round 처리 한다. */
            sInteger = sInteger / gPreDefinedPow[ sExponent -1 ];
            if( sInteger > 4 )
            {
                sInteger  = 1;
                sExponent = 0;
            }
            else
            {
                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                {
                    sReduceLen--;
                }
                else
                {
                    *sResult = ' ';
                    sResult++;
                }

                sResultLen = aFormatInfo->mToCharResultLen;

                sResultLen = sResultLen - 2 /* blank , zero  */;
                while( sResultLen > 0 )
                {
                    if( sZeroFormatFlag == STL_FALSE )
                    {
                        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                        {
                            sReduceLen--;
                        }
                        else
                        {
                            *sResult = ' ';
                            sResult++;
                        }
                    }
                    else
                    {
                        *sResult = '0';
                        sResult++;
                    }
                    sResultLen--;
                }
       
                *sResult = '0';
                sResult++;
                      
                STL_THROW( RAMP_SUCCESS );
            }
        }
        else
        {
            /* .0XXX */

            /* equal to zero value */
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
            {
                sReduceLen--;
            }
            else
            {
                *sResult = ' ';
                sResult++;
            }
            
            sResultLen = aFormatInfo->mToCharResultLen;

            sResultLen = sResultLen - 2 /* blank , zero  */;
            while( sResultLen > 0 )
            {
                if( sZeroFormatFlag == STL_FALSE )
                {
                    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                    {
                        sReduceLen--;
                    }
                    else
                    {
                        *sResult = ' ';
                        sResult++;
                    }
                }
                else
                {
                    *sResult = '0';
                    sResult++;
                }
                sResultLen--;
            }
       
            *sResult = '0';
            sResult++;
                      
            STL_THROW( RAMP_SUCCESS );
            sInteger  = 0;
            sExponent = 0;
        }
    }
    else
    {
        /* XXX000 */

        /* do nothing */
    }

    /**
     * Digit를 stlUInt64형태로 변환
     */

    /* Result Value 초기화 */
    sResultValue[0] = (stlUInt64)0;
    sResultValue[1] = (stlUInt64)0;
    sResultValue[2] = (stlUInt64)0;
    sResultValue[3] = (stlUInt64)0;

    sUInt64Value = (stlUInt64)sInteger;

    STL_TRY_THROW( _dtfAddUInt64ToUInt256( sResultValue ,sUInt64Value  ) == STL_TRUE,
                   RAMP_CANNOT_DISPLAY );

    while( sExponent > 0 )
    {        
        sTmpExponent = sExponent / 19 ;
        if( sTmpExponent == 0 )
        {
            sTmpExponent = sExponent % 19;
            STL_TRY_THROW( _dtfMulUInt64ToUInt256( sResultValue , gPreDefinedPow[ sTmpExponent ] ) == STL_TRUE,
                       RAMP_CANNOT_DISPLAY );

            sExponent -= sTmpExponent;
        }
        else
        {
            STL_TRY_THROW( _dtfMulUInt64ToUInt256( sResultValue , gPreDefinedPow[ 19 ] ) == STL_TRUE,
                       RAMP_CANNOT_DISPLAY );

            sExponent -= 19;
        }       
    }

#ifndef STL_IS_BIGENDIAN   /* This platform has little endian */
    /* Bigendian형태로 바꾸어 stlUInt8의 Array로 접근 가능하도록 한다. */
    for( i = 0; i < 4; i++ )
    {
        if( sResultValue[i] > (stlUInt64)0 )
        {
            sUInt64Value = sResultValue[i];
            ((stlUInt8*)(&sResultValue[i]))[0] = ((stlUInt8*)(&sUInt64Value))[7];
            ((stlUInt8*)(&sResultValue[i]))[1] = ((stlUInt8*)(&sUInt64Value))[6];
            ((stlUInt8*)(&sResultValue[i]))[2] = ((stlUInt8*)(&sUInt64Value))[5];
            ((stlUInt8*)(&sResultValue[i]))[3] = ((stlUInt8*)(&sUInt64Value))[4];
            ((stlUInt8*)(&sResultValue[i]))[4] = ((stlUInt8*)(&sUInt64Value))[3];
            ((stlUInt8*)(&sResultValue[i]))[5] = ((stlUInt8*)(&sUInt64Value))[2];
            ((stlUInt8*)(&sResultValue[i]))[6] = ((stlUInt8*)(&sUInt64Value))[1];
            ((stlUInt8*)(&sResultValue[i]))[7] = ((stlUInt8*)(&sUInt64Value))[0];
        }
    }
#endif

    /* 결과 Sting을 만들기 위해 변환할 Digit 및 변환된 String Size 계산 */
    sDigit = (stlUInt8*)sResultValue;
    sDigitCount = 64;
    while( *sDigit == (stlUInt8)0 )
    {
        sDigit++;
        sDigitCount -= 2;
    }

    if( *sDigit < (stlUInt8)0x10 )
    {
        sCarry = STL_TRUE;
        sDigitCount--;
    }
    else
    {
        sCarry = STL_FALSE;
    }
    

    /* 변환된 수가 Result Length에 포함되는지 판단 */
    STL_TRY_THROW( sDigitCount < aFormatInfo->mToCharResultLen,
                   RAMP_CANNOT_DISPLAY );

    /* String의 앞에 공백 추가 */
    sResult = aResult;
    
    /* '0' 길이 계산 */
    if( sZeroFormatFlag == STL_TRUE )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
        {
            sReduceLen--;
        }
        else
        {
            *sResult = ' ';
            sResult++;
        }
        
        sZeroLen = aFormatInfo->mToCharResultLen - sDigitCount -1;
        while( sZeroLen > 0 )
        {
            *sResult = '0';
            sResult++;
            sZeroLen--;
        }   
    }
    else
    {
        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
        {   
            sReduceLen -= (aFormatInfo->mToCharResultLen - sDigitCount);
        }
        else
        {
            stlMemset( sResult, ' ', STL_SIZEOF( stlChar ) * (aFormatInfo->mToCharResultLen - sDigitCount) );
            sResult += (aFormatInfo->mToCharResultLen - sDigitCount);
        }
    }   

    /* String의 실제 값 추가 */
    if( sCarry == STL_TRUE )
    {
        *sResult = dtfHexaToString[sLowerUpperIdx][*sDigit];
        sResult++;
        sDigit++;
        sDigitCount--;
    }

    while( sDigitCount > 0 )
    {
        *sResult = dtfHexaToString[sLowerUpperIdx][( (*sDigit) & (stlUInt8)0xF0 ) >> 4];
        sResult++;
        *sResult = dtfHexaToString[sLowerUpperIdx][(*sDigit) & (stlUInt8)0x0F];
        sResult++;
        sDigit++;
        sDigitCount -= 2;
    }

    STL_THROW( RAMP_SUCCESS );

    STL_RAMP( RAMP_CANNOT_DISPLAY );

    /* Result의 모든 자리에 #으로 표시한다. */
    stlMemset( aResult, '#', STL_SIZEOF( stlChar ) * aFormatInfo->mToCharResultLen );
    
    STL_RAMP( RAMP_SUCCESS );

    *aLength = aFormatInfo->mToCharResultLen + sReduceLen;

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    } 

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief native_real type을 EEEE format string에 맞게 string으로 변환한다.
 *
 * @param[in]  aDataValue          dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharRealToEEEEFormatString( dtlDataValue         * aDataValue,
                                           dtfNumberFormatInfo  * aFormatInfo,
                                           stlInt64               aAvailableSize,
                                           stlChar              * aResult,
                                           stlInt64             * aLength,
                                           dtlFuncVector        * aVectorFunc,
                                           void                 * aVectorArg,
                                           stlErrorStack        * aErrorStack )
{
    dtlRealType     sValue;
    stlFloat64      sPow;
    stlBool         sIsNegative              = STL_FALSE;
    stlInt64        sInteger                 = 0;
    stlBool         sIsNegativeExp           = STL_FALSE;
    stlInt32        sExponent                = 0;
    stlInt64        sTotDigitCount           = 0;
    stlInt64        sIntDigitCount           = 0;
    stlUInt8        sCarry                   = 0;
    stlInt32        i                        = 0;
    stlInt32        sTmpValue                = 0;
    stlInt32        sRemainder               = 0;
    stlInt32        sZeroLen                 = 0;
    stlInt32        sOrgDigitCntAfterDecimal = 0;
    stlInt32        sDigitCntAfterDecimal    = 0;
    stlInt32        sDigitCntBeforeDecimal   = 0;
    stlInt32        sResultLen               = aFormatInfo->mToCharResultLen;
    stlInt32        sReduceLen               = 0;
    stlInt32        sRoundDigit              = 0;
    stlInt32        sDisplayExponent         = 0;
    stlUInt8        sLen                     = 0;
    stlChar       * sResult                  = aResult;
    stlChar       * sTmpStr                  = NULL;
    stlChar       * sFormatStrEnd            = aFormatInfo->mStr + aFormatInfo->mStrLen -1;
    stlBool         sPlusOneDigitForRound    = STL_FALSE;
    stlBool         sNumberIsZero            = STL_FALSE;
    stlBool         sIsPrintZero             = STL_FALSE;

    sValue = *((dtlRealType*) aDataValue->mValue);

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        STL_TRY_THROW( aAvailableSize >= DTL_BINARY_REAL_INFINITY_SIZE, ERROR_NOT_ENOUGH_BUFFER );
        stlMemcpy( (stlChar*)aResult, DTL_BINARY_REAL_INFINITY, DTL_BINARY_REAL_INFINITY_SIZE );
        *aLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }


    if( sValue == 0.0 )
    {
        sNumberIsZero            = STL_TRUE;
        sIsNegative              = STL_FALSE;
        sDisplayExponent         = 0;
        sDigitCntAfterDecimal = aFormatInfo->mDigitCntAfterDecimal;
        if( ( (aFormatInfo->mDigitCntBeforeDecimal > 0) && (sDigitCntAfterDecimal == 0 ) ) ||
            ( aFormatInfo->mZeroStart > 0 ) )
        {
            /*
             * 예) TO_CHAR( 0, '9EEEE' ) => '  0E+00'
             *     TO_CHAR( 0, '0EEEE' ) => '  0E+00'
             *     TO_CHAR( 0, '0.999EEEE' ) => '  0.000E+00'
             */     
            sDigitCntBeforeDecimal = 1;
        }
        else
        {
            /* 예) TO_CHAR( 0, '9.999EEEE' ) => '   .000E+00' */
            sDigitCntBeforeDecimal = 0;
        }
        STL_THROW( RAMP_VALUE_IS_ZERO );
        
    }
    else
    {
        /* Do Nothing */
    }

    /* STL_DBL_DIG만큼의 유효숫자(trailing zero는 제거한 상태)와 
     * exponent의 형태로 만든다. */
    if( sValue < 0.0 )
    {
        sIsNegative = STL_TRUE;
        sValue      = -sValue;
    }
    else
    {
        sIsNegative = STL_FALSE;
    }

    sTotDigitCount = stlLog10f( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }


    sExponent = sTotDigitCount - STL_FLT_DIG;
    DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
    sInteger = (stlInt64)(sValue / sPow + 0.5);

    DTL_IS_VALID( stlGet10Pow( STL_FLT_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_FLT_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sIsNegative      sValue의 음수여부
     * sInteger         sValue의 STL_FLT_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값
     * */

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent <= sIntDigitCount )
        {
            sDisplayExponent = (sIntDigitCount == sExponent ? 1 : sIntDigitCount - sExponent - 1);
        }
        else
        {
            sDisplayExponent = sExponent - sIntDigitCount + 1;

        }
    }
    else
    {
        sDisplayExponent = sExponent + sIntDigitCount - 1;
    }

    sDigitCntAfterDecimal = aFormatInfo->mDigitCntAfterDecimal;
    sDigitCntBeforeDecimal = 1 + aFormatInfo->mDigitCntAfterV;
    sOrgDigitCntAfterDecimal = sIntDigitCount -1;

    

    /* 포맷에 의한 round 발생 */
    if( sOrgDigitCntAfterDecimal > ( sDigitCntAfterDecimal + aFormatInfo->mDigitCntAfterV ) )
    {
        i = sOrgDigitCntAfterDecimal - ( sDigitCntAfterDecimal + aFormatInfo->mDigitCntAfterV );
        if( i > 1 ) /* 1 자리 이상 차이 나는 경우 */
        {
            sTmpValue = i > STL_FLT_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
        }
        else 
        {
            sTmpValue = sInteger;
        }
                
        sRemainder = sTmpValue % 10;

        if( sRemainder > 4 )
        {
            sCarry = 1;
        }
        else
        {
            sCarry = 0;
        }               

        sTmpValue /= 10; 
        while( ( sCarry == 1 ) && ( sTmpValue > 9 ) )
        {
            if( ( sTmpValue % 10 ) == 9 )
            {
                sTmpValue /= 10;
                sCarry = 1;
            }
            else
            {
                sCarry = 0;
            }
        }

        /*
         * 정수가 한자리 늘어나는 경우
         * ex) 9999 =  9.999E...
         *  -> 2000 = 10.000E...
         */
        if( ( sCarry == 1 ) && ( sTmpValue == 9 ) )
        {          
            sPlusOneDigitForRound  = STL_TRUE;
        }
        else
        {
            sPlusOneDigitForRound = STL_FALSE;
        }
    }

    STL_RAMP( RAMP_VALUE_IS_ZERO );

    /* B Format */
    if( sNumberIsZero == STL_TRUE )    
    {
        if( DTF_NUMBER_FMT_FLAG_IS_BLANK( aFormatInfo ) == STL_TRUE )
        {
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                /* 예) TO_CHAR( 0, 'B9EEEE' ) */
                stlMemset( sResult, ' ', sResultLen );
            }
            else
            {
                sReduceLen = -aFormatInfo->mToCharResultLen;
            }

            *aLength = aFormatInfo->mToCharResultLen + sReduceLen;
            STL_THROW( RAMP_END );
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

    /*
     * 공백 & 부호 표기 & 0인 숫자에 대한 '0' 표기
     */
    if( aFormatInfo->mDigitCntAfterV == 0 )
    {
        if( ( sNumberIsZero == STL_TRUE ) && ( sDigitCntBeforeDecimal == 0 ) )
        {
            /*
             * 예) to_char( 0, '9.999999EEEE' )  =>  [][][][].000000E+00
             */

            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                *sResult = ' ';
                sResult++;
            }
            else
            {
                sReduceLen -= 1;
            }
        }
        else
        {
            /*
             * 예) to_char( 0, '0.999999EEEE' )  =>  [][][]0.000000E+00
             */
            
            /* Do Nothing */
        }
    }
    else
    {
        
        if( ( sNumberIsZero == STL_TRUE ) && ( aFormatInfo->mZeroStart == 0 ) )
        {
            /*
             * 예) to_char( 0, '9v9eeee' )  =>  [][][]0E+00
             */

            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                *sResult = ' ';
                sResult++;
            }
            else
            {
                sReduceLen -= 1;
            }
        }
        else
        {
            /*
             * 예) to_char( 0, '0v9eeee' )  =>  [][]00E+00
             */
            
            /* Do Nothing */
        }
    }

    /* 공백 */

    /* DisplayExponent */
    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent >= sIntDigitCount )
        {
            sDisplayExponent = (-1) * sDisplayExponent;
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* Do Nothing */
    }

    if( sPlusOneDigitForRound == STL_TRUE )
    {
        sInteger++;
        sOrgDigitCntAfterDecimal++;
        sDisplayExponent++;

    }
    else
    {
        /* do nothing */
    }

    if( ( sDisplayExponent > -100 ) && ( sDisplayExponent < 100 ) )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
        {
            *sResult = ' ';
            sResult++;
        }
        else
        {
            sReduceLen -= 1;
        }
    }
    else
    {
        /* Do Nothing */
    }

    sTmpStr = sFormatStrEnd;


    /**
     * 'EEEE, S, MI, PR' skip
     */
    while( STL_TRUE )
    {
        if( (*sTmpStr == 'E') || (*sTmpStr == 'e' ) )
        {
            sTmpStr -= 4;
            continue;
        }

        if( (*sTmpStr == 'S') || (*sTmpStr == 's') )
        {
            sTmpStr -= 1;
            continue;
        }

        if( (*sTmpStr == 'I') || (*sTmpStr == 'i') )
        {
            sTmpStr -= 2;
            continue;
        }

        if( (*sTmpStr == 'R') || (*sTmpStr == 'r') )
        {
            sTmpStr -= 2;
            continue;
        }

        break;
    }

    /**
     * 부호 표기 ( +, -, PR, MI )
     */
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
    {
        dtfNumberFmtSetSignForNormal( aFormatInfo, aResult, sResultLen, !sIsNegative, sResult, &sResult );
    }
    else
    {
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  sResultLen,
                                  !sIsNegative,
                                  STL_TRUE, // aIsSignLocatePre
                                  &sResultLen,
                                  sResult,
                                  &sResult );            
    }

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_BRACKET( aFormatInfo ) == STL_TRUE )
        {
            sReduceLen -= (sIsNegative == STL_TRUE ? 0 : 2);
        }
        else if( DTF_NUMBER_FMT_FLAG_IS_SIGN( aFormatInfo ) == STL_FALSE )
        {
            sReduceLen -= (sIsNegative == STL_TRUE ? 0 : 1);
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* do nothing */
    }


    /**
     * DOLLAR 표기
     */

    DTF_NUMBER_FMT_SET_DOLLAR( aFormatInfo, sResult );


    

    
    /**
     * 숫자 표기 ( 0 인 경우 )
     */
    if( sNumberIsZero == STL_TRUE )
    {
        if( aFormatInfo->mDigitCntAfterV == 0 )
        {
            if( sDigitCntBeforeDecimal >= 1 )
            {
                *sResult = '0';
                sResult++;
            }
            else 
            {
                if( (aFormatInfo->mDigitCntBeforeDecimal > 0) &&
                    (aFormatInfo->mZeroEndAfterDecimal == 0 ) &&
                    (DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE) )
                {
                    *sResult = '0';
                    sResult++;
                    sReduceLen++;
                }
                /* do nothing */
            }

        }
        else
        {
            if( aFormatInfo->mZeroStart == 0 )
            {
                sZeroLen = aFormatInfo->mDigitCntAfterV;
            }
            else
            {
                sZeroLen = 1 + aFormatInfo->mDigitCntAfterV;
            }

            while( sZeroLen > 0 )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }
        
        if( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aFormatInfo ) == STL_TRUE )
        {
            *sResult = '.';
            sResult++;
        }
        else
        {
            /* Do Nothing */
        }

        sZeroLen = sDigitCntAfterDecimal;
        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
        {
            sZeroLen = aFormatInfo->mZeroEndAfterDecimal;
            sReduceLen -= ( sDigitCntAfterDecimal - sZeroLen );

            while( sZeroLen > 0 )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }
        else
        {
            /* 정방향 출력 */
            while( sZeroLen > 0 )
            {
                *sResult = '0';
                sResult++;
                sZeroLen--;
            }
        }

        dtfNumberFmtSetEEEEExp( aFormatInfo, sDisplayExponent, sResult, &sLen );

        STL_THROW( RAMP_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }
  

     /**
     * 숫자 표기 ( 0 이 아닌 경우 )
     */
    
    sRoundDigit = sDigitCntBeforeDecimal + sDigitCntAfterDecimal;
        
    /*  round 가 발생하는 경우
     *  round 되는 값을 미리 keep */
    if( sOrgDigitCntAfterDecimal > ( sDigitCntAfterDecimal + aFormatInfo->mDigitCntAfterV ) )
    {
        i = sOrgDigitCntAfterDecimal - ( sDigitCntAfterDecimal + aFormatInfo->mDigitCntAfterV );
        if( i > 1 ) /* 1 자리 이상 차이 나는 경우 */
        {
            sInteger = i > STL_DBL_DIG ? 0 : sInteger / gPreDefinedPow[ i -1 ];
        }
        else 
        {
            /* do nothing */
        }

        sRemainder = sInteger % 10;
        sInteger   = sInteger / 10;
                

        if( sRemainder > 4 )
        {
            sCarry = 1;
        }
        else
        {
            sCarry = 0;
        }
    }
    else
    {
        sCarry = 0;
    }

    sResult--;
    sResult   += sRoundDigit +
                 ( sDigitCntAfterDecimal > 0 ? 1 :
                 ( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aFormatInfo ) == STL_TRUE ? 1 : 0 ) ) /* decimal point */;
    sExponent  = sDigitCntAfterDecimal;

    /**
     * V format trailing zero
     */
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        sZeroLen = aFormatInfo->mDigitCntAfterV - sExponent - sOrgDigitCntAfterDecimal;
    }
    else
    {
        sZeroLen = aFormatInfo->mDigitCntAfterV + sExponent - sOrgDigitCntAfterDecimal;
    }
    while( sZeroLen > 0 )
    {
        *sResult = '0';
        sExponent--;
        sResult--;
        sZeroLen--;
    }

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        if( *sTmpStr == '0' )
        {
            sIsPrintZero = STL_TRUE;
        }
        else
        {
            /* do nothing */
        }

        /**
         * not V format trailing zero
         */
        sZeroLen = sDigitCntAfterDecimal - sOrgDigitCntAfterDecimal;

        
        /**
         * FM format : '.' 이후 제로는 '0' 이전에 '9' 가 있더라도
         * 계속 제로를 출력 한다.
         */
        while( sZeroLen > 0 )
        {
            if( (sIsPrintZero == STL_TRUE ) || (*sTmpStr == '0') )
            {
                *sResult = '0';
            }
            else
            {
                sReduceLen--;
            }

            sExponent--;
            sResult--;
            sZeroLen--;
                
            sTmpStr--;
                
        }
        
        /* 소수점 digit 출력 */
        while( ( sExponent > 0) && (sIntDigitCount > 0 ) )
        {
            sTmpValue = sInteger % 10;

            sIsPrintZero = sTmpValue > 0 ? STL_TRUE : sIsPrintZero;

            if( (sIsPrintZero == STL_TRUE ) || (*sTmpStr == '0') )
            {
                *sResult = sTmpValue + '0';
            }
            else
            {
                sReduceLen--;
            }

            sInteger = sInteger / 10;

            sExponent--;
            sResult--;
            sRoundDigit--;
            sIntDigitCount--;
        
        }
    }
    else
    {
        /* 소수점 digit 출력 */
        while( ( sExponent > 0) && (sIntDigitCount > 0 ) )
        {
            sTmpValue = sInteger % 10;
            *sResult = sTmpValue + '0';

            sInteger = sInteger / 10;

            sExponent--;
            sResult--;
            sRoundDigit--;
            sIntDigitCount--;
        
        }
    }

    if( (sDigitCntAfterDecimal > 0) || (DTF_NUMBER_FMT_FLAG_IS_PERIOD( aFormatInfo ) == STL_TRUE) )
    {
        *sResult = '.';
        sResult--;
    }
    else
    {
        /* Do Nothing */
    }

    /* 정수부 출력 */
    while( ( sRoundDigit > 0 ) && ( sIntDigitCount > 0 ) )
    {
        sTmpValue = sInteger % 10;
        *sResult = sTmpValue + '0';               

        sInteger = sInteger / 10;
        sRoundDigit--;
        sIntDigitCount--;
        sResult--;
    }

    if( sCarry == 1 )
    {
        sRoundDigit = 1 + sDigitCntAfterDecimal + aFormatInfo->mDigitCntAfterV +
                      ( sDigitCntAfterDecimal > 0 ? 1: 0 ); /* decimal point */
            
        sResult += sRoundDigit;

        while( ( sCarry == 1 ) && ( sRoundDigit > 0 ) )
        {                   
            *sResult += sCarry;

            if( *sResult > '9' )
            {
                *sResult = '0';
                sCarry = 1;
            }
            else
            {
                sCarry = 0;
            }

            sRoundDigit--;
            sResult--;
        }
    }
    else
    {
        /* Do Nothing */
    }


    
    sRoundDigit = ( aFormatInfo->mSignLocate == DTF_NUMBER_SIGN_LOCATE_POST ? 0 : 1 ) +
        1 + sDigitCntAfterDecimal +
        aFormatInfo->mDigitCntAfterV +
        ( DTF_NUMBER_FMT_FLAG_IS_DOLLAR( aFormatInfo ) == STL_TRUE ? 1 : 0 ) +
        ( sDigitCntAfterDecimal > 0 ? 1 :
          ( DTF_NUMBER_FMT_FLAG_IS_PERIOD( aFormatInfo ) == STL_TRUE ? 1 : 0 ) ) /* decimal point */ +
        ( sDisplayExponent > -100  ? ( sDisplayExponent < 100 ? 1 : 0 ) : 0 );

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        sRoundDigit += sReduceLen;
        if( ((DTF_NUMBER_FMT_FLAG_IS_MINUS( aFormatInfo ) == STL_TRUE) ||
             (DTF_NUMBER_FMT_FLAG_IS_BRACKET( aFormatInfo ) == STL_TRUE) )&&
            (sIsNegative == STL_FALSE ) )
        {
            sRoundDigit++;
        }
        
    
    }
    else
    {
        /* do nothing */
    }
                  
    sResult = aResult + sRoundDigit;

    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        /**
         * Carry 에 의해 생성된 '0' 을 제거
         */
        sResult--;
        sZeroLen = sDigitCntAfterDecimal;

        while( sZeroLen > aFormatInfo->mZeroEndAfterDecimal )
        {
            if( *sResult == '0' )
            {
                sResult--;
                sReduceLen--;
            }

            sZeroLen--;
        }
        sResult++;
        
    }
    else
    {
        /* do nothing */
    }
    
    dtfNumberFmtSetEEEEExp( aFormatInfo, sDisplayExponent, sResult, &sLen );

    STL_RAMP( RAMP_SUCCESS );
    
    *aLength = aFormatInfo->mToCharResultLen + sReduceLen;

    /**
     * FM format 시 decimal point 이후 '9' format 에 해당하는
     * '0' 문자의 위치를 선계산 할 수 없으므로
     * 문자열 변환이 끝나고 'S,MI,PR' 등을 재 출력 한다.
     */
    if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
    {
        sResult = aResult + (*aLength -1 < 0 ? 0 : *aLength -1 );
        dtfNumberFmtSetSignForFM( aFormatInfo,
                                  aResult,
                                  *aLength,
                                  !sIsNegative,
                                  STL_FALSE,
                                  &sResultLen,
                                  sResult,
                                  &sResult );

    }
    else
    {
        /* do nothing */
    }

    STL_RAMP( RAMP_END );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    } 

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief native_real type을 RN, rn format string에 맞게 string으로 변환한다.
 * @param[in]  aDataValue          dtlDataValue
 * @param[in]  aFormatInfo         dtfNumberFormatInfo
 * @param[in]  aAvailableSize      aAvailableSize
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfToCharRealToRNFormatString( dtlDataValue         * aDataValue,
                                         dtfNumberFormatInfo  * aFormatInfo,
                                         stlInt64               aAvailableSize,
                                         stlChar              * aResult,
                                         stlInt64             * aLength,
                                         dtlFuncVector        * aVectorFunc,
                                         void                 * aVectorArg,
                                         stlErrorStack        * aErrorStack )
{

    dtlRealType        sValue;
    stlFloat64         sPow;
    stlInt64           sInteger;
    stlBool            sIsNegativeExp;
    stlInt32           sExponent;
    stlInt64           sTotDigitCount;
    stlInt64           sIntDigitCount;
    stlUInt8           sCarry                         = 0;
    stlInt32           sTmpValue                      = 0;
    stlChar          * sResult                        = aResult;
    stlInt32           sLeftSpace                     = 0;
    stlInt32           sIndex                         = 0;
    stlInt32           sReduceLen                     = 0;
    stlInt32           sLength                        = 0;
    const stlInt32     RESULT_STRING_SIZE             = DTF_NUMBER_TO_CHAR_RESULT_ROMAN_NUMERAL_STRING_SIZE;
    stlInt32           sLowerUpperIdx                 = aFormatInfo->mLowerUpperIdx;
    const stlInt64     ROMAN_NUMERAL_MAX_VALUE        = 3999;

    sValue = *((dtlRealType*) aDataValue->mValue);

    if( stlIsfinite(sValue) == STL_IS_FINITE_FALSE )
    {
        STL_TRY_THROW( aAvailableSize >= DTL_BINARY_REAL_INFINITY_SIZE, ERROR_NOT_ENOUGH_BUFFER );
        stlMemcpy( (stlChar*)aResult, DTL_BINARY_REAL_INFINITY, DTL_BINARY_REAL_INFINITY_SIZE );
        *aLength = DTL_BINARY_REAL_INFINITY_SIZE;
        STL_THROW( RAMP_SUCCESS );
    }

    if( sValue == 0 )
    {
        /* zero 는 로마숫자로 표현하지 않음 */
        STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
    }
    else
    {
        /* Do Nothing */
    }

    /* STL_DBL_DIG만큼의 유효숫자(trailing zero는 제거한 상태)와 
     * exponent의 형태로 만든다. */
    if( sValue < 0.0 )
    {
        /* 음수는 로마숫자로 표현하지 않는다. */
        STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
    }
    else
    {
        /* do nothing */
    }

    sTotDigitCount = stlLog10f( sValue );
    if( sValue < 1 )
    {
        DTL_IS_VALID( stlGet10Pow( sTotDigitCount, &sPow, aErrorStack ) );
        if( sValue == sPow )
        {
            sTotDigitCount++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sTotDigitCount++;
    }

    sExponent = sTotDigitCount - STL_FLT_DIG;
    DTL_IS_VALID( stlGet10Pow( sExponent, &sPow, aErrorStack ) );
    sInteger = (stlInt64)(sValue / sPow + 0.5);

    DTL_IS_VALID( stlGet10Pow( STL_FLT_DIG - 1, &sPow, aErrorStack ) );
    while( sInteger < (stlInt64)sPow )
    {
        sInteger *= 10;
        sExponent--;
    }

    sIntDigitCount = STL_FLT_DIG;
    while( sInteger % 10 == 0 )
    {
        sExponent++;
        sIntDigitCount--;
        sInteger /= 10;
    }

    if( sIntDigitCount == 0 )
    {
        /* 반올림으로 인해 자리수가 증가한 경우 */
        sIntDigitCount = 1;
    }
    else
    {
        /* Do Nothing */
    }

    if( sExponent < 0 )
    {
        sIsNegativeExp = STL_TRUE;
        sExponent = -sExponent;
    }
    else
    {
        sIsNegativeExp = STL_FALSE;
    }

    /* 지금까지 처리된 결과는 아래와 같다.
     * sInteger         sValue의 STL_DBL_DIG만큼의 유효숫자에서 
     *                  trailing zero를 제거한 수
     * sIntDigitCount   sInteger의 digit count
     * sIsNegativeExp   exponent의 음수여부
     * sExponent        exponent의 값
     * */
  
    sLeftSpace  = RESULT_STRING_SIZE;

    if( sIsNegativeExp == STL_TRUE )
    {
        if( sExponent > sIntDigitCount )
        {
            /* .000XXX */
            STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
        }
        else if( sExponent == sIntDigitCount )
        {
            /* .XXXXXX
             * 소수점 첫째자리만 고려한다. */
            sInteger = sInteger / gPreDefinedPow[ sExponent -1 ];
            if( sInteger < 5 )
            {
                /* 0.1, 0.2, 0.3, 0.4 */
                STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
            }
            else
            {
                /* 0.5, 0.6, 0.7, 0.8, 0.9 */
                if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_TRUE )
                {
                    sReduceLen   = RESULT_STRING_SIZE - 1;
                    aResult[ 0 ] = dtfRomanNumeralStringList[ sLowerUpperIdx ][ 0 ][ 1 ].mStr[0];
                }
                else
                {
                    stlMemset( sResult,
                               ' ',
                               RESULT_STRING_SIZE - 1 );
                    aResult[ 14 ] = dtfRomanNumeralStringList[ sLowerUpperIdx ][ 0 ][ 1 ].mStr[0];
                }
            }
        }
        else
        {
            /* 소수점 반올림 */
            while( sExponent > 0 )
            {
                sTmpValue = sInteger % 10;
                if( sTmpValue > 4 )
                {
                    sCarry = 1;
                }
                else
                {
                    sCarry = 0;
                }
                 
                sInteger = sInteger / 10;
                sInteger += sCarry;

                sExponent--;
            }

            /* 최대값 예외 처리 */
            if( sInteger > ROMAN_NUMERAL_MAX_VALUE )
            {
                STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
            }
            else
            {
                /* do nothing */
            }
            

            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                sIndex = 0;
                while( sInteger > 0 )
                {
                    sTmpValue = sInteger % 10;
                    sInteger = sInteger / 10;
                                    
                    /* copy roman numeral */
                    sLength = dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStrLen;
                    stlMemcpy(
                        sResult + ( sLeftSpace - sLength ),
                        dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStr ,
                        sLength );

                    sLeftSpace -= sLength;

                    sIndex++;
                }
                        
                stlMemset( sResult,
                           ' ',
                           sLeftSpace );
            }
            else
            {
                sLength = 0;
                

                if( (sInteger / gPreDefinedPow[ sTotDigitCount ]) > 0 )
                {
                    sTotDigitCount++;
                    sIndex = sTotDigitCount -1;
                }
                else
                {
                    sIndex = sTotDigitCount -1;
                }
                while( sTotDigitCount > 0 )
                {
                    if( (sTotDigitCount - 1) > 0 )
                    {
                        sTmpValue = sInteger / gPreDefinedPow[ sTotDigitCount -1 ];
                        sInteger  = sInteger % gPreDefinedPow[ sTotDigitCount -1 ];
                    }
                    else
                    {
                        sTmpValue = sInteger;
                        sInteger  = 0;
                    }

                    if( sTmpValue == 0 )
                    {
                        sIndex--;
                        sTotDigitCount--;
                        continue;
                    }

                    /* copy roman numeral */
                    stlMemcpy(
                        sResult + sLength,
                        dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStr ,
                        dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStrLen );

                    sLength += dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStrLen;
                 
                    sIndex--;
                    sTotDigitCount--;
                }
                sReduceLen = RESULT_STRING_SIZE - sLength;
            }
        }
    }
    else
    {
        /* XXXXXX000 */

        /* 4자리 이상의 수  */
        if( sExponent + 1 > 4 )
        {
            STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
        }
        else
        {
            /* do nothing */
        }
         
        sIndex = 0;
        if( sExponent > 0 )
        {
            sInteger *= gPreDefinedPow[ sExponent ];
        }
        else
        {
            /* do nothing */
        }

        /* 최대값 예외 처리 */
        if( sInteger > ROMAN_NUMERAL_MAX_VALUE )
        {
            STL_THROW( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
        }
        else
        {
            /* do nothing */
        }

        if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
        {
            sIndex = 0;
            while( sInteger > 0 )
            {
                sTmpValue = sInteger % 10;
                sInteger = sInteger / 10;
                                    
                /* copy roman numeral */
                sLength = dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStrLen;
                stlMemcpy(
                    sResult + ( sLeftSpace - sLength ),
                    dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStr ,
                    sLength );

                sLeftSpace -= sLength;

                sIndex++;
            }
                        
            stlMemset( sResult,
                       ' ',
                       sLeftSpace );
        }
        else
        {
            sLength = 0;
            sIndex = sTotDigitCount -1;
            while( sTotDigitCount > 0 )
            {
                if( (sTotDigitCount - 1) > 0 )
                {
                    sTmpValue = sInteger / gPreDefinedPow[ sTotDigitCount -1 ];
                    sInteger  = sInteger % gPreDefinedPow[ sTotDigitCount -1 ];
                }
                else
                {
                    sTmpValue = sInteger;
                    sInteger  = 0;
                }

                if( sTmpValue == 0 )
                {
                    sIndex--;
                    sTotDigitCount--;
                    continue;
                }

                /* copy roman numeral */
                stlMemcpy(
                    sResult + sLength,
                    dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStr ,
                    dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStrLen );

                sLength += dtfRomanNumeralStringList[ sLowerUpperIdx ][ sIndex ][ sTmpValue ].mStrLen;

                sIndex--;
                sTotDigitCount--;
            }
            sReduceLen = RESULT_STRING_SIZE - sLength;
        }
    }  

    STL_THROW( RAMP_SUCCESS );

    STL_RAMP( RAMP_NOT_RANGE_OF_ROMAN_NUMERALS );
    
    stlMemset( sResult , '#' , RESULT_STRING_SIZE );

    STL_RAMP( RAMP_SUCCESS );

    *aLength = RESULT_STRING_SIZE - sReduceLen;

    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_WHOLE_DIGITS_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    } 

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief format string 으로 formatNode (format info)를 구성한다.
 * @param[in]   aDataType                        dtlDataType
 * @param[in]   aFormatString                    format string
 * @param[in]   aFormatStringLen                 format string length
 * @param[in,out]  aFormatInfoBuffer             to_number to_native_real to_native_double format info를 위한 공간
 * @param[in]   aFormatInfoBufferSize            to_number to_native_real to_native_double format info를 위한 공간 사이즈
 * @param[in]   aVectorFunc                      aVectorArg
 * @param[in]   aVectorArg                       aVectorArg
 * @param[in]   aErrorStack                      에러 스택
 */
stlStatus dtfGetNumberFormatInfoForToNumber( dtlDataType      aDataType,
                                             stlChar        * aFormatString,
                                             stlInt64         aFormatStringLen,
                                             void           * aFormatInfoBuffer,
                                             stlInt32         aFormatInfoBufferSize,
                                             dtlFuncVector  * aVectorFunc,
                                             void           * aVectorArg,
                                             stlErrorStack  * aErrorStack )
{
    dtfNumberFormatInfo           * sFormatInfo       = NULL;
    const dtfNumberFormatKeyWord  * sKeyWord          = NULL;
    stlChar                       * sFormatString     = NULL;
    stlChar                       * sFormatStringEnd  = NULL;
    
    /**
     * format string 길이 검사 & 초기화 
     */
    
    /* format string의 길이 검사 */
    STL_TRY_THROW( aFormatStringLen <= DTF_NLS_NUMBER_FORMAT_STRING_MAX_SIZE,
                   ERROR_NUMBER_FORMAT_LENGTH );
    
    /**
     *  dtfNumberFormatInfo 공간 확보
     */
    
    STL_DASSERT( aFormatInfoBufferSize >= STL_SIZEOF( dtfNumberFormatInfo ) );
    
    sFormatInfo = (dtfNumberFormatInfo *)aFormatInfoBuffer;
    
    DTF_INIT_NUMBER_FORMAT_INFO( sFormatInfo );
    
    /**
     * format string 정보 복사
     */
    
    stlMemcpy( sFormatInfo->mStr, aFormatString, aFormatStringLen );
    sFormatInfo->mStrLen = aFormatStringLen;
    
    sFormatString     = sFormatInfo->mStr;
    sFormatStringEnd  = sFormatInfo->mStr + sFormatInfo->mStrLen;
    
    /**
     * format key word 를 찾아 정보를 구축한다.
     */
    
    while( sFormatString < sFormatStringEnd )
    {
        sKeyWord = dtfGetNumberFormatKeyWord( sFormatString );
        
        if( sKeyWord != NULL )
        {
            STL_TRY( dtfSetNumberFormatInfo( sFormatString,
                                             sKeyWord,
                                             sFormatInfo,
                                             aVectorFunc,
                                             aVectorArg,
                                             aErrorStack )
                     == STL_SUCCESS );
            
            sFormatString = sFormatString + sKeyWord->mLen;
        }
        else
        {
            STL_THROW( ERROR_INVALID_NUMBER_FORMAT );
        }
    }
    
    /**
     * RN , V  format element 는
     * to_number, to_native_real, to_native_double 에서 사용할 수 없다.
     */
    
    STL_TRY_THROW( ( DTF_NUMBER_FMT_FLAG_IS_RN( sFormatInfo ) == STL_FALSE ) &&
                   ( DTF_NUMBER_FMT_FLAG_IS_V( sFormatInfo ) == STL_FALSE ),
                   ERROR_INVALID_NUMBER_FORMAT );

    /**
     * 분석된 format info로 'B' format elemnt  to_char의 최종 result 결과 길이를 구한다.
     */
    
    if( DTF_NUMBER_FMT_FLAG_IS_BLANK( sFormatInfo ) == STL_TRUE )
    {
        if( DTF_NUMBER_FMT_FLAG_IS_EEEE( sFormatInfo ) == STL_TRUE )
        {
            DTF_SET_NUMBER_EEEE_TO_CHAR_RESULT_LEN( sFormatInfo );
        }
        else
        {
            DTF_SET_NUMBER_TO_CHAR_RESULT_LEN( sFormatInfo );
        }
    }
    else
    {
        /* Do Nothing */
    }

    if( DTF_NUMBER_FMT_FLAG_IS_X( sFormatInfo ) == STL_TRUE )
    {
        STL_TRY_THROW( aFormatStringLen < DTF_NLS_NUMBER_FORMAT_STRING_MAX_SIZE,
                       ERROR_NUMBER_X_FORMAT_FORMAT_LENGTH );
    }
    else
    {
        /* Do Nothing */
    }

    return STL_SUCCESS;

    STL_CATCH( ERROR_NUMBER_FORMAT_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMBER_FORMAT_LENGTH,
                      NULL,
                      aErrorStack,
                      DTF_NLS_NUMBER_FORMAT_STRING_MAX_SIZE );
    }

    STL_CATCH( ERROR_NUMBER_X_FORMAT_FORMAT_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_NUMBER_FORMAT_LENGTH,
                      NULL,
                      aErrorStack,
                      DTF_NLS_NUMBER_FORMAT_STRING_MAX_SIZE - 1 );
    }
    
    STL_CATCH( ERROR_INVALID_NUMBER_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_NUMBER_FORMAT_MODEL,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;
    
    return STL_FAILURE;    
}


/**
 * @brief to_number( str, fmt ), to_binary_real( str, fmt ), to_binary_double( str, fmt ) <BR>
 *        함수의 str 인자를 fmt에 맞추어 number literal에 맞는 string으로 변환한다.
 * @param[in]  aDataValue            dtlDataValue ( to_number( str, fmt )의 str 에 대한 datavalue )
 * @param[in]  aFormatInfo           dtfNumberFormatInfo
 * @param[out] aNumberLiteralBuf     변환된 결과 string
 * @param[out] aNumberLiteralBufLen  변환된 결과 string의 길이
 * @param[in]  aVectorFunc           aVectorArg
 * @param[in]  aVectorArg            aVectorArg
 * @param[in]  aErrorStack           에러 스택
 */
stlStatus dtfGetNumberLiteralFromString( dtlDataValue          * aDataValue,
                                         dtfNumberFormatInfo   * aFormatInfo,
                                         stlChar               * aNumberLiteralBuf,
                                         stlInt64              * aNumberLiteralBufLen,
                                         dtlFuncVector         * aVectorFunc,
                                         void                  * aVectorArg,
                                         stlErrorStack         * aErrorStack )
{
    stlChar   * sStr = (stlChar *)(aDataValue->mValue);
    stlChar   * sTmpStrStart = NULL;
    stlChar   * sTmpStrEnd   = NULL;
    stlChar   * sStrEnd  = sStr + aDataValue->mLength;
    
    stlChar   * sTmpFormatStr = aFormatInfo->mStr;
    /* stlChar   * sFormatStrEnd = aFormatInfo->mStr + aFormatInfo->mStrLen; */
    
    stlChar   * sNumStr = aNumberLiteralBuf;
    stlInt32    sNumStrLen = 0;
    stlUInt8    sDigitCntBeforeDecimal = 0;
    stlUInt8    sDigitCntAfterDecimal = 0;
    stlUInt8    sLeadingZeroCnt = 0;
    stlUInt8    sTotalDigitCntBeforeDecimal = 0;
    stlUInt8    sDigitCntApplyZeroFormat = ( aFormatInfo->mDigitCntBeforeDecimal - aFormatInfo->mZeroStart + 1 );
    
    stlUInt8    sCommaCnt = 0;
    stlUInt8    sFormatStrCommaCnt = 0;
    stlUInt8    sSkipFormatDigitCnt = 0;
    stlBool     sIsExistPeriod = STL_FALSE;

    stlInt32    i = 0;
    stlInt8     sNumIdx = 0;
    stlInt8     sNum = 0;
    stlUInt8    sHexaDigitLen = 0;
    stlUInt8    sByteLen = 0;
    stlUInt8  * sDigitPtr = NULL;
    stlUInt64   sResultValue[4];
    stlUInt64   sRemainder[4];
    stlUInt64   sUInt64Value = 0;
    stlChar     sNumberToStr[11] = "0123456789";    

    
    /*
     * SKIP LEADING SPACE 
     */
    
    DTF_SKIP_LEADING_SPACE( sStr, sStrEnd );


    /*
     * B Format element 검사
     */
    
    if( DTF_NUMBER_FMT_FLAG_IS_BLANK( aFormatInfo ) == STL_TRUE )
    {
        if( sStr == sStrEnd )
        {
            STL_TRY_THROW( aFormatInfo->mToCharResultLen == aDataValue->mLength,
                           ERROR_INVALID_INPUT_STRING );

            sNumStr[ 0 ] = '0';
            sNumStrLen = 1;
            
            STL_THROW( RAMP_CHECK_STR );
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
    
    STL_TRY_THROW( ( sStrEnd - sStr ) <= DTF_NUMBER_TO_CHAR_RESULT_STRING_MAX_SIZE,
                   ERROR_INVALID_INPUT_STRING );
    
    
    /*
     * X Format element 검사
     */
    
    if( DTF_NUMBER_FMT_FLAG_IS_X( aFormatInfo ) == STL_TRUE )
    {
        STL_TRY_THROW( ( sStrEnd - sStr ) <= DTF_NUMBER_MAX_DISPLAY_COUNT_FOR_X_FORMAT,
                       ERROR_INVALID_INPUT_STRING );
        
        sResultValue[0] = (stlUInt64)0;
        sResultValue[1] = (stlUInt64)0;
        sResultValue[2] = (stlUInt64)0;
        sResultValue[3] = (stlUInt64)0;

        sRemainder[0] = (stlUInt64)0;
        sRemainder[1] = (stlUInt64)0;
        sRemainder[2] = (stlUInt64)0;
        sRemainder[3] = (stlUInt64)0;
        
        if( aFormatInfo->mDigitCntBeforeDecimal > 0 )
        {
            STL_TRY_THROW( (sStrEnd - sStr) ==
                           (aFormatInfo->mDigitCntBeforeDecimal + aFormatInfo->mXCnt),
                           ERROR_INVALID_INPUT_STRING );
        }
        else
        {
            STL_TRY_THROW( (sStrEnd - sStr) <= aFormatInfo->mXCnt,
                           ERROR_INVALID_INPUT_STRING );
        }
        
        DTF_SKIP_LEADING_ZERO_CHARACTERS( sStr, sStrEnd, sLeadingZeroCnt );
        
        sHexaDigitLen = sStrEnd - sStr;
        
        if( ( sHexaDigitLen == 0 ) && ( sLeadingZeroCnt > 0 ) )
        {
            sNumStr[ sNumStrLen ] = '0';
            sNumStrLen = 1;
            
            STL_THROW( RAMP_CHECK_STR );
        }
        else
        {
            /* Do Nothing */
        }

        /*
         * hexa digit string으로 십진수를 만든다.
         * 
         * Bigendian이라 가정하고 stlUInt8의 Array로 접근한다.
         */

        if( ( sHexaDigitLen - (sHexaDigitLen / 2 * 2) ) == 0 )
        {
            sByteLen = ( sHexaDigitLen / 2 );
            sDigitPtr = ((stlUInt8*)(sResultValue)) + ( 32 - sByteLen );
        }
        else
        {
            /* hexa digit 갯수가 홀수개로 맨 처음 hexa digit 하나를 먼저 처리한다. */
            
            sByteLen = ( sHexaDigitLen / 2 ) + 1;
            sDigitPtr = ((stlUInt8*)(sResultValue)) + ( 32 - sByteLen );
            
            sNum = dtdHexLookup[ (stlUInt8)(*sStr) ];
            STL_TRY_THROW( sNum != -1, ERROR_INVALID_INPUT_STRING );
            
            *sDigitPtr = (stlUInt8)sNum;
            
            sStr++;
            sDigitPtr++;
        }
        
        while( sStr < sStrEnd )
        {
            sNum = dtdHexLookup[ (stlUInt8)(*sStr) ];
            STL_TRY_THROW( sNum != -1, ERROR_INVALID_INPUT_STRING );
            
            *sDigitPtr = (stlUInt8)sNum << 4;
            
            sStr++;
            
            sNum = dtdHexLookup[ (stlUInt8)(*sStr) ];
            STL_TRY_THROW( sNum != -1, ERROR_INVALID_INPUT_STRING );
            
            *sDigitPtr += (stlUInt8)sNum;
            
            sStr++;
            
            sDigitPtr++;
            STL_DASSERT( sDigitPtr <= ((stlUInt8*)(sResultValue)) + 32 );
        }

#ifndef STL_IS_BIGENDIAN   /* This platform has little endian */
        /* 윗 부분에서 Hexa string을 Bigendian형태라 가정하고 처리했으므로,
         * 플랫폼에 맞게 숫자를 재배열한다.
         */
        for( i = 0; i < 4; i++ )
        {
            if( sResultValue[i] > (stlUInt64)0 )
            {
                sUInt64Value = sResultValue[i];
                ((stlUInt8*)(&sResultValue[i]))[0] = ((stlUInt8*)(&sUInt64Value))[7];
                ((stlUInt8*)(&sResultValue[i]))[1] = ((stlUInt8*)(&sUInt64Value))[6];
                ((stlUInt8*)(&sResultValue[i]))[2] = ((stlUInt8*)(&sUInt64Value))[5];
                ((stlUInt8*)(&sResultValue[i]))[3] = ((stlUInt8*)(&sUInt64Value))[4];
                ((stlUInt8*)(&sResultValue[i]))[4] = ((stlUInt8*)(&sUInt64Value))[3];
                ((stlUInt8*)(&sResultValue[i]))[5] = ((stlUInt8*)(&sUInt64Value))[2];
                ((stlUInt8*)(&sResultValue[i]))[6] = ((stlUInt8*)(&sUInt64Value))[1];
                ((stlUInt8*)(&sResultValue[i]))[7] = ((stlUInt8*)(&sUInt64Value))[0];
            }
        }
#endif
        
        /*
         * 십진수를 string으로 만든다.
         */

        sNumIdx = 3;
        
        if( sByteLen > 8 )
        {
            while( sNumIdx >= 0 )
            {
                STL_TRY_THROW( _dtfDivisionUInt256By64( sResultValue,
                                                        gPreDefinedPow[19],
                                                        sResultValue,
                                                        & sRemainder[sNumIdx] )
                               == STL_TRUE,
                               ERROR_INVALID_INPUT_STRING );
                
                if( ( sResultValue[0] == 0 ) &&
                    ( sResultValue[1] == 0 ) &&
                    ( sResultValue[2] == 0 ) &&
                    ( sResultValue[3] == 0 ) )
                {
                    break;
                }
                else
                {
                    sNumIdx--;
                }
            }
        }
        else
        {
            sRemainder[sNumIdx] = sResultValue[sNumIdx];
        }
        
        for( i = DTL_DECIMAL_BIGINT_DEFAULT_PRECISION; i >= 0; i-- )
        {
            if( ( sRemainder[sNumIdx] / gPreDefinedPow[i] ) == 0 )
            {
                sDigitPtr++;
            }
            else
            {
                break;
            }
        }
        
        for( ; i >= 0; i-- )
        {
            sUInt64Value = sRemainder[sNumIdx] / gPreDefinedPow[i];
            
            sNumStr[ sNumStrLen ] = sNumberToStr[ sUInt64Value ];
            sNumStrLen++;
            
            sRemainder[sNumIdx] -= sUInt64Value * gPreDefinedPow[i];
        }
        
        sNumIdx++;
        
        for( ; sNumIdx < 4; sNumIdx++  )
        {
            for( i = (DTL_DECIMAL_BIGINT_DEFAULT_PRECISION - 1); i >= 0; i-- )
            {
                sUInt64Value = sRemainder[sNumIdx] / gPreDefinedPow[i];
                
                sNumStr[ sNumStrLen ] = sNumberToStr[ sUInt64Value ];
                sNumStrLen++;
                
                sRemainder[sNumIdx] -= sUInt64Value * gPreDefinedPow[i];
            }
        }
        
        STL_THROW( RAMP_CHECK_STR );
    }
    else
    {
        /* Do Nothing */
    }
    
    
    /*
     * 부호 검사
     */
    
    if( DTF_NUMBER_FMT_FLAG_IS_SIGN( aFormatInfo ) == STL_TRUE )
    {
        if( ( *sStr == '-' ) || ( *sStr == '+' ) )
        {
            STL_TRY_THROW( aFormatInfo->mSignLocate == DTF_NUMBER_SIGN_LOCATE_PRE,
                           ERROR_INVALID_INPUT_STRING );            
            
            sNumStr[ sNumStrLen ] = *sStr;
            sNumStrLen++;
            sStr++;
        }
        else
        {
            STL_TRY_THROW( ( aFormatInfo->mSignLocate == DTF_NUMBER_SIGN_LOCATE_POST ) &&
                           ( ( *(sStrEnd - 1) == '+' ) || ( *(sStrEnd - 1) == '-' ) ),
                           ERROR_INVALID_INPUT_STRING );
            
            sNumStr[ sNumStrLen ] = *(sStrEnd - 1);
            sNumStrLen++;
            sStrEnd--;
        }
    }
    else if( DTF_NUMBER_FMT_FLAG_IS_MINUS( aFormatInfo ) == STL_TRUE )
    {
        if( *(sStrEnd - 1) == ' ' )
        {
            sStrEnd--;
        }
        else if( ( *(sStrEnd - 1) == '-' ) || ( *(sStrEnd - 1) == '+' ) )
        {
            sNumStr[ sNumStrLen ] = *(sStrEnd - 1);
            sNumStrLen++;
            
            sStrEnd--;
        }
        else
        {
            if( DTF_NUMBER_FMT_FLAG_IS_FM( aFormatInfo ) == STL_FALSE )
            {
                STL_THROW( ERROR_INVALID_INPUT_STRING );
            }
            else
            {
                /* Do Nothing */
            }
        }
    }
    else if( DTF_NUMBER_FMT_FLAG_IS_BRACKET( aFormatInfo ) == STL_TRUE )
    {
        if( ( *sStr == '<' ) && ( *(sStrEnd - 1) == '>' ) )
        {
            sNumStr[ sNumStrLen ] = '-';
            sNumStrLen++;

            sStr++;
            sStrEnd--;
        }
        else 
        {
            if( sStr > (stlChar *)(aDataValue->mValue) )
            {
                /* 앞에 공백이 하나 이상 있는 경우 */
                
                if( ( *(sStr - 1) == ' ' ) && ( *(sStrEnd - 1) == ' ' ) )
                {
                    sStrEnd--;
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
        }
    }
    else
    {
        if( *sStr == '-' )
        {
            sNumStr[ sNumStrLen ] = *sStr;
            sNumStrLen++;
            sStr++;
        }
        else if( *sStr == '+' )
        {
            STL_THROW( ERROR_INVALID_INPUT_STRING );
        }
        else
        {
            /* Do Nothing */
        }
    }

    
    /*
     * $ 검사
     */
    
    if( DTF_NUMBER_FMT_FLAG_IS_DOLLAR( aFormatInfo ) == STL_TRUE )
    {
        STL_TRY_THROW( *sStr == '$', ERROR_INVALID_INPUT_STRING );
        sStr++;
        
        STL_TRY_THROW( sStr < sStrEnd, ERROR_INVALID_INPUT_STRING );
    }
    else
    {
        /* Do Nothing */
    }

    /* 공백, 부호, $ 를 제외한 포인터를 저장한다. */
    sTmpStrStart = sStr;
    
    DTF_SKIP_LEADING_ZERO_CHARACTERS( sStr, sStrEnd, sLeadingZeroCnt );
    
    while( sStr < sStrEnd )
    {
        if( DTF_IS_DIGIT( *sStr ) == STL_TRUE )
        {
            sNumStr[ sNumStrLen ] = *sStr;
            sNumStrLen++;
            sDigitCntBeforeDecimal++;
        }
        else
        {
            switch( *sStr )
            {
                case '.' :
                    {
                        STL_TRY_THROW( sIsExistPeriod == STL_FALSE,
                                       ERROR_INVALID_INPUT_STRING );
                        
                        if( DTF_NUMBER_FMT_FLAG_IS_EEEE( aFormatInfo ) == STL_FALSE )
                        {
                            sNumStr[ sNumStrLen ] = *sStr;
                            sNumStrLen++;

                            /* 소수점이하 trailing zero character 제거 */
                            DTF_SKIP_TRAILING_ZERO_EXCEPT_AFTER_DECIMAL_ONE_ZERO_CHARACTERS(
                                sStr,
                                sStrEnd,
                                sDigitCntAfterDecimal );
                        }
                        else
                        {
                            if( ( sDigitCntBeforeDecimal == 0 ) &&
                                ( sLeadingZeroCnt > 0 ) )
                            {
                                /* ex) TO_NUMBER( '0.E+00', 'FM0.9EEEE' ) */
                                sNumStr[ sNumStrLen ] = '0';
                                sNumStrLen++;
                                
                                sNumStr[ sNumStrLen ] = *sStr;
                                sNumStrLen++;
                            }
                            else
                            {
                                sNumStr[ sNumStrLen ] = *sStr;
                                sNumStrLen++;
                            }
                        }
                        
                        sIsExistPeriod = STL_TRUE;
                        sStr++;
                        
                        STL_THROW( RAMP_CHECK_DIGIT_BEFORE_DECIMAL );
                        
                        break;
                    }
                case ',' :
                    {
                        sCommaCnt++;
                        
                        break;
                    }
                case 'E':
                case 'e':
                    {
                        STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_EEEE( aFormatInfo ) == STL_TRUE,
                                       ERROR_INVALID_INPUT_STRING );
                        
                        STL_THROW( RAMP_CHECK_EEEE_NOTATION );
                        
                        break;
                    }
                default:
                    {
                        STL_THROW( ERROR_INVALID_INPUT_STRING );
                        
                        break;
                    }
            }
        }
        
        sStr++;
    }
    
    STL_RAMP( RAMP_CHECK_DIGIT_BEFORE_DECIMAL );

    if( sIsExistPeriod == STL_TRUE )
    {
        while( sStr < sStrEnd )
        {
            if( DTF_IS_DIGIT( *sStr ) == STL_TRUE )
            {
                sNumStr[ sNumStrLen ] = *sStr;
                sNumStrLen++;
                sDigitCntAfterDecimal++;
            }
            else
            {
                if( ( *sStr == 'E' ) || ( *sStr == 'e' ) )
                {
                    STL_TRY_THROW( DTF_NUMBER_FMT_FLAG_IS_EEEE( aFormatInfo ) == STL_TRUE,
                                       ERROR_INVALID_INPUT_STRING );
                    
                    STL_THROW( RAMP_CHECK_EEEE_NOTATION );
                }
                else
                {
                    STL_THROW( ERROR_INVALID_INPUT_STRING );
                }
            }
            
            sStr++;
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    
    STL_RAMP( RAMP_CHECK_EEEE_NOTATION );
    
    sTotalDigitCntBeforeDecimal = sDigitCntBeforeDecimal + sLeadingZeroCnt;    

    if( DTF_NUMBER_FMT_FLAG_IS_EEEE( aFormatInfo ) == STL_FALSE )
    {
        if( ( sLeadingZeroCnt > 0 ) &&
            ( sDigitCntBeforeDecimal == 0 ) &&
            ( sDigitCntAfterDecimal == 0 ) )
        {
            sNumStr[ sNumStrLen ] = '0';
            sNumStrLen++;
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        if( ( sLeadingZeroCnt > 0 ) &&
            ( sDigitCntBeforeDecimal == 0 ) &&
            ( sDigitCntAfterDecimal == 0 ) &&
            ( sIsExistPeriod == STL_FALSE ) )
        {
            sNumStr[ sNumStrLen ] = '0';
            sNumStrLen++;
        }
        else
        {
            /* Do Nothing */
        }

        STL_TRY_THROW( ( *sStr == 'E' ) || ( *sStr == 'e' ),
                       ERROR_INVALID_INPUT_STRING );
        
        sTmpStrEnd = sStr;
        sDigitCntApplyZeroFormat = 0;
        
        sNumStr[ sNumStrLen ] = *sStr;
        sNumStrLen++;
        sStr++;

        if( ( *sStr == '+' ) || ( *sStr == '-' ) )
        {
            sNumStr[ sNumStrLen ] = *sStr;
            sNumStrLen++;
            sStr++;
        }
        else
        {
            /* Do Nothing */
        }
        
        while( sStr < sStrEnd )
        {
            if( DTF_IS_DIGIT( *sStr ) == STL_TRUE )
            {
                sNumStr[ sNumStrLen ] = *sStr;
                sNumStrLen++;
            }
            else
            {
                STL_THROW( ERROR_INVALID_INPUT_STRING );
            }
            
            sStr++;
        }

        sStrEnd = sTmpStrEnd;
        
        STL_TRY_THROW( sTotalDigitCntBeforeDecimal <= 1,
                       ERROR_INVALID_INPUT_STRING );
    }
    
    if( DTF_NUMBER_FMT_FLAG_IS_ZERO( aFormatInfo ) == STL_TRUE )
    {
        if( sTotalDigitCntBeforeDecimal <= sDigitCntApplyZeroFormat )
        {
            STL_TRY_THROW( sTotalDigitCntBeforeDecimal == sDigitCntApplyZeroFormat,
                           ERROR_INVALID_INPUT_STRING );
        }
        else
        {
            STL_TRY_THROW( sTotalDigitCntBeforeDecimal <= aFormatInfo->mDigitCntBeforeDecimal,
                           ERROR_INVALID_INPUT_STRING );
        }
    }
    else
    {
        STL_TRY_THROW( sTotalDigitCntBeforeDecimal <= aFormatInfo->mDigitCntBeforeDecimal,
                       ERROR_INVALID_INPUT_STRING );
    }
    
    if( aFormatInfo->mCommaCnt > 0 )
    {
        sStr = sTmpStrStart;
        sFormatStrCommaCnt = aFormatInfo->mCommaCnt;
        sSkipFormatDigitCnt = aFormatInfo->mDigitCntBeforeDecimal - sTotalDigitCntBeforeDecimal;
        
        /* format string 중 digit 앞에 나올 수 있는 FM, S, $, B 를 제외한 포인터를 저장한다. */

        DTF_NUMBER_FMT_SKIP_FM_SIGN_NEXT_FORMAT_STR( sTmpFormatStr );
        DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpFormatStr );
        
        STL_DASSERT( DTF_IS_DIGIT( *sTmpFormatStr ) == STL_TRUE );
        
        while( sSkipFormatDigitCnt > 0 )
        {
            if( DTF_IS_DIGIT( *sTmpFormatStr ) == STL_TRUE )
            {
                sSkipFormatDigitCnt--;
            }
            else if( *sTmpFormatStr == ',' )
            {
                sFormatStrCommaCnt--;
            }
            else
            {
                /* Do Nothing  */
                STL_DASSERT( STL_FALSE );
            }
            
            sTmpFormatStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpFormatStr );
        }

        DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpFormatStr );

        while( *sTmpFormatStr == ',' )
        {
            sFormatStrCommaCnt--;
            sTmpFormatStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpFormatStr );
        }

        DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpFormatStr );
        
        STL_TRY_THROW( sCommaCnt == sFormatStrCommaCnt,
                       ERROR_INVALID_INPUT_STRING );

        while( sCommaCnt > 0 )
        {
            if( DTF_IS_DIGIT( *sStr ) == STL_TRUE )
            {
                STL_TRY_THROW( DTF_IS_DIGIT( *sTmpFormatStr ) == STL_TRUE,
                               ERROR_INVALID_INPUT_STRING );
            }
            else if( *sStr == ',' )
            {
                STL_TRY_THROW( *sTmpFormatStr == ',',
                               ERROR_INVALID_INPUT_STRING );
                sCommaCnt--;
            }
            else
            {
                STL_DASSERT( STL_FALSE );
            }
            
            sStr++;
            sTmpFormatStr++;
            DTF_NUMBER_FMT_SKIP_BLANK_DOLLAR_NEXT_FORMAT_STR( sTmpFormatStr );
        }
        
        STL_DASSERT( sCommaCnt == 0 );
    }
    else
    {
        STL_TRY_THROW( sCommaCnt == 0,
                       ERROR_INVALID_INPUT_STRING );
    }
    
    STL_TRY_THROW( sDigitCntAfterDecimal <= aFormatInfo->mDigitCntAfterDecimal,
                   ERROR_INVALID_INPUT_STRING );
    
    STL_RAMP( RAMP_CHECK_STR );
    
    *aNumberLiteralBufLen = sNumStrLen;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_INPUT_STRING )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_VALUE_NOT_NUMERIC_LITERAL,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/**
 * @brief 입력받은 datetime typed literal을 'SYYYY-MM-DD' format에 맞게 date 정보를 얻는다.
 * @param[in]  aDtlDataType            datetime datatype
 * @param[in]  aDateTimeString         datetime으로 변경할 string
 * @param[in]  aDateTimeStringLength   datetime으로 변경할 string length
 * @param[out] aDtlExpTime             dtlExpTime
 * @param[out] aRemainDateTimeStr      remain datetime str
 * @param[out] aRemainDateTimeStrLen   remain datetime str length
 * @param[in]  aVectorFunc             aVectorArg
 * @param[in]  aVectorArg              aVectorArg
 * @param[in]  aErrorStack             에러 스택
 */
stlStatus dtfToDateInfoFromTypedLiteral( dtlDataType                aDtlDataType,
                                         stlChar                  * aDateTimeString,
                                         stlInt64                   aDateTimeStringLength,
                                         dtlExpTime               * aDtlExpTime,
                                         stlChar                 ** aRemainDateTimeStr,
                                         stlInt64                 * aRemainDateTimeStrLen,
                                         dtlFuncVector            * aVectorFunc,
                                         void                     * aVectorArg,
                                         stlErrorStack            * aErrorStack )
{
    stlChar   * sDateTimeStringPtr = aDateTimeString;
    stlChar   * sDateTimeStringPtrEnd = aDateTimeString + aDateTimeStringLength;

    stlInt32   sIntValue = 0;
    stlInt32   sIntValueDigitCnt = 0;
    stlInt32   sLeadingZeroDigitCnt = 0;
    stlInt32   sElementStrLen = 0;
    stlBool    sIsNegative = STL_FALSE;

    
    STL_DASSERT( ( aDtlDataType == DTL_TYPE_DATE ) ||
                 ( aDtlDataType == DTL_TYPE_TIMESTAMP ) ||
                 ( aDtlDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ) );
    
    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );
    
    /*********************************************
     * SYYYY ( 년 )
     *********************************************/
    
    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );
    
    if( *sDateTimeStringPtr == '+' )
    {
        sIsNegative = STL_FALSE;
        sDateTimeStringPtr++;
    }
    else if( *sDateTimeStringPtr == '-' )
    {
        sIsNegative = STL_TRUE;
        sDateTimeStringPtr++;
    }
    else
    {
        sIsNegative = STL_FALSE;
    }

    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );
    
    STL_TRY( dtfToDateTimeGetIntValueFromDateTimeTypedLiteral(
                 sDateTimeStringPtr,
                 sDateTimeStringPtrEnd,
                 &(dtfDateTimeFormattingKeyword[DTF_DATETIME_FMT_SYYYY]),
                 & sIntValue,
                 & sIntValueDigitCnt,
                 & sLeadingZeroDigitCnt,
                 & sElementStrLen,
                 aVectorFunc,
                 aVectorArg,
                 aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( (sIntValue != 0), ERROR_DATETIME_YEAR_VALUE );
    
    if( sIsNegative == STL_TRUE )
    {
        aDtlExpTime->mYear = -(sIntValue - 1);
    }
    else
    {
        aDtlExpTime->mYear = sIntValue;
    }
    
    sDateTimeStringPtr += sElementStrLen;

    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );

    /*********************************************
     * -
     *********************************************/

    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );

    STL_TRY_THROW( *sDateTimeStringPtr == '-', ERROR_NOT_MATCH_FORMAT_STRING );

    sDateTimeStringPtr++;

    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );

    /*********************************************
     * MM ( 월 )
     *********************************************/

    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );

    STL_TRY( dtfToDateTimeGetIntValueFromDateTimeTypedLiteral(
                 sDateTimeStringPtr,
                 sDateTimeStringPtrEnd,
                 &(dtfDateTimeFormattingKeyword[DTF_DATETIME_FMT_MM]),
                 & sIntValue,
                 & sIntValueDigitCnt,
                 & sLeadingZeroDigitCnt,
                 & sElementStrLen,
                 aVectorFunc,
                 aVectorArg,
                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( (sIntValue >= 1) && (sIntValue <= DTL_MONTHS_PER_YEAR),
                   ERROR_MM_FIELD_OVERFLOW );
    
    aDtlExpTime->mMonth = sIntValue;

    sDateTimeStringPtr += sElementStrLen;
    
    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );

    /*********************************************
     * -
     *********************************************/ 

    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );
    
    STL_TRY_THROW( *sDateTimeStringPtr == '-', ERROR_NOT_MATCH_FORMAT_STRING );

    sDateTimeStringPtr++;

    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );

    /*********************************************
     * DD ( 일 )
     *********************************************/

    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );
    
    STL_TRY( dtfToDateTimeGetIntValueFromDateTimeTypedLiteral(
                 sDateTimeStringPtr,
                 sDateTimeStringPtrEnd,
                 &(dtfDateTimeFormattingKeyword[DTF_DATETIME_FMT_DD]),
                 & sIntValue,
                 & sIntValueDigitCnt,
                 & sLeadingZeroDigitCnt,
                 & sElementStrLen,
                 aVectorFunc,
                 aVectorArg,
                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( (sIntValue >= DTF_FIRST_DAY_OF_MONTH) &&
                   (sIntValue <= DTF_MAX_LAST_DAYS_PER_MONTH),
                   ERROR_DD_FIELD_OVERFLOW );
    
    aDtlExpTime->mDay = sIntValue;

    sDateTimeStringPtr += sElementStrLen;
    
    if( aDtlDataType == DTL_TYPE_DATE )
    {
        /* field간의 공백 무시 */
        DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );
        
        STL_TRY_THROW( sDateTimeStringPtr == sDateTimeStringPtrEnd,
                       ERROR_NOT_MATCH_FORMAT_STRING );
    }
    else
    {
        /* Do Nothing */
    }

    /* 일 validate */
    if( ( aDtlDataType == DTL_TYPE_DATE ) ||
        ( aDtlDataType == DTL_TYPE_TIMESTAMP ) ||
        ( aDtlDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ) )
    {
        STL_TRY_THROW(
            (aDtlExpTime->mDay <=
             dtlDayTab[DTL_IS_LEAP(aDtlExpTime->mYear)][aDtlExpTime->mMonth - 1]),
            ERROR_DATE_FOR_MONTH_SPECIFIED );
    }
    else
    {
        /* Do Nothing */
    }

    *aRemainDateTimeStr = sDateTimeStringPtr;
    *aRemainDateTimeStrLen = ( sDateTimeStringPtrEnd - sDateTimeStringPtr );    
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_DATETIME_YEAR_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_INVALID_YEAR_VALUE,
                      NULL,
                      aErrorStack,
                      DTF_DATE_MIN_YEAR_STRING,
                      DTF_DATE_MAX_YEAR_STRING );
    }

    STL_CATCH( ERROR_MM_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_INVALID_MONTH_VALUE,
                      NULL,
                      aErrorStack );
    }    

    STL_CATCH( ERROR_DD_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_INVALID_LAST_DAY_OF_MONTH_VALUE,
                      NULL,
                      aErrorStack );
    }
    
    STL_CATCH( ERROR_NOT_MATCH_FORMAT_STRING )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_NOT_MATCH_FORMAT_STRING,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_DATE_FOR_MONTH_SPECIFIED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_NOT_VALID_FOR_MONTH_SPECIFIED,
                      NULL,
                      aErrorStack );
        
    }    
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief 입력받은 datetime typed literal을 'HH24:MI:SS[.[FF6]]' format에 맞게 time 정보를 얻는다.
 * @param[in]  aDtlDataType            datetime datatype
 * @param[in]  aDateTimeString         datetime으로 변경할 string
 * @param[in]  aDateTimeStringLength   datetime으로 변경할 string length
 * @param[out] aDtlExpTime             dtlExpTime
 * @param[out] aFractionalSecond       fractional second
 * @param[out] aRemainDateTimeStr      remain datetime str
 * @param[out] aRemainDateTimeStrLen   remain datetime str length
 * @param[in]  aVectorFunc             aVectorArg
 * @param[in]  aVectorArg              aVectorArg
 * @param[in]  aErrorStack             에러 스택
 */
stlStatus dtfToTimeInfoFromTypedLiteral( dtlDataType                aDtlDataType,
                                         stlChar                  * aDateTimeString,
                                         stlInt64                   aDateTimeStringLength,
                                         dtlExpTime               * aDtlExpTime,
                                         dtlFractionalSecond      * aFractionalSecond,
                                         stlChar                 ** aRemainDateTimeStr,
                                         stlInt64                 * aRemainDateTimeStrLen,
                                         dtlFuncVector            * aVectorFunc,
                                         void                     * aVectorArg,
                                         stlErrorStack            * aErrorStack )
{
    stlChar   * sDateTimeStringPtr = aDateTimeString;
    stlChar   * sDateTimeStringPtrEnd = aDateTimeString + aDateTimeStringLength;
    
    stlInt32   sIntValue  = 0;
    stlInt32   sIntValueDigitCnt = 0;
    stlInt32   sLeadingZeroDigitCnt = 0;
    stlInt32   sElementStrLen = 0;

    STL_DASSERT( ( aDtlDataType == DTL_TYPE_TIME ) ||
                 ( aDtlDataType == DTL_TYPE_TIME_WITH_TIME_ZONE ) ||
                 ( aDtlDataType == DTL_TYPE_TIMESTAMP ) ||
                 ( aDtlDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ) );

    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );
    
    if( ( aDtlDataType == DTL_TYPE_TIMESTAMP ) ||
        ( aDtlDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ) )
    {
        STL_TRY_THROW( aDateTimeString < sDateTimeStringPtr,
                       ERROR_NOT_MATCH_FORMAT_STRING );
    }
    else
    {
        /* Do Nothing */
    }

    /*********************************************
     * HH24 ( 시 )
     *********************************************/

    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );

    STL_TRY( dtfToDateTimeGetIntValueFromDateTimeTypedLiteral(
                 sDateTimeStringPtr,
                 sDateTimeStringPtrEnd,
                 &(dtfDateTimeFormattingKeyword[DTF_DATETIME_FMT_HH24]),
                 & sIntValue,
                 & sIntValueDigitCnt,
                 & sLeadingZeroDigitCnt,
                 & sElementStrLen,
                 aVectorFunc,
                 aVectorArg,
                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( (sElementStrLen > 0) && (sIntValue >= DTF_HH24_MIN) && (sIntValue <= DTF_HH24_MAX),
                   ERROR_HH24_FILED_OVERFLOW );
    
    aDtlExpTime->mHour = sIntValue;

    sDateTimeStringPtr += sElementStrLen;

    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );

    /*********************************************
     * : 
     *********************************************/

    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );
    
    STL_TRY_THROW( *sDateTimeStringPtr == ':', ERROR_NOT_MATCH_FORMAT_STRING );
    
    sDateTimeStringPtr++;

    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );
    
    /*********************************************
     * MI ( 분 )
     *********************************************/

    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );

    STL_TRY( dtfToDateTimeGetIntValueFromDateTimeTypedLiteral(
                 sDateTimeStringPtr,
                 sDateTimeStringPtrEnd,
                 &(dtfDateTimeFormattingKeyword[DTF_DATETIME_FMT_MI]),
                 & sIntValue,
                 & sIntValueDigitCnt,
                 & sLeadingZeroDigitCnt,
                 & sElementStrLen,
                 aVectorFunc,
                 aVectorArg,
                 aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( (sElementStrLen > 0) && (sIntValue >= DTF_MINUTE_MIN) && (sIntValue <= DTF_MINUTE_MAX),
                   ERROR_MINUTE_FILED_OVERFLOW );
    
    aDtlExpTime->mMinute = sIntValue;
    
    sDateTimeStringPtr += sElementStrLen;
    
    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );

    /*********************************************
     * : 
     *********************************************/
    
    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );
    
    STL_TRY_THROW( *sDateTimeStringPtr == ':', ERROR_NOT_MATCH_FORMAT_STRING );
    
    sDateTimeStringPtr++;

    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );
    
    /*********************************************
     * SS ( 초 )
     *********************************************/

    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );

    STL_TRY( dtfToDateTimeGetIntValueFromDateTimeTypedLiteral(
                 sDateTimeStringPtr,
                 sDateTimeStringPtrEnd,
                 &(dtfDateTimeFormattingKeyword[DTF_DATETIME_FMT_SS]),
                 & sIntValue,
                 & sIntValueDigitCnt,
                 & sLeadingZeroDigitCnt,
                 & sElementStrLen,
                 aVectorFunc,
                 aVectorArg,
                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( (sElementStrLen > 0) && (sIntValue >= DTF_SECOND_MIN) && (sIntValue <= DTF_SECOND_MAX),
                   ERROR_SECOND_FIELD_OVERFLOW );
    
    aDtlExpTime->mSecond = sIntValue;    

    sDateTimeStringPtr += sElementStrLen;

    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );

    /*********************************************
     * fractional second
     *********************************************/ 

    *aFractionalSecond = 0;
    
    if( sDateTimeStringPtr < sDateTimeStringPtrEnd )
    {
        if( *sDateTimeStringPtr == '.' )
        {    
            /*********************************************
             * . 
             *********************************************/
            
            sDateTimeStringPtr++;
            
            /* field간의 공백 무시 */
            DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );
            
            if( sDateTimeStringPtr < sDateTimeStringPtrEnd )
            {
                /*********************************************
                 * FF6 ( fractional second )
                 *********************************************/
                
                STL_TRY( dtfToDateTimeGetIntValueFromDateTimeTypedLiteral(
                             sDateTimeStringPtr,
                             sDateTimeStringPtrEnd,
                             &(dtfDateTimeFormattingKeyword[DTF_DATETIME_FMT_FF6]),
                             & sIntValue,
                             & sIntValueDigitCnt,
                             & sLeadingZeroDigitCnt,
                             & sElementStrLen,
                             aVectorFunc,
                             aVectorArg,
                             aErrorStack )
                         == STL_SUCCESS );

                if( sElementStrLen > 0 )
                {
                    STL_TRY_THROW( (sIntValue >= DTF_FRACTIONAL_SECOND_MIN) &&
                                   (sIntValue <= DTF_FRACTIONAL_SECOND_FF6_MAX),
                                   ERROR_FRACTIONAL_SECOND_FIELD_OVERFLOW );
                    
                    *aFractionalSecond =
                        sIntValue * dtfFractionalSecondAdjustScale[sIntValueDigitCnt + sLeadingZeroDigitCnt];
                    
                    sDateTimeStringPtr += sElementStrLen;
                }
                else
                {
                    *aFractionalSecond = 0;
                }
            }
            else
            {
                STL_DASSERT( sDateTimeStringPtr == sDateTimeStringPtrEnd );
                
                /* Do Nothing */
            }
        }
        else
        {
            /* Do Nothing */
        }
        
        /* field간의 공백 무시 */
        DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );
    }
    else
    {
        STL_DASSERT( sDateTimeStringPtr == sDateTimeStringPtrEnd );
        
        /* Do Nothing */
    }
    
    if( ( aDtlDataType == DTL_TYPE_TIME ) ||
        ( aDtlDataType == DTL_TYPE_TIMESTAMP ) )
    {
        STL_TRY_THROW( sDateTimeStringPtr == sDateTimeStringPtrEnd,
                       ERROR_NOT_MATCH_FORMAT_STRING );
    }
    else
    {
        /* Do Nothing */
    }

    *aRemainDateTimeStr = sDateTimeStringPtr;
    *aRemainDateTimeStrLen = ( sDateTimeStringPtrEnd - sDateTimeStringPtr );

    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_MATCH_FORMAT_STRING )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_NOT_MATCH_FORMAT_STRING,
                      NULL,
                      aErrorStack );
    }    

    STL_CATCH( ERROR_HH24_FILED_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_INVALID_HOUR_VALUE,
                      NULL,
                      aErrorStack,
                      DTF_HH24_MIN,
                      DTF_HH24_MAX );
    }

    STL_CATCH( ERROR_MINUTE_FILED_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_INVALID_MINUTE_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_SECOND_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_INVALID_SECOND_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_FRACTIONAL_SECOND_FIELD_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_INVALID_FRACTIONAL_SECOND_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 입력받은 datetime typed literal을 {+|-}TZH:TZM format에 맞게 timezone 정보를 얻는다.
 * @param[in]  aDtlDataType            datetime datatype
 * @param[in]  aDateTimeString         datetime으로 변경할 string
 * @param[in]  aDateTimeStringLength   datetime으로 변경할 string length
 * @param[out] aTimeZone               timezone 정보 
 * @param[in]  aVectorFunc             aVectorArg
 * @param[in]  aVectorArg              aVectorArg
 * @param[in]  aErrorStack             에러 스택
 */
stlStatus dtfToTimeZoneInfoFromTypedLiteral( dtlDataType                aDtlDataType,
                                             stlChar                  * aDateTimeString,
                                             stlInt64                   aDateTimeStringLength,
                                             stlInt32                 * aTimeZone,
                                             dtlFuncVector            * aVectorFunc,
                                             void                     * aVectorArg,
                                             stlErrorStack            * aErrorStack )
{
    stlChar   * sDateTimeStringPtr = aDateTimeString;
    stlChar   * sDateTimeStringPtrEnd = aDateTimeString + aDateTimeStringLength;
    
    stlInt32   sIntValue  = 0;
    stlInt32   sIntValueDigitCnt = 0;
    stlInt32   sLeadingZeroDigitCnt = 0;
    stlInt32   sElementStrLen = 0;
    stlInt32   sTimeZoneHour = 0;
    stlInt32   sTimeZoneMinute = 0;
    stlInt32   sTimeZone = 0;
    stlBool    sIsNegative = STL_FALSE;
    
    
    STL_DASSERT( ( aDtlDataType == DTL_TYPE_TIME_WITH_TIME_ZONE ) ||
                 ( aDtlDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ) );

    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );

    /*********************************************
     * SING { + | - }
     *********************************************/

    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );
    
    if( *sDateTimeStringPtr == '+' )
    {
        sIsNegative = STL_FALSE;
    }
    else if( *sDateTimeStringPtr == '-' )
    {
        sIsNegative = STL_TRUE;
    }
    else
    {
        STL_THROW( ERROR_TZDISP_OVERFLOW );
    }

    sDateTimeStringPtr++;

    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );

    /*********************************************
     * TZH
     *********************************************/

    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );

    STL_TRY( dtfToDateTimeGetIntValueFromDateTimeTypedLiteral(
                 sDateTimeStringPtr,
                 sDateTimeStringPtrEnd,
                 &(dtfDateTimeFormattingKeyword[DTF_DATETIME_FMT_TZH]),
                 & sIntValue,
                 & sIntValueDigitCnt,
                 & sLeadingZeroDigitCnt,
                 & sElementStrLen,
                 aVectorFunc,
                 aVectorArg,
                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( (sElementStrLen > 0) &&
                   (sIntValue >= DTL_TIMEZONE_MIN_HOUR) &&
                   (sIntValue <= DTL_TIMEZONE_MAX_HOUR),
                   ERROR_TIMEZONE_HOUR_OVERFLOW );
    
    sTimeZoneHour = sIntValue * DTL_SECS_PER_HOUR;
    
    sDateTimeStringPtr += sElementStrLen;
    
    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );

    /*********************************************
     * :
     *********************************************/

    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );
    
    STL_TRY_THROW( *sDateTimeStringPtr == ':', ERROR_NOT_MATCH_FORMAT_STRING );
    
    sDateTimeStringPtr++;
    
    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );

    /*********************************************
     * TZM
     *********************************************/

    STL_TRY_THROW( sDateTimeStringPtr < sDateTimeStringPtrEnd, ERROR_NOT_MATCH_FORMAT_STRING );
    
    STL_TRY( dtfToDateTimeGetIntValueFromDateTimeTypedLiteral(
                 sDateTimeStringPtr,
                 sDateTimeStringPtrEnd,
                 &(dtfDateTimeFormattingKeyword[DTF_DATETIME_FMT_TZM]),
                 & sIntValue,
                 & sIntValueDigitCnt,
                 & sLeadingZeroDigitCnt,
                 & sElementStrLen,
                 aVectorFunc,
                 aVectorArg,
                 aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY_THROW( (sElementStrLen > 0) &&
                   (sIntValue >= DTF_MINUTE_MIN) &&
                   (sIntValue <= DTF_MINUTE_MAX),
                   ERROR_TIMEZONE_MINUTE_OVERFLOW );
    
    sTimeZoneMinute = sIntValue * DTL_SECS_PER_MINUTE;

    sDateTimeStringPtr += sElementStrLen;
    
    /* field간의 공백 무시 */
    DTF_SKIP_LEADING_SPACE( sDateTimeStringPtr, sDateTimeStringPtrEnd );

    STL_TRY_THROW( sDateTimeStringPtr == sDateTimeStringPtrEnd,
                   ERROR_NOT_MATCH_FORMAT_STRING );

    /*********************************************
     * TZH:TZM 을 second단위의 값으로 변경.
     *********************************************/
    
    sTimeZone = sTimeZoneHour + sTimeZoneMinute;
    
    sTimeZone = (sIsNegative == STL_FALSE) ? sTimeZone : -sTimeZone;
    
    STL_TRY_THROW( (sTimeZone >= DTL_TIMEZONE_MIN_OFFSET) &&
                   (sTimeZone <= DTL_TIMEZONE_MAX_OFFSET),
                   ERROR_TZDISP_OVERFLOW );

    *aTimeZone = -sTimeZone;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_TZDISP_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_TIME_ZONE_DISPLACEMENT_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_TIMEZONE_HOUR_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_INVALID_TIMEZONE_HOUR_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_TIMEZONE_MINUTE_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_TIME_INVALID_TIMEZONE_MINUTE_VALUE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_NOT_MATCH_FORMAT_STRING )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_NOT_MATCH_FORMAT_STRING,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief datetime typed literal string 중 format에 맞게 숫자로 변환해야 할 부분을 숫자로 변환한다.
 * @param[in]  aDateTimeString       datetime으로 변경할 string
 * @param[in]  aDateTimeStringEnd    datetime으로 변경할 string length
 * @param[in]  aKeyWord              dtlDateTimeFormatKeyWord(변경하고자하는 string의 format 정보)
 * @param[out] aIntValue             format에 맞게 datetime string을 int value로 변환
 * @param[out] aIntValueDigitCnt     변환된 int value의 digit count
 * @param[out] aLeadingZeroDigitCnt  제거된 leading zero digit count ( fractional second 값을 구하기 위해 사용됨.)
 * @param[out] aElementStrLen        format에 맞게 해석된 datetime string의 길이
 * @param[in]  aVectorFunc           aVectorArg
 * @param[in]  aVectorArg            aVectorArg
 * @param[in]  aErrorStack           에러 스택
 */
stlStatus dtfToDateTimeGetIntValueFromDateTimeTypedLiteral(
    stlChar                  * aDateTimeString,
    stlChar                  * aDateTimeStringEnd,
    const dtlDateTimeFormatKeyWord * aKeyWord,
    stlInt32                 * aIntValue,
    stlInt32                 * aIntValueDigitCnt,
    stlInt32                 * aLeadingZeroDigitCnt,
    stlInt32                 * aElementStrLen,
    dtlFuncVector            * aVectorFunc,
    void                     * aVectorArg,
    stlErrorStack            * aErrorStack )
{
    stlChar  * sDateTimeStr = aDateTimeString;
    stlChar    sValue[DTL_DECIMAL_INTEGER_DEFAULT_PRECISION + 1] = {0,};
    stlInt32   sValueLength = 0;
    stlInt32   sLeadingZeroDigitCnt = 0;
    stlInt32   sIntValue = 0;
    stlInt32   sElementStrLen = 0;
    stlInt32   i;

    /* leading zero 처리 */
    DTF_SKIP_LEADING_ZERO_CHARACTERS( sDateTimeStr, aDateTimeStringEnd, sLeadingZeroDigitCnt );
    
    /* 숫자 처리 */
    while( sDateTimeStr < aDateTimeStringEnd )
    {
        if( DTF_IS_DIGIT( *sDateTimeStr ) == STL_TRUE )
        {
            sValue[sValueLength] = *sDateTimeStr;
            sValueLength++;
            sDateTimeStr++;
        }
        else
        {
            break;
        }
    }
    
    sElementStrLen = sLeadingZeroDigitCnt + sValueLength;
    STL_TRY_THROW( sElementStrLen <= aKeyWord->mValueMaxLen,
                   ERROR_NOT_MATCH_FORMAT_STRING );
    
    if( sValueLength > 0 )
    {
        for( i = 0; i < sValueLength; i++ )
        {
            sIntValue = sIntValue * 10;
            sIntValue = sIntValue + (sValue[i] - '0');
        }
    }
    else
    {
        STL_DASSERT( sValueLength == 0 );
        sIntValue = 0;
    }
    
    *aIntValue = sIntValue;
    *aIntValueDigitCnt = sValueLength;
    *aLeadingZeroDigitCnt = sLeadingZeroDigitCnt;
    *aElementStrLen = sElementStrLen;
    
    STL_DASSERT( (*aIntValue >= 0) &&
                 (*aIntValueDigitCnt >= 0) &&
                 (*aLeadingZeroDigitCnt >= 0) &&
                 (*aElementStrLen >= 0) );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_MATCH_FORMAT_STRING )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATE_FORMAT_NOT_MATCH_FORMAT_STRING,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief datetime string 에 {+|-}tzh:tzm 형식의 문자열이 존재하는지 여부 반환
 *   <BR> 단지, {+|-}tzh:tzm 형식의 문자열이 존재하는지 여부만 체크하고,
 *   <BR>       validate 검사는 하지 않는다.
 * @param[in] aDateTimeString          datetime string
 * @param[in] aDateTimeStringLength    datetime string length
 * @param[in] aIsTimestampStr          년-월-일을 포함한 timestamp string인지 여부
 */
stlBool dtfIsExistTimeZoneStr( stlChar     * aDateTimeString,
                               stlInt64      aDateTimeStringLength,
                               stlBool       aIsTimestampStr )
{
    stlChar   * sDateTimeStringPtr = aDateTimeString;
    stlChar   * sDateTimeStringPtrEnd = aDateTimeString + aDateTimeStringLength;
    stlBool     sIsExist = STL_FALSE;
    stlInt8     sCnt = 0;


#define DTF_CHECK_REMAIN_STR( sDateTimeStringPtr, sDateTimeStringPtrEnd )  \
    {                                                                      \
        if( sDateTimeStringPtr < sDateTimeStringPtrEnd )                   \
        {                                                                  \
            /* Do Nothing */                                               \
        }                                                                  \
        else                                                               \
        {                                                                  \
            sIsExist = STL_FALSE;                                          \
            STL_THROW( RAMP_FINISH );                                      \
        }                                                                  \
    }

    DTF_CHECK_REMAIN_STR( sDateTimeStringPtr, sDateTimeStringPtrEnd );
    
    if( aIsTimestampStr == STL_TRUE )
    {
        /*
         * SYYYY-MM-DD
         */

        while( sDateTimeStringPtr < sDateTimeStringPtrEnd )
        {
            if( *sDateTimeStringPtr == '-' )
            {
                sCnt++;
                
                if( sCnt == 2 )
                {
                    sDateTimeStringPtr++;
                    break;
                }
            }
            
            sDateTimeStringPtr++;
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    DTF_CHECK_REMAIN_STR( sDateTimeStringPtr, sDateTimeStringPtrEnd );
    
    /*
     * HH24:MI:SS[.[FF6]]
     */

    sCnt = 0;
    
    while( sDateTimeStringPtr < sDateTimeStringPtrEnd )
    {
        if( *sDateTimeStringPtr == ':' )
        {
            sCnt++;
            
            if( sCnt == 2 )
            {
                sDateTimeStringPtr++;
                break;
            }
        }
        
        sDateTimeStringPtr++;
    }

    DTF_CHECK_REMAIN_STR( sDateTimeStringPtr, sDateTimeStringPtrEnd );

    /*
     * {+|-}TZH:TZM
     */ 
    
    while( sDateTimeStringPtr < sDateTimeStringPtrEnd )
    {
        if( ( *sDateTimeStringPtr == '+' ) || ( *sDateTimeStringPtr == '-' ) )
        {
            sIsExist = STL_TRUE;
            sDateTimeStringPtr++;
            break;
        }
        else
        {
            /* Do Nothing */
        }
        
        sDateTimeStringPtr++;
    }

    DTF_CHECK_REMAIN_STR( sDateTimeStringPtr, sDateTimeStringPtrEnd );

    if( sIsExist == STL_TRUE )
    {
        sIsExist = STL_FALSE;
        while( sDateTimeStringPtr < sDateTimeStringPtrEnd )
        {
            if( *sDateTimeStringPtr == ':' )
            {
                sIsExist = STL_TRUE;
                sDateTimeStringPtr++;
                break;
            }
            else
            {
                /* Do Nothing */
            }
            
            sDateTimeStringPtr++;
        }
    }
    else
    {
        /* Do Nothing */
    }

    STL_RAMP( RAMP_FINISH );
    
    return sIsExist;
}


/**
 * @brief 입력받은 unicode( utf16, utf32 ) string을 format string에 맞게 datetime으로 변환한다.
 * @param[in]  aUniEncoding            unicode encoding ( utf16 또는 utf32 )
 * @param[in]  aDateTimeType           datetime type
 * @param[in]  aDateTimeString         datetime으로 변경할 string ( unicode string )
 * @param[in]  aDateTimeStringLength   datetime으로 변경할 string length
 * @param[in]  aToDateTimeFormatInfo   format string에 대한 분석 정보
 * @param[out] aDtlExpTime             dtlExpTime
 * @param[out] aFractionalSecond       fractional second
 * @param[out] aTimeZone               time zone
 * @param[in]  aVectorFunc             aVectorArg
 * @param[in]  aVectorArg              aVectorArg
 * @param[in]  aErrorStack             에러 스택
 */
stlStatus dtfToDateTimeFromWCharString( dtlUnicodeEncoding         aUniEncoding,
                                        dtlDataType                aDateTimeType,
                                        void                     * aDateTimeString,
                                        stlInt64                   aDateTimeStringLength,
                                        dtlDateTimeFormatInfo    * aToDateTimeFormatInfo,
                                        dtlExpTime               * aDtlExpTime,
                                        dtlFractionalSecond      * aFractionalSecond,
                                        stlInt32                 * aTimeZone,
                                        dtlFuncVector            * aVectorFunc,
                                        void                     * aVectorArg,
                                        stlErrorStack            * aErrorStack )
{
    stlInt64          sOffset;
    stlInt64          sDestStringLength;
    dtlCharacterSet   sDestCharacterSet;
    stlChar           sDBCharsetDateTimeString[ DTL_CHAR_MAX_BUFFER_SIZE ];

    /**
     * @todo  sDBCharsetDateTimeString Buffer Size ???  
     * @todo  db charset 으로 미리 변환하지 않고, ???
     */

    sDestCharacterSet = aVectorFunc->mGetCharSetIDFunc(aVectorArg);

    STL_TRY( dtlUnicodeToMbConversion( aUniEncoding,
                                       sDestCharacterSet,
                                       aDateTimeString,
                                       aDateTimeStringLength,
                                       sDBCharsetDateTimeString,
                                       DTL_CHAR_MAX_BUFFER_SIZE,
                                       &sOffset,
                                       &sDestStringLength,
                                       aErrorStack ) == STL_SUCCESS );
    
    STL_DASSERT( sDestStringLength <= DTL_CHAR_MAX_BUFFER_SIZE );
    STL_TRY_THROW( sDestStringLength <= DTL_CHAR_MAX_BUFFER_SIZE,
                   ERRCODE_INTERNAL );

    STL_TRY( dtfToDateTimeFromString( aDateTimeType,
                                      sDBCharsetDateTimeString,
                                      sDestStringLength,
                                      aToDateTimeFormatInfo,
                                      aDtlExpTime,
                                      aFractionalSecond,
                                      aTimeZone,
                                      aVectorFunc,
                                      aVectorArg,
                                      aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( ERRCODE_INTERNAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INTERNAL,
                      NULL,
                      aErrorStack,
                      "dtfToDateTimeFromWCharString()" );  
    } 
    
    STL_FINISH;    
    
    return STL_FAILURE;    
}


/**
 * @brief datetime 정보를 format string에 맞게 ( utf16, utf32 ) string으로 변환한다.
 * @param[in]  aUniEncoding        unicode encoding ( utf16 또는 utf32 )
 * @param[in]  aFormatInfo         format string에 대한 분석 정보
 * @param[in]  aDtlExpTime         datetime 정보
 * @param[in]  aFractionalSecond   fractional second
 * @param[in]  aTimeZone           time zone
 * @param[in]  aAvailableSize      aAvailableSize 
 * @param[out] aResult             변환된 string
 * @param[out] aLength             변환된 string의 길이
 * @param[in]  aVectorFunc         aVectorArg
 * @param[in]  aVectorArg          aVectorArg
 * @param[in]  aErrorStack         에러 스택
 */
stlStatus dtfDateTimeToWCharString( dtlUnicodeEncoding      aUniEncoding,
                                    dtlDateTimeFormatInfo * aFormatInfo,
                                    dtlExpTime            * aDtlExpTime,
                                    dtlFractionalSecond     aFractionalSecond,
                                    stlInt32              * aTimeZone,
                                    stlInt64                aAvailableSize,
                                    void                  * aResult,
                                    stlInt64              * aLength,
                                    dtlFuncVector         * aVectorFunc,
                                    void                  * aVectorArg,
                                    stlErrorStack         * aErrorStack )
{
    stlInt64          sOffset;
    stlInt64          sDestStringLength;
    dtlCharacterSet   sCharacterSet;

    stlChar           sDateTimeString[DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE];
    stlInt64          sDateTimeStringLen;

    STL_TRY( dtfDateTimeToChar( aFormatInfo,
                                aDtlExpTime,
                                aFractionalSecond,
                                aTimeZone,
                                DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE,
                                sDateTimeString,
                                & sDateTimeStringLen,
                                aVectorFunc,
                                aVectorArg,
                                aErrorStack )
             == STL_SUCCESS );

    STL_DASSERT( sDateTimeStringLen <= DTL_DATETIME_TO_CHAR_RESULT_STRING_MAX_SIZE );

    sCharacterSet = aVectorFunc->mGetCharSetIDFunc(aVectorArg);
    
    STL_TRY( dtlMbToUnicodeConversion( sCharacterSet,
                                       aUniEncoding,
                                       sDateTimeString,
                                       sDateTimeStringLen,
                                       aResult,
                                       aAvailableSize,
                                       &sOffset,
                                       &sDestStringLength,
                                       aErrorStack ) == STL_SUCCESS );
    
    STL_TRY_THROW( sDestStringLength <= aAvailableSize, ERROR_NOT_ENOUGH_BUFFER );

    *aLength = sDestStringLength;

    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_ENOUGH_BUFFER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_DATE_BYTE_LENGTH_GREATER_BUFFER_LENGTH,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;    
    
    return STL_FAILURE;    
}

/** @} */

