	
	; BPB here
	bpb_oem_name 		db "puretear"
	bpb_bytes_per_sec 	dw 512
	bpb_sects_per_cls 	db 1
	bpb_resrv 		dw 1
	bpb_fat_table 		db 2
	bpb_rootdir 		dw 0E0H
	bpb_total_sects 	dw 0B40H
	bpb_media_type 		db 0F0H
	bpb_sects_per_fat 	dw 9
	bpb_sects_per_track 	dw 012H
	bpb_heads 		dw 2
	bpb_hidden_sects 	dw 0

; Some useful constants
ROOTDIR_SECTORS		equ 14
ROOTDIR_SECTOR_NUMBER	equ 19
FAT1_SECTOR_NUMBER	equ 1
DELTA_NO		equ 17


