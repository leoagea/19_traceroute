/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 20:51:48 by lagea             #+#    #+#             */
/*   Updated: 2025/08/28 12:56:33 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ft_traceroute.h"

int parse_args(char **av)
{
	if (ft_strncmp(av[1], "--help", INT_MAX) == 0 || ft_strncmp(av[1], "-h", INT_MAX) == 0){
		help();
		free_data(g_data);
		exit(EXIT_SUCCESS);
	}
	else if (ft_strncmp(av[1], "--version", INT_MAX) == 0 || ft_strncmp(av[1], "-v", INT_MAX) == 0){
		_(STDOUT_FILENO, "ft_traceroute version 1.0.0\n");
		free_data(g_data);
		exit(EXIT_SUCCESS);
	}

	g_data->target = ft_strdup(av[1]);
	if (!g_data->target)
		exit_error("ft_traceroute: Memory allocation failed for target.\n");

	return (EXIT_SUCCESS);
}

void resolve_dns(const char *target)
{
	size_t status = 0;
	struct addrinfo hints, *res;

	ft_memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_flags = AI_CANONNAME;
	
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