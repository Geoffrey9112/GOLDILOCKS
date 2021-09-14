ps -ef | grep -v grep | grep $1 | awk '{print "stack.sh " $2}' | sh -v
