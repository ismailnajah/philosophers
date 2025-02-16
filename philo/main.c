/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 08:42:29 by inajah            #+#    #+#             */
/*   Updated: 2025/02/16 17:24:20 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

typedef struct s_simulation	t_simulation;

typedef	struct s_philosopher
{
	int				id;
	int				meal_count;
	time_t			eat_time;
	bool			is_dead;
	bool			done;
	time_t			start_time;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	*done_lock;
	pthread_mutex_t	*eat_time_lock;
	t_simulation	*sim;
}	t_philosopher;

struct s_simulation
{
	int				init_flag;
	bool			end_flag;
	bool			death_occured;
	long			*setting;
	t_philosopher	*philos;
	pthread_mutex_t	*forks;
	pthread_mutex_t	*done_locks;
	pthread_mutex_t	*eat_time_locks;
	pthread_mutex_t	*death_lock;
	pthread_mutex_t *print_lock;
};


bool	is_death_occured(t_simulation *sim)
{
	bool death_occured;

	pthread_mutex_lock(sim->death_lock);
	death_occured = sim->death_occured;
	pthread_mutex_unlock(sim->death_lock);
	return (death_occured);
}


time_t	get_current_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (-1);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	ft_sleep(t_philosopher *philo, time_t duration_ms)
{
	time_t	start;

	start = get_current_time_ms();
	usleep((duration_ms * 1000) * 0.7);
	while (true)
	{
		if (get_current_time_ms() - start >= duration_ms || is_death_occured(philo->sim))
			break ;
		usleep(500);
	}
}

void	mutex_list_destroy(pthread_mutex_t *mutexes, int size)
{
	int	i;

	if (mutexes == NULL)
		return ;
	i = 0;
	while (i < size)
	{
		pthread_mutex_destroy(mutexes + i);
		i++;
	}
}

bool	mutex_list_init(pthread_mutex_t *mutexes, int size)
{
	int	i;

	if (mutexes == NULL)
		return (false);
	i = 0;
	while (i < size)
	{
		if (pthread_mutex_init(mutexes + i, NULL) != 0)
		{
			mutex_list_destroy(mutexes, i);
			return (false);
		}
		i++;
	}
	return (true);
}

int	min(int a, int b)
{
	if (a < b)
		return (a);
	return (b);
}

int	max(int a, int b)
{
	if (a > b)
		return (a);
	return (b);
}

void	philosopher_init(t_simulation *sim, int index)
{
	t_philosopher	*philo;
	int				nb_philos;

	nb_philos = sim->setting[NB_PHILOS];
	philo = &sim->philos[index];
	philo->id = index + 1;
	philo->meal_count = 0;
	philo->eat_time = 0;
	philo->eat_time_lock = &sim->eat_time_locks[index];
	philo->right_fork = &sim->forks[index];
	philo->left_fork = &sim->forks[(index + 1) % nb_philos];
	philo->done_lock = &sim->done_locks[index];
	philo->sim = sim;
}

bool	simulation_philos_init(t_simulation *sim)
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

# define INIT_FORKS 1
# define INIT_MEAL_LOCKS 1<<1
# define INIT_EAT_TIME_LOCKS 1<<2
# define INIT_DEATH_LOCK 1<<3
# define INIT_PRINT_LOCK 1<<4

bool	simulation_mutexes_init(t_simulation *sim)
{
	int	nb_philos;

	nb_philos = sim->setting[NB_PHILOS];
	sim->forks = safe_malloc(nb_philos * sizeof(pthread_mutex_t));
	if (sim->forks == NULL)
		return (false);
	sim->done_locks = safe_malloc(nb_philos * sizeof(pthread_mutex_t));
	if (sim->done_locks == NULL)
		return (false);
	sim->eat_time_locks = safe_malloc(nb_philos * sizeof(pthread_mutex_t));
	if (sim->eat_time_locks == NULL)
		return (false);
	sim->death_lock = safe_malloc(sizeof(pthread_mutex_t));
	if (sim->death_lock == NULL)
		return (false);
	sim->print_lock = safe_malloc(sizeof(pthread_mutex_t));
	if (sim->print_lock == NULL)
		return (false);
	sim->init_flag |= mutex_list_init(sim->forks, nb_philos);
	sim->init_flag |= mutex_list_init(sim->done_locks, nb_philos) << 1;
	sim->init_flag |= mutex_list_init(sim->eat_time_locks, nb_philos) << 2;
	sim->init_flag |= mutex_list_init(sim->death_lock, 1) << 3;
	sim->init_flag |= mutex_list_init(sim->print_lock, 1) << 4;
	return (sim->init_flag & (INIT_FORKS | INIT_MEAL_LOCKS | INIT_EAT_TIME_LOCKS | INIT_DEATH_LOCK | INIT_PRINT_LOCK));
}

void	*simulation_free(t_simulation *sim)
{
	if (sim == NULL)
		return (NULL);
	free(sim->forks);
	free(sim->done_locks);
	free(sim->eat_time_locks);
	free(sim->death_lock);
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
	if (sim->init_flag & INIT_MEAL_LOCKS)
		mutex_list_destroy(sim->done_locks, sim->setting[NB_PHILOS]);
	if (sim->init_flag & INIT_EAT_TIME_LOCKS)
		mutex_list_destroy(sim->eat_time_locks, sim->setting[NB_PHILOS]);
	if (sim->init_flag & INIT_DEATH_LOCK)
		mutex_list_destroy(sim->death_lock, 1);
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

# define FORK_MESSAGE  "has taken a fork"
# define EATING_MESSAGE     "is eating" 
# define THINKING_MESSAGE   "is thinking"
# define SLEEPING_MESSAGE   "is sleeping"
# define DIED_MESSAGE       "died"

bool	print_message(t_philosopher *philo, char *msg, bool check_death)
{
	long	start;

	start = philo->start_time;
	pthread_mutex_lock(philo->sim->print_lock);
	if (check_death && is_death_occured(philo->sim))
	{
		pthread_mutex_unlock(philo->sim->print_lock);
		return (false);
	}
	printf("%ld %d %s\n", get_current_time_ms() - start, philo->id, msg);
	pthread_mutex_unlock(philo->sim->print_lock);
	return (true);
}


bool	lock_single_fork(t_philosopher *philo, pthread_mutex_t *fork)
{
	if (is_death_occured(philo->sim))
		return (false);
	pthread_mutex_lock(fork);
	if (is_death_occured(philo->sim))
	{
		pthread_mutex_unlock(fork);
		return (false);
	}
	print_message(philo, FORK_MESSAGE, true);
	return (true);
}

bool	lock_forks(t_philosopher *philo)
{
	pthread_mutex_t	*first_fork;
	pthread_mutex_t	*second_fork;

	first_fork = philo->left_fork;
	second_fork = philo->right_fork;
	if (philo->id % 2 != 0)
	{
		first_fork = philo->right_fork;
		second_fork = philo->left_fork;
	}
	if (!lock_single_fork(philo, first_fork))
		return (false);
	if (!lock_single_fork(philo, second_fork))
		return (false);
	return (true);
}

void	unlock_forks(t_philosopher *philo)
{
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}

bool	is_philo_dead(t_philosopher *philo)
{
	long	time_to_die;
	long	diff;
	bool	dead;

	time_to_die = philo->sim->setting[TIME_TO_DIE];
	diff = get_current_time_ms() - philo->eat_time; 
	dead = (diff > time_to_die);
	pthread_mutex_lock(philo->eat_time_lock);
	philo->is_dead = dead;
	pthread_mutex_unlock(philo->eat_time_lock);
	return (dead);
}

bool	philo_eat(t_philosopher *philo)
{
	long	*setting;

	if (!lock_forks(philo))
		return (false);
	setting = philo->sim->setting;
	if (is_philo_dead(philo))
	{
		unlock_forks(philo);
		return (false);
	}
	philo->eat_time = get_current_time_ms();
	print_message(philo, EATING_MESSAGE, true);
	ft_sleep(philo, setting[TIME_TO_EAT]);
	unlock_forks(philo);
	return (true);
}

bool	philo_sleep(t_philosopher *philo)
{
	if(!print_message(philo, SLEEPING_MESSAGE, true))
		return (false);
	ft_sleep(philo, philo->sim->setting[TIME_TO_SLEEP]);
	return (true);
}

bool	philo_think(t_philosopher *philo)
{
	long	time_till_death;
	long	time_to_die;

	//TODO: think of a better way to solve starvation problem specially in the case 3 180 60 60
	//		- create a dynamic wait time based on the eat_time and time_to_die.
	
	if(!print_message(philo, THINKING_MESSAGE, true))
		return (false);
	time_to_die = philo->sim->setting[TIME_TO_DIE];
	time_till_death = get_current_time_ms() - philo->eat_time;
	if (time_till_death < time_to_die * 0.7)
		usleep(1000);
	return (true);
}

void	*philo_thread(void *philo_ptr)
{
	t_philosopher	*philo;
	
	philo = philo_ptr;
	while (get_current_time_ms() < philo->sim->setting[START_TIME])
		usleep(250);
	philo->start_time = get_current_time_ms();
	philo->eat_time = philo->start_time;
	if (philo->id % 2 != 0)
		usleep(1000);
	while (true)
	{
		if (!philo_eat(philo))
			break;
		if (!philo_sleep(philo))
			break;
		if (!philo_think(philo))
			break;
		//TODO: add philo done. when number of iterations is specified.
	}
	return (NULL);
}

bool	someone_died(t_philosopher *philo)
{

	bool	dead;

	pthread_mutex_lock(philo->eat_time_lock);
	dead = philo->is_dead;
	pthread_mutex_unlock(philo->eat_time_lock);
	return (dead);

	/*
	long	current;
	long	eat_time;
	long	time_to_die;

	time_to_die = philo->sim->setting[TIME_TO_DIE];
	pthread_mutex_lock(philo->eat_time_lock);
	current = get_current_time_ms();
	eat_time = philo->eat_time;
	pthread_mutex_unlock(philo->eat_time_lock);
	return ((current - eat_time) > time_to_die);
	*/
}

