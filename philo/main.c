/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 08:42:29 by inajah            #+#    #+#             */
/*   Updated: 2025/02/13 15:58:19 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo.h>

typedef	struct s_philosopher
{
	int				id;
	int				*setting;
	int				meal_count;
	int				err;
	bool			died;
	long			time_of_last_meal;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	*print_lock;
}	t_philosopher;

typedef struct s_simulation
{
	int				*setting;
	t_philosopher	*philos;
	pthread_mutex_t	*forks;
	pthread_mutex_t *print_lock;
}	t_simulation;

long long	get_current_time_usec(void)
{
	struct timeval	curr;
	
	//TODO: no idea what should be done if this fails.
	gettimeofday(&curr, NULL);
	return (curr.tv_sec * 1e6 + curr.tv_usec);
}

void	simulation_mutex_destroy(pthread_mutex_t *mutexes, int size)
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

bool	simulation_mutex_init(pthread_mutex_t *mutexes, int size)
{
	int	i;

	if (mutexes == NULL)
		return (false);
	i = 0;
	while (i < size)
	{
		if (pthread_mutex_init(mutexes + i, NULL) != 0)
		{
			simulation_mutex_destroy(mutexes, i);
			return (false);
		}
		i++;
	}
	return (true);
}

void	*simulation_free(t_simulation *sim)
{
	if (sim == NULL)
		return (NULL);
	free(sim->forks);
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
	simulation_mutex_destroy(sim->forks, sim->setting[NB_PHILOS]);
	simulation_mutex_destroy(sim->print_lock, 1);
	return (simulation_free(sim));
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

bool	simulation_init_philos(t_simulation *sim)
{
	t_philosopher	*philos;
	int				i;
	int				nb_philos;

	nb_philos = sim->setting[NB_PHILOS];
	philos = safe_malloc(nb_philos * sizeof(t_philosopher));
	if (philos == NULL)
		return (false);
	i = 0;
	while (i < nb_philos)
	{
		philos[i].id = i + 1;
		philos[i].time_of_last_meal = get_current_time_usec();
		philos[i].right_fork = &sim->forks[max(i, (i + 1) % nb_philos)];
		philos[i].left_fork = &sim->forks[min(i, (i + 1) % nb_philos)];
		philos[i].print_lock = sim->print_lock;
		philos[i].setting = sim->setting;
		i++;
	}
	sim->philos = philos;
	return (true);
}

t_simulation	*simulation_init(int *setting)
{
	t_simulation	*sim;

	sim = safe_malloc(sizeof(t_simulation));
	if (sim == NULL)
		return (NULL);
	sim->setting = setting;
	sim->forks = safe_malloc(setting[NB_PHILOS] * sizeof(pthread_mutex_t));
	if (sim->forks == NULL)
		return (simulation_free(sim));
	sim->print_lock = safe_malloc(sizeof(pthread_mutex_t));
	if (sim->print_lock == NULL)
		return (simulation_free(sim));
	if (!simulation_mutex_init(sim->forks, setting[NB_PHILOS]))
		return (simulation_free(sim));
	if (!simulation_mutex_init(sim->print_lock, 1))
	{
		simulation_mutex_destroy(sim->forks, setting[NB_PHILOS]);
		return (simulation_free(sim));
	}
	if (!simulation_init_philos(sim))
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
	long			current;

	current = get_current_time_usec();
	pthread_mutex_lock(philo->print_lock);
	//printf("%ld %d %s (err: %d)\n", time_in_ms, philo->id, msg, philo->err/1000);
	printf("%ld %d %s\n", current / 1000, philo->id, msg);
	pthread_mutex_unlock(philo->print_lock);
	return (get_current_time_usec() - current);
}

void	lock_forks(t_philosopher *philo)
{
	pthread_mutex_lock(philo->left_fork);
	print_message(philo, LEFT_LOCK_MESSAGE);
	pthread_mutex_lock(philo->right_fork);
	print_message(philo, RIGHT_LOCK_MESSAGE);
}

void	unlock_forks(t_philosopher *philo)
{
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}

void	start_eating(t_philosopher *philo)
{
	long	current_time;
	long	delay;

	current_time = get_current_time_usec();
	philo->err = current_time - philo->time_of_last_meal;
	philo->died = philo->err > philo->setting[TIME_TO_DIE] * 1000;
	if (philo->died)
	{
		print_message(philo, "died");
		_exit(1);
	}
	philo->time_of_last_meal = current_time;
	delay = print_message(philo, EATING_MESSAGE);
	usleep(philo->setting[TIME_TO_EAT] * 1000 - delay);
}

void	*philo_thread(void *philo_ptr)
{
	t_philosopher	*philo;
	long			delay;

	philo = philo_ptr;
	if (philo->id % 2 == 0)
		usleep(1000);
	while (true)
	{
		philo->err = 0;
		lock_forks(philo);
		start_eating(philo);
		unlock_forks(philo);
		//TODO: printing messages can cause a delay bigger than the sleeping duration?
		delay = print_message(philo, SLEEPING_MESSAGE);
		usleep(philo->setting[TIME_TO_SLEEP] * 1000 - delay);
		print_message(philo, THINKING_MESSAGE);
	}
}

void	simulation_start(t_simulation *sim)
{
	pthread_t	tid;
	int			i;

	i = 0;
	while (i < sim->setting[NB_PHILOS])
	{
		if (pthread_create(&tid, NULL, philo_thread, sim->philos + i) != 0)
		{
			printf("bad stuff happened\n");
			exit(1);
		}
		pthread_detach(tid);
		i++;
	}
	while (true)
		usleep(1000 * 1000);
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
	print_setting(setting);
	sim = simulation_init(setting);
	simulation_start(sim);
	simulation_abort(sim);
	return (0);
}
