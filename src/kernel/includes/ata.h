/**========================================================================
 *                               ATA DRIVER
 *   description: Header file for the ATA driver.
 *   Author: Daniil Stepanov
 *   Date: 2026-02-03
 *
 *========================================================================**/

 #ifndef ATA_H
     #define ATA_H

    #include <stdint.h>

    // ATA I/O Ports (Primary Bus)
    #define ATA_PRIMARY_DATA       0x1F0
    #define ATA_PRIMARY_ERROR      0x1F1
    #define ATA_PRIMARY_SECCOUNT   0x1F2
    #define ATA_PRIMARY_LBA_LO     0x1F3
    #define ATA_PRIMARY_LBA_MID    0x1F4
    #define ATA_PRIMARY_LBA_HI     0x1F5
    #define ATA_PRIMARY_DRIVE_HEAD 0x1F6
    #define ATA_PRIMARY_STATUS     0x1F7
    #define ATA_PRIMARY_COMMAND    0x1F7

    // ATA Status Bits
    #define ATA_STATUS_BSY  0x80    // Busy
    #define ATA_STATUS_DRDY 0x40    // Drive ready
    #define ATA_STATUS_DF   0x20    // Drive fault
    #define ATA_STATUS_ERR  0x01    // Error
    #define ATA_STATUS_DRQ  0x08    // Data request ready

    // ATA Commands
    #define ATA_CMD_READ_PIO       0x20
    #define ATA_CMD_READ_PIO_EXT   0x24
    #define ATA_CMD_WRITE_PIO      0x30
    #define ATA_CMD_WRITE_PIO_EXT  0x34
    #define ATA_CMD_IDENTIFY       0xEC

    int ata_init(void);
    int disk_read_sectors(uint32_t lba, uint32_t count, void* buffer);
    int disk_write_sectors(uint32_t lba, uint32_t count, const void* buffer);


    struct MBR {
        uint8_t bootstrap_code[446];
        struct PartitionEntry {
            uint8_t  status;           // 0x80 = bootable, 0x00 = inactive
            uint8_t  first_chs[3];     // CHS address (legacy)
            uint8_t  partition_type;   // 0x0B = FAT32, 0x83 = Linux, etc.
            uint8_t  last_chs[3];
            uint32_t lba_start;        // Starting LBA sector
            uint32_t num_sectors;      // Size in sectors
        } partitions[4];
        uint16_t signature;            // 0xAA55
    } __attribute__((packed));


 #endif // ATA_H