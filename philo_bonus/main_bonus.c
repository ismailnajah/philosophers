/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 15:36:35 by inajah            #+#    #+#             */
/*   Updated: 2025/02/19 17:06:48 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

static void	*simulation_free(t_simulation *sim)
{
	if (sim == NULL)
		return (NULL);
	free(sim->setting);
	free(sim->state_lock);
	free(sim->end_lock);
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
		pthread_mutex_destroy(sim->state_lock);
	if (sim->init_flag & INIT_DEATH_LOCK)
		sem_close(sim->death_lock);
	if (sim->init_flag & INIT_DEATH_MESSAGE_LOCK)
		sem_close(sim->death_message_lock);
	if (sim->init_flag & INIT_DONE_LOCK)
		sem_close(sim->done_lock);
	if (sim->init_flag & INIT_END_LOCK)
		pthread_mutex_destroy(sim->end_lock);
	if (sim->init_flag & INIT_PRINT_LOCK)
		sem_close(sim->print_lock);
	return (simulation_free(sim));
}

# define SEM_FORKS "philo_forks"
# define SEM_DEATH_LOCK "death_lock"
# define SEM_DEATH_MESSAGE_LOCK "death_message_lock"
# define SEM_DONE_LOCK "done_lock"
# define SEM_PRINT_LOCK "print_lock"

pthread_mutex_t	*simulation_mutex_init(void)
{
	pthread_mutex_t	*mutex;

	mutex = safe_malloc(sizeof(pthread_mutex_t));
	if (mutex == NULL)
		return (NULL);
	if (pthread_mutex_init(mutex, NULL) != 0)
	{
		free(mutex);
		return (NULL);
	}
	return (mutex);
}

void	remove_sem(void)
{
	sem_unlink(SEM_FORKS);
	sem_unlink(SEM_DEATH_LOCK);
	sem_unlink(SEM_DEATH_MESSAGE_LOCK);
	sem_unlink(SEM_DONE_LOCK);
	sem_unlink(SEM_PRINT_LOCK);
}

bool	simulation_locks_init(t_simulation *sim)
{
	remove_sem();
	sim->forks = sem_open(SEM_FORKS, O_CREAT, O_RDWR, sim->setting[NB_PHILOS]);
	sim->state_lock = simulation_mutex_init();
	sim->end_lock = simulation_mutex_init();
	sim->death_lock = sem_open(SEM_DEATH_LOCK, O_CREAT, O_RDWR, 0);
	sim->death_message_lock = sem_open(SEM_DEATH_MESSAGE_LOCK, O_CREAT, O_RDWR, 1);
	sim->done_lock = sem_open(SEM_DONE_LOCK, O_CREAT, O_RDWR, 0);
	sim->print_lock = sem_open(SEM_PRINT_LOCK, O_CREAT, O_RDWR, 1);
	sim->init_flag |= (sim->forks != SEM_FAILED);
	sim->init_flag |= (sim->state_lock != NULL) << 1;
	sim->init_flag |= (sim->death_lock != SEM_FAILED) << 2;
	sim->init_flag |= (sim->death_message_lock != SEM_FAILED) << 3;
	sim->init_flag |= (sim->end_lock != NULL) << 4;
	sim->init_flag |= (sim->done_lock != SEM_FAILED) << 5;
	sim->init_flag |= (sim->print_lock != SEM_FAILED) << 6;
	return (sim->init_flag & 63);
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
	//if (!simulation_philos_init(sim))
	//	return (simulation_abort(sim));
	return (sim);
}

bool	is_end_simulation(t_simulation *sim)
{
	bool	end_flag;

	pthread_mutex_lock(sim->end_lock);
	end_flag = sim->end_flag;
	pthread_mutex_unlock(sim->end_lock);
	return (end_flag);
}


bool	print_message(t_philosopher *philo, char *msg, bool check_death)
{
	long	start;

	start = philo->start_time;
	sem_wait(philo->sim->print_lock);
	if (check_death && is_end_simulation(philo->sim))
	{
		sem_post(philo->sim->print_lock);
		return (false);
	}
	printf("%ld %d %s\n", get_current_time_ms() - start, philo->id, msg);
	sem_post(philo->sim->print_lock);
	return (true);
}

bool	philo_died(t_philosopher *philo)
{
	long	time_to_die;
	bool	dead;

	time_to_die = philo->sim->setting[TIME_TO_DIE];
	pthread_mutex_lock(philo->state_lock);
	dead = (get_current_time_ms() - philo->eat_time > time_to_die);
	pthread_mutex_unlock(philo->state_lock);
	return (dead);
}

bool	lock_single_fork(t_philosopher *philo)
{
	if (is_end_simulation(philo->sim))
		return (false);
	sem_wait(philo->sim->forks);
	if (is_end_simulation(philo->sim))
	{
		sem_post(philo->sim->forks);
		return (false);
	}
	if (!print_message(philo, FORK_MESSAGE, true))
	{
		sem_post(philo->sim->forks);
		return (false);
	}
	return (true);
}

bool	lock_forks(t_philosopher *philo)
{
	if (!lock_single_fork(philo))
		return (false);
	if (!lock_single_fork(philo))
		return (false);
	return (true);
}

void	unlock_forks(t_philosopher *philo)
{
	sem_post(philo->sim->forks);
	sem_post(philo->sim->forks);
}

bool	philo_eat(t_philosopher *philo)
{
	t_simulation *sim;

	sim = philo->sim;
	if(!lock_forks(philo))
		return (false);
	if (is_end_simulation(philo->sim))
	{
		sem_post(sim->forks);
		return (false);
	}
	pthread_mutex_lock(philo->state_lock);
	philo->eat_time = get_current_time_ms();
	pthread_mutex_unlock(philo->state_lock);
	philo->meal_count++;
	if (!print_message(philo, EATING_MESSAGE, true))
		return (false);
	ft_sleep(philo, sim->setting[TIME_TO_EAT]);
	unlock_forks(philo);
	return (true);
}

bool	philo_sleep(t_philosopher *philo)
{
	t_simulation *sim;

	sim = philo->sim;
	if(!print_message(philo, SLEEPING_MESSAGE, true))
		return (false);
	ft_sleep(philo, sim->setting[TIME_TO_SLEEP]);
	return (true);
}

bool	philo_think(t_philosopher *philo)
{
	t_simulation	*sim;
	long			time_to_die;
	long			time_till_death;

	sim = philo->sim;
	if(!print_message(philo, THINKING_MESSAGE, true))
		return (false);
	if (sim->setting[NB_PHILOS] % 2 == 0)
		return (true);
	time_to_die = sim->setting[TIME_TO_DIE];
	time_till_death = get_current_time_ms() - philo->eat_time;
	if (time_till_death < time_to_die * 0.7)
		usleep(1000);
	return (true);
}

bool	philo_done(t_philosopher *philo)
{
	long	nb_iterations;
	bool	done;

	nb_iterations = philo->sim->setting[NB_ITERATIONS];
	if (nb_iterations < 0)
		return (false);
	done = (philo->meal_count == nb_iterations);
	if (done)
		sem_post(philo->sim->done_lock);
	return (done);
}

void	*monitor_thread(void *philo_ptr)
{
	t_philosopher	*philo;
	t_simulation	*sim;

	philo = philo_ptr;
	sim = philo->sim;
	sem_wait(sim->death_lock);
	sem_post(sim->death_lock);
	sem_post(sim->done_lock);
	pthread_mutex_lock(sim->end_lock);
	sim->end_flag = true;
	pthread_mutex_unlock(sim->end_lock);
	return (NULL);
}

void	*death_thread(void *philo_ptr)
{
	t_philosopher	*philo;

	philo = philo_ptr;
	while (get_current_time_ms() < philo->sim->setting[START_TIME])
		usleep(250);
	usleep(philo->sim->setting[TIME_TO_DIE] * 800);
	while (true)
	{	
		if (is_end_simulation(philo->sim))
			break ;
		if (philo_died(philo))
		{
			sem_post(philo->sim->death_lock);
			sem_wait(philo->sim->death_message_lock);
			if (is_end_simulation(philo->sim))
				break;
			usleep(3000);
			print_message(philo, DIED_MESSAGE, false);
			pthread_mutex_lock(philo->sim->end_lock);
			philo->sim->end_flag = true;
			pthread_mutex_unlock(philo->sim->end_lock);
			return (NULL);
		}
		usleep(250);
	}
	return (NULL);
}

pthread_t start_thread(t_philosopher *philo, void *(*routine)(void *))
{
	pthread_t	tid;

	if (pthread_create(&tid, NULL, routine, philo) != 0)
	{
		printf("[ERROR] "ERR_THREAD);
		sem_post(philo->sim->death_lock);
		simulation_abort(philo->sim);
		exit(1);
	}
	return (tid);
}

void	philo_start_time(t_philosopher *philo)
{
	while (get_current_time_ms() < philo->sim->setting[START_TIME])
		usleep(250);
	philo->start_time = get_current_time_ms();
	pthread_mutex_lock(philo->state_lock);
	philo->eat_time = philo->start_time;
	pthread_mutex_unlock(philo->state_lock);
	if (philo->id % 2 != 0)
		usleep(1000);
}

bool	philo_alive(t_philosopher *philo)
{
	return (philo_eat(philo)
			&& !philo_done(philo)
			&& philo_sleep(philo)
			&& philo_think(philo));
}

void	philo_process(t_philosopher *philo)
{
	pthread_t	monitor_tid;
	pthread_t	death_tid;

	monitor_tid = start_thread(philo, monitor_thread);
	death_tid = start_thread(philo, death_thread);
	philo_start_time(philo);
	while (philo_alive(philo));
	pthread_join(monitor_tid, NULL);
	pthread_join(death_tid, NULL);
	simulation_abort(philo->sim);
	exit(0);
}

void	philo_init(t_philosopher *philo, t_simulation *sim, int id)
{
	memset(philo, 0, sizeof(t_philosopher));
	philo->id = id;
	philo->state_lock = sim->state_lock;
	philo->sim = sim;
}

//TODO: remove allocation from philo_bonus, use stack allocated structs;

//TODO: create a monitor in the parent process that
// uses sem_wait(death_lock) and kills all the child processes;


void	broadcast_sem(sem_t *sem, int nb)
{
	int i;

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
			printf("[ERROR] Philo %d "ERR_PROCESS, i + 1);
			sem_post(sim->death_lock);
			break ;
		}
		else if (pid == 0)
			philo_process(&philo);
		i++;
	}
	simulation_wait(sim);
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
	remove_sem();
	return (0);
}
