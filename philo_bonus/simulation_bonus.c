/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 17:44:05 by inajah            #+#    #+#             */
/*   Updated: 2025/02/22 08:05:54 by inajah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <philo_bonus.h>

static size_t	ft_strlen(const char *s)
{
	size_t	length;

	if (s == NULL)
		return (0);
	length = 0;
	while (s[length])
		length++;
	return (length);
}

void	*ft_memcpy(void *dst, const void *src, size_t len)
{
	unsigned char		*d;
	const unsigned char	*s;

	d = dst;
	s = src;
	while (len-- > 0)
		*d++ = *s++;
	return (dst);
}

char	*ft_strdup(const char *s1)
{
	size_t	len;
	char	*string;

	if (s1 == NULL)
		return (NULL);
	len = ft_strlen(s1) + 1;
	string = malloc(len);
	if (!string)
		return (NULL);
	ft_memcpy(string, s1, len);
	return (string);
}

static char	*ft_itoa(int n)
{
	char			*p;
	long			tmp;
	unsigned int	len;
	long			nbr;

	len = (n < 0) + 1;
	nbr = n * ((-2 * (n < 0) + 1));
	tmp = nbr;
	while (tmp > 9)
	{
		tmp /= 10;
		len++;
	}
	p = safe_malloc((len + 1) * sizeof(char));
	if (!p)
		return (NULL);
	while (len--)
	{
		p[len] = '0' + nbr % 10;
		nbr /= 10;
	}
	if (n < 0)
		p[0] = '-';
	return (p);
}

static size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	dst_length;
	size_t	src_length;
	size_t	i;

	dst_length = ft_strlen(dst);
	src_length = ft_strlen(src);
	if (dstsize <= dst_length)
		return (dstsize + src_length);
	i = dst_length;
	while (*src && i < dstsize - 1)
		dst[i++] = *src++;
	dst[i] = '\0';
	return (dst_length + src_length);
}

void	lock_init(t_lock *lock, char *name, int value)
{
	lock->name = name;
	lock->sem = sem_open(name, O_CREAT, O_RDWR, value);
	lock->is_opened = (lock->sem != SEM_FAILED);
}

void	lock_destroy(t_lock *lock)
{
	free(lock->name);
	if (lock->is_opened)
		sem_close(lock->sem);
}

static void *simulation_free(t_simulation *sim)
{
	if (sim == NULL)
		return (NULL);
	free(sim->setting);
	free(sim->global_locks);
	free(sim->local_state_locks);
	free(sim->local_end_locks);
	free(sim);
	return (NULL);
}

void	*simulation_abort(t_simulation *sim)
{
	int	i;

	if (sim == NULL)
		return (NULL);
	i = 0;
	while (i < GLOBAL_LOCKS_COUNT)
	{
		lock_destroy(&sim->global_locks[i]);
		i++;
	}
	i = 0;
	while (i < sim->setting[NB_PHILOS])
	{
		lock_destroy(&sim->local_state_locks[i]);
		lock_destroy(&sim->local_end_locks[i]);
		i++;
	}
	return (simulation_free(sim));
}

t_lock_def	get_global_lock_def(long *setting, int lock)
{
	t_lock_def			lock_def;
	static t_lock_def	g_locks_def[GLOBAL_LOCKS_COUNT] = {
	{"forks", 1},
	{"death", 0},
	{"death_message", 1},
	{"done", 0},
	{"print", 1},
	};

	lock_def.name = ft_strdup(g_locks_def[lock].name);
	lock_def.value = g_locks_def[lock].value;
	if (lock == FORKS)
		lock_def.value = setting[NB_PHILOS];
	return (lock_def);
}

t_lock_def get_local_lock_def(int lock, int id)
{
	t_lock_def lock_def;
	char *name;
	char *str_id;
	static int	name_size = 256;
	static char *prefix[LOCAL_LOCKS_COUNT] = {
	"state",
	"end",
	};

	lock_def.name = NULL;
	lock_def.value = 0;
	name = safe_malloc(name_size * sizeof(char));
	if (name == NULL)
		return (lock_def);
	ft_strlcat(name, prefix[lock], name_size);
	str_id = ft_itoa(id);
	ft_strlcat(name, str_id, name_size);
	free(str_id);
	lock_def.name = name;
	lock_def.value = 1;
	return (lock_def);
}

static bool	is_simulation_locks_valid(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < GLOBAL_LOCKS_COUNT)
	{
		if (sim->global_locks[i].is_opened == false)
			return (false);
		i++;
	}
	i = 0;
	while (i < sim->setting[NB_PHILOS])
	{
		if (sim->local_state_locks[i].is_opened == false)
			return (false);
		if (sim->local_end_locks[i].is_opened == false)
			return (false);
		i++;
	}
	return (true);
}

static bool simulation_local_locks_init(t_simulation *sim)
{
	int i;
	int	nb_philos;
	t_lock_def	lock_def;

	nb_philos = sim->setting[NB_PHILOS];
	sim->local_state_locks = safe_malloc(nb_philos * sizeof(t_lock));
	if (sim->local_state_locks == NULL)
		return (false);
	sim->local_end_locks = safe_malloc(nb_philos * sizeof(t_lock));
	if (sim->local_end_locks == NULL)
		return (false);
	i = 0;
	while (i < sim->setting[NB_PHILOS])
	{
		lock_def = get_local_lock_def(STATE, i + 1);
		lock_init(&sim->local_state_locks[i], lock_def.name, lock_def.value);
		lock_def = get_local_lock_def(END, i + 1);
		lock_init(&sim->local_end_locks[i], lock_def.name, lock_def.value);
		i++;
	}
	return (true);
}

static bool simulation_locks_init(t_simulation *sim)
{
	t_lock_def	lock_def;
	int	i;

	remove_sem(sim);
	sim->global_locks = safe_malloc(GLOBAL_LOCKS_COUNT * sizeof(t_lock));
	if (sim->global_locks == NULL)
		return (false);
	i = 0;
	while (i < GLOBAL_LOCKS_COUNT)
	{
		lock_def = get_global_lock_def(sim->setting, i);
		lock_init(sim->global_locks + i, lock_def.name, lock_def.value);
		i++;
	}
	if (!simulation_local_locks_init(sim))
		return (false);
	return (is_simulation_locks_valid(sim));
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
	return (sim);
}
