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

main(int argc, char **argv) {
	
	pcap_if_t *alldevs, *d;
	pcap_t *fp;
	u_int inum, i=0;
	char errbuf[PCAP_ERRBUF_SIZE];

	printf("kdump: saves the network traffic to file using WinPcap kernel-level dump faeature.\n");
	printf("\t Usage: %s [adapter] | dump_file_name max_size max_packs\n", argv[0]);
	printf("\t Where: max_size is the maximum size that the dump file will reach (0 means no limit)\n");
	printf("\t Where: max_packs is the maximum number of packets that will be saved (0 means no limit)\n\n");


	if(argc < 5){

		/* The user didn't provide a packet source: Retrieve the device list */
		if (pcap_findalldevs(&alldevs, errbuf) == -1)
		{
			fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
			exit(1);
		}
		
		/* Print the list */
		for(d=alldevs; d; d=d->next)
		{
			printf("%d. %s", ++i, d->name);
			if (d->description)
				printf(" (%s)\n", d->description);
			else
				printf(" (No description available)\n");
		}
		
		if(i==0)
		{
			printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
			return -1;
		}
		
		printf("Enter the interface number (1-%d):",i);
		scanf("%d", &inum);
		
		if(inum < 1 || inum > i)
		{
			printf("\nInterface number out of range.\n");
			/* Free the device list */
			return -1;
		}
		
		/* Jump to the selected adapter */
		for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);
		
		/* Open the device */
		if ( (fp = pcap_open_live(d->name, 100, 1, 20, errbuf) ) == NULL)
		{
			fprintf(stderr,"\nError opening adapter\n");
			return -1;
		}

		/* Free the device list */
		pcap_freealldevs(alldevs);

		/* Start the dump */
		if(pcap_live_dump(fp, argv[1], atoi(argv[2]), atoi(argv[3]))==-1){
			printf("Unable to start the dump, %s\n", pcap_geterr(fp));
			return -1;
		}
	}
	else{
		
		/* Open the device */
		if ( (fp= pcap_open_live(argv[1], 100, 1, 20, errbuf) ) == NULL)
		{
			fprintf(stderr,"\nError opening adapter\n");
			return -1;
		}

		/* Start the dump */
		if(pcap_live_dump(fp, argv[0], atoi(argv[1]), atoi(argv[2]))==-1){
			printf("Unable to start the dump, %s\n", pcap_geterr(fp));
			return -1;
		}
	}

	/* Wait until the dump finishes, i.e. when  max_size or max_packs is reached*/
	pcap_live_dump_ended(fp, TRUE);
	
	/* Close the adapter, so that the file is correctly flushed */
	pcap_close(fp);

	return 0;
}
