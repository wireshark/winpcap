#include <windows.h>
#include <stdio.h>

DWORD
delete_service
(
			   FILE* log,
			   LPCTSTR ServiceName
)
{
	SC_HANDLE SCM_Handle;
	SC_HANDLE ServiceHandle;
	SERVICE_STATUS ServiceStatus;
	DWORD LastError;
	DWORD ReturnValue;

	SCM_Handle=OpenSCManager(NULL,  /*local machine  */
		NULL,						/*active database*/
		SC_MANAGER_ALL_ACCESS);

	if (SCM_Handle==NULL)
	{
		LastError = GetLastError();
		if (log != NULL)	
		{
			fprintf(log,"Error opening Service Control Manager: %8.8x\n", LastError);
		}
		return LastError;
	}

	ServiceHandle=OpenService(SCM_Handle,
					ServiceName,
					SERVICE_ALL_ACCESS);


	if (ServiceHandle==NULL)
	{
		LastError = GetLastError();
		if (log != NULL)
		{
			fprintf(log,"Error opening Service Control Manager: %8.8x\n", LastError);
		}
		
		if (!CloseServiceHandle(SCM_Handle))
		{
			LastError = GetLastError();
			if (log != NULL)
			{
				fprintf(log,"Error closing Service control Manager: %8.8x\n", LastError);
			}
		}
		
		return LastError;
	}

	ReturnValue= NO_ERROR;

	if (!ControlService(ServiceHandle,SERVICE_CONTROL_STOP,&ServiceStatus))
	{
		LastError = GetLastError();

		if (LastError != ERROR_SERVICE_NOT_ACTIVE)
		{
			if (log != NULL)
			{
				fprintf(log,"Error stopping service %s: %8.8x\n",LastError);
			}

			ReturnValue= LastError;
		}
	}
	else
		if (log != NULL)
		{
			fprintf(log,"Service %s successfully stopped\n",ServiceName);
		}

	if (!DeleteService(ServiceHandle))
	{
		LastError = GetLastError();

		if (log != NULL)
		{
			fprintf(log,"Error deleting service %s: %8.8x",ServiceName, LastError);
		}

		ReturnValue= LastError;
		
	}
	else
	{
		if (log != NULL)
		{
			fprintf(log,"Service %s successfully deleted\n",ServiceName);
		}
	}
	
	if (!CloseServiceHandle(ServiceHandle))
	{
		LastError = GetLastError();

		if (log != NULL)
		{
			fprintf(log,"Error closing service %s: %8.8x",ServiceName, LastError);
		}

		ReturnValue=LastError;
	}


	if (!CloseServiceHandle(SCM_Handle))
	{
		LastError = GetLastError();
		if (log != NULL)
		{
			fprintf(log,"Error closing Service control Manager: %8.8x\n", LastError);
		}
		
		ReturnValue=LastError;
	}
	
	return ReturnValue;
}

DWORD 
stop_service
(
	FILE* log,
	LPCTSTR ServiceName
)
{
	SC_HANDLE SCM_Handle;
	SC_HANDLE ServiceHandle;
	SERVICE_STATUS ServiceStatus;
	DWORD ReturnValue;
	DWORD LastError;

	SCM_Handle=OpenSCManager(NULL,  /*local machine  */
		NULL,						/*active database*/
		SC_MANAGER_ALL_ACCESS);

	if (SCM_Handle==NULL)
	{
		LastError = GetLastError();

		if (log !=NULL)
		{
			fprintf(log,"Error opening Service Control Manager: %8.8x", LastError);
		}
		
		return LastError;
	}

	ServiceHandle=OpenService(SCM_Handle,
					ServiceName,
					SERVICE_ALL_ACCESS);


	if (ServiceHandle == NULL)
	{
		LastError = GetLastError();
		if (log != NULL)
		{
			fprintf(log,"Error opening service %s: %8.8x",LastError);
		}

		if (!CloseServiceHandle(SCM_Handle))
			if (log != NULL)		
				fprintf(log,"Error closing Service control Manager\n");
					
		return LastError;
	}

	ReturnValue=NO_ERROR;

	if (!ControlService(ServiceHandle,SERVICE_CONTROL_STOP,&ServiceStatus))
	{
		LastError = GetLastError();
		if (log != NULL)
		{
			fprintf(log,"Error stopping service %s: %8.8x",ServiceName, LastError);
		}
		ReturnValue = LastError;
		
	}
	else
		if (log != NULL)
			fprintf(log,"Service %s successfully stopped\n",ServiceName);
	
	if (!CloseServiceHandle(ServiceHandle))
	{
		LastError = GetLastError();

		if (log != NULL)
		{
			fprintf(log,"Error closing service %s: %8.8\n",ServiceName, LastError);
		}
		
		ReturnValue = LastError;
	}

	if (!CloseServiceHandle(SCM_Handle))
	{
		LastError = GetLastError();
		if (log != NULL)
		{
			fprintf(log,"Error closing Service control Manager: %8.8x\n", LastError);
		}
		ReturnValue=LastError;
	}
	
	return ReturnValue;

}


