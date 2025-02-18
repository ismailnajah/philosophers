/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 15:36:35 by inajah            #+#    #+#             */
/*   Updated: 2025/02/18 11:48:20 by inajah           ###   ########.fr       */
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
	if (sim->init_flag & INIT_PRINT_LOCK)
		sem_close(sim->print_lock);
	return (simulation_free(sim));
}

# define SEM_FORKS "philo_forks"
# define SEM_PRINT_LOCK "print_lock"

bool	simulation_sem_init(t_simulation *sim)
{
	//TODO: create a function that returns a random semaphore name.
	sem_unlink(SEM_FORKS);
	sem_unlink(SEM_PRINT_LOCK);
	sim->forks = sem_open(SEM_FORKS, O_CREAT, O_RDWR, sim->setting[NB_PHILOS]);
	sim->print_lock = sem_open(SEM_PRINT_LOCK, O_CREAT, O_RDWR, 1);
	sim->init_flag |= (sim->forks != SEM_FAILED);
	sim->init_flag |= (sim->print_lock != SEM_FAILED) << 1;
	return (sim->init_flag & (INIT_FORKS | INIT_PRINT_LOCK));
}

t_simulation	*simulation_init(long *setting)
{
	t_simulation	*sim;

	sim = safe_malloc(sizeof(t_simulation));
	if (sim == NULL)
		return (NULL);
	sim->setting = setting;
	if (!simulation_sem_init(sim))
	{
		printf("bad stuff happened!!\n");
		return (simulation_abort(sim));
	}
	//if (!simulation_philos_init(sim))
	//	return (simulation_abort(sim));
	return (sim);
}



bool	print_message(t_philosopher *philo, char *msg)
{
	long	start;

	start = philo->start_time;
	sem_wait(philo->sim->print_lock);
	printf("%ld %d %s\n", get_current_time_ms() - start, philo->id, msg);
	sem_post(philo->sim->print_lock);
	return (true);
}

bool	philo_eat(t_philosopher *philo)
{
	t_simulation *sim;

	sim = philo->sim;
	sem_wait(sim->forks);
	//TODO: check if the philo died to kill the process
	philo->eat_time = get_current_time_ms();
	print_message(philo, EATING_MESSAGE);
	ft_sleep(philo, sim->setting[TIME_TO_EAT]);
	sem_post(sim->forks);
	return (true);
}

bool	philo_sleep(t_philosopher *philo)
{
	t_simulation *sim;

	sim = philo->sim;
	print_message(philo, SLEEPING_MESSAGE);
	ft_sleep(philo, sim->setting[TIME_TO_SLEEP]);
	return (true);
}

bool	philo_think(t_philosopher *philo)
{
	t_simulation	*sim;
	long			time_to_die;
	long			time_till_death;

	sim = philo->sim;
	print_message(philo, THINKING_MESSAGE);
	if (sim->setting[NB_PHILOS] % 2 == 0)
		return (true);
	time_to_die = sim->setting[TIME_TO_DIE];
	time_till_death = get_current_time_ms() - philo->eat_time;
	if (time_till_death < time_to_die * 0.7)
		usleep(1000);
	return (true);
}

void	philo_process(t_philosopher *philo)
{
	while (get_current_time_ms() < philo->sim->setting[START_TIME])
		usleep(250);
	philo->start_time = get_current_time_ms();
	while (true)
	{
		philo_eat(philo);
		philo_sleep(philo);
		philo_think(philo);
	}
	simulation_abort(philo->sim);
	exit(0);
}

void	philo_init(t_philosopher *philo, t_simulation *sim, int id)
{
	memset(philo, 0, sizeof(t_philosopher));
	philo->id = id;
	philo->sim = sim;
}

void	simulation_start(t_simulation *sim)
{
	t_philosopher	philo;
	pid_t			pid;
	int				i;

	sim->setting[START_TIME] = get_current_time_ms() + sim->setting[NB_PHILOS] * 10;
	i = 0;
	while (i < sim->setting[NB_PHILOS])
	{
		philo_init(&philo, sim, i + 1);
		pid = fork();
		if (pid < 0)
		{
			printf("bad stuff happened");
			break ;
		}
		else if (pid == 0)
			philo_process(&philo);
		i++;
	}
	wait(NULL);
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
