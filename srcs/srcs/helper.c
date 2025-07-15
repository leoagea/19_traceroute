/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 18:51:53 by lagea             #+#    #+#             */
/*   Updated: 2025/07/14 21:58:55 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ft_traceroute.h"

int		ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i = 0;

	if (n == 0)
		return (0);
	if (!s1 || !s2)
		return (0);
	while (i < n){
		if (s1[i] != s2[i] || s1[i] == '\0')
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}

	return (0);
}

void	*ft_calloc(size_t count, size_t size)
{
	void	*res;
	size_t	i = -1;
	size_t	test = count * size;

	if (size != 0 && (test / size) != count)
		return (NULL);
	res = (void *)malloc(count * size);
	if (!res)
		return (NULL);

	while (++i < test)
		*(unsigned char *)(res + i) = 0;

	return (res);
}

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	size_t	i = -1;

	if (!dst && !src)
		return (NULL);
	if (dst != src)
		while (++i < n)
			((unsigned char *)dst)[i] = ((unsigned char *)src)[i];

	return (dst);
}

void	*ft_memset(void *str, int c, size_t n)
{
	size_t	i = -1;
	char	*cpy = str;

	while (++i < n)
		cpy[i] = c;

	return (str);
}

size_t	ft_strlen(const char *msg)
{
	size_t len = 0;
	while (*msg++)
		len++;

	return (len);
}

char	*ft_strdup(const char *s1)
{
	size_t	len = ft_strlen(s1);
	char	*cpy = (char *)malloc((len + 1) * sizeof(char));
	
	if (!cpy)
		return (NULL);
	ft_memcpy(cpy, s1, len);
	cpy[len] = '\0';

	return (cpy);
}