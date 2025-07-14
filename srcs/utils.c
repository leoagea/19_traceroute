/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 18:59:34 by lagea             #+#    #+#             */
/*   Updated: 2025/07/14 19:26:24 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ft_traceroute.h"

void exit_error(const char *msg)
{
	if (msg)
		_(STDERR_FILENO, msg);
	exit(EXIT_FAILURE);
}

void usage(void)
{
	size_t len = 0;
	const char msg[BUF_SIZE] = {0};

	len = snprintf((char *)msg, BUF_SIZE, "%s",  "Usage: ft_traceroute <options> <destination>\n");
	len += snprintf((char *)msg + len, BUF_SIZE - len, "Options:\n");
	len += snprintf((char *)msg + len, BUF_SIZE - len, "\t  -h, --help       Show this help message\n");
	len += snprintf((char *)msg + len, BUF_SIZE - len, "\t  -v, --version    Show version information\n");  

	_(STDERR_FILENO, msg);
}