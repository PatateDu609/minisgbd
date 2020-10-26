################################################################################
#                               Name of the output                             #
################################################################################
NAME				=	minisgbd

################################################################################
#                          Paths of sources and objects                        #
################################################################################
PATH_INCLUDES		=	include
PATH_SRCS			=	src
PATH_OBJS			=	obj
PATH_LIBS			=	lib

################################################################################
#                              Name of sources files                           #
################################################################################

SRCS_BASENAME = main

################################################################################
#                             Commands and arguments                           #
################################################################################
RM					=	@rm -f
GCC					=	@g++
CFLAGS				=	-g -I$(PATH_INCLUDES)
LDFLAGS				=

# DEBUG LDFLAGS :
LDFLAGS				+=	-fsanitize=address -g

################################################################################
#                         DO NOT MODIFY BELOW THIS POINT                       #
################################################################################

SRCS_EXT			=	$(addsuffix .cpp, $(SRCS_BASENAME))

SRCS				=	$(addprefix $(PATH_SRCS)/, $(SRCS_EXT))
OBJS				=	$(addprefix $(PATH_OBJS)/, $(SRCS_EXT:.cpp=.o))

OS					=	$(shell uname)
ifeq ($(OS), Linux)
	LDFLAGS			+=	-fsanitize=leak
endif

$(PATH_OBJS)/%.o:	$(PATH_SRCS)/%.cpp
					$(GCC) $(CFLAGS) -c $< -o $@

$(NAME):			enter_name $(PATH_LIBS) $(PATH_OBJS) enter_objs $(OBJS)
					@echo "\033[46;90;1mLinking everything\033[0m"
					$(GCC) $(OBJS)  -o $(NAME) $(LDFLAGS)

$(PATH_LIBS):
					@echo "\033[92mCreating libs folder\033[0m"
					@mkdir -p $(PATH_LIBS)

$(PATH_OBJS):
					@echo "\033[92mCreating objs folders\033[0m"

all:				$(NAME)

re:					fclean all

.PHONY:				all clean fclean re

clean:
					$(RM) $(OBJS)

fclean:				clean
					$(RM) -r $(PATH_OBJS) $(PATH_LIBS)

enter_name:
					@echo "\033[31mMaking \033[1m$(NAME)\033[0m"

enter_objs:
					@echo "\033[92mCompiling objs\033[0m"

.PHONY:				all clean fclean re
