/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 15:23:05 by inajah            #+#    #+#             */
/*   Updated: 2025/02/17 12:32:01 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

int	parse_number(char *str)
{
	long	result;
	int		i;

	result = 0;
	i = 0;
	while (str[i] && is_digit(str[i]))
	{
		result = result * 10 + str[i] - '0';
		if (result > INT_MAX)
			return (-1);
		i++;
	}
	if (str[i] != '\0')
		return (-1);
	return (result);
}

long	*parse_args(int ac, char **av)
{
	long	*setting;
	int		i;

	if (ac < 5 || ac > 6)
		return (usage(av[0]), NULL);
	setting = safe_malloc(SETTING_COUNT * sizeof(long));
	if (setting == NULL)
		return (NULL);
	memset(setting, 0, SETTING_COUNT * sizeof(long));
	i = 1;
	while (i < ac)
	{
		setting[i - 1] = parse_number(av[i]);
		i++;
	}
	if (ac == 5)
		setting[NB_ITERATIONS] = -1;
	return (setting);
}
