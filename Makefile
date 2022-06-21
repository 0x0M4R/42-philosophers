NAME = philo
OBJS = ${SRCS:.c=.o}

all:	${NAME}

${NAME}:
	gcc -g -Wall -Wextra -Werror -pthread -I. philo_working_sep_var.c  -o philo
clean:
	rm -f ${NAME}
fclean:	clean

re:	fclean all