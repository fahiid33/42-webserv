# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bufallo <bufallo@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/15 00:53:51 by fstitou           #+#    #+#              #
#    Updated: 2023/06/13 19:08:11 by bufallo          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= webserv
CC		= c++
FLAGS	=
SRCS 	= main.cpp server.cpp request.cpp response.cpp socket.cpp

$(NAME): $(SRCS)
	$(CC) $(FLAGS) $(SRCS) -o $(NAME)

all: $(NAME)

clean:
	@rm -rf *.o

fclean: clean
	@rm -rf $(NAME)

re: fclean all