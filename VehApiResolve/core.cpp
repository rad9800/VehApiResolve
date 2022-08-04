#include "core.h"

PVOID GetProcAddrExH( UINT funcHash, UINT moduleHash )
{
	PPEB peb = NtCurrentTeb()->ProcessEnvironmentBlock;
	LIST_ENTRY* head = &peb->Ldr->InMemoryOrderModuleList;
	LIST_ENTRY* next = head->Flink;
	PVOID base = NULL;

	while( next != head )
	{
		LDR_DATA_TABLE_ENTRY* entry = (LDR_DATA_TABLE_ENTRY*)((PBYTE)next - offsetof( LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks ));

		UNICODE_STRING* fullname = &entry->FullDllName;
		UNICODE_STRING* basename = (UNICODE_STRING*)((PBYTE)fullname + sizeof( UNICODE_STRING ));

		char  name[64];
		if( basename->Length < sizeof( name ) - 1 )
		{
			int i = 0;
			while( basename->Buffer[i] && i < sizeof( name ) - 1 )
			{
				name[i] = upper( (char)basename->Buffer[i] );	// can never be sure so uppercase
				i++;
			}
			name[i] = 0;
			UINT hash = HASHALGO( name );
			// is this our moduleHash?
			if( hash == moduleHash ) {
				base = entry->DllBase;

				PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
				PIMAGE_NT_HEADERS nt = RVA2VA<PIMAGE_NT_HEADERS>( base, dos->e_lfanew );

				PIMAGE_EXPORT_DIRECTORY exports = RVA2VA<PIMAGE_EXPORT_DIRECTORY>( base, nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress );
				if( exports->AddressOfNames != 0 )
				{
					PWORD ordinals = RVA2VA<PWORD>( base, exports->AddressOfNameOrdinals );
					PDWORD names = RVA2VA<PDWORD>( base, exports->AddressOfNames );
					PDWORD functions = RVA2VA<PDWORD>( base, exports->AddressOfFunctions );

					for( DWORD i = 0; i < exports->NumberOfNames; i++ ) {
						LPSTR name = RVA2VA<LPSTR>( base, names[i] );
						if( HASHALGO( name ) == funcHash ) {
							PBYTE function = RVA2VA<PBYTE>( base, functions[ordinals[i]] );
							return function;
						}
					}
				}
			}
		}
		next = next->Flink;
	}

	return NULL;
}

LONG WINAPI ApiResolverHandler( PEXCEPTION_POINTERS ExceptionInfo )
{
	auto ExceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
	if( ExceptionCode == EXCEPTION_BREAKPOINT )
	{
		// ExceptionInfo->ContextRecord->Rdx holds our funcHash
		// ExceptionInfo->ContextRecord->R8 holds our moduleHash
		// ExceptionInfo->ContextRecord->Rax will hold the return value
		ExceptionInfo->ContextRecord->Rax = (ULONG_PTR)GetProcAddrExH( ExceptionInfo->ContextRecord->Rdx, ExceptionInfo->ContextRecord->R8 );
		ExceptionInfo->ContextRecord->Rip++;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

__forceinline char upper( char c )
{
	if( c >= 'a' && c <= 'z' ) {
		return c - 'a' + 'A';
	}

	return c;
}