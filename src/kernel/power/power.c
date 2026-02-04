/**========================================================================
 *                             power.c
 *  Power management implementation for system shutdown
 *  Author: Daniil Stepanov
 *  Date: 2025-10-04
 *========================================================================**/

#include "power.h"
#include "tty.h"
#include "string.h"

extern void ioport_out(unsigned short port, unsigned char value);
extern unsigned char ioport_in(unsigned short port);

static struct fadt* fadt_table = NULL;
static int acpi_initialized = 0;

// Memory mapping for ACPI tables
static uint8_t* find_rsdp(void) {
    uint16_t ebda = *(uint16_t*)0x40E;
    ebda <<= 4;

    // Search in EBDA (first 1KB)
    for (uint32_t addr = ebda; addr < ebda + 1024; addr += 16) {
        if (*(uint64_t*)addr == 0x2052545020445352ULL) { // "RSD PTR "
            return (uint8_t*)addr;
        }
    }

    // Search in BIOS area
    for (uint32_t addr = 0xE0000; addr < 0x100000; addr += 16) {
        if (*(uint64_t*)addr == 0x2052545020445352ULL) { // "RSD PTR "
            return (uint8_t*)addr;
        }
    }
    return NULL;
}

static int validate_checksum(uint8_t* table, uint32_t length) {
    uint8_t sum = 0;
    for (uint32_t i = 0; i < length; i++) {
        sum += table[i];
    }
    return sum == 0;
}

int init_acpi(void) {
    // Find RSDP
    uint8_t* rsdp_ptr = find_rsdp();
    if (!rsdp_ptr) {
        print_error("/!\\ RSDP not found");
        return -1;
    }
    struct rsdp_descriptor* rsdp = (struct rsdp_descriptor*)rsdp_ptr;
    // Validate RSDP checksum
    if (!validate_checksum((uint8_t*)rsdp, sizeof(struct rsdp_descriptor))) {
        print_error("/!\\ RSDP checksum invalid");
        return -1;
    }
    struct acpi_sdt_header* rsdt = (struct acpi_sdt_header*)(uint64_t)rsdp->rsdt_address;
    if (strncmp(rsdt->signature, "RSDT", 4) != 0) {
        print_error("/!\\ RSDT signature invalid");
        return -1;
    }
    if (!validate_checksum((uint8_t*)rsdt, rsdt->length)) {
        print_error("/!\\ RSDT checksum invalid");
        return -1;
    }

    uint32_t entries = (rsdt->length - sizeof(struct acpi_sdt_header)) / 4;
    uint32_t* entry_ptr = (uint32_t*)((uint8_t*)rsdt + sizeof(struct acpi_sdt_header));
    for (uint32_t i = 0; i < entries; i++) {
        struct acpi_sdt_header* header = (struct acpi_sdt_header*)(uint64_t)entry_ptr[i];
        if (strncmp(header->signature, "FACP", 4) == 0) {
            fadt_table = (struct fadt*)header;
            break;
        }
    }
    if (!fadt_table) {
        print_error("/!\\ FADT not found");
        return -1;
    }
    if (!validate_checksum((uint8_t*)fadt_table, fadt_table->header.length)) {
        print_error("/!\\ FADT checksum invalid");
        return -1;
    }
    acpi_initialized = 1;
    return 0;
}

void acpi_poweroff(void) {
    if (!acpi_initialized || !fadt_table) {
        print_error("/!\\ ACPI not initialized, trying legacy methods");
        legacy_poweroff();
        return;
    }
    if (fadt_table->smi_command_port != 0 && fadt_table->acpi_enable != 0) {
        ioport_out(fadt_table->smi_command_port, fadt_table->acpi_enable);
        // Wait for ACPI to be enabled
        for (int i = 0; i < 300; i++) {
            if (fadt_table->pm1a_control_block != 0) {
                uint16_t pm1_control = ioport_in(fadt_table->pm1a_control_block);
                if (pm1_control & 1) break; // SCI_EN bit set
            }
            // Simple delay
            for (volatile int j = 0; j < 10000; j++);
        }
    }
    // Send shutdown command via PM1a control register
    if (fadt_table->pm1a_control_block != 0) {
        // SLP_TYPa = 5 (shutdown), SLP_EN = 1
        uint16_t shutdown_value = (5 << 10) | (1 << 13);
        ioport_out(fadt_table->pm1a_control_block, shutdown_value & 0xFF);
        ioport_out(fadt_table->pm1a_control_block + 1, (shutdown_value >> 8) & 0xFF);
    }
    // If PM1b exists, send to it too
    if (fadt_table->pm1b_control_block != 0) {
        uint16_t shutdown_value = (5 << 10) | (1 << 13);
        ioport_out(fadt_table->pm1b_control_block, shutdown_value & 0xFF);
        ioport_out(fadt_table->pm1b_control_block + 1, (shutdown_value >> 8) & 0xFF);
    }
    // If ACPI didn't work, fall back to legacy methods
    print_error("/!\\ ACPI shutdown failed, trying legacy methods");
    legacy_poweroff();
}

void legacy_poweroff(void) {
    // Method 1: QEMU/Bochs - write 'shutdown' to port 0x8900
    const char* qemu_shutdown = "shutdown";
    for (int i = 0; qemu_shutdown[i]; i++) {
        ioport_out(0x8900, qemu_shutdown[i]);
    }
    // Method 2: VirtualBox - write to port 0x4004
    ioport_out(0x4004, 0x3400 & 0xFF);
    // Method 3: VMware - write to port 0x1004
    ioport_out(0x1004, 0x3400 & 0xFF);
    // Method 4: Try APM (Advanced Power Management)
    // APM BIOS call via port 0x53
    ioport_out(0x53, 0x00); // APM function: Set Power State
    ioport_out(0x53, 0x03); // Power state: Off
    // Method 5: PS/2 Controller reset (last resort)
    // This will reset the system rather than power off
    asm volatile("cli");
    uint8_t temp;
    do {
        temp = ioport_in(0x64);
        if (temp & 0x01) {
            ioport_in(0x60);
        }
    } while (temp & 0x02);
    ioport_out(0x64, 0xFE); // Reset system
    // If all else fails, halt
    print_error("/!\\All shutdown methods failed.");
}

void poweroff(void) {
    // Try ACPI first
    if (acpi_initialized) {
        acpi_poweroff();
    } else {
        if (init_acpi() == 0) {
            acpi_poweroff();
        } else {
            legacy_poweroff();
        }
    }
}