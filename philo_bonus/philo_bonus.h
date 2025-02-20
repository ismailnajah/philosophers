/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 08:50:01 by inajah            #+#    #+#             */
/*   Updated: 2025/02/20 15:14:28 by inajah           ###   ########.fr       */
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

# define SEM_FORKS "forks"
# define SEM_DEATH "death"
# define SEM_DEATH_MESSAGE "death_message"
# define SEM_DONE "done"
# define SEM_STATE "state"
# define SEM_END	"end"
# define SEM_PRINT "print"

# define ERR_PROCESS "process could not be instantiated.\n" 
# define ERR_THREAD "thread could not be instantiated.\n" 

# define DIED_MESSAGE "died"
# define EATING_MESSAGE "is eating" 
# define THINKING_MESSAGE "is thinking"
# define SLEEPING_MESSAGE "is sleeping"
# define FORK_MESSAGE "has taken a fork"

enum
{
	INIT_FORKS = 1,
	INIT_DEATH_LOCK = 1 << 1,
	INIT_DEATH_MESSAGE_LOCK = 1 << 2,
	INIT_DONE_LOCK = 1 << 3,
	INIT_PRINT_LOCK = 1 << 4,
	INIT_STATE_LOCK = 1 << 5,
	INIT_END_LOCK = 1 << 6,
	INIT_SUCCESS = 63,
};

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

typedef struct s_simulation
{
	int		init_flag;
	bool	end_flag;
	long	*setting;
	sem_t	*forks;
	sem_t	*death_lock;
	sem_t	*death_message_lock;
	sem_t	*done_lock;
	sem_t	*print_lock;
	sem_t	*end_lock;
	sem_t	*state_lock;
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

//simulation_utils_bonus.c
pthread_mutex_t	*simulation_mutex_init(void);
void			remove_sem(int nb_philos);
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

	sem_wait(sim->end_lock);
	end_flag = sim->end_flag;
	sem_post(sim->end_lock);
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
