/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 08:42:29 by inajah            #+#    #+#             */
/*   Updated: 2025/02/10 15:17:08 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

int parse_number(char *str)
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

int	*parse_args(int ac, char **av)
{
	static int	setting[5];
	int			i;

	if (ac < 5 || ac > 6)
		return (setting);
	i = 1;
	while (i < ac)
	{
		setting[i - 1] = parse_number(av[i]);
		i++;
	}
	return (setting);
}

int	main(int ac, char **av)
{
	int *setting;

	setting = parse_args(ac, av);
	if (!is_valid_setting(setting, ac - 1))
		return (usage(av[0]));
	print_setting(setting, ac - 1);
	return (0);
}
