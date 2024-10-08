NAME=webserv
CFLAGS=-Wall -Wextra -Werror -std=c++98
CC=c++
SRCS=$(wildcard srcs/*/*.cpp) srcs/main.cpp
INC=$(wildcard srcs/*/*.hpp)
OBJ=$(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp $(INC)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: clean
