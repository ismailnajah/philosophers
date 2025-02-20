/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_name_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 15:10:53 by inajah            #+#    #+#             */
/*   Updated: 2025/02/20 15:18:59 by inajah           ###   ########.fr       */
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

char	*get_state_name(int id)
{
	static char	name[256];
	char		*str_id;

	name[0] = '\0';
	ft_strlcat(name, SEM_STATE, sizeof(name));
	str_id = ft_itoa(id);
	ft_strlcat(name, str_id, sizeof(name));
	free(str_id);
	return (name);
}

char	*get_end_name(int id)
{
	static char	name[256];
	char		*str_id;

	name[0] = '\0';
	ft_strlcat(name, SEM_END, sizeof(name));
	str_id = ft_itoa(id);
	ft_strlcat(name, str_id, sizeof(name));
	free(str_id);
	return (name);
}
