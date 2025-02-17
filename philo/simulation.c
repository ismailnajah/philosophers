/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 12:03:53 by inajah            #+#    #+#             */
/*   Updated: 2025/02/17 12:40:04 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

static bool	simulation_philos_init(t_simulation *sim)
{
	int				i;
	int				nb_philos;

	nb_philos = sim->setting[NB_PHILOS];
	sim->philos = safe_malloc(nb_philos * sizeof(t_philosopher));
	if (sim->philos == NULL)
		return (false);
	i = 0;
	while (i < nb_philos)
	{
		philosopher_init(sim, i);
		i++;
	}
	return (true);
}

static bool	simulation_mutexes_init(t_simulation *sim)
{
	int	nb_philos;

	nb_philos = sim->setting[NB_PHILOS];
	sim->forks = safe_malloc(nb_philos * sizeof(pthread_mutex_t));
	if (sim->forks == NULL)
		return (false);
	sim->state_locks = safe_malloc(nb_philos * sizeof(pthread_mutex_t));
	if (sim->state_locks == NULL)
		return (false);
	sim->end_lock = safe_malloc(sizeof(pthread_mutex_t));
	if (sim->end_lock == NULL)
		return (false);
	sim->print_lock = safe_malloc(sizeof(pthread_mutex_t));
	if (sim->print_lock == NULL)
		return (false);
	sim->init_flag |= mutex_list_init(sim->forks, nb_philos);
	sim->init_flag |= mutex_list_init(sim->state_locks, nb_philos) << 1;
	sim->init_flag |= mutex_list_init(sim->end_lock, 1) << 2;
	sim->init_flag |= mutex_list_init(sim->print_lock, 1) << 3;
	return (sim->init_flag
		& (INIT_FORKS | INIT_STATE_LOCKS | INIT_END_LOCK | INIT_PRINT_LOCK));
}

static void	*simulation_free(t_simulation *sim)
{
	if (sim == NULL)
		return (NULL);
	free(sim->forks);
	free(sim->state_locks);
	free(sim->end_lock);
	free(sim->print_lock);
	free(sim->setting);
	free(sim->philos);
	free(sim);
	return (NULL);
}

void	*simulation_abort(t_simulation *sim)
{
	if (sim == NULL)
		return (NULL);
	if (sim->init_flag & INIT_FORKS)
		mutex_list_destroy(sim->forks, sim->setting[NB_PHILOS]);
	if (sim->init_flag & INIT_STATE_LOCKS)
		mutex_list_destroy(sim->state_locks, sim->setting[NB_PHILOS]);
	if (sim->init_flag & INIT_END_LOCK)
		mutex_list_destroy(sim->end_lock, 1);
	if (sim->init_flag & INIT_PRINT_LOCK)
		mutex_list_destroy(sim->print_lock, 1);
	return (simulation_free(sim));
}

t_simulation	*simulation_init(long *setting)
{
	t_simulation	*sim;

	sim = safe_malloc(sizeof(t_simulation));
	if (sim == NULL)
		return (NULL);
	sim->setting = setting;
	if (!simulation_mutexes_init(sim))
		return (simulation_abort(sim));
	if (!simulation_philos_init(sim))
		return (simulation_abort(sim));
	return (sim);
}
