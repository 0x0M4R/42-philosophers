#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <limits.h>

//Add state of forks
//check with helgrind
//find a way to remove exit(0)!!!
// check dead before any action
struct s_private_info
{
	struct timeval	t1;
	int				num_meals;
	int				last_meal;
	int				id;

};
struct s_shared_info
{
	struct s_private_info		*philos;
	pthread_t					*threadph;
	pthread_t					*death;
	pthread_mutex_t				*fork_lock; //shared
	pthread_mutex_t				*index_lock;
	pthread_mutex_t				*root_lock;
	pthread_mutex_t				*print_lock;
	pthread_mutex_t				*dead_lock;
	int							index; //shared
	int							dead;    //shared
	int							*fork_status; //shared
	int							num_phil;
	int							num_orders;
	int							time_start;
	int							time_die;
	int							time_eat;
	int							time_sleep;
	int							meals;
};

void	*routine(void *data);
void	print_status(char *msg, int id, struct s_shared_info *dinner_table);
void	grab_chopstick(struct s_shared_info *dinner_table, int i, int id);
void	down_chopsticks(struct s_shared_info *dinner_table, int c1, int c2);
//int		food_on_table(int i, struct s_philo *philo);
//void	get_token(struct s_philo *philos);
//void	return_token(struct s_philo *philos);

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
	int i;

	dinner_table = data;
	while (!dinner_table->dead)
	{
		i=0;
		while (!dinner_table->dead && i < dinner_table->num_phil && dinner_table->philos[i].num_meals< dinner_table->meals)
		{
			if ((current_time() - dinner_table->philos[i].last_meal) >= dinner_table->time_die)
			{
				pthread_mutex_lock(dinner_table->dead_lock);
				print_status("died", i, dinner_table);
				dinner_table->dead = 1;
				pthread_mutex_unlock(dinner_table->dead_lock);
				exit(0);
			}
			i++;
		}
	}
	return (NULL);
}
void return_token(struct s_shared_info *dinner_table)
{
    pthread_mutex_lock(dinner_table->root_lock);
    dinner_table->num_orders++;
    pthread_mutex_unlock(dinner_table->root_lock);
}

void get_token(struct s_shared_info *dinner_table)
{
	int successful = 0;

	while (!successful)
	 {
		pthread_mutex_lock(dinner_table->root_lock);
		if (dinner_table->num_orders > 0)
		{
			dinner_table->num_orders--;
			successful = 1;
		}
		else
		{
			successful = 0;
        }
        pthread_mutex_unlock(dinner_table->root_lock);
    }
}
void	*routine(void *data)
{
	struct s_shared_info	*dinner_table;
	int						id;

	dinner_table = data;
	pthread_mutex_lock(dinner_table->index_lock);
	id = dinner_table->index;
	dinner_table->index++;
	pthread_mutex_unlock(dinner_table->index_lock);
	while (dinner_table->philos[id].num_meals++ < dinner_table->meals)
	{
		//get_token(dinner_table);
		if(id != dinner_table->num_phil - 1)
		{
			pthread_mutex_lock(&dinner_table->fork_lock[id]);
			pthread_mutex_lock(&dinner_table->fork_lock[(id + 1) % dinner_table->num_phil]);
			while (dinner_table->fork_status[id] && dinner_table->fork_status[id+1%dinner_table->num_phil])
				better_usleep(1);
			pthread_mutex_unlock(&dinner_table->fork_lock[(id + 1) % dinner_table->num_phil]);
			pthread_mutex_unlock(&dinner_table->fork_lock[id]);
			grab_chopstick(dinner_table, id, id);
			grab_chopstick(dinner_table, ((id + 1) % dinner_table->num_phil), id);
			dinner_table->philos[id].last_meal = current_time();
			print_status("is eating", id, dinner_table);
			better_usleep(dinner_table->time_eat);
			down_chopsticks(dinner_table, (id + 1) % dinner_table->num_phil, id);
		}
		else
		{
			pthread_mutex_lock(&dinner_table->fork_lock[(id + 1) % dinner_table->num_phil]);
			pthread_mutex_lock(&dinner_table->fork_lock[id]);
			while (dinner_table->fork_status[id] && dinner_table->fork_status[id+1%dinner_table->num_phil])
				better_usleep(1);
			pthread_mutex_unlock(&dinner_table->fork_lock[id]);
			pthread_mutex_unlock(&dinner_table->fork_lock[(id + 1) % dinner_table->num_phil]);
			grab_chopstick(dinner_table, ((id + 1) % dinner_table->num_phil), id);
			grab_chopstick(dinner_table, id, id);
			dinner_table->philos[id].last_meal = current_time();
			print_status("is eating", id, dinner_table);
			better_usleep(dinner_table->time_eat);
			down_chopsticks(dinner_table, id, (id + 1) % dinner_table->num_phil);
		}
		print_status("is sleeping", id, dinner_table);
		better_usleep(dinner_table->time_sleep);
		print_status("is thinking", id, dinner_table);
	}
	return (NULL);
}

