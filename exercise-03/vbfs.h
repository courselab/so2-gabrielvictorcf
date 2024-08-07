#ifndef VERY_BASIC_FILESYSTEM_H
#define VERY_BASIC_FILESYSTEM_H

#define FS_SIGNATURE "\xeb\xety"    /* File system signature.                   */
#define FS_SIGLEN 4                 /* Signature length.                        */
#define SECTOR_LEN 512                /* Logical sector byte length.                        */
#define DIR_ENTRY_LEN 32 	         /* Max file name length in bytes.           */
#define MAX_SECTOR_COUNT 64
#define MAX_HEAD_COUNT 16

// Kernel's filesystem header
typedef struct fs_header_t
{
  unsigned char  signature[FS_SIGLEN];    /* The file system signature.              */
  unsigned short total_number_of_sectors; /* Number of 512-byte disk blocks.         */
  unsigned short number_of_boot_sectors;  /* Sectors reserved for boot code.         */
  unsigned short number_of_file_entries;  /* Maximum number of files in the disk.    */
  unsigned short max_file_size;		      /* Maximum size of a file in blocks.       */
  unsigned int unused_space;              /* Remaining space less than max_file_size.*/
} __attribute__((packed)) fs_header;      /* Disable alignment to preserve offsets.  */

fs_header* load_fs_header();
void load_directory(char* directory);
void load_program(int program_entry_i);

#endif  /* VERY_BASIC_FILESYSTEM_H  */