# Invoke make inside the following directories in this order: loader, launcher, test
all:
	make -C loader
	make -C launcher
	make -C test
	make move

# Move the lib_simpleloader.so and launch binaries inside bin directory
move:
	mkdir -p bin
	mv loader/libsimpleloader.so bin/
	mv launcher/launch bin/
	mv test/fib bin/

# Provide the command for cleanup
clean:
	make -C loader clean
	make -C launcher clean
	make -C test clean
	-@rm -rf bin

