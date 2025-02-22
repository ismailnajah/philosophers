/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 12:14:08 by inajah            #+#    #+#             */
/*   Updated: 2025/02/22 08:45:01 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

bool	philo_died(t_philosopher *philo)
{
	long	time_to_die;
	bool	dead;

	time_to_die = philo->sim->setting[TIME_TO_DIE];
	pthread_mutex_lock(philo->state_lock);
	dead = (get_current_time_ms() - philo->eat_time > time_to_die);
	pthread_mutex_unlock(philo->state_lock);
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
	if (is_end_simulation(philo->sim) || philo_died(philo))
	{
		unlock_forks(philo);
		return (false);
	}
	pthread_mutex_lock(philo->state_lock);
	philo->eat_time = get_current_time_ms();
	pthread_mutex_unlock(philo->state_lock);
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
	t_simulation	*sim;
	long			time_to_die;
	long			time_till_death;
	long			time_to_eat;
	long			time_to_sleep;

	sim = philo->sim;
	if (is_end_simulation(philo->sim) || philo_died(philo))
		return (false);
	if (!print_message(philo, THINKING_MESSAGE, true))
		return (false);
	if (sim->setting[NB_PHILOS] % 2 == 0)
		return (true);
	time_to_eat = sim->setting[TIME_TO_EAT];
	time_to_sleep = sim->setting[TIME_TO_SLEEP];
	time_to_die = sim->setting[TIME_TO_DIE];
	if (time_to_sleep < time_to_eat)
		ft_sleep(philo, time_to_eat - time_to_sleep);
	time_till_death = get_current_time_ms() - philo->eat_time;
	if (time_till_death < time_to_die * 0.7)
		usleep(1000);
	return (true);
}
