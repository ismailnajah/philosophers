/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_utils_bonus.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 17:46:07 by inajah            #+#    #+#             */
/*   Updated: 2025/02/22 08:28:26 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

void	remove_sem(t_simulation *sim)
{
	t_lock_def	lock_def;
	int			i;

	i = 0;
	while (i < GLOBAL_LOCKS_COUNT)
	{
		lock_def = get_global_lock_def(sim->setting, i);
		sem_unlink(lock_def.name);
		free(lock_def.name);
		i++;
	}
	i = 0;
	while (i < sim->setting[NB_PHILOS])
	{
		lock_def = get_local_lock_def(STATE, i + 1);
		sem_unlink(lock_def.name);
		free(lock_def.name);
		lock_def = get_local_lock_def(END, i + 1);
		sem_unlink(lock_def.name);
		free(lock_def.name);
		i++;
	}
}

static void	*simulation_free(t_simulation *sim)
{
	if (sim == NULL)
		return (NULL);
	free(sim->setting);
	free(sim->global_locks);
	free(sim->local_state_locks);
	free(sim->local_end_locks);
	free(sim);
	return (NULL);
}

void	*simulation_abort(t_simulation *sim)
{
	int	i;

	if (sim == NULL)
		return (NULL);
	i = 0;
	while (i < GLOBAL_LOCKS_COUNT)
	{
		lock_destroy(&sim->global_locks[i]);
		i++;
	}
	i = 0;
	while (i < sim->setting[NB_PHILOS])
	{
		lock_destroy(&sim->local_state_locks[i]);
		lock_destroy(&sim->local_end_locks[i]);
		i++;
	}
	return (simulation_free(sim));
}

void	simulation_set_end(t_simulation *sim, bool value)
{
	int	index;

	index = sim->philo_index;
	sem_wait(sim->local_end_locks[index].sem);
	sim->end_flag = value;
	sem_post(sim->local_end_locks[index].sem);
}

bool	print_message(t_philosopher *philo, char *msg, bool check_death)
{
	long	start;

	start = philo->start_time;
	sem_wait(philo->sim->global_locks[PRINT].sem);
	if (check_death && is_end_simulation(philo->sim))
	{
		sem_post(philo->sim->global_locks[PRINT].sem);
		return (false);
	}
	printf("%ld %d %s\n", get_current_time_ms() - start, philo->id, msg);
	sem_post(philo->sim->global_locks[PRINT].sem);
	return (true);
}
