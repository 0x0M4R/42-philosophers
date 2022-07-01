SRCS	= srcs/philo.c srcs/death_philo.c srcs/init_philo.c srcs/life_philo.c srcs/utils_philo.c

NAME = philo
OBJS = ${SRCS:.c=.o}

all:	${NAME}

${NAME}:
	gcc -Wall -Wextra -Werror -pthread -I. ${SRCS} -o ${NAME}

clean:
	rm -f ${NAME}

fclean:	clean

re:	fclean all