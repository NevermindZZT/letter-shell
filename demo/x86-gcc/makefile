all : mkdir out

out : main.o shell_port.o shell.o shell_ext.o shell_cmd_list.o
	gcc -o build/out -T shell.lds build/main.o build/shell_port.o build/shell.o build/shell_ext.o build/shell_cmd_list.o

main.o : main.c shell_port.h
	gcc -c main.c -I ./ -I ../../src -o build/main.o

shell_port.o : shell_port.c ../../src/shell.h
	gcc -c shell_port.c -I ./ -I ../../src -o build/shell_port.o

shell.o : ../../src/shell.c ../../src/shell.h shell_cfg.h
	gcc -c ../../src/shell.c -I ./ -I ../../src -o build/shell.o

shell_ext.o : ../../src/shell_ext.c ../../src/shell_ext.h ../../src/shell.h shell_cfg.h
	gcc -c ../../src/shell_ext.c -I ./ -I ../../src -o build/shell_ext.o

shell_cmd_list.o : ../../src/shell_cmd_list.c ../../src/shell.h shell_cfg.h
	gcc -c ../../src/shell_cmd_list.c -I ./ -I ../../src -o build/shell_cmd_list.o

.PHONY : all clean mkdir
clean :
	-rm -r build

mkdir :
	if [ ! -d "build" ]; then (mkdir build) fi