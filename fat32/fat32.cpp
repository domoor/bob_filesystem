#include <Windows.h>
#include <WinNls.h>
#include <algorithm>	// replace

#include <stdio.h>		// puts(), printf(), fprintf()
#include <cstdint>		// uint
#include <malloc.h>		// malloc
#include <string>		// string
#include "parse_fs.h"	// struct mbr, gpt, macro, Etc...

using namespace std;

#define JBC			"\xeb\x58\x90"	// jump boot code(eb 58 90)
#define FAT32_TYPE	"FAT32   "		// File System Type(FAT32)

int main(int argc, char **argv)
{
	if(argc != 2) {
		printf("Syntax: %s, <FAT32 file name>\n", argv[0]);
		printf("Sample: %s, fat32.dd\n", argv[0]);
		return -1;
	}

	FILE *rfp = fopen(argv[1], "rb");
	if(rfp == NULL) { puts("error: File not found"); return -1; }
	
	struct FAT32_hdr *fat32 = (struct FAT32_hdr*)malloc(sizeof(struct FAT32_hdr));
	fread(fat32, 1, sizeof(struct FAT32_hdr), rfp);

	if(memcmp(fat32->jbc, JBC, 3)) { puts("Not FAT Type!"); return -1; }
	if(fat32->bps != SECTOR_SIZE) puts("The parsed information is not trusted!");
	if(memcmp(fat32->fatbody.fs_type,FAT32_TYPE,sizeof(fat32->fatbody.fs_type))) { puts("Not FAT32 Type!"); return -1; }

	// Formulas for locating sectors.
	uint64_t offset = ((fat32->rsc) + (fat32->fatbody.FAT_size32) * (fat32->num_Fats)) * (fat32->bps);
	fseek(rfp, offset, SEEK_SET); // jump to sector2(root_dir_entry)?

	puts("[          File Name          ] [ type ] [   size   ] [ delete ]");
	uint8_t buff[FAT32_ENTRY];
	struct FAT32_Dir_Entry *fat32_Dir_E;
	do{
		fat32_Dir_E = (struct FAT32_Dir_Entry*)buff;
		fread(fat32_Dir_E, 1, FAT32_ENTRY, rfp);

		wstring lfn;
		string sfn;
		int i = 0;
		struct FAT32_LFN_Entry *fat32_lfn;

		switch(fat32_Dir_E->attribute >> 4) {
		case 0x0:		// 0x0?
			switch(fat32_Dir_E->attribute & 0x0f) {
			case 0x0:	// 0x00(file x)
				break;
			case 0x8:	// 0x08(Volume Label)
				sfn.clear();
				sfn.append(fat32_Dir_E->name, sizeof(fat32_Dir_E->name));
				printf(" %s\n", sfn.c_str());
				break;

			case 0xf:	// 0x0f(LFN)
				fat32_lfn = (struct FAT32_LFN_Entry*)buff;
				lfn.clear();
				lfn.append(fat32_lfn->name1, sizeof(fat32_lfn->name1)/sizeof(wchar_t));
				lfn.append(fat32_lfn->name2, sizeof(fat32_lfn->name2)/sizeof(wchar_t));
				lfn.append(fat32_lfn->name3, sizeof(fat32_lfn->name3)/sizeof(wchar_t));
				replace(lfn.begin(), lfn.end(), '\xff', '\x00');
				while (fat32_lfn->attribute == 0x0f) {
					i++;
					fread(fat32_lfn, 1, FAT32_ENTRY, rfp);
					if(fat32_lfn->attribute != 0x0f) break;
					wstring temp_lfn(fat32_lfn->name1, fat32_lfn->name1 + sizeof(fat32_lfn->name2)/sizeof(wchar_t)-1);
					temp_lfn.append(fat32_lfn->name2, sizeof(fat32_lfn->name2)/sizeof(wchar_t));
					temp_lfn.append(fat32_lfn->name3, sizeof(fat32_lfn->name3)/sizeof(wchar_t));
					replace(temp_lfn.begin(), temp_lfn.end(), '\xff', '\x00');
					lfn.insert(0, temp_lfn);
				}
				if(i==1) {
					sfn.clear();
					WideCharToMultiByte(CP_ACP, 0, &lfn[0], -1, &sfn[0], 256, NULL, NULL);
					printf(" %-32s", sfn.c_str());
				}
				else		wprintf(L" %-30ls  ", lfn.c_str());

				/* printf Type */
				switch (fat32_Dir_E->attribute >> 4) {
				case 1:
					printf("Dir      ");
					break;
				case 2:
					printf("File     ");
					break;
				default:
					printf("Etc...   ");
					break;
				}

				printf("%-13u", fat32_Dir_E->file_size);
				if(fat32_Dir_E->name[0] == '\xe5') { printf(" Delete"); }
				puts("");
				break;
			default:	// 0x01, 0x02, 0x04, etc...
				printf("\nupdate Attribute : %02x\n\n", fat32_Dir_E->attribute);
				break;
			}
			break;
		case 0x1:		// 0x1?(Directory)
		case 0x2:		// 0x2?(Archive File)
			sfn.clear();
			sfn.append(fat32_Dir_E->name, sizeof(fat32_Dir_E->name));
			sfn.append(fat32_Dir_E->ext, sizeof(fat32_Dir_E->ext));
			printf(" %-32s", sfn.c_str());

			/* printf Type */
			switch (fat32_Dir_E->attribute >> 4) {
			case 1:
				printf("Dir      ");
				break;
			case 2:
				printf("File     ");
				break;
			default:
				printf("Etc...   ");
				break;
			}

			printf("%-13u", fat32_Dir_E->file_size);
			if (fat32_Dir_E->name[0] == '\xe5') { printf(" Delete"); }
			puts("");
			break;
		default:		// etc...
			printf("\nupdate Attribute : %02x\n\n", fat32_Dir_E->attribute);
			break;
		}
	}while((fat32_Dir_E->name[0])&(0xff)); // FileName[0] == 0x00 -> break;
	free(fat32);
	fclose(rfp);

	return 0;
}
