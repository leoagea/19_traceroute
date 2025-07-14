/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_traceroute.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 18:44:28 by lagea             #+#    #+#             */
/*   Updated: 2025/07/14 21:59:04 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __FT_TRACEROUTE_H__
# define __FT_TRACEROUTE_H__

/*#############################################################################
# Includes
#############################################################################*/

#include "struct.h"

#include <unistd.h>				//write
#include <stdlib.h>				//malloc, free
#include <stdio.h>				//snprintf
#include <signal.h>				//sigaction, SIGINT, SIGTERM	
#include <errno.h>				//errno, strerror
#include <string.h>				//strerror
#include <limits.h>				//INT_MAX
#include <netdb.h>				//getaddrinfo
#include <stdbool.h>			//bool
#include <arpa/inet.h>			//inet_pton, sockaddr_in

/*#############################################################################
# Global Variables
#############################################################################*/

extern t_data *g_data;

/*#############################################################################
# Defines
#############################################################################*/

#define BUF_SIZE 1024

/*#############################################################################
# Macros
#############################################################################*/

#define _(fd, str) do { ssize_t unused = write(fd, str, ft_strlen(str)); (void) unused;} while (0)

/*#############################################################################
# Parse.c
#############################################################################*/

int		parse_args(char **av);
void	resolve_dns(const char *target);

/*#############################################################################
# Socket.c
#############################################################################*/

void	init_sockets(void);

/*#############################################################################
# Utils.c
#############################################################################*/

void	exit_error(const char *msg);
void	usage(void);
void	help(void);
void	print_gai_error(const char *target, int status);

/*#############################################################################
# Helper.c
#############################################################################*/

int		ft_strncmp(const char *s1, const char *s2, size_t n);
void	*ft_calloc(size_t count, size_t size);
void	*ft_memcpy(void *dst, const void *src, size_t n);
void	*ft_memset(void *str, int c, size_t n);
size_t	ft_strlen(const char *msg);
char	*ft_strdup(const char *s1);

/*#############################################################################
# Free.c
#############################################################################*/

void	free_data(t_data *data);

#endif