/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 17:44:05 by inajah            #+#    #+#             */
/*   Updated: 2025/02/20 15:16:58 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

static void	*simulation_free(t_simulation *sim)
{
	if (sim == NULL)
		return (NULL);
	free(sim->setting);
	free(sim);
	return (NULL);
}

void	*simulation_abort(t_simulation *sim)
{
	if (sim == NULL)
		return (NULL);
	if (sim->init_flag & INIT_FORKS)
		sem_close(sim->forks);
	if (sim->init_flag & INIT_STATE_LOCK)
		sem_close(sim->state_lock);
	if (sim->init_flag & INIT_DEATH_LOCK)
		sem_close(sim->death_lock);
	if (sim->init_flag & INIT_DEATH_MESSAGE_LOCK)
		sem_close(sim->death_message_lock);
	if (sim->init_flag & INIT_DONE_LOCK)
		sem_close(sim->done_lock);
	if (sim->init_flag & INIT_END_LOCK)
		sem_close(sim->end_lock);
	if (sim->init_flag & INIT_PRINT_LOCK)
		sem_close(sim->print_lock);
	return (simulation_free(sim));
}

static bool	is_simulation_locks_valid(t_simulation *sim)
{
	sim->init_flag |= (sim->forks != SEM_FAILED);
	sim->init_flag |= (sim->death_lock != SEM_FAILED) << 1;
	sim->init_flag |= (sim->death_message_lock != SEM_FAILED) << 2;
	sim->init_flag |= (sim->done_lock != SEM_FAILED) << 3;
	sim->init_flag |= (sim->print_lock != SEM_FAILED) << 4;
	return (sim->init_flag
		& (INIT_FORKS
			| INIT_DEATH_LOCK
			| INIT_DEATH_MESSAGE_LOCK
			| INIT_DONE_LOCK
			| INIT_PRINT_LOCK));
}

static bool	simulation_locks_init(t_simulation *sim)
{
	remove_sem(sim->setting[NB_PHILOS]);
	sim->forks = sem_open(SEM_FORKS, O_CREAT, O_RDWR, sim->setting[NB_PHILOS]);
	sim->death_lock = sem_open(SEM_DEATH, O_CREAT, O_RDWR, 0);
	sim->death_message_lock = sem_open(SEM_DEATH_MESSAGE, O_CREAT, O_RDWR, 1);
	sim->done_lock = sem_open(SEM_DONE, O_CREAT, O_RDWR, 0);
	sim->print_lock = sem_open(SEM_PRINT, O_CREAT, O_RDWR, 1);
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
