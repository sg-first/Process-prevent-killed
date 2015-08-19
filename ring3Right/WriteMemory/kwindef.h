#ifndef KWINDEF_H_
#define KWINDEF_H_


/*
* you'll find a list of NTSTATUS status codes in the DDK header
* ntstatus.h (\WINDDK\2600.1106\inc\ddk\wxp\)
*/
#define NT_SUCCESS(status)          ((NTSTATUS)(status)>=0)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define STATUS_ACCESS_DENIED        ((NTSTATUS)0xC0000022L)

/*
*************************************************************************
* ntddk.h
*/
typedef LONG    NTSTATUS;
typedef ULONG    ACCESS_MASK;
/*
* ntdef.h
*************************************************************************
*/

/*
*************************************************************************
* <<Windows NT/2000 Native API Reference>> - Gary Nebbett
*/

typedef enum _SYSTEM_INFORMATION_CLASS
{
    SystemHandleInformation = 16
} SYSTEM_INFORMATION_CLASS;

/*
*Information Class 16
*/
typedef struct _SYSTEM_HANDLE_INFORMATION
{
    ULONG            ProcessId;
    UCHAR            ObjectTypeNumber;
    UCHAR            Flags;
    USHORT            Handle;
    PVOID            Object;
    ACCESS_MASK        GrantedAccess;
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef struct _MEMORY_CHUNKS {
    ULONG Address;
    PVOID Data;
    ULONG Length;
}MEMORY_CHUNKS, *PMEMORY_CHUNKS;


typedef NTSTATUS ( __stdcall *ZWQUERYSYSTEMINFORMATION ) ( IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
														  IN OUT PVOID SystemInformation,
														  IN ULONG SystemInformationLength, 
														  OUT PULONG ReturnLength OPTIONAL );

typedef NTSTATUS ( __stdcall *NTSYSTEMDEBUGCONTROL ) ( IN int nCmd,
														  IN PVOID pInBuf,
														  IN ULONG nInLen, 
														  OUT PVOID pOutBuf,
														  IN ULONG nOutLen,
														  OUT PULONG nRetLen OPTIONAL);

#endif