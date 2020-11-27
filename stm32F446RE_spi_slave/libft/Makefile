NAME = libft.a

SRCSPATH = ft_printf/
OPT_PATH = opt_parse_arg/
LIBFT_INCLUDES = includes

SR =	ft_printf.c\
		flags.c\
		read_arg.c\
		do_d.c\
		do_f.c\
		do_u.c\
		do_o.c\
		do_x.c\
		do_p.c\
		do_c.c\
		do_b.c\
		do_ls.c\
		do_s.c\
		do_not_specifier.c

SRCL =	ft_atoi.c\
	ft_bzero.c\
	ft_capitaliz.c\
	ft_isdigit.c\
	ft_isprint.c\
	ft_itoa_base_buffer.c\
	ft_memcpy.c\
	ft_memset.c\
	ft_strstr.c\
	ft_strncat.c\
	ft_strlen.c

SRC =	$(SRCL)\
		$(addprefix $(SRCSPATH), $(SR))\
		$(addprefix $(OPT_PATH), $(OPT))\

WFLAGS = -Wall -Werror -Wextra

LFLAGS = -L libft/ -lft

CC = gcc

OBJ = $(SRC:.c=.o)

all : $(NAME)

%.o: %.c
	@$(CC) -c $(WFLAGS) -I $(LIBFT_INCLUDES)  $< -o $@

$(NAME) : $(OBJ)
	@echo 'compilation libft'
	@ar rc $(NAME) $(OBJ)
	@ranlib $(NAME)
	@echo '- - - - - - ok'

clean :
	@echo "CLEAN LIBFT"
	@rm -rf $(OBJ)

fclean : clean
	@echo "FCLEAN LIBFT"
	@rm -f $(NAME)

re : fclean all

.PHONY: all clean fclean re
