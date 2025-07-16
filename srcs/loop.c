/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 11:45:58 by lagea             #+#    #+#             */
/*   Updated: 2025/07/16 12:22:58 by lagea            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ft_traceroute.h"

static int receive_probe(t_probe_info *probe_info, char *hop_ip, double *rtt)
{
	fd_set read_fds;
	struct timeval timeout;
	struct timeval recv_time;
	char buffer[512];
	struct sockaddr_in from_addr;
	socklen_t from_len = sizeof(from_addr);
	
	// Set up select() with 3 second timeout
	FD_ZERO(&read_fds);
	FD_SET(g_data->icmp_socket, &read_fds);
	timeout.tv_sec = PROBE_TIMEOUT;
	timeout.tv_usec = 0;
	
	int select_result = select(g_data->icmp_socket + 1, &read_fds, NULL, NULL, &timeout);
	
	if (select_result == 0) {
		// Timeout - no response
		ft_strlcpy(hop_ip, "*", INET_ADDRSTRLEN);
		*rtt = -1.0;
		return 0;
	}
	
	if (select_result < 0) 
		return -1;
	
	ssize_t recv_bytes = recvfrom(g_data->icmp_socket, buffer, sizeof(buffer), MSG_DONTWAIT,
								  (struct sockaddr *)&from_addr, &from_len);
	if (recv_bytes < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK){
			ft_strlcpy(hop_ip, "*", INET_ADDRSTRLEN);
			*rtt = -1.0;
			return 0;
		}
		return -1;
	}
	
	gettimeofday(&recv_time, NULL);
	
	struct iphdr *ip_hdr = (struct iphdr *)buffer;
	int ip_header_len = ip_hdr->ihl * 4;
	
	struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + ip_header_len);
	
	if (icmp_hdr->type == ICMP_TIME_EXCEEDED || (icmp_hdr->type == ICMP_DEST_UNREACH && icmp_hdr->code == ICMP_PORT_UNREACH)) {
		
		struct iphdr *orig_ip = (struct iphdr *)(buffer + ip_header_len + sizeof(struct icmphdr));
		int orig_ip_len = orig_ip->ihl * 4;
		
		struct udphdr *orig_udp = (struct udphdr *)((char *)orig_ip + orig_ip_len);
		
		if (ntohs(orig_udp->dest) == probe_info->dest_port && 
			orig_ip->daddr == g_data->target_ip) {
			
			struct in_addr addr;
			addr.s_addr = from_addr.sin_addr.s_addr;
			char *ip_str = inet_ntoa(addr);
			ft_strlcpy(hop_ip, ip_str, INET_ADDRSTRLEN);
			
			double send_ms = probe_info->send_time.tv_sec * 1000.0 + probe_info->send_time.tv_usec / 1000.0;
			double recv_ms = recv_time.tv_sec * 1000.0 + recv_time.tv_usec / 1000.0;
			*rtt = recv_ms - send_ms;
			
			if (icmp_hdr->type == ICMP_TIME_EXCEEDED) {
				return 1; // Intermediate router
			} else {
				return 2; // Reached destination
			}
		}
	}
	
	// Not our probe or wrong ICMP type - ignore
	ft_strlcpy(hop_ip, "*", INET_ADDRSTRLEN);
	*rtt = -1.0;
	return 0;
}

// Updated send_probe to return probe info
static int send_probe(int ttl, t_probe_info *probe_info)
{
	char packet[sizeof(struct iphdr) + sizeof(struct udphdr) + PAYLOAD_SIZE];
	struct iphdr *ip = (struct iphdr *)packet;
	struct udphdr *udp = (struct udphdr *)(packet + sizeof(struct iphdr));
	char *payload = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
	
	// Create payload with timestamp
	struct timeval tv;
	gettimeofday(&tv, NULL);
	ft_memcpy(payload, &tv, sizeof(struct timeval));
	ft_memset(payload + sizeof(struct timeval), 0XAA, PAYLOAD_SIZE - sizeof(struct timeval));
	
	int total_len = sizeof(struct iphdr) + sizeof(struct udphdr) + PAYLOAD_SIZE;
	
	// Fill IP header
	ft_memset(ip, 0, sizeof(struct iphdr));
	ip->version = 4;
	ip->ihl = 5;
	ip->tos = 0;
	ip->tot_len = htons(total_len);
	ip->id = htons(getpid() + ttl);
	ip->frag_off = 0;
	ip->ttl = ttl;
	ip->protocol = IPPROTO_UDP;
	ip->saddr = INADDR_ANY;
	ip->daddr = g_data->target_ip;
	ip->check = 0;
	ip->check = calculate_checksum(ip, sizeof(struct iphdr));
	
	// Fill UDP header
	ft_memset(udp, 0, sizeof(struct udphdr));
	uint16_t dest_port = PROBE_PORT + ttl;
	udp->source = htons(32768 + ttl);
	udp->dest = htons(dest_port);
	udp->len = htons(sizeof(struct udphdr) + PAYLOAD_SIZE);
	udp->check = 0;
	
	struct sockaddr_in dest_addr;
	ft_memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = g_data->target_ip;
	
	// Store probe info for matching
	probe_info->dest_port = dest_port;
	probe_info->ttl = ttl;
	gettimeofday(&probe_info->send_time, NULL);
	
	ssize_t sent_bytes = sendto(g_data->raw_socket, packet, total_len, 0,
		(struct sockaddr *)&dest_addr, sizeof(dest_addr));
	
	return (sent_bytes < 0) ? -1 : 0;
}

static void print_hop_info(int ttl, char ip_arr[PROBE_NUM][INET_ADDRSTRLEN], double rtt_values[PROBE_NUM])
{
	size_t len = 0;
	char buf[BUF_SIZE] = {0};

	len = snprintf(buf, BUF_SIZE , "%2d  ", ttl);
	
	// Check if all IPs are the same (and not "*")
	bool all_same = true;
	bool has_valid_ip = false;
	
	for (int i = 0; i < PROBE_NUM; i++) {
		if (ft_strncmp(ip_arr[i], "*", INT_MAX) != 0) {
			has_valid_ip = true;
			break;
		}
	}
	
	if (has_valid_ip) {
		for (int i = 1; i < PROBE_NUM; i++) {
			if (ft_strncmp(ip_arr[0], ip_arr[i], INT_MAX) != 0) {
				all_same = false;
				break;
			}
		}
	} else
		all_same = false; // All are "*"
	
	if (all_same && has_valid_ip) {
		// All IPs are the same - print IP once, then all RTTs
		len += snprintf(buf + len, BUF_SIZE - len, "%s (%s) ", get_hostname(ip_arr[0]) ? get_hostname(ip_arr[0]) : ip_arr[0] ,ip_arr[0]);
		for (int i = 0; i < PROBE_NUM; i++) {
			if (rtt_values[i] >= 0)
				len += snprintf(buf + len, BUF_SIZE - len, "%.3f ms ", rtt_values[i]);
			else
				len += snprintf(buf + len, BUF_SIZE - len, "* ");
		}
	} else {
		// Different IPs or all timeouts - print each probe separately
		for (int i = 0; i < PROBE_NUM; i++) {
			if (ft_strncmp(ip_arr[i], "*", INT_MAX) == 0)
				len += snprintf(buf + len, BUF_SIZE - len, "* ");
			else 
				len += snprintf(buf + len, BUF_SIZE - len, "%s (%s) %.3f ms ", get_hostname(ip_arr[i]) ? get_hostname(ip_arr[i]) : ip_arr[i], ip_arr[i], rtt_values[i]);
		}
	}
	
	snprintf(buf + len, BUF_SIZE - len, "\n");
	_(STDOUT_FILENO, buf);
}

int run_loop(void)
{
	int ttl = 1;
	char buf[BUF_SIZE] = {0};

	struct in_addr addr;
    addr.s_addr = g_data->target_ip;
    char *ip_str = inet_ntoa(addr);
	snprintf(buf, BUF_SIZE, "traceroute to %s (%s), %d hops max, %ld byte packets\n", g_data->target, ip_str, MAX_TTL, PACKET_SIZE);
	_(STDOUT_FILENO, buf);
	
	while (ttl <= MAX_TTL) {
		char ip_arr[PROBE_NUM][INET_ADDRSTRLEN];
		double rtt_values[PROBE_NUM];
		bool reached_destination = false;
		
		for (int i = 0; i < PROBE_NUM; i++){
			ft_strlcpy(ip_arr[i], "*", INET_ADDRSTRLEN);
			rtt_values[i] = -1.0;
		}

		// Send all probes for this TTL and collect results
		for (int i = 0; i < PROBE_NUM; i++) {
			t_probe_info probe_info;
			char hop_ip[INET_ADDRSTRLEN];
			double rtt;
			
			ft_strlcpy(hop_ip, "*", INET_ADDRSTRLEN);
			rtt_values[i] = -1.0;
			
			if (send_probe(ttl, &probe_info) == -1)
				continue; 
			
			int result = receive_probe(&probe_info, hop_ip, &rtt);
			
			if (result == -1) {
				// Error - keep defaults
				continue;
			} else if (result > 0) {
				ft_strlcpy(ip_arr[i], hop_ip, INET_ADDRSTRLEN);
				rtt_values[i] = rtt;
				
				if (result == 2)
					reached_destination = true;
			}
		}
		
		print_hop_info(ttl, ip_arr, rtt_values);
		
		if (reached_destination)
			return 0;
		
		ttl++;
	}
	
	return 0;
}