/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_philo.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oabdalla <oabdalla@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/30 16:21:57 by oabdalla          #+#    #+#             */
/*   Updated: 2022/06/30 16:25:29 by oabdalla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

void	print_status(char *msg, int id, struct s_shared_info *dinner_table)
{
	pthread_mutex_lock(&dinner_table->print_lock);
	printf("%d %d %s\n", (current_time() - \
		dinner_table->time_start), id + 1, msg);
	pthread_mutex_unlock(&dinner_table->print_lock);
}
