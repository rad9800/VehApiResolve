/*++
* Inspired by https://www.0ffset.net/reverse-engineering/malware-analysis/dridex-veh-api-obfuscation/
* Kudos to @cPeterr for the great blog post and @herrcore for suggesting this
* @anthonyprintup for helping me loads to get this working inlined 
* and showing me a lot of cool tricks.
--*/
#include "core.h"
#include <stdio.h>

#pragma comment(linker,"/ENTRY:entry")


/// Exception handled. 
PVOID GetProcAddrExH( UINT, UINT );

/// Resolve exception and call GetProcAdddrH
LONG WINAPI ApiResolverHandler( PEXCEPTION_POINTERS ExceptionInfo );

[[gnu::always_inline, gnu::pure, nodiscard]]
void* LazyResolve( const UINT funcHash, const UINT moduleHash )
{
	PVOID address;

	asm volatile(
		"mov %[funcHashArg], %%r8;"
		"mov %[moduleHashArg], %%r9;"
		"int $3;"
		"ret"		//".byte 0xE9"	 
		: "=a" (address)
		: [funcHashArg] "rg" (funcHash), [moduleHashArg] "rg" (moduleHash)
		: "r8", "r9");

	return address;
}


int entry()
{
	SetUnhandledExceptionFilter( ApiResolverHandler );

	LoadLibraryW( L"USER32.dll" );
	if( 1 )
	{
		API( USER32, MessageBoxA )(NULL, "work", "plz", 0);
	}

	return 0;
}
