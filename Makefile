CC := gcc
CFLAGS := -g -Isrc -I/opt/homebrew/include
# LDFLAGS := -Wl,-rpath,'$$ORIGIN'
LDFLAGS := -Wl,-rpath,'@executable_path'


CURRENT_DIR := $(shell pwd)

BUILD := build
BIN := bin
SRCS := $(wildcard src/*.c)
TEST_SRCS := $(wildcard tests/*.c)
TARGETS := libcaph.so

SRC := $(filter-out $(patsubst %,%.c,$(TARGETS)), $(SRCS))
SRC_OBJ = $(patsubst src/%.c,$(BUILD)/%.o,$(SRC))
TEST_OBJ := $(patsubst tests/%.c,$(BUILD)/%.o,$(TEST_SRCS))

OUTPUTS := $(patsubst %,$(BIN)/%, $(TARGETS))
TEST_BIN := $(patsubst tests/%.c,$(BIN)/%,$(TEST_SRCS))


.PHONY: makedirs all clean
.PRECIOUS: $(SRC_OBJ) $(TEST_OBJ)


all: makedirs $(OUTPUTS) $(TEST_BIN)

debug:
	@echo $(TEST_BIN)
	@echo $(OUTPUTS)

makedirs: 
	@mkdir -p $(BIN)
	@mkdir -p $(BUILD)
	@mkdir -p $(BUILD)/utils

$(BIN): 
	@mkdir -p $@



$(BIN)/%.so: $(SRC_OBJ)
	$(CC) -shared $(CFLAGS) -o $@ $^ -L/opt/homebrew/lib  -ljson-c -lcurl

$(BIN)/%: $(BUILD)/%.o
	$(CC) $(LDFLAGS) -o $@ $^ -L$(CURRENT_DIR)/bin -lcaph 


$(BUILD)/%.o : src/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD)/%.o : tests/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(BIN) $(BUILD)
