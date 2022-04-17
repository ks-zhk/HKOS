# test1.o:test1.c
# 	gcc -c -o test1.o test1.c
# test2.o:test2.c
# 	gcc -c -o test2.o test2.c
%.o:%.c
	gcc -c -o $@ $^
objfile = test1.o test2.o
test.bin:$(objfile)
	gcc -o $@ $^
all:test.bin
	@echo "compile ok"