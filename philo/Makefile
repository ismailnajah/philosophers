NAME = philo
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -O3 #-fsanitize=thread
INCLUDE = .

SRC = main.c             \
	  lock.c             \
	  monitor.c          \
	  simulation.c       \
	  simulation_utils.c \
	  philo_actions.c    \
	  philo.c            \
	  parse.c            \
	  time.c             \
	  utils.c


HEADER = philo.h

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
