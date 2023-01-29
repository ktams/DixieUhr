# Gavin lyons 06-2021 
# Makefile to build example program for ST7735_TFT_RPI library.
# URL: https://github.com/gavinlyonsrepo/ST7735_TFT_RPI
# modified by Kersten Tams
# Options
# 1. make = create test file by bin/test from src/main.cpp , 
# 2. make clean = deletes .o files generated (by step 1) from build directory
# 3. make run = run test file (made by step 1), sudo bin/dixie

CC=g++
LDFLAGS= -lbcm2835 -lrt -lST7735_TFT_RPI
CFLAGS = -Iinclude/  -c -Wall 
MD=mkdir

SRC=src
OBJ=obj
SRCS = $(wildcard $(SRC)/*.cpp)
OBJS = $(patsubst $(SRC)/%.cpp,  $(OBJ)/%.o, $(SRCS))
BIN=./bin
TARGET=$(BIN)/dixie

all: clean pre-build $(TARGET) 

run:
	sudo $(TARGET) 


pre-build:
	@echo '***************'
	@echo 'START!'
	$(MD) -vp $(BIN)
	$(MD) -vp $(OBJ)


$(TARGET): $(OBJS) $(OBJ)
	@echo 'MAKE EXE FILE'
	$(CC)  $(OBJS) -o $@ $(LDFLAGS)
	@echo 'DONE!'
	@echo '***************'

$(OBJ)/%.o :  $(SRC)/%.cpp   $(OBJ) 
	@echo 'MAKE OBJECT FILE'
	$(CC) $(CFLAGS) $< -o  $@ 

clean:
	@echo '***************'
	@echo 'CLEANUP!'
	rm -vf  $(OBJS) $(TARGET)
	@echo 'DONE!'
	@echo '***************'


