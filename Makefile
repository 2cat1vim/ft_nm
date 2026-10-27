NAME = ft_nm
CC = cc
CFLAGS = -g -Wall -Wextra -Werror 
SRC = srcs/main.c \
	  srcs/reg.c \
	  srcs/check.c \
	  srcs/sort.c 

OBJS = $(SRC:.c=.o)

all: $(NAME)


$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) -lm

clean: 
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
