/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 08:42:29 by inajah            #+#    #+#             */
/*   Updated: 2025/02/17 12:19:44 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

int	simulation_philos_start(t_simulation *sim)
{
	t_philosopher	*philo;
	int				i;

	i = 0;
	while (i < sim->setting[NB_PHILOS])
	{
		philo = sim->philos + i;
		if (pthread_create(&philo->tid, NULL, philo_thread, philo) != 0)
		{
			printf("[ERROR] Philo %d "ERR_THREAD, i + 1);
			philo_set_state(philo, STATE_DEAD);
			return (i);
		}
		i++;
	}
	return (i);
}

void	pthread_join_philos(t_simulation *sim, int nb_philos)
{
	int	i;

	i = 0;
	while (i < nb_philos)
	{
		pthread_join(sim->philos[i].tid, NULL);
		i++;
	}
}

void	simulation_start(t_simulation *sim)
{
	pthread_t	monitor_tid;
	long		*setting;
	int			nb_started_philos;

	setting = sim->setting;
	setting[START_TIME] = get_current_time_ms() + setting[NB_PHILOS] * 10;
	if (pthread_create(&monitor_tid, NULL, monitor_thread, sim) != 0)
	{
		printf("[ERROR] Monitor "ERR_THREAD);
		return ;
	}
	nb_started_philos = simulation_philos_start(sim);
	pthread_join(monitor_tid, NULL);
	pthread_join_philos(sim, nb_started_philos);
}

int	main(int ac, char **av)
{
	long			*setting;
	t_simulation	*sim;

	setting = parse_args(ac, av);
	if (setting == NULL)
		return (1);
	if (!is_valid_setting(setting, ac - 1))
	{
		free(setting);
		return (usage(av[0]));
	}
	print_setting(setting);
	sim = simulation_init(setting);
	if (setting[NB_ITERATIONS] != 0)
		simulation_start(sim);
	simulation_abort(sim);
	return (0);
}
