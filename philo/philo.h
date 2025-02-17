/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 08:50:01 by inajah            #+#    #+#             */
/*   Updated: 2025/02/17 12:43:54 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <stdbool.h>
# include <limits.h>
# include <pthread.h>
# include <sys/time.h>

# define ERR_THREAD "thread could not be instantiated.\n" 

# define DIED_MESSAGE "died"
# define EATING_MESSAGE "is eating" 
# define THINKING_MESSAGE "is thinking"
# define SLEEPING_MESSAGE "is sleeping"
# define FORK_MESSAGE "has taken a fork"

enum
{
	INIT_FORKS = 1,
	INIT_STATE_LOCKS = 1<<1,
	INIT_END_LOCK = 1<<2,
	INIT_PRINT_LOCK = 1<<3,
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

typedef struct s_simulation	t_simulation;

typedef enum e_state
{
	STATE_ALIVE,
	STATE_DEAD,
	STATE_DONE,
}	t_state;

typedef struct s_philosopher
{
	int				id;
	pthread_t		tid;
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
	pthread_mutex_t	*print_lock;
};

//parse.c
long			*parse_args(int ac, char **av);

//simulation.c
t_simulation	*simulation_init(long *setting);
void			*simulation_abort(t_simulation *sim);

//simulation_utils.c
bool			print_message(t_philosopher *philo, char *msg, bool death);
bool			is_end_simulation(t_simulation *sim);
void			mutex_list_destroy(pthread_mutex_t *mutexes, int size);
bool			mutex_list_init(pthread_mutex_t *mutexes, int size);

//monitor.c
void			*monitor_thread(void *sim_ptr);

//philo.c
void			philo_set_state(t_philosopher *philo, t_state state);
t_state			philo_get_state(t_philosopher *philo);
void			philosopher_init(t_simulation *sim, int index);
void			*philo_thread(void *philo_ptr);

//philo_action.c
bool			philo_died(t_philosopher *philo);
bool			philo_done(t_philosopher *philo);
bool			philo_eat(t_philosopher *philo);
bool			philo_sleep(t_philosopher *philo);
bool			philo_think(t_philosopher *philo);

//lock.c
bool			lock_forks(t_philosopher *philo);
void			unlock_forks(t_philosopher *philo);

//time.c
time_t			get_current_time_ms(void);
void			ft_sleep(t_philosopher *philo, time_t duration_ms);

//utils.c
void			*safe_malloc(size_t size);
bool			is_valid_setting(long *setting, int size);
int				is_digit(char c);
void			print_setting(long *setting);
int				usage(char *program);

#endif // PHILO_H
