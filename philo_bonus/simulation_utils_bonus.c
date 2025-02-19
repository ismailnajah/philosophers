/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_utils_bonus.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 17:46:07 by inajah            #+#    #+#             */
/*   Updated: 2025/02/19 18:09:26 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

pthread_mutex_t	*simulation_mutex_init(void)
{
	pthread_mutex_t	*mutex;

	mutex = safe_malloc(sizeof(pthread_mutex_t));
	if (mutex == NULL)
		return (NULL);
	if (pthread_mutex_init(mutex, NULL) != 0)
	{
		free(mutex);
		return (NULL);
	}
	return (mutex);
}

void	remove_sem(void)
{
	sem_unlink(SEM_FORKS);
	sem_unlink(SEM_DEATH);
	sem_unlink(SEM_DEATH_MESSAGE);
	sem_unlink(SEM_DONE);
	sem_unlink(SEM_PRINT);
}

void	simulation_set_end(t_simulation *sim, bool value)
{
	pthread_mutex_lock(sim->end_lock);
	sim->end_flag = value;
	pthread_mutex_unlock(sim->end_lock);
}

bool	is_end_simulation(t_simulation *sim)
{
	bool	end_flag;

	pthread_mutex_lock(sim->end_lock);
	end_flag = sim->end_flag;
	pthread_mutex_unlock(sim->end_lock);
	return (end_flag);
}

bool	print_message(t_philosopher *philo, char *msg, bool check_death)
{
	long	start;

	start = philo->start_time;
	sem_wait(philo->sim->print_lock);
	if (check_death && is_end_simulation(philo->sim))
	{
		sem_post(philo->sim->print_lock);
		return (false);
	}
	printf("%ld %d %s\n", get_current_time_ms() - start, philo->id, msg);
	sem_post(philo->sim->print_lock);
	return (true);
}
