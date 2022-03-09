########################
#### VARIABLES #########
########################

NAME		 	= vilya
UTESTS_BIN 		= vilya_unit_tests

CC			 	= clang
RM			 	= rm -rf

INCLUDES 	 	= -I./includes/
LINK_FLAG    	=
CFLAGS 		 	= -Wall -Wextra -fPIC -Weverything -O2 \
				  -Wno-padded \
				  -Wno-gnu-zero-variadic-macro-arguments -Wno-extra-semi-stmt
PATTERN 	 	= .c
OBJPATTERN  	= .o
SRC_DIR 		= ./sources
OBJECT_DIR 		= ./objects

vpath %$(PATTERN) $(SRC_DIR)
vpath %$(PATTERN) $(SRC_DIR)/elf


MAIN		 	= main.c
UTILS 			= lexxer.c logs.c file.c
ELF 			=

SRC 			= $(MAIN) $(ELF) $(UTILS)
TESTS 			= $(ELF) $(UTILS)

########################
#### COMPILATION #######
########################

COMBINED		= $(SRC) $(MAIN)
OBJ 	 		= $(patsubst %$(PATTERN), $(OBJECT_DIR)/%$(OBJPATTERN), $(COMBINED))
OBJT 			= $(patsubst %$(PATTERN), $(OBJECT_DIR)/%$(OBJPATTERN), $(TESTS))

all: directories $(NAME) ## compiles the packer

$(OBJECT_DIR):
	@mkdir -p $@

directories: | $(OBJECT_DIR)

$(OBJECT_DIR)/%$(OBJPATTERN) : %$(PATTERN)
	@echo "Compiling $@"
	@$(CC) -o $@ -c $< $(CFLAGS) $(DEBUG_FLAGS) $(INCLUDES) $(LINK_FLAG)

$(NAME): $(OBJ)
	@$(CC) -o $(NAME) $^ $(CFLAGS) $(INCLUDES) $(LINK_FLAG)
	@echo "[*** COMPILATION SUCCESSFUL ***]"

clean: ## cleans object files and deletes binary
	@$(RM) objects $(NAME)
	@echo "[*** CLEAN ***]"

re: clean all ## cleans and rebuilds the binary

########################
###### TESTING #########
########################


test: clean directories $(OBJT) ## runs unit tests
	@$(CC) -o $(UTESTS_BIN) $(OBJT) --coverage $(INCLUDES) $(LINK_FLAG) -lcriterion
	@./$(UTESTS_BIN) > /dev/null
	@gcovr --exclude ./tests/ -s -p
	@$(RM) $(UTESTS_BIN) $(NAME) *.gcda *.gcno $(OBJECT_DIR)
	@echo "[*** TESTS DONE ***]"

lint: ## Runs linting script
	@find -type f -name "*.c" -o -type f -name "*.h" -exec ./tests/lint.sh {} \;

########################
##### HELP DIALOG ######
########################

help: ## Displays help dialog
	@echo "\033[34mVilya targets:\033[0m"
	@perl -nle'print $& if m{^[a-zA-Z_-\d]+:.*?## .*$$}' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-22s\033[0m %s\n", $$1, $$2}'


.PHONY: all clean clean re help test
