package sunje.goldilocks.jdbc.dt;

import java.io.InputStream;
import java.io.Reader;
import java.math.BigDecimal;
import java.nio.ByteBuffer;
import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.RowIdBase;
import sunje.goldilocks.jdbc.core4.VersionSpecific;
import sunje.goldilocks.jdbc.ex.ErrorMgr;
import sunje.goldilocks.jdbc.util.Utils;

public class ColumnNumber extends ColumnNonDateTime
{
    private static final byte NUMERIC_SIGN_ZERO = (byte)0x80;
    private static final int MAX_BUFFER_SIZE_FOR_NUMERIC = 60;
    private static final int MAX_DIGIT_COUNT = 20;
    private static final int DOUBLE_MAX_VALID_DIGIT_COUNT = 9;
    private static final int FLOAT_MAX_VALID_DIGIT_COUNT = 4;
    //private static final int NUMBER_MIN_EXPONENT = -130; /* DTL엔 있으나 jdbc에선 현재 사용하지 않음 */
    private static final int NUMBER_MAX_EXPONENT = 126;
    private static final int LONG_MAX_EXPONENT = 9;
    private static final int INT_MAX_EXPONENT = 4;
    private static final int SHORT_MAX_EXPONENT = 2;
    private static final int BYTE_MAX_EXPONENT = 1;
    private static final int DIGIT_POSITIVE_FENCE_VALUE = 1;
    private static final int DIGIT_NEGATIVE_FENCE_VALUE = 101;    
    private static final int DBL_DIG = 15;
    //private static final int MAX_SCALE = 128; /* DTL엔 있으나 jdbc에선 현재 사용하지 않음 */
    private static final int MIN_EXPONENT = -65;
    private static final int MAX_EXPONENT = 62;
    
    private static final int NUMERIC_TO_STRING_BUF_SIZE = 60; // numeric을 String으로 표시할 때 이보다 길게 표현되진 않는다.
    private static final int PREDEFINED_MAX_POW = 308;
    
