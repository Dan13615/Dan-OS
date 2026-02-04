#include "ata.h"

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void insw(uint16_t port, void* addr, uint32_t count) {
    __asm__ volatile ("rep insw" : "+D"(addr), "+c"(count) : "d"(port) : "memory");
}

static inline void outsw(uint16_t port, const void* addr, uint32_t count) {
    __asm__ volatile ("rep outsw" : "+S"(addr), "+c"(count) : "d"(port) : "memory");
}

static void ata_io_wait(void) {
    inb(ATA_PRIMARY_STATUS);
    inb(ATA_PRIMARY_STATUS);
    inb(ATA_PRIMARY_STATUS);
    inb(ATA_PRIMARY_STATUS);
}

static int ata_wait_busy(void) {
    uint8_t status;
    int timeout = 100000;

    while (timeout--) {
        status = inb(ATA_PRIMARY_STATUS);
        if (!(status & ATA_STATUS_BSY)) {
            return 0;
        }
    }
    return -1;
}

static int ata_wait_drq(void) {
    uint8_t status;
    int timeout = 100000;

    while (timeout--) {
        status = inb(ATA_PRIMARY_STATUS);
        if (status & ATA_STATUS_DRQ) {
            return 0;
        }
        if (status & ATA_STATUS_ERR) {
            return -1;
        }
    }
    return -1;
}

int ata_init(void) {
    outb(ATA_PRIMARY_DRIVE_HEAD, 0xA0);
    ata_io_wait();

    if (ata_wait_busy() != 0) {
        return -1;
    }

    outb(ATA_PRIMARY_COMMAND, ATA_CMD_IDENTIFY);
    ata_io_wait();

    if (ata_wait_drq() != 0) {
        return -1;
    }

    uint16_t identify_data[256];
    for (int i = 0; i < 256; i++) {
        identify_data[i] = inw(ATA_PRIMARY_DATA);
    }

    return 0;
}

int disk_read_sectors(uint32_t lba, uint32_t count, void* buffer) {
    if (count == 0 || count > 256) {
        return -1;
    }

    uint16_t* buf = (uint16_t*)buffer;

    if (ata_wait_busy() != 0) {
        return -1;
    }

    outb(ATA_PRIMARY_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));  // LBA mode, master, LBA[27:24]
    outb(ATA_PRIMARY_SECCOUNT, (uint8_t)count);                  // Sector count
    outb(ATA_PRIMARY_LBA_LO, (uint8_t)lba);                      // LBA[7:0]
    outb(ATA_PRIMARY_LBA_MID, (uint8_t)(lba >> 8));              // LBA[15:8]
    outb(ATA_PRIMARY_LBA_HI, (uint8_t)(lba >> 16));              // LBA[23:16]
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_READ_PIO);                 // READ command

    for (uint32_t i = 0; i < count; i++) {
        if (ata_wait_drq() != 0) {
            return -1;
        }

        insw(ATA_PRIMARY_DATA, buf, 256);
        buf += 256;
    }
    return 0;
}

int disk_write_sectors(uint32_t lba, uint32_t count, const void* buffer) {
    if (count == 0 || count > 256) {
        return -1;
    }

    const uint16_t* buf = (const uint16_t*)buffer;

    if (ata_wait_busy() != 0) {
        return -1;
    }

    outb(ATA_PRIMARY_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECCOUNT, (uint8_t)count);
    outb(ATA_PRIMARY_LBA_LO, (uint8_t)lba);
    outb(ATA_PRIMARY_LBA_MID, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HI, (uint8_t)(lba >> 16));
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_WRITE_PIO);

    for (uint32_t i = 0; i < count; i++) {
        if (ata_wait_drq() != 0) {
            return -1;
        }

        outsw(ATA_PRIMARY_DATA, buf, 256);
        buf += 256;
        outb(ATA_PRIMARY_COMMAND, 0xE7);  // CACHE FLUSH
        ata_wait_busy();
    }
    return 0;
}