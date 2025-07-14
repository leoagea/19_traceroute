/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 18:43:58 by lagea             #+#    #+#             */
/*   Updated: 2025/07/14 21:45:53 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __STRUCT_H__
# define __STRUCT_H__

#include <stdint.h>				//uint32_t

typedef struct s_data
{
	char		*target;			// Target IP or hostname
	uint32_t	target_ip;			// Target IP address in network byte order
} t_data;

#endif