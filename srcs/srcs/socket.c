/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 21:49:12 by lagea             #+#    #+#             */
/*   Updated: 2025/07/15 11:43:01 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ft_traceroute.h"

void init_sockets(void)
{
	g_data->raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (g_data->raw_socket < 0) {
		exit_error("ft_traceroute: raw socket creation failed");
	}

	g_data->icmp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (g_data->icmp_socket < 0) {
		close(g_data->raw_socket);
		exit_error("ft_traceroute: ICMP socket creation failed");
	}

	// Enable IP_HDRINCL to manually craft IP headers
	int hdrincl = 1;
	if (setsockopt(g_data->raw_socket, IPPROTO_IP, IP_HDRINCL, &hdrincl, sizeof(hdrincl)) < 0) {
		close(g_data->raw_socket);
		close(g_data->icmp_socket);
		exit_error("ft_traceroute: setsockopt IP_HDRINCL failed");
	}

	// Set initial TTL (will be modified for each hop)
	int ttl = 1;
	if (setsockopt(g_data->raw_socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
		close(g_data->raw_socket);
		close(g_data->icmp_socket);
		exit_error("ft_traceroute: setsockopt TTL failed");
	}

	// Set receive timeout for ICMP socket
	struct timeval timeout = { .tv_sec = 5, .tv_usec = 0 };
	if (setsockopt(g_data->icmp_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		close(g_data->raw_socket);
		close(g_data->icmp_socket);
		exit_error("ft_traceroute: setsockopt timeout failed");
	}

	// Bind ICMP socket to receive all ICMP packets
	struct sockaddr_in icmp_addr;
	ft_memset(&icmp_addr, 0, sizeof(icmp_addr));
	icmp_addr.sin_family = AF_INET;
	icmp_addr.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(g_data->icmp_socket, (struct sockaddr*)&icmp_addr, sizeof(icmp_addr)) < 0) {
		close(g_data->raw_socket);
		close(g_data->icmp_socket);
		exit_error("ft_traceroute: ICMP socket bind failed");
	}
}