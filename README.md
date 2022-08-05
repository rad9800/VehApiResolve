# VehApiResolve

Inspired by [Dridex Loader's 32 bit API obfuscation.](https://www.0ffset.net/reverse-engineering/malware-analysis/dridex-veh-api-obfuscation/)
Though, this is not specific to API hashing and can be used in various other contexts.

### How does this work?

We setup an exception handler. Although, it is possible to do this without with `__try{...} __except(...){...}`
```c
SetUnhandledExceptionFilter( ApiResolverHandler );
```
This exception handler will only handle exception's thrown as EXCEPTION_BREAKPOINT. 
We throw an exception with LazyResolve which is inlined assembly (it sets up the required hashes in the corresponding registers [R8/R9])
When it is thrown by us, we're able to capture the CONTEXT. We can get the [arguments passed](https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention). 
We can then dynamically resolve them with `GetProcAddrExH` as seen
```c
ExceptionInfo->ContextRecord->Rax = (ULONG_PTR)GetProcAddrExH( ExceptionInfo->ContextRecord->R8, ExceptionInfo->ContextRecord->R9 );
```
We're then able to capture the return value stored in Rax and return it to us.

1. Raise an exception. 
2. Handle this exception. 
3. Resolve the required function. 
4. Store it in RAX.
5. Return.

We can then use this address. 

### Why?
Probably makes a reverse engineers life harder, which is always good.

### Destorying IDA's disassembly.
After our `int3` we can add various instructions/opcodes such as ret or 0xE9 in order to break IDA's disassembly. In our exception handler just remember to make the RIP point to the right number of instructions past and including `int3`

Thanks to [@anthonyprintup](https://twitter.com/anthonyprintup) for helping out a bunch, and providing a lot of cool ideas!

TLDR: 
[Require's LLVM 14 at least. Intellisense may not work but it WILL compile.](https://docs.microsoft.com/en-us/cpp/build/clang-support-msbuild?view=msvc-170) 
