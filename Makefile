
SHELL := bash
.ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c
.DELETE_ON_ERROR:


MAKEFLAGS := $(MAKEFLAGS)
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables

ifeq ($(origin .RECIPEPREFIX), undefined)
  $(error This Make does not support .RECIPEPREFIX. Please use GNU Make 4.0 or later)
else
  .RECIPEPREFIX = >
endif

NAME := vilya


MKDIR := mkdir -p
RM    := rm -f
RMDIR := rm -fr
TOUCH := touch -a


SOURCES_DIR := ./sources
INCLUDE_DIR := ./includes
OBJECTS_DIR := ./objects

SOURCES_SUB_DIRS := $(shell find $(SOURCES_DIR) -type d)
OBJECTS_SUB_DIRS := $(SOURCES_SUB_DIRS:$(SOURCES_DIR)%=$(OBJECTS_DIR)%)


ifeq ($(MAKECMDGOALS), test)
  SOURCES := $(shell find $(SOURCES_DIR) -type f -not -wholename "*/main.c" -name "*.c")
else
  SOURCES := $(shell find $(SOURCES_DIR) -type f -name "*.c")
endif
OBJECTS := $(SOURCES:$(SOURCES_DIR)/%.c=$(OBJECTS_DIR)/%.o)
DEPS    := $(SOURCES:$(SOURCES_DIR)/%.c=$(OBJECTS_DIR)/%.d)


TEST_DIR         := ./tests
TEST_OBJECTS_DIR := ./tests_objects


TEST_SOURCES := $(shell find $(TEST_DIR) -type f -name "*.c")
TEST_OBJECTS := $(TEST_SOURCES:$(TEST_DIR)/%.c=$(TEST_OBJECTS_DIR)/%.o)


ifeq ($(MAKECMDGOALS), test)
  CC         := gcc
  C_WARNINGS := -Wall -Wextra
  CFLAGS     := -fprofile-arcs -ftest-coverage
  LDFLAGS    := -lgcov --coverage -lcriterion -lelf
else
  CC         := clang
  C_WARNINGS := -Weverything -Wno-gnu-zero-variadic-macro-arguments -Wno-format-nonliteral \
				-Wno-gnu-conditional-omitted-operand -Wno-documentation -Wno-documentation-unknown-command \
				-Wno-padded -Wno-extra-semi-stmt -Wno-cast-align -Wno-sign-conversion
  CFLAGS     := -static
  LDFLAGS    := -lelf
endif


C_DEPS      = -MT $(OBJECTS_DIR)/$*.o -MMD -MP -MF $(OBJECTS_DIR)/$*.d
C_TEST_DEPS = -MT $(TEST_OBJECTS_DIR)/$*.o -MMD -MP -MF $(TEST_OBJECTS_DIR)/$*.d
C_DEBUG     := -ggdb3
C_OPTIMIZE  := -O3


CFLAGS := $(CFLAGS) $(C_DEBUG) $(C_OPTIMIZE) $(C_WARNINGS) -I $(INCLUDE_DIR)


LDFLAGS := $(LDFLAGS)
LDLIBS :=


.PHONY: all
all: $(NAME) ## builds binary

$(NAME): $(OBJECTS)
> $(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

$(OBJECTS_DIR)/%.o: $(SOURCES_DIR)/%.c | $(OBJECTS_SUB_DIRS)
> $(CC) $(CFLAGS) $(C_DEPS) -c $< -o $@

$(OBJECTS_SUB_DIRS):
> $(MKDIR) $(OBJECTS_SUB_DIRS)

.PHONY: clean
clean: ## removes object files
> $(RMDIR) $(OBJECTS_DIR) $(TEST_OBJECTS_DIR)

.PHONY: fclean
fclean: clean ## cleans everything
> $(RM) $(NAME) unit_tests
.PHONY: re
re: fclean all ## rebuilds after cleaning

.PHONY: test ## runs unit tests
test: fclean $(OBJECTS) $(TEST_OBJECTS)
> $(CC) $(OBJECTS) $(TEST_OBJECTS) $(LDFLAGS) $(LDLIBS) -o unit_tests
> ./unit_tests

$(TEST_OBJECTS_DIR)/%.o: $(TEST_DIR)/%.c | $(TEST_OBJECTS_DIR)
> $(CC) $(CFLAGS) $(C_TEST_DEPS) -c $< -o $@

$(TEST_OBJECTS_DIR):
> $(MKDIR) $(TEST_OBJECTS_DIR)

-include $(DEPS)
