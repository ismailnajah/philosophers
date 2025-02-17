/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 15:16:09 by inajah            #+#    #+#             */
/*   Updated: 2025/02/17 12:34:12 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

void	*safe_malloc(size_t size)
{
	void	*ptr;

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
	printf("Usage: %s philos tdie teat tsleep [iters]\n" \
	"\tphilos : number of philosophers\n" \
	"\ttdie   : time to die in milliseconds\n" \
	"\tteat   : time to eat in milliseconds\n" \
	"\ttsleep : time to sleep in milliseconds\n" \
	"\titers  : number of times each philosopher must eat (optional)\n"\
	, program);
	return (1);
}

void	print_setting(long *setting)
{
	printf("number of philos : %ld\n", setting[NB_PHILOS]);
	printf("time to die      : %ld\n", setting[TIME_TO_DIE]);
	printf("time to eat      : %ld\n", setting[TIME_TO_EAT]);
	printf("time to sleep    : %ld\n", setting[TIME_TO_SLEEP]);
	printf("meals (optional) : %ld\n", setting[NB_ITERATIONS]);
}

bool	is_valid_setting(long *setting, int size)
{
	int	i;

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
