/*
 * Copyright (c) 1999, 2000
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

#include <stdarg.h>
#include <ntddk.h>
#include <ntiologc.h>
#include <ndis.h>
#include "debug.h"
#include "packet.h"

#include "win_bpf.h"

//-------------------------------------------------------------------

NTSTATUS
PacketOpenDumpFile(POPEN_INSTANCE Open , PUNICODE_STRING fileName, BOOLEAN Append)
{
	NTSTATUS ntStatus;
	IO_STATUS_BLOCK IoStatus;
	OBJECT_ATTRIBUTES ObjectAttributes;
	PWCHAR PathPrefix;
	USHORT PathLen;
	UNICODE_STRING FullFileName;
	ULONG FullFileNameLength;
	PDEVICE_OBJECT fsdDevice;

	FILE_STANDARD_INFORMATION StandardInfo;
	
    IF_LOUD(DbgPrint("NPF: OpenDumpFile.\n");)

	if(fileName->Buffer[0] == L'\\' &&
		fileName->Buffer[1] == L'?' &&
		fileName->Buffer[2] == L'?' &&
		fileName->Buffer[3] == L'\\'
	){
		PathLen = 0;
	}
	else{
		PathPrefix = L"\\??\\";
		PathLen = 8;
	}
	
	// Insert the correct path prefix.
	FullFileNameLength = PathLen + fileName->MaximumLength;
	
	FullFileName.Buffer = ExAllocatePool(NonPagedPool,
		FullFileNameLength);
	
	if (FullFileName.Buffer == NULL) {
		ntStatus = STATUS_INSUFFICIENT_RESOURCES;
		return ntStatus;
	}
	
	FullFileName.Length = PathLen;
	FullFileName.MaximumLength = (USHORT)FullFileNameLength;
	
	if(PathLen)
		RtlMoveMemory (FullFileName.Buffer, PathPrefix, PathLen);
	
	RtlAppendUnicodeStringToString (&FullFileName, fileName);
	
	IF_LOUD(DbgPrint( "Packet: Attempting to open %wZ\n", &FullFileName);)
	
	InitializeObjectAttributes ( &ObjectAttributes,
		&FullFileName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL );
	
	// Create the dump file
	ntStatus = ZwCreateFile( &Open->DumpFileHandle,
		SYNCHRONIZE | FILE_WRITE_DATA,
		&ObjectAttributes,
		&IoStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ,
		(Append)?FILE_OPEN_IF:FILE_SUPERSEDE,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0 );
	
    if ( !NT_SUCCESS( ntStatus ) )
    {
        IF_LOUD(DbgPrint("NPF: Error opening file %x\n", ntStatus);)
		
        ExFreePool(FullFileName.Buffer);
		Open->DumpFileHandle=NULL;
        ntStatus = STATUS_NO_SUCH_FILE;
        return ntStatus;
    }
	
	ExFreePool(FullFileName.Buffer);
	
	ntStatus = ObReferenceObjectByHandle(Open->DumpFileHandle,
		FILE_WRITE_ACCESS,
		*IoFileObjectType,
		KernelMode,
		&Open->DumpFileObject,
		0);
	
    if ( !NT_SUCCESS( ntStatus ) )
    {
        IF_LOUD(DbgPrint("NPF: Error creating file, status=%x\n", ntStatus);)
			
		ZwClose( Open->DumpFileHandle );
		Open->DumpFileHandle=NULL;
		
        ntStatus = STATUS_NO_SUCH_FILE;
        return ntStatus;
    }
	
    fsdDevice = IoGetRelatedDeviceObject(Open->DumpFileObject);

	IF_LOUD(DbgPrint("NPF: Dump: write file created succesfully, status=%d \n",ntStatus);)

	return ntStatus;
}	

//-------------------------------------------------------------------

NTSTATUS
PacketStartDump(POPEN_INSTANCE Open)
{
	NTSTATUS ntStatus;
	struct packet_file_header hdr;
	IO_STATUS_BLOCK IoStatus;
    NDIS_REQUEST pRequest;
	ULONG MediaType;
	OBJECT_ATTRIBUTES ObjectAttributes;

    IF_LOUD(DbgPrint("NPF: StartDump.\n");)

	// Init the file header
	hdr.magic = TCPDUMP_MAGIC;
	hdr.version_major = PCAP_VERSION_MAJOR;
	hdr.version_minor = PCAP_VERSION_MINOR;
	hdr.thiszone = 0; /*Currently not set*/
	hdr.snaplen = 1514;
	hdr.sigfigs = 0;

	// Detect the medium type
	switch (Open->Medium){
		
	case NdisMediumWan:
		hdr.linktype = DLT_EN10MB;
		break;
		
	case NdisMedium802_3:
		hdr.linktype = DLT_EN10MB;
		break;
		
	case NdisMediumFddi:
		hdr.linktype = DLT_FDDI;
		break;
		
	case NdisMedium802_5:			
		hdr.linktype = DLT_IEEE802;	
		break;
		
	case NdisMediumArcnet878_2:
		hdr.linktype = DLT_ARCNET;
		break;
		
	case NdisMediumAtm:
		hdr.linktype = DLT_ATM_RFC1483;
		break;
		
	default:
		hdr.linktype = DLT_EN10MB;
	}

	// Write the header.
	// We can use ZwWriteFile because we are in the context of the application
	ntStatus = ZwWriteFile(Open->DumpFileHandle,
		NULL,
		NULL,
		NULL,
		&IoStatus,
		&hdr,
		sizeof(hdr),
		NULL,
		NULL );

	
    if ( !NT_SUCCESS( ntStatus ) )
    {
        IF_LOUD(DbgPrint("NPF: Error dumping file %x\n", ntStatus);)
		
		ZwClose( Open->DumpFileHandle );
		Open->DumpFileHandle=NULL;
		
        ntStatus = STATUS_NO_SUCH_FILE;
        return ntStatus;
    }

	Open->DumpOffset.QuadPart=24;
			
	ntStatus = PsCreateSystemThread(&Open->DumpThreadHandle,
		THREAD_ALL_ACCESS,
		(ACCESS_MASK)0L,
		0,
		0,
		PacketDumpThread,
		Open);
	
    if ( !NT_SUCCESS( ntStatus ) )
    {
        IF_LOUD(DbgPrint("NPF: Error creating dump thread, status=%x\n", ntStatus);)
		
		ZwClose( Open->DumpFileHandle );
		Open->DumpFileHandle=NULL;

        return ntStatus;
    }  

	
	return ntStatus;
	
}

