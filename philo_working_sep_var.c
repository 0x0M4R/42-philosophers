#include "philo.h"

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

void	*check_dead(void *data)
{
	struct s_shared_info	*dinner_table;
	int						i;

	dinner_table = data;
	while (1)
	{
		i = 0;
		while (i < dinner_table->num_phil)
		{
			pthread_mutex_lock(&dinner_table->meal_lock);
			pthread_mutex_lock(&dinner_table->index_lock);
			if ((current_time() - dinner_table->philos[i].last_meal) >= \
				dinner_table->time_die && !dinner_table->done[i])
			{
				pthread_mutex_unlock(&dinner_table->index_lock);
				pthread_mutex_unlock(&dinner_table->meal_lock);
				pthread_mutex_lock(&dinner_table->dead_lock);
				print_status("died", i, dinner_table);
				dinner_table->dead = 1;
				pthread_mutex_unlock(&dinner_table->dead_lock);
				return (NULL);
			}
			else if (dinner_table->done[i])
			{
				pthread_mutex_unlock(&dinner_table->meal_lock);
				pthread_mutex_unlock(&dinner_table->index_lock);
				return (NULL);
			}
			pthread_mutex_unlock(&dinner_table->meal_lock);
			pthread_mutex_unlock(&dinner_table->index_lock);
			better_usleep(1);
			i++;
		}
	}
	return (NULL);
}

int is_dead(struct s_shared_info *dinner_table)
{
	pthread_mutex_lock(&dinner_table->dead_lock);
	if (dinner_table->dead)
	{
		pthread_mutex_unlock(&dinner_table->dead_lock);
		return (1);
	}
	pthread_mutex_unlock(&dinner_table->dead_lock);
	return (0);
}

void	do_life(struct s_shared_info *dinner_table, int left, int right, int id)
{
	pthread_mutex_lock(&dinner_table->fork_lock[left]);
	pthread_mutex_lock(&dinner_table->fork_lock[right]);
	while (dinner_table->fork_status[left] && dinner_table->fork_status[right])
		better_usleep(1);
	pthread_mutex_unlock(&dinner_table->fork_lock[right]);
	pthread_mutex_unlock(&dinner_table->fork_lock[left]);
	grab_chopstick(dinner_table, left, id);
	grab_chopstick(dinner_table, right, id);
	pthread_mutex_lock(&dinner_table->meal_lock);
	dinner_table->philos[id].last_meal = current_time();
	pthread_mutex_unlock(&dinner_table->meal_lock);
	if (!is_dead(dinner_table))
		print_status("is eating", id, dinner_table);
	better_usleep(dinner_table->time_eat);
	down_chopsticks(dinner_table, right, left);
	if (!is_dead(dinner_table))
		print_status("is sleeping", id, dinner_table);
	better_usleep(dinner_table->time_sleep);
	if (!is_dead(dinner_table))
		print_status("is thinking", id, dinner_table);
}
void	done_eating(struct s_shared_info *dinner_table, int id)
{
	if (dinner_table->num_phil > 1)
	{
		pthread_mutex_lock(&dinner_table->index_lock);
		dinner_table->done[id] = 1;
		pthread_mutex_unlock(&dinner_table->index_lock);
	}
}
void	*routine(void *data)
{
	struct s_shared_info	*dinner_table;
	int						id;
	int						right;

	dinner_table = data;
	pthread_mutex_lock(&dinner_table->index_lock);
	id = dinner_table->index;
	dinner_table->index++;
	pthread_mutex_unlock(&dinner_table->index_lock);
	right = (id + 1) % dinner_table->num_phil;
	//if (id % 2 == 1)
		//better_usleep(dinner_table->time_eat * 0.9 + 1);
	while (dinner_table->philos[id].num_meals++ < dinner_table->meals && dinner_table->num_phil > 1)
	{
		if (!is_dead(dinner_table))
		{
			if(id == dinner_table->num_phil - 1)
				do_life(dinner_table, id,right, id);
			else
				do_life(dinner_table, right,id, id);
		}
	}
	done_eating(dinner_table,id);
	return (NULL);
}

void	print_status(char *msg, int id, struct s_shared_info *dinner_table)
{
	pthread_mutex_lock(&dinner_table->print_lock);
	printf("%d %d %s\n", (current_time() - \
		dinner_table->time_start), id + 1, msg);
	pthread_mutex_unlock(&dinner_table->print_lock);
}

void	grab_chopstick(struct s_shared_info *dinner_table, int i, int id)
{
	pthread_mutex_lock(&dinner_table->fork_lock[i]);
	dinner_table->fork_status[i] = 1;
	if (!is_dead(dinner_table))
		print_status("has taken a fork", id, dinner_table);
}

