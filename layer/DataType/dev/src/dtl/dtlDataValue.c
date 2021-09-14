/*******************************************************************************
 * dtlDataValue.c
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
 * @file dtlDataType.c
 * @brief Data Type Layer Data Value 제어 함수 
 */

#include <dtl.h>
#include <dtdDataType.h>
#include <dtDef.h>
#include <dtfNumeric.h>
#include <dtfUnixTime.h>

/**
 * @addtogroup dtlDataValueControl
 * @{
 */


static stlChar dtlTrueValue[1] = { STL_TRUE };
static stlChar dtlFalseValue[1] = { STL_FALSE };

dtlBoolean dtlBooleanTrue[1] = 
{
    {
        DTL_TYPE_BOOLEAN,
        1,
        1,
        & dtlTrueValue
    }
};

dtlBoolean dtlBooleanFalse[1] = 
{
    {
        DTL_TYPE_BOOLEAN,
        1,
        1,
        & dtlFalseValue
    }
};

dtlBoolean dtlBooleanNull[1] = 
{
    {
        DTL_TYPE_BOOLEAN,
        0,
        0,
        NULL
    }
};


const dtlNumericDigitString dtdNumericDigitStringArr[2][102] =
{
    {
        {"XX"},{"XX"},
        {"99"},{"98"},{"97"},{"96"},{"95"},{"94"},{"93"},{"92"},{"91"},{"90"},
        {"89"},{"88"},{"87"},{"86"},{"85"},{"84"},{"83"},{"82"},{"81"},{"80"},
        {"79"},{"78"},{"77"},{"76"},{"75"},{"74"},{"73"},{"72"},{"71"},{"70"},
        {"69"},{"68"},{"67"},{"66"},{"65"},{"64"},{"63"},{"62"},{"61"},{"60"},
        {"59"},{"58"},{"57"},{"56"},{"55"},{"54"},{"53"},{"52"},{"51"},{"50"},
        {"49"},{"48"},{"47"},{"46"},{"45"},{"44"},{"43"},{"42"},{"41"},{"40"},
        {"39"},{"38"},{"37"},{"36"},{"35"},{"34"},{"33"},{"32"},{"31"},{"30"},
        {"29"},{"28"},{"27"},{"26"},{"25"},{"24"},{"23"},{"22"},{"21"},{"20"},
        {"19"},{"18"},{"17"},{"16"},{"15"},{"14"},{"13"},{"12"},{"11"},{"10"},
        {"09"},{"08"},{"07"},{"06"},{"05"},{"04"},{"03"},{"02"},{"01"},{"00"}
    },
    {
        {"XX"},
        {"00"},{"01"},{"02"},{"03"},{"04"},{"05"},{"06"},{"07"},{"08"},{"09"},
        {"10"},{"11"},{"12"},{"13"},{"14"},{"15"},{"16"},{"17"},{"18"},{"19"},
        {"20"},{"21"},{"22"},{"23"},{"24"},{"25"},{"26"},{"27"},{"28"},{"29"},
        {"30"},{"31"},{"32"},{"33"},{"34"},{"35"},{"36"},{"37"},{"38"},{"39"},
        {"40"},{"41"},{"42"},{"43"},{"44"},{"45"},{"46"},{"47"},{"48"},{"49"},
        {"50"},{"51"},{"52"},{"53"},{"54"},{"55"},{"56"},{"57"},{"58"},{"59"},
        {"60"},{"61"},{"62"},{"63"},{"64"},{"65"},{"66"},{"67"},{"68"},{"69"},
        {"70"},{"71"},{"72"},{"73"},{"74"},{"75"},{"76"},{"77"},{"78"},{"79"},
        {"80"},{"81"},{"82"},{"83"},{"84"},{"85"},{"86"},{"87"},{"88"},{"89"},
        {"90"},{"91"},{"92"},{"93"},{"94"},{"95"},{"96"},{"97"},{"98"},{"99"},
        {"XX"}
    }
};


const dtlExponentString dtdNumberExponentStringArr[650] =
{
    {5,"E-325"}, {5,"E-324"}, {5,"E-323"}, {5,"E-322"}, {5,"E-321"}, 
    {5,"E-320"}, {5,"E-319"}, {5,"E-318"}, {5,"E-317"}, {5,"E-316"}, 
    {5,"E-315"}, {5,"E-314"}, {5,"E-313"}, {5,"E-312"}, {5,"E-311"}, 
    {5,"E-310"}, {5,"E-309"}, {5,"E-308"}, {5,"E-307"}, {5,"E-306"}, 
    {5,"E-305"}, {5,"E-304"}, {5,"E-303"}, {5,"E-302"}, {5,"E-301"}, 
    {5,"E-300"}, {5,"E-299"}, {5,"E-298"}, {5,"E-297"}, {5,"E-296"}, 
    {5,"E-295"}, {5,"E-294"}, {5,"E-293"}, {5,"E-292"}, {5,"E-291"}, 
    {5,"E-290"}, {5,"E-289"}, {5,"E-288"}, {5,"E-287"}, {5,"E-286"}, 
    {5,"E-285"}, {5,"E-284"}, {5,"E-283"}, {5,"E-282"}, {5,"E-281"}, 
    {5,"E-280"}, {5,"E-279"}, {5,"E-278"}, {5,"E-277"}, {5,"E-276"}, 
    {5,"E-275"}, {5,"E-274"}, {5,"E-273"}, {5,"E-272"}, {5,"E-271"}, 
    {5,"E-270"}, {5,"E-269"}, {5,"E-268"}, {5,"E-267"}, {5,"E-266"}, 
    {5,"E-265"}, {5,"E-264"}, {5,"E-263"}, {5,"E-262"}, {5,"E-261"}, 
    {5,"E-260"}, {5,"E-259"}, {5,"E-258"}, {5,"E-257"}, {5,"E-256"}, 
    {5,"E-255"}, {5,"E-254"}, {5,"E-253"}, {5,"E-252"}, {5,"E-251"}, 
    {5,"E-250"}, {5,"E-249"}, {5,"E-248"}, {5,"E-247"}, {5,"E-246"}, 
    {5,"E-245"}, {5,"E-244"}, {5,"E-243"}, {5,"E-242"}, {5,"E-241"}, 
    {5,"E-240"}, {5,"E-239"}, {5,"E-238"}, {5,"E-237"}, {5,"E-236"}, 
    {5,"E-235"}, {5,"E-234"}, {5,"E-233"}, {5,"E-232"}, {5,"E-231"}, 
    {5,"E-230"}, {5,"E-229"}, {5,"E-228"}, {5,"E-227"}, {5,"E-226"}, 
    {5,"E-225"}, {5,"E-224"}, {5,"E-223"}, {5,"E-222"}, {5,"E-221"}, 
    {5,"E-220"}, {5,"E-219"}, {5,"E-218"}, {5,"E-217"}, {5,"E-216"}, 
    {5,"E-215"}, {5,"E-214"}, {5,"E-213"}, {5,"E-212"}, {5,"E-211"}, 
    {5,"E-210"}, {5,"E-209"}, {5,"E-208"}, {5,"E-207"}, {5,"E-206"}, 
    {5,"E-205"}, {5,"E-204"}, {5,"E-203"}, {5,"E-202"}, {5,"E-201"}, 
    {5,"E-200"}, {5,"E-199"}, {5,"E-198"}, {5,"E-197"}, {5,"E-196"}, 
    {5,"E-195"}, {5,"E-194"}, {5,"E-193"}, {5,"E-192"}, {5,"E-191"}, 
    {5,"E-190"}, {5,"E-189"}, {5,"E-188"}, {5,"E-187"}, {5,"E-186"}, 
    {5,"E-185"}, {5,"E-184"}, {5,"E-183"}, {5,"E-182"}, {5,"E-181"}, 
    {5,"E-180"}, {5,"E-179"}, {5,"E-178"}, {5,"E-177"}, {5,"E-176"}, 
    {5,"E-175"}, {5,"E-174"}, {5,"E-173"}, {5,"E-172"}, {5,"E-171"}, 
    {5,"E-170"}, {5,"E-169"}, {5,"E-168"}, {5,"E-167"}, {5,"E-166"}, 
    {5,"E-165"}, {5,"E-164"}, {5,"E-163"}, {5,"E-162"}, {5,"E-161"}, 
    {5,"E-160"}, {5,"E-159"}, {5,"E-158"}, {5,"E-157"}, {5,"E-156"}, 
    {5,"E-155"}, {5,"E-154"}, {5,"E-153"}, {5,"E-152"}, {5,"E-151"}, 
    {5,"E-150"}, {5,"E-149"}, {5,"E-148"}, {5,"E-147"}, {5,"E-146"}, 
    {5,"E-145"}, {5,"E-144"}, {5,"E-143"}, {5,"E-142"}, {5,"E-141"}, 
    {5,"E-140"}, {5,"E-139"}, {5,"E-138"}, {5,"E-137"}, {5,"E-136"}, 
    {5,"E-135"}, {5,"E-134"}, {5,"E-133"}, {5,"E-132"}, {5,"E-131"}, 
    {5,"E-130"}, {5,"E-129"}, {5,"E-128"}, {5,"E-127"}, {5,"E-126"}, 
    {5,"E-125"}, {5,"E-124"}, {5,"E-123"}, {5,"E-122"}, {5,"E-121"}, 
    {5,"E-120"}, {5,"E-119"}, {5,"E-118"}, {5,"E-117"}, {5,"E-116"}, 
    {5,"E-115"}, {5,"E-114"}, {5,"E-113"}, {5,"E-112"}, {5,"E-111"}, 
    {5,"E-110"}, {5,"E-109"}, {5,"E-108"}, {5,"E-107"}, {5,"E-106"}, 
    {5,"E-105"}, {5,"E-104"}, {5,"E-103"}, {5,"E-102"}, {5,"E-101"}, 
    {5,"E-100"}, {4,"E-99"}, {4,"E-98"}, {4,"E-97"}, {4,"E-96"}, 
    {4,"E-95"}, {4,"E-94"}, {4,"E-93"}, {4,"E-92"}, {4,"E-91"}, 
    {4,"E-90"}, {4,"E-89"}, {4,"E-88"}, {4,"E-87"}, {4,"E-86"}, 
    {4,"E-85"}, {4,"E-84"}, {4,"E-83"}, {4,"E-82"}, {4,"E-81"}, 
    {4,"E-80"}, {4,"E-79"}, {4,"E-78"}, {4,"E-77"}, {4,"E-76"}, 
    {4,"E-75"}, {4,"E-74"}, {4,"E-73"}, {4,"E-72"}, {4,"E-71"}, 
    {4,"E-70"}, {4,"E-69"}, {4,"E-68"}, {4,"E-67"}, {4,"E-66"}, 
    {4,"E-65"}, {4,"E-64"}, {4,"E-63"}, {4,"E-62"}, {4,"E-61"}, 
    {4,"E-60"}, {4,"E-59"}, {4,"E-58"}, {4,"E-57"}, {4,"E-56"}, 
    {4,"E-55"}, {4,"E-54"}, {4,"E-53"}, {4,"E-52"}, {4,"E-51"}, 
    {4,"E-50"}, {4,"E-49"}, {4,"E-48"}, {4,"E-47"}, {4,"E-46"}, 
    {4,"E-45"}, {4,"E-44"}, {4,"E-43"}, {4,"E-42"}, {4,"E-41"}, 
    {4,"E-40"}, {4,"E-39"}, {4,"E-38"}, {4,"E-37"}, {4,"E-36"}, 
    {4,"E-35"}, {4,"E-34"}, {4,"E-33"}, {4,"E-32"}, {4,"E-31"}, 
    {4,"E-30"}, {4,"E-29"}, {4,"E-28"}, {4,"E-27"}, {4,"E-26"}, 
    {4,"E-25"}, {4,"E-24"}, {4,"E-23"}, {4,"E-22"}, {4,"E-21"}, 
    {4,"E-20"}, {4,"E-19"}, {4,"E-18"}, {4,"E-17"}, {4,"E-16"}, 
    {4,"E-15"}, {4,"E-14"}, {4,"E-13"}, {4,"E-12"}, {4,"E-11"}, 
    {4,"E-10"}, {3,"E-9"}, {3,"E-8"}, {3,"E-7"}, {3,"E-6"}, 
    {3,"E-5"}, {3,"E-4"}, {3,"E-3"}, {3,"E-2"}, {3,"E-1"},
    
    {3,"E+0"}, {3,"E+1"}, {3,"E+2"}, {3,"E+3"}, {3,"E+4"}, 
    {3,"E+5"}, {3,"E+6"}, {3,"E+7"}, {3,"E+8"}, {3,"E+9"}, 
    {4,"E+10"}, {4,"E+11"}, {4,"E+12"}, {4,"E+13"}, {4,"E+14"}, 
    {4,"E+15"}, {4,"E+16"}, {4,"E+17"}, {4,"E+18"}, {4,"E+19"}, 
    {4,"E+20"}, {4,"E+21"}, {4,"E+22"}, {4,"E+23"}, {4,"E+24"}, 
    {4,"E+25"}, {4,"E+26"}, {4,"E+27"}, {4,"E+28"}, {4,"E+29"}, 
    {4,"E+30"}, {4,"E+31"}, {4,"E+32"}, {4,"E+33"}, {4,"E+34"}, 
    {4,"E+35"}, {4,"E+36"}, {4,"E+37"}, {4,"E+38"}, {4,"E+39"}, 
    {4,"E+40"}, {4,"E+41"}, {4,"E+42"}, {4,"E+43"}, {4,"E+44"}, 
    {4,"E+45"}, {4,"E+46"}, {4,"E+47"}, {4,"E+48"}, {4,"E+49"}, 
    {4,"E+50"}, {4,"E+51"}, {4,"E+52"}, {4,"E+53"}, {4,"E+54"}, 
    {4,"E+55"}, {4,"E+56"}, {4,"E+57"}, {4,"E+58"}, {4,"E+59"}, 
    {4,"E+60"}, {4,"E+61"}, {4,"E+62"}, {4,"E+63"}, {4,"E+64"}, 
    {4,"E+65"}, {4,"E+66"}, {4,"E+67"}, {4,"E+68"}, {4,"E+69"}, 
    {4,"E+70"}, {4,"E+71"}, {4,"E+72"}, {4,"E+73"}, {4,"E+74"}, 
    {4,"E+75"}, {4,"E+76"}, {4,"E+77"}, {4,"E+78"}, {4,"E+79"}, 
    {4,"E+80"}, {4,"E+81"}, {4,"E+82"}, {4,"E+83"}, {4,"E+84"}, 
    {4,"E+85"}, {4,"E+86"}, {4,"E+87"}, {4,"E+88"}, {4,"E+89"}, 
    {4,"E+90"}, {4,"E+91"}, {4,"E+92"}, {4,"E+93"}, {4,"E+94"}, 
    {4,"E+95"}, {4,"E+96"}, {4,"E+97"}, {4,"E+98"}, {4,"E+99"}, 
    {5,"E+100"}, {5,"E+101"}, {5,"E+102"}, {5,"E+103"}, {5,"E+104"}, 
    {5,"E+105"}, {5,"E+106"}, {5,"E+107"}, {5,"E+108"}, {5,"E+109"}, 
    {5,"E+110"}, {5,"E+111"}, {5,"E+112"}, {5,"E+113"}, {5,"E+114"}, 
    {5,"E+115"}, {5,"E+116"}, {5,"E+117"}, {5,"E+118"}, {5,"E+119"}, 
    {5,"E+120"}, {5,"E+121"}, {5,"E+122"}, {5,"E+123"}, {5,"E+124"}, 
    {5,"E+125"}, {5,"E+126"}, {5,"E+127"}, {5,"E+128"}, {5,"E+129"}, 
    {5,"E+130"}, {5,"E+131"}, {5,"E+132"}, {5,"E+133"}, {5,"E+134"}, 
    {5,"E+135"}, {5,"E+136"}, {5,"E+137"}, {5,"E+138"}, {5,"E+139"}, 
    {5,"E+140"}, {5,"E+141"}, {5,"E+142"}, {5,"E+143"}, {5,"E+144"}, 
    {5,"E+145"}, {5,"E+146"}, {5,"E+147"}, {5,"E+148"}, {5,"E+149"}, 
    {5,"E+150"}, {5,"E+151"}, {5,"E+152"}, {5,"E+153"}, {5,"E+154"}, 
    {5,"E+155"}, {5,"E+156"}, {5,"E+157"}, {5,"E+158"}, {5,"E+159"}, 
    {5,"E+160"}, {5,"E+161"}, {5,"E+162"}, {5,"E+163"}, {5,"E+164"}, 
    {5,"E+165"}, {5,"E+166"}, {5,"E+167"}, {5,"E+168"}, {5,"E+169"}, 
    {5,"E+170"}, {5,"E+171"}, {5,"E+172"}, {5,"E+173"}, {5,"E+174"}, 
    {5,"E+175"}, {5,"E+176"}, {5,"E+177"}, {5,"E+178"}, {5,"E+179"}, 
    {5,"E+180"}, {5,"E+181"}, {5,"E+182"}, {5,"E+183"}, {5,"E+184"}, 
    {5,"E+185"}, {5,"E+186"}, {5,"E+187"}, {5,"E+188"}, {5,"E+189"}, 
    {5,"E+190"}, {5,"E+191"}, {5,"E+192"}, {5,"E+193"}, {5,"E+194"}, 
    {5,"E+195"}, {5,"E+196"}, {5,"E+197"}, {5,"E+198"}, {5,"E+199"}, 
    {5,"E+200"}, {5,"E+201"}, {5,"E+202"}, {5,"E+203"}, {5,"E+204"}, 
    {5,"E+205"}, {5,"E+206"}, {5,"E+207"}, {5,"E+208"}, {5,"E+209"}, 
    {5,"E+210"}, {5,"E+211"}, {5,"E+212"}, {5,"E+213"}, {5,"E+214"}, 
    {5,"E+215"}, {5,"E+216"}, {5,"E+217"}, {5,"E+218"}, {5,"E+219"}, 
    {5,"E+220"}, {5,"E+221"}, {5,"E+222"}, {5,"E+223"}, {5,"E+224"}, 
    {5,"E+225"}, {5,"E+226"}, {5,"E+227"}, {5,"E+228"}, {5,"E+229"}, 
    {5,"E+230"}, {5,"E+231"}, {5,"E+232"}, {5,"E+233"}, {5,"E+234"}, 
    {5,"E+235"}, {5,"E+236"}, {5,"E+237"}, {5,"E+238"}, {5,"E+239"}, 
    {5,"E+240"}, {5,"E+241"}, {5,"E+242"}, {5,"E+243"}, {5,"E+244"}, 
    {5,"E+245"}, {5,"E+246"}, {5,"E+247"}, {5,"E+248"}, {5,"E+249"}, 
    {5,"E+250"}, {5,"E+251"}, {5,"E+252"}, {5,"E+253"}, {5,"E+254"}, 
    {5,"E+255"}, {5,"E+256"}, {5,"E+257"}, {5,"E+258"}, {5,"E+259"}, 
    {5,"E+260"}, {5,"E+261"}, {5,"E+262"}, {5,"E+263"}, {5,"E+264"}, 
    {5,"E+265"}, {5,"E+266"}, {5,"E+267"}, {5,"E+268"}, {5,"E+269"}, 
    {5,"E+270"}, {5,"E+271"}, {5,"E+272"}, {5,"E+273"}, {5,"E+274"}, 
    {5,"E+275"}, {5,"E+276"}, {5,"E+277"}, {5,"E+278"}, {5,"E+279"}, 
    {5,"E+280"}, {5,"E+281"}, {5,"E+282"}, {5,"E+283"}, {5,"E+284"}, 
    {5,"E+285"}, {5,"E+286"}, {5,"E+287"}, {5,"E+288"}, {5,"E+289"}, 
    {5,"E+290"}, {5,"E+291"}, {5,"E+292"}, {5,"E+293"}, {5,"E+294"}, 
    {5,"E+295"}, {5,"E+296"}, {5,"E+297"}, {5,"E+298"}, {5,"E+299"}, 
    {5,"E+300"}, {5,"E+301"}, {5,"E+302"}, {5,"E+303"}, {5,"E+304"}, 
    {5,"E+305"}, {5,"E+306"}, {5,"E+307"}, {5,"E+308"}, {5,"E+309"}, 
    {5,"E+310"}, {5,"E+311"}, {5,"E+312"}, {5,"E+313"}, {5,"E+314"}, 
    {5,"E+315"}, {5,"E+316"}, {5,"E+317"}, {5,"E+318"}, {5,"E+319"},
    {5,"E+320"}, {5,"E+321"}, {5,"E+322"}, {5,"E+323"}, {5,"E+324"}
};


const dtlNumericDigitString  * dtdNumericPosDigitString = dtdNumericDigitStringArr[1];
const dtlNumericDigitString  * dtdNumericNegDigitString = dtdNumericDigitStringArr[0];

const dtlExponentString  * dtdNumberExponentString = & dtdNumberExponentStringArr[325];

const dtlNumericCarryInfo dtdNumericInvalidCarryInfo[1] = { {0,0} };

