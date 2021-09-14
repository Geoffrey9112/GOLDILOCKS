#!/bin/sh

ps -ef | grep "gmaster" | grep $LOGNAME | kill -9 `awk '{print $2}'` 2> /dev/null
ps -ef | grep "gserver" | grep $LOGNAME | kill -9 `awk '{print $2}'` 2> /dev/null
ps -ef | grep "gdispatcher" | grep $LOGNAME | kill -9 `awk '{print $2}'` 2> /dev/null
ps -ef | grep "gbalancer" | grep $LOGNAME | kill -9 `awk '{print $2}'` 2> /dev/null
ps -ef | grep "glsnr" | grep $LOGNAME | kill -9 `awk '{print $2}'` 2> /dev/null
ipcs -m | grep $LOGNAME | awk '{print "ipcrm -m " $2}' | sh -v 2> /dev/null
ipcs -q | grep $LOGNAME | awk '{print "ipcrm -q " $2}' | sh -v 2> /dev/null
rm ${GOLDILOCKS_DATA}/db/*.dbf ${GOLDILOCKS_DATA}/wal/*.ctl ${GOLDILOCKS_DATA}/wal/*.log ${GOLDILOCKS_DATA}/archive_log/*.log ${GOLDILOCKS_DATA}/backup/*.inc ${GOLDILOCKS_DATA}/trc/opt_* ${GOLDILOCKS_DATA}/db/*TBS 2> /dev/null
rm ${GOLDILOCKS_DATA}/conf/goldilocks.properties.binary 2> /dev/null

# listener는 내부에서 fork(daemon)되어 초기화 시간이 필요함으로 먼저 실행함.
glsnr --start

gcreatedb --db_name="TEST_DB"

GSQL="gsql --as sysdba --no-prompt"
${GSQL} << EOF
\startup
--# for regression test
GRANT ALL PRIVILEGES ON DATABASE TO TEST WITH GRANT OPTION;
COMMIT;
EOF

# build DICTIONARY_SCHEMA
gsql --as sysdba --silent --import $GOLDILOCKS_HOME/admin/DictionarySchema.sql

# build INFORMATION_SCHEMA
gsql --as sysdba --silent --import $GOLDILOCKS_HOME/admin/InformationSchema.sql

# build PERFORMANCE_VIEW_SCHEMA
gsql --as sysdba --silent --import $GOLDILOCKS_HOME/admin/PerformanceViewSchema.sql

