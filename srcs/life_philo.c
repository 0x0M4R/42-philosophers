#include "philo.h"

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
	if (id % 2 == 1)
		better_usleep(dinner_table->time_eat * 0.95);
	while (dinner_table->philos[id].num_meals++ < dinner_table->meals \
	&& dinner_table->num_phil > 1)
	{
		if (is_dead(dinner_table))
			break ;
		if (id == dinner_table->num_phil - 1)
			do_life(dinner_table, right, id, id);
		else
			do_life(dinner_table, id, right, id);
	}
	done_eating(dinner_table, id);
	return (NULL);
}

void	do_life(struct s_shared_info *dinner_table, int left, int right, int id)
{
	pthread_mutex_lock(&dinner_table->fork_lock[left]);
	pthread_mutex_lock(&dinner_table->fork_lock[right]);
	while (dinner_table->fork_status[left] && dinner_table->fork_status[right])
		better_usleep(1);
	pthread_mutex_unlock(&dinner_table->fork_lock[left]);
	pthread_mutex_unlock(&dinner_table->fork_lock[right]);
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

void	done_eating(struct s_shared_info *dinner_table, int id)
{
	if (dinner_table->num_phil > 1)
	{
		pthread_mutex_lock(&dinner_table->index_lock);
		dinner_table->done[id] = 1;
		pthread_mutex_unlock(&dinner_table->index_lock);
	}
}
