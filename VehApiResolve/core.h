#pragma once
#include <Windows.h>
#include <winternl.h>
#include <intrin.h>

#define HASHALGO HashStringFowlerNollVoVariant1a         // specify algorithm here

#pragma region misc

// for VEH
#define CALL_FIRST 1
#define CALL_LAST 0

#define TOKENIZE( x ) #x
#define CONCAT( X, Y ) X##Y

#define API( DLL, FUNCNAME ) ( ( CONCAT( type, FUNCNAME ))LazyResolve( CONCAT( hash,FUNCNAME ), \
CONCAT( hash, DLL ) ) )		

template <typename Type>
inline Type RVA2VA( LPVOID Base, LONG Rva ) {
	return (Type)((ULONG_PTR)Base + Rva);
}

__forceinline char upper( char c );

#pragma endregion

#pragma region hashing

#pragma region HashStringFowlerNollVoVariant1a

constexpr ULONG HashStringFowlerNollVoVariant1a( const char* String )
{
	ULONG Hash = 0x811c9dc5;

	while( *String )
	{
		Hash ^= (UCHAR)*String++;
		Hash *= 0x01000193;
	}

	return Hash;
}

constexpr ULONG HashStringFowlerNollVoVariant1a( const wchar_t* String )
{
	ULONG Hash = 0x811c9dc5;

	while( *String )
	{
		Hash ^= (UCHAR)*String++;
		Hash *= 0x01000193;
	}

	return Hash;
}
#pragma endregion

#define hash( VAL ) constexpr auto CONCAT( hash, VAL ) = HASHALGO( TOKENIZE( VAL ) );							

#define dllhash(DLL, VAL ) constexpr auto CONCAT( hash, DLL ) = HASHALGO( VAL );												

#define hashFunc( FUNCNAME , RETTYPE, ...)																\
hash( FUNCNAME ) typedef RETTYPE( WINAPI* CONCAT( type, FUNCNAME ) )( __VA_ARGS__ );					

dllhash( KERNEL32, L"KERNEL32.DLL" )
dllhash( NTDLL, L"NTDLL.DLL" )
dllhash( USER32, L"USER32.DLL" )


hashFunc( MessageBoxA, int, HWND, LPCSTR, LPCSTR, UINT );

#pragma endregion

/// Call once exception handled. 
PVOID GetProcAddrExH( UINT, UINT );

/// Resolve exception and call GetProcAdddrH
LONG WINAPI ApiResolverHandler( PEXCEPTION_POINTERS ExceptionInfo );
