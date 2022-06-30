#include "philo.h"

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
			if ((is_starving(dinner_table, i) && !is_done(dinner_table, i)))
			{
				pthread_mutex_lock(&dinner_table->dead_lock);
				print_status("died", i, dinner_table);
				dinner_table->dead = 1;
				pthread_mutex_unlock(&dinner_table->dead_lock);
				return (NULL);
			}
			else if (is_done(dinner_table, i))
				return (NULL);
			better_usleep(1);
			i++;
		}
	}
	return (NULL);
}

int	is_dead(struct s_shared_info *dinner_table)
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

int	is_done(struct s_shared_info *dinner_table, int i)
{
	pthread_mutex_lock(&dinner_table->index_lock);
	if (dinner_table->done[i])
	{
		pthread_mutex_unlock(&dinner_table->index_lock);
		return (1);
	}
	pthread_mutex_unlock(&dinner_table->index_lock);
	return (0);
}

int	is_starving(struct s_shared_info *dinner_table, int i)
{
	pthread_mutex_lock(&dinner_table->meal_lock);
	if ((current_time() - dinner_table->philos[i].last_meal) \
	> dinner_table->time_die)
	{
		pthread_mutex_unlock(&dinner_table->meal_lock);
		return (1);
	}
	pthread_mutex_unlock(&dinner_table->meal_lock);
	return (0);
}
