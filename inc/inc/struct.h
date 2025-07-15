/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 18:43:58 by lagea             #+#    #+#             */
/*   Updated: 2025/07/15 13:06:24 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __STRUCT_H__
# define __STRUCT_H__

#include <stdint.h>				//uint32_t
#include <sys/time.h>			//struct timeval

typedef int sockfd;

// IP header structure
struct iphdr {
    uint8_t  ihl:4, version:4;
    uint8_t  tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t check;
    uint32_t saddr;
    uint32_t daddr;
};

// UDP header structure
struct udphdr {
    uint16_t source;
    uint16_t dest;
    uint16_t len;
    uint16_t check;
};

// ICMP header structure
struct icmphdr {
	uint8_t type;		/* message type */
	uint8_t code;		/* type sub-code */
	uint16_t checksum;
	struct {
		struct {
			uint16_t id;
			uint16_t sequence;
		} echo;			/* echo datagram */
	} un;
};

typedef struct {
	struct timeval send_time;
	uint16_t dest_port;
	int ttl;
} t_probe_info;

typedef struct s_data
{
	char		*target;			// Target IP or hostname
	uint32_t	target_ip;			// Target IP address in network byte order

	sockfd		raw_socket;		// Raw socket for traceroute
	sockfd		icmp_socket;		// ICMP socket for receiving responses
} t_data;

#endif