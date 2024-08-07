#    SPDX-FileCopyrightText: 2024 Otto Fernandes <ottofernandes2012@gmail.com>
#   
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