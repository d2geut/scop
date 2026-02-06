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

GLADNAME = glad
GLADDIR = ./lib/GLAD
GLADBUILD = $(GLADDIR)/build
GLADLIB = $(GLADBUILD)/include

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ -L$(GLADBUILD) -l$(GLADNAME) -L$(GLFWBUILD) -l$(GLFWNAME) \
		-lGL -lX11 -lXrandr -lXi -lXcursor -lm -ldl -lpthread

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -I$(GLADLIB) -I$(GLFWLIB) -c $< -o $@

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