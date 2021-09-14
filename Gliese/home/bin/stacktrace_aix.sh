#!/bin/bash

AWK=awk
GREP=grep
SED=sed
NM=nm

os_type=`uname`

###############################################################
# 본 script는 AIX에만 적용이 가능하다.
###############################################################
if [ $os_type != "AIX" ]
then
	echo 'This script must be used in "AIX" Platform'
    exit
fi # AIX

###############################################################
# nm check: GNU nm은 안된다. AIX native nm만 사용 가능
###############################################################
nm_version_file1=$0.nm_version_file1
nm_version_file2=$0.nm_version_file2
$NM -V >& $nm_version_file1
cat $nm_version_file1 | $GREP GNU > $nm_version_file2
if [ -s $nm_version_file2 ]
then
	echo 'Your nm program is GNU Binutils nm. Please, Change your AIX native nm'
	rm -f $nm_version_file1
	rm -f $nm_version_file2
    exit
fi
rm -f $nm_version_file1
rm -f $nm_version_file2

###############################################################
# argument가 맞지 않으면 Usage 출력
###############################################################
if [ $# -ne 2 ]
then
    echo "Usage: $0 <binary file> <log file>"
    exit;
fi

###############################################################
# binary file로부터 stack trace에 사용할 map file 생성
###############################################################
bin_file=$1
map_file=$1.text.map
log_file=$2
callstack_file=$2.callstack.log

# AIX 64bit gcc

$NM -X64 -Pp $bin_file | $GREP ' T ' | sort -k 3  | $AWK '{printf("0x000000010%s %s\n", substr($3,2), $1); }' > $map_file
#cat $bin_file | $GREP ' T ' | $AWK '{printf("0x000000010%s %s\n", substr($1,4), $3); }' | sort > stacktrace_temp.map

###############################################################
# log file에서 call stack address를 뽑아냄
###############################################################
$GREP CALLSTACK $2 | $SED -e 's/\[CALLSTACK:/ /' | $SED -e 's/\]/ /' | $AWK 'BEGIN {i=1;} {printf("%03d %s\n",i,$1); i++}' | tr '[A-Z]' '[a-z]' > $callstack_file

###############################################################
# print out stacktrace_temp
###############################################################
$AWK -v awk_callstack_file=$callstack_file -v awk_map_file=$map_file '
BEGIN {
    while( (getline < awk_callstack_file ) > 0 )
    {
        print $1, $2, getSymbol($2, awk_map_file);
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

rm -rf $map_file
rm -rf $callstack_file
