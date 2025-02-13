/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 15:16:09 by inajah            #+#    #+#             */
/*   Updated: 2025/02/13 11:49:53 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

void	*safe_malloc(size_t size)
{
	void *ptr;

	ptr = malloc(size);
	if (ptr == NULL)
	{
		printf("allocation faild, BUY MORE RAM!!\n");
		return (NULL);
	}
	memset(ptr, 0, size);
	return (ptr);
}

int	usage(char *program)
{
	printf("Usage: %s "USAGE_MESSAGE, program);
	return (1);
}

void	print_setting(int *setting)
{
	printf("number of philos : %d\n", setting[NB_PHILOS]);
	printf("time to die      : %d\n", setting[TIME_TO_DIE]);
	printf("time to eat      : %d\n", setting[TIME_TO_EAT]);
	printf("time to sleep    : %d\n", setting[TIME_TO_SLEEP]);
	printf("meals (optional) : %d\n", setting[NB_ITERATIONS]);
}

bool	is_valid_setting(int *setting, int size)
{
	int i;

	if (setting == NULL)
		return (false);
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

