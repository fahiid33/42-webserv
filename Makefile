# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bufallo <bufallo@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/15 00:53:51 by fstitou           #+#    #+#              #
#    Updated: 2023/06/17 08:15:33 by bufallo          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= webserv
CC		= c++
FLAGS	= 
SRCS 	= main.cpp srcs/MultiPlexing.cpp srcs/request.cpp srcs/response.cpp srcs/server.cpp srcs/Socket.cpp srcs/parse_conf.cpp

$(NAME): $(SRCS)
	$(CC) $(FLAGS) $(SRCS) -o $(NAME)

all: $(NAME)

clean:
	@rm -rf *.o

fclean: clean
	@rm -rf $(NAME)
	@rm -rf *.dSYM
	@rm -rf .vscode

re: fclean all