//-------------------------------------------------------------------

NTSTATUS PacketCloseDumpFile(POPEN_INSTANCE Open){
	NTSTATUS	ntStatus;
	IO_STATUS_BLOCK IoStatus;
    PMDL		WriteMdl;
    PUCHAR		VMBuff;
	UINT		VMBufLen;


    IF_LOUD(DbgPrint("NPF: PacketCloseDumpFile.\n");)

    IF_LOUD(DbgPrint("Dumpoffset=%d\n",Open->DumpOffset.QuadPart);)

	ZwClose( Open->DumpFileHandle );

	ObDereferenceObject(Open->DumpFileObject);

	PacketOpenDumpFile(Open,&Open->DumpFileName, TRUE);

	//fill the application buffer
	if(Open->Btail>Open->Bhead){

		ntStatus = ZwWriteFile(Open->DumpFileHandle,
			NULL,
			NULL,
			NULL,
			&IoStatus,
			Open->Buffer+Open->Bhead,
			Open->Btail-Open->Bhead,
			&Open->DumpOffset,
			NULL );	
	}
	else {
		
		ntStatus = ZwWriteFile(Open->DumpFileHandle,
			NULL,
			NULL,
			NULL,
			&IoStatus,
			Open->Buffer+Open->Bhead,
			Open->BLastByte-Open->Bhead,
			&Open->DumpOffset,
			NULL );	

	}	

	// Close The file
	ZwClose( Open->DumpFileHandle );
	Open->DumpFileHandle=NULL;

	ObDereferenceObject(Open->DumpFileObject);

	return STATUS_SUCCESS;
}

//-------------------------------------------------------------------
// Dump Thread
//-------------------------------------------------------------------

