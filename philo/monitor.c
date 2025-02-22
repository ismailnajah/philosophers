/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 12:20:00 by inajah            #+#    #+#             */
/*   Updated: 2025/02/22 08:45:04 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

static bool	philos_running(t_simulation *sim)
{
	int	i;
	int	philos_done;

	philos_done = 0;
	i = 0;
	while (i < sim->setting[NB_PHILOS])
	{
		if (philo_get_state(&sim->philos[i]) == STATE_DONE)
			philos_done++;
		else if (philo_died(&sim->philos[i]))
		{
			pthread_mutex_lock(sim->end_lock);
			sim->end_flag = true;
			pthread_mutex_unlock(sim->end_lock);
			print_message(&sim->philos[i], DIED_MESSAGE, false);
			return (false);
		}
		i++;
	}
	return (philos_done != sim->setting[NB_PHILOS]);
}

void	*monitor_thread(void *sim_ptr)
{
	t_simulation	*sim;

	sim = sim_ptr;
	while (get_current_time_ms() < sim->setting[START_TIME])
		usleep(250);
	usleep(sim->setting[TIME_TO_DIE] * 1000);
	while (philos_running(sim))
		usleep(500);
	return (NULL);
}
