/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 18:51:53 by lagea             #+#    #+#             */
/*   Updated: 2025/07/14 19:10:36 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ft_traceroute.h"

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

size_t	ft_strlen(const char *msg)
{
	size_t len = 0;
	while (*msg++)
		len++;

	return (len);
}
