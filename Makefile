NAME = scop
CXX = c++
CXFLAGS = -Wall -Wextra -Werror
RM = rm -f

SRCSDIR = ./src
SRCSNAME = main.cpp \
	common.cpp \
	shader.cpp \
	program.cpp \
	context.cpp \
	buffer.cpp \
	vertex_layout.cpp
SRCS = $(addprefix $(SRCSDIR)/, $(SRCSNAME))
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