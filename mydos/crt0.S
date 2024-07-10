#    SPDX-FileCopyrightText: 2024 Matheus Pereira Dias <matheuspd07@gmail.com>
#   
#    SPDX-License-Identifier: GPL-3.0-or-later
#
#    This file is part of SYSeg, available at https://gitlab.com/monaco/syseg.
/*
crt0.s:

.global _start

.section .text
_start:
    ; Initialize stack pointer
    mov $0xFE00, %sp

    ; Call the thunk function to get the current IP
    call get_ip_into_ax

    ; Push the return address and jump to main
    push %ax
    jmp main

get_ip_into_ax:
    mov (%sp), %ax  # Load %ax with the return address
    ret
*/