DWORD
change_start_type_service
(
	FILE* log,
	LPCTSTR ServiceName, 
	DWORD StartType
)
{
	SC_HANDLE SCM_Handle;
	SC_HANDLE ServiceHandle;
	DWORD ReturnValue;
	DWORD LastError;

	SCM_Handle=OpenSCManager(NULL,  /*local machine  */
		NULL,						/*active database*/
		SC_MANAGER_ALL_ACCESS);

	if (SCM_Handle == NULL)
	{
		LastError = GetLastError();
		if (log != NULL)
		{
			fprintf(log,"Error opening Service Control Manager: %8.8x", LastError);
		}
		return LastError;
	}

	ServiceHandle=OpenService(SCM_Handle,
					ServiceName,
					SERVICE_ALL_ACCESS);


	if (ServiceHandle == NULL)
	{
		LastError = GetLastError();

		if (log != NULL)
		{
			fprintf(log,"Error opening service %s: %8.8x",ServiceName, LastError);
		}

		if (!CloseServiceHandle(SCM_Handle))
			if (log != NULL)
				fprintf(log,"Error closing Service control Manager\n");
					
		return LastError;
	}

	ReturnValue=0;

	if (!ChangeServiceConfig(ServiceHandle,
		SERVICE_NO_CHANGE,
		StartType,
		SERVICE_NO_CHANGE,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL))
	{
		LastError = GetLastError();

		if (log != NULL)
		{
			fprintf(log, "Error changing start type for service %s: %8.8x\n",ServiceName, LastError);
		}

		ReturnValue=LastError;
		
	}
	else
		if (log != NULL)
			fprintf(log,"Successfully changed start-type for service %s\n",ServiceName);
	
	if (!CloseServiceHandle(ServiceHandle))
	{
		LastError = GetLastError();

		if (log != NULL)
		{
			fprintf(log,"Error closing service %s: %8.8x\n",ServiceName, LastError);
		}
		
		ReturnValue= LastError;
	}


	if (!CloseServiceHandle(SCM_Handle))
	{
		LastError = GetLastError();

		if (log != NULL)
		{
			fprintf(log,"Error closing Service control Manager: %8.8x\n", LastError);
		}

		ReturnValue= LastError;
	}
	
	return ReturnValue;

}


DWORD
start_service
(
	FILE* log,
	LPCTSTR ServiceName
)
{
	SC_HANDLE SCM_Handle;
	SC_HANDLE ServiceHandle;
	DWORD ReturnValue;
	DWORD LastError;

	SCM_Handle=OpenSCManager(NULL,  /*local machine  */
		NULL,						/*active database*/
		SC_MANAGER_ALL_ACCESS);

	if (SCM_Handle == NULL)
	{
		LastError = GetLastError();

		if (log != NULL)
		{
			fprintf(log,"Error opening Service Control Manager: %8.8x\n", LastError);
		}
		return LastError;
	}

	ServiceHandle=OpenService(SCM_Handle,
					ServiceName,
					SERVICE_ALL_ACCESS);


	if (ServiceHandle == NULL)
	{
		LastError = GetLastError();
		if (log != NULL)
		{
			fprintf(log,"Error opening service %s: %8.8x\n",ServiceName, LastError);
		}

		if (!CloseServiceHandle(SCM_Handle))
		{
			LastError = GetLastError();
			if (log != NULL)
			{
				fprintf(log,"Error closing Service control Manager: %8.8x\n", LastError);
			}
		}
					
		return LastError;
	}

	ReturnValue=0;

	if (!StartService(ServiceHandle,0,NULL))
	{
		LastError = GetLastError();
		if (log != NULL)
		{
			fprintf(log, "Error starting service %s: %8.8x\n",ServiceName, LastError);
		}
		ReturnValue = LastError;
		
	}
	else
		if (log != NULL)
		{
			fprintf(log,"Service %s successfully started\n",ServiceName);
		}
	
	if (!CloseServiceHandle(ServiceHandle))
	{
		LastError = GetLastError();
		if (log != NULL)
		{
			fprintf(log,"Error closing service %s: %8.8x\n",ServiceName, LastError);
		}

		ReturnValue = LastError;
	}

	if (!CloseServiceHandle(SCM_Handle))
	{
		LastError = GetLastError();
		if (log != NULL)
		{
			fprintf(log,"Error closing Service control Manager: %8.8x\n", LastError);
		}
		ReturnValue = LastError;
	}
	
	return ReturnValue;

}