VOID PacketDumpThread(POPEN_INSTANCE Open)
{
	UINT		Thead;
	UINT		Ttail;
	UINT		TLastByte;
	PUCHAR		CurrBuff;
	NTSTATUS	ntStatus;
	IO_STATUS_BLOCK IoStatus;
    PMDL		lMdl;
	ULONG				FrozenNic;

    IF_LOUD(DbgPrint("NPF: In the work routine.  Parameter = 0x%0x\n",Open);)

	while(TRUE){

		// Wait until some packets arrive or the timeout expires
		NdisWaitEvent(&Open->DumpEvent, 5000);  

		IF_LOUD(DbgPrint("NPF: Worker Thread - event signalled\n");)
			
		// BufSize=0 means that this instance was closed, or that the buffer is too small for any capture.
		// In both cases it is better to end the dump
		if(Open->BufSize==0){
			IF_LOUD(DbgPrint("NPF: Worker Thread - Exiting happily\n");)
				PsTerminateSystemThread(STATUS_SUCCESS);
			return;
		}
		
		NdisResetEvent(&Open->DumpEvent);
		
		// The buffer is full enough to be dumped,
		// The read call is completed
		Thead=Open->Bhead;
		Ttail=Open->Btail;
		TLastByte=Open->BLastByte;
		
		// Get the address of the buffer
		CurrBuff=Open->Buffer;
					
		// Fill the application buffer
		if( Ttail > Thead ){			
			lMdl=IoAllocateMdl(CurrBuff+Thead,Ttail-Thead,FALSE,FALSE,NULL);
			if (lMdl == NULL)
			{
				// No memory: packet lost
				IF_LOUD(DbgPrint("NPF: dump thread: Failed to allocate Mdl\n");)
				PsTerminateSystemThread(STATUS_SUCCESS);
				return;
			}
			
			MmBuildMdlForNonPagedPool(lMdl);

			// Write to disk
			PacketWriteDumpFile(Open->DumpFileObject,
				&Open->DumpOffset,
				Ttail-Thead,
				lMdl,
				&IoStatus);

			IoFreeMdl(lMdl);

			if(!NT_SUCCESS(IoStatus.Status)){
				PsTerminateSystemThread(STATUS_SUCCESS);
				return;
			}
			
			// Update the packet buffer
			Open->DumpOffset.QuadPart+=(Ttail-Thead);			
			Open->Bhead=Ttail;
			
		}
		else if( Ttail < Thead ){
			lMdl=IoAllocateMdl(CurrBuff+Thead,TLastByte-Thead,FALSE,FALSE,NULL);
			if (lMdl == NULL)
			{
				//no memory: packet lost
				IF_LOUD(DbgPrint("NPF: dump thread: Failed to allocate Mdl\n");)
				PsTerminateSystemThread(STATUS_SUCCESS);
				return;
			}
			
			MmBuildMdlForNonPagedPool(lMdl);
			
			// Write to disk
			PacketWriteDumpFile(Open->DumpFileObject,
				&Open->DumpOffset,
				TLastByte-Thead,
				lMdl,
				&IoStatus);

			IoFreeMdl(lMdl);

			if(!NT_SUCCESS(IoStatus.Status)){
				PsTerminateSystemThread(STATUS_SUCCESS);
				return;
			}

			// Update the packet buffer
			Open->DumpOffset.QuadPart+=(TLastByte-Thead);
			Open->BLastByte=Ttail;
			Open->Bhead=0;
		}
						
	}
	
}

//-------------------------------------------------------------------

static NTSTATUS PacketDumpCompletion(PDEVICE_OBJECT DeviceObject,
                                PIRP Irp,
                                PVOID Context)
{

    // Copy the status information back into the "user" IOSB
    *Irp->UserIosb = Irp->IoStatus;
    
    // Wake up the mainline code
    KeSetEvent(Irp->UserEvent, 0, FALSE);
          
    return STATUS_MORE_PROCESSING_REQUIRED;
}

//-------------------------------------------------------------------

VOID PacketWriteDumpFile(PFILE_OBJECT FileObject,
			                    PLARGE_INTEGER Offset,
								ULONG Length,
								PMDL Mdl,
								PIO_STATUS_BLOCK IoStatusBlock)
{
    PIRP irp;
    KEVENT event;
    PIO_STACK_LOCATION ioStackLocation;
    PDEVICE_OBJECT fsdDevice = IoGetRelatedDeviceObject(FileObject);
    NTSTATUS Status;
 
    // Set up the event we'll use
    KeInitializeEvent(&event, SynchronizationEvent, FALSE);
    
    // Allocate and build the IRP we'll be sending to the FSD
    irp = IoAllocateIrp(fsdDevice->StackSize, FALSE);

    if (!irp) {
        // Allocation failed, presumably due to memory allocation failure
        IoStatusBlock->Status = STATUS_INSUFFICIENT_RESOURCES;
        IoStatusBlock->Information = 0;

		return;
    }
    
    irp->MdlAddress = Mdl;
    irp->UserEvent = &event;
    irp->UserIosb = IoStatusBlock;
    irp->Tail.Overlay.Thread = PsGetCurrentThread();
    irp->Tail.Overlay.OriginalFileObject= FileObject;    
    irp->RequestorMode = KernelMode;
    
    // Indicate that this is a WRITE operation
    irp->Flags = IRP_WRITE_OPERATION;    
    
    // Set up the next I/O stack location
    ioStackLocation = IoGetNextIrpStackLocation(irp);
    ioStackLocation->MajorFunction = IRP_MJ_WRITE;
    ioStackLocation->MinorFunction = 0;
    ioStackLocation->DeviceObject = fsdDevice;
    ioStackLocation->FileObject = FileObject;
    IoSetCompletionRoutine(irp, PacketDumpCompletion, 0, TRUE, TRUE, TRUE);    
    ioStackLocation->Parameters.Write.Length = Length;    
    ioStackLocation->Parameters.Write.ByteOffset = *Offset;
    

    // Send it on.  Ignore the return code
    (void) IoCallDriver(fsdDevice, irp);
     
    // Wait for the I/O to complete.
    KeWaitForSingleObject(&event, Executive, KernelMode, TRUE, 0);

    // Free the IRP now that we are done with it
    IoFreeIrp(irp);

    return;

}
