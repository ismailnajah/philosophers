/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 15:16:09 by inajah            #+#    #+#             */
/*   Updated: 2025/02/10 15:17:26 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

void	print_setting(int *setting, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		printf("arg %d : %d\n", i, setting[i]);
		i++;
	}
}

bool	is_valid_setting(int *setting, int size)
{
	int i;

	i = 0;
	while (i < size)
	{
		if (setting[i] < 0)
			return (false);
		i++;
	}
	return (true);
}

int	is_digit(char c)
{
	return ('0' <= c && c <= '9');
}

int	usage(char *program)
{
	printf("Usage: %s "USAGE_MESSAGE, program);
	return (1);
}
