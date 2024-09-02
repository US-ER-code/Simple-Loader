# Compile launch.c by linking it with the lib_simpleloader.so, with 32-bit compilation
all:
	gcc -m32 -o launch launch.c -L../loader -lsimpleloader

# Provide the command for cleanup
clean:
	-@rm -f launch