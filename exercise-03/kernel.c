/*
 *    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
 *    SPDX-FileCopyrightText: 2024 gabrielvictorcf <gabrielvcf@outlook.com>
 *   
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *  This file is a derivative work from SYSeg (https://gitlab.com/monaco/syseg)
 *  and contains modifications carried out by the following author(s):
 *  gabrielvictorcf <gabrielvcf@outlook.com>
 */

/* This source file implements the kernel entry function 'kmain' called
   by the bootloader, and the command-line interpreter. Other kernel functions
   were implemented separately in another source file for legibility. */

#include "bios1.h"		/* For kwrite() etc.                */
#include "bios2.h"		/* For kread() etc.                 */
#include "kernel.h"		/* Essential kernel functions.      */
#include "kaux.h"		  /* Auxiliary kernel functions.      */
#include "vbfs.h"     /* Very basic filesystem functions  */
#include "tydos.h"

/* Kernel's entry function. */

void kmain(void)
{
  int i, j;
  
  register_syscall_handler();	/* Register syscall handler at int 0x21.*/

  // splash();			/* Uncessary spash screen.              */

  shell();			/* Invoke the command-line interpreter. */
  
  halt();			/* On exit, halt.                       */
  
}

/* Tiny Shell (command-line interpreter). */

char buffer[BUFF_SIZE];
int go_on = 1;

void shell()
{
  int i;
  clear();
  kwrite ("TinyDOS 1.0\n");

  while (go_on)
    {

      /* Read the user input. 
	 Commands are single-word ASCII tokens with no blanks. */
      do
	{
	  kwrite(PROMPT);
	  kread (buffer);
	}
      while (!buffer[0]);

      /* Check for matching built-in commands */
      
      i=0;
      while (cmds[i].funct)
	{
	  if (!strcmp(buffer, cmds[i].name))
	    {
	      cmds[i].funct();
	      break;
	    }
	  i++;
	}

      /* If the user input does not match any built-in command name, just
	 ignore and read the next command. If we were to execute external
	 programs, on the other hand, this is where we would search for a 
	 corresponding file with a matching name in the storage device, 
	 load it and transfer it the execution. Left as exercise. */
      
      if (!cmds[i].funct)
	kwrite ("Command not found\n");
    }
}


/* Array with built-in command names and respective function pointers. 
   Function prototypes are in kernel.h. */

struct cmd_t cmds[] =
  {
    {"help",    f_help},     /* Print a help message.               */
    {"quit",    f_quit},     /* Exit TyDOS.                         */
    {"list",    f_list},     /* List programs in filesystem         */
    {"exec",    f_exec},     /* Execute user program in filesystem  */
    {0, 0}
  };


/* Build-in shell command: help. */

void f_help()
{
  kwrite ("...me, Obi-Wan, you're my only hope!\n\n");
  kwrite ("   But we can try also some commands:\n");
  kwrite ("      list    (to list all available files in filesystem)\n");
  kwrite ("      exec    (to execute user program in filesystem)\n");
  kwrite ("      quit    (to exit TyDOS)\n");
}

void f_quit()
{
  kwrite ("Program halted. Bye.");
  go_on = 0;
}

void f_list() {
  fs_header* header = load_fs_header();

  char directory[header->number_of_file_entries][DIR_ENTRY_LEN];
  load_directory((char*) directory);

  /* Read all entries. */

  for (int i=0; i < header->number_of_file_entries; i++) {
    if(directory[i][0]) {
      kwrite(directory[i]);
      kwrite("\n");
    }
  }
}

void f_exec() {
  kwrite("Program name: ");

  char program_name[100];
  gets(program_name);

  fs_header* header = load_fs_header();

  char directory[header->number_of_file_entries][DIR_ENTRY_LEN];
  load_directory((char*) directory);

  char* entry;
  int program_i = -1;
  for (int i=0; i < header->number_of_file_entries; i++) {
    entry = directory[i];

    if(strcmp(program_name, entry) == 0) {
      program_i = i;
      break;
    }
  }

  if (program_i == -1) {
    kwrite("Couldn't match name with program on filestystem.\n");
    return;
  }

  // This has an additional sector because, due to directory header, there is no guarantee
  // that addreses in data region are aligned, they may be offset into a sector. So we always have to
  // read an additional sector and figure out the padding.
  char program_mem_pool[header->max_file_size + 1];
  load_program(program_i);

  // Execute user program
  kwrite("Found, executing now.\n");
  kfork();

  return;
}
