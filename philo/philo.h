/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 08:50:01 by inajah            #+#    #+#             */
/*   Updated: 2025/02/10 15:19:33 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H


# define USAGE_MESSAGE \
	"philos tdie teat tsleep [iters]\n" \
	"\tphilos : number of philosophers\n" \
	"\ttdie   : time to die in milliseconds\n" \
	"\tteat   : time to eat in milliseconds\n" \
	"\ttsleep : time to sleep in milliseconds\n" \
	"\titers  : number of times each philosopher must eat (optional)\n"

# include <stdio.h>
# include <stdbool.h>
# include <limits.h>

enum
{
	NB_PHILOS,
	TIME_TO_DIE,
	TIME_TO_EAT,
	TIME_TO_SLEEP,
	NB_ITERATIONS,
};


void	print_setting(int *setting, int size);
bool	is_valid_setting(int *setting, int size);
int		is_digit(char c);
int		usage(char *program);

#endif // PHILO_H
