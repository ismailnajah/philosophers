/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_actions_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 17:48:38 by inajah            #+#    #+#             */
/*   Updated: 2025/02/20 13:54:46 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

bool	philo_eat(t_philosopher *philo)
{
	t_simulation	*sim;

	sim = philo->sim;
	if (!lock_forks(philo))
		return (false);
	if (is_end_simulation(philo->sim) || philo_died(philo))
	{
		sem_post(sim->forks);
		return (false);
	}
	sem_wait(philo->state_lock);
	philo->eat_time = get_current_time_ms();
	sem_post(philo->state_lock);
	philo->meal_count++;
	if (!print_message(philo, EATING_MESSAGE, true))
		return (false);
	ft_sleep(philo, sim->setting[TIME_TO_EAT]);
	unlock_forks(philo);
	return (true);
}

bool	philo_sleep(t_philosopher *philo)
{
	t_simulation	*sim;

	sim = philo->sim;
	if (!print_message(philo, SLEEPING_MESSAGE, true))
		return (false);
	ft_sleep(philo, sim->setting[TIME_TO_SLEEP]);
	return (true);
}

bool	philo_think(t_philosopher *philo)
{
	t_simulation	*sim;
	long			time_to_die;
	long			time_till_death;

	sim = philo->sim;
	if (!print_message(philo, THINKING_MESSAGE, true))
		return (false);
	if (sim->setting[NB_PHILOS] % 2 == 0)
		return (true);
	time_to_die = sim->setting[TIME_TO_DIE];
	time_till_death = get_current_time_ms() - philo->eat_time;
	if (time_till_death < time_to_die * 0.7)
		usleep(1000);
	return (true);
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
		sem_post(philo->sim->done_lock);
	return (done);
}
