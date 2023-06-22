cmd_/home/cheems/test1/hello.mod := printf '%s\n'   hello.o | awk '!x[$$0]++ { print("/home/cheems/test1/"$$0) }' > /home/cheems/test1/hello.mod
