.code 

LazyRet proc
	int 3	; raise an exception 
	ret		; rax will hold address
LazyRet endp

end