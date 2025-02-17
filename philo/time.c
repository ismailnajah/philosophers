/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 12:01:24 by inajah            #+#    #+#             */
/*   Updated: 2025/02/17 12:02:58 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

time_t	get_current_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (-1);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	ft_sleep(t_philosopher *philo, time_t duration_ms)
{
	time_t	start;

	start = get_current_time_ms();
	usleep((duration_ms * 1000) * 0.7);
	while (true)
	{
		if (get_current_time_ms() - start >= duration_ms
			|| is_end_simulation(philo->sim))
			break ;
		usleep(500);
	}
}
