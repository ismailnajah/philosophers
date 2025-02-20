/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitors_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 17:50:38 by inajah            #+#    #+#             */
/*   Updated: 2025/02/20 15:05:39 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

void	*listener_thread(void *philo_ptr)
{
	t_philosopher	*philo;
	t_simulation	*sim;

	philo = philo_ptr;
	sim = philo->sim;
	sem_wait(sim->death_lock);
	sem_post(sim->death_lock);
	sem_post(sim->done_lock);
	simulation_set_end(sim, true);
	return (NULL);
}

void	*death_monitor_thread(void *philo_ptr)
{
	t_philosopher	*philo;

	philo = philo_ptr;
	while (get_current_time_ms() < philo->sim->setting[START_TIME])
		usleep(250);
	usleep(philo->sim->setting[TIME_TO_DIE] * 1000);
	while (true)
	{
		if (is_end_simulation(philo->sim))
			break ;
		if (philo_died(philo))
		{
			sem_post(philo->sim->death_lock);
			sem_wait(philo->sim->death_message_lock);
			if (is_end_simulation(philo->sim))
				break ;
			usleep(3000);
			print_message(philo, DIED_MESSAGE, false);
			simulation_set_end(philo->sim, true);
			return (NULL);
		}
		usleep(500);
	}
	return (NULL);
}

pthread_t	start_thread(t_philosopher *philo, void *(*routine)(void *))
{
	pthread_t	tid;

	if (pthread_create(&tid, NULL, routine, philo) != 0)
	{
		printf("[ERROR] "ERR_THREAD);
		sem_post(philo->sim->death_lock);
		simulation_abort(philo->sim);
		exit(1);
	}
	return (tid);
}
