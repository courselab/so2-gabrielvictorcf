/*
 *    SPDX-FileCopyrightText: 2024 Monaco F. J. <monaco@usp.br>
 *    SPDX-FileCopyrightText: 2024 gabrielvictorcf <gabrielvcf@outlook.com>
 *   
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *  This file is a derivative work from SYSeg (https://gitlab.com/monaco/syseg)
 *  and contains modifications carried out by the following author(s):
 *  gabrielvictorcf <gabrielvcf@outlook.com>
 */

#ifndef BIOS2_H
#define BIOS2_H

int __attribute__((fastcall)) kread(char *);

void __attribute__((fastcall)) udelay(unsigned short);

#endif  /* BIOS2_H  */
