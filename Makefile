
NAME = webserv

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g

# ============= Files path variable ============ #

SERVER_PATH = srcs/server/

CONFIG_FILE_PATH = srcs/configFile/

LOCATION_PATH = srcs/location/

MAIN_PATH = srcs/main/

DIRECTIVE_PATH = srcs/directive/

INCLUDE_PATH = includes/

# ============= Files path variable ============ #

C_FILES = main.cpp $(SERVER_PATH)Server.cpp $(CONFIG_FILE_PATH)configFile.cpp $(LOCATION_PATH)Location.cpp

O_FILES = $(C_FILES:.cpp=.o)

HEADERS = $(INCLUDE_PATH)webserv.h  $(SERVER_PATH)Server.hpp $(CONFIG_FILE_PATH)configFile.hpp $(LOCATION_PATH)Location.hpp $(MAIN_PATH)main.hpp $(DIRECTIVE_PATH)Directive.hpp

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