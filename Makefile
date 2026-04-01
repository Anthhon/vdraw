#!bin/bash

CC=gcc
CFLAGS=-Wall -Werror -Wno-error=unused-variable -Wextra -pedantic
LIBS=-lraylib -lm

PATH_SRC=src
PATH_BUILD=build

SRC_FILE=main.c
BIN_NAME=vdraw

build:
	@echo "[Make] Building binary..."
	$(CC) $(CFLAGS) -o $(PATH_BUILD)/$(BIN_NAME) $(addprefix $(PATH_SRC)/, $(SRC_FILE)) $(LIBS)
	@echo "[Make] Binary built!"
