NAME = webserv

CXX = c++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror
SRCS =  src/main.cpp \
		src/Server/Server.cpp \
		src/Server/Client.cpp \
		src/Socket/Socket.cpp \
		src/Request/HTTPRequest.cpp \
		src/Response/HTTPResponse.cpp \
		src/Parse/ConfigParser.cpp 
		# src/Parse/ConfigUtils.cpp
OBJS = $(SRCS:.cpp=.o)

INCLUDES = -I include

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all
