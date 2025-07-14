/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 20:51:48 by lagea             #+#    #+#             */
/*   Updated: 2025/07/14 21:59:11 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ft_traceroute.h"

int parse_args(char **av)
{
	if (ft_strncmp(av[1], "--help", INT_MAX) == 0 || ft_strncmp(av[1], "-h", INT_MAX) == 0){
		help();
		return (EXIT_SUCCESS);
	}
	else if (ft_strncmp(av[1], "--version", INT_MAX) == 0 || ft_strncmp(av[1], "-v", INT_MAX) == 0){
		_(STDOUT_FILENO, "ft_traceroute version 1.0.0\n");
		return (EXIT_SUCCESS);
	}

	g_data->target = ft_strdup(av[1]);
	if (!g_data->target)
		exit_error("ft_traceroute: Memory allocation failed for target.\n");

	return (EXIT_SUCCESS);
}

// static bool is_valid_ip4(const char *ip)
// {
// 	struct sockaddr_in sa;
// 	int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
// 	return (result == 1);
// }

void resolve_dns(const char *target)
{
	size_t status = 0;
	struct addrinfo hints, *res;

	ft_memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_RAW; // Raw socket for traceroute
	hints.ai_flags = AI_CANONNAME; // Get canonical name
	
	status = getaddrinfo(target, NULL, &hints, &res);
	if (status != 0){
		print_gai_error(target, status);
		exit_error(NULL);
	}

	struct addrinfo *p;
	for (p = res; p != NULL; p = p->ai_next) {
		if (p->ai_family == AF_INET) {
			struct sockaddr_in *addr_in = (struct sockaddr_in *)p->ai_addr;
            g_data->target_ip = addr_in->sin_addr.s_addr;
			freeaddrinfo(res);
			return ;
		}
	}
	
	freeaddrinfo(res);
	exit_error("ft_traceroute: Invalid target IP or hostname.\n");
}