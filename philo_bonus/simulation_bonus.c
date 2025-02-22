/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 17:44:05 by inajah            #+#    #+#             */
/*   Updated: 2025/02/22 08:28:46 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

static bool	is_simulation_locks_valid(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < GLOBAL_LOCKS_COUNT)
	{
		if (sim->global_locks[i].is_opened == false)
			return (false);
		i++;
	}
	i = 0;
	while (i < sim->setting[NB_PHILOS])
	{
		if (sim->local_state_locks[i].is_opened == false)
			return (false);
		if (sim->local_end_locks[i].is_opened == false)
			return (false);
		i++;
	}
	return (true);
}

static bool	simulation_local_locks_init(t_simulation *sim)
{
	t_lock_def	lock_def;
	int			nb_philos;
	int			i;

	nb_philos = sim->setting[NB_PHILOS];
	sim->local_state_locks = safe_malloc(nb_philos * sizeof(t_lock));
	if (sim->local_state_locks == NULL)
		return (false);
	sim->local_end_locks = safe_malloc(nb_philos * sizeof(t_lock));
	if (sim->local_end_locks == NULL)
		return (false);
	i = 0;
	while (i < sim->setting[NB_PHILOS])
	{
		lock_def = get_local_lock_def(STATE, i + 1);
		lock_init(&sim->local_state_locks[i], lock_def.name, lock_def.value);
		lock_def = get_local_lock_def(END, i + 1);
		lock_init(&sim->local_end_locks[i], lock_def.name, lock_def.value);
		i++;
	}
	return (true);
}

static bool	simulation_locks_init(t_simulation *sim)
{
	t_lock_def	lock_def;
	int			i;

	remove_sem(sim);
	sim->global_locks = safe_malloc(GLOBAL_LOCKS_COUNT * sizeof(t_lock));
	if (sim->global_locks == NULL)
		return (false);
	i = 0;
	while (i < GLOBAL_LOCKS_COUNT)
	{
		lock_def = get_global_lock_def(sim->setting, i);
		lock_init(sim->global_locks + i, lock_def.name, lock_def.value);
		i++;
	}
	if (!simulation_local_locks_init(sim))
		return (false);
	return (is_simulation_locks_valid(sim));
}

t_simulation	*simulation_init(long *setting)
{
	t_simulation	*sim;

	sim = safe_malloc(sizeof(t_simulation));
	if (sim == NULL)
		return (NULL);
	sim->setting = setting;
	if (!simulation_locks_init(sim))
	{
		printf("bad stuff happened!!\n");
		return (simulation_abort(sim));
	}
	return (sim);
}
