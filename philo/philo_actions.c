/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 12:14:08 by inajah            #+#    #+#             */
/*   Updated: 2025/02/17 16:19:45 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

bool	philo_died(t_philosopher *philo)
{
	long	time_to_die;
	bool	dead;

	time_to_die = philo->sim->setting[TIME_TO_DIE];
	dead = (get_current_time_ms() - philo->eat_time > time_to_die);
	if (dead)
		philo_set_state(philo, STATE_DEAD);
	return (dead);
}

bool	philo_done(t_philosopher *philo)
{
	long	nb_iterations;
	bool	done;

	nb_iterations = philo->sim->setting[NB_ITERATIONS];
	if (nb_iterations < 0)
		return (false);
	done = (philo->meal_count == nb_iterations);
	if (done)
		philo_set_state(philo, STATE_DONE);
	return (done);
}

bool	philo_eat(t_philosopher *philo)
{
	long	*setting;

	if (!lock_forks(philo))
		return (false);
	setting = philo->sim->setting;
	if (philo_died(philo))
	{
		unlock_forks(philo);
		return (false);
	}
	philo->eat_time = get_current_time_ms();
	philo->meal_count++;
	print_message(philo, EATING_MESSAGE, true);
	ft_sleep(philo, setting[TIME_TO_EAT]);
	unlock_forks(philo);
	return (true);
}

bool	philo_sleep(t_philosopher *philo)
{
	if (!print_message(philo, SLEEPING_MESSAGE, true))
		return (false);
	ft_sleep(philo, philo->sim->setting[TIME_TO_SLEEP]);
	return (true);
}

bool	philo_think(t_philosopher *philo)
{
	long	time_till_death;
	long	time_to_die;

	if (!print_message(philo, THINKING_MESSAGE, true))
		return (false);
	if (philo->sim->setting[NB_PHILOS] % 2 == 0)
		return (true);
	time_to_die = philo->sim->setting[TIME_TO_DIE];
	time_till_death = get_current_time_ms() - philo->eat_time;
	if (time_till_death < time_to_die * 0.7)
		usleep(1000);
	return (true);
}
