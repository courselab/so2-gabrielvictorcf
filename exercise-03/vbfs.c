#include "vbfs.h"
#include "bios1.h"
#include "kaux.h"

fs_header* load_fs_header() {
  return (fs_header*)0x7c00;
}

// // Yet, another good itoa implementation
// // returns: the length of the number string
int itoa(int value, char *sp, int radix) {
    char tmp[32];// be careful with the length of the buffer
    char *tp = tmp;
    int i;
    unsigned v;

    int sign = (radix == 10 && value < 0);    
    if (sign)
        v = -value;
    else
        v = (unsigned)value;

    while (v || tp == tmp)
    {
        i = v % radix;
        v /= radix;
        if (i < 10)
          *tp++ = i+'0';
        else
          *tp++ = i + 'a' - 10;
    }

    int len = tp - tmp;

    if (sign) 
    {
        *sp++ = '-';
        len++;
    }

    while (tp > tmp)
        *sp++ = *--tp;

    return len;
}

// lba = logic block address, 0-indexed
void dread(unsigned long lba, int n_sectors, void* output_buffer) {
    int sector = (lba % MAX_SECTOR_COUNT) + 1;
    int head = (lba / MAX_SECTOR_COUNT) % MAX_HEAD_COUNT;
    int cylinder = lba / (MAX_SECTOR_COUNT * MAX_HEAD_COUNT);
    
    asm volatile (
        "pusha;"
	    "mov boot_drive, %%dl;"		// drive number 0 (aka the drive # with 0x80)
	    "mov $0x2, %%ah;"      // 0x02 = function number for read; AL = unused
        "mov %[n_sectors], %%al;"	        // address of "disk address packet"
        "mov %[cylinder], %%ch;"	        // address of "disk address packet"
        "mov %[sector], %%cl;"	        // address of "disk address packet"
        "mov %[head], %%dh;"	        // address of "disk address packet"
        "mov %[buffer_addr], %%bx;"	        // address of "disk address packet"
	    "int $0x13;"
        "popa;"
    ::  [n_sectors] "g"(n_sectors),
        [cylinder] "g"(cylinder),
        [sector] "g"(sector),
        [head] "g"(head),
        [buffer_addr] "g"(output_buffer)
    );
}

void load_directory(char* directory) {
    fs_header* header = load_fs_header();

    unsigned long lba = header->number_of_boot_sectors; // we use the quantity here because lba is 0-indexed
    unsigned short n_sectors_to_read = (header->number_of_file_entries * DIR_ENTRY_LEN) / SECTOR_LEN;

    for (int i = 0; i < n_sectors_to_read * SECTOR_LEN; i++) {
        directory[i] = 0;
    }
    
    dread(lba, n_sectors_to_read, directory);
}

void load_program(int program_entry_i) {
    fs_header* header = load_fs_header();
    
    // for (int i = 0; i < header->max_file_size; i++) {
    //     program_mem_pool[i] = 0;
    // }

    int boot_sector_count = header->number_of_boot_sectors;
    int directory_sector_count = (header->number_of_file_entries * DIR_ENTRY_LEN) / SECTOR_LEN;
    int directory_offet = (header->number_of_file_entries * DIR_ENTRY_LEN) % SECTOR_LEN;
    int data_region_offset = program_entry_i * header->max_file_size;

    int lba = boot_sector_count + directory_sector_count + data_region_offset;
    unsigned short n_sectors_to_read = header->max_file_size;

    void* program_region = USERSPACE_ADDR - directory_offet;
    dread(lba, header->max_file_size, program_region);
}
