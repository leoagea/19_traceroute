/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lagea < lagea@student.s19.be >             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 11:45:58 by lagea             #+#    #+#             */
/*   Updated: 2025/07/15 15:13:25 by lagea            ###   ########.fr       */
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
		strcpy(hop_ip, "*");
		*rtt = -1.0;
		return 0;
	}
	
	if (select_result < 0) {
		// Error in select
		return -1;
	}
	
	// Data available - receive immediately and timestamp
	ssize_t recv_bytes = recvfrom(g_data->icmp_socket, buffer, sizeof(buffer), MSG_DONTWAIT,
								  (struct sockaddr *)&from_addr, &from_len);
	if (recv_bytes < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK){
			strcpy(hop_ip, "*");
			*rtt = -1.0;
			return 0;
		}
		return -1;
	}
	
	gettimeofday(&recv_time, NULL);
	
	// Parse the received IP packet
	struct iphdr *ip_hdr = (struct iphdr *)buffer;
	int ip_header_len = ip_hdr->ihl * 4;
	
	// Get ICMP header (after IP header)
	struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + ip_header_len);
	
	// Check ICMP type
	if (icmp_hdr->type == ICMP_TIME_EXCEEDED || 
		(icmp_hdr->type == ICMP_DEST_UNREACH && icmp_hdr->code == ICMP_PORT_UNREACH)) {
		
		// Get the original IP header from ICMP payload
		struct iphdr *orig_ip = (struct iphdr *)(buffer + ip_header_len + sizeof(struct icmphdr));
		int orig_ip_len = orig_ip->ihl * 4;
		
		// Get the original UDP header
		struct udphdr *orig_udp = (struct udphdr *)((char *)orig_ip + orig_ip_len);
		
		// Verify this is our probe by checking destination port and target IP
		if (ntohs(orig_udp->dest) == probe_info->dest_port && 
			orig_ip->daddr == g_data->target_ip) {
			
			// Extract source IP (the router that sent the ICMP)
			inet_ntop(AF_INET, &from_addr.sin_addr, hop_ip, INET_ADDRSTRLEN);
			
			// Calculate RTT in milliseconds
			double send_ms = probe_info->send_time.tv_sec * 1000.0 + 
						   probe_info->send_time.tv_usec / 1000.0;
			double recv_ms = recv_time.tv_sec * 1000.0 + 
						   recv_time.tv_usec / 1000.0;
			*rtt = recv_ms - send_ms;
			
			// Return different codes for different ICMP types
			if (icmp_hdr->type == ICMP_TIME_EXCEEDED) {
				return 1; // Intermediate router
			} else {
				return 2; // Reached destination
			}
		}
	}
	
	// Not our probe or wrong ICMP type - ignore
	strcpy(hop_ip, "*");
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
	printf("%2d  ", ttl);
	
	// Check if all IPs are the same (and not "*")
	bool all_same = true;
	bool has_valid_ip = false;
	
	for (int i = 0; i < PROBE_NUM; i++) {
		if (strcmp(ip_arr[i], "*") != 0) {
			has_valid_ip = true;
			break;
		}
	}
	
	if (has_valid_ip) {
		for (int i = 1; i < PROBE_NUM; i++) {
			if (strcmp(ip_arr[0], ip_arr[i]) != 0) {
				all_same = false;
				break;
			}
		}
	} else {
		all_same = false; // All are "*"
	}
	
	if (all_same && has_valid_ip) {
		// All IPs are the same - print IP once, then all RTTs
		char *host = get_hostname(ip_arr[0]);
		printf("%s (%s) ", host ? host : ip_arr[0] ,ip_arr[0]);
		for (int i = 0; i < PROBE_NUM; i++) {
			if (rtt_values[i] >= 0) {
				printf("%.3f ms ", rtt_values[i]);
			} else {
				printf("* ");
			}
		}
	} else {
		// Different IPs or all timeouts - print each probe separately
		for (int i = 0; i < PROBE_NUM; i++) {
			if (strcmp(ip_arr[i], "*") == 0) {
				printf("* ");
			} else {
				char *host = get_hostname(ip_arr[i]);
				printf("%s (%s) %.3f ms ", host ? host : ip_arr[i], ip_arr[i], rtt_values[i]);
			}
		}
	}
	
	printf("\n");
}

int run_loop(void)
{
	int ttl = 1;

	while (ttl <= MAX_TTL) {
		char ip_arr[PROBE_NUM][INET_ADDRSTRLEN];
		double rtt_values[PROBE_NUM];
		bool reached_destination = false;
		
		// Send all probes for this TTL and collect results
		for (int i = 0; i < PROBE_NUM; i++) {
			t_probe_info probe_info;
			char hop_ip[INET_ADDRSTRLEN];
			double rtt;
			
			// Initialize defaults
			strcpy(ip_arr[i], "*");
			rtt_values[i] = -1.0;
			
			// Send probe
			if (send_probe(ttl, &probe_info) == -1) {
				continue; // Keep defaults for failed send
			}
			
			// Receive response
			int result = receive_probe(&probe_info, hop_ip, &rtt);
			
			if (result == -1) {
				// Error - keep defaults
				continue;
			} else if (result > 0) {
				// Got a valid response
				strcpy(ip_arr[i], hop_ip);
				rtt_values[i] = rtt;
				
				if (result == 2) {
					reached_destination = true;
				}
			}
			// If result == 0, it's a timeout - keep defaults
		}
		
		// Print all results for this hop
		print_hop_info(ttl, ip_arr, rtt_values);
		
		// Check if we reached destination
		if (reached_destination) {
			printf("\nReached destination!\n");
			return 0;
		}
		
		ttl++;
	}
	
	return 0;
}