const dtlNumericCarryInfo dtdNumericDigitMap[2][2][203] =
{
    /**
     * Negative & Negative
     */
    
    {    
        {
            /* +0 ~ +1 : round num2 (false, true) */
            {0,0}, {-1,0},
        
            /* +2 ~ +11 */
            {-1,1}, {-1,2}, {-1,3}, {-1,4}, {-1,5}, {-1,6}, {-1,7}, {-1,8}, {-1,9}, {-1,10},

            /* +12 ~ +21 */
            {-1,11}, {-1,12}, {-1,13}, {-1,14}, {-1,15}, {-1,16}, {-1,17}, {-1,18}, {-1,19}, {-1,20},

            /* +22 ~ +31 */
            {-1,21}, {-1,22}, {-1,23}, {-1,24}, {-1,25}, {-1,26}, {-1,27}, {-1,28}, {-1,29}, {-1,30},

            /* +32 ~ +41 */
            {-1,31}, {-1,32}, {-1,33}, {-1,34}, {-1,35}, {-1,36}, {-1,37}, {-1,38}, {-1,39}, {-1,40},

            /* +42 ~ +51 */
            {-1,41}, {-1,42}, {-1,43}, {-1,44}, {-1,45}, {-1,46}, {-1,47}, {-1,48}, {-1,49}, {-1,50},

            /* +52 ~ +61 */
            {-1,51}, {-1,52}, {-1,53}, {-1,54}, {-1,55}, {-1,56}, {-1,57}, {-1,58}, {-1,59}, {-1,60},

            /* +62 ~ +71 */
            {-1,61}, {-1,62}, {-1,63}, {-1,64}, {-1,65}, {-1,66}, {-1,67}, {-1,68}, {-1,69}, {-1,70},

            /* +72 ~ +81 */
            {-1,71}, {-1,72}, {-1,73}, {-1,74}, {-1,75}, {-1,76}, {-1,77}, {-1,78}, {-1,79}, {-1,80},

            /* +82 ~ +91 */
            {-1,81}, {-1,82}, {-1,83}, {-1,84}, {-1,85}, {-1,86}, {-1,87}, {-1,88}, {-1,89}, {-1,90},

            /* +92 ~ +101 */
            {-1,91}, {-1,92}, {-1,93}, {-1,94}, {-1,95}, {-1,96}, {-1,97}, {-1,98}, {-1,99}, {-1,100},

            /* +102 ~ +111 */
            {-1,101}, {0,2}, {0,3}, {0,4}, {0,5}, {0,6}, {0,7}, {0,8}, {0,9}, {0,10},

            /* +112 ~ +121 */
            {0,11}, {0,12}, {0,13}, {0,14}, {0,15}, {0,16}, {0,17}, {0,18}, {0,19}, {0,20},

            /* +122 ~ +131 */
            {0,21}, {0,22}, {0,23}, {0,24}, {0,25}, {0,26}, {0,27}, {0,28}, {0,29}, {0,30},

            /* +132 ~ +141 */
            {0,31}, {0,32}, {0,33}, {0,34}, {0,35}, {0,36}, {0,37}, {0,38}, {0,39}, {0,40},

            /* +142 ~ +151 */
            {0,41}, {0,42}, {0,43}, {0,44}, {0,45}, {0,46}, {0,47}, {0,48}, {0,49}, {0,50},

            /* +152 ~ +161 */
            {0,51}, {0,52}, {0,53}, {0,54}, {0,55}, {0,56}, {0,57}, {0,58}, {0,59}, {0,60},

            /* +162 ~ +171 */
            {0,61}, {0,62}, {0,63}, {0,64}, {0,65}, {0,66}, {0,67}, {0,68}, {0,69}, {0,70},

            /* +172 ~ +181 */
            {0,71}, {0,72}, {0,73}, {0,74}, {0,75}, {0,76}, {0,77}, {0,78}, {0,79}, {0,80},

            /* +182 ~ +191 */
            {0,81}, {0,82}, {0,83}, {0,84}, {0,85}, {0,86}, {0,87}, {0,88}, {0,89}, {0,90},

            /* +192 ~ +201 */
            {0,91}, {0,92}, {0,93}, {0,94}, {0,95}, {0,96}, {0,97}, {0,98}, {0,99}, {0,100},

            /* +202 */
            {0,101}
        },

    
        /**
         * Negative & Positive
         */
        {
            /* +0 ~ +1 : round num2 (false, true) */
            {0,0}, {1,0},
        
            /* +2 ~ +11 */
            {0,1}, {0,2}, {0,3}, {0,4}, {0,5}, {0,6}, {0,7}, {0,8}, {0,9}, {0,10},

            /* +12 ~ +21 */
            {0,11}, {0,12}, {0,13}, {0,14}, {0,15}, {0,16}, {0,17}, {0,18}, {0,19}, {0,20},

            /* +22 ~ +31 */
            {0,21}, {0,22}, {0,23}, {0,24}, {0,25}, {0,26}, {0,27}, {0,28}, {0,29}, {0,30},

            /* +32 ~ +41 */
            {0,31}, {0,32}, {0,33}, {0,34}, {0,35}, {0,36}, {0,37}, {0,38}, {0,39}, {0,40},

            /* +42 ~ +51 */
            {0,41}, {0,42}, {0,43}, {0,44}, {0,45}, {0,46}, {0,47}, {0,48}, {0,49}, {0,50},

            /* +52 ~ +61 */
            {0,51}, {0,52}, {0,53}, {0,54}, {0,55}, {0,56}, {0,57}, {0,58}, {0,59}, {0,60},

            /* +62 ~ +71 */
            {0,61}, {0,62}, {0,63}, {0,64}, {0,65}, {0,66}, {0,67}, {0,68}, {0,69}, {0,70},

            /* +72 ~ +81 */
            {0,71}, {0,72}, {0,73}, {0,74}, {0,75}, {0,76}, {0,77}, {0,78}, {0,79}, {0,80},

            /* +82 ~ +91 */
            {0,81}, {0,82}, {0,83}, {0,84}, {0,85}, {0,86}, {0,87}, {0,88}, {0,89}, {0,90},

            /* +92 ~ +101 */
            {0,91}, {0,92}, {0,93}, {0,94}, {0,95}, {0,96}, {0,97}, {0,98}, {0,99}, {0,100},

            /* +102 ~ +111 */
            {0,101}, {1,2}, {1,3}, {1,4}, {1,5}, {1,6}, {1,7}, {1,8}, {1,9}, {1,10},

            /* +112 ~ +121 */
            {1,11}, {1,12}, {1,13}, {1,14}, {1,15}, {1,16}, {1,17}, {1,18}, {1,19}, {1,20},

            /* +122 ~ +131 */
            {1,21}, {1,22}, {1,23}, {1,24}, {1,25}, {1,26}, {1,27}, {1,28}, {1,29}, {1,30},

            /* +132 ~ +141 */
            {1,31}, {1,32}, {1,33}, {1,34}, {1,35}, {1,36}, {1,37}, {1,38}, {1,39}, {1,40},

            /* +142 ~ +151 */
            {1,41}, {1,42}, {1,43}, {1,44}, {1,45}, {1,46}, {1,47}, {1,48}, {1,49}, {1,50},

            /* +152 ~ +161 */
            {1,51}, {1,52}, {1,53}, {1,54}, {1,55}, {1,56}, {1,57}, {1,58}, {1,59}, {1,60},

            /* +162 ~ +171 */
            {1,61}, {1,62}, {1,63}, {1,64}, {1,65}, {1,66}, {1,67}, {1,68}, {1,69}, {1,70},

            /* +172 ~ +181 */
            {1,71}, {1,72}, {1,73}, {1,74}, {1,75}, {1,76}, {1,77}, {1,78}, {1,79}, {1,80},

            /* +182 ~ +191 */
            {1,81}, {1,82}, {1,83}, {1,84}, {1,85}, {1,86}, {1,87}, {1,88}, {1,89}, {1,90},

            /* +192 ~ +201 */
            {1,91}, {1,92}, {1,93}, {1,94}, {1,95}, {1,96}, {1,97}, {1,98}, {1,99}, {1,100},

            /* +202 */
            {1,101}
        }
    },

    
    /**
     * Positive & Negative
     */
    {
        {
            /* +0 ~ +1 : round num2 (false, true) */
            {0,0}, {-1,0},
        
            /* +2 ~ +11 */
            {-1,1}, {-1,2}, {-1,3}, {-1,4}, {-1,5}, {-1,6}, {-1,7}, {-1,8}, {-1,9}, {-1,10},

            /* +12 ~ +21 */
            {-1,11}, {-1,12}, {-1,13}, {-1,14}, {-1,15}, {-1,16}, {-1,17}, {-1,18}, {-1,19}, {-1,20},

            /* +22 ~ +31 */
            {-1,21}, {-1,22}, {-1,23}, {-1,24}, {-1,25}, {-1,26}, {-1,27}, {-1,28}, {-1,29}, {-1,30},

            /* +32 ~ +41 */
            {-1,31}, {-1,32}, {-1,33}, {-1,34}, {-1,35}, {-1,36}, {-1,37}, {-1,38}, {-1,39}, {-1,40},

            /* +42 ~ +51 */
            {-1,41}, {-1,42}, {-1,43}, {-1,44}, {-1,45}, {-1,46}, {-1,47}, {-1,48}, {-1,49}, {-1,50},

            /* +52 ~ +61 */
            {-1,51}, {-1,52}, {-1,53}, {-1,54}, {-1,55}, {-1,56}, {-1,57}, {-1,58}, {-1,59}, {-1,60},

            /* +62 ~ +71 */
            {-1,61}, {-1,62}, {-1,63}, {-1,64}, {-1,65}, {-1,66}, {-1,67}, {-1,68}, {-1,69}, {-1,70},

            /* +72 ~ +81 */
            {-1,71}, {-1,72}, {-1,73}, {-1,74}, {-1,75}, {-1,76}, {-1,77}, {-1,78}, {-1,79}, {-1,80},

            /* +82 ~ +91 */
            {-1,81}, {-1,82}, {-1,83}, {-1,84}, {-1,85}, {-1,86}, {-1,87}, {-1,88}, {-1,89}, {-1,90},

            /* +92 ~ +101 */
            {-1,91}, {-1,92}, {-1,93}, {-1,94}, {-1,95}, {-1,96}, {-1,97}, {-1,98}, {-1,99}, {-1,100},

            /* +102 ~ +111 */
            {0,1}, {0,2}, {0,3}, {0,4}, {0,5}, {0,6}, {0,7}, {0,8}, {0,9}, {0,10},

            /* +112 ~ +121 */
            {0,11}, {0,12}, {0,13}, {0,14}, {0,15}, {0,16}, {0,17}, {0,18}, {0,19}, {0,20},

            /* +122 ~ +131 */
            {0,21}, {0,22}, {0,23}, {0,24}, {0,25}, {0,26}, {0,27}, {0,28}, {0,29}, {0,30},

            /* +132 ~ +141 */
            {0,31}, {0,32}, {0,33}, {0,34}, {0,35}, {0,36}, {0,37}, {0,38}, {0,39}, {0,40},

            /* +142 ~ +151 */
            {0,41}, {0,42}, {0,43}, {0,44}, {0,45}, {0,46}, {0,47}, {0,48}, {0,49}, {0,50},

            /* +152 ~ +161 */
            {0,51}, {0,52}, {0,53}, {0,54}, {0,55}, {0,56}, {0,57}, {0,58}, {0,59}, {0,60},

            /* +162 ~ +171 */
            {0,61}, {0,62}, {0,63}, {0,64}, {0,65}, {0,66}, {0,67}, {0,68}, {0,69}, {0,70},

            /* +172 ~ +181 */
            {0,71}, {0,72}, {0,73}, {0,74}, {0,75}, {0,76}, {0,77}, {0,78}, {0,79}, {0,80},

            /* +182 ~ +191 */
            {0,81}, {0,82}, {0,83}, {0,84}, {0,85}, {0,86}, {0,87}, {0,88}, {0,89}, {0,90},

            /* +192 ~ +201 */
            {0,91}, {0,92}, {0,93}, {0,94}, {0,95}, {0,96}, {0,97}, {0,98}, {0,99}, {0,100},

            /* +202 */
            {0,0}
        },

    
        /**
         * Positive & Positive
         */

        {
            /* +0 ~ +1 : round num2 (false, true) */
            {0,0}, {1,0},
        
            /* +2 ~ +11 */
            {0,1}, {0,2}, {0,3}, {0,4}, {0,5}, {0,6}, {0,7}, {0,8}, {0,9}, {0,10},

            /* +12 ~ +21 */
            {0,11}, {0,12}, {0,13}, {0,14}, {0,15}, {0,16}, {0,17}, {0,18}, {0,19}, {0,20},

            /* +22 ~ +31 */
            {0,21}, {0,22}, {0,23}, {0,24}, {0,25}, {0,26}, {0,27}, {0,28}, {0,29}, {0,30},

            /* +32 ~ +41 */
            {0,31}, {0,32}, {0,33}, {0,34}, {0,35}, {0,36}, {0,37}, {0,38}, {0,39}, {0,40},

            /* +42 ~ +51 */
            {0,41}, {0,42}, {0,43}, {0,44}, {0,45}, {0,46}, {0,47}, {0,48}, {0,49}, {0,50},

            /* +52 ~ +61 */
            {0,51}, {0,52}, {0,53}, {0,54}, {0,55}, {0,56}, {0,57}, {0,58}, {0,59}, {0,60},

            /* +62 ~ +71 */
            {0,61}, {0,62}, {0,63}, {0,64}, {0,65}, {0,66}, {0,67}, {0,68}, {0,69}, {0,70},

            /* +72 ~ +81 */
            {0,71}, {0,72}, {0,73}, {0,74}, {0,75}, {0,76}, {0,77}, {0,78}, {0,79}, {0,80},

            /* +82 ~ +91 */
            {0,81}, {0,82}, {0,83}, {0,84}, {0,85}, {0,86}, {0,87}, {0,88}, {0,89}, {0,90},

            /* +92 ~ +101 */
            {0,91}, {0,92}, {0,93}, {0,94}, {0,95}, {0,96}, {0,97}, {0,98}, {0,99}, {0,100},

            /* +102 ~ +111 */
            {1,1}, {1,2}, {1,3}, {1,4}, {1,5}, {1,6}, {1,7}, {1,8}, {1,9}, {1,10},

            /* +112 ~ +121 */
            {1,11}, {1,12}, {1,13}, {1,14}, {1,15}, {1,16}, {1,17}, {1,18}, {1,19}, {1,20},

            /* +122 ~ +131 */
            {1,21}, {1,22}, {1,23}, {1,24}, {1,25}, {1,26}, {1,27}, {1,28}, {1,29}, {1,30},

            /* +132 ~ +141 */
            {1,31}, {1,32}, {1,33}, {1,34}, {1,35}, {1,36}, {1,37}, {1,38}, {1,39}, {1,40},

            /* +142 ~ +151 */
            {1,41}, {1,42}, {1,43}, {1,44}, {1,45}, {1,46}, {1,47}, {1,48}, {1,49}, {1,50},

            /* +152 ~ +161 */
            {1,51}, {1,52}, {1,53}, {1,54}, {1,55}, {1,56}, {1,57}, {1,58}, {1,59}, {1,60},

            /* +162 ~ +171 */
            {1,61}, {1,62}, {1,63}, {1,64}, {1,65}, {1,66}, {1,67}, {1,68}, {1,69}, {1,70},

            /* +172 ~ +181 */
            {1,71}, {1,72}, {1,73}, {1,74}, {1,75}, {1,76}, {1,77}, {1,78}, {1,79}, {1,80},

            /* +182 ~ +191 */
            {1,81}, {1,82}, {1,83}, {1,84}, {1,85}, {1,86}, {1,87}, {1,88}, {1,89}, {1,90},

            /* +192 ~ +201 */
            {1,91}, {1,92}, {1,93}, {1,94}, {1,95}, {1,96}, {1,97}, {1,98}, {1,99}, {1,100},

            /* +202 */
            {0,0}
        }
    }
};

const stlUInt8 dtlNumericZero[2] =
{
    DTL_NUMERIC_DIGIT_NEGATIVE_FENCE_VALUE,
    DTL_NUMERIC_DIGIT_POSITIVE_FENCE_VALUE
};

const dtlCompareResult dtlNumericCompResult[2] =
{
    DTL_COMPARISON_GREATER,
    DTL_COMPARISON_LESS
};


/**
 * @brief dtlDataType's value의 byte length 얻어오기 (dtlDataValue의 mValue의 크기)
 * @param[in]  aType       dtlDataType
 * @param[in]  aPrecision  각 데이타 타입의 precision
 * @param[in]  aStringUnit string length unit (dtlStringLengthUnit 참조) 
 * @param[out] aLength     value의 byte length
 * @param[in]  aVectorFunc Function Vector
 * @param[in]  aVectorArg  Vector Argument
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtlGetDataValueBufferSize( dtlDataType           aType,
                                     stlInt64              aPrecision,
                                     dtlStringLengthUnit   aStringUnit,
                                     stlInt64            * aLength,
                                     dtlFuncVector       * aVectorFunc,
                                     void                * aVectorArg,
                                     stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( ( aType >= 0 ) &&
                        ( aType < DTL_TYPE_MAX ),
                        aErrorStack );
    
    STL_TRY( dtdGetLengthFuncList[aType] ( aPrecision,
                                           aStringUnit,
                                           aLength,
                                           aVectorFunc,
                                           aVectorArg,
                                           aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 입력 문자열을 통해 dtlDataType's value의 byte length 얻어오기 (dtlDataValue의 mValue의 크기)
 * dtlDataValue의 mLength의 정확한 mValue의 공간을 할당 받을시 사용
 * @param[in]  aType         dtlDataType
 * @param[in]  aPrecision    각 데이타 타입의 precision
 * @param[in]  aScale        scale 정보
 * @param[in]  aStringUnit   string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aString       입력 문자열
 * @param[in]  aStringLength aString의 길이    
 * @param[out] aLength       Char의 byte length
 * @param[in]  aVectorFunc   Function Vector
 * @param[in]  aVectorArg    Vector Argument
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtlGetDataValueBufferSizeFromString( dtlDataType           aType,
                                               stlInt64              aPrecision,
                                               stlInt64              aScale,
                                               dtlStringLengthUnit   aStringUnit,
                                               stlChar             * aString,
                                               stlInt64              aStringLength,
                                               stlInt64            * aLength,
                                               dtlFuncVector       * aVectorFunc,
                                               void                * aVectorArg,
                                               stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( ( aType >= 0 ) &&
                        ( aType < DTL_TYPE_MAX ),
                        aErrorStack );
    
    STL_TRY( dtdGetLengthFromStringFuncList[aType] ( aPrecision,
                                                     aScale,
                                                     aStringUnit,
                                                     aString,
                                                     aStringLength,
                                                     aLength,
                                                     aVectorFunc,
                                                     aVectorArg,
                                                     aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 입력 숫자를 통해 dtlDataType's value의 byte length 얻어오기 (dtlDataValue의 mValue의 크기)
 * dtlDataValue의 mLength의 정확한 mValue의 공간을 할당 받을시 사용 * 
 * @param[in]  aType       dtlDataType
 * @param[in]  aPrecision  각 데이타 타입의 precision
 * @param[in]  aScale      각 데이타 타입의 scale
 * @param[in]  aInteger    value에 저장될 입력 숫자
 * @param[out] aLength     value의 byte length
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtlGetDataValueBufferSizeFromInteger( dtlDataType       aType,
                                                stlInt64          aPrecision,
                                                stlInt64          aScale,
                                                stlInt64          aInteger, 
                                                stlInt64        * aLength,
                                                stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( ( aType >= 0 ) &&
                        ( aType < DTL_TYPE_MAX ),
                        aErrorStack );
    
    STL_TRY( dtdGetLengthFromIntegerFuncList[aType] ( aPrecision,
                                                      aScale,
                                                      aInteger,
                                                      aLength,
                                                      aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 입력 실수를 통해 dtlDataType's value의 byte length 얻어오기 (dtlDataValue의 mValue의 크기)
 * dtlDataValue의 mLength의 정확한 mValue의 공간을 할당 받을시 사용 * 
 * @param[in]  aType       dtlDataType
 * @param[in]  aPrecision  각 데이타 타입의 precision
 * @param[in]  aScale      각 데이타 타입의 scale
 * @param[in]  aReal       value에 저장될 입력 실수
 * @param[out] aLength     value의 byte length
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtlGetDataValueBufferSizeFromReal( dtlDataType       aType,
                                             stlInt64          aPrecision,
                                             stlInt64          aScale,
                                             stlFloat64        aReal, 
                                             stlInt64        * aLength,
                                             stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( ( aType >= 0 ) &&
                        ( aType < DTL_TYPE_MAX ),
                        aErrorStack );
    
    STL_TRY( dtdGetLengthFromRealFuncList[aType] ( aPrecision,
                                                   aScale,
                                                   aReal,
                                                   aLength,
                                                   aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief dtlDataType 구조체 초기화하기 (mValue는 초기화하지 않음, redirect 함수 아님)
 * @param[in]  aType       dtlDataType
 * @param[in]  aBufferSize Buffer 공간
 * @param[out] aValue      dtlDataValue의 주소
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtlInitDataValue( dtlDataType       aType,
                            stlInt32          aBufferSize,
                            dtlDataValue    * aValue,
                            stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( ( aType >= 0 ) &&
                        ( aType < DTL_TYPE_MAX ),
                        aErrorStack );

    STL_PARAM_VALIDATE( aValue != NULL, aErrorStack );
    
    aValue->mType   = aType;
    aValue->mLength = 0;
    aValue->mBufferSize = aBufferSize;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 입력 문자열을 통해 mValue를 구성 
 * @param[in]  aType              dtlDataType
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aPrecision         각 데이타 타입의 precision
 * @param[in]  aScale             각 데이타 타입의 scale
 * @param[in]  aStringUnit        string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator INTERVAL 타입의 Indicator
 *                                <BR> dtlIntervalIndicator 참조
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aSourceVectorFunc  Source Function Vector
 * @param[in]  aSourceVectorArg   Source Vector Argument
 * @param[in]  aDestVectorFunc    Dest Function Vector
 * @param[in]  aDestVectorArg     Dest Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtlSetValueFromString( dtlDataType           aType,
                                 stlChar             * aString,
                                 stlInt64              aStringLength,
                                 stlInt64              aPrecision,
                                 stlInt64              aScale,
                                 dtlStringLengthUnit   aStringUnit,
                                 dtlIntervalIndicator  aIntervalIndicator,
                                 stlInt64              aAvailableSize,
                                 dtlDataValue        * aValue,
                                 stlBool             * aSuccessWithInfo,
                                 dtlFuncVector       * aSourceVectorFunc,
                                 void                * aSourceVectorArg,
                                 dtlFuncVector       * aDestVectorFunc,
                                 void                * aDestVectorArg,
                                 stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( ( aType >= 0 ) &&
                        ( aType < DTL_TYPE_MAX ),
                        aErrorStack );
    
    STL_TRY( dtdSetValueFromStringFuncList[aType] ( aString,
                                                    aStringLength,
                                                    aPrecision,
                                                    aScale,
                                                    aStringUnit,
                                                    aIntervalIndicator,
                                                    aAvailableSize,
                                                    aValue,
                                                    aSuccessWithInfo,
                                                    aSourceVectorFunc,
                                                    aSourceVectorArg,
                                                    aDestVectorFunc,
                                                    aDestVectorArg,
                                                    aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 입력 숫자를 통해 mValue를 구성
 * @param[in]  aType              dtlDataType
 * @param[in]  aInteger           value에 저장될 입력 숫자
 * @param[in]  aPrecision         각 데이타 타입의 precision
 * @param[in]  aScale             각 데이타 타입의 scale
 * @param[in]  aStringUnit        string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator INTERVAL 타입의 Indicator
 *                           <BR> dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtlSetValueFromInteger( dtlDataType            aType,
                                  stlInt64               aInteger,
                                  stlInt64               aPrecision,
                                  stlInt64               aScale,
                                  dtlStringLengthUnit    aStringUnit,
                                  dtlIntervalIndicator   aIntervalIndicator,
                                  stlInt64               aAvailableSize,
                                  dtlDataValue         * aValue,
                                  stlBool              * aSuccessWithInfo,
                                  dtlFuncVector        * aVectorFunc,
                                  void                 * aVectorArg,
                                  stlErrorStack        * aErrorStack )
{
    STL_PARAM_VALIDATE( ( aType >= 0 ) &&
                        ( aType < DTL_TYPE_MAX ),
                        aErrorStack );
    
    STL_TRY( dtdSetValueFromIntegerFuncList[aType] ( aInteger,
                                                     aPrecision,
                                                     aScale,
                                                     aStringUnit,
                                                     aIntervalIndicator,
                                                     aAvailableSize,
                                                     aValue,
                                                     aSuccessWithInfo,
                                                     aVectorFunc,
                                                     aVectorArg,
                                                     aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 입력 실수를 통해 mValue를 구성
 * @param[in]  aType              dtlDataType
 * @param[in]  aReal              value에 저장될 입력 실수
 * @param[in]  aPrecision         각 데이타 타입의 precision
 * @param[in]  aScale             각 데이타 타입의 scale
 * @param[in]  aStringUnit        string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator INTERVAL 타입의 Indicator
 *                           <BR> dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtlSetValueFromReal( dtlDataType            aType,
                               stlFloat64             aReal,
                               stlInt64               aPrecision,
                               stlInt64               aScale,
                               dtlStringLengthUnit    aStringUnit,
                               dtlIntervalIndicator   aIntervalIndicator,
                               stlInt64               aAvailableSize,
                               dtlDataValue         * aValue,
                               stlBool              * aSuccessWithInfo,
                               dtlFuncVector        * aVectorFunc,
                               void                 * aVectorArg,
                               stlErrorStack        * aErrorStack )
{
    STL_PARAM_VALIDATE( ( aType >= 0 ) &&
                        ( aType < DTL_TYPE_MAX ),
                        aErrorStack );
    
    STL_TRY( dtdSetValueFromRealFuncList[aType] ( aReal,
                                                  aPrecision,
                                                  aScale,
                                                  aStringUnit,
                                                  aIntervalIndicator,
                                                  aAvailableSize,
                                                  aValue,
                                                  aSuccessWithInfo,
                                                  aVectorFunc,
                                                  aVectorArg,
                                                  aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief datetime의 typed literal 입력 문자열을 통해 mValue를 구성
 *    <BR> DATE'2014-10-27'
 * @param[in]  aType              dtlDataType
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이   
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtlSetDateTimeValueFromTypedLiteral( dtlDataType           aType,
                                               stlChar             * aString,
                                               stlInt64              aStringLength,
                                               stlInt64              aAvailableSize,
                                               dtlDataValue        * aValue,
                                               dtlFuncVector       * aVectorFunc,
                                               void                * aVectorArg,
                                               stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( ( aType == DTL_TYPE_DATE ) ||
                        ( aType == DTL_TYPE_TIME ) ||
                        ( aType == DTL_TYPE_TIME_WITH_TIME_ZONE ) ||
                        ( aType == DTL_TYPE_TIMESTAMP ) ||
                        ( aType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE ),
                        aErrorStack );
    
    STL_TRY( dtdDateTimeSetValueFromTypedLiteralFuncList[ aType ]( aString,
                                                                   aStringLength,
                                                                   aAvailableSize,
                                                                   aValue,
                                                                   aVectorFunc,
                                                                   aVectorArg,
                                                                   aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief dtlDataValue의 mValue의 Byte Order 변경
 * @param[in]  aType           dtlDataType
 * @param[in]  aValue          dtlDataValue의 mValue
 * @param[in]  aIsSameEndian   byte swapping이 필요한지 여부
 * @param[out] aErrorStack     에러 스택
 */
