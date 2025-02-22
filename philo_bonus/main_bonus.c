/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 15:36:35 by inajah            #+#    #+#             */
/*   Updated: 2025/02/22 08:14:40 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

void	broadcast_sem(t_lock *lock, int nb)
{
	int	i;

	i = 0;
	while (i < nb)
	{
		sem_post(lock->sem);
		i++;
	}
}

void	simulation_wait(t_simulation *sim)
{
	long	nb_philos;
	int		i;

	nb_philos = sim->setting[NB_PHILOS];
	i = 0;
	while (i < nb_philos)
	{
		sem_wait(sim->global_locks[DONE].sem);
		i++;
	}
	broadcast_sem(&sim->global_locks[DEATH], nb_philos);
	broadcast_sem(&sim->global_locks[DEATH_MESSAGE], nb_philos);
	while (i > 0)
	{
		wait(NULL);
		i--;
	}
}

void	init_error(t_simulation *sim)
{
	sem_post(sim->global_locks[DEATH].sem);
}

void	simulation_start(t_simulation *sim)
{
	t_philosopher	philo;
	int				nb_philos;
	pid_t			pid;
	int				i;

	nb_philos = sim->setting[NB_PHILOS];
	sim->setting[START_TIME] = get_current_time_ms() + nb_philos * 10;
	i = 0;
	while (i < sim->setting[NB_PHILOS])
	{
		sim->philo_index = i;
		if (!philo_init(&philo, sim, i + 1))
			return (init_error(sim));
		pid = fork();
		if (pid < 0)
			return (init_error(sim));
		else if (pid == 0)
			philo_process(&philo);
		i++;
	}
	simulation_wait(sim);
}

int	main(int ac, char **av)
{
	long			*setting;
	long			nb_philos;
	t_simulation	*sim;

	setting = parse_args(ac, av);
	if (setting == NULL)
		return (1);
	if (!is_valid_setting(setting, ac - 1))
	{
		free(setting);
		return (usage(av[0]));
	}
	nb_philos = setting[NB_PHILOS];
	sim = simulation_init(setting);
	if (setting[NB_ITERATIONS] != 0)
		simulation_start(sim);
	remove_sem(sim);
	simulation_abort(sim);
	return (0);
}
