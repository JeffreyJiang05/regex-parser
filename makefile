CC := gcc
EXEC := regex

SOURCE := src/
INCLUDE := include/
TEST := test/
BUILD := build/
BIN := bin/

CFLAGS := -Wall -Werror -Wno-unused-function -DNFA_STATE_LOCKING -DDFA_STATE_LOCKING
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

define remove_slashes
  $(shell echo $(subst /,.,$1) | sed 's/\./\//')
endef

SRCF := $(shell find src/ -type f -name '*.c')
OBJF := $(patsubst $(SOURCE)%,$(BUILD)%,$(SRCF:.c=.o))
OBJF := $(foreach f, $(OBJF), $(call remove_slashes, $(f)))
OBJF_COUNT := $(words $(OBJF))
FUNCF := $(filter-out $(BUILD)main.o, $(OBJF))
TESTF := $(shell find $(TEST) -type f -name *.c)

TEST_EXEC := $(EXEC)_tests

.PHONY: clean all debug setup

all: CFLAGS += $(OFLAGS)
all: prod

prod: setup $(BIN)$(EXEC) $(BIN)$(TEST_EXEC)

debug: CFLAGS += $(DFLAGS) $(PRINT_STATEMENTS)
debug: prod

setup: $(BIN) $(BUILD)

$(BIN):
	mkdir -p $(BIN)
$(BUILD):
	mkdir -p $(BUILD)

$(BIN)$(EXEC): $(OBJF)
	$(CC) $^ -o $@ $(LIBS)

$(BIN)$(TEST_EXEC): $(FUNCF) $(TESTF)
	$(CC) $(CFLAGS) $(INC) $(FUNCF) $(TESTF) $(TEST_LIBS) $(LIBS) -o $@

define BUILD_RECIPE_TEMPLATE
$1: $2
	$$(CC) $$(CFLAGS) $$(INC) -c $$< -o $$@
endef
$(foreach i, $(shell seq $(OBJF_COUNT)), \
		$(eval $(call BUILD_RECIPE_TEMPLATE, $(word $(i), $(OBJF)), $(word $(i), $(SRCF))) ))

clean:
	rm -rfv $(BUILD) $(BIN)

.PRECIOUS: $(BUILD)*.d