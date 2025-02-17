/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 08:42:29 by inajah            #+#    #+#             */
/*   Updated: 2025/02/17 11:01:28 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

typedef struct s_simulation	t_simulation;

typedef enum e_state
{
	STATE_ALIVE,
	STATE_DEAD,
	STATE_DONE,
} t_state;

typedef	struct s_philosopher
{
	int				id;
	pthread_t			tid;
	t_state			state;
	int				meal_count;
	time_t			eat_time;
	time_t			start_time;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	*state_lock;
	t_simulation	*sim;
}	t_philosopher;

struct s_simulation
{
	int				init_flag;
	bool			end_flag;
	long			*setting;
	t_philosopher	*philos;
	pthread_mutex_t	*forks;
	pthread_mutex_t	*state_locks;
	pthread_mutex_t	*end_lock;
	pthread_mutex_t *print_lock;
};


bool	is_end_simulation(t_simulation *sim)
{
	bool end_flag;

	pthread_mutex_lock(sim->end_lock);
	end_flag = sim->end_flag;
	pthread_mutex_unlock(sim->end_lock);
	return (end_flag);
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
		if (get_current_time_ms() - start >= duration_ms
			|| is_end_simulation(philo->sim))
			break;
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

void	philosopher_init(t_simulation *sim, int index)
{
	t_philosopher	*philo;
	int				nb_philos;

	nb_philos = sim->setting[NB_PHILOS];
	philo = &sim->philos[index];
	philo->id = index + 1;
	philo->right_fork = &sim->forks[index];
	philo->left_fork = &sim->forks[(index + 1) % nb_philos];
	philo->state_lock = &sim->state_locks[index];
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
# define INIT_STATE_LOCKS 1<<1
# define INIT_END_LOCK 1<<2
# define INIT_PRINT_LOCK 1<<3

bool	simulation_mutexes_init(t_simulation *sim)
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
	return (sim->init_flag & (INIT_FORKS | INIT_STATE_LOCKS | INIT_END_LOCK | INIT_PRINT_LOCK));
}

void	*simulation_free(t_simulation *sim)
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
	if (check_death && is_end_simulation(philo->sim))
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
	if (is_end_simulation(philo->sim))
		return (false);
	pthread_mutex_lock(fork);
	if (is_end_simulation(philo->sim))
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

void	philo_set_state(t_philosopher *philo, t_state state)
{
	pthread_mutex_lock(philo->state_lock);
	philo->state = state;
	pthread_mutex_unlock(philo->state_lock);
}

t_state	philo_get_state(t_philosopher *philo)
{
	t_state state;

	pthread_mutex_lock(philo->state_lock);
	state = philo->state;
	pthread_mutex_unlock(philo->state_lock);
	return (state);
}

bool	philo_died(t_philosopher *philo)
{
	long	time_to_die;
	bool	dead;

	time_to_die = philo->sim->setting[TIME_TO_DIE];
	dead = (get_current_time_ms() - philo->eat_time > time_to_die);
	if (dead)
		philo_set_state(philo, STATE_DEAD);
	return (dead);
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
		philo_set_state(philo, STATE_DONE);
	return (done); 
}

bool	philo_eat(t_philosopher *philo)
{
	long	*setting;

	if (!lock_forks(philo))
		return (false);
	setting = philo->sim->setting;
	if (philo_died(philo))
	{
		unlock_forks(philo);
		return (false);
	}
	philo->eat_time = get_current_time_ms();
	philo->meal_count++;
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
		if (philo_done(philo))
			break;
		if (!philo_sleep(philo))
			break;
		if (!philo_think(philo))
			break;
	}
	return (NULL);
}

void	*monitor_thread(void *sim_ptr)
{
	t_simulation	*sim;
	int				philo_id;
	int				philos_done;

	sim = sim_ptr;
	philos_done = 0;
	while (get_current_time_ms() < sim->setting[START_TIME])
		usleep(250);
	while (philos_done < sim->setting[NB_PHILOS])
	{
		philos_done = 0;
		philo_id = 0;
		while (philo_id < sim->setting[NB_PHILOS])
		{
			if (philo_get_state(&sim->philos[philo_id]) == STATE_DONE)
				philos_done++;
			else if (philo_get_state(&sim->philos[philo_id]) == STATE_DEAD)
			{
				pthread_mutex_lock(sim->end_lock);
				sim->end_flag = true;
				pthread_mutex_unlock(sim->end_lock);
				print_message(&sim->philos[philo_id], DIED_MESSAGE, false);
				return (NULL);
			}
			philo_id++;
		}
		usleep(500);
	}
	return (NULL);
}

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
			printf("[ERROR] Philo %d thread could not be instantiated.\n", i + 1);
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
	int			nb_started_philos;

	sim->setting[START_TIME] = get_current_time_ms() + sim->setting[NB_PHILOS] * 10;
	if (pthread_create(&monitor_tid, NULL, monitor_thread, sim) != 0)
	{
		printf("[ERROR] Monitor could not be instantiated.\n");
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
