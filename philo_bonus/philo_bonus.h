/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 08:50:01 by inajah            #+#    #+#             */
/*   Updated: 2025/02/18 17:50:07 by inajah           ###   ########.fr       */
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
	INIT_FORKS = 1,
	INIT_STATE_LOCK = 1 << 1,
	INIT_DEATH_LOCK = 1 << 2,
	INIT_DEATH_MESSAGE_LOCK = 1 << 3,
	INIT_END_LOCK = 1 << 4,
	INIT_PRINT_LOCK = 1<< 5,
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

typedef enum e_state
{
	STATE_ALIVE,
	STATE_DEAD,
	STATE_DONE,
}	t_state;

typedef struct s_simulation
{
	int		init_flag;
	bool	end_flag;
	long	*setting;
	sem_t	*forks;
	sem_t	*death_lock;
	sem_t	*death_message_lock;
	sem_t	*print_lock;
	pthread_mutex_t	*state_lock;
	pthread_mutex_t	*end_lock;
}	t_simulation;

typedef struct s_philosopher
{
	int				id;
	t_state			state;
	int				meal_count;
	time_t			eat_time;
	time_t			start_time;
	pthread_mutex_t	*state_lock;
	t_simulation	*sim;
}	t_philosopher;



//parse.c
long			*parse_args(int ac, char **av);

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
