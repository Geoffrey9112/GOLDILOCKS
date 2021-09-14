#!/bin/bash

os_type=`uname`

if [ $# -ne 2 ]
then
echo "Usage: $0 <map file> <log file>"
exit;
fi

AWK=awk
GREP=grep
SED=sed

###############################################################
# map file로부터 stack trace에 사용할 symbol table 생성
###############################################################
if [ $os_type == "Linux" ]
then
    # Linux
    cat $1 | $GREP ' T ' | $AWK '{printf("0x%s %s\n", $1, $3); }' | sort > stacktrace_temp.map
else
    if [ $os_type == "AIX" ]
    then
        # AIX 64bit gcc
        cat $1 | $GREP ' T ' | $AWK '{printf("0x000000010%s %s\n", substr($1,4), $3); }' | sort > stacktrace_temp.map
    fi # AIX
fi # Linux

###############################################################
# log file에서 call stack address를 뽑아냄
###############################################################
$GREP CALLSTACK $2 | $SED -e 's/\[CALLSTACK:/ /' | $SED -e 's/\]/ /' | $AWK 'BEGIN {i=1;} {printf("%03d %s\n",i,$1); i++}' | tr '[A-Z]' '[a-z]' > stacktrace_temp.log

###############################################################
# print out stacktrace_temp
###############################################################
$AWK '
BEGIN {
    while( (getline < "stacktrace_temp.log") > 0 )
    {
        print $1, $2, getSymbol($2, "stacktrace_temp.map");
    }
}

function getSymbol(aSymbolAddress, aMapFile)
{
    while( (getline < aMapFile) > 0 )
    {
        if( $1 > aSymbolAddress )
        {
            close(aMapFile);
            return sSymbolName;
        }
        sSymbolName = $2;
    }

    close(aMapFile);
    return "unknown";
}'

rm -rf stacktrace_temp.map
rm -rf stacktrace_temp.log
