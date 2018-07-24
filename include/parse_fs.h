#pragma once
#include <stdint.h>			// uint

#pragma pack(push, 1)
struct part_tb_hdr {
	uint8_t boot_flag;		// Booting Flag
	uint8_t chs_s_addr[3];
	uint8_t partition_type;
	uint8_t chs_e_addr[3];
	uint32_t lba_s_addr;
	uint32_t partition_size;
};

struct mbr_hdr {
	uint8_t boot_code[446];
	struct part_tb_hdr partition_table[4];
	uint16_t signature;
};

struct gpt_hdr {
	uint64_t signature;		// GPT Signature
	uint32_t revision;
	uint32_t header_size;	// Header Size(Little Endian)
	uint32_t crc_header;	// CRC32 of header
	uint32_t reserved;
	uint64_t currentLBA;	// Current LBA(location of header)
	uint64_t backupLBA;		// Backup LBA(Location of backup header)
	uint64_t usableLBA1st;	// First usable LBA(usually 34)
	uint64_t usableLBAend;	// Last usable LBA
	uint8_t diskGUID[16];
	uint64_t PEstartLBA;	// Partition Entries starting LBA(2)
	uint32_t num_PE;		// Number of partition entries
	uint32_t entry_size;	// Size of partition entry(128)
	uint32_t crc_array;		// CRC32 of partition array
};

struct gpt_Entry128_hdr {
	uint8_t PT_GUID[16];	// Partition Type GUID
	uint8_t UP_GUID[16];	// Unique Partition GUID
	uint64_t fir_LBA;		// First LBA
	uint64_t last_LBA;		// Last LBA
	uint64_t attr_flag;		// Attribute flags
	uint8_t UTF_16LE[72];	// Partition name 36 UTF-16LE
};
#pragma pack(pop)

#define BF_80			0x80
#define BF_00			0x00

#define TYPE_NO			0x00
#define TYPE_EP			0x05
#define TYPE_NTFS		0x07
#define TYPE_exFAT		0x07

#define BR_SIGNATURE	0xAA55	// 

#define SECTOR_SIZE		0x200	// 512(10)
#define ENTRY_SIZE		0x80	// 128(10)