void	down_chopsticks(struct s_shared_info *dinner_table, int c1, int c2)
{
	dinner_table->fork_status[c1] = 0;
	dinner_table->fork_status[c2] = 0;
	pthread_mutex_unlock(&dinner_table->fork_lock[c2]);
	pthread_mutex_unlock(&dinner_table->fork_lock[c1]);
}

int	parse_input(struct s_shared_info *dinner_table, int ac, char **av)
{
	if (ac == 5)
		dinner_table->meals = INT_MAX;
	else if (ac == 6 && atoi(av[5]) >= 0)
		dinner_table->meals = atoi(av[5]);
	else
		return (1);
	if (atoi(av[1]) >= 0 && atoi(av[2]) >= 0
		&& atoi(av[3]) >= 0 && atoi(av[4]) >= 0)
	{
		dinner_table->num_phil = atoi(av[1]);
		dinner_table->time_die = atoi(av[2]);
		dinner_table->time_eat = atoi(av[3]);
		dinner_table->time_sleep = atoi(av[4]);
	}
	else
		return (1);
	return (0);
}

int	init_philos(struct s_shared_info *dinner_table)
{
	int	i;

	dinner_table->time_start = current_time();
	dinner_table->dead = 0;
	dinner_table->num_orders = dinner_table->num_phil - 1;
	dinner_table->threadph = malloc(sizeof(pthread_t) * dinner_table->num_phil);
	dinner_table->fork_lock = malloc(sizeof(pthread_mutex_t) \
								* dinner_table->num_phil);
	dinner_table->fork_status = malloc(sizeof(int) * dinner_table->num_phil);
	dinner_table->done = malloc(sizeof(int) * dinner_table->num_phil);
	dinner_table->philos = \
		malloc(sizeof(struct s_private_info) * dinner_table->num_phil);
	if (!dinner_table->threadph || !dinner_table->fork_lock || \
	!dinner_table->fork_status || !dinner_table->done || !dinner_table->philos)
		return (1);
	i = 0;
	while (i < dinner_table->num_phil)
	{
		dinner_table->philos[i].num_meals = 0;
		dinner_table->fork_status[i] = 0;
		dinner_table->philos[i].last_meal = current_time();
		dinner_table->done[i] = 0;
		i++;
	}
	return (0);
}

int	init_mutex(struct s_shared_info *dinner_table)
{
	int	i;

	if (pthread_mutex_init(&dinner_table->meal_lock, NULL))
		return (1);
	if (pthread_mutex_init(&dinner_table->print_lock, NULL))
		return (1);
	if (pthread_mutex_init(&dinner_table->dead_lock, NULL))
		return (1);
	if (pthread_mutex_init(&dinner_table->index_lock, NULL))
		return (1);
	i = 0;
	while (i < dinner_table->num_phil)
	{
		if (pthread_mutex_init(&dinner_table->fork_lock[i], NULL))
			return (1);
		i++;
	}
	return (0);
}

int	create_threads(struct s_shared_info *dinner_table)
{
	int				i;

	i = 0;
	dinner_table->index = 0;
	while (i < dinner_table->num_phil)
	{
		if (pthread_create(&dinner_table->threadph[i], \
		NULL, routine, dinner_table))
			return (1);
		i++;
	}
	if (pthread_create(&dinner_table->death, NULL, check_dead, dinner_table))
		return (1);
	i = 0;
	while (i < dinner_table->num_phil)
	{
		if (pthread_join(dinner_table->threadph[i], NULL))
			return (1);
		i++;
	}
	if (pthread_join(dinner_table->death, NULL))
		return (1);
	return (0);
}

int	destroy_free(struct s_shared_info *dinner_table)
{
	int	i;

	i = 0;
	while (i < dinner_table->num_phil)
	{
		if (pthread_mutex_destroy(&dinner_table->fork_lock[i]))
			return (1);
		i++;
	}
	pthread_mutex_destroy(&dinner_table->index_lock);
	pthread_mutex_destroy(&dinner_table->meal_lock);
	pthread_mutex_destroy(&dinner_table->print_lock);
	pthread_mutex_destroy(&dinner_table->dead_lock);
	free(dinner_table->fork_lock);
	free(dinner_table->fork_status);
	free(dinner_table->done);
	free(dinner_table->threadph);
	free(dinner_table->philos);
	return (0);
}

int	print_error(char *msg)
{
	printf("ERROR %s\n", msg);
	return (1);
}

int	main(int ac, char **av)
{
	struct s_shared_info	*dinner_table;

	dinner_table = malloc(sizeof(struct s_shared_info));
	if (parse_input(dinner_table, ac, av))
		return (print_error("invalid argument(s)"));
	if (init_philos(dinner_table))
		return (print_error("malloc error"));
	if (init_mutex(dinner_table))
		return (print_error("mutex initializing failed"));
	if (create_threads(dinner_table))
		return (print_error("thread creation error"));
	if (destroy_free(dinner_table))
		return (print_error("mutex destroy failed"));
	free(dinner_table);
	return (0);
}
