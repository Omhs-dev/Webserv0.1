NAME = webserv

CXX = c++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror
SRCS =  src/main.cpp \
		src/Server.cpp \
		src/Socket.cpp \
		src/HTTPRequest.cpp \
		src/HTTPResponse.cpp \
		src/ConfigParser.cpp
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
