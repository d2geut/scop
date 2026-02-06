NAME = scop
CXX = c++
CXFLAGS = -Wall -Wextra -Werror -std=c++11
RM = rm -f

SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

GLFWNAME = glfw3
GLFWDIR = ./lib/GLFW
GLFWBUILD = $(GLFWDIR)/build/src
GLFWLIB = $(GLFWDIR)/include

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ -L$(GLFWBUILD) -l$(GLFWNAME) \
		-lGL -lX11 -lXrandr -lXi -lXcursor -lm -ldl -lpthread

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -I$(GLFWLIB) -c $< -o $@

all : $(NAME)

clean : 
	$(RM) $(OBJS)

fclean :
	$(RM) $(OBJS)
	$(RM) $(NAME)

re :
	$(MAKE) fclean
	$(MAKE) all

.PHONY : all clean fclean re