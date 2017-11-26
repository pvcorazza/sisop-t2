# 
# Makefile com as regras para gerar a "libt2fs" no diretório lib
#

CC=gcc
LIB_DIR=./lib/
SRC_DIR=./src/

all: lib

lib: $(SRC_DIR)t2fs.o 
	ar crs $(LIB_DIR)libt2fs.a $(SRC_DIR)t2fs.o $(LIB_DIR)apidisk.o

t2fs.o: $(SRC_DIR)t2fs.c
	$(CC) -c $(SRC_DIR)t2fs.c -Wall

clean:
	rm -rf $(SRC_DIR)t2fs.o $(LIB_DIR)libt2fs.a
