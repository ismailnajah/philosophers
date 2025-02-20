/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 17:54:14 by inajah            #+#    #+#             */
/*   Updated: 2025/02/20 08:42:16 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

void	philo_init(t_philosopher *philo, t_simulation *sim, int id)
{
	memset(philo, 0, sizeof(t_philosopher));
	philo->id = id;
	philo->state_lock = sim->state_lock;
	philo->sim = sim;
}

static void	philo_start_time(t_philosopher *philo)
{
	while (get_current_time_ms() < philo->sim->setting[START_TIME])
		usleep(250);
	philo->start_time = get_current_time_ms();
	pthread_mutex_lock(philo->state_lock);
	philo->eat_time = philo->start_time;
	pthread_mutex_unlock(philo->state_lock);
	if (philo->id % 2 != 0)
		usleep(1000);
}

static bool	philo_alive(t_philosopher *philo)
{
	return (philo_eat(philo)
		&& !philo_done(philo)
		&& philo_sleep(philo)
		&& philo_think(philo));
}

void	philo_process(t_philosopher *philo)
{
	pthread_t	listener_tid;
	pthread_t	death_monitor_tid;

	listener_tid = start_thread(philo, listener_thread);
	death_monitor_tid = start_thread(philo, death_monitor_thread);
	philo_start_time(philo);
	while (true)
	{
		if (!philo_alive(philo))
			break ;
	}
	pthread_join(listener_tid, NULL);
	pthread_join(death_monitor_tid, NULL);
	simulation_abort(philo->sim);
	exit(0);
}
