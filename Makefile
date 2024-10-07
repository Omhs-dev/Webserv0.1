NAME = webserv

CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -I./includes

SRCS =  main.cpp \
		./Network/Servers/Server.cpp \
		./Network/Servers/TestServer.cpp \
		./Network/Sockets/BindingSocket.cpp \
		./Network/Sockets/ConnectingSocket.cpp \
		./Network/Sockets/ListeningSocket.cpp \
		./Network/Sockets/Socket.cpp \
		./Network/Parse/ConfigParser.cpp \
		./Network/Parse/ConfigUtils.cpp

OBJS_DIR = .objs

OBJS = $(patsubst ./Network/%.cpp, $(OBJS_DIR)/%.o, $(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: ./Network/%.cpp
	# Create necessary directories (including subdirectories)
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re