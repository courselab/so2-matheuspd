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

/* This source file implements the kernel entry function 'kmain' called
   by the bootloader, and the command-line interpreter. Other kernel functions
   were implemented separately in another source file for legibility. */

#include "bios1.h"	/* For kwrite() etc.            */
#include "bios2.h"	/* For kread() etc.             */
#include "kernel.h" /* Essential kernel functions.  */
#include "kaux.h"	/* Auxiliary kernel functions.  */

/* Kernel's entry function. */

void kmain(void)
{
	int i, j;

	register_syscall_handler(); /* Register syscall handler at int 0x21.*/

	splash(); /* Uncessary spash screen.              */

	shell(); /* Invoke the command-line interpreter. */

	halt(); /* On exit, halt.                       */
}

/* Tiny Shell (command-line interpreter). */

char buffer[BUFF_SIZE];
int go_on = 1;

void shell()
{
	int i;
	clear();
	kwrite("SOS 1.0\n");

	while (go_on)
	{

		/* Read the user input.
	   Commands are single-word ASCII tokens with no blanks. */
		do
		{
			kwrite(PROMPT);
			kread(buffer);
		} while (!buffer[0]);

		/* Check for matching built-in commands */

		i = 0;
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
			kwrite("Command not found\n");
	}
}

/* Array with built-in command names and respective function pointers.
   Function prototypes are in kernel.h. */

struct cmd_t cmds[] =
	{
		{"help", f_help},	                /* Print a help message.       */
		{"quit", f_quit},	                /* Exit TyDOS.                 */
		{"exec_prog",    f_exec_prog},     	/* Execute an example program. */
		{"list", f_list_disk},              /* List all files */
		{0, 0}};

struct fs_header_t
{
	unsigned char signature[FS_SIGLEN];		/* The file system signature.              */
	unsigned short total_number_of_sectors; /* Number of 512-byte disk blocks.         */
	unsigned short number_of_boot_sectors;	/* Sectors reserved for boot code.         */
	unsigned short number_of_file_entries;	/* Maximum number of files in the disk.    */
	unsigned short max_file_size;			/* Maximum size of a file in blocks.       */
	unsigned int unused_space;				/* Remaining space less than max_file_size.*/
} __attribute__((packed)) fs_header;		/* Disable alignment to preserve offsets.  */

/* Build-in shell command: help. */

void f_help()
{
	kwrite("   Commands:\n");
	kwrite("      exec_prog    (to execute an user program example\n");
	kwrite("      list    (to list all the files\n");
	kwrite("      quit    (to exit TyDOS)\n");
}

void f_quit()
{
	kwrite("Program halted. Bye.");
	go_on = 0;
}

/* Built-in shell command: example.

   Execute an example user program which invokes a syscall.

   The example program (built from the source 'prog.c') is statically linked
   to the kernel by the linker script (tydos.ld). In order to extend the
   example, and load and external C program, edit 'f_exec' and 'prog.c' choosing
   a different name for the entry function, such that it does not conflict with
   the 'main' function of the external program.  Even better: remove 'f_exec'
   entirely, and suppress the 'example_program' section from the tydos.ld, and
   edit the Makefile not to include 'prog.o' and 'libtydos.o' from 'tydos.bin'.

  */

void load_disk(int sector, int num_entries, void *addres)
{
	__asm__ volatile(
		"pusha \n"
		"mov boot_drive, %%dl \n" /* Select the boot drive (from rt0.o) */
		"mov $0x2, %%ah \n"		  /* BIOS disk service: op. read sector */
		"mov %[NumEntr], %%al \n" /* How many sectors to read          */
		"mov $0x0, %%ch \n"		  /* Cylinder (starts at 0)  */
		"mov %[Sect], %%cl \n"	  /* Sector   (starts at 1)  */
		"mov $0x0, %%dh \n"		  /* Head     (starts at 0)      */
		"mov %[Addr], %%bx \n"	  /* Where to load the file system (rt0.o)   */
		"int $0x13 \n"			  /* Call BIOS disk service 0x13        */
		"popa \n" ::
		[Sect] "g"(sector),
		[NumEntr] "g"(num_entries),
		[Addr] "g"(addres));
}

// Function to list the contents of the disk
void f_list_disk()
{
	// Pointer to the FS header in RAM
	struct fs_header_t *header = (struct fs_header_t *)0x7c00;

	// Calculate the starting sector of the directory region
	unsigned short dir_start_sector = header->number_of_boot_sectors + 1;
	unsigned short num_entries = header->number_of_file_entries*DIR_ENTRY_LEN/SECTOR_SIZE;

	extern unsigned char _MEM_POOL;
	void *dir_address = (void *)&_MEM_POOL;

	load_disk(dir_start_sector, num_entries, dir_address);

	for (int i = 0; i < header->number_of_file_entries; i++)
	{
		char *file_name = dir_address + DIR_ENTRY_LEN * i;
		if (file_name[0])
		{
			kwrite(file_name);
			kwrite(" ");
		}
	}
	kwrite("\n");
}

void f_exec_prog() {
    // Program to execute
    char *prog_name;
	kwrite("Enter the executable's name: ");
    char program_name[DIR_ENTRY_LEN];
    kread(program_name);

	// Pointer to the FS header in RAM
    struct fs_header_t *header = (struct fs_header_t *)0x7c00;

	// Calculate the starting sector of the directory region
    unsigned short dir_start_sector = header->number_of_boot_sectors + 1;
    unsigned short num_entries = header->number_of_file_entries*DIR_ENTRY_LEN/SECTOR_SIZE;

    extern unsigned char _MEM_POOL;
    void *dir_address = (void *)&_MEM_POOL;

    // Load the directory entries into memory
    load_disk(dir_start_sector, num_entries, dir_address);

    // Find the program in the directory entries
    unsigned char *entry = (unsigned char *)dir_address;
    int found = 0;
	int index = 0;
    for (index = 0; index < header->number_of_file_entries; index++, entry += DIR_ENTRY_LEN) {
        if (strcmp((char *)entry, prog_name) == 0) {
            found = 1;
            break;
        }
    }

    if (!found) {
        kwrite("Program not found.\n");
        return;
    }

    // Calculate the starting sector of the program and memory offset
	int prog_start_sector = dir_start_sector + num_entries + header->max_file_size*index - 1;
  	int mem_offset = header->number_of_file_entries*DIR_ENTRY_LEN - (num_entries - 1)*SECTOR_SIZE;

  	void *mem_loc = (void *)USER_PROGRAM_LOAD_ADDRESS - mem_offset;

    // Load the program into memory at 0xFE00
    load_disk(prog_start_sector, header->max_file_size, mem_loc);

	 __asm__ volatile(
    "call save_return_address \n"
    "original_address: \n"
    "	push %%ebx \n"
    "	jmp %[programAddress] \n"
    "save_return_address: \n"
    "	mov (%%esp), %%ebx \n"
    "	mov $program_end, %%ecx\n"
    "	sub $original_address, %%ecx\n"
    "	add %%ecx, %%ebx\n"
    "	ret \n"
    "program_end: \n" ::
	[programAddress] "r" (USER_PROGRAM_LOAD_ADDRESS));
}