void	print_status(char *msg, int id, struct s_shared_info *dinner_table)
{
	//if (!philo->dead)
	//{
	pthread_mutex_lock(dinner_table->print_lock);
	printf("%d %d %s\n", (current_time() - dinner_table->time_start), id + 1, msg);
	pthread_mutex_unlock(dinner_table->print_lock);
	//}
}
/*
int	food_on_table(int i, struct s_philo *philo)
{
	//pthread_mutex_lock(&(philo->food_lock));
	(philo->num_meals)[i]++;
	//pthread_mutex_unlock(&philo->food_lock);
	return (philo->num_meals[i]);
}*/

void	grab_chopstick(struct s_shared_info *dinner_table, int i, int id)
{
	pthread_mutex_lock(&dinner_table->fork_lock[i]);
	dinner_table->fork_status[i] = 1;
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
	dinner_table->death = malloc(sizeof(pthread_t));
	dinner_table->fork_lock = malloc(sizeof(pthread_mutex_t) * dinner_table->num_phil);

	dinner_table->root_lock = malloc(sizeof(pthread_mutex_t));
	dinner_table->print_lock = malloc(sizeof(pthread_mutex_t));
	dinner_table->dead_lock = malloc(sizeof(pthread_mutex_t));
	dinner_table->index_lock = malloc(sizeof(pthread_mutex_t));
	dinner_table->fork_status = malloc(sizeof(int) * dinner_table->num_phil);
	dinner_table->philos = \
		malloc(sizeof(struct s_private_info) * dinner_table->num_phil);
	//philos->num_meals = malloc(sizeof(int) * philos->num_phil);
	//philos->last_meal = malloc(sizeof(int) * philos->num_phil);
	if (!dinner_table->threadph || !dinner_table->fork_lock || !dinner_table->fork_status || !dinner_table->philos)
		return (1);
	i = 0;
	while (i < dinner_table->num_phil)
	{
		//dinner_table->fork_lock[i] = malloc(sizeof(pthread_mutex_t));
		dinner_table->philos[i].num_meals = 0;
		dinner_table->fork_status[i] = 0;
		dinner_table->philos[i].last_meal = dinner_table->time_start;
		i++;
	}
	return (0);
}

int	init_mutex(struct s_shared_info *dinner_table)
{
	int	i;

	if (pthread_mutex_init(dinner_table->root_lock, NULL))
		return (1);
	if (pthread_mutex_init(dinner_table->print_lock, NULL))
		return (1);
	if (pthread_mutex_init(dinner_table->dead_lock, NULL))
		return (1);
	if (pthread_mutex_init(dinner_table->index_lock, NULL))
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
		//pthread_mutex_lock(dinner_table->root_lock);
		//if(dinner_table->index == i)
		//{
			if (pthread_create(&dinner_table->threadph[i], NULL, routine, dinner_table))
				return (1);
			i++;
		//}
		//pthread_mutex_unlock(dinner_table->root_lock);
	}
	i = 0;
	if (pthread_create(dinner_table->death, NULL, check_dead, dinner_table))
			return (1);
	while (i < dinner_table->num_phil)
	{
		if (pthread_join(dinner_table->threadph[i], NULL))
			return (1);
		i++;
	}
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
	return (0);
}
