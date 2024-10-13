
NAME = webserv

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g

# ============= Files path variable ============ #

SERVER_PATH = srcs/server/

CONFIG_FILE_PATH = srcs/configFile/

MAIN_PATH = srcs/main/

RESPONSE_PATH = srcs/response/

INCLUDE_PATH = includes/

# ============= Files path variable ============ #

C_FILES = main.cpp $(SERVER_PATH)Server.cpp $(CONFIG_FILE_PATH)configFile.cpp $(CONFIG_FILE_PATH)Location.cpp $(RESPONSE_PATH)Response.cpp

O_FILES = $(C_FILES:.cpp=.o)

HEADERS = $(INCLUDE_PATH)webserv.h  $(SERVER_PATH)Server.hpp $(CONFIG_FILE_PATH)configFile.hpp $(CONFIG_FILE_PATH)Location.hpp $(MAIN_PATH)main.hpp $(CONFIG_FILE_PATH)Directive.hpp $(RESPONSE_PATH)Response.hpp

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