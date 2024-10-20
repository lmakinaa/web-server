
NAME = webserv

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98 #-g -fsanitize=address

# ============= Files path variable ============ #

SERVER_PATH = srcs/server/

CONFIG_FILE_PATH = srcs/configFile/

MAIN_PATH = srcs/main/

INCLUDE_PATH = includes/

# ============= Files path variable ============ #

C_FILES = main.cpp $(SERVER_PATH)Server.cpp $(CONFIG_FILE_PATH)configFile.cpp $(CONFIG_FILE_PATH)Location.cpp $(MAIN_PATH)WebServ.cpp srcs/KQueue/KQueue.cpp srcs/CGI/CGI.cpp\
	srcs/HttpProtocol/Request.cpp  srcs/HttpProtocol/Response.cpp

O_FILES = $(C_FILES:.cpp=.o)

HEADERS = $(INCLUDE_PATH)webserv.h  $(SERVER_PATH)Server.hpp $(CONFIG_FILE_PATH)configFile.hpp $(CONFIG_FILE_PATH)Location.hpp $(MAIN_PATH)WebServ.hpp $(CONFIG_FILE_PATH)Directive.hpp\
	srcs/KQueue/KQueue.hpp srcs/CGI/CGI.hpp srcs/HttpProtocol/Request.hpp  srcs/HttpProtocol/Response.hpp

all : $(NAME)

$(NAME) : $(O_FILES)
	$(CC) $(FLAGS) -o $@ $(O_FILES)

%.o : %.cpp $(HEADERS)
	$(CC) $(FLAGS) -c $< -o $@

clean :
	rm -f $(O_FILES)

fclean : clean
	rm -f $(NAME)

re : fclean all