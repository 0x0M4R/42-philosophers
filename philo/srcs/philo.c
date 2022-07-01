/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oabdalla <oabdalla@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/30 16:21:52 by oabdalla          #+#    #+#             */
/*   Updated: 2022/06/30 16:21:53 by oabdalla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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
	pthread_mutex_destroy(&dinner_table->token_lock);
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
