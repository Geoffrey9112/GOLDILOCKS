#!/bin/sh

ps -ef | grep "gmaster" | grep $LOGNAME | kill -9 `awk '{print $2}'`
ps -ef | grep "gserver" | grep $LOGNAME | kill -9 `awk '{print $2}'`
ps -ef | grep "gdispatcher" | grep $LOGNAME | kill -9 `awk '{print $2}'`
ps -ef | grep "gbalancer" | grep $LOGNAME | kill -9 `awk '{print $2}'`
ps -ef | grep "glsnr" | grep $LOGNAME | kill -9 `awk '{print $2}'`
ipcs -m | grep $LOGNAME | awk '{print "ipcrm -m " $2}' | sh -v
ipcs -q | grep $LOGNAME | awk '{print "ipcrm -q " $2}' | sh -v
echo ""
