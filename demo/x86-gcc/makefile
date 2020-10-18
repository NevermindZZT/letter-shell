all : mkdir out

out : main.o shell_port.o shell.o shell_ext.o shell_cmd_list.o shell_companion.o shell_fs.o shell_cmd_group.o
	gcc -o build/out -T shell.lds build/main.o build/shell_port.o \
	build/shell.o build/shell_ext.o build/shell_cmd_list.o \
	build/shell_companion.o build/shell_fs.o build/shell_cmd_group.o

main.o : main.c shell_port.h
	gcc -c main.c -I ./ -I ../../src -o build/main.o

shell_port.o : shell_port.c ../../src/shell.h ../../extensions/fs_support/shell_fs.h
	gcc -c shell_port.c -I ./ -I ../../src -I ../../extensions/fs_support -o build/shell_port.o

shell.o : ../../src/shell.c ../../src/shell.h shell_cfg.h
	gcc -c ../../src/shell.c -I ./ -I ../../src -o build/shell.o

shell_ext.o : ../../src/shell_ext.c ../../src/shell_ext.h ../../src/shell.h shell_cfg.h
	gcc -c ../../src/shell_ext.c -I ./ -I ../../src -o build/shell_ext.o

shell_cmd_list.o : ../../src/shell_cmd_list.c ../../src/shell.h shell_cfg.h
	gcc -c ../../src/shell_cmd_list.c -I ./ -I ../../src -o build/shell_cmd_list.o

shell_companion.o : ../../src/shell_companion.c ../../src/shell.h shell_cfg.h
	gcc -c ../../src/shell_companion.c -I ./ -I ../../src -o build/shell_companion.o

shell_fs.o : ../../extensions/fs_support/shell_fs.c ../../extensions/fs_support/shell_fs.h shell_cfg.h ../../src/shell.h
	gcc -c ../../extensions/fs_support/shell_fs.c -I ./ -I ../../src -I ../../extensions/fs_support -o build/shell_fs.o

shell_cmd_group.o : ../../extensions/shell_enhance/shell_cmd_group.c ../../extensions/shell_enhance/shell_cmd_group.h shell_cfg.h ../../src/shell.h
	gcc -c ../../extensions/shell_enhance/shell_cmd_group.c -I ./ -I ../../src -I ../../extensions/shell_enhance -o build/shell_cmd_group.o

.PHONY : all clean mkdir
clean :
	-rm -r build

mkdir :
	if [ ! -d "build" ]; then (mkdir build) fi