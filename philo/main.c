/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 08:42:29 by inajah            #+#    #+#             */
/*   Updated: 2025/02/12 17:10:32 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

typedef struct s_simulation
{
	int				*setting;
	pthread_mutex_t	*forks;
	pthread_mutex_t *print_lock;
	int				philo_id;
}	t_simulation;

bool	simulation_mutex_init(pthread_mutex_t *mutexes, int size)
{
	if (mutexes == NULL)
		return (false);
	while (false && size > 0)
	{
		//TODO: if init fails destroy the previous mutexes;
		if (pthread_mutex_init(mutexes + size - 1, NULL) != 0)
			return (false);
		size--;
	}
	return (true);
}

void	simulation_mutex_destroy(pthread_mutex_t *mutexes, int size)
{
	if (mutexes == NULL)
		return ;
	while (size > 0)
	{
		pthread_mutex_destroy(mutexes + size - 1);
		size--;
	}
}

void	*simulation_abort(t_simulation *sim)
{
	if (sim == NULL)
		return (NULL);
	simulation_mutex_destroy(sim->forks, sim->setting[NB_PHILOS]);
	simulation_mutex_destroy(sim->print_lock, 1);
	free(sim->forks);
	free(sim->print_lock);
	free(sim->setting);
	free(sim);
	return (NULL);
}

t_simulation	*simulation_init(int *setting)
{
	t_simulation	*sim;
	bool			abort;

	sim = safe_malloc(sizeof(t_simulation));
	if (sim == NULL)
		return (NULL);
	sim->setting = setting;
	sim->forks = safe_malloc(setting[NB_PHILOS] * sizeof(pthread_mutex_t));
	if (sim->forks == NULL)
		return (simulation_abort(sim));
	abort = !simulation_mutex_init(sim->forks, setting[NB_PHILOS]);
	sim->print_lock = safe_malloc(sizeof(pthread_mutex_t));
	if (sim->print_lock == NULL)
		return (simulation_abort(sim));
	abort |= !simulation_mutex_init(sim->print_lock, 1);
	if (abort)
		return (simulation_abort(sim), NULL);
	return (sim);
}

int	main(int ac, char **av)
{
	int *setting;
	t_simulation *sim;

	setting = parse_args(ac, av);
	if (setting == NULL)
		return (1);
	if (!is_valid_setting(setting, ac - 1))
	{
		free(setting);
		return (usage(av[0]));
	}
	print_setting(setting, ac - 1);
	sim = simulation_init(setting);
	simulation_abort(sim);
	return (0);
}
