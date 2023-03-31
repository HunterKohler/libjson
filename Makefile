SHELL = bash

GCOV ?= gcov
LCOV ?= lcov
GENHTML ?= genhtml
DOXYGEN ?= doxygen

BUILD_DIR = ./build
SOURCE_DIR = ./src
INC_DIR = ./include
TEST_DIR = ./test


BUILD_DIR = ./build
LIB_DIR = $(BUILD_DIR)/lib
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
COV_DIR = $(BUILD_DIR)/coverage
INC_DIR = ./include
SRC_DIR = ./src
TEST_DIR = ./test

LIBJSON_LIB = $(LIB_DIR)/libjson.a
LIBJSON_LIB_SRC = $(shell find '$(SRC_DIR)/libjson' -type f -name '*.c')
LIBJSON_LIB_OBJ = \
	$(patsubst $(SRC_DIR)/libjson/%.c,$(OBJ_DIR)/libjson/%.o,$(LIBJSON_LIB_SRC))

LIBJSON_TEST_SRC = $(shell find '$(TEST_DIR)/libjson' -type f -name '*.c')
LIBJSON_TEST_OBJ = \
	$(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%.o,$(LIBJSON_TEST_SRC))
LIBJSON_TEST_BIN = \
	$(patsubst $(TEST_DIR)/%.c,$(BIN_DIR)/%,$(LIBJSON_TEST_SRC))

LIBJSON_CPPFLAGS = -MP -MD -I$(INC_DIR)
LIBJSON_CFLAGS = \
	-Wall \
	-Wextra \
	-Wcast-align=strict \
	-Wstrict-overflow=5 \
	-Wwrite-strings \
	-Wcast-qual \
	-Wunreachable-code \
	-Wpointer-arith \
	-Warray-bounds \
	-Wno-sign-compare \
	-Wno-switch \
	-Wno-implicit-fallthrough
LIBJSON_LDFLAGS =
LIBJSON_LDLIBS =

ifneq ($(LIBJSON_DEBUG),)
LIBJSON_CPPFLAGS += -DJSON_DEBUG=1
LIBJSON_CFLAGS += -g3 -fsanitize=undefined -fsanitize=address
LIBJSON_LDFLAGS += -g3 -fsanitize=undefined -fsanitize=address
endif

ifneq ($(LIBJSON_OPTIMIZE),)
LIBJSON_CFLAGS += -O3
LIBJSON_LDFLAGS += -03 -flto
else
LIBJSON_CFLAGS += -O0
LIBJSON_LDFLAGS += -O0
endif

CPPFLAGS += $(LIBJSON_CPPFLAGS)
CFLAGS += $(LIBJSON_CFLAGS)
LDFLAGS += $(LIBJSON_LDFLAGS)
LDLIBS += $(LIBJSON_LDLIBS)

.PHONY: all lib test clean docs

all: lib $(LIBJSON_TEST_BIN) docs

lib: $(LIBJSON_LIB)

test: $(LIBJSON_TEST_BIN)

docs:
	@ $(DOXYGEN)

clean:
	@ $(RM) -r ./build

%.o:
	@ mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $(filter-out %.h,$^)

%.a:
	@ mkdir -p $(@D)
	$(AR) rcs $@ $^

$(LIBJSON_LIB): $(LIBJSON_LIB_OBJ)
$(LIBJSON_LIB_OBJ) : build/obj/%.o : $(SRC_DIR)/%.c
$(LIBJSON_TEST_OBJ) : build/obj/%.o : $(TEST_DIR)/%.c
# $(LIBJSON_TEST_BIN):
# 	@mkdir -p $(@D)
# 	$(CXX) $(LDFLAGS) $(LDLIBS) -o $@ $^

-include $(shell find build -name \*.d 2>/dev/null)
