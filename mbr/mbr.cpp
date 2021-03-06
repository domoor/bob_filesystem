#include <stdio.h>		// puts(), printf(), fprintf()
#include <stdint.h>		// uint
#include "parse_fs.h"		// struct mbr, gpt, macro, Etc...

int count;

int mbr_parse(FILE *rfp, uint32_t abs_addr, uint32_t offset) {
	static uint8_t buf[SECTOR_SIZE];
	fread(buf, 1, SECTOR_SIZE, rfp);
	struct mbr_hdr *mbr = (struct mbr_hdr*)buf;
	if(mbr->signature != BR_SIGNATURE) puts("[ The parsed information is not trusted! ]");

	for(int i = 0; i<4; i++) {
		uint8_t P_type = mbr->partition_table[i].partition_type;
		uint32_t start_addr = (mbr->partition_table[i].lba_s_addr) * SECTOR_SIZE;

		if(P_type == TYPE_EP) {
			fseek(rfp, start_addr + offset, SEEK_SET);
			if(offset == 0)	mbr_parse(rfp, start_addr, start_addr);
			else		mbr_parse(rfp, offset+start_addr, offset);
		}
		else if(P_type == TYPE_NTFS) {
			uint32_t size = (mbr->partition_table[i].partition_size) * SECTOR_SIZE;

			printf("Partition%2d     Start_Address: %08X     Size: %d\n", ++count, abs_addr + start_addr, size);
		}
		else if(P_type == TYPE_NO) { return 0; }
		else {
			puts("[ I don't know the type! ]");
			printf("Partition Type = %02X\n", P_type);
			return -1;
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	if(argc != 2) {
		printf("Syntax: %s, <mbr file name>\n", argv[0]);
		printf("Sample: %s, mbr_128.dd\n", argv[0]);
		return -1;
	}

	FILE *rfp = fopen(argv[1], "rb");
	if (rfp == NULL) { puts("error: File not found"); return -1; }
	mbr_parse(rfp, 0, 0);
	fclose(rfp);

    return 0;
}
