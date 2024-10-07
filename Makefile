
NAME = webserv

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

C_FILES = srcs/server/Server.cpp srcs/HttpProtocol/Request.cpp  srcs/HttpProtocol/Response.cpp

O_FILES = $(C_FILES:.cpp=.o)

HEADERS = srcs/server/Server.hpp srcs/HttpProtocol/Request.hpp srcs/HttpProtocol/Response.hpp

all : $(NAME)

$(NAME) : $(O_FILES)
	$(CC) $(FLAGS) -o $@ $(O_FILES)


clean :
	rm -f $(O_FILES)

fclean : clean
	rm -f $(NAME)

re : fclean all
