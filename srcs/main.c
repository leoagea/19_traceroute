/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 16:34:03 by lagea             #+#    #+#             */
/*   Updated: 2025/08/27 16:35:25 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ft_traceroute.h"

t_data *g_data = NULL;

int main(int ac, char **av)
{
	if (ac != 2)
		return (usage(), EXIT_SUCCESS);
	
	if (getuid() != 0)
		exit_error("ft_traceroute: This program must be run as root to enable SOCK_RAW creation.\n");

	g_data = ft_calloc(sizeof(t_data), 1);
	if (!g_data)
		return (EXIT_FAILURE);
	
	parse_args(av);
	resolve_dns(g_data->target);
	init_sockets();
	if (run_loop() == -1){
		char buf[BUF_SIZE] = {0};
		snprintf(buf, BUF_SIZE, "ft_traceroute: %s\n", strerror(errno));
		exit_error(buf);
	}

	free_data(g_data);
	return (EXIT_SUCCESS);
}