int 
create_service
(
	FILE* log,
	LPCTSTR ServiceName,
	LPCTSTR ServiceDescriptionShort,
	LPCTSTR ServiceDescriptionLong,
	LPCTSTR ServicePath,
	DWORD ServiceType)
{
	SC_HANDLE SCM_Handle;
	SC_HANDLE ServiceHandle;
	DWORD ReturnValue;
	SERVICE_DESCRIPTION ServiceDescription;
	HMODULE hModule;
	MyChangeServiceConfig2 fMyChangeServiceConfig2;


	SCM_Handle=OpenSCManager(NULL,  /*local machine  */
		NULL,						/*active database*/
		SC_MANAGER_ALL_ACCESS);

	if (SCM_Handle==NULL)
	{

		if (log != NULL)
		{
			fprintf(log,"Error opening Service Control Manager: ");
			DisplayErrorText(GetLastError(),log);
		}
		
		return -1;
	}


	ServiceHandle=CreateService(SCM_Handle,
					ServiceName,
					ServiceDescriptionShort,
					SERVICE_ALL_ACCESS,
					SERVICE_TYPE,
					SERVICE_DEMAND_START,
					SERVICE_ERROR_NORMAL,
					ServicePath,
					NULL,
					NULL,
					"",
					NULL,
					NULL);

	if (ServiceHandle==NULL)
	{

		if (log != NULL)
		{
			fprintf(log,"Error creating service %s: ",ServiceName);
			DisplayErrorText(GetLastError(),log);
		}

		if (!CloseServiceHandle(SCM_Handle))
			if (log != NULL)
				fprintf(log,"Error closing Service control Manager\n");
					
		return -1;
	}

	if (log != NULL)
		fprintf(log,"Service %s successfully created.\n",ServiceName);

	//the following hack is necessary because the ChangeServiceConfig2 is not defined on NT4, so 
	//we cannot statically import ChangeServiceConfig2 from advapi32.dll, we have to load it dynamically

	hModule = LoadLibrary("advapi32.dll");
	
	if (hModule != NULL)
	{
		fMyChangeServiceConfig2 = (MyChangeServiceConfig2)GetProcAddress(hModule,"ChangeServiceConfig2A");
		if (fMyChangeServiceConfig2 != NULL)
		{
	
			ServiceDescription.lpDescription = (LPTSTR)ServiceDescriptionLong;

			if (!fMyChangeServiceConfig2(ServiceHandle,SERVICE_CONFIG_DESCRIPTION,&ServiceDescription))
			{
				if (log != NULL)
				{
					fprintf(log,"Error setting service description: ");
					DisplayErrorText(GetLastError(),log);
				}
			}
		}
		FreeLibrary(hModule);
	}

	
	ReturnValue=0;

	if (!CloseServiceHandle(ServiceHandle))
	{
		if (log != NULL)
			fprintf(log,"Error closing service %s.\n",ServiceName);
		ReturnValue=-1;
	}


	if (!CloseServiceHandle(SCM_Handle))
	{
		if (log != NULL)
			fprintf(log,"Error closing Service control Manager\n");
		ReturnValue=-1;
	}
	
	return ReturnValue;

}

