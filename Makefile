NAME = webserv

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

INCLUDES = -I./includes

SRCS = 
#$(wildcard ./Network/*.cpp)

OBJS_DIR = .objs
OBJS = $(patsubst ./Network/%.cpp, $(OBJS_DIR)/%.o, $(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: ./Network/%.cpp
	@mkdir -p $(OBJS_DIR)
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re