    private static final double[] PREDEFINED_10_POW_VALUES =
      { 1.0E-308, 1.0E-307, 1.0E-306, 1.0E-305, 1.0E-304, 1.0E-303, 1.0E-302, 1.0E-301,

        1.0E-300, 1.0E-299, 1.0E-298, 1.0E-297, 1.0E-296, 1.0E-295, 1.0E-294, 1.0E-293, 1.0E-292, 1.0E-291,
        1.0E-290, 1.0E-289, 1.0E-288, 1.0E-287, 1.0E-286, 1.0E-285, 1.0E-284, 1.0E-283, 1.0E-282, 1.0E-281,
        1.0E-280, 1.0E-279, 1.0E-278, 1.0E-277, 1.0E-276, 1.0E-275, 1.0E-274, 1.0E-273, 1.0E-272, 1.0E-271,
        1.0E-270, 1.0E-269, 1.0E-268, 1.0E-267, 1.0E-266, 1.0E-265, 1.0E-264, 1.0E-263, 1.0E-262, 1.0E-261,
        1.0E-260, 1.0E-259, 1.0E-258, 1.0E-257, 1.0E-256, 1.0E-255, 1.0E-254, 1.0E-253, 1.0E-252, 1.0E-251,
        1.0E-250, 1.0E-249, 1.0E-248, 1.0E-247, 1.0E-246, 1.0E-245, 1.0E-244, 1.0E-243, 1.0E-242, 1.0E-241,
        1.0E-240, 1.0E-239, 1.0E-238, 1.0E-237, 1.0E-236, 1.0E-235, 1.0E-234, 1.0E-233, 1.0E-232, 1.0E-231,
        1.0E-230, 1.0E-229, 1.0E-228, 1.0E-227, 1.0E-226, 1.0E-225, 1.0E-224, 1.0E-223, 1.0E-222, 1.0E-221,
        1.0E-220, 1.0E-219, 1.0E-218, 1.0E-217, 1.0E-216, 1.0E-215, 1.0E-214, 1.0E-213, 1.0E-212, 1.0E-211,
        1.0E-210, 1.0E-209, 1.0E-208, 1.0E-207, 1.0E-206, 1.0E-205, 1.0E-204, 1.0E-203, 1.0E-202, 1.0E-201,
        
        1.0E-200, 1.0E-199, 1.0E-198, 1.0E-197, 1.0E-196, 1.0E-195, 1.0E-194, 1.0E-193, 1.0E-192, 1.0E-191,
        1.0E-190, 1.0E-189, 1.0E-188, 1.0E-187, 1.0E-186, 1.0E-185, 1.0E-184, 1.0E-183, 1.0E-182, 1.0E-181,
        1.0E-180, 1.0E-179, 1.0E-178, 1.0E-177, 1.0E-176, 1.0E-175, 1.0E-174, 1.0E-173, 1.0E-172, 1.0E-171,
        1.0E-170, 1.0E-169, 1.0E-168, 1.0E-167, 1.0E-166, 1.0E-165, 1.0E-164, 1.0E-163, 1.0E-162, 1.0E-161,
        1.0E-160, 1.0E-159, 1.0E-158, 1.0E-157, 1.0E-156, 1.0E-155, 1.0E-154, 1.0E-153, 1.0E-152, 1.0E-151,
        1.0E-150, 1.0E-149, 1.0E-148, 1.0E-147, 1.0E-146, 1.0E-145, 1.0E-144, 1.0E-143, 1.0E-142, 1.0E-141,
        1.0E-140, 1.0E-139, 1.0E-138, 1.0E-137, 1.0E-136, 1.0E-135, 1.0E-134, 1.0E-133, 1.0E-132, 1.0E-131,
        1.0E-130, 1.0E-129, 1.0E-128, 1.0E-127, 1.0E-126, 1.0E-125, 1.0E-124, 1.0E-123, 1.0E-122, 1.0E-121,
        1.0E-120, 1.0E-119, 1.0E-118, 1.0E-117, 1.0E-116, 1.0E-115, 1.0E-114, 1.0E-113, 1.0E-112, 1.0E-111,
        1.0E-110, 1.0E-109, 1.0E-108, 1.0E-107, 1.0E-106, 1.0E-105, 1.0E-104, 1.0E-103, 1.0E-102, 1.0E-101,
        
        1.0E-100, 1.0E-99,  1.0E-98,  1.0E-97,  1.0E-96,  1.0E-95,  1.0E-94,  1.0E-93,  1.0E-92,  1.0E-91,
        1.0E-90,  1.0E-89,  1.0E-88,  1.0E-87,  1.0E-86,  1.0E-85,  1.0E-84,  1.0E-83,  1.0E-82,  1.0E-81,
        1.0E-80,  1.0E-79,  1.0E-78,  1.0E-77,  1.0E-76,  1.0E-75,  1.0E-74,  1.0E-73,  1.0E-72,  1.0E-71,
        1.0E-70,  1.0E-69,  1.0E-68,  1.0E-67,  1.0E-66,  1.0E-65,  1.0E-64,  1.0E-63,  1.0E-62,  1.0E-61,
        1.0E-60,  1.0E-59,  1.0E-58,  1.0E-57,  1.0E-56,  1.0E-55,  1.0E-54,  1.0E-53,  1.0E-52,  1.0E-51,
        1.0E-50,  1.0E-49,  1.0E-48,  1.0E-47,  1.0E-46,  1.0E-45,  1.0E-44,  1.0E-43,  1.0E-42,  1.0E-41,
        1.0E-40,  1.0E-39,  1.0E-38,  1.0E-37,  1.0E-36,  1.0E-35,  1.0E-34,  1.0E-33,  1.0E-32,  1.0E-31,
        1.0E-30,  1.0E-29,  1.0E-28,  1.0E-27,  1.0E-26,  1.0E-25,  1.0E-24,  1.0E-23,  1.0E-22,  1.0E-21,
        1.0E-20,  1.0E-19,  1.0E-18,  1.0E-17,  1.0E-16,  1.0E-15,  1.0E-14,  1.0E-13,  1.0E-12,  1.0E-11,
        1.0E-10,  1.0E-9,   1.0E-8,   1.0E-7,   1.0E-6,   1.0E-5,   1.0E-4,   1.0E-3,   1.0E-2,   1.0E-1,
        
        1.0,
        1.0E1,    1.0E2,    1.0E3,    1.0E4,    1.0E5,    1.0E6,    1.0E7,    1.0E8,    1.0E9,    1.0E10,
        1.0E11,   1.0E12,   1.0E13,   1.0E14,   1.0E15,   1.0E16,   1.0E17,   1.0E18,   1.0E19,   1.0E20,
        1.0E21,   1.0E22,   1.0E23,   1.0E24,   1.0E25,   1.0E26,   1.0E27,   1.0E28,   1.0E29,   1.0E30,
        1.0E31,   1.0E32,   1.0E33,   1.0E34,   1.0E35,   1.0E36,   1.0E37,   1.0E38,   1.0E39,   1.0E40,
        1.0E41,   1.0E42,   1.0E43,   1.0E44,   1.0E45,   1.0E46,   1.0E47,   1.0E48,   1.0E49,   1.0E50,
        1.0E51,   1.0E52,   1.0E53,   1.0E54,   1.0E55,   1.0E56,   1.0E57,   1.0E58,   1.0E59,   1.0E60,
        1.0E61,   1.0E62,   1.0E63,   1.0E64,   1.0E65,   1.0E66,   1.0E67,   1.0E68,   1.0E69,   1.0E70,
        1.0E71,   1.0E72,   1.0E73,   1.0E74,   1.0E75,   1.0E76,   1.0E77,   1.0E78,   1.0E79,   1.0E80,
        1.0E81,   1.0E82,   1.0E83,   1.0E84,   1.0E85,   1.0E86,   1.0E87,   1.0E88,   1.0E89,   1.0E90,
        1.0E91,   1.0E92,   1.0E93,   1.0E94,   1.0E95,   1.0E96,   1.0E97,   1.0E98,   1.0E99,   1.0E100,
        
        1.0E101,  1.0E102,  1.0E103,  1.0E104,  1.0E105,  1.0E106,  1.0E107,  1.0E108,  1.0E109,  1.0E110,
        1.0E111,  1.0E112,  1.0E113,  1.0E114,  1.0E115,  1.0E116,  1.0E117,  1.0E118,  1.0E119,  1.0E120,
        1.0E121,  1.0E122,  1.0E123,  1.0E124,  1.0E125,  1.0E126,  1.0E127,  1.0E128,  1.0E129,  1.0E130,
        1.0E131,  1.0E132,  1.0E133,  1.0E134,  1.0E135,  1.0E136,  1.0E137,  1.0E138,  1.0E139,  1.0E140,
        1.0E141,  1.0E142,  1.0E143,  1.0E144,  1.0E145,  1.0E146,  1.0E147,  1.0E148,  1.0E149,  1.0E150,
        1.0E151,  1.0E152,  1.0E153,  1.0E154,  1.0E155,  1.0E156,  1.0E157,  1.0E158,  1.0E159,  1.0E160,
        1.0E161,  1.0E162,  1.0E163,  1.0E164,  1.0E165,  1.0E166,  1.0E167,  1.0E168,  1.0E169,  1.0E170,
        1.0E171,  1.0E172,  1.0E173,  1.0E174,  1.0E175,  1.0E176,  1.0E177,  1.0E178,  1.0E179,  1.0E180,
        1.0E181,  1.0E182,  1.0E183,  1.0E184,  1.0E185,  1.0E186,  1.0E187,  1.0E188,  1.0E189,  1.0E190,
        1.0E191,  1.0E192,  1.0E193,  1.0E194,  1.0E195,  1.0E196,  1.0E197,  1.0E198,  1.0E199,  1.0E200,
        
        1.0E201,  1.0E202,  1.0E203,  1.0E204,  1.0E205,  1.0E206,  1.0E207,  1.0E208,  1.0E209,  1.0E210,
        1.0E211,  1.0E212,  1.0E213,  1.0E214,  1.0E215,  1.0E216,  1.0E217,  1.0E218,  1.0E219,  1.0E220,
        1.0E221,  1.0E222,  1.0E223,  1.0E224,  1.0E225,  1.0E226,  1.0E227,  1.0E228,  1.0E229,  1.0E230,
        1.0E231,  1.0E232,  1.0E233,  1.0E234,  1.0E235,  1.0E236,  1.0E237,  1.0E238,  1.0E239,  1.0E240,
        1.0E241,  1.0E242,  1.0E243,  1.0E244,  1.0E245,  1.0E246,  1.0E247,  1.0E248,  1.0E249,  1.0E250,
        1.0E251,  1.0E252,  1.0E253,  1.0E254,  1.0E255,  1.0E256,  1.0E257,  1.0E258,  1.0E259,  1.0E260,
        1.0E261,  1.0E262,  1.0E263,  1.0E264,  1.0E265,  1.0E266,  1.0E267,  1.0E268,  1.0E269,  1.0E270,
        1.0E271,  1.0E272,  1.0E273,  1.0E274,  1.0E275,  1.0E276,  1.0E277,  1.0E278,  1.0E279,  1.0E280,
        1.0E281,  1.0E282,  1.0E283,  1.0E284,  1.0E285,  1.0E286,  1.0E287,  1.0E288,  1.0E289,  1.0E290,
        1.0E291,  1.0E292,  1.0E293,  1.0E294,  1.0E295,  1.0E296,  1.0E297,  1.0E298,  1.0E299,  1.0E300,
        
        1.0E301,  1.0E302,  1.0E303,  1.0E304,  1.0E305,  1.0E306,  1.0E307,  1.0E308
    };
    
