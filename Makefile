NAME=webserv
CC=c++
CFLAGS=-Wall -Werror -std=c++98 -g
SRCS=main.cpp parse_conf.cpp

# Path: srcs/

ALL: $(NAME)

$(NAME): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(NAME)
