################################################################################
#                                Name of outputs                               #
################################################################################
NAME				=	minisgbd
TESTS_NAME			=	minisgbd_tests

################################################################################
#                          Paths of sources and objects                        #
################################################################################
PATH_INCLUDES		=	Code/include
PATH_SRCS			=	Code/src
PATH_TESTS			=	Code/tests
PATH_OBJS			=	Code/obj
PATH_OBJS_TESTS		=	$(PATH_OBJS)/tests
PATH_LIBS			=	Code/lib
PATH_DB				=	DB
PATH_DB_TESTS		=	$(PATH_DB)/Tests

################################################################################
#                              Name of sources files                           #
################################################################################

SRCS_BASENAME		=	main			\
						Operators		\
						BufferManager	\
						DBInfo			\
						DBManager		\
						DiskManager		\
						Input			\
						Record			\
						HeapFile		\
						FileManager		\
						RelationInfo	\
						parser			\

################################################################################
#                              Name of test files                              #
################################################################################

TESTS_BASENAME		=	main			\
						DiskManager		\
						BufferManager	\
						Record			\
						DBInfo			\
						HeapFile		\
						parser			\

################################################################################
#                             Commands and arguments                           #
################################################################################
RM					=	@rm -f
GCC					=	@g++
CFLAGS				=	-g -I$(PATH_INCLUDES) -Wall -Werror -Wextra
LDFLAGS				=	-L$(PATH_LIBS) -ltermcap -lgtest -lgtest_main -lpthread -lboost_serialization
LDFLAGS_TESTS		=	$(LDFLAGS)

# DEBUG LDFLAGS :
LDFLAGS				+=	-fsanitize=address -g

################################################################################
#                         DO NOT MODIFY BELOW THIS POINT                       #
################################################################################

SRCS_EXT			=	$(addsuffix .cpp, $(SRCS_BASENAME))
TESTS_EXT			=	$(addsuffix Tests.cpp, $(TESTS_BASENAME))

SRCS				=	$(addprefix $(PATH_SRCS)/, $(SRCS_EXT))
TESTS				=	$(addprefix $(PATH_TESTS)/, $(TESTS_EXT))
OBJS				=	$(addprefix $(PATH_OBJS)/, $(SRCS_EXT:.cpp=.o))
OBJS_TESTS			=	$(addprefix $(PATH_OBJS_TESTS)/, $(TESTS_EXT:.cpp=.o))

OS					=	$(shell uname)
ifeq ($(OS), Linux)
	LDFLAGS			+=	-fsanitize=leak
endif

$(PATH_OBJS)/%.o:			$(PATH_SRCS)/%.cpp
							$(GCC) $(CFLAGS) -c $< -o $@

$(PATH_OBJS_TESTS)/%.o:		$(PATH_TESTS)/%.cpp
							$(GCC) $(CFLAGS) -c $< -o $@

$(NAME):					enter_name $(PATH_DB) $(PATH_LIBS) $(PATH_OBJS) enter_objs $(OBJS)
							@echo "\033[46;90;1mLinking everything\033[0m"
							$(GCC) $(OBJS) -o $(NAME) $(LDFLAGS)

$(TESTS_NAME):				enter_tests $(PATH_DB_TESTS) $(PATH_LIBS) $(PATH_OBJS) enter_objs $(OBJS) enter_tests_objs $(OBJS_TESTS)
							@echo "\033[46;90;1mLinking all tests\033[0m"
							$(GCC) $(filter-out $(PATH_OBJS)/main.o, $(OBJS)) $(OBJS_TESTS) -o $(TESTS_NAME) $(LDFLAGS_TESTS)

$(PATH_LIBS):
							@echo "\033[92mCreating libs folder\033[0m"
							@mkdir $(PATH_LIBS)

$(PATH_OBJS):
							@echo "\033[92mCreating objs folders\033[0m"
							@mkdir $(PATH_OBJS)
							@mkdir $(PATH_OBJS_TESTS)

$(PATH_DB):
							@echo "\033[92mCreating database folder\033[0m"
							@mkdir $(PATH_DB)

$(PATH_DB_TESTS):
							@echo "\033[92mCreating test database folder\033[0m"
							@mkdir -p $(PATH_DB_TESTS)

all:						$(NAME)

tests:						$(TESTS_NAME)
							@echo "\n"
							@echo "\033[94;1mLaunching tests\033[0m\n"
							@./$(TESTS_NAME)

re:							fclean all

.PHONY:						all clean fclean re

clean:
							$(RM) $(OBJS)

fclean:						clean
							$(RM) -r $(PATH_OBJS)

enter_name:
							@echo "\033[31mMaking \033[1m$(NAME)\033[0m"

enter_tests:
							@echo "\033[31mMaking \033[1m$(NAME)\033[0;31m but with tests\033[0,"

enter_objs:
							@echo "\033[92mCompiling objs\033[0m"

enter_tests_objs:
							@echo "\033[92mCompiling test objs\033[0m"

.PHONY:						all clean fclean re tests
