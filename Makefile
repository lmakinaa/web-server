
NAME = webserv

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

C_FILES = srcs/server/Server.cpp

O_FILES = $(C_FILES:.cpp=.o)

HEADERS = srcs/server/Server.hpp

all : $(NAME)

$(NAME) : $(O_FILES)
	$(CC) $(FLAGS) -o $@ $(O_FILES)

srcs/server/%.o : srcs/server/%.cpp $(HEADERS)
	$(CC) $(FLAGS) -c $< -o $@

clean :
	rm -f $(O_FILES)

fclean : clean
	rm -f $(NAME)

re : fclean all