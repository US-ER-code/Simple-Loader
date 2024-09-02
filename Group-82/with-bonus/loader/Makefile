# Create lib_simpleloader.so from loader.c with 32-bit compilation
all:
	gcc -m32 -fPIC -shared -o libsimpleloader.so loader.c

# Provide the command for cleanup
clean:
	-@rm -f lib_simpleloader.so
