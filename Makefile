#!bin/bash

CC=gcc
CFLAGS=-Wall -Werror -Wno-error=unused-variable -Wextra -pedantic
LIBS=-lraylib -lm

PATH_SRC=src
PATH_BUILD=build

SRC_FILES=main.c renderer.c
TARGET=vdraw

build:
	@echo "[Make] Building binary..."
	$(CC) $(CFLAGS) -o $(PATH_BUILD)/$(TARGET) $(addprefix $(PATH_SRC)/, $(SRC_FILES)) $(LIBS)
	@echo "[Make] Binary built!"
