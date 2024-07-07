.global _start
_start:
    call get_ip_into_ax
    push %ax        # Push the return address onto the stack
    call main       # Call the main function
    jmp .           # Hang if main returns

get_ip_into_ax:
    mov (%sp), %ax  # Load %ax with the return address
    ret
