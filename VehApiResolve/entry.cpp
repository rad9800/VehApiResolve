#include "core.h"
#pragma comment(linker,"/ENTRY:entry")

/// Exception handled. 
PVOID GetProcAddrExH( UINT, UINT );

/// Resolve exception and call GetProcAdddrH
LONG WINAPI ApiResolverHandler( PEXCEPTION_POINTERS ExceptionInfo );

/// Force exception and return dynamically resolved address
extern "C" __forceinline PVOID LazyRet( PVOID placeholder, UINT funcHash, UINT moduleHash );

int entry()
{
	AddVectoredExceptionHandler( CALL_FIRST, ApiResolverHandler );

	LoadLibraryW( L"USER32.dll" );
	if( 1 )
	{
		auto pMessageBoxA = (typeMessageBoxA)LazyRet( NULL, hashMessageBoxA, hashUSER32 );
		pMessageBoxA( NULL, "work", "plz", 0 );
	}

	return 0;
}
