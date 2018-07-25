#include <stdio.h>		// puts(), printf(), fprintf()
#include <stdint.h>		// uint
#include "parse_fs.h"		// struct mbr, gpt, macro, Etc...

int main(int argc, char **argv)
{
	if(argc != 2) {
		printf("Syntax: %s, <gpt file name>\n", argv[0]);
		printf("Sample: %s, gpt_128.dd\n", argv[0]);
		return -1;
	}

	FILE *rfp = fopen(argv[1], "rb");
	if(rfp == NULL) { puts("error: File not found"); return -1; }
	
	fseek(rfp, SECTOR_SIZE, SEEK_SET); // Jump to Sector1
	uint8_t buf[SECTOR_SIZE];
	fread(buf, 1, SECTOR_SIZE, rfp);
	struct gpt_hdr *gpt = (struct gpt_hdr*)buf;
	if(gpt->entry_size != ENTRY_SIZE) puts("The parsed information is not trusted!");

	for(int i=2; i<=33; i++) {
		uint8_t buff[ENTRY_SIZE];
		fread(buff, 1, ENTRY_SIZE, rfp);
		struct gpt_Entry128_hdr *gpt_entry = (struct gpt_Entry128_hdr*)buff;

		if(gpt_entry->last_LBA == 0) break;
		uint64_t start_addr = (gpt_entry->fir_LBA) * SECTOR_SIZE;
		uint64_t size = ((gpt_entry->last_LBA) - (gpt_entry->fir_LBA) + 1) * SECTOR_SIZE;

		printf("Partition%2d     Start_Address: %016X     Size: %d\n", i-1, start_addr, size);
	}
	fclose(rfp);

	return 0;
}
