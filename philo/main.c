/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 08:42:29 by inajah            #+#    #+#             */
/*   Updated: 2025/02/15 15:39:45 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

typedef	struct s_philosopher
{
	int				id;
	int				meal_count;
	bool			died;
	long			time_of_last_meal;
	long			*setting;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	*print_lock;
	pthread_mutex_t	*meal_lock;
}	t_philosopher;

typedef struct s_simulation
{
	long			*setting;
	t_philosopher	*philos;
	pthread_mutex_t	*forks;
	pthread_mutex_t	*meal_locks;
	pthread_mutex_t *print_lock;
	int				init_flag;
}	t_simulation;

long long	get_current_time_usec(void)
{
	struct timeval	curr;
	
	//TODO: no idea what should be done if this fails.
	gettimeofday(&curr, NULL);
	return (curr.tv_sec * 1e6 + curr.tv_usec);
}

void ft_usleep(long duration)
{
	long start;
	long end;

	start = get_current_time_usec();
	end = start + duration;
	usleep(duration - (duration / 30));
	while (get_current_time_usec() < end)
		usleep(100);
}

void ft_sleep(long duration)
{
	if (duration < 0)
		return ;
	//usleep(duration);
	ft_usleep(duration);
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
	philo->time_of_last_meal = get_current_time_usec();
	//philo->right_fork = &sim->forks[max(index, (index + 1) % nb_philos)];
	//philo->left_fork = &sim->forks[min(index, (index + 1) % nb_philos)];
	philo->right_fork = &sim->forks[index];
	philo->left_fork = &sim->forks[(index + 1) % nb_philos];
	philo->meal_lock = &sim->meal_locks[index];
	philo->print_lock = sim->print_lock;
	philo->setting = sim->setting;
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
# define INIT_PRINT_LOCK 1<<2

bool	simulation_mutexes_init(t_simulation *sim)
{
	int	nb_philos;

	nb_philos = sim->setting[NB_PHILOS];
	sim->forks = safe_malloc(nb_philos * sizeof(pthread_mutex_t));
	if (sim->forks == NULL)
		return (false);
	sim->meal_locks = safe_malloc(nb_philos * sizeof(pthread_mutex_t));
	if (sim->meal_locks == NULL)
		return (false);
	sim->print_lock = safe_malloc(sizeof(pthread_mutex_t));
	if (sim->print_lock == NULL)
		return (false);
	sim->init_flag |= mutex_list_init(sim->forks, nb_philos);
	sim->init_flag |= mutex_list_init(sim->meal_locks, nb_philos) << 1;
	sim->init_flag |= mutex_list_init(sim->print_lock, 1) << 2;
	return (sim->init_flag & (INIT_FORKS | INIT_MEAL_LOCKS | INIT_PRINT_LOCK));
}

void	*simulation_free(t_simulation *sim)
{
	if (sim == NULL)
		return (NULL);
	free(sim->forks);
	free(sim->meal_locks);
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
		mutex_list_destroy(sim->meal_locks, sim->setting[NB_PHILOS]);
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

# define LEFT_LOCK_MESSAGE  "has taken left fork"
# define RIGHT_LOCK_MESSAGE  "has taken right fork"
# define EATING_MESSAGE     "is eating" 
# define THINKING_MESSAGE   "is thinking"
# define SLEEPING_MESSAGE   "is sleeping"

long	print_message(t_philosopher *philo, char *msg)
{
	long	current;
	long	start;

	start = philo->setting[START_TIME];
	//TODO: research about printf thread safety and understand the meaning of a thread-safe function
	pthread_mutex_lock(philo->print_lock);
	current = get_current_time_usec();
	printf("%ld %d %s\n", (current - start) / 1000, philo->id, msg);
	pthread_mutex_unlock(philo->print_lock);
	return (get_current_time_usec() - current);
}

void	lock_forks(t_philosopher *philo)
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
	pthread_mutex_lock(first_fork);
	print_message(philo, LEFT_LOCK_MESSAGE);
	pthread_mutex_lock(second_fork);
	print_message(philo, RIGHT_LOCK_MESSAGE);
}

void	unlock_forks(t_philosopher *philo)
{
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}

void	philo_eat(t_philosopher *philo)
{
	long	current_time;
	long	delay;
	long	err;

	current_time = get_current_time_usec();
	err = (current_time - philo->time_of_last_meal) / 1000;
	philo->died = (err > philo->setting[TIME_TO_DIE]);
	if (philo->died)
	{
		print_message(philo, "died");
		//TODO: cleanup should happend outside here,
		//which means this thread should mark a shared flag indicating his death
		//so that all the other threads terminates.;
		_exit(1);
	}
	philo->time_of_last_meal = current_time;
	delay = print_message(philo, EATING_MESSAGE);
	ft_sleep(philo->setting[TIME_TO_EAT] * 1000 - delay);
}

void	philo_sleep(t_philosopher *philo)
{
	long	delay;

	delay = print_message(philo, SLEEPING_MESSAGE);
	ft_sleep(philo->setting[TIME_TO_SLEEP] * 1000 - delay);
}

void	philo_think(t_philosopher *philo)
{
	long	time_till_death;
	long	time_to_die;

	//TODO: think of a better way to solve starvation problem specially in the case 3 180 60 60
	//		- create a dynamic wait time based on the time_of_last_meal and time_to_die.
	print_message(philo, THINKING_MESSAGE);
	time_to_die = philo->setting[TIME_TO_DIE] * 1000;
	time_till_death = get_current_time_usec() - philo->time_of_last_meal;
	if (time_till_death < time_to_die * 0.8)
		ft_sleep((time_to_die - time_till_death) / 3);
}

void	*philo_thread(void *philo_ptr)
{
	t_philosopher	*philo;

	philo = philo_ptr;
	while (get_current_time_usec() < philo->setting[START_TIME])
	{
		ft_sleep(100);
	}
	philo->time_of_last_meal = get_current_time_usec();
	if (philo->id % 2 != 0)
		ft_sleep(1000);
	while (true)
	{
		lock_forks(philo);
		philo_eat(philo);
		unlock_forks(philo);
		philo_sleep(philo);
		philo_think(philo);
	}
}

void	simulation_start(t_simulation *sim)
{
	pthread_t	tid;
	int			i;

	i = 0;
	sim->setting[START_TIME] = get_current_time_usec() + sim->setting[NB_PHILOS] * 10000;
	while (i < sim->setting[NB_PHILOS])
	{
		//TODO: thread creation can have a delay, specially when the number of thread is large.
		// which makes the start time differ form thread to thread
		// - add a wait time at the start to sync thread appon creation.
		if (pthread_create(&tid, NULL, philo_thread, sim->philos + i) != 0)
		{
			printf("bad stuff happened\n");
			return ;
		}
		pthread_detach(tid);
		i++;
	}
	while (true);
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
