/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 08:50:01 by inajah            #+#    #+#             */
/*   Updated: 2025/02/19 18:30:46 by inajah           ###   ########.fr       */
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

# define SEM_FORKS "philo_forks"
# define SEM_DEATH "death_lock"
# define SEM_DEATH_MESSAGE "death_message_lock"
# define SEM_DONE "done_lock"
# define SEM_PRINT "print_lock"

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
	INIT_STATE_LOCK = 1 << 1,
	INIT_DEATH_LOCK = 1 << 2,
	INIT_DEATH_MESSAGE_LOCK = 1 << 3,
	INIT_END_LOCK = 1 << 4,
	INIT_DONE_LOCK = 1 << 5,
	INIT_PRINT_LOCK = 1<< 6,
	INIT_SUCCESS = 0x3F,
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
	int				init_flag;
	bool			end_flag;
	long			*setting;
	sem_t			*forks;
	sem_t			*death_lock;
	sem_t			*death_message_lock;
	sem_t			*done_lock;
	sem_t			*print_lock;
	pthread_mutex_t	*state_lock;
	pthread_mutex_t	*end_lock;

}	t_simulation;

typedef struct s_philosopher
{
	int				id;
	int				meal_count;
	time_t			eat_time;
	time_t			start_time;
	pthread_mutex_t	*state_lock;
	t_simulation	*sim;
}	t_philosopher;

//parse_bonus.c
long			*parse_args(int ac, char **av);

//simulation_bonus.c
void			*simulation_abort(t_simulation *sim);
t_simulation	*simulation_init(long *setting);

//simulation_utils_bonus.c
pthread_mutex_t	*simulation_mutex_init(void);
void			remove_sem(void);
void			simulation_set_end(t_simulation *sim, bool value);
bool			is_end_simulation(t_simulation *sim);
bool			print_message(t_philosopher *philo, char *msg, bool death);

//monitors_bonus.c
pthread_t		start_thread(t_philosopher *philo, void *(*routine)(void *));
void			*listener_thread(void *philo_ptr);
void			*death_monitor_thread(void *philo_ptr);

//philo_bonus.c
void			philo_init(t_philosopher *philo, t_simulation *sim, int id);
void			philo_process(t_philosopher *philo);

//philo_actions_bonus.c
bool			philo_died(t_philosopher *philo);
bool			philo_eat(t_philosopher *philo);
bool			philo_sleep(t_philosopher *philo);
bool			philo_think(t_philosopher *philo);
bool			philo_done(t_philosopher *philo);

//locks_bonus.c
bool			lock_forks(t_philosopher *philo);
void			unlock_forks(t_philosopher *philo);

//time_bonus.c
time_t			get_current_time_ms(void);
void			ft_sleep(t_philosopher *philo, time_t duration_ms);

//utils_bonus.c
void			*safe_malloc(size_t size);
bool			is_valid_setting(long *setting, int size);
int				is_digit(char c);
void			print_setting(long *setting);
int				usage(char *program);

#endif // PHILO_BONUS_H
