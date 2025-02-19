/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lock_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 17:47:37 by inajah            #+#    #+#             */
/*   Updated: 2025/02/19 18:51:05 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

bool	lock_single_fork(t_philosopher *philo)
{
	if (is_end_simulation(philo->sim))
		return (false);
	sem_wait(philo->sim->forks);
	if (is_end_simulation(philo->sim))
	{
		sem_post(philo->sim->forks);
		return (false);
	}
	if (!print_message(philo, FORK_MESSAGE, true))
	{
		sem_post(philo->sim->forks);
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
		return (false);
	return (true);
}

void	unlock_forks(t_philosopher *philo)
{
	sem_post(philo->sim->forks);
	sem_post(philo->sim->forks);
}