    private static final char[][][] NUMERIC_DIGIT_STRING =
       {{{'X','X'}, {'X','X'},
         {'9','9'}, {'9','8'}, {'9','7'}, {'9','6'}, {'9','5'}, {'9','4'}, {'9','3'}, {'9','2'}, {'9','1'}, {'9','0'},
         {'8','9'}, {'8','8'}, {'8','7'}, {'8','6'}, {'8','5'}, {'8','4'}, {'8','3'}, {'8','2'}, {'8','1'}, {'8','0'},
         {'7','9'}, {'7','8'}, {'7','7'}, {'7','6'}, {'7','5'}, {'7','4'}, {'7','3'}, {'7','2'}, {'7','1'}, {'7','0'},
         {'6','9'}, {'6','8'}, {'6','7'}, {'6','6'}, {'6','5'}, {'6','4'}, {'6','3'}, {'6','2'}, {'6','1'}, {'6','0'},
         {'5','9'}, {'5','8'}, {'5','7'}, {'5','6'}, {'5','5'}, {'5','4'}, {'5','3'}, {'5','2'}, {'5','1'}, {'5','0'},
         {'4','9'}, {'4','8'}, {'4','7'}, {'4','6'}, {'4','5'}, {'4','4'}, {'4','3'}, {'4','2'}, {'4','1'}, {'4','0'},
         {'3','9'}, {'3','8'}, {'3','7'}, {'3','6'}, {'3','5'}, {'3','4'}, {'3','3'}, {'3','2'}, {'3','1'}, {'3','0'},
         {'2','9'}, {'2','8'}, {'2','7'}, {'2','6'}, {'2','5'}, {'2','4'}, {'2','3'}, {'2','2'}, {'2','1'}, {'2','0'},
         {'1','9'}, {'1','8'}, {'1','7'}, {'1','6'}, {'1','5'}, {'1','4'}, {'1','3'}, {'1','2'}, {'1','1'}, {'1','0'},
         {'0','9'}, {'0','8'}, {'0','7'}, {'0','6'}, {'0','5'}, {'0','4'}, {'0','3'}, {'0','2'}, {'0','1'}, {'0','0'}},
        {{'X','X'},
         {'0','0'}, {'0','1'}, {'0','2'}, {'0','3'}, {'0','4'}, {'0','5'}, {'0','6'}, {'0','7'}, {'0','8'}, {'0','9'},
         {'1','0'}, {'1','1'}, {'1','2'}, {'1','3'}, {'1','4'}, {'1','5'}, {'1','6'}, {'1','7'}, {'1','8'}, {'1','9'},
         {'2','0'}, {'2','1'}, {'2','2'}, {'2','3'}, {'2','4'}, {'2','5'}, {'2','6'}, {'2','7'}, {'2','8'}, {'2','9'},
         {'3','0'}, {'3','1'}, {'3','2'}, {'3','3'}, {'3','4'}, {'3','5'}, {'3','6'}, {'3','7'}, {'3','8'}, {'3','9'},
         {'4','0'}, {'4','1'}, {'4','2'}, {'4','3'}, {'4','4'}, {'4','5'}, {'4','6'}, {'4','7'}, {'4','8'}, {'4','9'},
         {'5','0'}, {'5','1'}, {'5','2'}, {'5','3'}, {'5','4'}, {'5','5'}, {'5','6'}, {'5','7'}, {'5','8'}, {'5','9'},
         {'6','0'}, {'6','1'}, {'6','2'}, {'6','3'}, {'6','4'}, {'6','5'}, {'6','6'}, {'6','7'}, {'6','8'}, {'6','9'},
         {'7','0'}, {'7','1'}, {'7','2'}, {'7','3'}, {'7','4'}, {'7','5'}, {'7','6'}, {'7','7'}, {'7','8'}, {'7','9'},
         {'8','0'}, {'8','1'}, {'8','2'}, {'8','3'}, {'8','4'}, {'8','5'}, {'8','6'}, {'8','7'}, {'8','8'}, {'8','9'},
         {'9','0'}, {'9','1'}, {'9','2'}, {'9','3'}, {'9','4'}, {'9','5'}, {'9','6'}, {'9','7'}, {'9','8'}, {'9','9'},
         {'X','X'}}};

    private static final String[] EXPONENT_STRING =
        { "E-130", "E-129", "E-128", "E-127", "E-126",
          "E-125", "E-124", "E-123", "E-122", "E-121",
          "E-120", "E-119", "E-118", "E-117", "E-116",
          "E-115", "E-114", "E-113", "E-112", "E-111",
          "E-110", "E-109", "E-108", "E-107", "E-106",
          "E-105", "E-104", "E-103", "E-102", "E-101",
          "E-100", "E-99", "E-98", "E-97", "E-96",
          "E-95", "E-94", "E-93", "E-92", "E-91",
          "E-90", "E-89", "E-88", "E-87", "E-86",
          "E-85", "E-84", "E-83", "E-82", "E-81",
          "E-80", "E-79", "E-78", "E-77", "E-76",
          "E-75", "E-74", "E-73", "E-72", "E-71",
          "E-70", "E-69", "E-68", "E-67", "E-66",
          "E-65", "E-64", "E-63", "E-62", "E-61",
          "E-60", "E-59", "E-58", "E-57", "E-56",
          "E-55", "E-54", "E-53", "E-52", "E-51",
          "E-50", "E-49", "E-48", "E-47", "E-46",
          "E-45", "E-44", "E-43", "E-42", "E-41",
          "E-40", "E-39", "E-38", "E-37", "E-36",
          "E-35", "E-34", "E-33", "E-32", "E-31",
          "E-30", "E-29", "E-28", "E-27", "E-26",
          "E-25", "E-24", "E-23", "E-22", "E-21",
          "E-20", "E-19", "E-18", "E-17", "E-16",
          "E-15", "E-14", "E-13", "E-12", "E-11",
          "E-10", "E-9", "E-8", "E-7", "E-6",
          "E-5", "E-4", "E-3", "E-2", "E-1",
       
          "E0", "E1", "E2", "E3", "E4",
          "E5", "E6", "E7", "E8", "E9",
          "E10", "E11", "E12", "E13", "E14",
          "E15", "E16", "E17", "E18", "E19",
          "E20", "E21", "E22", "E23", "E24",
          "E25", "E26", "E27", "E28", "E29",
          "E30", "E31", "E32", "E33", "E34",
          "E35", "E36", "E37", "E38", "E39",
          "E40", "E41", "E42", "E43", "E44",
          "E45", "E46", "E47", "E48", "E49",
          "E50", "E51", "E52", "E53", "E54",
          "E55", "E56", "E57", "E58", "E59",
          "E60", "E61", "E62", "E63", "E64",
          "E65", "E66", "E67", "E68", "E69",
          "E70", "E71", "E72", "E73", "E74",
          "E75", "E76", "E77", "E78", "E79",
          "E80", "E81", "E82", "E83", "E84",
          "E85", "E86", "E87", "E88", "E89",
          "E90", "E91", "E92", "E93", "E94",
          "E95", "E96", "E97", "E98", "E99",
          "E100", "E101", "E102", "E103", "E104",
          "E105", "E106", "E107", "E108", "E109",
          "E110", "E111", "E112", "E113", "E114",
          "E115", "E116", "E117", "E118", "E119",
          "E120", "E121", "E122", "E123", "E124",
          "E125", "E126", "E127", "E128", "E129",
          "E130", "E131", "E132", "E133", "E134",
          "E135", "E136", "E137", "E138", "E139",
          "E140", "E141", "E142", "E143", "E144",
          "E145", "E146", "E147", "E148", "E149",
          "E150", "E151", "E152", "E153", "E154",
          "E155", "E156", "E157", "E158", "E159",
          "E160", "E161", "E162", "E163", "E164",
          "E165", "E166", "E167", "E168", "E169" };
    
