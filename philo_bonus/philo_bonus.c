/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 17:54:14 by inajah            #+#    #+#             */
/*   Updated: 2025/02/21 08:21:44 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

bool	philo_init(t_philosopher *philo, t_simulation *sim, int id)
{
	char	*sem_state_name;
	char	*sem_end_name;

	sem_state_name = get_state_name(id);
	sem_end_name = get_end_name(id);
	memset(philo, 0, sizeof(t_philosopher));
	philo->id = id;
	sim->state_lock = sem_open(sem_state_name, O_CREAT, O_RDWR, 1);
	sim->end_lock = sem_open(sem_end_name, O_CREAT, O_RDWR, 1);
	sim->init_flag |= (sim->state_lock != SEM_FAILED) << 5;
	sim->init_flag |= (sim->end_lock != SEM_FAILED) << 6;
	if ((sim->init_flag & (INIT_STATE_LOCK | INIT_END_LOCK)) == 0)
		return (false);
	philo->state_lock = sim->state_lock;
	philo->sim = sim;
	return (true);
}

static void	philo_start_time(t_philosopher *philo)
{
	while (get_current_time_ms() < philo->sim->setting[START_TIME])
		usleep(250);
	philo->start_time = get_current_time_ms();
	sem_wait(philo->state_lock);
	philo->eat_time = philo->start_time;
	sem_post(philo->state_lock);
	if (philo->id % 2 != 0)
		usleep(40 * 1000);
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
