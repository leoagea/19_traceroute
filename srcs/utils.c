/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 18:59:34 by lagea             #+#    #+#             */
/*   Updated: 2025/07/14 21:35:14 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ft_traceroute.h"

void exit_error(const char *msg)
{
	if (msg)
		_(STDERR_FILENO, msg);
	free_data(g_data);
	exit(EXIT_FAILURE);
}

void usage(void)
{
	size_t len = 0;
	const char msg[BUF_SIZE] = {0};

	len = snprintf((char *)msg, BUF_SIZE, "%s",  "Usage: ft_traceroute <options> <destination>\n");
	snprintf((char *)msg + len, BUF_SIZE - len, "%s", "Use --help for more info\n");

	_(STDERR_FILENO, msg);
}

void help(void)
{
	size_t len = 0;
	const char msg[BUF_SIZE] = {0};

	len = snprintf((char *)msg, BUF_SIZE, "%s",  "Usage: ft_traceroute <options> <destination>\n");
	len += snprintf((char *)msg + len, BUF_SIZE - len, "Options:\n");
	len += snprintf((char *)msg + len, BUF_SIZE - len, "\t  -h, --help       Show this help message\n");
	len += snprintf((char *)msg + len, BUF_SIZE - len, "\t  -v, --version    Show version information\n");  

	_(STDERR_FILENO, msg);
}

void print_gai_error(const char *target, int status)
{
	char buf[BUF_SIZE] = {0};

	snprintf(buf, BUF_SIZE, "ft_traceroute: getaddrinfo: %s: %s\n", target, gai_strerror(status));
	_(STDERR_FILENO, buf);
}