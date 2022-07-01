/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oabdalla <oabdalla@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/30 16:23:41 by oabdalla          #+#    #+#             */
/*   Updated: 2022/06/30 16:23:54 by oabdalla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H
# include <stdio.h>
# include <pthread.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <limits.h>

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
	pthread_t					death;
	pthread_mutex_t				*fork_lock;
	pthread_mutex_t				index_lock;
	pthread_mutex_t				token_lock;
	pthread_mutex_t				meal_lock;
	pthread_mutex_t				print_lock;
	pthread_mutex_t				dead_lock;
	int							index;
	int							dead;
	int							*fork_status;
	int							num_phil;
	int							num_orders;
	int							time_start;
	int							time_die;
	int							time_eat;
	int							time_sleep;
	int							meals;
	int							*done;
};

int		init_philos(struct s_shared_info *dinner_table);
int		init_mutex(struct s_shared_info *dinner_table);
int		create_threads(struct s_shared_info *dinner_table);
void	*routine(void *data);
void	do_life(struct s_shared_info *dinner_table, int left, \
				int right, int id);
void	*check_dead(void *data);
void	done_eating(struct s_shared_info *dinner_table, int id);
int		is_done(struct s_shared_info *dinner_table, int i);
int		is_starving(struct s_shared_info *dinner_table, int i);
void	print_status(char *msg, int id, struct s_shared_info *dinner_table);
void	grab_chopstick(struct s_shared_info *dinner_table, int i, int id);
void	down_chopsticks(struct s_shared_info *dinner_table, int c1, int c2);
int		is_dead(struct s_shared_info *dinner_table);
int		current_time(void);
void	better_usleep(int t);

#endif