# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nboulaye <nboulaye@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/12/08 00:00:00 by nboulaye          #+#    #+#              #
#    Updated: 2020/12/08 00:00:00 by nboulaye         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = spi

SRC = spi_test.c

FLAG = -Wall -Werror -Wextra

CC = gcc

OBJ = $(SRC:.c=.o)

all : $(NAME)

%.o: %.c
	$(CC) -c $(FLAG) $< -o $@

$(NAME) :  ${OBJ}
	$(CC) $(FLAG) -o $(NAME) $(SRC)

clean :
	rm -rf $(OBJ)

fclean : clean	
	rm -f $(NAME)

re : fclean all

.PHONY: all clean fclean re
