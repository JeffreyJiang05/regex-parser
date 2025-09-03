CC := gcc
EXEC := regex

SOURCE := src/
INCLUDE := include/
TEST := test/
BUILD := build/
BIN := bin/

CFLAGS := -Wall -Werror -Wno-unused-function -DNFA_STATE_LOCKING
DFLAGS := -g -DDEBUG -DCOLOR
OFLAGS := -O3 -march=native
PRINT_STATEMENTS := -DERROR -DSUCCESS -DWARN -DINFO

STD := -std=c17
POSIX := -D_POSIX_SOURCE
BSD := -D_DEFAULT_SOURCE
GNU := -D_GNU_SOURCE

INC := -I$(INCLUDE)
LIBS := -lm
TEST_LIBS := -lcriterion

CFLAGS += $(STD) $(POSIX) $(BSD) $(GNU)

SRCF := $(shell find $(SOURCE) -type f -name *.c)
OBJF := $(patsubst $(SOURCE)%,$(BUILD)%,$(SRCF:.c=.o))
FUNCF := $(filter-out $(BUILD)main.o, $(OBJF))
TESTF := $(shell find $(TEST) -type f -name *.c)

TEST_EXEC := $(EXEC)_tests

.PHONY: clean all debug setup

all: CFLAGS += $(OFLAGS)
all: prod

prod: setup $(BIN)$(EXEC) $(BIN)$(TEST_EXEC)

debug: CFLAGS += $(DFLAGS) $(PRINT_STATEMENTS)
debug: all

setup: $(BIN) $(BUILD)

$(BIN):
	mkdir -p $(BIN)
$(BUILD):
	mkdir -p $(BUILD)

$(BIN)$(EXEC): $(OBJF)
	echo $(SRCF)
	$(CC) $^ -o $@ $(LIBS)

$(BIN)$(TEST_EXEC): $(FUNCF) $(TESTF)
	$(CC) $(CFLAGS) $(INC) $(FUNCF) $(TESTF) $(TEST_LIBS) $(LIBS) -o $@

$(BUILD)%.o: $(SOURCE)%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -rfv $(BUILD) $(BIN)

.PRECIOUS: $(BUILD)*.d