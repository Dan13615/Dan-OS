/**========================================================================
 *                             Fat32 Header File
 *  description: This header file defines structures and constants for FAT32
 *  author: Daniil Stepanov
 *  date: 2026-02-03
 *========================================================================**/

#ifndef FAT32_H
    #define FAT32_H

    #include <stdint.h>
    #include "string.h"


    struct FAT32_BootSector {
        uint8_t  jump[3];
        uint8_t  oem_name[8];
        uint16_t bytes_per_sector;     // Usually 512
        uint8_t  sectors_per_cluster;  // Power of 2
        uint16_t reserved_sectors;     // Usually 32 for FAT32
        uint8_t  num_fats;             // Usually 2
        uint16_t root_entries;         // 0 for FAT32
        uint16_t total_sectors_16;     // 0 for FAT32
        uint8_t  media_descriptor;
        uint16_t fat_size_16;          // 0 for FAT32
        uint16_t sectors_per_track;
        uint16_t num_heads;
        uint32_t hidden_sectors;
        uint32_t total_sectors_32;

        // FAT32 Extended
        uint32_t fat_size_32;
        uint16_t flags;
        uint16_t version;
        uint32_t root_cluster;         // Usually 2
        uint16_t fsinfo_sector;
        uint16_t backup_boot_sector;
        uint8_t  reserved[12];
        uint8_t  drive_number;
        uint8_t  reserved1;
        uint8_t  boot_signature;
        uint32_t volume_id;
        uint8_t  volume_label[11];
        uint8_t  fs_type[8];           // "FAT32   "
    } __attribute__((packed));

    struct FAT32_DirEntry {
        uint8_t  name[11];           // 8.3 format (padded with spaces)
        uint8_t  attributes;         // READ_ONLY=0x01, HIDDEN=0x02, SYSTEM=0x04, 
                                    // VOLUME_ID=0x08, DIRECTORY=0x10, ARCHIVE=0x20
        uint8_t  reserved;
        uint8_t  creation_time_ms;
        uint16_t creation_time;
        uint16_t creation_date;
        uint16_t access_date;
        uint16_t first_cluster_hi;   // High 16 bits of first cluster
        uint16_t modified_time;
        uint16_t modified_date;
        uint16_t first_cluster_lo;   // Low 16 bits of first cluster
        uint32_t file_size;
    } __attribute__((packed));

    typedef struct {
        uint32_t partition_lba_start;
        uint32_t fat_start_sector;
        uint32_t data_start_sector;
        uint32_t root_cluster;
        uint32_t sectors_per_cluster;
        uint32_t bytes_per_sector;
        uint32_t fat_size;
    } FAT32_Context;

    void fat32_init(uint32_t partition_lba);
    uint32_t fat32_read_fat(uint32_t cluster);
    void fat32_write_fat(uint32_t cluster, uint32_t value);
    uint32_t fat32_find_free_cluster(void);
    uint32_t cluster_to_lba(uint32_t cluster);
    int fat32_read_file(uint32_t first_cluster, void* buffer, uint32_t size);
    int fat32_write_file(uint32_t first_cluster, const void* buffer, uint32_t size);
    struct FAT32_DirEntry* fat32_find_file(uint32_t dir_cluster, const char* name);
    int fat32_create_file(uint32_t dir_cluster, const char* name, uint32_t first_cluster, uint32_t size);

#endif // FAT32_H
