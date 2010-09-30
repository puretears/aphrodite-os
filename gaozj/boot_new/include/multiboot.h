#ifndef MULTIBOOT
typedef unsigned int u_32;
typedef unsigned short u_16;
typedef unsigned char u_8;
typedef unsigned long long u_64;
struct mmap_struct{
	u_32 size;
	u_64 base;
	u_64 limit;
	u_32 type;
};
typedef struct mmap_struct mmap;

struct multi_info{
	u_32 flags;
	u_32 mem_lower;
	u_32 mem_upper;
	u_32 boot_device;
	u_32 cmdline;
	u_32 mods_count;
	u_32 mods_addr;
	u_8 syms[16];
	u_32 mmap_length;	/* Memory Mapping buffer */
	mmap *mmap_addr;
	u_32 drives_length;
	u_32 drives_addr;
	u_32 config_table;
	u_32 boot_loader_name;
	u_32 apm_table;
	u_32 vbe_contuol_info;
	u_32 vbe_mode_info;
	u_32 vbe_mode;
	u_32 vbe_interface_seg;
	u_32 vbe_interface_off;
	u_32 vbe_interface_len;
};
typedef struct multi_info multi_info_t;

#endif
