########################
#### VARIABLES #########
########################

NAME		 	= vilya
CC			 	= clang
RM			 	= rm -rf

INCLUDES 	 	= -I./includes/
LINK_FLAG    	=
CFLAGS 		 	= -Wall -Wextra -fPIC -Weverything -O2 \
				  -Wno-padded -Wno-gnu-zero-variadic-macro-arguments -Wno-extra-semi-stmt
PATTERN 	 	= .c
OBJPATTERN  	= .o
SRC_DIR 		= ./sources
OBJECT_DIR 		= ./objects

vpath %$(PATTERN) $(SRC_DIR)
vpath %$(PATTERN) $(SRC_DIR)/elf


MAIN		 	= main.c
UTILS 			= lexxer.c logs.c
ELF 			= file.c

SRC 			= $(MAIN) $(ELF) $(UTILS)

########################
#### COMPILATION #######
########################

COMBINED		= $(SRC) $(MAIN)
OBJ 	 		= $(patsubst %$(PATTERN), $(OBJECT_DIR)/%$(OBJPATTERN), $(COMBINED))

all: directories $(NAME)

$(OBJECT_DIR):
	@mkdir -p $@

directories: | $(OBJECT_DIR)

$(OBJECT_DIR)/%$(OBJPATTERN) : %$(PATTERN)
	@echo "Compiling $@"
	@$(CC) -o $@ -c $< $(CFLAGS) $(DEBUG_FLAGS) $(INCLUDES) $(LINK_FLAG)

$(NAME): $(OBJ)
	@$(CC) -o $(NAME) $^ $(CFLAGS) $(INCLUDES) $(LINK_FLAG)
	@echo "[*** COMPILATION SUCCESSFUL ***]"

clean:
	@$(RM) objects $(NAME)
	@echo "[*** CLEAN ***]"

re: clean all

.PHONY: all clean clean re
