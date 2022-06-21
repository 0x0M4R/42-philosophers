#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <limits.h>
#define FOOD 50

struct s_philo
{
	pthread_t		*threadph;
	pthread_t		*death;
	pthread_mutex_t	*fork_lock;
	pthread_mutex_t	time_lock;
	pthread_mutex_t	dead_lock;
	struct timeval	t1;
	int				*num_meals;
	int				dead;
	int				id;
	int				*last_meal;
	int				num_phil;
	int				num_allowed;
	int				time_start;
	int				time_die;
	int				time_eat;
	int				time_sleep;
	int				meals;
};

void	*routine(void *num);
void	print_status(char *msg, int id, struct s_philo *philos);
void	grab_chopstick(struct s_philo *philos, int i, int id);
void	down_chopsticks(struct s_philo *philos, int c1, int c2);
int		food_on_table(int i, struct s_philo *philo);
void	get_token(struct s_philo *philos);
void	return_token(struct s_philo *philos);

int	current_time(void)
{
	struct timeval	curr;

	gettimeofday(&curr, NULL);
	return (curr.tv_sec * 1000 + curr.tv_usec / 1000);
}

void	better_usleep(int t)
{
	int	start;

	start = current_time();
	while (current_time() - start < t)
		usleep(100);
}

void	*check_dead(void *num)
{
	struct s_philo	philo;

	philo = *(struct s_philo *)(num);
	while ((philo.num_meals[philo.id] < philo.meals))
	{
		if ((current_time() - philo.last_meal[philo.id]) >= philo.time_die)
		{
			pthread_mutex_lock(&philo.dead_lock);
			print_status("died", philo.id, &philo);
			philo.dead = 1;
			pthread_mutex_unlock(&philo.dead_lock);
			exit(0);
		}
	}
	return (NULL);
}

void	*routine(void *num)
{
	struct s_philo	philo;
	int				id;

	philo = *(struct s_philo *)(num);
	id = philo.id;
	if (id % 2 == 1)
		better_usleep(philo.time_eat / 50);
	while (philo.num_meals[id]++ <= philo.meals)
	{
		grab_chopstick(&philo, id, id);
		grab_chopstick(&philo, id + 1 % philo.num_phil, id);
		philo.last_meal[id] = current_time();
		print_status("is eating", id, &philo);
		better_usleep(philo.time_eat);
		down_chopsticks(&philo, id, id + 1 % philo.num_phil);
		print_status("is sleeping", id, &philo);
		better_usleep(philo.time_sleep);
		print_status("is thinking", id, &philo);
	}
	return (NULL);
}

void	print_status(char *msg, int id, struct s_philo *philo)
{
	if (!philo->dead)
	{
		pthread_mutex_lock(&philo->time_lock);
		printf("%d %d %s\n", (current_time() - philo->time_start), id, msg);
		pthread_mutex_unlock(&philo->time_lock);
	}
}
void	grab_chopstick(struct s_philo *philo, int i, int id)
{
	pthread_mutex_lock(&philo->fork_lock[i]);
	print_status("has taken a fork", id, philo);
}

void	down_chopsticks(struct s_philo *philos, int c1, int c2)
{
	pthread_mutex_unlock(&philos->fork_lock[c1]);
	pthread_mutex_unlock(&philos->fork_lock[c2]);
}

int	parse_input(int ac, char **av, struct s_philo *philos)
{
	if (ac == 5)
		philos->meals = INT_MAX;
	else if (ac == 6 && atoi(av[5]) >= 0)
		philos->meals = atoi(av[5]);
	else
		return (1);
	if (atoi(av[1]) >= 0 && atoi(av[2]) >= 0
		&& atoi(av[3]) >= 0 && atoi(av[4]) >= 0)
	{
		philos->num_phil = atoi(av[1]);
		philos->time_die = atoi(av[2]);
		philos->time_eat = atoi(av[3]);
		philos->time_sleep = atoi(av[4]);
	}
	else
		return (1);
	return (0);
}

int	init_philos(struct s_philo *philos)
{
	int	i;

	philos->time_start = current_time();
	philos->dead = 0;
	philos->threadph = malloc(sizeof(pthread_t) * philos->num_phil);
	philos->death = malloc(sizeof(pthread_t) * philos->num_phil);
	philos->fork_lock = malloc(sizeof(pthread_mutex_t) * philos->num_phil);
	philos->num_meals = malloc(sizeof(int) * philos->num_phil);
	philos->last_meal = malloc(sizeof(int) * philos->num_phil);
	if (!philos->threadph || !philos->death ||!philos->fork_lock
		|| !philos->num_meals || !philos->last_meal)
		return (1);
	i = 0;
	while (i < philos->num_phil)
	{
		philos->num_meals[i] = 0;
		philos->last_meal[i] = philos->time_start;
		i++;
	}
	return (0);
}

int	init_mutex(struct s_philo *philos)
{
	int	i;

	if (pthread_mutex_init(&philos->dead_lock, NULL))
		return (1);
	i = 0;
	while (i < philos->num_phil)
	{
		if (pthread_mutex_init(&philos->fork_lock[i], NULL))
			return (1);
		i++;
	}
	return (0);
}

int	create_threads(struct s_philo *philos)
{
	int				i;
	struct s_philo	*temp;

	i = 0;
	while (i < philos->num_phil)
	{
		temp = malloc(sizeof(struct s_philo));
		*temp = *philos;
		(*temp).id = i;
		if (pthread_create(&philos->threadph[i], NULL, (void *)routine, temp))
			return (1);
		if (pthread_create(&philos->death[i], NULL, (void *)check_dead, temp))
			return (1);
		i++;
	}
	i = 0;
	while (i < philos->num_phil)
	{
		if (pthread_join(philos->threadph[i], NULL))
			return (1);
		if (pthread_join(philos->death[i], NULL))
			return (1);
	}
	return (0);
}

int	destroy_free(struct s_philo *philos)
{
	int	i;

	i = 0;
	while (i < philos->num_phil)
	{
		if (pthread_mutex_destroy(&philos->fork_lock[i]))
			return (1);
	}

	return (0);
}

int	print_error(char *msg)
{
	printf("ERROR %s\n", msg);
	return (1);
}

int	main(int ac, char **av)
{
	struct s_philo	philos;

	if (parse_input(ac, av, &philos))
		return (print_error("invalid argument(s)"));
	if (init_philos(&philos))
		return (print_error("malloc error"));
	if (init_mutex(&philos))
		return (print_error("mutex initializing failed"));
	if (create_threads(&philos))
		return (print_error("thread creation error"));
	if (destroy_free(&philos))
		return (print_error("mutex destroy failed"));
	return (0);
}
