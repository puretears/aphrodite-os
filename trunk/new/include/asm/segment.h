#ifndef ASM_SEGMENT_H
#define ASM_SEGMENT_H

/* Simple GDT entries for booting only.*/
#define GDT_ENTRY_BOOT_CS 2
#define __BOOT_CS (GDT_ENTRY_BOOT_CS * 8)

#define GDT_ENTRY_BOOT_DS 3
#define __BOOT_DS (GDT_ENTRY_BOOT_DS * 8)

#define IDT_ENTRIES 256

#endif
