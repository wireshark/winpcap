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

#include <stdlib.h>
#include <stdio.h>

#include <pcap.h>
#include <remote-ext.h>

void usage();

void main(int argc, char **argv)
{
	pcap_t *indesc,*outdesc;
	char errbuf[PCAP_ERRBUF_SIZE];
	char source[PCAP_BUF_SIZE];
	FILE *capfile;
	int caplen, sync;
	u_int res;
	pcap_send_queue *squeue;
	struct pcap_pkthdr *pktheader;
	u_char *pktdata;
	float cpu_time;
	u_int npacks = 0;
	
	/* Check the validity of the command line */
	if (argc <= 2 || argc >= 5)
	{
		usage();
		return;
	}
		
	/* Retrieve the length of the capture file */
	capfile=fopen(argv[1],"rb");
	if(!capfile){
		printf("Capture file not found!\n");
		return;
	}
	
	fseek(capfile , 0, SEEK_END);
	caplen= ftell(capfile)- sizeof(struct pcap_file_header);
	fclose(capfile);
			
	/* Chek if the timestamps must be respected */
	if(argc == 4 && argv[3][0] == 's')
		sync = TRUE;
	else
		sync = FALSE;

	/* Open the capture */
	/* Create the source string according to the new WinPcap syntax */
	if ( pcap_createsrcstr(	source,			// variable that will keep the source string
							PCAP_SRC_FILE,	// we want to open a file
							NULL,			// remote host
							NULL,			// port on the remote host
							argv[1],		// name of the file we want to open
							errbuf			// error buffer
							) != 0)
	{
		fprintf(stderr,"\nError creating a source string\n");
		return;
	}
	
	/* Open the capture file */
	if ( (indesc= pcap_open(source, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errbuf) ) == NULL)
	{
		fprintf(stderr,"\nUnable to open the file %s.\n", source);
		return;
	}

	/* Open the output adapter */
	if ( (outdesc= pcap_open(argv[2], 100, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errbuf) ) == NULL)
	{
		fprintf(stderr,"\nUnable to open adapter %s.\n", source);
		return;
	}

	/* Check the MAC type */
	if (pcap_datalink(indesc) != pcap_datalink(outdesc))
	{
		printf("Warning: the datalink of the capture differs from the one of the selected interface.\n");
		printf("Press a key to continue, or CTRL+C to stop.\n");
		getchar();
	}

	/* Allocate a send queue */
	squeue = pcap_sendqueue_alloc(caplen);

	/* Fill the queue with the packets from the file */
	while ((res = pcap_next_ex( indesc, &pktheader, &pktdata)) == 1)
	{
		if (pcap_sendqueue_queue(squeue, pktheader, pktdata) == -1)
		{
			printf("Warning: packet buffer too small, not all the packets will be sent.\n");
			break;
		}

		npacks++;
	}

	if (res == -1)
	{
		printf("Corrupted input file.\n");
		pcap_sendqueue_destroy(squeue);
		return;
	}

	/* Transmit the queue */
	
	cpu_time = (float)clock ();

	if ((res = pcap_sendqueue_transmit(outdesc, squeue, sync)) < squeue->len)
	{
		printf("An error occurred sending the packets: %s. Only %d bytes were sent\n", pcap_geterr(outdesc), res);
	}
	
	cpu_time = (clock() - cpu_time)/CLK_TCK;
	
	printf ("\n\nElapsed time: %5.3f\n", cpu_time);
	printf ("\nTotal packets generated = %d", npacks);
	printf ("\nAverage packets per second = %d", (int)((double)npacks/cpu_time));
	printf ("\n");

	/* free the send queue */
	pcap_sendqueue_destroy(squeue);

	/* Close the input file */
	pcap_close(indesc);

	/* 
	 * lose the output adapter 
	 * IMPORTANT: remember to close the adapter, otherwise there will be no guarantee that all the 
	 * packets will be sent!
	 */
	pcap_close(outdesc);


	return;
}


void usage()
{
	
	printf("\nSendcap, sends a libpcap/tcpdump capture file to the net. Copyright (C) 2002 Loris Degioanni.\n");
	printf("\nUsage:\n");
	printf("\t sendcap file_name adapter [s]\n");
	printf("\nParameters:\n");
	printf("\nfile_name: the name of the dump file that will be sent to the network\n");
	printf("\nadapter: the device to use. Use \"WinDump -D\" for a list of valid devices\n");
	printf("\ns: if present, forces the packets to be sent synchronously, i.e. respecting the timestamps in the dump file. This option will work only under Windows NTx.\n\n");

	exit(0);
}