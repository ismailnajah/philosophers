NAME = philo_bonus
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -g -O3 #-fsanitize=thread
INCLUDE = .

SRC = main_bonus.c       \
	  parse_bonus.c      \
	  time_bonus.c       \
	  utils_bonus.c


HEADER = philo_bonus.h

OBJDIR = obj
OBJ := $(SRC:%.c=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ) $(HEADER)
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $(NAME) $(OBJ)

$(OBJDIR):
	mkdir -p obj

$(OBJDIR)/%.o: %.c $(HEADER) | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ -c $<

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all re clean fclean
.SECONDARY: $(OBJDIR) $(OBJ)
