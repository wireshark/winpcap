#include "../WinPcapInstallerHelper.h"
#include <stdio.h>

int main()
{
	int i;
	
	printf("Testing netmon installer");
	
	i = manage_netmon("log.txt", 'u');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_netmon("log.txt", 'i');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_netmon(NULL, 'u');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_netmon(NULL, 'i');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_netmon(NULL, '8');
	if (i == 0)
		printf("X");
	else
		printf(".");

	printf("DONE\n");

	printf("Testing npf installer");
	
	i = manage_npf_driver("log.txt", 'u');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_npf_driver("log.txt", 'i');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_npf_driver("log.txt", 'r');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_npf_driver("log.txt", 's');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_npf_driver("log.txt", 'x');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_npf_driver("log.txt", 'a');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_npf_driver("log.txt", 'd');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_npf_driver(NULL, 'u');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_npf_driver(NULL, 'i');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_npf_driver(NULL, 'r');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_npf_driver(NULL, 's');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_npf_driver(NULL, 'x');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_npf_driver(NULL, 'a');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_npf_driver(NULL, 'd');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_npf_driver(NULL, '8');
	if (i == 0)
		printf("X");
	else
		printf(".");
	
	printf("DONE\n");

	printf("Testing the rpcapd installer");

	i = manage_rpcapd_service("log.txt", 'u');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_rpcapd_service("log.txt", 'i');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_rpcapd_service("log.txt", 'r');
	if (i != 0)
		printf("X");
	else
		printf(".");

	//
	// note: due to some sort of bug in the rpcapd
	// service code, the service doesn't seem to start/stop
	// using the SCM or net start rpcapd..
	// so this call fails
	//
	i = manage_rpcapd_service("log.txt", 's');
	if (i != 0)
		printf("X");
	else
		printf(".");

	//
	// note: due to some sort of bug in the rpcapd
	// service code, the service doesn't seem to start/stop
	// using the SCM or net start rpcapd..
	// so this call fails
	//
	i = manage_rpcapd_service("log.txt", 'x');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_rpcapd_service("log.txt", 'a');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_rpcapd_service("log.txt", 'd');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_rpcapd_service(NULL, 'u');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_rpcapd_service(NULL, 'i');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_rpcapd_service(NULL, 'r');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_rpcapd_service(NULL, 's');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_rpcapd_service(NULL, 'x');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_rpcapd_service(NULL, 'a');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_rpcapd_service(NULL, 'd');
	if (i != 0)
		printf("X");
	else
		printf(".");

	i = manage_rpcapd_service(NULL, '9');
	
	if (i == 0)
		printf("X");
	else
		printf(".");
	
	printf("DONE\n");
	

	
	
	return 0;

}