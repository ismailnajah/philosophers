/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 08:50:01 by inajah            #+#    #+#             */
/*   Updated: 2025/02/22 08:06:04 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <stdbool.h>
# include <limits.h>
# include <fcntl.h>
# include <semaphore.h>
# include <pthread.h>
# include <sys/time.h>
# include <sys/stat.h>
# include <sys/wait.h>

# define ERR_PROCESS "process could not be instantiated.\n" 
# define ERR_THREAD "thread could not be instantiated.\n" 

# define DIED_MESSAGE "died"
# define EATING_MESSAGE "is eating" 
# define THINKING_MESSAGE "is thinking"
# define SLEEPING_MESSAGE "is sleeping"
# define FORK_MESSAGE "has taken a fork"

enum
{
	NB_PHILOS,
	TIME_TO_DIE,
	TIME_TO_EAT,
	TIME_TO_SLEEP,
	NB_ITERATIONS,
	START_TIME,
	SETTING_COUNT,
};

enum e_global_lock
{
	FORKS,
	DEATH,
	DEATH_MESSAGE,
	DONE,
	PRINT,
	GLOBAL_LOCKS_COUNT,
}; 

enum e_local_lock
{
	STATE,
	END,
	LOCAL_LOCKS_COUNT,
};

typedef struct s_lock_def
{
	char	*name;
	int		value;
}	t_lock_def;

typedef struct s_lock
{
	char	*name;
	sem_t	*sem;
	bool	is_opened;
}	t_lock;

typedef struct s_simulation
{
	int		philo_index;
	bool	end_flag;
	long	*setting;
	t_lock	*global_locks;
	t_lock	*local_state_locks;
	t_lock	*local_end_locks;
}	t_simulation;

typedef struct s_philosopher
{
	int				id;
	int				meal_count;
	time_t			eat_time;
	time_t			start_time;
	sem_t			*state_lock;
	t_simulation	*sim;
}	t_philosopher;

//parse_bonus.c
long			*parse_args(int ac, char **av);

//simulation_bonus.c
void			*simulation_abort(t_simulation *sim);
t_simulation	*simulation_init(long *setting);
t_lock_def		get_global_lock_def(long *setting, int lock);
t_lock_def		get_local_lock_def(int lock, int id);

//simulation_utils_bonus.c
pthread_mutex_t	*simulation_mutex_init(void);
void			remove_sem(t_simulation *sem);
void			simulation_set_end(t_simulation *sim, bool value);
bool			print_message(t_philosopher *philo, char *msg, bool death);

//monitors_bonus.c
pthread_t		start_thread(t_philosopher *philo, void *(*routine)(void *));
void			*listener_thread(void *philo_ptr);
void			*death_monitor_thread(void *philo_ptr);

//philo_bonus.c
bool			philo_init(t_philosopher *philo, t_simulation *sim, int id);
void			philo_process(t_philosopher *philo);
char			*get_end_name(int id);
char			*get_state_name(int id);

//philo_actions_bonus.c
bool			philo_eat(t_philosopher *philo);
bool			philo_sleep(t_philosopher *philo);
bool			philo_think(t_philosopher *philo);
bool			philo_done(t_philosopher *philo);

//locks_bonus.c
bool			lock_forks(t_philosopher *philo);
void			unlock_forks(t_philosopher *philo);

//time_bonus.c
void			ft_sleep(t_philosopher *philo, time_t duration_ms);

//utils_bonus.c
void			*safe_malloc(size_t size);
bool			is_valid_setting(long *setting, int size);
int				is_digit(char c);
void			print_setting(long *setting);
int				usage(char *program);

static inline time_t	get_current_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (-1);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

static inline bool	is_end_simulation(t_simulation *sim)
{
	bool	end_flag;
	int		philo_index;

	philo_index = sim->philo_index;
	sem_wait(sim->local_end_locks[philo_index].sem);
	end_flag = sim->end_flag;
	sem_post(sim->local_end_locks[philo_index].sem);
	return (end_flag);
}

static inline bool	philo_died(t_philosopher *philo)
{
	long	time_to_die;
	bool	dead;

	time_to_die = philo->sim->setting[TIME_TO_DIE];
	sem_wait(philo->state_lock);
	dead = (get_current_time_ms() - philo->eat_time > time_to_die);
	sem_post(philo->state_lock);
	return (dead);
}

#endif // PHILO_BONUS_H
