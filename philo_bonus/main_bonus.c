/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 15:36:35 by inajah            #+#    #+#             */
/*   Updated: 2025/02/20 21:44:17 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

void	broadcast_sem(sem_t *sem, int nb)
{
	int	i;

	i = 0;
	while (i < nb)
	{
		sem_post(sem);
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
		sem_wait(sim->done_lock);
		i++;
	}
	broadcast_sem(sim->death_lock, nb_philos);
	broadcast_sem(sim->death_message_lock, nb_philos);
	while (i > 0)
	{
		wait(NULL);
		i--;
	}
}

void	init_error(t_simulation *sim)
{
	sem_post(sim->death_lock);
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
		//TODO: create array of semaphors for state_lock and end_lock;
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
	simulation_abort(sim);
	remove_sem(nb_philos);
	return (0);
}
