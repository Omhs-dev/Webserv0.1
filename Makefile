# Executable names
NAME = webserv
TESTNAME = runTests

# Compiler settings
CXX = c++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror #-fsanitize=address -fsanitize=undefined -g 

# Directories
SRC_DIR = src
TEST_DIR = src/Test
INCLUDE_DIR = include

# Source files
SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/Server/Server.cpp \
       $(SRC_DIR)/Server/Client.cpp \
       $(SRC_DIR)/Socket/Socket.cpp \
       $(SRC_DIR)/Request/HTTPRequest.cpp \
       $(SRC_DIR)/Response/HTTPResponse.cpp \
       $(SRC_DIR)/Response/HTTPResponseUtils.cpp \
       $(SRC_DIR)/Parse/ConfigParser.cpp \
       $(SRC_DIR)/Parse/LocationConfig.cpp \
       $(SRC_DIR)/Parse/ServerConfig.cpp \
       $(SRC_DIR)/Logger/Logger.cpp

# Object files (excluding the main application file for tests)
OBJS = $(filter-out $(SRC_DIR)/main.o, $(SRCS:.cpp=.o))

# Test source files
TESTSRCS = $(TEST_DIR)/test.cpp \
           $(TEST_DIR)/HTTPRequestTest.cpp \
           $(TEST_DIR)/HTTPResponseTest.cpp \
           $(TEST_DIR)/ServerTest.cpp \
           $(TEST_DIR)/ClientTest.cpp \
           $(TEST_DIR)/TestUtils.cpp

# Test object files
TESTOBJS = $(TESTSRCS:.cpp=.o)

# Include paths
INCLUDES = -I $(INCLUDE_DIR)

# Build the main executable
all: $(NAME)

$(NAME): $(OBJS) $(SRC_DIR)/main.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(SRC_DIR)/main.o -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean object files and executables
clean:
	rm -rf $(OBJS) $(TESTOBJS) $(SRC_DIR)/*.o $(TEST_DIR)/*.o

fclean: clean
	rm -rf $(NAME) $(TESTNAME)

re: fclean all

# Test target
test: $(TESTNAME)

$(TESTNAME): $(TESTOBJS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(TESTOBJS) $(OBJS) -o $(TESTNAME)

# Clean test files only
testclean:
	rm -rf $(TESTOBJS)

testfclean: testclean
	rm -rf $(TESTNAME)

testre: testfclean test
