# VehApiResolve

Inspired by [Dridex Loader's 32 bit API obfuscation.](https://www.0ffset.net/reverse-engineering/malware-analysis/dridex-veh-api-obfuscation/)

### How does this work?

We setup an exception handler. 
```c
AddVectoredExceptionHandler( CALL_FIRST, ApiResolverHandler );
```
This exception handler will only handle exception's thrown as EXCEPTION_BREAKPOINT. When it is thrown by us, we're able to capture the CONTEXT. We can get the [arguments passed](https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention). 
RDX and R8 will hold our function and module hash. We can then dynamically resolve them with `GetProcAddrExH` as seen
```c
ExceptionInfo->ContextRecord->Rax = (ULONG_PTR)GetProcAddrExH( ExceptionInfo->ContextRecord->Rdx, ExceptionInfo->ContextRecord->R8 );
```
The RAX is often used to return values. When we return, if the address was resolved, it'll be returned.
```asm
LazyRet proc
	int 3	; raise an exception 
	ret		; rax will hold address
LazyRet endp
```

1. Raise an exception. 
2. Handle this exception. 
3. Resolve the required function. 
4. Store it in RAX.
5. Return.

We can then use this address. 

### Why?
Probably makes a reverse engineers life harder, which is always good.
