NAME = pipex
NAME_C = pipex.c

SRC = src
BUILD_DIR =	build

LIBFT_DIR = libft
LIBFT_NAME = ft
LIBFT = $(LIBFT_DIR)/libft.a

INCLUDES = -Iincludes
INCLUDES += -I$(LIBFT_DIR)/includes

CC = cc
# CFLAGS = -Wall -Wextra -Werror -g -fsanitize=address,undefined
CFLAGS = -Wall -Wextra -Werror -g -O0 $(INCLUDES)

LFLAGS = -L$(LIBFT_DIR) -l$(LIBFT_NAME)

C_FDF_FILES = \
	parcer/parcer.c \
	parcer/get_cmd_path.c \
	ast/ast_core.c \
	ast/ast_node_core.c \
	ast/ast_node_core_2.c \
	shell/shell_core.c \
	error/output_error.c 



C_FIlES = $(addprefix $(SRC)/, $(C_FDF_FILES))
C_OBJ_FIlES =	$(C_FIlES:%.c=$(BUILD_DIR)/%.o)

BUILD_DIRS := $(sort $(dir $(C_OBJ_FIlES)))

all : $(NAME)

${NAME} : $(LIBFT) $(BUILD_DIRS) $(C_OBJ_FIlES)
	$(CC) $(CFLAGS) $(C_OBJ_FIlES) $(NAME_C) $(LFLAGS) -o $(NAME)

clean :
	rm -rf $(BUILD_DIR)
	make -C $(LIBFT_DIR) clean

fclean f: clean
	rm -f $(NAME)
	make -C $(LIBFT_DIR) fclean

re	: fclean all

$(BUILD_DIRS):
	mkdir -p $@

$(BUILD_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT) :
	make -C $(LIBFT_DIR) all

.PHONY : all clean fclean re bonus
