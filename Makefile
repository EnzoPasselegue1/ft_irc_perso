
NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRC_DIR = src
INC_DIR = includes
OBJ_DIR = obj


SRC_SERVER =	$(SRC_DIR)/server/Server.cpp

SRC_CLIENT =	$(SRC_DIR)/client/Client.cpp

SRC_CHANNEL =	$(SRC_DIR)/channel/Channel.cpp

SRC_COMMANDS =	$(SRC_DIR)/commands/CommandHandler.cpp \
				$(SRC_DIR)/commands/Pass.cpp \
				$(SRC_DIR)/commands/Nick.cpp \
				$(SRC_DIR)/commands/User.cpp \
				$(SRC_DIR)/commands/Join.cpp \
				$(SRC_DIR)/commands/Part.cpp \
				$(SRC_DIR)/commands/Privmsg.cpp \
				$(SRC_DIR)/commands/Kick.cpp \
				$(SRC_DIR)/commands/Invite.cpp \
				$(SRC_DIR)/commands/Topic.cpp \
				$(SRC_DIR)/commands/Mode.cpp \
				$(SRC_DIR)/commands/Quit.cpp \
				$(SRC_DIR)/commands/Ping.cpp \
				$(SRC_DIR)/commands/Who.cpp

SRC_UTILS =		$(SRC_DIR)/utils/Utils.cpp \
				$(SRC_DIR)/utils/Parser.cpp

SRC_MAIN =		$(SRC_DIR)/main.cpp

SRCS = $(SRC_MAIN) $(SRC_SERVER) $(SRC_CLIENT) $(SRC_CHANNEL) $(SRC_COMMANDS) $(SRC_UTILS)

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

INCLUDES = -I$(INC_DIR)

GREEN = \033[0;32m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m


all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(GREEN)Linking $(NAME)...$(NC)"
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) compiled successfully!$(NC)"


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@echo "$(YELLOW)Compiling $<...$(NC)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(NC)"
	rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(NC)"
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