stlStatus dtlReverseByteOrderValue( dtlDataType       aType,
                                    void            * aValue,
                                    stlBool           aIsSameEndian,
                                    stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( ( aType >= 0 ) &&
                        ( aType < DTL_TYPE_MAX ),
                        aErrorStack );

    STL_PARAM_VALIDATE( aValue != NULL, aErrorStack );
    
    if( aIsSameEndian == STL_FALSE )
    {
        STL_TRY( dtdReverseByteOrderFuncList[aType] ( aValue,
                                                      aIsSameEndian,
                                                      aErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        /* Do Nothing */
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief dtlDataValue를 문자열로 변환
 * @param[in]  aValue          dtlDataValue
 * @param[in]  aPrecision      precision 정보 (현재는 interval type에서만 필요한 정보임.)
 * @param[in]  aScale          scale 정보 (현재는 interval type에서만 필요한 정보임.)
 * @param[in]  aAvailableSize  aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer         문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength         변환된 문자열의 길이
 * @param[in]  aVectorFunc     Function Vector
 * @param[in]  aVectorArg      Vector Argument
 * @param[out] aErrorStack     에러 스택
 * @remark dtlToStringDataValue 함수내에서 변환된 문자는 NULL 종료된 문자가 아니다.
 *      <BR> NULL 종료된 문자가 필요하다면 함수를 지원할 수 있음.
 */
stlStatus dtlToStringDataValue( dtlDataValue    * aValue,
                                stlInt64          aPrecision,
                                stlInt64          aScale,
                                stlInt64          aAvailableSize,
                                void            * aBuffer,
                                stlInt64        * aLength,
                                dtlFuncVector   * aVectorFunc,
                                void            * aVectorArg,
                                stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( aValue != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aValue->mValue != NULL, aErrorStack );
    STL_PARAM_VALIDATE( ( aValue->mType >= 0 ) &&
                        ( aValue->mType < DTL_TYPE_MAX ),
                        aErrorStack );
    
    STL_TRY( dtdToStringFuncList[aValue->mType] ( aValue,
                                                  aPrecision,
                                                  aScale,
                                                  aAvailableSize,
                                                  aBuffer,
                                                  aLength,
                                                  aVectorFunc,
                                                  aVectorArg,
                                                  aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief dtlDataValue를 사용가능한 버퍼사이즈에 맞게 잘라서 문자열로 변환한다.
 * @param[in]  aValue          dtlDataValue
 * @param[in]  aPrecision      precision (현재는 interval type에서만 필요한 정보임.)
 * @param[in]  aScale          scale (현재는 interval type에서만 필요한 정보임.)
 * @param[in]  aAvailableSize  aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer         문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength         변환된 문자열의 길이
 * @param[in]  aVectorFunc     Function Vector
 * @param[in]  aVectorArg      Vector Argument
 * @param[out] aErrorStack     에러 스택
 * @remark dtlToStringDataValue 함수내에서 변환된 문자는 NULL 종료된 문자가 아니다.
 *      <BR> NULL 종료된 문자가 필요하다면 함수를 지원할 수 있음.
 */
stlStatus dtlToStringForAvailableSizeDataValue( dtlDataValue    * aValue,
                                                stlInt64          aPrecision,
                                                stlInt64          aScale,
                                                stlInt64          aAvailableSize,
                                                void            * aBuffer,
                                                stlInt64        * aLength,
                                                dtlFuncVector   * aVectorFunc,
                                                void            * aVectorArg,
                                                stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( aValue != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aValue->mValue != NULL, aErrorStack );
    STL_PARAM_VALIDATE( ( aValue->mType >= 0 ) &&
                        ( aValue->mType < DTL_TYPE_MAX ),
                        aErrorStack );
    
    STL_TRY( dtdToStringForAvailableSizeFuncList[aValue->mType] ( aValue,
                                                                  aPrecision,
                                                                  aScale,
                                                                  aAvailableSize,
                                                                  aBuffer,
                                                                  aLength,
                                                                  aVectorFunc,
                                                                  aVectorArg,
                                                                  aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief data value를 unicode 문자열로 변환
 * @param[in]  aUnicodeEncoding  unicode encoding ( utf16 또는 utf32 )
 * @param[in]  aValue            dtlDataValue
 * @param[in]  aPrecision        precision
 * @param[in]  aScale            scale 정보
 * @param[in]  aAvailableSize    aBuffer에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aBuffer           문자열로 변환한 결과가 저장될 메모리 공간
 * @param[out] aLength           변환된 문자열의 길이
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlToWCharStringDataValue( dtlUnicodeEncoding    aUnicodeEncoding,
                                     dtlDataValue        * aValue,
                                     stlInt64              aPrecision,
                                     stlInt64              aScale,
                                     stlInt64              aAvailableSize,
                                     void                * aBuffer,
                                     stlInt64            * aLength,
                                     dtlFuncVector       * aVectorFunc,
                                     void                * aVectorArg,
                                     stlErrorStack       * aErrorStack )
{
    STL_PARAM_VALIDATE( aValue != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aValue->mValue != NULL, aErrorStack );
    STL_PARAM_VALIDATE( ( aValue->mType >= 0 ) &&
                        ( aValue->mType < DTL_TYPE_MAX ),
                        aErrorStack );
    
    STL_TRY( dtdToWCharStringFuncList[aValue->mType][aUnicodeEncoding]( aValue,
                                                                        aPrecision,
                                                                        aScale,
                                                                        aAvailableSize,
                                                                        aBuffer,
                                                                        aLength,
                                                                        aVectorFunc,
                                                                        aVectorArg,
                                                                        aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief 문자열을 BINARY STRING으로 변환 (BINARY, VARYING BINARY, LONG VARYING BINARY)
 *        16진수들을 10진수들로 변환 (redirect 함수 아님, 내부 함수)
 * @param[in]  aHex           16진수 배열
 * @param[in]  aHexLength     16진수 배열 길이
 * @param[in]  aAvailableSize 10진수 배열이 저장될 메모리 공간의 크기
 * @param[out] aNumber        10진수 배열
 * @param[out] aNumberLength  10진수 배열 길이
 * @param[out] aErrorStack    에러 스택
 *
 * 공백 제거하여 decode 수행.
 */
stlStatus dtlToBinaryStringFromString( stlChar         * aHex,
                                       stlInt64          aHexLength,
                                       stlInt64          aAvailableSize,
                                       stlInt8         * aNumber,
                                       stlInt64        * aNumberLength, 
                                       stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( aHex != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aNumber != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aNumberLength != NULL, aErrorStack );
    
    STL_TRY( dtdToBinaryStringFromString ( aHex,
                                           aHexLength,
                                           aAvailableSize,
                                           aNumber,
                                           aNumberLength,
                                           aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief 숫자문자열을 주어진 길이만큼 resizing 한다.
 * @param[in]  aNumberString         value
 * @param[in]  aNumberStringLength   value 길이
 * @param[in]  aAvailableSize        resizing 길이 
 * @param[out] aResult               resizing 된 문자열이 저장될 buffer
 * @param[out] aResultLength         resizing 된 문자열의 길이
 * @param[out] aErrorStack           에러 스택 
 */
stlStatus dtlResizeNumberStringToString( stlChar         * aNumberString,
                                         stlInt64          aNumberStringLength,
                                         stlInt64          aAvailableSize,
                                         stlChar         * aResult,
                                         stlInt64        * aResultLength,
                                         stlErrorStack   * aErrorStack )
{
    STL_PARAM_VALIDATE( aNumberString != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aNumberStringLength > 0, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize > 0, aErrorStack );
    STL_PARAM_VALIDATE( aResult != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aResultLength != NULL, aErrorStack );

    STL_TRY( dtdResizeNumberStringToString( aNumberString,
                                            aNumberStringLength,
                                            aAvailableSize,
                                            aResult,
                                            aResultLength,
                                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Float32 value로부터 변환된 문자열의 길이
 * @param[in]  aFloat32             Float32 value
 * @param[out] aIntegralLength      정수부분 문자열의 길이
 * @param[out] aEntirePrintLength   전체 숫자를 그대로 표현할때 문자열의 길이
 * @param[out] aExpPrintLength      전체 숫자를 exponent 형태로 표현할때  문자열의 길이
 * @param[out] aErrorStack          에러 스택
 */
stlStatus dtlGetStringLengthFromFloat32( stlFloat32      * aFloat32,
                                         stlInt64        * aIntegralLength,
                                         stlInt64        * aEntirePrintLength,
                                         stlInt64        * aExpPrintLength,
                                         stlErrorStack   * aErrorStack )
{
    return dtdGetStringLengthFromFloat32( aFloat32,
                                          aIntegralLength,
                                          aEntirePrintLength,
                                          aExpPrintLength,
                                          aErrorStack );
}

/**
 * @brief Float64 value로부터 변환된 문자열의 길이
 * @param[in]  aFloat64             Float64 value
 * @param[out] aIntegralLength      정수부분 문자열의 길이
 * @param[out] aEntirePrintLength   전체 숫자를 그대로 표현할때 문자열의 길이
 * @param[out] aExpPrintLength      전체 숫자를 exponent 형태로 표현할때  문자열의 길이
 * @param[out] aErrorStack          에러 스택
 */
stlStatus dtlGetStringLengthFromFloat64( stlFloat64      * aFloat64,
                                         stlInt64        * aIntegralLength,
                                         stlInt64        * aEntirePrintLength,
                                         stlInt64        * aExpPrintLength,
                                         stlErrorStack   * aErrorStack )
{
    return dtdGetStringLengthFromFloat64( aFloat64,
                                          aIntegralLength,
                                          aEntirePrintLength,
                                          aExpPrintLength,
                                          aErrorStack );
}

/**
 * @brief Float32 value로부터 유효한 숫자의 길이
 * @param[in]  aFloat32             Float32 value
 * @param[out] aValidNumberLength   유효한 숫자의 길의 ( 소수점 및 trailing zero 제외 )
 * @param[out] aErrorStack          에러 스택
 * @remark value 의 유효숫자중 소수점에 해당하는 부분을 제외한 부분을 유효한 숫자로 정의 하며 <br>
 *         trailing zero 있는 ( 정수타입 값 ) 경우 trailing zero 를 제외한 값
 */
stlStatus dtlGetValidNumberLengthFromFloat32( stlFloat32      * aFloat32,
                                              stlInt64        * aValidNumberLength,
                                              stlErrorStack   * aErrorStack )
{
    return dtdGetValidNumberLengthFromFloat32( aFloat32,
                                               aValidNumberLength,
                                               aErrorStack );
}

/**
 * @brief Float64 value로부터 유요한 숫자의 길이 
 * @param[in]  aFloat64             Float32 value
 * @param[out] aValidNumberLength   유효한 숫자의 길의 ( 소수점 및 trailing zero 제외 )
 * @param[out] aErrorStack          에러 스택
 * @remark value 의 유효숫자중 소수점에 해당하는 부분을 제외한 부분을 유효한 숫자로 정의 하며 <br>
 *         trailing zero 있는 ( 정수타입 값 ) 경우 trailing zero 를 제외한 값
 */
stlStatus dtlGetValidNumberLengthFromFloat64( stlFloat64      * aFloat64,
                                              stlInt64        * aValidNumberLength,
                                              stlErrorStack   * aErrorStack )
{
    return dtdGetValidNumberLengthFromFloat64( aFloat64,
                                               aValidNumberLength,
                                               aErrorStack );
}

/**
 * @brief 문자열로 부터 integer 값을 얻어냄
 * @param[in]  aString             입력 문자열
 * @param[in]  aStringLength       입력 문자열 길이
 * @param[out] aInteger            integer 변환 값
 * @param[out] aErrorStack         에러 스택
 */
stlStatus dtlGetIntegerFromString( stlChar        * aString,
                                   stlInt32         aStringLength,
                                   stlInt64       * aInteger,
                                   stlErrorStack  * aErrorStack )
{
    return dtdToInt64FromString( aString,
                                 aStringLength,
                                 aInteger,
                                 aErrorStack );
}


/**
 * @brief 문자열로부터 Numeric Value를 설정
 * @param[in]  aString             입력 문자열
 * @param[in]  aStringLength       입력 문자열 길이
 * @param[in]  aPrecision          dtlNumericType의 입력 precision
 * @param[in]  aScale              dtlNumericType의 입력 scale
 * @param[out] aNumeric            Numeric Value
 * @param[out] aErrorStack         에러 스택
 *
 * 앞뒤 공백만 제거하여 decode 수행.
 */
stlStatus dtlGetNumericFromString( stlChar         * aString,
                                   stlInt64          aStringLength,
                                   stlInt64          aPrecision,
                                   stlInt64          aScale,
                                   dtlDataValue    * aNumeric,
                                   stlErrorStack   * aErrorStack )
{
    return dtdToNumericFromString( aString,
                                   aStringLength,
                                   aPrecision,
                                   aScale,
                                   aNumeric,
                                   aErrorStack );
}


/**
 * @brief 문자열로 부터 integer 값을 얻어냄 (반올림하지 않음)
 * @param[in]  aString             입력 문자열
 * @param[in]  aStringLength       입력 문자열 길이
 * @param[out] aInteger            integer 변환 값
 * @param[out] aErrorStack         에러 스택
 */
stlStatus dtlGetIntegerFromStringWithoutRound( stlChar        * aString,
                                               stlInt32         aStringLength,
                                               stlInt64       * aInteger,
                                               stlErrorStack  * aErrorStack )
{
    return dtdToInt64FromStringWithoutRound( aString,
                                             aStringLength,
                                             aInteger,
                                             aErrorStack );
}


/**
 * @brief integer로부터 Boolean 타입으로 표현 가능한지 판단
 * @param[in]  aInteger            입력 숫자
 * @param[out] aIsValid            변환 가능 여부 
 * @param[out] aErrorStack         에러 스택
 */
stlStatus dtlValidateBoolean( stlInt64         aInteger,
                              stlBool        * aIsValid,
                              stlErrorStack  * aErrorStack )
{
    if( ( aInteger == STL_TRUE ) ||
        ( aInteger == STL_FALSE ) )
    {
        *aIsValid = STL_TRUE;
    }
    else
    {
        *aIsValid = STL_FALSE;
    }
    
    
    return STL_SUCCESS;
}


/**
 * @brief Float 문자열이 숫자형 타입으로 표현 가능한지 판단
 * @param[in]  aString             입력 문자열
 * @param[out] aIsValid            변환 가능 여부 
 * @param[out] aErrorStack         에러 스택
 */
void dtlValidateFloat( stlChar        * aString,
                       stlBool        * aIsValid,
                       stlErrorStack  * aErrorStack )
{
//    stlBool            sIsPositive;   /* asign만 하고 사용하지 않는 변수 주석처리 */
    stlChar          * sEndPtr;
    stlChar          * sFence;
    stlFloat64         sConvertedValue;
    stlBool            sIsValid;

    sIsValid    = STL_FALSE;
//    sIsPositive = STL_TRUE;
    sFence      = aString + stlStrlen( aString );

    while( aString < sFence )
    {
        if( *aString == '+' )
        {
//            sIsPositive = STL_TRUE;
            aString++;
            break;
        }
        else if( *aString == '-' )
        {
//            sIsPositive = STL_FALSE;
            aString++;
            break;
        }
        else if( (*aString == ' ')  ||
                 (*aString == '\n') ||
                 (*aString == '\t') ||
                 (*aString == '\r') )
        {
            aString++;
        }
        else
        {
            break;
        }
    }

    /* truncate leading spaces */
    while( aString < sFence )
    {
        if( (*aString == ' ')  ||
            (*aString == '\n') ||
            (*aString == '\t') ||
            (*aString == '\r') )
        {
            aString++;
        }
        else
        {
            break;
        }
    }

    /* re-arrange fence */
    while( sFence > aString )
    {
        if( (*sFence == ' ')  ||
            (*sFence == '\0') ||
            (*sFence == '\n') ||
            (*sFence == '\t') ||
            (*sFence == '\r') )
        {
            sFence--;
        }
        else
        {
            break;
        }
    }

    sFence++;
    
    /* string value로부터 Real 타입의 value 구성 */
    if( stlStrToFloat64( (stlChar *) aString,
                         STL_NTS,
                         & sEndPtr,
                         & sConvertedValue,
                         aErrorStack )
        == STL_SUCCESS )
    {
        if( ( sEndPtr == sFence ) &&
            ( stlIsfinite(sConvertedValue) != STL_IS_FINITE_FALSE ) )
        {
            sIsValid = STL_TRUE;
        }
        else
        {
            sIsValid = STL_FALSE;
        }
    }
    else
    {
        while( stlPopError( aErrorStack ) != NULL )
        {
            /* Do Nothing */
        }
         
        sIsValid = STL_FALSE;
    }

    *aIsValid = sIsValid;
}


/**
 * @brief Bit String 문자열이 Binary 타입으로 표현 가능한지 판단
 * @param[in]  aBitString          입력 문자열
 * @param[out] aIsValid            변환 가능 여부
 * @param[out] aPrecision          Binary 타입으로 변환됐을때의 precision
 * @param[out] aErrorStack         에러 스택
 */
stlStatus dtlValidateBitString( stlChar        * aBitString,
                                stlBool        * aIsValid,
                                stlInt64       * aPrecision,
                                stlErrorStack  * aErrorStack )
{
    stlInt64    sLength = 0;

    STL_PARAM_VALIDATE( aBitString != NULL, aErrorStack);

    if( dtdGetLengthOfDecodedHex( aBitString,
                                  stlStrlen( aBitString ),
                                  & sLength,
                                  aErrorStack )
        == STL_SUCCESS )
    {
        *aIsValid = STL_TRUE;
    }
    else
    {
        *aIsValid = STL_FALSE;
    }

    *aPrecision = sLength;

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief dtlDateType 에서 해당 field 값 반환
 * @param[in]  aType       dtl 타입 데이터
 * @param[out] aYear       year
 * @param[out] aMonth      month
 * @param[out] aDay        day
 * @param[out] aHour       hour
 * @param[out] aMinute     minute
 * @param[out] aSecond     second
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtlGetTimeInfoFromDate( dtlDateType   * aType,
                                  stlInt32      * aYear,
                                  stlInt32      * aMonth,
                                  stlInt32      * aDay,
                                  stlInt32      * aHour,
                                  stlInt32      * aMinute,
                                  stlInt32      * aSecond,                                  
                                  stlErrorStack * aErrorStack )
{
    dtlExpTime           sDtlExpTime;
    dtlFractionalSecond  sFractionalSecond = 0;
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    STL_TRY( dtdDateTime2dtlExpTime( *aType,
                                     & sDtlExpTime,
                                     & sFractionalSecond,
                                     aErrorStack )
             == STL_SUCCESS );
    
    STL_DASSERT( sFractionalSecond == 0 );
    
    *aYear = sDtlExpTime.mYear;
    *aMonth = sDtlExpTime.mMonth;
    *aDay = sDtlExpTime.mDay;
    *aHour = sDtlExpTime.mHour;
    *aMinute = sDtlExpTime.mMinute;
    *aSecond = sDtlExpTime.mSecond;
    
    if( *aYear > 0 )
    {
        // Do Nothing
    }
    else
    {    
        *aYear = *aYear - 1;
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief dtlTimeType 에서 해당 field 값 반환
 * @param[in]  aType       dtl 타입 데이터
 * @param[out] aHour       hour
 * @param[out] aMinute     minute
 * @param[out] aSecond     second
 * @param[out] aFraction   fraction
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtlGetTimeInfoFromTime( dtlTimeType   * aType,
                                  stlInt32      * aHour,
                                  stlInt32      * aMinute,
                                  stlInt32      * aSecond,
                                  stlInt32      * aFraction,
                                  stlErrorStack * aErrorStack )
{
    dtlTimeType         sTimeType;
    dtlExpTime          sDtlExpTime;
    dtlFractionalSecond sFractionalSecond;

    sTimeType = *aType;

    dtdTime2dtlExpTime( sTimeType,
                        & sDtlExpTime,
                        & sFractionalSecond );

    *aHour     = sDtlExpTime.mHour;
    *aMinute   = sDtlExpTime.mMinute;
    *aSecond   = sDtlExpTime.mSecond;
    *aFraction = sFractionalSecond;
    
    return STL_SUCCESS;
}

/**
 * @brief dtlTimeTzType 에서 해당 field 값 반환
 * @param[in]  aType            dtl 타입 데이터
 * @param[out] aHour            hour
 * @param[out] aMinute          minute
 * @param[out] aSecond          second
 * @param[out] aFraction        fraction
 * @param[out] aTimeZoneSign    timezone의 부호 ( '-' => 0, '+' => 1 )
 * @param[out] aTimeZoneHour    timezone의 시
 * @param[out] aTimeZoneMinute  timezone의 분 
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtlGetTimeInfoFromTimeTz( dtlTimeTzType   * aType,
                                    stlInt32        * aHour,
                                    stlInt32        * aMinute,
                                    stlInt32        * aSecond,
                                    stlInt32        * aFraction,
                                    stlInt32        * aTimeZoneSign,
                                    stlUInt32       * aTimeZoneHour,
                                    stlUInt32       * aTimeZoneMinute,
                                    stlErrorStack   * aErrorStack )
{
    dtlTimeTzType        sTimeTzType;
    dtlExpTime           sDtlExpTime;
    dtlFractionalSecond  sFractionalSecond;
    stlInt32             sTimeZone = 0;

    sTimeTzType = *aType;

    dtdTimeTz2dtlExpTime( & sTimeTzType,
                          & sDtlExpTime,
                          & sFractionalSecond,
                          & sTimeZone );

    *aHour     = sDtlExpTime.mHour;
    *aMinute   = sDtlExpTime.mMinute;
    *aSecond   = sDtlExpTime.mSecond;
    *aFraction = sFractionalSecond;

    if( sTimeZone <= 0 )
    {
        *aTimeZoneSign = 1;
        
        *aTimeZoneHour = (sTimeZone / DTL_SECS_PER_HOUR) * -1;
        sTimeZone = sTimeZone % DTL_SECS_PER_HOUR;
        *aTimeZoneMinute = (sTimeZone / DTL_SECS_PER_MINUTE ) * -1;
    }
    else
    {
        *aTimeZoneSign = 0;

        *aTimeZoneHour = (sTimeZone / DTL_SECS_PER_HOUR);
        sTimeZone = sTimeZone % DTL_SECS_PER_HOUR;
        *aTimeZoneMinute = (sTimeZone / DTL_SECS_PER_MINUTE );
    }    
    
    return STL_SUCCESS;
}

/**
 * @brief dtlTimestampType 에서 해당 field 값 반환
 * @param[in]  aType       dtl 타입 데이터
 * @param[out] aYear       year
 * @param[out] aMonth      month
 * @param[out] aDay        day
 * @param[out] aHour       hour
 * @param[out] aMinute     minute
 * @param[out] aSecond     second
 * @param[out] aFraction   fraction
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtlGetTimeInfoFromTimestamp( dtlTimestampType * aType,
                                       stlInt32         * aYear,
                                       stlInt32         * aMonth,
                                       stlInt32         * aDay,
                                       stlInt32         * aHour,
                                       stlInt32         * aMinute,
                                       stlInt32         * aSecond,
                                       stlInt32         * aFraction,
                                       stlErrorStack    * aErrorStack )
{
    dtlTimestampType    sTimestampType;
    dtlExpTime          sDtlExpTime;
    dtlFractionalSecond sFractionalSecond;

    sTimestampType = *aType;

    STL_TRY( dtdDateTime2dtlExpTime(sTimestampType,
                                    & sDtlExpTime,
                                    & sFractionalSecond,
                                    aErrorStack )
             == STL_SUCCESS );
    
    *aYear = sDtlExpTime.mYear;
    *aMonth = sDtlExpTime.mMonth;
    *aDay = sDtlExpTime.mDay;
    *aHour = sDtlExpTime.mHour;
    *aMinute = sDtlExpTime.mMinute;
    *aSecond = sDtlExpTime.mSecond;
    *aFraction = sFractionalSecond;

    if( *aYear > 0 )
    {
        // Do Nothing
    }
    else
    {    
        *aYear = *aYear - 1;
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief dtlTimestampTzType 에서 해당 field 값 반환
 * @param[in]  aType       dtl 타입 데이터
 * @param[out] aYear       year
 * @param[out] aMonth      month
 * @param[out] aDay        day
 * @param[out] aHour       hour
 * @param[out] aMinute     minute
 * @param[out] aSecond     second
 * @param[out] aFraction   fraction
 * @param[out] aTimeZoneSign    timezone의 부호 ( '-' => 0, '+' => 1 )
 * @param[out] aTimeZoneHour    timezone의 시
 * @param[out] aTimeZoneMinute  timezone의 분 
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtlGetTimeInfoFromTimestampTz( dtlTimestampTzType * aType,
                                         stlInt32           * aYear,
                                         stlInt32           * aMonth,
                                         stlInt32           * aDay,
                                         stlInt32           * aHour,
                                         stlInt32           * aMinute,
                                         stlInt32           * aSecond,
                                         stlInt32           * aFraction,
                                         stlInt32           * aTimeZoneSign,
                                         stlUInt32          * aTimeZoneHour,
                                         stlUInt32          * aTimeZoneMinute,
                                         stlErrorStack      * aErrorStack )
{
    dtlTimestampTzType    sTimestampTzType;
    dtlExpTime            sDtlExpTime;
    dtlFractionalSecond   sFractionalSecond;
    stlInt32              sTimeZone;

    sTimestampTzType.mTimestamp = aType->mTimestamp;
    sTimestampTzType.mTimeZone  = aType->mTimeZone;

    STL_TRY( dtdTimestampTz2dtlExpTime(sTimestampTzType,
                                       & sTimeZone,        /* aTimeZone */
                                       & sDtlExpTime,
                                       & sFractionalSecond,
                                       aErrorStack )
             == STL_SUCCESS );

    *aYear = sDtlExpTime.mYear;
    *aMonth = sDtlExpTime.mMonth;
    *aDay = sDtlExpTime.mDay;
    *aHour = sDtlExpTime.mHour;
    *aMinute = sDtlExpTime.mMinute;
    *aSecond = sDtlExpTime.mSecond;
    *aFraction = sFractionalSecond;

    if( *aYear > 0 )
    {
        // Do Nothing
    }
    else
    {    
        *aYear = *aYear - 1;
    }

    if( sTimeZone <= 0 )
    {
        *aTimeZoneSign = 1;
        
        *aTimeZoneHour = (sTimeZone / DTL_SECS_PER_HOUR) * -1;
        sTimeZone = sTimeZone % DTL_SECS_PER_HOUR;
        *aTimeZoneMinute = (sTimeZone / DTL_SECS_PER_MINUTE ) * -1;
    }
    else
    {
        *aTimeZoneSign = 0;

        *aTimeZoneHour = (sTimeZone / DTL_SECS_PER_HOUR);
        sTimeZone = sTimeZone % DTL_SECS_PER_HOUR;
        *aTimeZoneMinute = (sTimeZone / DTL_SECS_PER_MINUTE );
    }    
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief dtlIntervalYearToMonthType 에서 year_month field 값 반환
 * @param[in]  aType       dtlIntervalYearToMonthType 데이터
 * @param[out] aYear       year
 * @param[out] aMonth      month
 * @param[out] aIsNegative 음수 여부
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtlGetTimeInfoFromYearMonthInterval( dtlIntervalYearToMonthType * aType,
                                               stlUInt32                  * aYear,
                                               stlUInt32                  * aMonth,
                                               stlBool                    * aIsNegative,
                                               stlErrorStack              * aErrorStack )
{
    dtlIntervalYearToMonthType  sIntervalYearToMonthType;
    dtlExpTime                  sDtlExpTime;
    
    DTL_PARAM_VALIDATE( ((aType->mIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                         (aType->mIndicator == DTL_INTERVAL_INDICATOR_MONTH) ||
                         (aType->mIndicator == DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH))
                        == STL_TRUE,
                        aErrorStack );

    sIntervalYearToMonthType.mIndicator = aType->mIndicator;
    sIntervalYearToMonthType.mMonth = aType->mMonth;

    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    dtdIntervalYearToMonth2DtlExpTime( sIntervalYearToMonthType,
                                       & sDtlExpTime );

    if( (sIntervalYearToMonthType.mMonth < 0) )
    {
        *aIsNegative = STL_TRUE;
        
        *aYear = sDtlExpTime.mYear * (-1);
        *aMonth = sDtlExpTime.mMonth * (-1);
    }
    else
    {
        *aIsNegative = STL_FALSE;
        
        *aYear = sDtlExpTime.mYear;
        *aMonth = sDtlExpTime.mMonth;
    }

    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief dtlIntervalDayToSecondType 에서 day_second field 값 반환
 * @param[in]  aType       dtlIntervalDayToSecondType 데이터
 * @param[out] aDay        day
 * @param[out] aHour       hour
 * @param[out] aMinute     minute
 * @param[out] aSecond     second
 * @param[out] aFraction   fraction
 * @param[out] aIsNegative 음수 여부
 * @param[out] aErrorStack 에러 스택
 */
stlStatus dtlGetTimeInfoFromDaySecondInterval( dtlIntervalDayToSecondType * aType,
                                               stlUInt32                  * aDay,
                                               stlUInt32                  * aHour,
                                               stlUInt32                  * aMinute,
                                               stlUInt32                  * aSecond,
                                               stlUInt32                  * aFraction,
                                               stlBool                    * aIsNegative,
                                               stlErrorStack              * aErrorStack )
{
    dtlIntervalDayToSecondType  sIntervalDayToSecondType;
    dtlExpTime                  sDtlExpTime;
    dtlFractionalSecond         sFractionalSecond;    
    
    DTL_PARAM_VALIDATE( ((aType->mIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                         (aType->mIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                         (aType->mIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                         (aType->mIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                         (aType->mIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                         (aType->mIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                         (aType->mIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                         (aType->mIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                         (aType->mIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                         (aType->mIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND))
                        == STL_TRUE,
                        aErrorStack );

    sIntervalDayToSecondType.mIndicator = aType->mIndicator;
    sIntervalDayToSecondType.mDay = aType->mDay;
    sIntervalDayToSecondType.mTime = aType->mTime;

    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    dtdIntervalDayToSecond2DtlExpTime( sIntervalDayToSecondType,
                                       & sDtlExpTime,
                                       & sFractionalSecond );

    if( (sIntervalDayToSecondType.mDay < 0) ||
        (sIntervalDayToSecondType.mTime < 0) )
    {
        *aIsNegative = STL_TRUE;

        *aDay = sDtlExpTime.mDay * (-1);
        *aHour = sDtlExpTime.mHour * (-1);
        *aMinute = sDtlExpTime.mMinute * (-1);
        *aSecond = sDtlExpTime.mSecond * (-1);
        *aFraction = sFractionalSecond * (-1);
    }
    else
    {
        *aIsNegative = STL_FALSE;

        *aDay = sDtlExpTime.mDay;
        *aHour = sDtlExpTime.mHour;
        *aMinute = sDtlExpTime.mMinute;
        *aSecond = sDtlExpTime.mSecond;
        *aFraction = sFractionalSecond;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief dtlNumericType 에서 interval second 값 반환
 *        ODBC에서 호출하는 함수로 fractional second값이 반올림되지 않고, truncate됨.
 * @param[in]  aNumericType                data value
 * @param[in]  aIntervalIndicator          aIntervalIndicator
 * @param[in]  aLeadingPrecision           interval leading field precision
 * @param[in]  aSecondFractionalPrecision  interval second field precision
 * @param[out] aSecond                     second
 * @param[out] aFractionalSecond           fractional second
 * @param[out] aSuccessWithInfo            waring 발생 여부
 * @param[out] aIsNegative                 음수 여부
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetIntervalSecondInfoFromDtlNumericValue( dtlDataValue         * aNumericType,
                                                       dtlIntervalIndicator   aIntervalIndicator,
                                                       stlInt64               aLeadingPrecision,
                                                       stlInt64               aSecondFractionalPrecision,
                                                       stlUInt32            * aSecond,
                                                       stlUInt32            * aFractionalSecond,
                                                       stlBool              * aIsNegative,
                                                       stlBool              * aSuccessWithInfo,
                                                       stlErrorStack        * aErrorStack )
{
    dtlNumericType  * sNumeric;
    stlUInt8        * sDigitPtr;    
    stlInt32          sDigitCount;
    stlInt32          sExponent;
    stlInt32          sCount;
    
    stlInt64          sValue = 0;
    stlUInt32         sFractionalSecond = 0;
    dtlIntervalDayToSecondType  sIntervalDayToSecondType;
    dtlExpTime                  sDtlExpTime;
    
    DTL_PARAM_VALIDATE( aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND, aErrorStack );
    DTL_PARAM_VALIDATE( (aLeadingPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aLeadingPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( (aSecondFractionalPrecision >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                        (aSecondFractionalPrecision <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION),
                        aErrorStack );
    
    sNumeric  = DTF_NUMERIC( aNumericType );
    
    sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );
    
    /*
     * DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION (6)을 넘는지 체크.
     */   
    STL_TRY_THROW( sExponent < 3, ERROR_OUT_OF_PRECISION );

    if( DTL_NUMERIC_IS_ZERO( sNumeric, aNumericType->mLength ) == STL_TRUE )
    {
        sValue = 0;
        *aIsNegative = STL_FALSE;
    }
    else
    {
        if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) )
        {
            /**
             * 양수
             */

            *aIsNegative = STL_FALSE;                        
                
            sValue = 0;
            sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( aNumericType->mLength );
            sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

            /**
             * Digit Count가 크기 때문에 Exponent + 1 만큼
             * Decoding하고 Exponent + 2의 Digit을 이용하여
             * 버림을 판단한다.
             */
                    
            if( ( (sDigitCount - 1) - sExponent ) > 3 )
            {
                /* 소수점자리가 7이상인 경우, scale 6에 맞추어 버림. */
                        
                if( sExponent > 0 )
                {
                    sCount = sExponent + 3;
                }
                else
                {
                    sCount = 3;
                }

                while( sCount >= 0 )
                {
                    sValue *= 100;
                    sValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr );
                    sDigitPtr++;
                    sCount--;
                }
            }
            else
            {
                sCount = sDigitCount;
                        
                while( sCount > 0 )
                {
                    sValue *= 100;
                    sValue += DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sDigitPtr );
                    sDigitPtr++;
                    sCount--;
                }

                sValue *= (stlInt64)gPreDefinedPow[ DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION -
                                                    ((sDigitCount - 1 - sExponent) * 2) ];
            }
        }
        else
        {
            /**
             * 음수
             */

            *aIsNegative = STL_TRUE;                        

            sValue = 0;
            sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( aNumericType->mLength );
            sDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

            if( ( (sDigitCount - 1) - sExponent ) > 3 )
            {
                /* 소수점자리가 7이상인 경우, scale 6에 맞추어 버림. */
                        
                if( sExponent > 0 )
                {
                    sCount = sExponent + 3;
                }
                else
                {
                    sCount = 3;
                }

                while( sCount >= 0 )
                {
                    sValue *= 100;
                    sValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr );
                    sDigitPtr++;
                    sCount--;
                }
            }
            else
            {
                sCount = sDigitCount;
                        
                while( sCount > 0 )
                {
                    sValue *= 100;
                    sValue += DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sDigitPtr );
                    sDigitPtr++;
                    sCount--;
                }
                    
                sValue *= (stlInt64)gPreDefinedPow[ DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION -
                                                    ((sDigitCount - 1 - sExponent) * 2) ];
            }
        }
    }
        
    sIntervalDayToSecondType.mIndicator = aIntervalIndicator;
    sIntervalDayToSecondType.mDay       = sValue / DTL_USECS_PER_DAY;
    sIntervalDayToSecondType.mTime      = sValue % DTL_USECS_PER_DAY;

    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    dtdIntervalDayToSecond2DtlExpTime( sIntervalDayToSecondType,
                                       & sDtlExpTime,
                                       (dtlFractionalSecond *)(& sFractionalSecond) );

    STL_TRY( dtlAdjustIntervalDayToSecond( aIntervalIndicator,
                                           aLeadingPrecision,
                                           aSecondFractionalPrecision,
                                           sDtlExpTime.mDay,  
                                           sDtlExpTime.mHour, 
                                           sDtlExpTime.mMinute, 
                                           sDtlExpTime.mSecond,
                                           & sFractionalSecond,
                                           aSuccessWithInfo,
                                           aErrorStack )
             == STL_SUCCESS );

    *aSecond = sValue / DTL_USECS_PER_SEC;
    *aFractionalSecond = sFractionalSecond;

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_YM_INTERVAL_SOME_FILEDS_TRUNCATED_CONVERSION,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief BINARY STRING을 문자열로 변환 (BINARY, VARYING BINARY, LONG VARYING BINARY)
 *        10진수들을 16진수들로 변환
 * 
 * @param[in]  aNumber        10진수 배열
 * @param[in]  aNumberLength  10진수 배열 길이
 * @param[in]  aAvailableSize 16진수 배열이 저장될 메모리 공간의 크기
 * @param[out] aHex           16진수 배열
 * @param[out] aHexLength     16진수 배열 길이
 * @param[out] aErrorStack    에러 스택
 */
stlStatus dtlGetStringFromBinaryString( stlInt8         * aNumber,
                                        stlInt64          aNumberLength,
                                        stlInt64          aAvailableSize,
                                        stlChar         * aHex,
                                        stlInt64        * aHexLength, 
                                        stlErrorStack   * aErrorStack )
{
    return dtdToStringFromBinaryString( aNumber,
                                        aNumberLength,
                                        aAvailableSize,
                                        aHex,
                                        aHexLength, 
                                        aErrorStack );
}

/**
 * @brief BINARY STRING을 unicode 문자열로 변환 (BINARY, VARYING BINARY, LONG VARYING BINARY)
 *        10진수들을 16진수들로 변환
 * 
 * @param[in]  aUniEncoding   unicode encoding ( utf16 또는 utf32 )
 * @param[in]  aNumber        10진수 배열
 * @param[in]  aNumberLength  10진수 배열 길이
 * @param[in]  aAvailableSize 16진수 배열이 저장될 메모리 공간의 크기
 * @param[out] aHex           16진수 배열
 * @param[out] aHexLength     16진수 배열 길이
 * @param[out] aErrorStack    에러 스택
 */
stlStatus dtlGetWCharStringFromBinaryString( dtlUnicodeEncoding   aUniEncoding,
                                             stlInt8            * aNumber,
                                             stlInt64             aNumberLength,
                                             stlInt64             aAvailableSize,
                                             void               * aHex,
                                             stlInt64           * aHexLength, 
                                             stlErrorStack      * aErrorStack )
{
    return dtdGetWCharStringFromBinaryStringFuncList[aUniEncoding]( aNumber,
                                                                    aNumberLength,
                                                                    aAvailableSize,
                                                                    aHex,
                                                                    aHexLength, 
                                                                    aErrorStack );
}


/**
 * @brief dtlTimeType value를 precision(fractional second precision)에 맞게 조정.
 * @param[in,out] aTimeType       dtlTimeType
 * @param[in]     aTimePrecision  dtlTimeType의 precision(fractional second precision)
 * @param[out]    aErrorStack     에러 스택 
 */
stlStatus dtlAdjustTime( dtlTimeType   * aTimeType,
                         stlInt32        aTimePrecision,
                         stlErrorStack * aErrorStack )
{
    return dtdAdjustTime( aTimeType,
                          aTimePrecision,
                          aErrorStack );
}

/**
 * @brief dtlTimeTzType value를 precision(fractional second precision)에 맞게 조정.
 * @param[in,out] aTimeTzType       dtlTimeTzType
 * @param[in]     aTimeTzPrecision  dtlTimeTzType의 precision(fractional second precision)
 * @param[out]    aErrorStack       에러 스택 
 */
stlStatus dtlAdjustTimeTz( dtlTimeTzType   * aTimeTzType,
                           stlInt32          aTimeTzPrecision,
                           stlErrorStack   * aErrorStack )
{
    return dtdAdjustTime( &(aTimeTzType->mTime),
                          aTimeTzPrecision,
                          aErrorStack );
}

/**
 * @brief dtlTimestampType를 precision(fractional second precision)에 맞게 조정.
 * @param[in,out] aTimestampType       dtlTimestampType
 * @param[in]     aTimestampPrecision  dtlTimestampType의 precision(fractional second precision)
 * @param[out]    aErrorStack          에러 스택 
 */
stlStatus dtlAdjustTimestamp( dtlTimestampType * aTimestampType,
                              stlInt32           aTimestampPrecision,
                              stlErrorStack    * aErrorStack )
{
    return dtdAdjustTimestamp( aTimestampType,
                               aTimestampPrecision,
                               aErrorStack );
}

/**
 * @brief dtlTimestampTzType를 precision(fractional second precision)에 맞게 조정.
 * @param[in,out] aTimestampTzType      dtlTimestampTzType
 * @param[in]     aTimestampTzPrecision dtlTimestampTzType의 precision(fractional second precision)
 * @param[out]    aErrorStack           에러 스택 
 */
stlStatus dtlAdjustTimestampTz( dtlTimestampTzType * aTimestampTzType,
                                stlInt32             aTimestampTzPrecision,
                                stlErrorStack      * aErrorStack )
{
    return dtdAdjustTimestamp( & (aTimestampTzType->mTimestamp),
                               aTimestampTzPrecision,
                               aErrorStack );
}

/**
 * @brief YEAR TO MONTH INTERVAL TYPE에 대한 leading precision을 체크.
 * @param[in]  aIntervalIndicator  dtlIntervalIndicator
 * @param[in]  aLeadingPrecision   leading precision
 * @param[in]  aYear               year
 * @param[in]  aMonth              month
 * @param[out] aSuccessWithInfo    warning 발생 여부 
 * @param[out] aErrorStack         에러 스택
 */
stlStatus dtlAdjustIntervalYearToMonth( dtlIntervalIndicator   aIntervalIndicator,
                                        stlInt32               aLeadingPrecision,
                                        stlUInt32              aYear,
                                        stlUInt32              aMonth,
                                        stlBool              * aSuccessWithInfo,
                                        stlErrorStack        * aErrorStack )
{
    dtlIntervalYearToMonthType  sIntervalYearToMonthType;
    dtlExpTime                  sDtlExpTime;
    stlBool                     sIsTruncated = STL_FALSE;    
    
    DTL_PARAM_VALIDATE( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH) ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH),
                        aErrorStack );
    DTL_PARAM_VALIDATE( (aLeadingPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aLeadingPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );

    sDtlExpTime.mYear = aYear;
    sDtlExpTime.mMonth = aMonth;
    sDtlExpTime.mDay = 0;    
    sDtlExpTime.mHour = 0;
    sDtlExpTime.mMinute = 0;
    sDtlExpTime.mSecond = 0;
    
    dtdDtlExpTime2IntervalYearToMonth( & sDtlExpTime,
                                       aIntervalIndicator,
                                       & sIntervalYearToMonthType );

    /* leading precision 체크 */
    STL_TRY( dtdAdjustIntervalYearToMonth( & sIntervalYearToMonthType,
                                           aIntervalIndicator,
                                           aLeadingPrecision,
                                           aErrorStack )
             == STL_SUCCESS );

    /*
     * value truncate check
     */
    switch( aIntervalIndicator )
    {
        case DTL_INTERVAL_INDICATOR_YEAR:
            {
                if( aMonth == 0 )
                {
                    // Do Nothing
                }
                else
                {
                    sIsTruncated = STL_TRUE;
                }
            }
            break;
        case DTL_INTERVAL_INDICATOR_MONTH:
        case DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH:
            // Do Nothing
            break;
        default:
            /* Do Nothing */
            break;
    }

    *aSuccessWithInfo = sIsTruncated;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief DAY TO SECOND INTERVAL TYPE에 대한 leading precision을 체크하고,
 *   <BR> fractional second precision 에 맞게 fractional second를 조정한다.
 * @param[in]  aIntervalIndicator    dtlIntervalIndicator
 * @param[in]  aLeadingPrecision     leading precision
 * @param[in]  aFractionalSecondPrecision  fractional second precision
 * @param[in]  aDay                  day
 * @param[in]  aHour                 hour
 * @param[in]  aMinute               minute
 * @param[in]  aSecond               second
 * @param[in,out] aFractionalSecond  fractional second
 * @param[out] aSuccessWithInfo      warning 발생 여부 
 * @param[out] aErrorStack           에러 스택
 */
stlStatus dtlAdjustIntervalDayToSecond( dtlIntervalIndicator   aIntervalIndicator,
                                        stlInt32               aLeadingPrecision,
                                        stlInt32               aFractionalSecondPrecision,
                                        stlUInt32              aDay,
                                        stlUInt32              aHour,
                                        stlUInt32              aMinute,
                                        stlUInt32              aSecond,
                                        stlUInt32            * aFractionalSecond,
                                        stlBool              * aSuccessWithInfo,
                                        stlErrorStack        * aErrorStack )
{
    dtlIntervalDayToSecondType  sIntervalDayToSecondType;
    dtlExpTime                  sDtlExpTime;
    dtlFractionalSecond         sTempFractionalSecond;
    dtlFractionalSecond         sFractionalSecond;
    stlUInt8                    i;
    stlInt32                    sTailZero;
    stlBool                     sIsTruncated = STL_FALSE;
    
    DTL_PARAM_VALIDATE(
        ((aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY)            ||
         (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR)           ||
         (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE)         ||
         (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND)         ||
         (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR)    ||
         (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE)  ||
         (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND)  ||
         (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
         (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
         (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND))
        == STL_TRUE,
        aErrorStack );
    DTL_PARAM_VALIDATE( (aLeadingPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aLeadingPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( ( (aFractionalSecondPrecision >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                          (aFractionalSecondPrecision <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ||
                        ( aFractionalSecondPrecision == DTL_SCALE_NA ),
                        aErrorStack );

    DTL_PARAM_VALIDATE( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) )
                        ||
                        ( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) ) &&
                          ( (aFractionalSecondPrecision >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                            (aFractionalSecondPrecision <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ),
                        aErrorStack );
    
    sDtlExpTime.mYear = 0;
    sDtlExpTime.mMonth = 0;
    sDtlExpTime.mDay = aDay;    
    sDtlExpTime.mHour = aHour;
    sDtlExpTime.mMinute = aMinute;
    sDtlExpTime.mSecond = aSecond;

    /*
     * ODBC에서 bindColumn시 호출하는 함수로 fractional second는 반올림하지 않고 truncate됨.
     * fractionalSecondPrecison에 맞게 필요한 경우 fractionalsecond를 truncate함.
     */    
    sFractionalSecond = *aFractionalSecond;

    if( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
    {
        for( i = aFractionalSecondPrecision;
             (i < DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) && ( sFractionalSecond != 0);
             i++ )
        {
            sTempFractionalSecond =  sFractionalSecond / 10;
            sFractionalSecond = sTempFractionalSecond;
        }

        sTailZero = ( aFractionalSecondPrecision == 0 ? 1000000 :
                     (aFractionalSecondPrecision == 1 ? 100000 :
                     (aFractionalSecondPrecision == 2 ? 10000  :
                     (aFractionalSecondPrecision == 3 ? 1000   :
                     (aFractionalSecondPrecision == 4 ? 100    :
                     (aFractionalSecondPrecision == 5 ? 10     :
                                                        1 ) ) ) ) ) );

        sFractionalSecond = sFractionalSecond * sTailZero;
    }
    else
    {
        // Do Nothing
    }
    
    dtdDtlExpTime2IntervalDayToSecond( & sDtlExpTime,
                                       sFractionalSecond,
                                       aIntervalIndicator,
                                       & sIntervalDayToSecondType );

    STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecondType,
                                           aIntervalIndicator,
                                           aLeadingPrecision,
                                           aFractionalSecondPrecision,
                                           aErrorStack )
             == STL_SUCCESS );

    sTempFractionalSecond = *aFractionalSecond;    
    *aFractionalSecond = sFractionalSecond;

    /*
     * value truncate check
     */
    switch( aIntervalIndicator )
    {
        case DTL_INTERVAL_INDICATOR_DAY:
            {
                if( (aHour == 0) && (aMinute == 0) && (aSecond == 0) && (*aFractionalSecond == 0) )
                {
                    // Do Nothing
                }
                else
                {
                    sIsTruncated = STL_TRUE;
                }
            }
            break;
        case DTL_INTERVAL_INDICATOR_HOUR:
        case DTL_INTERVAL_INDICATOR_DAY_TO_HOUR:
            {
                if( (aMinute == 0) && (aSecond == 0) && (*aFractionalSecond == 0) )
                {
                    // Do Nothing
                }
                else
                {
                    sIsTruncated = STL_TRUE;
                }
            }
            break;
        case DTL_INTERVAL_INDICATOR_MINUTE:
        case DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE:
        case DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE:
            {
                if( (aSecond == 0) && (*aFractionalSecond == 0) )
                {
                    // Do Nothing
                }
                else
                {
                    sIsTruncated = STL_TRUE;
                }
            }
            break;
        case DTL_INTERVAL_INDICATOR_SECOND:
        case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND:
        case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND:
        case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND:
            {
                if( sTempFractionalSecond == *aFractionalSecond )
                {
                    // Do Nothing
                }
                else
                {
                    sIsTruncated = STL_TRUE;
                }
            }
            break;
        default:
            /* Do Nothing */
            break;
    }

    *aSuccessWithInfo = sIsTruncated;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief stlInt64 value를 dtlNumericType value로 변환한다.
 * @param[in]  aInt64        stlInt64 value
 * @param[out] aResult       dtlNumericType value로 변환된 결과값
 * @param[out] aErrorStack   에러 스택
 */

stlStatus dtlInt64ToNumeric( stlInt64          aInt64,
                             dtlDataValue    * aResult,
                             stlErrorStack   * aErrorStack )
{
    stlBool           sIsPositive;
    stlInt32          sDigitCount;
    stlUInt8        * sDigitPtr;
    stlUInt8        * sEndDigitPtr;
    stlUInt8        * sTmpDigitPtr;
    stlInt64          sNum;
    stlInt64          sQuot;
    dtlNumericType  * sNumeric;

    /**
     * Parameter Validation
     */

    DTL_CHECK_TYPE( DTL_TYPE_NUMBER, aResult, aErrorStack );

    
    if( aInt64 < 0 )
    {
        sIsPositive = STL_FALSE;
        sNum = -aInt64;
    }
    else
    {
        sIsPositive = STL_TRUE;
        sNum = aInt64;
    }
    
    sNumeric = DTF_NUMERIC( aResult );
    sEndDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric ) + DTL_NUMERIC_MAX_DIGIT_COUNT - 1;
    sDigitPtr = sEndDigitPtr;

    while( sNum > 0 )
    {
        sQuot = sNum / 100;
        *sDigitPtr = DTL_NUMERIC_GET_DIGIT_FROM_VALUE( sIsPositive, (sNum - sQuot * 100) );
        sNum = sQuot;
        sDigitPtr--;
    }

    sDigitCount = sEndDigitPtr - sDigitPtr;

    aResult->mLength = sDigitCount + 1;
    DTL_NUMERIC_SET_SIGN_AND_EXPONENT( sNumeric, sIsPositive, sDigitCount - 1 );

    sDigitPtr++;
    sTmpDigitPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
    while( sDigitPtr <= sEndDigitPtr )
    {
        *sTmpDigitPtr = *sDigitPtr;
        sTmpDigitPtr++;
        sDigitPtr++;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief dtlNumericType value를 stlInt64 value로 변환한다.
 * @param[in]  aNumericType  dtlNumericType value
 * @param[out] aResult       stlInt64 value로 변환된 결과값
 * @param[out] aIsTruncated  소수점 이하의 숫자가 존재했는지 여부
 * @param[out] aErrorStack   에러 스택
 *
 * @remark round 적용하지 않음
 */
stlStatus dtlNumericToInt64( dtlDataValue    * aNumericType,
                             stlInt64        * aResult,
                             stlBool         * aIsTruncated,
                             stlErrorStack   * aErrorStack )
{
    dtlNumericType  * sNumeric;
    stlInt64          sResult;

    stlInt64          sMaxFence = STL_INT64_MAX / 100;
    stlInt64          sMinFence = STL_INT64_MIN / 100;

    stlUInt8        * sNumericDataPtr;
    stlInt32          sExponent;
    stlInt32          sDigitCount;


    sNumeric  = DTF_NUMERIC( aNumericType );

    sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );
    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( aNumericType->mLength );
    if( sExponent < 0 )
    {
        sResult   = 0;
        STL_THROW( RAMP_FINISH );
    }
    else if( sExponent > 9 )
    {
        STL_THROW( ERROR_OUT_OF_RANGE );
    }


    sResult = 0;
    sNumericDataPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

    if( DTL_NUMERIC_IS_POSITIVE( sNumeric ) == STL_TRUE )
    {
        if( (sDigitCount - 1) < sExponent )
        {
            sExponent -= sDigitCount;
            while( sDigitCount > 0 )
            {
                sResult =
                    sResult * 100 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
                ++sNumericDataPtr;
                --sDigitCount;
            }
            
            sResult *= (stlInt64)gPreDefinedPow[sExponent*2];
            STL_TRY_THROW( sResult < sMaxFence, ERROR_OUT_OF_RANGE );
            sResult *= 100;
            *aIsTruncated =  STL_FALSE;
        }
        else
        {
            *aIsTruncated = (((sDigitCount -1) > sExponent) ? STL_TRUE : STL_FALSE);
            while( sExponent > 0 )
            {
                sResult =
                    sResult * 100 +
                    DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
                ++sNumericDataPtr;
                --sExponent;
            }

            if( sResult > sMaxFence )
            {
                STL_THROW( ERROR_OUT_OF_RANGE );
            }
            else if( sResult == sMaxFence )
            {
                STL_TRY_THROW( *sNumericDataPtr < DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 8 ),
                               ERROR_OUT_OF_RANGE );
            }

            sResult =
                sResult * 100 +
                DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
        }
    }
    else
    {
        if( (sDigitCount - 1) < sExponent )
        {
            sExponent -= sDigitCount;
            while( sDigitCount > 0 )
            {
                sResult =
                    sResult * 100 -
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sNumericDataPtr );
                ++sNumericDataPtr;
                --sDigitCount;
            }
            
            sResult *= (stlInt64)gPreDefinedPow[sExponent*2];
            STL_TRY_THROW( sResult > sMinFence, ERROR_OUT_OF_RANGE );
            sResult *= 100;
            *aIsTruncated =  STL_FALSE;
        }
        else
        {
            *aIsTruncated = (((sDigitCount -1) > sExponent) ? STL_TRUE : STL_FALSE);
            while( sExponent > 0 )
            {
                sResult =
                    sResult * 100 -
                    DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sNumericDataPtr );
                ++sNumericDataPtr;
                --sExponent;
            }
            
            if( sResult < sMinFence )
            {
                STL_THROW( ERROR_OUT_OF_RANGE );
            }
            else if( sResult == sMinFence )
            {
                STL_TRY_THROW( *sNumericDataPtr > DTL_NUMERIC_GET_NEGATIVE_DIGIT_FROM_VALUE( 9 ),
                               ERROR_OUT_OF_RANGE );
            }

            sResult =
                sResult * 100 -
                DTL_NUMERIC_GET_VALUE_FROM_NEGATIVE_DIGIT( *sNumericDataPtr );
        }
    }

    
    STL_RAMP( RAMP_FINISH );

    *aResult = sResult;
 

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;
        
    return STL_FAILURE;
}

/**
 * @brief dtlNumericType value를 stlUInt64 value로 변환한다.
 * @param[in]  aNumericType  dtlNumericType value
 * @param[out] aResult       stlUInt64 value로 변환된 결과값
 * @param[out] aIsTruncated  소수점 이하의 숫자가 존재했는지 여부
 * @param[out] aErrorStack   에러 스택 
 * 
 * @remark round 적용하지 않음
 */
stlStatus dtlNumericToUInt64( dtlDataValue    * aNumericType,
                              stlUInt64       * aResult,
                              stlBool         * aIsTruncated,
                              stlErrorStack   * aErrorStack )
{
    dtlNumericType  * sNumeric;
    stlUInt64         sResult;

    stlInt64          sMaxFence = STL_INT64_MAX / 100;

    stlUInt8        * sNumericDataPtr;
    stlInt32          sExponent;
    stlInt32          sDigitCount;


    sNumeric = DTF_NUMERIC( aNumericType );

    sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );
    sDigitCount = DTL_NUMERIC_GET_DIGIT_COUNT( aNumericType->mLength );
    if( sExponent < 0 )
    {
        sResult   = 0;
        STL_THROW( RAMP_FINISH );
    }
    else if( sExponent > 9 ||
             DTL_NUMERIC_IS_NEGATIVE( sNumeric ) == STL_TRUE )
    {
        STL_THROW( ERROR_OUT_OF_RANGE );
    }

    
    sResult = 0;
    sNumericDataPtr = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );

    if( (sDigitCount - 1) < sExponent )
    {
        sExponent -= sDigitCount;
        while( sDigitCount > 0 )
        {
            sResult =
                sResult * 100 +
                DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
            ++sNumericDataPtr;
            --sDigitCount;
        }

        sResult *= (stlUInt64)gPreDefinedPow[sExponent*2];
        STL_TRY_THROW( sResult < sMaxFence, ERROR_OUT_OF_RANGE );
        sResult *= 100;
        *aIsTruncated = STL_FALSE;
    }
    else
    {
        *aIsTruncated = (((sDigitCount -1) > sExponent) ? STL_TRUE : STL_FALSE);
        while( sExponent > 0 )
        {
            sResult =
                sResult * 100 +
                DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
            ++sNumericDataPtr;
            --sExponent;
        }

        if( sResult > sMaxFence )
        {
            STL_THROW( ERROR_OUT_OF_RANGE );
        }
        else if( sResult == sMaxFence )
        {
            STL_TRY_THROW( *sNumericDataPtr < DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 17 ),
                           ERROR_OUT_OF_RANGE );
        }

        sResult =
            sResult * 100 +
            DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT( *sNumericDataPtr );
    }
   
    STL_RAMP( RAMP_FINISH );

    *aResult = sResult;
 

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_SQL_NUMBER_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;
        
    return STL_FAILURE;
}


/**
 * @brief dtlNumericType value를 stlFloat32 value로 변환한다.
 * @param[in]  aNumericType  dtlNumericType value
 * @param[in]  aResult       stlFloat32 value로 변환된 결과값
 * @param[out] aErrorStack   에러 스택 
 * 
 * @remark round 적용하지 않음
 */
stlStatus dtlNumericToFloat32( dtlDataValue    * aNumericType,
                               stlFloat32      * aResult,
                               stlErrorStack   * aErrorStack )
{
    return dtdToFloat32FromNumeric( aNumericType,
                                    aResult,
                                    aErrorStack );
}


/**
 * @brief dtlNumericType value를 stlFloat64 value로 변환한다.
 * @param[in]  aNumericType  dtlNumericType value
 * @param[in]  aResult       stlFloat64 value로 변환된 결과값
 * @param[out] aErrorStack   에러 스택 
 * 
 * @remark round 적용하지 않음
 */
stlStatus dtlNumericToFloat64( dtlDataValue    * aNumericType,
                               stlFloat64      * aResult,
                               stlErrorStack   * aErrorStack )
{
    return dtdToFloat64FromNumeric( aNumericType,
                                    aResult,
                                    aErrorStack );
}


/**
 * @brief 인자로 받은 year, month, day가 valid한지 여부 검사.
 *   <BR> 인자로 받은 YEAR, MONTH, DAY는 그레고리력(양력달력)임.
 * @param[in] aYear   year
 * @param[in] aMonth  month
 * @param[in] aDay    day
 * @return valid 결과 리턴 
 */
stlBool dtlIsValidateDate( stlInt32        aYear,
                           stlInt32        aMonth,
                           stlInt32        aDay )
{
    stlBool sIsValidate = STL_FALSE;
    
    /* check valid month */
    STL_TRY_THROW( ((aMonth >= 1) && (aMonth <= DTL_MONTHS_PER_YEAR)) == STL_TRUE,
                   RAMP_EXIT_VALIDATE );
    
    /* check valid day */
    STL_TRY_THROW( ((aDay >= 1) && (aDay <= 31)) == STL_TRUE,
                   RAMP_EXIT_VALIDATE );
    
    /* 윤년 체크 포함해서 해당년월의 일(day)을 체크 . */
    STL_TRY_THROW( (aDay <=
                    dtlDayTab[DTL_IS_LEAP(aYear)][aMonth - 1]),
                   RAMP_EXIT_VALIDATE );    
    
    sIsValidate = DTL_IS_VALID_JULIAN( aYear, aMonth, aDay );
    
    STL_RAMP( RAMP_EXIT_VALIDATE );
    
    return sIsValidate;
}

/**
 * @brief 인자로 받은 hour, minute, second가 valid한지 여부 검사.
 *   <BR> 인자로 받은 HOUR, MINUTE, SECOND는 그레고리력(양력달력)임.
 * @param[in] aHour    hour
 * @param[in] aMinute  minute
 * @param[in] aSecond  second
 * @return valid 결과 리턴 
 */
stlBool dtlIsValidateTime( stlInt32        aHour,
                           stlInt32        aMinute,
                           stlInt32        aSecond )
{
    stlBool sIsValidate;

    if( (aHour < 0)   || (aHour > 23)   ||
        (aMinute < 0) || (aMinute > 59) ||
        (aSecond < 0) || (aSecond > 59) )
    {
        sIsValidate = STL_FALSE;
    }
    else
    {
        sIsValidate = STL_TRUE;
    }
    
    return sIsValidate;
}

/**
 * @brief 인자로 받은 year, month, day, hour, minute, second, fraction이 valid한지 여부 검사.
 *   <BR> 인자로 받은 YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, ... 는 그레고리력(양력달력)임.
 * @param[in] aYear     year
 * @param[in] aMonth    month
 * @param[in] aDay      day
 * @param[in] aHour     hour
 * @param[in] aMinute   minute
 * @param[in] aSecond   second
 * @param[in] aFraction fractional second
 * @return valid 결과 리턴 
 */
stlBool dtlIsValidateTimestamp( stlInt32         aYear,
                                stlInt32         aMonth,
                                stlInt32         aDay,
                                stlInt32         aHour,
                                stlInt32         aMinute,
                                stlInt32         aSecond,
                                stlInt32         aFraction )
{
    stlBool sIsValidate = STL_FALSE;
    
    STL_TRY_THROW( dtlIsValidateDate( aYear,
                                      aMonth,
                                      aDay ) == STL_TRUE,
                   RAMP_EXIT_VALIDATE );
    
    STL_TRY_THROW( dtlIsValidateTime( aHour,
                                      aMinute,
                                      aSecond ) == STL_TRUE,
                   RAMP_EXIT_VALIDATE );

    /* DTL_TIMESTAMP_MAX_PRECISION (6)이어서 0 < aFraction < 999999로 검사 */
    STL_TRY_THROW( ((aFraction >= 0) && (aFraction <= 999999)) == STL_TRUE,
                   RAMP_EXIT_VALIDATE );

    sIsValidate = STL_TRUE;
    
    STL_RAMP( RAMP_EXIT_VALIDATE );    
    
    return sIsValidate;
}

/**
 * @brief 인자로 받은 timezone hour, timezone minute 가 valid한지 여부 검사.
 * @param[in]  aTimeZoneHour    timezone hour
 * @param[in]  aTimeZoneMinute  timezone minute
 * @return valid 결과 리턴
 */
stlBool dtlIsValidateTimeZone( stlInt32  aTimeZoneHour,
                               stlInt32  aTimeZoneMinute )
{
    stlBool   sIsValid = STL_FALSE;
    stlInt32  sSecond = 0;
    
    STL_TRY_THROW( ( (aTimeZoneHour > 0) && (aTimeZoneMinute >= 0) )
                   ||
                   ( (aTimeZoneHour < 0) && (aTimeZoneMinute <= 0) )
                   ||
                   ( (aTimeZoneHour == 0) &&
                     ((aTimeZoneMinute >= 0) || (aTimeZoneMinute < 0)) ),
                   RAMP_EXIT_VALIDATE );

    sSecond = (aTimeZoneHour * DTL_SECS_PER_HOUR) + (aTimeZoneMinute * DTL_SECS_PER_MINUTE);
    
    STL_TRY_THROW( ( (sSecond >= DTL_TIMEZONE_MIN_OFFSET) &&
                     (sSecond <= DTL_TIMEZONE_MAX_OFFSET) ),
                   RAMP_EXIT_VALIDATE );
    
    sIsValid = STL_TRUE;
    
    STL_RAMP( RAMP_EXIT_VALIDATE );    
    
    return sIsValid;    
}


/**
 * @brief stlTime으로부터 Date 타입의 value 구성
 *   <BR> stlTime(stlNow()) -> date 로 변환.
 * @param[in]  aStlTime         value에 저장될 입력 숫자
 * @param[in]  aPrecision      
 * @param[in]  aScale          
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue           dtlDataValue의 주소
 * @param[in]  aVectorFunc      Function Vector
 * @param[in]  aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtlDateSetValueFromStlTime( stlTime           aStlTime,
                                      stlInt64          aPrecision,
                                      stlInt64          aScale,
                                      stlInt64          aAvailableSize,
                                      dtlDataValue    * aValue,
                                      dtlFuncVector   * aVectorFunc,
                                      void            * aVectorArg,
                                      stlErrorStack   * aErrorStack )
{
    stlInt32 sGMTOffset = 0;
    
    /**
     * Parameter Validation
     */
    
    DTL_CHECK_TYPE( DTL_TYPE_DATE, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_DATE_SIZE, aErrorStack );
    
    /* TIME ZONE 값 획득 */
    sGMTOffset = aVectorFunc->mGetGMTOffsetFunc(aVectorArg);

    /* stlTime 값과 time zone 값으로 date 값을 얻는다. */
    STL_TRY( dtdDateSetValueFromStlTimeAndTimeZone( aStlTime,
                                                    sGMTOffset,
                                                    aValue,
                                                    aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief stlTime으로부터 Time 타입의 value 구성
 *   <BR> 예) stlTime(stlNow()) -> time 로 변환.
 * @param[in]  aStlTime         value에 저장될 입력 숫자
 * @param[in]  aPrecision      
 * @param[in]  aScale          
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue           dtlDataValue의 주소
 * @param[in]  aVectorFunc      Function Vector
 * @param[in]  aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtlTimeSetValueFromStlTime( stlTime           aStlTime,
                                      stlInt64          aPrecision,
                                      stlInt64          aScale,
                                      stlInt64          aAvailableSize,
                                      dtlDataValue    * aValue,
                                      dtlFuncVector   * aVectorFunc,
                                      void            * aVectorArg,
                                      stlErrorStack   * aErrorStack )
{
    dtlTimestampType    sTimestampUnixTZ;
    
    dtlTimeType         sTimeValue;

    stlInt32 sGMTOffset = 0;
    
    /**
     * Parameter Validation
     */

    DTL_CHECK_TYPE( DTL_TYPE_TIME, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_TIME_SIZE, aErrorStack );
    STL_PARAM_VALIDATE( (aPrecision >= DTL_TIME_MIN_PRECISION) &&
                        (aPrecision <= DTL_TIME_MAX_PRECISION),
                        aErrorStack );
    STL_PARAM_VALIDATE( aVectorFunc != NULL, aErrorStack );
    
    /**
     * DB TimeZone 값으로 변경 
     */

    sGMTOffset = aVectorFunc->mGetGMTOffsetFunc(aVectorArg);
    sTimestampUnixTZ = aStlTime + (sGMTOffset * DTL_USECS_PER_SEC);

    /**
     * Timestamp TZ 를 Time(without TZ) 로 변경
     */
    
    sTimeValue = sTimestampUnixTZ % DTL_USECS_PER_DAY;

    /**
     * Precision 보정
     */
    STL_TRY( dtdAdjustTime( & sTimeValue,
                            aPrecision,
                            aErrorStack )
             == STL_SUCCESS );
    
    /**
     * Result 설정
     */

    stlMemcpy( aValue->mValue,
               & sTimeValue,
               STL_SIZEOF(dtlTimeType) );
    
    aValue->mLength = DTL_TIME_SIZE;
                          
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief stlTime으로부터 TimeTz 타입의 value 구성
 *   <BR> 예) stlTime(stlNow()) -> timetz 로 변환.
 * @param[in]  aStlTime         value에 저장될 입력 숫자
 * @param[in]  aPrecision      
 * @param[in]  aScale          
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue           dtlDataValue의 주소
 * @param[in]  aVectorFunc      Function Vector
 * @param[in]  aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtlTimeTzSetValueFromStlTime( stlTime           aStlTime,
                                        stlInt64          aPrecision,
                                        stlInt64          aScale,
                                        stlInt64          aAvailableSize,
                                        dtlDataValue    * aValue,
                                        dtlFuncVector   * aVectorFunc,
                                        void            * aVectorArg,
                                        stlErrorStack   * aErrorStack )
{
    stlInt32 sGMTOffset = 0;
    
    /**
     * Parameter Validation
     */

    DTL_CHECK_TYPE( DTL_TYPE_TIME_WITH_TIME_ZONE, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_TIMETZ_SIZE, aErrorStack );
    STL_PARAM_VALIDATE( (aPrecision >= DTL_TIMETZ_MIN_PRECISION) &&
                        (aPrecision <= DTL_TIMETZ_MAX_PRECISION),
                        aErrorStack );
    STL_PARAM_VALIDATE( aVectorFunc != NULL, aErrorStack );
    
    /**
     * DB TimeZone 값으로 변경 
     */

    sGMTOffset = aVectorFunc->mGetGMTOffsetFunc(aVectorArg);

    STL_TRY( dtdTimeTzSetValueFromStlTimeAndTimeZone( aStlTime,
                                                      sGMTOffset,
                                                      aPrecision,
                                                      aValue,
                                                      aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief stlTime으로부터 Timestamp 타입의 value 구성
 *   <BR> 예) stlTime(stlNow()) -> timestamp 로 변환.
 * @param[in]  aStlTime         value에 저장될 입력 숫자
 * @param[in]  aPrecision       timestamp type의 fractional second precision
 * @param[in]  aScale           사용하지 않음.
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue           dtlDataValue의 주소
 * @param[in]  aVectorFunc      Function Vector
 * @param[in]  aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtlTimestampSetValueFromStlTime( stlTime           aStlTime,
                                           stlInt64          aPrecision,
                                           stlInt64          aScale,
                                           stlInt64          aAvailableSize,
                                           dtlDataValue    * aValue,
                                           dtlFuncVector   * aVectorFunc,
                                           void            * aVectorArg,
                                           stlErrorStack   * aErrorStack )
{
    dtlTimestampType    sTimestampJulian = 0;
    stlInt32            sGMTOffset = 0;

    /**
     * Parameter Validation
     */
    
    DTL_CHECK_TYPE( DTL_TYPE_TIMESTAMP, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_TIMESTAMP_SIZE, aErrorStack );
    STL_PARAM_VALIDATE( (aPrecision >= DTL_TIMESTAMP_MIN_PRECISION) &&
                        (aPrecision <= DTL_TIMESTAMP_MAX_PRECISION),
                        aErrorStack );
    STL_PARAM_VALIDATE( aVectorFunc != NULL, aErrorStack );

    /**
     * timestamp에서 최대한 수용할 수 있는 stlTime의 값 체크
     */

    STL_TRY_THROW( aStlTime <= DTF_FROM_UNIXTIME_MAX_STLTIME,
                   ERROR_OUT_OF_RANGE );
    
    /**
     * Unix Time 을 Julian Time 으로 변경
     * (2000-01-01 부터 계산된 값으로 저장됨)
     */
    
    sTimestampJulian = aStlTime - ((DTL_EPOCH_JDATE - DTL_UNIX_EPOCH_JDATE) * DTL_USECS_PER_DAY);

    /**
     * TimeZone 값으로 변경 
     */

    sGMTOffset = aVectorFunc->mGetGMTOffsetFunc(aVectorArg);
    sTimestampJulian = sTimestampJulian + (sGMTOffset * DTL_USECS_PER_SEC);
    
    /**
     * Precision 보정
     */
    
    STL_TRY( dtdAdjustTimestamp( & sTimestampJulian,
                                 aPrecision,
                                 aErrorStack )
             == STL_SUCCESS );
    
    /**
     * overflosw 체크
     * ( 1970-01-01 00:00:00.000000 ) ~ ( 9999-12-31 23:59:59.999999 )
     */
    
    STL_TRY_THROW( (sTimestampJulian >= DTF_FROM_UNIXTIME_MIN_TIMESTAMP) &&
                   (sTimestampJulian <= DTF_FROM_UNIXTIME_MAX_TIMESTAMP),
                   ERROR_OUT_OF_RANGE );
    
    /**
     * Result 설정
     */
    
    *(dtlTimestampType *)(aValue->mValue) = sTimestampJulian;
    aValue->mLength = DTL_TIMESTAMP_SIZE;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_DATETIME_FIELD_OVERFLOW,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief stlTime으로부터 TimestampTz 타입의 value 구성
 *   <BR> 예) stlTime(stlNow()) -> timestamptz 로 변환.
 * @param[in]  aStlTime         value에 저장될 입력 숫자
 * @param[in]  aPrecision      
 * @param[in]  aScale          
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue           dtlDataValue의 주소
 * @param[in]  aVectorFunc      Function Vector
 * @param[in]  aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtlTimestampTzSetValueFromStlTime( stlTime           aStlTime,
                                             stlInt64          aPrecision,
                                             stlInt64          aScale,
                                             stlInt64          aAvailableSize,
                                             dtlDataValue    * aValue,
                                             dtlFuncVector   * aVectorFunc,
                                             void            * aVectorArg,
                                             stlErrorStack   * aErrorStack )
{
    stlInt32   sGMTOffset;

    /**
     * Parameter Validation
     */
    
    DTL_CHECK_TYPE( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_TIMESTAMPTZ_SIZE, aErrorStack );
    STL_PARAM_VALIDATE( (aPrecision >= DTL_TIMESTAMPTZ_MIN_PRECISION) &&
                        (aPrecision <= DTL_TIMESTAMPTZ_MAX_PRECISION),
                        aErrorStack );
    STL_PARAM_VALIDATE( aVectorFunc != NULL, aErrorStack );
    
    /**
     * DB TimeZone 값으로 변경 
     */

    sGMTOffset = aVectorFunc->mGetGMTOffsetFunc(aVectorArg);

    STL_TRY( dtdTimestampTzSetValueFromStlTimeAndTimeZone( aStlTime,
                                                           sGMTOffset,
                                                           aPrecision,
                                                           aValue,
                                                           aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief stlTime으로부터 SysTimestamp( Timestamp With Time Zone 타입)의 value 구성
 *   <BR> 예) stlTime(stlNow()) -> timestamptz 로 변환.
 * @param[in]  aStlTime         value에 저장될 입력 숫자
 * @param[in]  aPrecision      
 * @param[in]  aScale          
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue           dtlDataValue의 주소
 * @param[in]  aVectorFunc      Function Vector
 * @param[in]  aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtlSysTimestampTzSetValueFromStlTime( stlTime           aStlTime,
                                                stlInt64          aPrecision,
                                                stlInt64          aScale,
                                                stlInt64          aAvailableSize,
                                                dtlDataValue    * aValue,
                                                dtlFuncVector   * aVectorFunc,
                                                void            * aVectorArg,
                                                stlErrorStack   * aErrorStack )
{
    stlExpTime    sStlExpTime;

    /**
     * Parameter Validation
     */
    
    DTL_CHECK_TYPE( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE, aValue, aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_TIMESTAMPTZ_SIZE, aErrorStack );
    STL_PARAM_VALIDATE( (aPrecision >= DTL_TIMESTAMPTZ_MIN_PRECISION) &&
                        (aPrecision <= DTL_TIMESTAMPTZ_MAX_PRECISION),
                        aErrorStack );
    STL_PARAM_VALIDATE( aVectorFunc != NULL, aErrorStack );
    
    /**
     * OS TimeZone 값으로 변경 
     */

    stlMakeExpTimeByLocalTz( &sStlExpTime, aStlTime );
    
    STL_TRY( dtdTimestampTzSetValueFromStlTimeAndTimeZone( aStlTime,
                                                           sStlExpTime.mGMTOffset,
                                                           aPrecision,
                                                           aValue,
                                                           aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief 인자로 받은 타임정보로 dtlIntervalYearToMonthType의 dtlValue를 만든다.
 * @param[in]  aYear                     year
 * @param[in]  aMonth                    month
 * @param[in]  aIsNegative               부호
 * @param[in]  aLeadingPrecision         leading precision
 * @param[in]  aIntervalIndicator        생성하고자하는 interval indicator
 * @param[in]  aAvailableSize            mValue에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aValue                    dtlDataValue
 * @param[out] aSuccessWithInfo          warning 발생 여부
 * @param[out] aErrorStack               에러 스택 
 */
stlStatus dtlIntervalYearToMonthSetValueFromTimeInfo(
    stlUInt32                    aYear,
    stlUInt32                    aMonth,
    stlBool                      aIsNegative,
    stlInt64                     aLeadingPrecision,
    dtlIntervalIndicator         aIntervalIndicator,
    stlInt64                     aAvailableSize,
    dtlDataValue               * aValue,
    stlBool                    * aSuccessWithInfo,    
    stlErrorStack              * aErrorStack )
{
    dtlIntervalYearToMonthType  sIntervalYearToMonthType;
    dtlExpTime                  sDtlExpTime;

    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_INTERVAL_YEAR_TO_MONTH_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_YEAR_TO_MONTH, aValue, aErrorStack );

    DTL_PARAM_VALIDATE( (aLeadingPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aLeadingPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    
    DTL_PARAM_VALIDATE( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR)  ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH) ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH),
                        aErrorStack );

    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    sDtlExpTime.mYear  = (aIsNegative == STL_TRUE) ? -(stlInt32)aYear : aYear;
    sDtlExpTime.mMonth = (aIsNegative == STL_TRUE) ? -(stlInt32)aMonth : aMonth;

    /**
     * value truncated check
     */
    STL_TRY( dtdIntervalValueTruncatedCheck( & sDtlExpTime,
                                             aIntervalIndicator,
                                             0, /* aFractionalSecond */
                                             aErrorStack )
             == STL_SUCCESS );
    
    /**
     * dtlIntervalYearToMonthType 
     */
    dtdDtlExpTime2IntervalYearToMonth( & sDtlExpTime,
                                       aIntervalIndicator,
                                       & sIntervalYearToMonthType );

    /**
     * leading precision 체크 
     */
    STL_TRY( dtdAdjustIntervalYearToMonth( & sIntervalYearToMonthType,
                                           aIntervalIndicator,
                                           aLeadingPrecision,
                                           aErrorStack )
             == STL_SUCCESS );

    ((dtlIntervalYearToMonthType*)(aValue->mValue))->mIndicator =
        sIntervalYearToMonthType.mIndicator;
    ((dtlIntervalYearToMonthType*)(aValue->mValue))->mMonth = sIntervalYearToMonthType.mMonth;
    
    aValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;    
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief 인자로 받은 타임정보로 dtlIntervalDayToSecondType의 dtlValue를 만든다.
 * @param[in]  aDay                      day
 * @param[in]  aHour                     hour
 * @param[in]  aMinute                   minute
 * @param[in]  aSecond                   second
 * @param[in]  aFractionalSecond         fractional second
 * @param[in]  aIsNegative               부호
 * @param[in]  aLeadingPrecision         leading precision
 * @param[in]  aFractionalSecondPrecision fractional second precision * 
 * @param[in]  aIntervalIndicator        생성하고자하는 interval indicator
 * @param[in]  aAvailableSize            mValue에 사용할 수 있는 메모리 공간의 크기
 * @param[out] aValue                    dtlDataValue
 * @param[out] aSuccessWithInfo          warning 발생 여부
 * @param[out] aErrorStack               에러 스택 
 */
stlStatus dtlIntervalDayToSecondSetValueFromTimeInfo(
    stlUInt32                    aDay,
    stlUInt32                    aHour,
    stlUInt32                    aMinute,
    stlUInt32                    aSecond,
    stlUInt32                    aFractionalSecond,
    stlBool                      aIsNegative,
    stlInt64                     aLeadingPrecision,
    stlInt64                     aFractionalSecondPrecision,
    dtlIntervalIndicator         aIntervalIndicator,
    stlInt64                     aAvailableSize,
    dtlDataValue               * aValue,
    stlBool                    * aSuccessWithInfo,
    stlErrorStack              * aErrorStack )
{
    dtlIntervalDayToSecondType  sIntervalDayToSecondType;
    dtlIntervalDayToSecondType  sTempIntervalDayToSecondType;    
    dtlExpTime                  sDtlExpTime;
    dtlFractionalSecond         sFractionalSecond;

    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_INTERVAL_DAY_TO_SECOND_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_DAY_TO_SECOND, aValue, aErrorStack );
    
    DTL_PARAM_VALIDATE( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY)            ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR)           ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE)         ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND)         ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR)    ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE)  ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND)  ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND),
                        aErrorStack );

    DTL_PARAM_VALIDATE( (aLeadingPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aLeadingPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( ( (aFractionalSecondPrecision >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                          (aFractionalSecondPrecision <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ||
                        ( aFractionalSecondPrecision == DTL_SCALE_NA ),
                        aErrorStack );

    DTL_PARAM_VALIDATE( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) )
                        ||
                        ( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) ) &&
                          ( (aFractionalSecondPrecision >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                            (aFractionalSecondPrecision <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ),
                        aErrorStack );

    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    sDtlExpTime.mDay    = (aIsNegative == STL_TRUE) ? -(stlInt32)aDay : aDay;
    sDtlExpTime.mHour   = (aIsNegative == STL_TRUE) ? -(stlInt32)aHour : aHour;
    sDtlExpTime.mMinute = (aIsNegative == STL_TRUE) ? -(stlInt32)aMinute : aMinute;
    sDtlExpTime.mSecond = (aIsNegative == STL_TRUE) ? -(stlInt32)aSecond : aSecond;
    sFractionalSecond   = (aIsNegative == STL_TRUE) ? -(stlInt32)aFractionalSecond : aFractionalSecond;

    /**
     * value truncated check
     */
    STL_TRY( dtdIntervalValueTruncatedCheck( & sDtlExpTime,
                                             aIntervalIndicator,
                                             & sFractionalSecond,
                                             aErrorStack )
             == STL_SUCCESS );
    
    /**
     * dtlIntervalDayToSecond
     */
    dtdDtlExpTime2IntervalDayToSecond( & sDtlExpTime,
                                       sFractionalSecond,
                                       aIntervalIndicator,
                                       & sIntervalDayToSecondType );


    /**
     * leading precision, fractionalSecond precision 체크 
     */
    
    sTempIntervalDayToSecondType.mIndicator = sIntervalDayToSecondType.mIndicator;
    sTempIntervalDayToSecondType.mDay       = sIntervalDayToSecondType.mDay;
    sTempIntervalDayToSecondType.mTime      = sIntervalDayToSecondType.mTime;
    
    STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecondType,
                                           aIntervalIndicator,
                                           aLeadingPrecision,
                                           aFractionalSecondPrecision,
                                           aErrorStack )
             == STL_SUCCESS );

    (((dtlIntervalDayToSecondType *)(aValue->mValue))->mIndicator) =
        sIntervalDayToSecondType.mIndicator;

    ((dtlIntervalDayToSecondType *)(aValue->mValue))->mDay = sIntervalDayToSecondType.mDay;
    ((dtlIntervalDayToSecondType *)(aValue->mValue))->mTime = sIntervalDayToSecondType.mTime;

    aValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;

    if( sIntervalDayToSecondType.mTime < sTempIntervalDayToSecondType.mTime )
    {
        *aSuccessWithInfo = STL_TRUE;
    }
    else
    {
        // Do Nothing
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief UInt64 integer value로부터 Char 타입의 value 구성. ( Null Padding하지 않는다. )
 * @param[in]  aUInt64Value       value에 저장될 입력 숫자
 * @param[in]  aPrecision      
 * @param[in]  aScale
 * @param[in]  aStringUnit        string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator INTERVAL 타입의 Indicator
 *                           <BR> dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtlCharSetValueFromUInt64IntegerWithoutPaddNull( stlUInt64               aUInt64Value,
                                                           stlInt64                aPrecision,
                                                           stlInt64                aScale,
                                                           dtlStringLengthUnit     aStringUnit,
                                                           dtlIntervalIndicator    aIntervalIndicator,
                                                           stlInt64                aAvailableSize,
                                                           dtlDataValue          * aValue,
                                                           stlBool               * aSuccessWithInfo,
                                                           dtlFuncVector         * aVectorFunc,
                                                           void                  * aVectorArg,
                                                           stlErrorStack         * aErrorStack )
{
    STL_PARAM_VALIDATE( (aPrecision >= DTL_CHAR_MIN_PRECISION) &&
                        (aPrecision <= DTL_CHAR_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( (aStringUnit > DTL_STRING_LENGTH_UNIT_NA ) &&
                        (aStringUnit < DTL_STRING_LENGTH_UNIT_MAX),
                        aErrorStack );
    
    STL_TRY( dtlVarcharSetValueFromUInt64Integer( aUInt64Value,
                                                  aPrecision,
                                                  aScale,
                                                  aStringUnit,
                                                  aIntervalIndicator,
                                                  aAvailableSize,
                                                  aValue,
                                                  aSuccessWithInfo,
                                                  aVectorFunc,
                                                  aVectorArg,
                                                  aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief UInt64 integer value로부터 Varchar 타입의 value 구성
 * @param[in]  aUInt64Value       value에 저장될 입력 숫자
 * @param[in]  aPrecision      
 * @param[in]  aScale
 * @param[in]  aStringUnit        string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator INTERVAL 타입의 Indicator
 *                           <BR> dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtlVarcharSetValueFromUInt64Integer( stlUInt64              aUInt64Value,
                                               stlInt64               aPrecision,
                                               stlInt64               aScale,
                                               dtlStringLengthUnit    aStringUnit,
                                               dtlIntervalIndicator   aIntervalIndicator,
                                               stlInt64               aAvailableSize,
                                               dtlDataValue         * aValue,
                                               stlBool              * aSuccessWithInfo,
                                               dtlFuncVector        * aVectorFunc,
                                               void                 * aVectorArg,
                                               stlErrorStack        * aErrorStack )
{
    stlInt64   sLength;

    DTL_CHECK_TYPE2( DTL_TYPE_CHAR, DTL_TYPE_VARCHAR, aValue, aErrorStack );
    STL_PARAM_VALIDATE( (aPrecision >= DTL_VARCHAR_MIN_PRECISION) &&
                        (aPrecision <= DTL_VARCHAR_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( (aStringUnit > DTL_STRING_LENGTH_UNIT_NA ) &&
                        (aStringUnit < DTL_STRING_LENGTH_UNIT_MAX),
                        aErrorStack );

    dtdUInt64ToString( aUInt64Value,
                       (stlChar *)(aValue->mValue),
                       & sLength );

    STL_TRY_THROW( aPrecision >= sLength, ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    STL_TRY_THROW( aAvailableSize >= sLength,
                   ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    aValue->mLength = sLength;

    return STL_SUCCESS;

    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
    {
        if ( aStringUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                          NULL,
                          aErrorStack );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                          NULL,
                          aErrorStack );
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief UInt64 integer value로부터 Longvarchar 타입의 value 구성
 * @param[in]  aUInt64Value       value에 저장될 입력 숫자
 * @param[in]  aPrecision      
 * @param[in]  aScale
 * @param[in]  aStringUnit        string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator INTERVAL 타입의 Indicator
 *                           <BR> dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtlLongvarcharSetValueFromUInt64Integer( stlUInt64              aUInt64Value,
                                                   stlInt64               aPrecision,
                                                   stlInt64               aScale,
                                                   dtlStringLengthUnit    aStringUnit,
                                                   dtlIntervalIndicator   aIntervalIndicator,
                                                   stlInt64               aAvailableSize,
                                                   dtlDataValue         * aValue,
                                                   stlBool              * aSuccessWithInfo,
                                                   dtlFuncVector        * aVectorFunc,
                                                   void                 * aVectorArg,
                                                   stlErrorStack        * aErrorStack )
{
    stlInt64   sLength;

    DTL_CHECK_TYPE( DTL_TYPE_LONGVARCHAR, aValue, aErrorStack );
    /* DTL_PARAM_VALIDATE( aPrecision == DTL_LONGVARCHAR_MAX_PRECISION, aErrorStack ); */
    STL_PARAM_VALIDATE( aStringUnit == DTL_STRING_LENGTH_UNIT_OCTETS, aErrorStack );

    dtdUInt64ToString( aUInt64Value,
                       (stlChar *)(aValue->mValue),
                       & sLength );

    STL_TRY_THROW( aAvailableSize >= sLength,
                   ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    aValue->mLength = sLength;

    return STL_SUCCESS;

    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
    {
        if ( aStringUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS )
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                          NULL,
                          aErrorStack );
        }
        else
        {
            stlPushError( STL_ERROR_LEVEL_ABORT,
                          DTL_ERRCODE_C_STRING_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                          NULL,
                          aErrorStack );
        }
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief UInt64 integer value로부터 number 타입의 value 구성
 * @param[in]  aUInt64Value       value에 저장될 입력 숫자
 * @param[in]  aPrecision      
 * @param[in]  aScale
 * @param[in]  aStringUnit        string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator INTERVAL 타입의 Indicator
 *                           <BR> dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 */
stlStatus dtlNumericSetValueFromUInt64Integer( stlUInt64              aUInt64Value,
                                               stlInt64               aPrecision,
                                               stlInt64               aScale,
                                               dtlStringLengthUnit    aStringUnit,
                                               dtlIntervalIndicator   aIntervalIndicator,
                                               stlInt64               aAvailableSize,
                                               dtlDataValue         * aValue,
                                               stlBool              * aSuccessWithInfo,
                                               dtlFuncVector        * aVectorFunc,
                                               void                 * aVectorArg,
                                               stlErrorStack        * aErrorStack )
{
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aValue, aErrorStack );
    STL_PARAM_VALIDATE( ((aPrecision >= DTL_NUMERIC_MIN_PRECISION) &&
                         (aPrecision <= DTL_NUMERIC_MAX_PRECISION)),
                        aErrorStack );
    STL_PARAM_VALIDATE( aAvailableSize >= DTL_NUMERIC_SIZE( aPrecision ),
                        aErrorStack );
    
    *aSuccessWithInfo = STL_FALSE;
    
    STL_TRY( dtdToNumericFromUInt64( aUInt64Value,
                                     aPrecision,
                                     aScale,
                                     aValue,
                                     aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Numeric value로부터 변환된 문자열의 길이
 * @param[in]  aNumeric            Numeric value
 * @param[out] aValidNumberLength  유효숫자( 소수점 및 trailing zero제외) 문자열의 길이
 * @param[out] aEntirePrintLength  전체 숫자를 그대로 표현할때 문자열의 길이
 * @param[out] aExpPrintLength     전체 숫자를 exponent 형태로 표현할때 문자열의 길이
 * @param[out] aErrorStack         에러 스택
 */
stlStatus dtlGetStringLengthFromNumeric( dtlDataValue    * aNumeric,
                                         stlInt64        * aValidNumberLength,
                                         stlInt64        * aEntirePrintLength,
                                         stlInt64        * aExpPrintLength,
                                         stlErrorStack   * aErrorStack )
{
    dtlNumericType    * sNumeric;
    stlInt16            sIsPositive;
    stlUInt8          * sDigit;
    stlUInt8            sDigitLen;
    stlInt32            sExponent;
    stlInt32            sRealExponent;
    stlInt32            sZeroLen = 0;
    stlBool             sIsOdd = STL_FALSE;
    dtlExponentString * sExpoStrInfo;
    
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aNumeric, aErrorStack );
    STL_PARAM_VALIDATE( aValidNumberLength != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aEntirePrintLength != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aExpPrintLength != NULL, aErrorStack );

    sNumeric = DTF_NUMERIC( aNumeric );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric, aNumeric->mLength ) == STL_TRUE )
    {
        *aValidNumberLength = 1;
        *aEntirePrintLength = 1;
        *aExpPrintLength    = 1;
    }
    else
    { 
        /* sign */
        sIsPositive         = DTL_NUMERIC_IS_POSITIVE( sNumeric );
        *aEntirePrintLength = !sIsPositive;
        *aExpPrintLength    = !sIsPositive;

        /* 유효 숫자 길이 */
        sDigitLen    = DTL_NUMERIC_GET_DIGIT_COUNT( aNumeric->mLength );
        sDigit       = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
        sExponent    = DTL_NUMERIC_GET_EXPONENT( sNumeric );       
        sRealExponent    = sExponent << 1;
        sIsOdd       = ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' );
        sRealExponent   += (!sIsOdd);
        sExpoStrInfo = (dtlExponentString *) & dtdNumberExponentString[ sRealExponent ];

        if( sExponent >= sDigitLen - 1 )
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : 3
             * String : 1234000
             */
            
            /* digits : truncate leading zero & add trailing zeroes */
            sZeroLen = ( sExponent - sDigitLen + 1 ) << 1;
            *aEntirePrintLength += ( ( sZeroLen + ( sDigitLen << 1 ) ) -
                                     ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) );

            *aExpPrintLength += ( sDigitLen << 1 ) -
                ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit)[ 0 ] == '0' ) -
                ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen -1 ])[ 1 ] == '0' ) +
                sExpoStrInfo->mStringCnt + 1 /* decimal point, 'E', exp sign */;

            *aValidNumberLength = ( sDigitLen << 1 ) + (!sIsPositive) -
                ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) -
                ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen -1 ] )[ 1 ] == '0' ); 
        }
        else
        {
            if( sExponent < 0 )
            {
                /**
                 * < Example >
                 * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
                 * Expo   : -2
                 * String : .0001234
                 */
                
                /* decimal point 포함 */
                /* digits : truncate trailing zero */
                *aEntirePrintLength += ( ( ( sDigitLen - sExponent - 1 ) << 1 ) -
                                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen - 1 ] )[ 1 ] == '0' ) );
                
                /* decimal point */
                (*aEntirePrintLength)++;

                *aExpPrintLength += ( sDigitLen << 1 ) -
                    ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit)[ 0 ] == '0' ) -
                    ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen -1 ])[ 1 ] == '0' ) +
                    sExpoStrInfo->mStringCnt + 1 /* decimal point, 'E', exp sign */;

                *aValidNumberLength = 1 + (!sIsPositive);         
            }
            else
            {
                /**
                 * < Example >
                 * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
                 * Expo   : 1
                 * String : 123.4
                 */

                /* decimal point 포함 */
                /* digits : truncate leading zero & truncate trailing zero */
                *aEntirePrintLength += ( ( sDigitLen << 1 ) -
                                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) -
                                         ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen - 1 ] )[ 1 ] == '0' ) );
                
                /* decimal point */
                (*aEntirePrintLength)++;

                *aExpPrintLength += ( sDigitLen << 1 ) -
                    ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit)[ 0 ] == '0' ) -
                    ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen -1 ])[ 1 ] == '0' ) +
                    sExpoStrInfo->mStringCnt + 1 /* decimal point, 'E', exp sign */;

                if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' )
                {
                    /**
                     * < Example >
                     * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
                     * Expo   : 1
                     * String : 123.4
                     */
                    *aValidNumberLength = ( sExponent << 1 ) + (!sIsPositive) + 1;
                }
                else
                {
                    /**
                     * < Example >
                     * Digits : 12.345   ( 12 | 34 | 50, length = 3 )
                     * Expo   : 2
                     * String : 1234.5
                     */
                    *aValidNumberLength = ( sExponent << 1 ) + (!sIsPositive) + 2;
                }
            }
        }
    }

    /*
     * Digits : 6399999999999999999999999999999999999936
     * Expo   : 123
     * String : -6.399999999999999999999999999999999999936E+123
     * digit의 갯수가 38 를 초과 하는 경우 round truncate 된다는 가정하에
     * ExpPrintLength 를 DTL_NUMERIC_STRING_MAX_SIZE 로 제한 한다.
     */
    *aExpPrintLength = *aExpPrintLength > DTL_NUMERIC_STRING_MAX_SIZE -1 ?
        DTL_NUMERIC_STRING_MAX_SIZE -1 : *aExpPrintLength;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Numeric value로부터 유효한 숫자 길이를 반환
 * @param[in]  aNumeric             Numeric value
 * @param[out] aValidNumberLength   유효한 숫자( 소수점 및 trailing zero 제외 ) 길이
 * @param[out] aErrorStack          에러 스택
 * @remark value 의 유효숫자중 소수점에 해당하는 부분을 제외한 부분을 유효한 숫자로 정의 하며 <br>
 *         trailing zero 있는 ( 정수타입 값 ) 경우 trailing zero 를 제외한 값
 */
stlStatus dtlGetValidNumberLengthFromNumeric( dtlDataValue    * aNumeric,
                                              stlInt64        * aValidNumberLength,
                                              stlErrorStack   * aErrorStack )
{
    dtlNumericType  * sNumeric;
    stlInt16          sIsPositive;
    stlUInt8        * sDigit;
    stlUInt8          sDigitLen;
    stlInt32          sExponent;
    
    DTL_CHECK_TYPE2( DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, aNumeric, aErrorStack );
    STL_PARAM_VALIDATE( aValidNumberLength != NULL, aErrorStack );

    sNumeric = DTF_NUMERIC( aNumeric );
    
    if( DTL_NUMERIC_IS_ZERO( sNumeric, aNumeric->mLength ) == STL_TRUE )
    {
        *aValidNumberLength  = 1;
    }
    else
    {
        /* sign */
        sIsPositive = DTL_NUMERIC_IS_POSITIVE( sNumeric );

        /* 유효 숫자 길이 */
        sDigitLen = DTL_NUMERIC_GET_DIGIT_COUNT( aNumeric->mLength );
        sDigit    = DTL_NUMERIC_GET_DIGIT_PTR( sNumeric );
        sExponent = DTL_NUMERIC_GET_EXPONENT( sNumeric );

        if( sExponent >= sDigitLen - 1 )
        {
            /**
             * < Example >
             * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
             * Expo   : 3
             * String : 1234000
             */

            *aValidNumberLength = ( sDigitLen << 1 ) + (!sIsPositive) -
                ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' ) -
                ( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, sDigit[ sDigitLen -1 ] )[ 1 ] == '0' );  
        }
        else
        {
            if( sExponent < 0 )
            {
                /**
                 * < Example >
                 * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
                 * Expo   : -2
                 * String : .0001234
                 */

                *aValidNumberLength = 1 + (!sIsPositive);         
            }
            else
            {
                if( DTD_NUMERIC_GET_DIGIT_STRING( sIsPositive, *sDigit )[ 0 ] == '0' )
                {
                    /**
                     * < Example >
                     * Digits : 1.2340   ( 01 | 23 | 40, length = 3 )
                     * Expo   : 1
                     * String : 123.4
                     */
                    *aValidNumberLength = ( sExponent << 1 ) + (!sIsPositive) + 1;
                }
                else
                {
                    /**
                     * < Example >
                     * Digits : 12.345   ( 12 | 34 | 50, length = 3 )
                     * Expo   : 2
                     * String : 1234.5
                     */
                    *aValidNumberLength = ( sExponent << 1 ) + (!sIsPositive) + 2;
                }
            }
        }
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief string value로부터 dtdIntervalYearToMonth 타입의 value 구성
 *   <BR> truncated되는 부분이 있는지 체크하지 않고,
 *   <BR> parsing된 정보로 dtdIntervalDayToSecond 타입의 value를 구성한다.
 *   <BR> 이 함수는 주로 Gliese Library의 BindColumn 변환 함수에서 사용할 목적으로 만들어짐.
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이  
 * @param[in]  aPrecision         precision
 *                           <BR> interval type에서는 leading field precision 정보 
 * @param[in]  aScale             scale
 *                           <BR> interval type에서는 fractional second precision 정보
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator INTERVAL 타입의 Indicator
 *                           <BR> dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 *
 * @remark
 * YEAR, MONTH, YEAR TO MONTH interval type에 string으로 입력 가능한 포멧은 다음과 같다.
 * @code
 *  (1) year-month
 *      '1-2'  =>  ( 1 year 2 month )
 *     예) YEAR  => '1-0' ( O ), '+1-0' ( O ), '-1-1' ( O ),  '1-9' ( X )
 *     예) MONTH => '1-0' ( O ), '0-12' ( O ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
 *     예) YEAR TO MONTH => '1-0' ( O ), '0-12' ( O ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
 *  (2) number
 *     예1) INTERVAL YEAR에 '3' 입력시,
 *          '3'을 YEAR의 값으로 인식해 +3-00으로 데이타가 들어감.
 *     예2) INTERVAL YEAR TO MONTH에 '3' 입력시,
 *          '3'을 month의 값으로 인식해 +0-03으로 데이타가 들어감.
 *          (즉, interval의 두번째 필드의 값으로 인식함.)
 * @endcode
 */
stlStatus dtlIntervalYearToMonthSetNotTruncatedValueFromString(
    stlChar              * aString,
    stlInt64               aStringLength,
    stlInt64               aPrecision,
    stlInt64               aScale,
    dtlStringLengthUnit    aStringUnit,
    dtlIntervalIndicator   aIntervalIndicator,
    stlInt64               aAvailableSize,
    dtlDataValue         * aValue,
    stlBool              * aSuccessWithInfo,
    dtlFuncVector        * aVectorFunc,
    void                 * aVectorArg,
    stlErrorStack        * aErrorStack )
{
    dtlIntervalYearToMonthType   sIntervalYearToMonthType;
    dtlExpTime                   sDtlExpTime;
    dtlFractionalSecond          sFractionalSecond;
    stlChar                      sBuffer[256];
    stlChar                    * sField[DTL_MAX_DATE_FIELDS];
    stlInt32                     sFieldType[DTL_MAX_DATE_FIELDS];
    stlInt32                     sNumFields;
    stlInt32                     sDateType;

    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_INTERVAL_YEAR_TO_MONTH_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_YEAR_TO_MONTH, aValue, aErrorStack );

    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR)  ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH) ||
                        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH),
                        aErrorStack );

    sIntervalYearToMonthType.mIndicator = aIntervalIndicator;

    DTL_INIT_DTLEXPTIME( sDtlExpTime );    
    
    /*
     * INTERVAL의 aPrecision, aScale은 다음과 같은 용도로 사용된다.
     * aPrecision => interval leading field precision
     * aScale     => interval fractional second precision
     * 예)interval year[(year precision = aPrecision)] to month
     * 예)interval day[(day precision = aPrecision)] to second[(fraction_second_precision = aScale)]
     */

    STL_TRY( dtdParseDateTime( aString,
                               aStringLength,
                               DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
                               sBuffer,
                               STL_SIZEOF(sBuffer),
                               sField,
                               sFieldType,
                               DTL_MAX_DATE_FIELDS,
                               & sNumFields,
                               aErrorStack )
             == STL_SUCCESS );

    /*
     * input string에 대한 체크
     *  YEAR, MONTH, YEAR TO MONTH interval type이외의 불필요한 정보가 있는 경우 에러처리한다.
     * 예) '1-2' ( O )
     *     '1-2 3 11:11:11.12345' ( X )
     */

    /*
     * YEAR, MONTH, YEAR TO MONTH interval type에 string으로 입력 가능한 포멧은 다음과 같다.
     *  (1) year-month
     *      '1-2'  =>  ( 1 year 2 month )
     *     예) YEAR  => '1-0' ( O ), '+1-0' ( O ), '-1-1' ( O ),  '1-9' ( X )
     *     예) MONTH => '1-0' ( O ), '0-12' ( O ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
     *     예) YEAR TO MONTH => '1-0' ( O ), '0-12' ( O ), '1-1' ( O ), '+1-1' ( O ), '-1-1' ( O )
     *  (2) number
     *      number는 single field에만 해당됨. ( INTERVAL YEAR, INTERVAL MONTH )
     *     예1) INTERVAL YEAR에 '3' 입력시,
     *          '3'을 YEAR의 값으로 인식해 +3-00으로 데이타가 들어감.
     */

    STL_TRY_THROW( (sNumFields == 1) &&
                   ((sFieldType[0] == DTL_DTK_DATE) ||
                    (sFieldType[0] == DTL_DTK_TZ)   ||
                    (sFieldType[0] == DTL_DTK_NUMBER)),
                   RAMP_ERROR_BAD_FORMAT );

    if( sFieldType[0] == DTL_DTK_TZ )
    {
        if( stlStrchr((sField[0] + 1), '-') != NULL )
        {
            // Do Nothing
        }
        else if( stlStrchr((sField[0] + 1), ':') != NULL )
        {
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
        }
        else
        {
            sFieldType[0] = DTL_DTK_NUMBER;
        }
    }
    else
    {
        // Do Nothing
    }
    
    if( sFieldType[0] == DTL_DTK_NUMBER )
    {
        STL_TRY_THROW( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                       (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH),
                       RAMP_ERROR_BAD_FORMAT );
    }
    else
    {
        // Do Nothing
    }

    STL_TRY( dtdDecodeInterval( sField,
                                sFieldType,
                                sNumFields,
                                aIntervalIndicator,
                                & sDateType,
                                & sDtlExpTime,
                                & sFractionalSecond,
                                aErrorStack )
             == STL_SUCCESS );

    switch( sDateType )
    {
        case DTL_DTK_DELTA:
            {
                dtdDtlExpTime2IntervalYearToMonth( & sDtlExpTime,
                                                   aIntervalIndicator,
                                                   & sIntervalYearToMonthType );
            }
            break;
        default:
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
            break;
    }
    
    STL_TRY( dtdAdjustIntervalYearToMonth( & sIntervalYearToMonthType,
                                           aIntervalIndicator,
                                           aPrecision,
                                           aErrorStack )
             == STL_SUCCESS );
    
    ((dtlIntervalYearToMonthType *)(aValue->mValue))->mIndicator =
        sIntervalYearToMonthType.mIndicator;
    ((dtlIntervalYearToMonthType *)(aValue->mValue))->mMonth = sIntervalYearToMonthType.mMonth;

    aValue->mLength = DTL_INTERVAL_YEAR_TO_MONTH_SIZE;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_BAD_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_NOT_VALID_INTERVAL_LITERAL,
                      NULL,
                      aErrorStack );
    };

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief string value로부터 dtdIntervalDayToSecond 타입의 value 구성
 *   <BR> truncated되는 부분이 있는지 체크하지 않고,
 *   <BR> parsing된 정보로 dtdIntervalDayToSecond 타입의 value를 구성한다.
 *   <BR> 이 함수는 주로 Gliese Library의 BindColumn 변환 함수에서 사용할 목적으로 만들어짐.
 * @param[in]  aString            value에 저장될 입력 문자열
 * @param[in]  aStringLength      aString의 길이  
 * @param[in]  aPrecision         precision
 *                           <BR> interval type에서는 leading field precision 정보 
 * @param[in]  aScale             scale
 *                           <BR> interval type에서는 fractional second precision 정보
 * @param[in]  aStringUnit      string length unit (dtlStringLengthUnit 참조) 
 * @param[in]  aIntervalIndicator INTERVAL 타입의 Indicator
 *                           <BR> dtlIntervalIndicator 참조 
 * @param[in]  aAvailableSize     mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue             dtlDataValue의 주소
 * @param[out] aSuccessWithInfo   warning 발생 여부
 * @param[in]  aVectorFunc        Function Vector
 * @param[in]  aVectorArg         Vector Argument
 * @param[out] aErrorStack        에러 스택
 *
 * @remark
 * DAY, HOUR, MINUTE, SECOND,
 * DAY TO HOUR, DAY TO MINUTE, DAY TO SECOND,
 * HOUR TO MINUTE, HOUR TO SECOND,
 * MINUTE TO SECOND interval type에 string으로 입력 가능한 포멧은 다음과 같다.
 * @code
 *  (1) day hour:minute:second.fractionalsecond
 *      '3 10:11:12.123456' ( 3 day 10 hour 11 minute 12.123456 second )
 *  (2) time
 *      '10:11:12.123456'
 *  (3) number
 *      예1) INTERVAL DAY에 '3' 입력시,
 *          '3'을 DAY의 값으로 인식해서 +3 00:00:00으로 데이타가 들어감.
 *      예1) INTERVAL DAY TO SECOND 에 '3' 입력시,
 *          '3'을 SECOND의 값으로 인식해서 +0 00:00:03으로 데이타가 들어감.
 *          (즉, interval의 두번째 필드의 값으로 인식함.)
 *  (4) time zone시간대
 *      '+09:00'
 * @endcode
 */
stlStatus dtlIntervalDayToSecondSetNotTruncatedValueFromString(
    stlChar              * aString,
    stlInt64               aStringLength,
    stlInt64               aPrecision,
    stlInt64               aScale,
    dtlStringLengthUnit    aStringUnit,
    dtlIntervalIndicator   aIntervalIndicator,
    stlInt64               aAvailableSize,
    dtlDataValue         * aValue,
    stlBool              * aSuccessWithInfo,
    dtlFuncVector        * aVectorFunc,
    void                 * aVectorArg,
    stlErrorStack        * aErrorStack )
{
    dtlIntervalDayToSecondType  sIntervalDayToSecondType;
    dtlIntervalDayToSecondType  sTempIntervalDayToSecondType;
    dtlExpTime                  sDtlExpTime;
    dtlFractionalSecond         sFractionalSecond;
    stlChar                     sBuffer[256];
    stlChar                   * sField[DTL_MAX_DATE_FIELDS];
    stlInt32                    sFieldType[DTL_MAX_DATE_FIELDS];
    stlInt32                    sNumFields;
    stlInt32                    sDateType;

    DTL_PARAM_VALIDATE( aAvailableSize >= DTL_INTERVAL_DAY_TO_SECOND_SIZE, aErrorStack );
    DTL_CHECK_TYPE( DTL_TYPE_INTERVAL_DAY_TO_SECOND, aValue, aErrorStack );

    DTL_PARAM_VALIDATE( aString != NULL, aErrorStack );

    DTL_PARAM_VALIDATE( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        == STL_TRUE,
                        aErrorStack );

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    DTL_PARAM_VALIDATE( ( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                          (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ||
                        ( aScale == DTL_SCALE_NA ),
                        aErrorStack );

    DTL_PARAM_VALIDATE( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) )
                        ||
                        ( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                            (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) ) &&
                          ( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                            (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) ) ),
                        aErrorStack );

    sIntervalDayToSecondType.mIndicator = aIntervalIndicator;

    DTL_INIT_DTLEXPTIME( sDtlExpTime );

    /*
     * INTERVAL의 aPrecision, aScale은 다음과 같은 용도로 사용된다.
     * aPrecision => interval leading field precision
     * aScale     => interval fractional second precision
     * 예)interval year[(year precision = aPrecision)] to month
     * 예)interval day[(day precision = aPrecision)] to second[(fraction_second_precision = aScale)]
     */

    STL_TRY( dtdParseDateTime( aString,
                               aStringLength,
                               DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                               sBuffer,
                               STL_SIZEOF(sBuffer),
                               sField,
                               sFieldType,
                               DTL_MAX_DATE_FIELDS,
                               & sNumFields,
                               aErrorStack )
             == STL_SUCCESS );

    /*
     * input string에 대한 체크
     * DAY, HOUR, MINUTE, SECOND,
     * DAY TO HOUR, DAY TO MINUTE, DAY TO SECOND,
     * HOUR TO MINUTE, HOUR TO SECOND,
     * MINUTE TO SECOND interval type이외의 불필요한 정보가 있는 경우 에러처리한다.
     * 예) '3 11:22:33.123456' ( O )
     *     '1-2 3 11:22:33.12345' ( X )
     */

    /*
     * DAY, HOUR, MINUTE, SECOND,
     * DAY TO HOUR, DAY TO MINUTE, DAY TO SECOND,
     * HOUR TO MINUTE, HOUR TO SECOND,
     * MINUTE TO SECOND interval type에 string으로 입력 가능한 포멧은 다음과 같다.
     *  (1) day hour:minute:second.fractionalsecond
     *      '3 10:11:12.123456' ( 3 day 10 hour 11 minute 12.123456 second )
     *  (2) time
     *      '10:11:12.123456'
     *  (3) number
     *      number는 single field에만 해당됨.
     *      ( INTERVAL DAY, INTERVAL HOUR, INTERVAL MINUTE, INTERVAL SECOND )
     *      예1) INTERVAL DAY에 '3' 입력시,
     *          '3'을 DAY의 값으로 인식해서 +3 00:00:00으로 데이타가 들어감.
     *  (4) time zone시간대
     *      '+09:00'
     */
    switch( sNumFields )
    {
        case 2:
            {
                STL_TRY_THROW( sFieldType[1] == DTL_DTK_TIME,
                               RAMP_ERROR_BAD_FORMAT );

                STL_TRY_THROW( (sFieldType[0] == DTL_DTK_NUMBER) ||
                               (sFieldType[0] == DTL_DTK_TZ),
                               RAMP_ERROR_BAD_FORMAT );

                break;
            }
        case 1:
            {
                STL_TRY_THROW( (sFieldType[0] == DTL_DTK_TIME) ||
                               (sFieldType[0] == DTL_DTK_NUMBER) ||
                               (sFieldType[0] == DTL_DTK_TZ),
                               RAMP_ERROR_BAD_FORMAT );

                if( sFieldType[0] == DTL_DTK_TZ )
                {
                    if( stlStrchr((sField[0] + 1), ':') != NULL )
                    {
                        // Do Nothing
                    }
                    else
                    {
                        sFieldType[0] = DTL_DTK_NUMBER;
                    }
                }
                else
                {
                    // Do Nothing
                }

                if( sFieldType[0] == DTL_DTK_NUMBER )
                {
                    STL_TRY_THROW( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY)    ||
                                   (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR)   ||
                                   (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                                   (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND),
                                   RAMP_ERROR_BAD_FORMAT );
                }
                else
                {
                    // Do Nothing
                }

                break;
            }
        default:
            {
                STL_THROW( RAMP_ERROR_BAD_FORMAT );
                break;
            }
    }
    
    STL_TRY( dtdDecodeInterval( sField,
                                sFieldType,
                                sNumFields,
                                aIntervalIndicator,
                                & sDateType,
                                & sDtlExpTime,
                                & sFractionalSecond,
                                aErrorStack )
             == STL_SUCCESS );
    
    switch( sDateType )
    {
        case DTL_DTK_DELTA:
            {
                dtdDtlExpTime2IntervalDayToSecond( & sDtlExpTime,
                                                   sFractionalSecond,
                                                   aIntervalIndicator,
                                                   & sIntervalDayToSecondType );
            }
            break;
        default:
            STL_THROW( RAMP_ERROR_BAD_FORMAT );
            break;
    }

    sTempIntervalDayToSecondType.mIndicator = sIntervalDayToSecondType.mIndicator;
    sTempIntervalDayToSecondType.mDay       = sIntervalDayToSecondType.mDay;
    sTempIntervalDayToSecondType.mTime      = sIntervalDayToSecondType.mTime;
    
    STL_TRY( dtdAdjustIntervalDayToSecond( & sIntervalDayToSecondType,
                                           aIntervalIndicator,
                                           aPrecision,
                                           aScale,
                                           aErrorStack )
             == STL_SUCCESS );    
    
    (((dtlIntervalDayToSecondType *)(aValue->mValue))->mIndicator) =
        sIntervalDayToSecondType.mIndicator;

    ((dtlIntervalDayToSecondType *)(aValue->mValue))->mDay = sIntervalDayToSecondType.mDay;
    ((dtlIntervalDayToSecondType *)(aValue->mValue))->mTime = sIntervalDayToSecondType.mTime;

    aValue->mLength = DTL_INTERVAL_DAY_TO_SECOND_SIZE;

    if( sIntervalDayToSecondType.mTime != sTempIntervalDayToSecondType.mTime )
    {
        *aSuccessWithInfo = STL_TRUE;
    }
    else
    {
        // Do Nothing
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_BAD_FORMAT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_NOT_VALID_INTERVAL_LITERAL,
                      NULL,
                      aErrorStack );
    };

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief objectid, tablespace id, page id, page off 정보로 ROWID value 정보를 구성한다.
 * @param[in]  aObjectId     Table Id
 * @param[in]  aTbsId        Tablespace Id
 * @param[in]  aPageId       Page Id
 * @param[in]  aRowNumber    aRowNumber (Page Offset)
 * @param[out] aValue        rowid value 정보를 저장할 dtlDataValue
 * @param[in]  aErrorStack   에러스택
 */
stlStatus dtlRowIdSetValueFromRowIdInfo( stlInt64        aObjectId,
                                         stlUInt16       aTbsId,
                                         stlUInt32       aPageId,
                                         stlInt16        aRowNumber,
                                         dtlDataValue  * aValue,
                                         stlErrorStack * aErrorStack )
{
    dtlRowIdType  * sRowIdType;
    
    DTL_CHECK_TYPE( DTL_TYPE_ROWID, aValue, aErrorStack );

    sRowIdType = (dtlRowIdType *)(aValue->mValue);

    /*
     * Table ID : stlInt64
     */

    DTD_OBJECT_ID_TO_ROWID_DIGIT( aObjectId, sRowIdType );
    
    /*
     * Tablespace ID : stlUInt16
     */

    DTD_TABLESPACE_ID_TO_ROWID_DIGIT( aTbsId, sRowIdType );

    /*
     * Page ID : stlUInt32
     */

    DTD_PAGE_ID_TO_ROWID_DIGIT( aPageId, sRowIdType );
    
    /*
     * Page OFFSET : stlInt16
     */

    DTD_ROW_NUMBER_TO_ROWID_DIGIT( aRowNumber, sRowIdType );
    
    aValue->mLength = DTL_ROWID_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief dtfNumericAddition의 Wrapping 함수
 * @param[in]  aValue1      dtfNumericType
 * @param[in]  aValue2      dtfNumericType
 * @param[out] aResult      덧셈 연산 결과
 * @param[out] aErrorStack  에러 스택
 */
stlStatus dtlNumAddition( dtlDataValue   * aValue1,
                          dtlDataValue   * aValue2,
                          dtlDataValue   * aResult,
                          stlErrorStack  * aErrorStack )
{
    return dtfNumericAddition( aValue1,
                               aValue2,
                               aResult,
                               aErrorStack );
}


/**
 * @brief dtfNumericMultiplication의 Wrapping 함수
 * @param[in]  aValue1      dtfNumericType
 * @param[in]  aValue2      dtfNumericType
 * @param[out] aResult      덧셈 연산 결과
 * @param[out] aErrorStack  에러 스택
 */
stlStatus dtlNumericMultiplication( dtlDataValue   * aValue1,
                                    dtlDataValue   * aValue2,
                                    dtlDataValue   * aResult,
                                    stlErrorStack  * aErrorStack )
{
    return dtfNumericMultiplication( aValue1,
                                     aValue2,
                                     aResult,
                                     aErrorStack );
}


/**
 * @brief dtlExpTime을 leadng/second precision에 자리수에 맞추어 문자열로 변환
 * @param[in]  aDtlExpTime        dtlExpTime
 * @param[in]  aLeadingPrecision  leading field precision
 * @param[out] aStr               변환된 string을 저장할 공간
 * @param[out] aLength            변환된 문자열의 길이
 * @param[out] aErrorStack        에러 스택
 * @remark
 *    <BR> interval year(6) to month 의 data 인 경우,
 *    <BR> precision 자리수에 맞추어 string으로 변환함.
 *    <BR> 예) +000009-01
 */
stlStatus dtlIntervalYearToMonthToFormatString( dtlExpTime      * aDtlExpTime,
                                                stlInt64          aLeadingPrecision,
                                                stlChar         * aStr,
                                                stlInt64        * aLength,
                                                stlErrorStack   * aErrorStack )
{
    stlChar   * sStr;
    stlInt32    sYear;
    stlInt32    sMonth;
    stlInt32    sSize = 0;
    stlInt32    sUintFormatStrSize = 0;
    
    sStr = aStr;
    sYear = aDtlExpTime->mYear;
    sMonth = aDtlExpTime->mMonth;

    if( aLeadingPrecision == DTL_PRECISION_NA )
    {
        /*
         * leading precision이 DTL_PRECISION_NA 로 오는 경우,
         * leading max precision 으로 처리한다.
         */
        aLeadingPrecision = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
    }
    else
    {
        /* Do Nothing */
    }

    DTL_PARAM_VALIDATE( (aLeadingPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aLeadingPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    
    if( (sYear < 0) || (sMonth < 0) )
    {
        *(sStr + sSize) = '-';
        sSize++;
        sYear = (sYear < 0) ? -sYear : sYear;
        sMonth = (sMonth < 0) ? -sMonth : sMonth;
    }
    else
    {
        *(sStr + sSize) = '+';
        sSize++;
    }

    /*
     * interval year, interval month, interval year to month 에 구분하지 않고,
     * leading precision은 string으로 변환했을때 최상위표현(year)의 자리수로 지정한다.
     * 이는 oracle 과 동일한 표현방식임.
     *
     * 예1) interval year(6)
     *      select interval'9'year(6) from dual; => +000009-00
     * 예2) interval month(6)
     *      select interval'9'month(6) from dual; => +000000-09
     * 예3) interval year(6) to month
     *      select interval'9-9'year(6) to month from dual; => +000009-09
     * 
     */ 
    STL_TRY( dtdIntegerToLeadingZeroFormatString( sYear,
                                                  aLeadingPrecision,
                                                  & sUintFormatStrSize,
                                                  sStr + sSize,
                                                  aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;
                
    *(sStr + sSize) = '-';
    sSize++;
    
    STL_TRY( dtdIntegerToLeadingZeroFormatString( sMonth,
                                                  2,
                                                  & sUintFormatStrSize,
                                                  sStr + sSize,
                                                  aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;

    *aLength = sSize;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief dtlExpTime을 leadng/second precision에 자리수에 맞추어 utf16 문자열로 변환
 * @param[in]  aDtlExpTime        dtlExpTime
 * @param[in]  aLeadingPrecision  leading field precision
 * @param[out] aStr               변환된 string을 저장할 공간
 * @param[out] aLength            변환된 문자열의 길이
 * @param[out] aErrorStack        에러 스택
 * @remark
 *    <BR> interval year(6) to month 의 data 인 경우,
 *    <BR> precision 자리수에 맞추어 string으로 변환함.
 *    <BR> 예) +000009-01
 */
stlStatus dtlIntervalYearToMonthToFormatUtf16String( dtlExpTime      * aDtlExpTime,
                                                     stlInt64          aLeadingPrecision,
                                                     stlUInt16       * aStr,
                                                     stlInt64        * aLength,
                                                     stlErrorStack   * aErrorStack )
{
    stlUInt16 * sStr;
    stlInt32    sYear;
    stlInt32    sMonth;
    stlInt32    sSize = 0;
    stlInt32    sUintFormatStrSize = 0;
    stlInt32    sOneCharSize = STL_UINT16_SIZE;
    
    sStr = aStr;
    sYear = aDtlExpTime->mYear;
    sMonth = aDtlExpTime->mMonth;

    /* if( aLeadingPrecision == DTL_PRECISION_NA ) */
    /* { */
    /*     /\* */
    /*      * leading precision이 DTL_PRECISION_NA 로 오는 경우, */
    /*      * leading max precision 으로 처리한다. */
    /*      *\/ */
    /*     aLeadingPrecision = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION; */
    /* } */
    /* else */
    /* { */
    /*     /\* Do Nothing *\/ */
    /* } */

    DTL_PARAM_VALIDATE( (aLeadingPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aLeadingPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    
    if( (sYear < 0) || (sMonth < 0) )
    {
        *sStr = '-';
        sSize = sOneCharSize;
        sYear = (sYear < 0) ? -sYear : sYear;
        sMonth = (sMonth < 0) ? -sMonth : sMonth;
    }
    else
    {
        *sStr = '+';
        sSize = sOneCharSize;
    }

    /*
     * interval year, interval month, interval year to month 에 구분하지 않고,
     * leading precision은 string으로 변환했을때 최상위표현(year)의 자리수로 지정한다.
     * 이는 oracle 과 동일한 표현방식임.
     *
     * 예1) interval year(6)
     *      select interval'9'year(6) from dual; => +000009-00
     * 예2) interval month(6)
     *      select interval'9'month(6) from dual; => +000000-09
     * 예3) interval year(6) to month
     *      select interval'9-9'year(6) to month from dual; => +000009-09
     * 
     */ 
    STL_TRY( dtdIntegerToLeadingZeroFormatUtf16String( sYear,
                                                       aLeadingPrecision,
                                                       & sUintFormatStrSize,
                                                       sStr + (sSize/sOneCharSize),
                                                       aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;
                
    *(sStr + (sSize/sOneCharSize)) = '-';
    sSize = sSize + sOneCharSize;
    
    STL_TRY( dtdIntegerToLeadingZeroFormatUtf16String( sMonth,
                                                       2,
                                                       & sUintFormatStrSize,
                                                       sStr + (sSize/sOneCharSize),
                                                       aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;

    *aLength = sSize;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief dtlExpTime을 leadng/second precision에 자리수에 맞추어 utf32 문자열로 변환
 * @param[in]  aDtlExpTime        dtlExpTime
 * @param[in]  aLeadingPrecision  leading field precision
 * @param[out] aStr               변환된 string을 저장할 공간
 * @param[out] aLength            변환된 문자열의 길이
 * @param[out] aErrorStack        에러 스택
 * @remark
 *    <BR> interval year(6) to month 의 data 인 경우,
 *    <BR> precision 자리수에 맞추어 string으로 변환함.
 *    <BR> 예) +000009-01
 */
stlStatus dtlIntervalYearToMonthToFormatUtf32String( dtlExpTime      * aDtlExpTime,
                                                     stlInt64          aLeadingPrecision,
                                                     stlUInt32       * aStr,
                                                     stlInt64        * aLength,
                                                     stlErrorStack   * aErrorStack )
{
    stlUInt32 * sStr;
    stlInt32    sYear;
    stlInt32    sMonth;
    stlInt32    sSize = 0;
    stlInt32    sUintFormatStrSize = 0;
    stlInt32    sOneCharSize = STL_UINT32_SIZE;
    
    sStr = aStr;
    sYear = aDtlExpTime->mYear;
    sMonth = aDtlExpTime->mMonth;

    /* if( aLeadingPrecision == DTL_PRECISION_NA ) */
    /* { */
    /*     /\* */
    /*      * leading precision이 DTL_PRECISION_NA 로 오는 경우, */
    /*      * leading max precision 으로 처리한다. */
    /*      *\/ */
    /*     aLeadingPrecision = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION; */
    /* } */
    /* else */
    /* { */
    /*     /\* Do Nothing *\/ */
    /* } */

    DTL_PARAM_VALIDATE( (aLeadingPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aLeadingPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );
    
    if( (sYear < 0) || (sMonth < 0) )
    {
        *sStr = '-';
        sSize = sOneCharSize;
        sYear = (sYear < 0) ? -sYear : sYear;
        sMonth = (sMonth < 0) ? -sMonth : sMonth;
    }
    else
    {
        *sStr = '+';
        sSize = sOneCharSize;
    }

    /*
     * interval year, interval month, interval year to month 에 구분하지 않고,
     * leading precision은 string으로 변환했을때 최상위표현(year)의 자리수로 지정한다.
     * 이는 oracle 과 동일한 표현방식임.
     *
     * 예1) interval year(6)
     *      select interval'9'year(6) from dual; => +000009-00
     * 예2) interval month(6)
     *      select interval'9'month(6) from dual; => +000000-09
     * 예3) interval year(6) to month
     *      select interval'9-9'year(6) to month from dual; => +000009-09
     * 
     */ 
    STL_TRY( dtdIntegerToLeadingZeroFormatUtf32String( sYear,
                                                       aLeadingPrecision,
                                                       & sUintFormatStrSize,
                                                       sStr + (sSize/sOneCharSize),
                                                       aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;
                
    *(sStr + (sSize/sOneCharSize)) = '-';
    sSize = sSize + sOneCharSize;
    
    STL_TRY( dtdIntegerToLeadingZeroFormatUtf32String( sMonth,
                                                       2,
                                                       & sUintFormatStrSize,
                                                       sStr + (sSize/sOneCharSize),
                                                       aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;

    *aLength = sSize;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}


/**
 * @brief dtlExpTime을 leadng/second precision에 자리수에 맞추어 문자열로 변환
 * @param[in]  aIntervalIndicator           interval indicator
 * @param[in]  aDtlExpTime                  dtlExpTime
 * @param[in]  aFractionalSecond            fractional second
 * @param[in]  aLeadingPrecision            leading field precision
 * @param[in]  aFractionalSecondPrecision   fractional second precision
 * @param[out] aStr                         변환된 string을 저장할 공간
 * @param[out] aLength                      변환된 문자열의 길이
 * @param[out] aErrorStack                  에러 스택
 * @remark
 *    <BR> interval day(6) to second(6) 의 data 인 경우,
 *    <BR> precision 자리수에 맞추어 string으로 변환함.
 *    <BR> 예) +000009 23:59:59.900000
 */
stlStatus dtlIntervalDayToSecondToFormatString( dtlIntervalIndicator  aIntervalIndicator,
                                                dtlExpTime          * aDtlExpTime,
                                                dtlFractionalSecond   aFractionalSecond,
                                                stlInt64              aLeadingPrecision,
                                                stlInt64              aFractionalSecondPrecision,
                                                stlChar             * aStr,
                                                stlInt64            * aLength,
                                                stlErrorStack       * aErrorStack )
{
    stlChar             * sStr;
    stlInt32              sDay;
    stlInt32              sHour;
    stlInt32              sMinute;
    stlInt32              sSecond;
    dtlFractionalSecond   sFractionalSecond;
    stlInt32              sSize = 0;
    stlInt32              sUintFormatStrSize = 0;

    sStr    = aStr;
    sDay    = aDtlExpTime->mDay;
    sHour   = aDtlExpTime->mHour;
    sMinute = aDtlExpTime->mMinute;
    sSecond = aDtlExpTime->mSecond;
    sFractionalSecond = aFractionalSecond;

    if( aLeadingPrecision == DTL_PRECISION_NA )
    {
        /*
         * leading precision 이 DTL_PRECISION_NA 로 오는 경우,
         * leading max precision 으로 처리한다.
         */
        aLeadingPrecision = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION;
    }
    else
    {
        /* Do Nothing */
    }

    if( aFractionalSecondPrecision == DTL_SCALE_NA )
    {
        /*
         * fractional second precision 이 DTL_SCALE_NA 로 오는 경우,
         * fractional second max precision 으로 처리한다.
         */
        
        aFractionalSecondPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION;
    }
    else
    {
        /* Do Nothing */
    }

    DTL_PARAM_VALIDATE( (aLeadingPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aLeadingPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );

    DTL_PARAM_VALIDATE( (aFractionalSecondPrecision >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                        (aFractionalSecondPrecision <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION),
                        aErrorStack );

    DTL_PARAM_VALIDATE( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        == STL_TRUE,
                        aErrorStack );

    if( (sDay < 0)  ||
        (sHour < 0) || (sMinute < 0) || (sSecond < 0) || (aFractionalSecond < 0) )
    {
        *(sStr + sSize) = '-';
        sSize++;

        sDay = (sDay < 0) ? -sDay : sDay;
        sHour = (sHour < 0) ? -sHour : sHour;
        sMinute = (sMinute < 0) ? -sMinute : sMinute;
        sSecond = (sSecond < 0) ? -sSecond : sSecond;
        sFractionalSecond =
            (aFractionalSecond < 0) ? -aFractionalSecond : aFractionalSecond;
    }
    else
    {
        *(sStr + sSize) = '+';
        sSize++;
    }

    /*
     * interval day, interval hour, interval minute, interval second,
     * interval day to hour,    interval day to minute, interval day to second,
     * interval hour to minute, interval hour to second,
     * inserval minute to second 에 구분하지 않고,
     * leading precision은 string으로 변환했을때 최상위표현(day)의 자리수로 지정한다.
     * 이는 oracle 과 동일한 표현방식임.
     *
     * 예1) interval day(6)
     *      select interval'9'day(6) from dual; => +000009 00:00:00
     * 예2) interval hour(6)
     *      select interval'9'hour(6) from dual; => +000000 09:00:00
     * 예3) interval minute(6)
     *      select interval'9'minute(6) from dual; => +000000 00:09:00
     * 예4) interval second(6)
     *      select interval'9'second(6) from dual; => +000000 00:00:09.000000
     *
     * ...
     * 
     */ 

    STL_TRY( dtdIntegerToLeadingZeroFormatString( sDay,
                                                  aLeadingPrecision,
                                                  & sUintFormatStrSize,
                                                  sStr + sSize,
                                                  aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;
    
    *(sStr + sSize) = ' ';
    sSize++;

    STL_TRY( dtdIntegerToLeadingZeroFormatString( sHour,
                                                  2,
                                                  & sUintFormatStrSize,
                                                  sStr + sSize,
                                                  aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;
    
    *(sStr + sSize) = ':';
    sSize++;
                
    STL_TRY( dtdIntegerToLeadingZeroFormatString( sMinute,
                                                  2,
                                                  & sUintFormatStrSize,
                                                  sStr + sSize,
                                                  aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;
    
    *(sStr + sSize) = ':';
    sSize++;

    STL_TRY( dtdIntegerToLeadingZeroFormatString( sSecond,
                                                  2,
                                                  & sUintFormatStrSize,
                                                  sStr + sSize,
                                                  aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;

    if( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) )
    {
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }
    
    if( aFractionalSecondPrecision > 0 )
    {
        *(sStr + sSize) = '.';
        sSize++;

        switch( aFractionalSecondPrecision )
        {
            case 1:
                sFractionalSecond = sFractionalSecond / 100000;
                break;
            case 2:
                sFractionalSecond = sFractionalSecond / 10000;
                break;
            case 3:
                sFractionalSecond = sFractionalSecond / 1000;            
                break;
            case 4:
                sFractionalSecond = sFractionalSecond / 100;            
                break;
            case 5:
                sFractionalSecond = sFractionalSecond / 10;            
                break;
            case 6:
                /* Do Nothing */
                break;
            default:
                STL_DASSERT( STL_FALSE );
                break;
        }
    
        STL_TRY( dtdIntegerToLeadingZeroFormatString( sFractionalSecond,
                                                      aFractionalSecondPrecision,
                                                      & sUintFormatStrSize,
                                                      sStr + sSize,
                                                      aErrorStack )
                 == STL_SUCCESS );
        sSize += sUintFormatStrSize;
    }
    else
    {
        /* Do Nothing */
    }

    STL_RAMP( RAMP_FINISH );
    
    *aLength = sSize;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/**
 * @brief dtlExpTime을 leadng/second precision에 자리수에 맞추어 utf16 문자열로 변환
 * @param[in]  aIntervalIndicator           interval indicator
 * @param[in]  aDtlExpTime                  dtlExpTime
 * @param[in]  aFractionalSecond            fractional second
 * @param[in]  aLeadingPrecision            leading field precision
 * @param[in]  aFractionalSecondPrecision   fractional second precision
 * @param[out] aStr                         변환된 string을 저장할 공간
 * @param[out] aLength                      변환된 문자열의 길이
 * @param[out] aErrorStack                  에러 스택
 * @remark
 *    <BR> interval day(6) to second(6) 의 data 인 경우,
 *    <BR> precision 자리수에 맞추어 string으로 변환함.
 *    <BR> 예) +000009 23:59:59.900000
 */
stlStatus dtlIntervalDayToSecondToFormatUtf16String( dtlIntervalIndicator  aIntervalIndicator,
                                                     dtlExpTime          * aDtlExpTime,
                                                     dtlFractionalSecond   aFractionalSecond,
                                                     stlInt64              aLeadingPrecision,
                                                     stlInt64              aFractionalSecondPrecision,
                                                     stlUInt16           * aStr,
                                                     stlInt64            * aLength,
                                                     stlErrorStack       * aErrorStack )
{
    stlUInt16           * sStr;
    stlInt32              sDay;
    stlInt32              sHour;
    stlInt32              sMinute;
    stlInt32              sSecond;
    dtlFractionalSecond   sFractionalSecond;
    stlInt32              sSize = 0;
    stlInt32              sUintFormatStrSize = 0;
    stlInt32              sOneCharSize = STL_UINT16_SIZE;

    sStr    = aStr;
    sDay    = aDtlExpTime->mDay;
    sHour   = aDtlExpTime->mHour;
    sMinute = aDtlExpTime->mMinute;
    sSecond = aDtlExpTime->mSecond;
    sFractionalSecond = aFractionalSecond;

    /* if( aLeadingPrecision == DTL_PRECISION_NA ) */
    /* { */
    /*     /\* */
    /*      * leading precision 이 DTL_PRECISION_NA 로 오는 경우, */
    /*      * leading max precision 으로 처리한다. */
    /*      *\/ */
    /*     aLeadingPrecision = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION; */
    /* } */
    /* else */
    /* { */
    /*     /\* Do Nothing *\/ */
    /* } */

    /* if( aFractionalSecondPrecision == DTL_SCALE_NA ) */
    /* { */
    /*     /\* */
    /*      * fractional second precision 이 DTL_SCALE_NA 로 오는 경우, */
    /*      * fractional second max precision 으로 처리한다. */
    /*      *\/ */
        
    /*     aFractionalSecondPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION; */
    /* } */
    /* else */
    /* { */
    /*     /\* Do Nothing *\/ */
    /* } */

    DTL_PARAM_VALIDATE( (aLeadingPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aLeadingPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );

    DTL_PARAM_VALIDATE( (aFractionalSecondPrecision >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                        (aFractionalSecondPrecision <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION),
                        aErrorStack );

    DTL_PARAM_VALIDATE( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        == STL_TRUE,
                        aErrorStack );

    if( (sDay < 0)  ||
        (sHour < 0) || (sMinute < 0) || (sSecond < 0) || (aFractionalSecond < 0) )
    {
        *sStr = '-';
        sSize = sOneCharSize;

        sDay = (sDay < 0) ? -sDay : sDay;
        sHour = (sHour < 0) ? -sHour : sHour;
        sMinute = (sMinute < 0) ? -sMinute : sMinute;
        sSecond = (sSecond < 0) ? -sSecond : sSecond;
        sFractionalSecond =
            (aFractionalSecond < 0) ? -aFractionalSecond : aFractionalSecond;
    }
    else
    {
        *sStr = '+';
        sSize = sOneCharSize;
    }

    /*
     * interval day, interval hour, interval minute, interval second,
     * interval day to hour,    interval day to minute, interval day to second,
     * interval hour to minute, interval hour to second,
     * inserval minute to second 에 구분하지 않고,
     * leading precision은 string으로 변환했을때 최상위표현(day)의 자리수로 지정한다.
     * 이는 oracle 과 동일한 표현방식임.
     *
     * 예1) interval day(6)
     *      select interval'9'day(6) from dual; => +000009 00:00:00
     * 예2) interval hour(6)
     *      select interval'9'hour(6) from dual; => +000000 09:00:00
     * 예3) interval minute(6)
     *      select interval'9'minute(6) from dual; => +000000 00:09:00
     * 예4) interval second(6)
     *      select interval'9'second(6) from dual; => +000000 00:00:09.000000
     *
     * ...
     * 
     */ 

    STL_TRY( dtdIntegerToLeadingZeroFormatUtf16String( sDay,
                                                       aLeadingPrecision,
                                                       & sUintFormatStrSize,
                                                       sStr + (sSize/sOneCharSize),
                                                       aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;
    
    *(sStr + (sSize/sOneCharSize)) = ' ';
    sSize = sSize + sOneCharSize;

    STL_TRY( dtdIntegerToLeadingZeroFormatUtf16String( sHour,
                                                       2,
                                                       & sUintFormatStrSize,
                                                       sStr + (sSize/sOneCharSize),
                                                       aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;
    
    *(sStr + (sSize/sOneCharSize)) = ':';
    sSize = sSize + sOneCharSize;
                
    STL_TRY( dtdIntegerToLeadingZeroFormatUtf16String( sMinute,
                                                       2,
                                                       & sUintFormatStrSize,
                                                       sStr + (sSize/sOneCharSize),
                                                       aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;
    
    *(sStr + (sSize/sOneCharSize)) = ':';
    sSize = sSize + sOneCharSize;

    STL_TRY( dtdIntegerToLeadingZeroFormatUtf16String( sSecond,
                                                       2,
                                                       & sUintFormatStrSize,
                                                       sStr + (sSize/sOneCharSize),
                                                       aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;

    if( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) )
    {
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }
    
    if( aFractionalSecondPrecision > 0 )
    {
        *(sStr + (sSize/sOneCharSize)) = '.';
        sSize = sSize + sOneCharSize;

        switch( aFractionalSecondPrecision )
        {
            case 1:
                sFractionalSecond = sFractionalSecond / 100000;
                break;
            case 2:
                sFractionalSecond = sFractionalSecond / 10000;
                break;
            case 3:
                sFractionalSecond = sFractionalSecond / 1000;            
                break;
            case 4:
                sFractionalSecond = sFractionalSecond / 100;            
                break;
            case 5:
                sFractionalSecond = sFractionalSecond / 10;            
                break;
            case 6:
                /* Do Nothing */
                break;
            default:
                STL_DASSERT( STL_FALSE );
                break;
        }
    
        STL_TRY( dtdIntegerToLeadingZeroFormatUtf16String( sFractionalSecond,
                                                           aFractionalSecondPrecision,
                                                           & sUintFormatStrSize,
                                                           sStr + (sSize/sOneCharSize),
                                                           aErrorStack )
                 == STL_SUCCESS );
        sSize += sUintFormatStrSize;
    }
    else
    {
        /* Do Nothing */
    }

    STL_RAMP( RAMP_FINISH );
    
    *aLength = sSize;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/**
 * @brief dtlExpTime을 leadng/second precision에 자리수에 맞추어 utf32 문자열로 변환
 * @param[in]  aIntervalIndicator           interval indicator
 * @param[in]  aDtlExpTime                  dtlExpTime
 * @param[in]  aFractionalSecond            fractional second
 * @param[in]  aLeadingPrecision            leading field precision
 * @param[in]  aFractionalSecondPrecision   fractional second precision
 * @param[out] aStr                         변환된 string을 저장할 공간
 * @param[out] aLength                      변환된 문자열의 길이
 * @param[out] aErrorStack                  에러 스택
 * @remark
 *    <BR> interval day(6) to second(6) 의 data 인 경우,
 *    <BR> precision 자리수에 맞추어 string으로 변환함.
 *    <BR> 예) +000009 23:59:59.900000
 */
stlStatus dtlIntervalDayToSecondToFormatUtf32String( dtlIntervalIndicator  aIntervalIndicator,
                                                     dtlExpTime          * aDtlExpTime,
                                                     dtlFractionalSecond   aFractionalSecond,
                                                     stlInt64              aLeadingPrecision,
                                                     stlInt64              aFractionalSecondPrecision,
                                                     stlUInt32           * aStr,
                                                     stlInt64            * aLength,
                                                     stlErrorStack       * aErrorStack )
{
    stlUInt32           * sStr;
    stlInt32              sDay;
    stlInt32              sHour;
    stlInt32              sMinute;
    stlInt32              sSecond;
    dtlFractionalSecond   sFractionalSecond;
    stlInt32              sSize = 0;
    stlInt32              sUintFormatStrSize = 0;
    stlInt32              sOneCharSize = STL_UINT32_SIZE;

    sStr    = aStr;
    sDay    = aDtlExpTime->mDay;
    sHour   = aDtlExpTime->mHour;
    sMinute = aDtlExpTime->mMinute;
    sSecond = aDtlExpTime->mSecond;
    sFractionalSecond = aFractionalSecond;

    /* if( aLeadingPrecision == DTL_PRECISION_NA ) */
    /* { */
    /*     /\* */
    /*      * leading precision 이 DTL_PRECISION_NA 로 오는 경우, */
    /*      * leading max precision 으로 처리한다. */
    /*      *\/ */
    /*     aLeadingPrecision = DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION; */
    /* } */
    /* else */
    /* { */
    /*     /\* Do Nothing *\/ */
    /* } */

    /* if( aFractionalSecondPrecision == DTL_SCALE_NA ) */
    /* { */
    /*     /\* */
    /*      * fractional second precision 이 DTL_SCALE_NA 로 오는 경우, */
    /*      * fractional second max precision 으로 처리한다. */
    /*      *\/ */
        
    /*     aFractionalSecondPrecision = DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION; */
    /* } */
    /* else */
    /* { */
    /*     /\* Do Nothing *\/ */
    /* } */

    DTL_PARAM_VALIDATE( (aLeadingPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
                        (aLeadingPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION),
                        aErrorStack );

    DTL_PARAM_VALIDATE( (aFractionalSecondPrecision >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                        (aFractionalSecondPrecision <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION),
                        aErrorStack );

    DTL_PARAM_VALIDATE( ( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND) ||
                          (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND) )
                        == STL_TRUE,
                        aErrorStack );

    if( (sDay < 0)  ||
        (sHour < 0) || (sMinute < 0) || (sSecond < 0) || (aFractionalSecond < 0) )
    {
        *sStr = '-';
        sSize = sOneCharSize;

        sDay = (sDay < 0) ? -sDay : sDay;
        sHour = (sHour < 0) ? -sHour : sHour;
        sMinute = (sMinute < 0) ? -sMinute : sMinute;
        sSecond = (sSecond < 0) ? -sSecond : sSecond;
        sFractionalSecond =
            (aFractionalSecond < 0) ? -aFractionalSecond : aFractionalSecond;
    }
    else
    {
        *sStr = '+';
        sSize = sOneCharSize;
    }

    /*
     * interval day, interval hour, interval minute, interval second,
     * interval day to hour,    interval day to minute, interval day to second,
     * interval hour to minute, interval hour to second,
     * inserval minute to second 에 구분하지 않고,
     * leading precision은 string으로 변환했을때 최상위표현(day)의 자리수로 지정한다.
     * 이는 oracle 과 동일한 표현방식임.
     *
     * 예1) interval day(6)
     *      select interval'9'day(6) from dual; => +000009 00:00:00
     * 예2) interval hour(6)
     *      select interval'9'hour(6) from dual; => +000000 09:00:00
     * 예3) interval minute(6)
     *      select interval'9'minute(6) from dual; => +000000 00:09:00
     * 예4) interval second(6)
     *      select interval'9'second(6) from dual; => +000000 00:00:09.000000
     *
     * ...
     * 
     */ 

    STL_TRY( dtdIntegerToLeadingZeroFormatUtf32String( sDay,
                                                       aLeadingPrecision,
                                                       & sUintFormatStrSize,
                                                       sStr + (sSize/sOneCharSize),
                                                       aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;
    
    *(sStr + (sSize/sOneCharSize)) = ' ';
    sSize = sSize + sOneCharSize;

    STL_TRY( dtdIntegerToLeadingZeroFormatUtf32String( sHour,
                                                       2,
                                                       & sUintFormatStrSize,
                                                       sStr + (sSize/sOneCharSize),
                                                       aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;
    
    *(sStr + (sSize/sOneCharSize)) = ':';
    sSize = sSize + sOneCharSize;
                
    STL_TRY( dtdIntegerToLeadingZeroFormatUtf32String( sMinute,
                                                       2,
                                                       & sUintFormatStrSize,
                                                       sStr + (sSize/sOneCharSize),
                                                       aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;
    
    *(sStr + (sSize/sOneCharSize)) = ':';
    sSize = sSize + sOneCharSize;

    STL_TRY( dtdIntegerToLeadingZeroFormatUtf32String( sSecond,
                                                       2,
                                                       & sUintFormatStrSize,
                                                       sStr + (sSize/sOneCharSize),
                                                       aErrorStack )
             == STL_SUCCESS );
    sSize += sUintFormatStrSize;

    if( (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_HOUR) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE) ||
        (aIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE) )
    {
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        /* Do Nothing */
    }
    
    if( aFractionalSecondPrecision > 0 )
    {
        *(sStr + (sSize/sOneCharSize)) = '.';
        sSize = sSize + sOneCharSize;

        switch( aFractionalSecondPrecision )
        {
            case 1:
                sFractionalSecond = sFractionalSecond / 100000;
                break;
            case 2:
                sFractionalSecond = sFractionalSecond / 10000;
                break;
            case 3:
                sFractionalSecond = sFractionalSecond / 1000;            
                break;
            case 4:
                sFractionalSecond = sFractionalSecond / 100;            
                break;
            case 5:
                sFractionalSecond = sFractionalSecond / 10;            
                break;
            case 6:
                /* Do Nothing */
                break;
            default:
                STL_DASSERT( STL_FALSE );
                break;
        }
    
        STL_TRY( dtdIntegerToLeadingZeroFormatUtf32String( sFractionalSecond,
                                                           aFractionalSecondPrecision,
                                                           & sUintFormatStrSize,
                                                           sStr + (sSize/sOneCharSize),
                                                           aErrorStack )
                 == STL_SUCCESS );
        sSize += sUintFormatStrSize;
    }
    else
    {
        /* Do Nothing */
    }

    STL_RAMP( RAMP_FINISH );
    
    *aLength = sSize;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/**
 * @brief 년월일시분초를 Date type value로 만든다.
 * @param[in]  aYear            year
 * @param[in]  aMonth           month
 * @param[in]  aDay             day
 * @param[in]  aHour            hour
 * @param[in]  aMinute          minute
 * @param[in]  aSecond          second
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue           dtlDataValue의 주소  
 * @param[out] aVectorFunc      Function Vector
 * @param[out] aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtlDateSetValueFromTimeInfo( stlInt32           aYear,
                                       stlInt32           aMonth,
                                       stlInt32           aDay,
                                       stlInt32           aHour,
                                       stlInt32           aMinute,
                                       stlInt32           aSecond,
                                       stlInt64           aAvailableSize,
                                       dtlDataValue     * aValue,
                                       dtlFuncVector    * aVectorFunc,
                                       void             * aVectorArg,
                                       stlErrorStack    * aErrorStack )
{
    dtlDateType         * sDateType    = NULL;
    dtlExpTime            sDtlExpTime;
    
    STL_DASSERT( aAvailableSize >= DTL_DATE_SIZE );
    
    STL_DASSERT( dtlIsValidateDate( aYear,
                                    aMonth,
                                    aDay ) == STL_TRUE );
    
    STL_DASSERT( dtlIsValidateTime( aHour,
                                    aMinute,
                                    aSecond ) == STL_TRUE );
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    sDtlExpTime.mYear   = DTL_GET_DTLEXPTIME_YEAR_FROM_YEAR( aYear );
    sDtlExpTime.mMonth  = aMonth;
    sDtlExpTime.mDay    = aDay;
    sDtlExpTime.mHour   = aHour;
    sDtlExpTime.mMinute = aMinute;
    sDtlExpTime.mSecond = aSecond;
    
    sDateType = (dtlDateType *)(aValue->mValue);
    
    STL_TRY( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                           0, /* fractional second */
                                           sDateType,
                                           aErrorStack )
             == STL_SUCCESS );
    
    aValue->mLength = DTL_DATE_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief 시분초를 Time type value로 만든다.
 * @param[in]  aHour            hour
 * @param[in]  aMinute          minute
 * @param[in]  aSecond          second
 * @param[in]  aFraction        fractional second
 * @param[in]  aPrecision       precision
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue           dtlDataValue의 주소  
 * @param[out] aVectorFunc      Function Vector
 * @param[out] aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtlTimeSetValueFromTimeInfo( stlInt32           aHour,
                                       stlInt32           aMinute,
                                       stlInt32           aSecond,
                                       stlInt32           aFraction,
                                       stlInt64           aPrecision,
                                       stlInt64           aAvailableSize,
                                       dtlDataValue     * aValue,
                                       dtlFuncVector    * aVectorFunc,
                                       void             * aVectorArg,
                                       stlErrorStack    * aErrorStack )
{
    dtlTimeType         * sTimeType    = NULL;
    dtlExpTime            sDtlExpTime;
    dtlFractionalSecond   sFractionalSecond = 0;
    
    STL_DASSERT( aAvailableSize >= DTL_TIME_SIZE );
    
    STL_DASSERT( dtlIsValidateTime( aHour,
                                    aMinute,
                                    aSecond ) == STL_TRUE );
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    sDtlExpTime.mHour   = aHour;
    sDtlExpTime.mMinute = aMinute;
    sDtlExpTime.mSecond = aSecond;
    
    sFractionalSecond = aFraction;
    
    sTimeType = (dtlTimeType *)(aValue->mValue);
    
    dtdDtlExpTime2Time( & sDtlExpTime,
                        sFractionalSecond,
                        sTimeType );
    
    STL_TRY( dtdAdjustTime( sTimeType,
                            aPrecision,
                            aErrorStack )
             == STL_SUCCESS );
    
    aValue->mLength = DTL_TIME_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief 시분초 timezone hour/minute를 Time with time zone type value로 만든다.
 * @param[in]  aHour            hour
 * @param[in]  aMinute          minute
 * @param[in]  aSecond          second
 * @param[in]  aFraction        fractional second
 * @param[in]  aTimeZoneHour    timezone hour
 * @param[in]  aTimeZoneMinute  timezone minute
 * @param[in]  aPrecision       precision
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue           dtlDataValue의 주소  
 * @param[out] aVectorFunc      Function Vector
 * @param[out] aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtlTimeTzSetValueFromTimeInfo( stlInt32           aHour,
                                         stlInt32           aMinute,
                                         stlInt32           aSecond,
                                         stlInt32           aFraction,
                                         stlInt32           aTimeZoneHour,
                                         stlInt32           aTimeZoneMinute,
                                         stlInt64           aPrecision,
                                         stlInt64           aAvailableSize,
                                         dtlDataValue     * aValue,
                                         dtlFuncVector    * aVectorFunc,
                                         void             * aVectorArg,
                                         stlErrorStack    * aErrorStack )
{
    dtlTimeTzType       * sTimeTzType    = NULL;
    dtlExpTime            sDtlExpTime;
    dtlFractionalSecond   sFractionalSecond = 0;
    stlInt32              sTimeZone = 0;
    
    STL_DASSERT( aAvailableSize >= DTL_TIMETZ_SIZE );
    
    STL_DASSERT( dtlIsValidateTime( aHour,
                                    aMinute,
                                    aSecond ) == STL_TRUE );

    STL_DASSERT( dtlIsValidateTimeZone( aTimeZoneHour,
                                        aTimeZoneMinute ) == STL_TRUE );
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    sDtlExpTime.mHour   = aHour;
    sDtlExpTime.mMinute = aMinute;
    sDtlExpTime.mSecond = aSecond;
    
    sFractionalSecond = aFraction;
    
    sTimeTzType = (dtlTimeTzType *)(aValue->mValue);
    
    sTimeZone = -((aTimeZoneHour * DTL_SECS_PER_HOUR) + (aTimeZoneMinute * DTL_SECS_PER_MINUTE));
    
    dtdDtlExpTime2TimeTz( & sDtlExpTime,
                          sFractionalSecond,
                          sTimeZone,
                          sTimeTzType );
    
    STL_TRY( dtdAdjustTime( & (sTimeTzType->mTime),
                            aPrecision,
                            aErrorStack )
             == STL_SUCCESS );
    
    aValue->mLength = DTL_TIMETZ_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief 년월일시분초를 Timestamp type value로 만든다.
 * @param[in]  aYear            year
 * @param[in]  aMonth           month
 * @param[in]  aDay             day
 * @param[in]  aHour            hour
 * @param[in]  aMinute          minute
 * @param[in]  aSecond          second
 * @param[in]  aFraction        fractional second
 * @param[in]  aPrecision       precision
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue           dtlDataValue의 주소  
 * @param[out] aVectorFunc      Function Vector
 * @param[out] aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtlTimestampSetValueFromTimeInfo( stlInt32           aYear,
                                            stlInt32           aMonth,
                                            stlInt32           aDay,
                                            stlInt32           aHour,
                                            stlInt32           aMinute,
                                            stlInt32           aSecond,
                                            stlInt32           aFraction,
                                            stlInt64           aPrecision,
                                            stlInt64           aAvailableSize,
                                            dtlDataValue     * aValue,
                                            dtlFuncVector    * aVectorFunc,
                                            void             * aVectorArg,
                                            stlErrorStack    * aErrorStack )
{
    dtlTimestampType    * sTimestampType    = NULL;
    dtlExpTime            sDtlExpTime;
    dtlFractionalSecond   sFractionalSecond = 0;

    DTL_PARAM_VALIDATE( (aPrecision >= DTL_TIMESTAMP_MIN_PRECISION) &&
                        (aPrecision <= DTL_TIMESTAMP_MAX_PRECISION),
                        aErrorStack );    
    STL_DASSERT( aAvailableSize >= DTL_TIMESTAMP_SIZE );
    
    STL_DASSERT( dtlIsValidateTimestamp( aYear,
                                         aMonth,
                                         aDay,
                                         aHour,
                                         aMinute,
                                         aSecond,
                                         aFraction ) == STL_TRUE );

    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    sDtlExpTime.mYear = DTL_GET_DTLEXPTIME_YEAR_FROM_YEAR( aYear );
    sDtlExpTime.mMonth = aMonth;
    sDtlExpTime.mDay = aDay;
    sDtlExpTime.mHour = aHour;
    sDtlExpTime.mMinute = aMinute;
    sDtlExpTime.mSecond = aSecond;
    
    sFractionalSecond = aFraction;
    
    sTimestampType = (dtlTimestampType *)(aValue->mValue);
    
    STL_TRY( dtdDtlExpTime2DateTimeOffset( & sDtlExpTime,
                                           sFractionalSecond,
                                           sTimestampType,
                                           aErrorStack )
             == STL_SUCCESS );
    
    STL_TRY( dtdAdjustTimestamp( sTimestampType,
                                 aPrecision,
                                 aErrorStack )
             == STL_SUCCESS );
    
    aValue->mLength = DTL_TIMESTAMP_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 년월일시분초 timezone hour/minute를 Timestamp with time zone type value로 만든다.
 * @param[in]  aYear            year
 * @param[in]  aMonth           month
 * @param[in]  aDay             day
 * @param[in]  aHour            hour
 * @param[in]  aMinute          minute
 * @param[in]  aSecond          second
 * @param[in]  aFraction        fractional second
 * @param[in]  aTimeZoneHour    timezone hour
 * @param[in]  aTimeZoneMinute  timezone minute
 * @param[in]  aPrecision       precision
 * @param[in]  aAvailableSize   mValue에 사용할 수 있는 메모리 공간의 크기  
 * @param[out] aValue           dtlDataValue의 주소  
 * @param[out] aVectorFunc      Function Vector
 * @param[out] aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtlTimestampTzSetValueFromTimeInfo( stlInt32           aYear,
                                              stlInt32           aMonth,
                                              stlInt32           aDay,
                                              stlInt32           aHour,
                                              stlInt32           aMinute,
                                              stlInt32           aSecond,
                                              stlInt32           aFraction,
                                              stlInt32           aTimeZoneHour,
                                              stlInt32           aTimeZoneMinute,
                                              stlInt64           aPrecision,
                                              stlInt64           aAvailableSize,
                                              dtlDataValue     * aValue,
                                              dtlFuncVector    * aVectorFunc,
                                              void             * aVectorArg,
                                              stlErrorStack    * aErrorStack )
{
    dtlTimestampTzType  * sTimestampTzType    = NULL;
    dtlExpTime            sDtlExpTime;
    dtlFractionalSecond   sFractionalSecond = 0;
    stlInt32              sTimeZone         = 0;   
    
    DTL_PARAM_VALIDATE( (aPrecision >= DTL_TIMESTAMPTZ_MIN_PRECISION) &&
                        (aPrecision <= DTL_TIMESTAMPTZ_MAX_PRECISION),
                        aErrorStack );
    STL_DASSERT( aAvailableSize >= DTL_TIMESTAMPTZ_SIZE );
    
    STL_DASSERT( dtlIsValidateTimestamp( aYear,
                                         aMonth,
                                         aDay,
                                         aHour,
                                         aMinute,
                                         aSecond,
                                         aFraction ) == STL_TRUE );

    STL_DASSERT( dtlIsValidateTimeZone( aTimeZoneHour,
                                        aTimeZoneMinute ) == STL_TRUE );
    
    DTL_INIT_DTLEXPTIME( sDtlExpTime );
    
    sDtlExpTime.mYear = DTL_GET_DTLEXPTIME_YEAR_FROM_YEAR( aYear );
    sDtlExpTime.mMonth = aMonth;
    sDtlExpTime.mDay    = aDay;
    sDtlExpTime.mHour  = aHour;
    sDtlExpTime.mMinute = aMinute;
    sDtlExpTime.mSecond = aSecond;
    
    sFractionalSecond = aFraction;
    
    sTimestampTzType = (dtlTimestampTzType *)(aValue->mValue);
    
    sTimeZone = -((aTimeZoneHour * DTL_SECS_PER_HOUR) + (aTimeZoneMinute * DTL_SECS_PER_MINUTE));
    
    STL_TRY( dtdDtlExpTime2TimestampTz( NULL,
                                        & sDtlExpTime,
                                        sFractionalSecond,
                                        & sTimeZone,
                                        sTimestampTzType,
                                        aErrorStack )
             == STL_SUCCESS );

    STL_TRY( dtdAdjustTimestamp( & (sTimestampTzType->mTimestamp),
                                 aPrecision,
                                 aErrorStack )
             == STL_SUCCESS );
    
    aValue->mLength = DTL_TIMESTAMPTZ_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief timestamp with time zone type 값을 date type의 값으로 반환한다.
 * @param[in]  aTimestampTz   timestamp with time zone type value
 * @param[out] aDate          date type value
 * @param[out] aErrorStack    에러스택
 */
stlStatus dtlTimestampTzToDate( dtlTimestampTzType  * aTimestampTz,
                                dtlDateType         * aDate,
                                stlErrorStack       * aErrorStack )
{
    return dtdTimestampTzToDate( aTimestampTz,
                                 aDate,
                                 aErrorStack );
}

/**
 * @brief timestamp with time zone type 값을 time with time zone type의 값으로 반환한다.
 * @param[in]  aTimestampTz   timestamp with time zone type value
 * @param[in]  aPrecision     precision( time with time zone type )
 * @param[out] aTimeTz        time with time zone type value
 * @param[out] aErrorStack    에러스택
 */
stlStatus dtlTimestampTzToTimeTz( dtlTimestampTzType  * aTimestampTz,
                                  stlInt64              aPrecision,
                                  dtlTimeTzType       * aTimeTz,
                                  stlErrorStack       * aErrorStack )
{
    return dtdTimestampTzToTimeTz( aTimestampTz,
                                   aPrecision,
                                   aTimeTz,
                                   aErrorStack );
}

/**
 * @brief timestamp with time zone type 값을 time without time zone type의 값으로 반환한다.
 * @param[in]  aTimestampTz   timestamp with time zone type value
 * @param[in]  aPrecision     precision( time without time zone type )
 * @param[out] aTime          time without time zone type value
 * @param[out] aErrorStack    에러스택
 */
stlStatus dtlTimestampTzToTime( dtlTimestampTzType  * aTimestampTz,
                                stlInt64              aPrecision,
                                dtlTimeType         * aTime,
                                stlErrorStack       * aErrorStack )
{
    return dtdTimestampTzToTime( aTimestampTz,
                                 aPrecision,
                                 aTime,
                                 aErrorStack );
}

/**
 * @brief timestamp with time zone type 값을 timestamp without time zone type의 값으로 반환한다.
 * @param[in]  aTimestampTz   timestamp with time zone type value
 * @param[in]  aPrecision     precision( timestamp without time zone type )
 * @param[out] aTimestamp     timestamp without time zone type value
 * @param[out] aErrorStack    에러스택
 */
stlStatus dtlTimestampTzToTimestamp( dtlTimestampTzType  * aTimestampTz,
                                     stlInt64              aPrecision,
                                     dtlTimestampType    * aTimestamp,
                                     stlErrorStack       * aErrorStack )
{
    return dtdTimestampTzToTimestamp( aTimestampTz,
                                      aPrecision,
                                      aTimestamp,
                                      aErrorStack );
}


/**
 * @brief Timestamp Type Value 로부터 stlTime value 구성
 *   <BR> 예) timestamp -> stlTime 값으로 변환.
 * @param[in]  aTimestamp       Timestamp Type Value
 * @param[out] aStlTime         변환된 stlTime Value
 * @param[in]  aVectorFunc      Function Vector
 * @param[in]  aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtlFromTimestampToStlTime( dtlTimestampType * aTimestamp,
                                     stlTime          * aStlTime,
                                     dtlFuncVector    * aVectorFunc,
                                     void             * aVectorArg,
                                     stlErrorStack    * aErrorStack )
{
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( aStlTime != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aVectorFunc != NULL, aErrorStack );
    
    STL_TRY( dtfFromTimestampToStlTime( aTimestamp,
                                        aStlTime,
                                        aVectorFunc,
                                        aVectorArg,
                                        aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief stlTime 에 number 를 더한 stlTime 값을 반환한다.
 * @param[in]  aStlTime         stlTime
 * @param[in]  aNumber          aNumber dtlDataValue
 * @param[out] aResultStlTime   결과 stlTime
 * @param[in]  aVectorFunc      Function Vector
 * @param[in]  aVectorArg       Vector Argument
 * @param[out] aErrorStack      에러 스택
 */
stlStatus dtlFuncStlTimeAddNumber( stlTime          aStlTime,
                                   dtlDataValue   * aNumber,
                                   stlTime        * aResultStlTime,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,
                                   stlErrorStack  * aErrorStack )
{
    stlTime        sNumberStlTime = 0;
    stlBool        sIsTruncated = STL_FALSE;    
    
    stlInt32       sNumericAlignMaxSize = STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE);
    stlUInt8       sNumeric[STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE)];
    stlUInt8       sTempNumeric[STL_ALIGN_DEFAULT(DTL_NUMERIC_MAX_SIZE)];    
    dtlDataValue   sNumericValue;
    dtlDataValue   sTempNumericValue;
    
    DTF_CHECK_DTL_VALUE( aNumber, DTL_TYPE_NUMBER, aErrorStack );
    
    sNumericValue.mType = DTL_TYPE_NUMBER;
    sNumericValue.mBufferSize = sNumericAlignMaxSize;
    sNumericValue.mValue = (dtlNumericType *)(&sNumeric);
    
    sTempNumericValue.mType = DTL_TYPE_NUMBER;
    sTempNumericValue.mBufferSize = sNumericAlignMaxSize;
    sTempNumericValue.mValue = (dtlNumericType *)(&sTempNumeric);
    
    /**
     * number value 를 stlTime value 로 변경
     */
    
    if( DTL_NUMERIC_IS_ZERO( (dtlNumericType *)(aNumber->mValue), aNumber->mLength ) )
    {
        sNumberStlTime = 0;
    }
    else
    {
        /**
         * number 의 단위가 day 이므로, 이를 usecs 로 변경한다.
         * number * DTL_USECS_PER_DAY
         */
        
        STL_TRY( dtdToNumericFromInt64( (stlInt64)DTL_USECS_PER_DAY,
                                         DTL_NUMERIC_MAX_PRECISION,
                                         0,
                                         & sTempNumericValue,
                                         aErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY( dtfNumericMultiplication( aNumber,
                                           & sTempNumericValue,
                                           & sNumericValue,
                                           aErrorStack )
                 == STL_SUCCESS );
        
        /**
         * number * DTL_USECS_PER_DAY 를 integer 로 변경
         */
        
        STL_TRY( dtlNumericToUInt64( & sNumericValue,
                                     & sNumberStlTime,
                                     & sIsTruncated,
                                     aErrorStack )
                 == STL_SUCCESS );
    }
    
    /**
     * stlTime + number
     */
    
    *aResultStlTime = aStlTime + sNumberStlTime;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/**
 * @brief Data Value 가 유효한 값인지 검증
 * @param[in] aDataValue Data Value
 * @result stlBool
 * @remarks
 * DEBUG 용도임
 */
stlBool dtlIsValidDataValue( dtlDataValue * aDataValue )
{
    stlBool sResult = STL_TRUE;

    if ( aDataValue == NULL )
    {
        sResult = STL_FALSE;
        STL_THROW( RAMP_FINISH );
    }

    if ( (aDataValue->mType >= DTL_TYPE_BOOLEAN) && (aDataValue->mType < DTL_TYPE_MAX) )
    {
    }
    else
    {
        sResult = STL_FALSE;
        STL_THROW( RAMP_FINISH );
    }

    if ( DTL_IS_NULL( aDataValue ) == STL_TRUE )
    {
        sResult = STL_TRUE;
        STL_THROW( RAMP_FINISH );
    }
    else
    {
        switch ( aDataValue->mType )
        {
            case DTL_TYPE_BOOLEAN:
                {
                    break;
                }
            case DTL_TYPE_NATIVE_SMALLINT:
                {
                    break;
                }
            case DTL_TYPE_NATIVE_INTEGER:
                {
                    break;
                }
            case DTL_TYPE_NATIVE_BIGINT:
                {
                    break;
                }
            case DTL_TYPE_NATIVE_REAL:
                {
                    break;
                }
            case DTL_TYPE_NATIVE_DOUBLE:
                {
                    break;
                }
            case DTL_TYPE_FLOAT:
                {
                    break;
                }
            case DTL_TYPE_NUMBER:
                {
                    break;
                }
            case DTL_TYPE_DECIMAL:
                {
                    sResult = STL_FALSE;
                    STL_THROW( RAMP_FINISH );
                    break;
                }
            case DTL_TYPE_CHAR:
                {
                    break;
                }
            case DTL_TYPE_VARCHAR:
                {
                    break;
                }
            case DTL_TYPE_LONGVARCHAR:
                {
                    break;
                }
            case DTL_TYPE_CLOB:
                {
                    sResult = STL_FALSE;
                    STL_THROW( RAMP_FINISH );
                    break;
                }
            case DTL_TYPE_BINARY:
                {
                    break;
                }
            case DTL_TYPE_VARBINARY:
                {
                    break;
                }
            case DTL_TYPE_LONGVARBINARY:
                {
                    break;
                }
            case DTL_TYPE_BLOB:
                {
                    sResult = STL_FALSE;
                    STL_THROW( RAMP_FINISH );
                    break;
                }
            case DTL_TYPE_DATE:
                {
                    break;
                }
            case DTL_TYPE_TIME:
                {
                    dtlTimeType sValue = *(dtlTimeType*)aDataValue->mValue;
                    if( sValue < DTL_USECS_PER_DAY )
                    {
                        /* ok */
                    }
                    else
                    {
                        sResult = STL_FALSE;
                        STL_THROW( RAMP_FINISH );
                    }
                    break;
                }
            case DTL_TYPE_TIMESTAMP:
                {
                    break;
                }
            case DTL_TYPE_TIME_WITH_TIME_ZONE:
                {
                    break;
                }
            case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE:
                {
                    break;
                }
            case DTL_TYPE_INTERVAL_YEAR_TO_MONTH:
                {
                    break;
                }
            case DTL_TYPE_INTERVAL_DAY_TO_SECOND:
                {
                    break;
                }
            case DTL_TYPE_ROWID:
                {
                    break;
                }
            default:
                {
                    sResult = STL_FALSE;
                    STL_THROW( RAMP_FINISH );
                    break;
                }
        }
    }
    
    STL_RAMP( RAMP_FINISH );
    
    return sResult;
}

/** @} dtlDataValueControl */
       
