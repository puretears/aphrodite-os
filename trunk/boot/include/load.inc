LOADER_BASE	equ 09000H
LOADER_OFFSET	equ 200H

KERNEL_BASE	equ 08000H
KERNEL_OFFSET	equ 0H

LOADERBASE_PHY_ADDR	equ LOADER_BASE * 16
KERNELBASE_PHY_ADDR	equ KERNEL_BASE * 16
KERNEL_ENTRYPOINT_PHY_ADDR equ 00030400H


PAGE_DIR_BASE	equ 200000H
PAGE_TBL_BASE	equ 201000H