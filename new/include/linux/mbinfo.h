#ifndef MBINFO_H
#define MBINFO_H

#include "type.h"

struct mmap {
	u_int size;
	u_int64 addr;
	u_int64 limit;
	u_int type;
};

struct mbinfo {
	u_int flags;			// Required
	u_int mem_lower;		// Present if flags[0] is set
	u_int mem_upper;
	u_int boot_device;		// Present if flags[1] is set
	u_int cmdline;			// Present if flags[2] is set
	u_int mods_count;		// Present if flags[3] is set
	u_int mods_addr;
	u_char syms[16];		// Present if flags[4] or [5] is set
	u_int mmap_length;		// Present if flags[6] is set
	mmap *mmap_addr;
	u_int drives_length;	// Present if flags[7] is set
	u_int drives_addr;	
	u_int config_table;		// Present if flags[8] is set
	u_int boot_loader_name; // Present if flags[9] is set
	u_int apm_table;		// Present if flags[10] is set
	u_int vbe_control_info;	// Present if flags[11] is set
	u_int vbe_mode_info;
	u_int vbe_mode;
	u_int vbe_interface_seg;
	u_int vbe_interface_off;
	u_int vbe_interface_len;
};

#endif
