/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 12:01:24 by inajah            #+#    #+#             */
/*   Updated: 2025/02/20 15:02:13 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

void	ft_sleep(t_philosopher *philo, time_t duration_ms)
{
	time_t	start;

	(void) philo;
	start = get_current_time_ms();
	usleep((duration_ms * 1000) * 0.7);
	while (true)
	{
		if (get_current_time_ms() - start >= duration_ms)
			break ;
		usleep(500);
	}
}
