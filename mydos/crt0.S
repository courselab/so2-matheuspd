#    SPDX-FileCopyrightText: 2024 Matheus Pereira Dias <matheuspd07@gmail.com>
#    SPDX-FileCopyrightText: 2024 Gabriel Franceschi Libardi <libardi.120205@protonmail.com>
#    SPDX-License-Identifier: GPL-3.0-or-later
#
#    This file is part of SYSeg, available at https://gitlab.com/monaco/syseg.

.code16gcc
.global _start

.text

    jmp _start

_start:
    call main
    ret