/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_utils_bonus.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 17:46:07 by inajah            #+#    #+#             */
/*   Updated: 2025/02/20 15:16:10 by inajah           ###   ########.fr       */
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

void	remove_sem(int nb_philos)
{
	int	i;

	sem_unlink(SEM_FORKS);
	sem_unlink(SEM_DEATH);
	sem_unlink(SEM_DEATH_MESSAGE);
	sem_unlink(SEM_DONE);
	sem_unlink(SEM_PRINT);
	i = 0;
	while (i < nb_philos)
	{
		sem_unlink(get_state_name(i + 1));
		sem_unlink(get_end_name(i + 1));
		i++;
	}
}

void	simulation_set_end(t_simulation *sim, bool value)
{
	sem_wait(sim->end_lock);
	sim->end_flag = value;
	sem_post(sim->end_lock);
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
