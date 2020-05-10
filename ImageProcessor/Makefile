
# pip install python-magic
# python -m pip install --user python-magic
# https://imagemagick.org/MagickWand/ 
# Compilar make
# probar make test0

CC = gcc
main: main.c
	@${CC} main.c -o main.o ImageProcessor.c `pkg-config --cflags --libs MagickWand`
test:
	@./main.o test1.png

test0: main2.c
	@${CC} main2.c -o wand.o `pkg-config --cflags --libs MagickWand`
test1: 
		./wand.o pixel.png pixel-media.png pixel-mediana.png  pixel-filtered.png
test2: 
		./wand.o test1.png test1-media.png test1-mediana.png
clean:
		rm -rf *.o *.out

