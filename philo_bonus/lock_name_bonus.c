/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lock_name_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 15:10:53 by inajah            #+#    #+#             */
/*   Updated: 2025/02/22 08:27:05 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

t_lock_def	get_global_lock_def(long *setting, int lock)
{
	t_lock_def			lock_def;
	static t_lock_def	g_locks_def[GLOBAL_LOCKS_COUNT] = {
	{"forks", 1},
	{"death", 0},
	{"death_message", 1},
	{"done", 0},
	{"print", 1},
	};

	lock_def.name = ft_strdup(g_locks_def[lock].name);
	lock_def.value = g_locks_def[lock].value;
	if (lock == FORKS)
		lock_def.value = setting[NB_PHILOS];
	return (lock_def);
}

t_lock_def	get_local_lock_def(int lock, int id)
{
	t_lock_def	lock_def;
	char		*name;
	char		*str_id;
	static int	name_size = 256;
	static char	*prefix[LOCAL_LOCKS_COUNT] = {
		"state",
		"end",
	};

	lock_def.name = NULL;
	lock_def.value = 0;
	name = safe_malloc(name_size * sizeof(char));
	if (name == NULL)
		return (lock_def);
	ft_strlcat(name, prefix[lock], name_size);
	str_id = ft_itoa(id);
	ft_strlcat(name, str_id, name_size);
	free(str_id);
	lock_def.name = name;
	lock_def.value = 1;
	return (lock_def);
}
