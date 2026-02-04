#include "fat32.h"
#include "ata.h"

static FAT32_Context fs_ctx;

void fat32_init(uint32_t partition_lba) {
    struct FAT32_BootSector boot;
    disk_read_sectors(partition_lba, 1, &boot);

    fs_ctx.partition_lba_start = partition_lba;
    fs_ctx.bytes_per_sector = boot.bytes_per_sector;
    fs_ctx.sectors_per_cluster = boot.sectors_per_cluster;
    fs_ctx.fat_start_sector = partition_lba + boot.reserved_sectors;
    fs_ctx.fat_size = boot.fat_size_32;

    uint32_t root_dir_sectors = ((boot.root_entries * 32) + (boot.bytes_per_sector - 1)) / boot.bytes_per_sector;

    fs_ctx.data_start_sector = fs_ctx.fat_start_sector + (boot.num_fats * boot.fat_size_32);
    fs_ctx.root_cluster = boot.root_cluster;
}

uint32_t fat32_read_fat(uint32_t cluster) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = fs_ctx.fat_start_sector + (fat_offset / fs_ctx.bytes_per_sector);
    uint32_t entry_offset = fat_offset % fs_ctx.bytes_per_sector;

    uint8_t buffer[512];
    disk_read_sectors(fat_sector, 1, buffer);

    uint32_t* fat_entry = (uint32_t*)&buffer[entry_offset];
    return (*fat_entry) & 0x0FFFFFFF;
}

void fat32_write_fat(uint32_t cluster, uint32_t value) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = fs_ctx.fat_start_sector + (fat_offset / fs_ctx.bytes_per_sector);
    uint32_t entry_offset = fat_offset % fs_ctx.bytes_per_sector;

    uint8_t buffer[512];
    disk_read_sectors(fat_sector, 1, buffer);

    uint32_t* fat_entry = (uint32_t*)&buffer[entry_offset];
    *fat_entry = (*fat_entry & 0xF0000000) | (value & 0x0FFFFFFF);

    disk_write_sectors(fat_sector, 1, buffer);
}

uint32_t fat32_find_free_cluster() {
    for (uint32_t cluster = 2; cluster < 0x0FFFFFF7; cluster++) {
        if (fat32_read_fat(cluster) == 0) {
            return cluster;
        }
    }
    return 0;
}

uint32_t cluster_to_lba(uint32_t cluster) {
    return fs_ctx.data_start_sector + ((cluster - 2) * fs_ctx.sectors_per_cluster);
}

int fat32_read_file(uint32_t first_cluster, void* buffer, uint32_t size) {
    uint32_t cluster = first_cluster;
    uint32_t bytes_read = 0;
    uint8_t cluster_buffer[512 * fs_ctx.sectors_per_cluster];

    while (cluster < 0x0FFFFFF8 && bytes_read < size) {
        uint32_t lba = cluster_to_lba(cluster);
        disk_read_sectors(lba, fs_ctx.sectors_per_cluster, cluster_buffer);
        uint32_t bytes_to_copy = (size - bytes_read < sizeof(cluster_buffer)) ? (size - bytes_read) : sizeof(cluster_buffer);

        memcpy((uint8_t*)buffer + bytes_read, cluster_buffer, bytes_to_copy);
        bytes_read += bytes_to_copy;

        cluster = fat32_read_fat(cluster);
    }
    return bytes_read;
}

int fat32_write_file(uint32_t first_cluster, const void* buffer, uint32_t size) {
    uint32_t cluster = first_cluster;
    uint32_t bytes_written = 0;
    uint32_t cluster_size = fs_ctx.bytes_per_sector * fs_ctx.sectors_per_cluster;

    while (bytes_written < size) {
        uint8_t cluster_buffer[cluster_size];
        uint32_t bytes_to_write = (size - bytes_written < cluster_size) ? (size - bytes_written) : cluster_size;

        memcpy(cluster_buffer, (uint8_t*)buffer + bytes_written, bytes_to_write);

        uint32_t lba = cluster_to_lba(cluster);
        disk_write_sectors(lba, fs_ctx.sectors_per_cluster, cluster_buffer);

        bytes_written += bytes_to_write;

        if (bytes_written < size) {
            uint32_t next_cluster = fat32_read_fat(cluster);
            if (next_cluster >= 0x0FFFFFF8) {
                next_cluster = fat32_find_free_cluster();
                if (next_cluster == 0) return -1;

                fat32_write_fat(cluster, next_cluster);
                fat32_write_fat(next_cluster, 0x0FFFFFFF);
            }
            cluster = next_cluster;
        }
    }

    return bytes_written;
}

struct FAT32_DirEntry* fat32_find_file(uint32_t dir_cluster, const char* name) {
    static struct FAT32_DirEntry entries[16];  // One sector worth
    uint32_t cluster = dir_cluster;

    while (cluster < 0x0FFFFFF8) {
        uint32_t lba = cluster_to_lba(cluster);

        for (uint32_t sector = 0; sector < fs_ctx.sectors_per_cluster; sector++) {
            disk_read_sectors(lba + sector, 1, entries);

            for (int i = 0; i < 16; i++) {
                if (entries[i].name[0] == 0x00) return NULL;  // End of directory
                if (entries[i].name[0] == 0xE5) continue;     // Deleted entry

                if (memcmp(entries[i].name, name, 11) == 0) {
                    return &entries[i];
                }
            }
        }
        cluster = fat32_read_fat(cluster);
    }
    return NULL;
}

int fat32_create_file(uint32_t dir_cluster, const char* name, uint32_t first_cluster, uint32_t size) {
    struct FAT32_DirEntry entries[16];
    uint32_t cluster = dir_cluster;

    while (cluster < 0x0FFFFFF8) {
        uint32_t lba = cluster_to_lba(cluster);
        for (uint32_t sector = 0; sector < fs_ctx.sectors_per_cluster; sector++) {
            disk_read_sectors(lba + sector, 1, entries);
            for (int i = 0; i < 16; i++) {
                if (entries[i].name[0] == 0x00 || entries[i].name[0] == 0xE5) {
                    memcpy(entries[i].name, name, 11);
                    entries[i].attributes = 0x20;
                    entries[i].first_cluster_lo = first_cluster & 0xFFFF;
                    entries[i].first_cluster_hi = (first_cluster >> 16) & 0xFFFF;
                    entries[i].file_size = size;
                    disk_write_sectors(lba + sector, 1, entries);
                    return 0;
                }
            }
        }
        cluster = fat32_read_fat(cluster);
    }
    return -1;
}