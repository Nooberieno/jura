CC := gcc
CFLAGS := -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -pedantic
INSTALL_DIR := /usr/local/bin

TARGET := jura
SRC := jura.c

.PHONY: all install develop clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

install: $(TARGET)
	install -m 755 $(TARGET) $(INSTALL_DIR)

develop: CFLAGS += -g -Og
develop: clean $(TARGET)

clean:
	rm -f $(TARGET)

uninstall: 
	rm -f $(INSTALL_DIR)/$(TARGET)
