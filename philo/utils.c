/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 15:16:09 by inajah            #+#    #+#             */
/*   Updated: 2025/02/12 16:44:01 by inajah           ###   ########.fr       */
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

