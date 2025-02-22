/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 12:11:44 by inajah            #+#    #+#             */
/*   Updated: 2025/02/22 08:39:00 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

void	philo_set_state(t_philosopher *philo, t_state state)
{
	pthread_mutex_lock(philo->state_lock);
	philo->state = state;
	pthread_mutex_unlock(philo->state_lock);
}

t_state	philo_get_state(t_philosopher *philo)
{
	t_state	state;

	pthread_mutex_lock(philo->state_lock);
	state = philo->state;
	pthread_mutex_unlock(philo->state_lock);
	return (state);
}

void	philosopher_init(t_simulation *sim, int index)
{
	t_philosopher	*philo;
	int				nb_philos;

	nb_philos = sim->setting[NB_PHILOS];
	philo = &sim->philos[index];
	philo->id = index + 1;
	philo->right_fork = &sim->forks[index];
	philo->left_fork = &sim->forks[(index + 1) % nb_philos];
	philo->state_lock = &sim->state_locks[index];
	philo->sim = sim;
}

void	*philo_thread(void *philo_ptr)
{
	t_philosopher	*philo;

	philo = philo_ptr;
	while (get_current_time_ms() < philo->sim->setting[START_TIME])
		usleep(250);
	philo->start_time = get_current_time_ms();
	pthread_mutex_lock(philo->state_lock);
	philo->eat_time = philo->start_time;
	pthread_mutex_unlock(philo->state_lock);
	if (philo->id % 2 != 0)
		usleep(40 * 1000);
	while (true)
	{
		if (!philo_eat(philo))
			break ;
		if (philo_done(philo))
			break ;
		if (!philo_sleep(philo))
			break ;
		if (!philo_think(philo))
			break ;
	}
	return (NULL);
}
