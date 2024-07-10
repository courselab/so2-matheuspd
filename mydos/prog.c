/*
 *    SPDX-FileCopyrightText: 2024 Matheus Pereira Dias <matheuspd07@gmail.com>
 *   
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *  This file is a derivative work from SYSeg (https://gitlab.com/monaco/syseg)
 *  and contains modifications carried out by the following author(s):
 *  Matheus Pereira Dias <matheuspd07@gmail.com>
 */

#include "tydos.h"

int main()
{
  puts ("\nHello World\n");
  puts ("\nWrite your name: ");
  char buffer[32];
  fgets(buffer, 32);
  puts ("Hello ");
  puts (buffer);
  puts ("!!!\n");
  return 0;
}