void	*monitor_thread(void *sim_ptr)
{
	t_simulation	*sim;
	int				philo_id;

	sim = sim_ptr;
	while (get_current_time_ms() < sim->setting[START_TIME])
		usleep(250);
	while (true)
	{
		philo_id = 0;
		while (philo_id < sim->setting[NB_PHILOS])
		{
			if (someone_died(&sim->philos[philo_id]))
			{
				pthread_mutex_lock(sim->death_lock);
				sim->death_occured = true;
				pthread_mutex_unlock(sim->death_lock);
				print_message(&sim->philos[philo_id], DIED_MESSAGE, false);
				return (NULL);
			}
			philo_id++;
		}
		usleep(500);
	}
}

void	simulation_start(t_simulation *sim)
{
	pthread_t	tid;
	int			i;

	i = 0;
	//TODO: thread creation can have a delay, specially when the number of threads is large.
	// which makes the start time differ form thread to thread
	// - add a wait time at the start of each thread to sync threads start time.
	sim->setting[START_TIME] = get_current_time_ms() + sim->setting[NB_PHILOS] * 10;
	while (i < sim->setting[NB_PHILOS])
	{
		if (pthread_create(&tid, NULL, philo_thread, sim->philos + i) != 0)
		{
			printf("bad stuff happened\n");
			return ;
		}
		pthread_detach(tid);
		i++;
	}
	//TODO: add monitor thread;
	if (pthread_create(&tid, NULL, monitor_thread, sim) != 0)
	{
		printf("[ERROR] Monitor could not be instantiated.\n");
		return ;
	}
	pthread_join(tid, NULL);
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
	simulation_start(sim);
	simulation_abort(sim);
	return (0);
}
