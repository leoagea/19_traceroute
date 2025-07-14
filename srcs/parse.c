/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 20:51:48 by lagea             #+#    #+#             */
/*   Updated: 2025/07/14 21:17:18 by lagea            ###   ########.fr       */
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