/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 19:15:39 by lagea             #+#    #+#             */
/*   Updated: 2025/07/14 21:58:10 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ft_traceroute.h"

void free_data(t_data *data)
{
	if (data){
		if (data->target)
			free(data->target);
		if (data->raw_socket > 0)
			close(data->raw_socket);
		if (data->icmp_socket > 0)
			close(data->icmp_socket);
		free(data);
	}
}