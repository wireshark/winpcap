/*
 * Copyright (c) 1999 - 2002
 *	Politecnico di Torino.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the Politecnico
 * di Torino, and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/* Definitions */
struct pcap_send_queue{
	u_int maxlen;
	u_int len;
	char *buffer;
};

typedef struct pcap_send_queue pcap_send_queue;

/* Prototypes */
pcap_send_queue* pcap_sendqueue_alloc(u_int memsize);
void pcap_sendqueue_destroy(pcap_send_queue* queue);
int pcap_sendqueue_queue(pcap_send_queue* queue, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);
u_int pcap_sendqueue_transmit(pcap_t *p, pcap_send_queue* queue, int sync);
HANDLE pcap_getevent(pcap_t *p);
int pcap_read_ex(pcap_t *p, struct pcap_pkthdr **pkt_header, u_char **pkt_data);
int pcap_setuserbuffer(pcap_t *p, int size);
