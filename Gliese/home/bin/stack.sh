gdb -batch --eval-command="thread apply all bt" -nx /proc/$1/exe $1 |
/bin/sed -n \
  -e 's/^(gdb) //' \
  -e '/^#/p' \
  -e '/^Thread/p'
