/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 12:07:06 by inajah            #+#    #+#             */
/*   Updated: 2025/02/17 12:35:51 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

bool	print_message(t_philosopher *philo, char *msg, bool check_death)
{
	long	start;

	start = philo->start_time;
	pthread_mutex_lock(philo->sim->print_lock);
	if (check_death && is_end_simulation(philo->sim))
	{
		pthread_mutex_unlock(philo->sim->print_lock);
		return (false);
	}
	printf("%ld %d %s\n", get_current_time_ms() - start, philo->id, msg);
	pthread_mutex_unlock(philo->sim->print_lock);
	return (true);
}

bool	is_end_simulation(t_simulation *sim)
{
	bool	end_flag;

	pthread_mutex_lock(sim->end_lock);
	end_flag = sim->end_flag;
	pthread_mutex_unlock(sim->end_lock);
	return (end_flag);
}

void	mutex_list_destroy(pthread_mutex_t *mutexes, int size)
{
	int	i;

	if (mutexes == NULL)
		return ;
	i = 0;
	while (i < size)
	{
		pthread_mutex_destroy(mutexes + i);
		i++;
	}
}

bool	mutex_list_init(pthread_mutex_t *mutexes, int size)
{
	int	i;

	if (mutexes == NULL)
		return (false);
	i = 0;
	while (i < size)
	{
		if (pthread_mutex_init(mutexes + i, NULL) != 0)
		{
			mutex_list_destroy(mutexes, i);
			return (false);
		}
		i++;
	}
	return (true);
}
