SECTOR_OF_ROOTDIR   equ 19
BYTES_PER_SECTOR    equ 200H
SECTORS_PER_TRACK   equ 18
TOTAL_ROOTDIR_SEC   equ 14
DIRENTRY_PER_SEC    equ 16
TOTAL_ROOTDIR_ENTRY equ 224 
OFFSET_OF_DATASEC	equ 31

;BPB here
BS_OEMName          db "PURETEAR"
BPB_BytesPerSec     dw 0200H
BPB_SecPerClus      db 1
BPB_RsvdSecCnt      dw 1
BPB_NumFATs         db 2
BPB_RootEntCnt      dw 0E0H
BPB_TotSec16        dw 0B40H
BPB_Media           db 0xF0
BPB_FATSz16         dw 9
BPB_SecPerTrk       dw 12H 
BPB_NumHeads        dw 2
BPB_HiddenSec       dd 0
BPB_TotSec32        dd 0
BS_DrvNum           db 0
BS_Reserved1        db 0
BS_BootSig          db 0x29
BS_VolID            dd 0
BS_VolLab           db "puretears  "
BS_FileSysType      db "FAT12   "   

