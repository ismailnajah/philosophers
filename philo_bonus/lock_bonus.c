/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lock_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 17:47:37 by inajah            #+#    #+#             */
/*   Updated: 2025/02/21 21:16:40 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

bool	lock_single_fork(t_philosopher *philo)
{
	if (is_end_simulation(philo->sim) || philo_died(philo))
		return (false);
	sem_wait(philo->sim->global_locks[FORKS].sem);
	if (is_end_simulation(philo->sim) || philo_died(philo))
	{
		sem_post(philo->sim->global_locks[FORKS].sem);
		return (false);
	}
	if (!print_message(philo, FORK_MESSAGE, true))
	{
		sem_post(philo->sim->global_locks[FORKS].sem);
		return (false);
	}
	return (true);
}

bool	lock_forks(t_philosopher *philo)
{
	if (!lock_single_fork(philo))
		return (false);
	if (philo->sim->setting[NB_PHILOS] == 1)
	{
		ft_sleep(philo, philo->sim->setting[TIME_TO_DIE]);
		return (false);
	}
	if (!lock_single_fork(philo))
	{
		sem_post(philo->sim->global_locks[FORKS].sem);
		return (false);
	}
	return (true);
}

void	unlock_forks(t_philosopher *philo)
{
	sem_post(philo->sim->global_locks[FORKS].sem);
	sem_post(philo->sim->global_locks[FORKS].sem);
}
