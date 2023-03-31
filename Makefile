build:
	mkdir build
	gcc src/utils.c src/main.c -o build/tsflood

clean:
	rm -rf build
