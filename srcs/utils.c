/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 18:59:34 by lagea             #+#    #+#             */
/*   Updated: 2025/08/27 17:04:40 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ft_traceroute.h"

void free_data(t_data *data)
{
	if (data){
		if (data->target)
			free(data->target);
		if (data->raw_socket > 0)
			close(data->raw_socket);
		if (data->icmp_socket > 0)
			close(data->icmp_socket);
		free(data);
	}
}

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

	len = snprintf((char *)msg, BUF_SIZE, "%s",  "Usage: ft_traceroute <options> or <destination>\n");
	len += snprintf((char *)msg + len, BUF_SIZE - len, "Destinations:\n");
	len += snprintf((char *)msg + len, BUF_SIZE - len, "\t  <hostname> or <IPv4 address>\n");
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

char* get_hostname(const char* ip_str) 
{
	static char hostname[256];
	struct sockaddr_in sa;
	
	if (inet_pton(AF_INET, ip_str, &sa.sin_addr) != 1)
		return NULL;
	
	sa.sin_family = AF_INET;
	
	// Reverse DNS lookup
	if (getnameinfo((struct sockaddr*)&sa, sizeof(sa), hostname, sizeof(hostname), NULL, 0, 0) == 0)
		return hostname;
	
	return NULL; // Lookup failed
}

uint16_t calculate_checksum(void *data, int len)
{
	uint16_t *ptr = (uint16_t *)data;
	uint32_t sum = 0;
	
	while (len > 1) {
		sum += *ptr++;
		len -= 2;
	}
	
	if (len == 1)
		sum += *(uint8_t*)ptr;
	
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);
	
	return (uint16_t)(~sum);
}

