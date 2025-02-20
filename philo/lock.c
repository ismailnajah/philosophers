/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lock.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 12:17:28 by inajah            #+#    #+#             */
/*   Updated: 2025/02/19 19:31:27 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

static bool	lock_single_fork(t_philosopher *philo, pthread_mutex_t *fork)
{
	if (is_end_simulation(philo->sim))
		return (false);
	pthread_mutex_lock(fork);
	if (is_end_simulation(philo->sim))
	{
		pthread_mutex_unlock(fork);
		return (false);
	}
	print_message(philo, FORK_MESSAGE, true);
	return (true);
}

bool	lock_forks(t_philosopher *philo)
{
	pthread_mutex_t	*first_fork;
	pthread_mutex_t	*second_fork;

	first_fork = philo->left_fork;
	second_fork = philo->right_fork;
	if (philo->id % 2 != 0)
	{
		first_fork = philo->right_fork;
		second_fork = philo->left_fork;
	}
	if (!lock_single_fork(philo, first_fork))
		return (false);
	if (philo->sim->setting[NB_PHILOS] == 1)
	{
		ft_sleep(philo, philo->sim->setting[TIME_TO_DIE]);
		return (false);
	}
	if (!lock_single_fork(philo, second_fork))
		return (false);
	return (true);
}

void	unlock_forks(t_philosopher *philo)
{
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}
