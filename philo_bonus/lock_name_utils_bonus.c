/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lock_name_utils_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inajah <inajah@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 08:11:30 by inajah            #+#    #+#             */
/*   Updated: 2025/02/22 08:24:56 by inajah           ###   ########.fr       */
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

char	*ft_itoa(int n)
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

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
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
