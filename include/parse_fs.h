#pragma once
#include <stdint.h>			// uint
#include <wchar.h>			// wchar_t

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

/* FAT32 Parsing */
struct FAT32_body {
	uint32_t FAT_size32;			// FAT Size 32
	uint16_t ext_flags;				// Ext Flags(0x00)
	uint16_t fs_ver;				// File System Version(0x00)
	uint32_t root_dir_cluster;		// Root Dir Cluster(2)
	uint16_t fs_info;				// File System Info(1)
	uint16_t boot_record_backup_sec;// Boot Record Backup Sec(6)
	uint8_t reserved[12];			// Reserved(0)
	uint8_t dirve_num;				// Drive Number(1)
	uint8_t reserved1;				// Reserved1(0)
	uint8_t boot_signature;			// Boot Signature(0x29)
	uint32_t vol_ID;				// Volume ID
	uint8_t vol_label[11];			// Volume Label
	uint8_t fs_type[8];				// File System Type(FAT32)
};

struct FAT32_hdr {
	uint8_t jbc[3];					// Jump Boot Code(EB 3c 90)
	uint64_t oem_name;				// OEM Name
	uint16_t bps;					// Bytes Per Sector(512)
	uint8_t spc;					// Sector Per Cluster(8)
	uint16_t rsc;					// Reserved Sector Count(FAT16 = 1 / FAT32 = 32) v
	uint8_t num_Fats;				// Number of FATs(2) v
	uint16_t root_dir_entry_count;	// Root Dir Entry Count(FAT16 = 512 / FAT32 = 0)
	uint16_t total_sector16;		// Total Sector 16(FAT32 = 0 / FAT16 = ?)
	uint8_t media;					// Media(0xf8)
	uint16_t FAT_size16;			// FAT Size 16(FAT32 = 0)
	uint16_t sector_per_track;		// Sector Per Track(63)
	uint16_t num_of_heads;			// Number of Heads(255)
	uint32_t hidden_sector;			// Hidden Sector(32)
	uint32_t total_sector32;		// Total Sector 32(FAT32��)
	struct FAT32_body fatbody;			// struct fat32 header
};

struct FAT32_Dir_Entry {
	char name[8];				// Name ���ϸ�, �⺻������ �����̳� ���丮 �����ִ� 8�ڸ��� ����, �빮�ڸ� �����ϰ� ������� 0x20(�����̽�)�� ä���.
	char ext[3];					// Ext Ȯ���� �ִ� 3�ڸ�, Name �� �����ѼӼ��� ������.
	uint8_t attribute;				// Attribute
	uint8_t nt_resource;			// NT Resource(���ప:0)
	uint8_t create_time_tenth;		// Create Time Tenth(������ ������ �ð��� 1/10�� ������ ������׸�)
	uint16_t create_time;			// Create Time
	uint16_t create_date;			// Create Date
	uint16_t last_access_date;		// Last Access Date(�ֱ� ���� ��¥�� ���)

	uint16_t fir_cluster_high2byte;	// First Cluster High 2 bytes
	uint16_t write_time;			// Write Time
	uint16_t write_date;			// Write Date
	uint16_t fir_cluster_low2byte;	// First Cluster Low 2 Bytes
	uint32_t file_size;				// Filesize(Directory == 0)
};

struct FAT32_LFN_Entry {
	uint8_t order;					// Order // LFN�� ���ĵ� ������ �����. 6��° ��Ʈ(0x40)�� 1�̸� ������ �ش� ������ ������ LFN Entry��
	wchar_t name1[5];				// Name1 // UTF-16���� �ѱ��ڴ� 2 bytes�� ����
	uint8_t attribute;				// Attribute(0x0F)
	uint8_t type;					// Type(0)
	uint8_t check_sum;				// Check Sum(Short File Name Check Sum)
	wchar_t name2[6];				// Name2
	uint16_t fir_cluster_low;		// First Cluster Low(0)
	wchar_t name3[2];				// Name3 // 10 + 12 + 4 = 26 ���� �� 13�ڰ� ����
};
#pragma pack(pop)

/* FAT32 Attribute define */
/* ���߿� Ŭ���� �����ؼ� ������ �� */
#define ATTR_RO			0x01		// Attribute Read Only
#define ATTR_H			0x02		// Attribute Hidden
#define ATTR_SYS		0x04		// Attribute System(OS use)
#define ATTR_VL			0x08		// Attribute Volume Label
#define ATTR_DIR		0x10		// Attribute Directory
#define ATTR_ARCHIVE	0x20		// Attribute Archive File
#define ATTR_LFN		0xF0		// Attribute Long File Name Entry

/* FAT32 ENTRY define */
/* ���߿� Ŭ���� �����ؼ� ������ �� */
#define FAT32_ENTRY		0x20		// 32(10)

#define BF_80			0x80
#define BF_00			0x00

#define TYPE_NO			0x00
#define TYPE_EP			0x05
#define TYPE_NTFS		0x07
#define TYPE_exFAT		0x07

#define BR_SIGNATURE	0xAA55	// 

#define SECTOR_SIZE		0x200	// 512(10)
#define ENTRY_SIZE		0x80	// 128(10)