    private static final byte[] HEX_TABLE = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
        -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    
    private ByteBuffer mValue = ByteBuffer.allocate(MAX_BUFFER_SIZE_FOR_NUMERIC);
    
    ColumnNumber(CodeColumn aCode)
    {
        super(aCode);
    }
    
    public long getArgNum1()
    {
        return mPrecision;
    }
    
    public long getArgNum2()
    {
        return mScale;
    }

    protected void setArgNum1(long aArgNum)
    {
        mPrecision = (int)aArgNum;
    }
    
    protected void setArgNum2(long aArgNum)
    {
        mScale = (int)aArgNum;
    }
    
    protected void loadValueFromCursorImpl() throws SQLException
    {
        mValue.clear();
        mCache.readBytes(mId, mValue);
        mValue.flip();
    }
    
    protected void storeValueToCursorImpl() throws SQLException
    {
        mValue.rewind();
        mCache.writeBytes(mId, mValue);
    }

    protected void setBooleanImpl(boolean aValue) throws SQLException
    {
        mValue.clear();
        longToNumeric(aValue ? 1 : 0, mValue);
    }

    protected void setByteImpl(byte aValue) throws SQLException
    {
        mValue.clear();
        longToNumeric(aValue, mValue);     
    }

    protected void setBytesImpl(byte[] aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "byte[]", mCode.getTypeName());
    }
    
    protected void setShortImpl(short aValue) throws SQLException
    {
        mValue.clear();
        longToNumeric(aValue, mValue);     
    }

    protected void setIntImpl(int aValue) throws SQLException
    {
        mValue.clear();
        longToNumeric(aValue, mValue);     
    }

    protected void setLongImpl(long aValue) throws SQLException
    {
        mValue.clear();
        longToNumeric(aValue, mValue);     
    }
    
    protected void setFloatImpl(float aValue) throws SQLException
    {
        mValue.clear();
        doubleToNumeric(aValue, (int)mPrecision, (int)mScale, mValue);     
    }

    protected void setDoubleImpl(double aValue) throws SQLException
    {
        mValue.clear();
        doubleToNumeric(aValue, (int)mPrecision, (int)mScale, mValue);     
    }

    protected void setStringImpl(String aValue) throws SQLException
    {
        mValue.clear();
        stringToNumeric(aValue, (int)mPrecision, (int)mScale, mValue);
    }

    protected void setObjectImpl(Object aValue) throws SQLException
    {
        mValue.clear();
        if (aValue instanceof Number)
        {
            doubleToNumeric(((Number)aValue).doubleValue(), (int)mPrecision, (int)mScale, mValue);
        }
        else
        {
            ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, aValue.getClass().getName(), mCode.getTypeName());
        }
    }

    protected void setBigDecimalImpl(BigDecimal aValue) throws SQLException
    {
        mValue.clear();
        doubleToNumeric(aValue.doubleValue(), (int)mPrecision, (int)mScale, mValue);
    }

    protected void setBinaryStreamImpl(InputStream aValue, long aLength) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "InputStream", mCode.getTypeName());
    }
    
    protected void setCharacterStreamImpl(Reader aValue, long aLength) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "Reader", mCode.getTypeName());
    }
    
    protected void setRowIdImpl(RowIdBase aValue) throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, "RowId", mCode.getTypeName());
    }

    protected boolean getBooleanImpl() throws SQLException
    {
        mValue.rewind();
        long sResult = numericToLong(mValue,
                                     BYTE_MAX_EXPONENT,
                                     "boolean");
        if (sResult != 0 && sResult != 1)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(sResult), "boolean");
        }
        return sResult != 0;
    }

    protected byte getByteImpl() throws SQLException
    {
        mValue.rewind();
        long sResult = numericToLong(mValue,
                                     BYTE_MAX_EXPONENT,
                                     "byte");
        if (sResult < Byte.MIN_VALUE || sResult > Byte.MAX_VALUE)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(sResult), "byte");
        }
        return (byte)sResult;
    }

    protected short getShortImpl() throws SQLException
    {
        mValue.rewind();
        long sResult = numericToLong(mValue,
                                     SHORT_MAX_EXPONENT,
                                     "short");
        if (sResult < Short.MIN_VALUE || sResult > Short.MAX_VALUE)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(sResult), "short");
        }
        return (short)sResult;
    }

    protected int getIntImpl() throws SQLException
    {
        mValue.rewind();
        long sResult = numericToLong(mValue,
                                     INT_MAX_EXPONENT,
                                     "int");
        if (sResult < Integer.MIN_VALUE || sResult > Integer.MAX_VALUE)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(sResult), "int");
        }
        return (int)sResult;
    }

    protected long getLongImpl() throws SQLException
    {
        mValue.rewind();
        return numericToLong(mValue,
                             LONG_MAX_EXPONENT,
                             "long");
    }
    
    protected float getFloatImpl() throws SQLException
    {
        mValue.rewind();
        return (float)numericToDouble(mValue,
                                      FLOAT_MAX_VALID_DIGIT_COUNT);
    }

    protected double getDoubleImpl() throws SQLException
    {
        mValue.rewind();
        return numericToDouble(mValue,
                               DOUBLE_MAX_VALID_DIGIT_COUNT);
    }

    protected String getStringImpl() throws SQLException
    {
        mValue.rewind();
        return numericToString(mValue,
                               mCache.getEnv().getMaxTrailingZeroCount(),
                               mCache.getEnv().getMaxLeadingZeroCount());
    }

    protected Object getObjectImpl() throws SQLException
    {
        mValue.rewind();
        /*
         * new BigDecimal(6.05)하면 6.0499999999999... 이런식으로 나오는 문제가 있음
         */
        return Utils.makeBigDecimal(numericToString(mValue,
                                                    mCache.getEnv().getMaxTrailingZeroCount(),
                                                    mCache.getEnv().getMaxLeadingZeroCount()));
    }

    protected BigDecimal getBigDecimalImpl() throws SQLException
    {
        mValue.rewind();
        return Utils.makeBigDecimal(numericToString(mValue,
                                                    mCache.getEnv().getMaxTrailingZeroCount(),
                                                    mCache.getEnv().getMaxLeadingZeroCount()));
    }
    
    protected Reader getCharacterStreamImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "Reader");
        return null;
    }

    protected RowIdBase getRowIdImpl() throws SQLException
    {
        ErrorMgr.raise(ErrorMgr.TYPE_CONVERSION_ERROR, mCode.getTypeName(), "RowId");
        return null;
    }

    private static void longToNumeric(long aValue, ByteBuffer aResult) throws SQLException
    {
        boolean sIsNegative = false;
        int sExponent = 0;
        byte[] sTempDigit = new byte[MAX_DIGIT_COUNT];
        int sDigitBeginIndex = MAX_DIGIT_COUNT - 1;
        int sDigitEndIndex = sDigitBeginIndex;
        int sDigitCount = 0;
        long sNum = aValue;
        int sSupplement = 0;
        
        if (aValue == 0)
        {
            aResult.put(NUMERIC_SIGN_ZERO);
            aResult.flip();
            return;
        }
        else if (aValue == Long.MIN_VALUE)
        {
            sIsNegative = true;
            aValue = -(aValue + 1);
            sSupplement = 1;
        }
        else if (aValue < 0)
        {
            sIsNegative = true;
            aValue = -aValue;
        }        

        while (aValue != 0)
        {
            sNum = aValue;
            aValue = sNum / 100;
            sTempDigit[sDigitBeginIndex] = (byte)(sNum - aValue * 100);
            if (sSupplement > 0)
            {
                sTempDigit[sDigitBeginIndex] += sSupplement;
                sSupplement = 0;
            }
            sDigitBeginIndex--;
        }
        
        while (sTempDigit[sDigitEndIndex] == 0)
        {
            sDigitEndIndex--;
            sExponent++;
        }
        
        sDigitCount = sDigitEndIndex - sDigitBeginIndex;
        sExponent += sDigitCount - 1;
        sDigitBeginIndex++;
        
        if (sIsNegative)
        {
            // 음수
            aResult.put((byte)(62 - sExponent));
            while (sDigitBeginIndex <= sDigitEndIndex)
            {
                aResult.put((byte)(101 - sTempDigit[sDigitBeginIndex++]));                
            }
        }
        else
        {
            // 양수
            aResult.put((byte)(193 + sExponent));
            while (sDigitBeginIndex <= sDigitEndIndex)
            {
                aResult.put((byte)(1 + sTempDigit[sDigitBeginIndex++]));                
            }
        }
        aResult.flip();
    }

    private static double get10Pow(int aValue)
    {
        if (aValue >= -PREDEFINED_MAX_POW && aValue <= PREDEFINED_MAX_POW)
        {
            return PREDEFINED_10_POW_VALUES[PREDEFINED_MAX_POW + aValue];
        }
        else
        {
            return Math.pow(10.0, aValue);
        }
    }
    
    private static void doubleToNumeric(double aValue, int aPrecision, int aScale, ByteBuffer aResult) throws SQLException
    {
        double sDouble;
        boolean sIsPositive;
        int sMaxExponent;
        int sMinExponent;
        double sPow;
        double sRoundDouble;
        long sDigitValue;
        int sDigitValueCount;
        int sDoubleValueCount;
        boolean sIsLastDigit;
        int sDigitLength;
        long sQuot;
        int sValue;
        
        if (aValue == 0)
        {
            aResult.put(NUMERIC_SIGN_ZERO);
            aResult.flip();
            return;
        }
        
        if (aValue < 0)
        {
            sDouble = -aValue;
            sIsPositive = false;
        }
        else
        {
            sDouble = aValue;
            sIsPositive = true;
        }
        
        sMaxExponent = VersionSpecific.SINGLETON.log10ToInt(sDouble);
        
        if (sMaxExponent < MIN_EXPONENT * 2)
        {
            aResult.put(NUMERIC_SIGN_ZERO);
            aResult.flip();
            return;
        }
        
        sPow = get10Pow(sMaxExponent - DBL_DIG - 1);
        sRoundDouble = sDouble + sPow * 5;
        if (sRoundDouble < 1)
        {
            sPow = get10Pow(sMaxExponent);
            if (sRoundDouble == sPow)
            {
                sDouble = sRoundDouble;
            }
            else
            {
                if (sMaxExponent == VersionSpecific.SINGLETON.log10ToInt(sRoundDouble))
                {
                    sMaxExponent--;
                }
                else
                {
                    sDouble = sRoundDouble;
                }
            }
        }
        else
        {
            if (sMaxExponent != VersionSpecific.SINGLETON.log10ToInt(sRoundDouble))
            {
                sDouble = sRoundDouble;
                sMaxExponent++;
            }
        }
        
        if (aScale == CodeColumn.SCALE_NA)
        {
            if (sMaxExponent >= NUMBER_MAX_EXPONENT)
            {
                ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(aValue), "Numeric");
            }
            
            aPrecision = aPrecision > DBL_DIG ? DBL_DIG : aPrecision;
            sMinExponent = sMaxExponent - aPrecision;
        }
        else
        {
            if (sMaxExponent >= aPrecision - aScale)
            {
                ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(aValue), "Numeric");
            }

            int sDestScale = -aScale - 1;
            aPrecision = (aPrecision > DBL_DIG ? DBL_DIG : aPrecision);
            sMinExponent = sMaxExponent - aPrecision;
            
            if (sMinExponent < sDestScale)
            {
                aPrecision += sMinExponent - sDestScale;
                sMinExponent = sDestScale;
            }
        }
        
        sPow = get10Pow(-sMinExponent);
        sDigitValue = ((long)(sDouble * sPow) + 5) / 10;
        
        sDigitValueCount = VersionSpecific.SINGLETON.log10ToInt(sDigitValue) + 1;
        sDoubleValueCount = sMaxExponent - sMinExponent;
        
        if (sDigitValueCount > sDoubleValueCount)
        {
            aPrecision++;
            sMaxExponent++;
        }
        
        if (aPrecision <= 0)
        {
            aResult.put(NUMERIC_SIGN_ZERO);
            aResult.flip();
            return;
        }
        
        if (sDigitValue >= PREDEFINED_10_POW_VALUES[PREDEFINED_MAX_POW + aPrecision])
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, String.valueOf(aValue), "Numeric");
        }
        
        if (sDigitValue == 0 || sMaxExponent < MIN_EXPONENT * 2)
        {
            aResult.put(NUMERIC_SIGN_ZERO);
            aResult.flip();
            return;
        }
        
        byte[] sDigit = new byte[MAX_DIGIT_COUNT + 2];
        sIsLastDigit = true;
        
        if ((sMinExponent & 0x01) == 1)
        {
            sDigitLength = (aPrecision + 1) >> 1;
        }
        else
        {
            sDigitLength = (aPrecision >> 1) + 1;
            sDigitValue *= 10;
        }
        
        if (sIsPositive)
        {
            for (int i = sDigitLength; i > 0; i--)
            {
                sQuot = sDigitValue / 100;
                sValue = (int)(sDigitValue - sQuot * 100);
                if (sIsLastDigit && sValue == 0)
                {
                    sDigitLength--;
                }
                else
                {
                    sIsLastDigit = false;
                    sDigit[i] = (byte)(DIGIT_POSITIVE_FENCE_VALUE + sValue);                    
                }
                
                sDigitValue = sQuot;
            }
            sDigit[0] = (byte)(NUMERIC_SIGN_ZERO | ((sMaxExponent >> 1) - MIN_EXPONENT));
        }
        else
        {
            for (int i = sDigitLength; i > 0; i--)
            {
                sQuot = sDigitValue / 100;
                sValue = (byte)(sDigitValue - sQuot * 100);
                if (sIsLastDigit && sValue == 0)
                {
                    sDigitLength--;
                }
                else
                {
                    sIsLastDigit = false;
                    sDigit[i] = (byte)(DIGIT_NEGATIVE_FENCE_VALUE - sValue);
                }
                
                sDigitValue = sQuot;
            }
            sDigit[0] = (byte)(MAX_EXPONENT - (sMaxExponent >> 1));
        }
        
        aResult.put(sDigit, 0, sDigitLength + 1);
        aResult.flip();
    }
    
    private static void stringToNumeric(String aValue, int aPrecision, int aScale, ByteBuffer aResult) throws SQLException
    {
        aValue = aValue.trim();

        boolean sIsPositive = true;
        int sValueIndex = 0;
        char[] sValue = aValue.toCharArray();
        int sValueFence = sValue.length;
        int sNumCnt = 0;
        byte[] sNumBuf = new byte[MAX_DIGIT_COUNT * 2];
        int sNumIndex = 1;
        boolean sHasDecimalPoint = false;
        int sExponent = 0;
        boolean sWhileDo = true;
        int sRoundPos = 0;
        
        if (sValue[sValueIndex] == '-')
        {
            sIsPositive = false;
            sValueIndex++;
        }
        else if (sValue[sValueIndex] == '+')
        {
            sValueIndex++;
        }
        while (sValueIndex < sValueFence && sValue[sValueIndex] == '0')
        {
            sValueIndex++;
        }
        if (sValueIndex == sValueFence)
        {
            aResult.put(NUMERIC_SIGN_ZERO);
            aResult.flip();
            return;
        }
        
        while (sValueIndex < sValueFence && sWhileDo)
        {
            if (sValue[sValueIndex] >= '0' && sValue[sValueIndex] <= '9')
            {
                if (sNumCnt < 39)
                {
                    sNumBuf[sNumIndex] = HEX_TABLE[sValue[sValueIndex]];
                    sNumIndex++;
                    sNumCnt++;
                }
                else
                {
                    if (sHasDecimalPoint == false)
                    {
                        sExponent++;                    
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else
            {
                switch (sValue[sValueIndex])
                {
                    case '.' :
                        if (sHasDecimalPoint)
                        {
                            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "Numeric");
                        }
                        
                        if (sNumCnt == 0)
                        {
                            sValueIndex++;
                            sExponent = 0;
                            while (sValueIndex < sValueFence && sValue[sValueIndex] == '0')
                            {
                                sValueIndex++;
                                sExponent++;
                            }
                            sValueIndex--;
                            sExponent = -sExponent;
                        }
                        else
                        {
                            sExponent += sNumCnt;
                        }
                        sHasDecimalPoint = true;
                        break;
                    case 'e' :                        
                    case 'E' :
                        if (sValueIndex + 1 >= sValueFence || sNumCnt <= 0)
                        {
                            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "Numeric");
                        }
                        sWhileDo = false;
                        break;
                    default :
                        ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "Numeric");
                        break;
                }
            }
            sValueIndex++;
        }
        
        if (sHasDecimalPoint == false)
        {
            sExponent += sNumCnt;
        }
        
        if (sWhileDo == false)
        {
            /* exponent 구문 파싱 */
            boolean sIsPositiveExponent = true;
            int sTempExpo = 0;
            
            if (sValue[sValueIndex] == '+')
            {
                sValueIndex++;
            }
            else if (sValue[sValueIndex] == '-')
            {
                sIsPositiveExponent = false;
                sValueIndex++;
            }
            if (sValueIndex >= sValueFence)
            {
                ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "Numeric");
            }
            
            while (sValueIndex < sValueFence && sValue[sValueIndex] == '0')
            {
                sValueIndex++;
            }
            
            while (sValueIndex < sValueFence)
            {
                if (sValue[sValueIndex] < '0' || sValue[sValueIndex] > '9')
                {
                    ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "Numeric");
                }
                sTempExpo *= 10;
                sTempExpo += HEX_TABLE[sValue[sValueIndex]];
                if (sTempExpo > Short.MAX_VALUE)
                {
                    ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "Numeric");
                }
                sValueIndex++;
            }
            
            if (sIsPositiveExponent)
            {
                sExponent += sTempExpo;
            }
            else
            {
                sExponent -= sTempExpo;
            }
        }
        
        if (aScale == CodeColumn.SCALE_NA)
        {
            if (sExponent > NUMBER_MAX_EXPONENT)
            {
                ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "Numeric");
            }
            sRoundPos = aPrecision;
        }
        else
        {
            /* [coverage]
             * number 타입으로 바인딩할 때, scale을 지정해줄 방법은 없다.
             * 따라서 SCALE_NA가 아닌 경우는 없지만, 그래도 남겨놓는다.
             */
            if (sExponent > aPrecision - aScale)
            {
                ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "Numeric");
            }
            sRoundPos = sExponent + aScale;
        }
        
        if (sRoundPos < 0)
        {
            /* [coverage]
             * 이 코드도 탈 일이 없다.
             */
            aResult.put(NUMERIC_SIGN_ZERO);
            aResult.flip();
            return;
        }
        
        if (sRoundPos <= sNumCnt)
        {
            sNumIndex = sRoundPos;
            boolean sCarry = (sNumBuf[sNumIndex + 1] & 0xFF) >= 5;
            while (sCarry && sRoundPos > 0)
            {
                sCarry = ++sNumBuf[sNumIndex] == 10;
                if (sCarry)
                {
                    sNumBuf[sNumIndex] = 0;
                    sRoundPos--;
                    sNumIndex--;
                }
            }
            
            if (sRoundPos == 0)
            {
                sNumCnt = sNumIndex + 1;
                sNumIndex = 0;
                sNumBuf[sNumIndex] = (sCarry ? (byte)1 : (byte)0);
                sExponent++;
            }
            else
            {
                sNumCnt = sNumIndex;
                sNumIndex = 1;
            }
            
            if (aScale == CodeColumn.SCALE_NA)
            {
                if (sExponent > NUMBER_MAX_EXPONENT)
                {
                    ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "Numeric");
                }
            }
            else
            {
                if (sExponent > aPrecision - aScale)
                {
                    ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "Numeric");
                }
            }
        }
        else
        {
            sNumIndex = 1;
        }
        
        boolean sIsOdd = (sExponent & 0x01) == 1;
        byte[] sDigit = new byte[MAX_DIGIT_COUNT + 2];
        int sDigitIndex = 1;
        
        sDigit[sDigitIndex] = 0;
        while (sNumCnt > 0)
        {
            if (sIsOdd)
            {
                sDigit[sDigitIndex] += sNumBuf[sNumIndex];
                sDigitIndex++;
            }
            else
            {
                sDigit[sDigitIndex] = (byte)(sNumBuf[sNumIndex] * 10);
            }
            sIsOdd = !sIsOdd;
            sNumCnt--;
            sNumIndex++;
        }
        
        int sDigitCnt = sDigitIndex - (sIsOdd ? 0 : 1);
        sDigitIndex = sDigitCnt;
        while (sDigitCnt > 0 && sDigit[sDigitIndex] == 0)
        {
            sDigitCnt--;
            sDigitIndex--;
        }
        
        int sValueLen = sDigitCnt + 1;
        int sResultExpo = ((sExponent + 1) >> 1) - 1;
        
        if (sResultExpo > MAX_EXPONENT)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aValue, "Numeric");
        }
        
        if (sDigitCnt == 0 || sResultExpo < MIN_EXPONENT)
        {
            aResult.put(NUMERIC_SIGN_ZERO);
            aResult.flip();
            return;
        }
        else
        {
            if (sIsPositive)
            {
                while (sDigitCnt > 0)
                {
                    sDigit[sDigitIndex]++;
                    sDigitIndex--;
                    sDigitCnt--;
                }
                sDigit[0] = (byte)(NUMERIC_SIGN_ZERO | (65 + sResultExpo));
            }
            else
            {
                while (sDigitCnt > 0)
                {
                    sDigit[sDigitIndex] = (byte)(DIGIT_NEGATIVE_FENCE_VALUE - sDigit[sDigitIndex]);
                    sDigitIndex--;
                    sDigitCnt--;
                }
                sDigit[0] = (byte)(62 - sResultExpo);
            }
        }
        
        aResult.put(sDigit, 0, sValueLen);
        aResult.flip();
    }
    
    private static int getExponent(int aDigit)
    {
        if (aDigit > 127)
        {
            // positive
            return aDigit - 193;
        }
        else
        {
            // negative
            return 62 - aDigit;
        }
    }
    
    private static long numericToLong(ByteBuffer aValue,
                                      int aMaxExponent,
                                      String aTargetTypeName) throws SQLException
    {
        int sLen = aValue.remaining();
        long sResult = 0;
        int sDigit;
        int sExponent;
        int sDigitCount;
        long sMaxFence = Long.MAX_VALUE / 100;
        long sMinFence = Long.MIN_VALUE / 100;
        int sMaxFenceRemain = (int)(Long.MAX_VALUE % 100);
        int sMinFenceRemain = (int)(Long.MIN_VALUE % 100);
        int sLastValue;

        /* null 처리는 상위 클래스인 Column에서 모두 처리되었어야 한다.*/
        if (sLen <= 0)
        {
            ErrorMgr.raiseRuntimeError("Numeric null processing");
        }
       
        sDigit = aValue.get() & 0xFF;
        sExponent = getExponent(sDigit);
        
        if (sExponent > aMaxExponent)
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, "Numeric value", aTargetTypeName);
        }
        
        if (sExponent < 0)
        {
            if (sExponent == -1)
            {
                if (sDigit > 127)
                {
                    sResult = ((aValue.get() & 0xFF) + 49) / 100;
                }
                else
                {
                    sResult = (151 - (aValue.get() & 0xFF)) / 100;
                    sResult = -sResult;
                }
            }
        }
        else if (sDigit > 127)
        {
            // positive
            sDigitCount = sLen - 1;
            if ((sDigitCount - 1) < sExponent)
            {
                sExponent -= sDigitCount;
                while (sDigitCount > 0)
                {
                    sResult = sResult * 100 + (aValue.get() & 0xFF) - 1;
                    sDigitCount--;
                }
                sResult *= (long)(PREDEFINED_10_POW_VALUES[PREDEFINED_MAX_POW + sExponent*2]);
                if (sResult > sMaxFence)
                {
                    ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, "Numeric value", aTargetTypeName);
                }
                sResult *= 100;
            }
            else
            {
                sDigitCount -= sExponent + 1;
                while (sExponent > 0)
                {
                    sResult = sResult * 100 + (aValue.get() & 0xFF) - 1;
                    sExponent--;
                }
                if (sResult > sMaxFence)
                {
                    ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, "Numeric value", aTargetTypeName);
                }
        
                sLastValue = (aValue.get() & 0xFF) - 1;
                if (sDigitCount > 0)
                {
                    // 소수점 반올림
                    sLastValue += (aValue.get() & 0xFF) > 50 ? 1 : 0;
                }
                if (sResult == sMaxFence && sLastValue > sMaxFenceRemain)
                {
                    ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, "Numeric value", aTargetTypeName);
                }
                sResult = sResult * 100 + sLastValue;
            }
        }
        else
        {
            // negative
            sDigitCount = sLen - 1;
            if ((sDigitCount - 1) < sExponent)
            {
                sExponent -= sDigitCount;
                while (sDigitCount > 0)
                {
                    sResult = sResult * 100 - 101 + (aValue.get() & 0xFF);
                    sDigitCount--;
                }
                sResult *= (long)(PREDEFINED_10_POW_VALUES[PREDEFINED_MAX_POW+sExponent*2]);
                if (sResult < sMinFence)
                {
                    ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, "Numeric value", aTargetTypeName);
                }
                sResult *= 100;
            }
            else
            {
                sDigitCount -= sExponent + 1;
                while (sExponent > 0)
                {
                    sResult = sResult * 100 - 101 + (aValue.get() & 0xFF);
                    sExponent--;
                }
                if (sResult < sMinFence)
                {
                    ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, "Numeric value", aTargetTypeName);
                }
        
                sLastValue = -101 + (aValue.get() & 0xFF);
                if (sDigitCount > 0)
                {
                    sLastValue -= (aValue.get() & 0xFF) < 52 ? 1 : 0;
                }
                if (sResult == sMinFence && sLastValue < sMinFenceRemain)
                {
                    ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, "Numeric value", aTargetTypeName);
                }
                sResult = sResult * 100 + sLastValue;
            }
        }
        return sResult;
    }

    private static double numericToDouble(ByteBuffer aValue,
                                          int aValidDigit)
    {
        int sLen = aValue.remaining();
        double sResult;
        int sExponent;
        int sDigitLength;
        
        /* null 처리는 상위 클래스인 Column에서 모두 처리되었어야 한다.*/
        if (sLen <= 0)
        {
            ErrorMgr.raiseRuntimeError("Numeric null processing");
        }

        if (sLen == 1 && aValue.get() == NUMERIC_SIGN_ZERO)
        {
            sResult = 0.0;
        }
        else
        {
            int sDigit = aValue.get() & 0xFF;
            if (sDigit > 127)
            {
                // positive
                sExponent = (sDigit - 193) << 1;
                sDigitLength = Math.min(sLen - 1, aValidDigit);
                sResult = 0.0;
                
                while (sDigitLength > 0)
                {
                    sResult += VersionSpecific.SINGLETON.multiplyAccurately((aValue.get() & 0xFF) - 1,
                                                  PREDEFINED_10_POW_VALUES[sExponent + PREDEFINED_MAX_POW]);
                    sDigitLength--;
                    sExponent -= 2;
                }
            }
            else
            {
                // negative
                sExponent = (62 - sDigit) << 1;
                sDigitLength = Math.min(sLen - 1, aValidDigit);
                sResult = 0.0;
                
                while (sDigitLength > 0)
                {
                    sResult -= VersionSpecific.SINGLETON.multiplyAccurately(101 - (aValue.get() & 0xFF),
                                                  PREDEFINED_10_POW_VALUES[sExponent + PREDEFINED_MAX_POW]);
                    sDigitLength--;
                    sExponent -= 2;
                }
            }
        }
        return sResult;
    }
    
    public static String numericToString(ByteBuffer aValue,
                                         int aMaxTrailingZeroCount,
                                         int aMaxLeadingZeroCount)
    {
        int sLen = aValue.remaining();
        
        /* null 처리는 상위 클래스인 Column에서 모두 처리되었어야 한다.*/
        if (sLen <= 0)
        {
            ErrorMgr.raiseRuntimeError("Numeric null processing");
        }

        int sFirstDigit = aValue.get() & 0xFF;
        
        if (sFirstDigit == 128 && sLen == 1)
        {
            return "0";
        }
        
        int sDigitLen = sLen - 1;
        int sExponent = getExponent(sFirstDigit);
        int sDigitIndex = aValue.position();
        char[] sResult = new char[NUMERIC_TO_STRING_BUF_SIZE];
        int sResultIndex = 0;
        int sIsPositive;
        int sZeroLen = 0;
        boolean sFullExpression = true;
        
        if (sFirstDigit > 127)
        {
            // positive
            sIsPositive = 1;
        }
        else
        {
            // negative
            sIsPositive = 0;
            sResult[sResultIndex] = '-';
            sResultIndex++;
        }

        if (sExponent < 0)
        {
            // 소수점 이하 leading-0의 개수
            sZeroLen = -((sExponent + 1) << 1);
            int sDigitSubIndex = sDigitIndex;
            while (sDigitSubIndex <= sDigitLen)
            {
                if (NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitSubIndex)][0] != '0')
                {
                    break;                    
                }
                sZeroLen++;
                if (NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitSubIndex)][1] != '0')
                {
                    break;                    
                }
                sZeroLen++;
                sDigitSubIndex++;
            }
            if (sZeroLen > aMaxLeadingZeroCount)
            {
                sFullExpression = false;
            }
        }
        else if (sDigitLen > sExponent + 1)
        {
            sZeroLen = 0;
            int sDigitSubIndex = sExponent + sDigitIndex;
            while (sDigitSubIndex > 0)
            {
                if (NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitSubIndex)][1] != '0')
                {
                    break;                    
                }
                sZeroLen++;
                if (NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitSubIndex)][0] != '0')
                {
                    break;                    
                }
                sZeroLen++;
                sDigitSubIndex--;
            }
            if (sZeroLen > aMaxTrailingZeroCount)
            {
                sFullExpression = false;
            }
        }
        else
        {
            // 소수점 이상 trailing-0의 개수
            sZeroLen = (sExponent - sDigitLen + 1) << 1;
            int sDigitSubIndex = sDigitLen + sDigitIndex - 1;
            while (sDigitSubIndex > 0)
            {
                if (NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitSubIndex)][1] != '0')
                {
                    break;                    
                }
                sZeroLen++;
                if (NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitSubIndex)][0] != '0')
                {
                    break;                    
                }
                sZeroLen++;
                sDigitSubIndex--;
            }
            if (sZeroLen > aMaxTrailingZeroCount)
            {
                sFullExpression = false;
            }
        }

        if (sFullExpression)
        {
            /* decimal point style expression */
            if (sExponent < 0)
            {
                sResult[sResultIndex++] = '.';
                sZeroLen = -((++sExponent) << 1);
                while (sZeroLen > 0)
                {
                    sResult[sResultIndex++] = '0';
                    sZeroLen--;
                }
                while (sDigitLen > 1)
                {
                    sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][0];
                    sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][1];
                    sDigitIndex++;
                    sDigitLen--;
                }
                sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][0];
                if (NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][1] != '0')
                {
                    sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][1];
                }
            }
            else if (sDigitLen > sExponent + 1)
            {
                if (NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][0] != '0')
                {
                    sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][0];                
                }
                sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][1];
                sDigitIndex++;
                sDigitLen--;
                while (sExponent > 0)
                {
                    sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][0];
                    sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][1];
                    sDigitIndex++;
                    sDigitLen--;
                    sExponent--;
                }
                sResult[sResultIndex++] = '.';
                while (sDigitLen > 1)
                {
                    sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][0];
                    sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][1];
                    sDigitIndex++;
                    sDigitLen--;
                }
                sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][0];
                if (NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][1] != '0')
                {
                    sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][1];
                }
            }
            else
            {
                sZeroLen = (sExponent - sDigitLen + 1) << 1;
                
                if (NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][0] != '0')
                {
                    sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][0];
                }
                sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][1];
                sDigitIndex++;
                sDigitLen--;
                while (sDigitLen > 0)
                {
                    sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][0];
                    sResult[sResultIndex++] = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][1];
                    sDigitIndex++;
                    sDigitLen--;
                }
                while (sZeroLen > 0)
                {
                    sResult[sResultIndex++] = '0';
                    sZeroLen--;
                }
            }
        }
        else
        {
            /* exponent style expression */
            sExponent = sExponent << 1;
            boolean sIsOdd = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)][0] == '0';
            sExponent += (sIsOdd ? 0 : 1);
            
            int sDigitPos = sResultIndex + 1;
            char[] sDigit = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)];
            while (sDigitLen > 0)
            {
                if (sIsOdd)
                {
                    sResult[sDigitPos] = sDigit[1];
                    sDigitIndex++;
                    sDigitLen--;
                }
                else
                {
                    sDigit = NUMERIC_DIGIT_STRING[sIsPositive][aValue.get(sDigitIndex)];
                    sResult[sDigitPos] = sDigit[0];
                }
                sIsOdd = !sIsOdd;
                sDigitPos++;
            }

            /* trailing zero */
            sDigitPos--;
            while (sDigitPos > 0 && sResult[sDigitPos] == '0')
            {
                sDigitPos--;
            }
            
            /* decimal point 설정 */
            if (sDigitPos == 0)
            {
                sResult[sResultIndex++] = '1';
                sExponent++;
            }
            else
            {
                sResult[sResultIndex] = sResult[sResultIndex+1];
                sResultIndex++;
                if (sDigitPos != 1)
                {
                    sResult[sResultIndex] = '.';
                    sResultIndex = sDigitPos + 1;
                }
            }
            
            /* exponent 설정 */
            String sExpoStr = EXPONENT_STRING[130 + sExponent];
            int sExpoStrIndex = 0;
            switch (sExpoStr.length())
            {
                case 5:
                    sResult[sResultIndex++] = sExpoStr.charAt(sExpoStrIndex++);
                case 4:
                    sResult[sResultIndex++] = sExpoStr.charAt(sExpoStrIndex++);
                case 3:
                    sResult[sResultIndex++] = sExpoStr.charAt(sExpoStrIndex++);
                case 2:
                    sResult[sResultIndex++] = sExpoStr.charAt(sExpoStrIndex++);
                case 1:
                    sResult[sResultIndex++] = sExpoStr.charAt(sExpoStrIndex);
                    break;
            }
        }
        
        return new String(sResult, 0, sResultIndex);
    }
}
