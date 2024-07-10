/*
 *    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
 *    SPDX-FileCopyrightText: 2024 Matheus Pereira Dias <matheuspd07@gmail.com>
 *   
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *  This file is a derivative work from SYSeg (https://gitlab.com/monaco/syseg)
 *  and contains modifications carried out by the following author(s):
 *  Matheus Pereira Dias <matheuspd07@gmail.com>
 */

/* This is the linker script used to build user programs. */

OUTPUT_FORMAT(binary)        /* Output flat binary (no structure). */
ENTRY(_start)                /* Define the entry point as _start. */

SECTIONS
{
    PRG_LOAD_ADDR = 0xFE00;  /* Program load address. */

    . = PRG_LOAD_ADDR;

    .text : {
        crt0.o (.text)       /* Include the text section of crt0.o first */
        *(.text)             /* Include the text sections of other objects */
    }
    .data : {
        crt0.o (.data)       /* Include the data section of crt0.o */
        *(.data)             /* Include the data sections of other objects */
    }
    .bss : {
        crt0.o (.bss)        /* Include the bss section of crt0.o */
        *(.bss)              /* Include the bss sections of other objects */
    }
    .rodata : {
        crt0.o (.rodata)     /* Include the rodata section of crt0.o */
        *(.rodata)           /* Include the rodata sections of other objects */
    }
    .bin : {
        crt0.o (.text .data .bss .rodata) /* Ensure crt0.o sections are included first */
        *(.text .data .bss .rodata)       /* Include all sections from other objects */
        libtydos.o(.text .data .bss .rodata) /* Include runtime library sections */
    }
}

INPUT(libtydos.a)            /* Link with the TyDOS user library. */





