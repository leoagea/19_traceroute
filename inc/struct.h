/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 18:43:58 by lagea             #+#    #+#             */
/*   Updated: 2025/07/14 21:54:45 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __STRUCT_H__
# define __STRUCT_H__

#include <stdint.h>				//uint32_t

typedef int sockfd;

typedef struct s_data
{
	char		*target;			// Target IP or hostname
	uint32_t	target_ip;			// Target IP address in network byte order

	sockfd		raw_socket;		// Raw socket for traceroute
	sockfd		icmp_socket;		// ICMP socket for receiving responses
} t_data;

#endif