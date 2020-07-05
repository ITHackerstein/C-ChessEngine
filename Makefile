default: build/main.exe

run: build/main.exe
	build\main.exe

build/main.exe: build/main.o build/Chessboard.o
	gcc -std=c99 -o build/main.exe build/main.o build/Chessboard.o -L libsdl/lib -l mingw32 -l SDL2main -l SDL2 -l SDL2_image

build/main.o: src/main.c
	gcc -std=c99 -c -o build/main.o src/main.c -I libsdl/include

build/Chessboard.o: src/Chessboard.c
	gcc -std=c99 -c -o build/Chessboard.o src/Chessboard.c -I libsdl/include
