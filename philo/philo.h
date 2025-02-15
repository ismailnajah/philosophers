/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 08:50:01 by inajah            #+#    #+#             */
/*   Updated: 2025/02/15 14:59:29 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <stdbool.h>
# include <limits.h>
# include <pthread.h>
# include <sys/time.h>

# define USAGE_MESSAGE \
	"philos tdie teat tsleep [iters]\n" \
	"\tphilos : number of philosophers\n" \
	"\ttdie   : time to die in milliseconds\n" \
	"\tteat   : time to eat in milliseconds\n" \
	"\ttsleep : time to sleep in milliseconds\n" \
	"\titers  : number of times each philosopher must eat (optional)\n"

enum
{
	NB_PHILOS,
	TIME_TO_DIE,
	TIME_TO_EAT,
	TIME_TO_SLEEP,
	NB_ITERATIONS,
	START_TIME,
	SETTING_COUNT,
};

//parse.c
long	*parse_args(int ac, char **av);

//utils.c
void	*safe_malloc(size_t size);
bool	is_valid_setting(long *setting, int size);
int		is_digit(char c);
void	print_setting(long *setting);
int		usage(char *program);

#endif // PHILO_H
