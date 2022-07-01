/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_philo.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oabdalla <oabdalla@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/30 16:22:08 by oabdalla          #+#    #+#             */
/*   Updated: 2022/07/01 10:37:53 by oabdalla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	init_philos(struct s_shared_info *dinner_table)
{
	int	i;

	dinner_table->time_start = current_time();
	dinner_table->dead = 0;
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
