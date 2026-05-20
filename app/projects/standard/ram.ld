#include "config.h"

ENTRY(_start)

/* Define the flash max size */
__max_flash_size = FLASH_CODE_SIZE;

__base = 0x10000000;

/*  sram0空间分配图示,具体查看下方MEMORY项实际分配
|-------sram ------|0x0001 0000
    sram0
    |-----------|0x0001 0000
    |   boot
    |-----------|0x0001 0400
    |   stack
    |-----------|0x0001 0800
    |   data
    |-----------|0x0001 0C00
    |   comm
    |-----------|0x0001 1C00
    |           |-----------|0x0001 1C00
    |           |   music(pdn_comm)
    |   aram    |-----------|0x0001 1E00
    |           ||--------------0x0001 1E00-------------|
    |           ||          mp3/lsbc/esbc/midi/upd      |
    |           ||--------------0x0001 5000-------------|
    |-----------|0x0001 5000

    cache ram
    |-----------|0x0007 0000
    |   data1
    |-----------|0x0007 1000
|------- end ------|0x0001 5000
*/

/* Define the area */
__comm_vma              = 0x10C00;
__comm_ram_size         = 0x1000;
__data_vma              = 0x10800;
__data_ram_size         = 0x400;
__data1_vma             = 0x70000;
__data1_ram_size        = 0x1000;
__aram_vma              = 0x11C00;

/* 复用RAM临时当公共区*/
__aram_size             = 0x3400;

__pdn_comm_vma          = 0x11C00;  //开关机代码区

__aram_msc_comm_vma     = 0x11C00;  //music代码区起始地址

__aram_mp3_comm_vma     = 0x11E00;  //mp3代码区起始地址
__aram_mp3_vma          = 0x12A00;  //mp3变量区起始地址
__aram_mp3_size         = 0x2600;   //mp3变量区长度

__aram_esbc_comm_vma    = 0x11E00;  //esbc代码区起始地址
__aram_esbc_vma         = 0x14600;  //esbc变量区起始地址
__aram_esbc_size        = 0xA00;    //esbc变量区长度

__aram_lsbc_comm_vma    = 0x11E00;  //lsbc代码区起始地址
__aram_lsbc_vma         = 0x12600;  //lsbc变量区起始地址
__aram_lsbc_size        = 0x2A00;   //lsbc变量区长度

__aram_pitch_vma        = 0x12A00;  //wav_pitch变量区起始地址
__aram_pitch_size       = 0xa00;    //wav_pitch变量区长度

__aram_upd_vma          = 0x14600;  //upd变量区起始地址
__aram_upd_size         = 0xa00;    //upd变量区长度

__aram_midi_comm_vma    = 0x11E00;  //midi代码区起始地址
__aram_midi_vma         = 0x12100;  //midi变量区起始地址
__aram_midi_size        = 0x2F00;   //midi变量区长度

MEMORY
{
    init                : org = __base,                 len = 512
    flash(rx)           : org = __base + 512,           len = __max_flash_size
    comm(rx)            : org = __comm_vma,             len = __comm_ram_size
    pdn_comm(rx)        : org = __pdn_comm_vma,         len = 0x1000

    stack               : org = 0x10400,                len = 1k
    data                : org = __data_vma,             len = __data_ram_size
    data1               : org = __data1_vma,            len = __data1_ram_size
    aram                : org = __aram_vma,             len = __aram_size

    aram_msc_comm(rx)   : org = __aram_msc_comm_vma,    len = 0x200

    aram_mp3_comm(rx)   : org = __aram_mp3_comm_vma,    len = 0xC00
    aram_mp3            : org = __aram_mp3_vma,         len = __aram_mp3_size

    aram_esbc_comm(rx)  : org = __aram_esbc_comm_vma,   len = 0x2800
    aram_esbc           : org = __aram_esbc_vma,        len = __aram_esbc_size

    aram_lsbc_comm(rx)  : org = __aram_lsbc_comm_vma,   len = 0x800
    aram_lsbc           : org = __aram_lsbc_vma,        len = __aram_lsbc_size

    aram_midi_comm(rx)  : org = __aram_midi_comm_vma,   len = 0x300
    aram_midi           : org = __aram_midi_vma,        len = __aram_midi_size

    aram_pitch          : org = __aram_pitch_vma,       len = __aram_pitch_size /* 开wav变调需和MP3复用，不能叠加播放了 */

    aram_upd            : org = __aram_upd_vma,         len = __aram_upd_size
}

SECTIONS
{
    .init : {
        *(.reset)
    } > init

    .comm : {
        KEEP(*(.vector))
        *(.plt)
        *(.com_text*)
        *(.com_rodata*)
        *(.srodata*)
        *debug.o(.rodata*)
        *(.data*)
        *(.sdata*)
        . = ALIGN(512);
    } > comm AT > flash

    .pdn_comm : {
        *(.pdncom_text*)
        *(.pdncom_buf*)
        . = ALIGN(512);
    } > pdn_comm AT > flash

    .aram_msc_comm : {
        *(.music_text*)
        *(.music_buf)
        *(.ext_msc*)
        . = ALIGN(512);
    } > aram_msc_comm AT > flash

    .aram_mp3_comm : {
        *(.mp3_text*)
        . = ALIGN(512);
    } > aram_mp3_comm AT > flash

    .aram_esbc_comm : {
        *(.esbc.text*)
        . = ALIGN(512);
    } > aram_esbc_comm AT > flash

    .aram_lsbc_comm : {
        *(.lsbc_text*)
        . = ALIGN(512);
    } > aram_lsbc_comm AT > flash

    .aram_midi_comm : {
        *(.midi_text*)
        . = ALIGN(512);
    } > aram_midi_comm AT > flash

    .flash : {
        __code_start_update = .;
        *(.text.update*)
        . = ALIGN(512);
        __code_end_update = .;

        __code_start_drc = .;
        *(.drc_text*)
        *(.drc_rodata*)
        . = ALIGN(512);
        __code_end_drc = .;

        __code_start_lsbcdec = .;
        /* *(.text.lsbc.*) */
        /* *(.rodata.lsbc.*) */
        . = ALIGN(512);
        __code_end_lsbcdec = .;

        #if FFT_PROC_EN
        __code_start_fft = .;
        *(.text.fft*)
        *(.fft_rodata.windown)
        *(.fft_rodata.sqrt)
        . = ALIGN(512);
        __code_end_fft = .;
        #endif

        *(.text*)
        *(.wavdec*)
        *(.rodata*)
        *(.rela*)
        LONG(0)
        . = ALIGN(512);
    } > flash

    .stack (NOLOAD) : {
        __irq_stack_start = .;
        . = 0x400;              /*中断的栈空间*//*运行时的栈空间*/
        __irq_stack = .;
    } > stack
    __irq_stack_size = __irq_stack - __irq_stack_start;

    .bss (NOLOAD) : {
        __bss_start = .;
        *(COMMON)
        *(.bss*)
        *(.sbss*)
        *(.buf*)
        *(.huart_buf)
        *(.cm_buf)
        __bss_end = .;
    } > data
    __bss_size = __bss_end - __bss_start;

    .bss1 (NOLOAD) : {
        __bss1_start = .;
        *(.m_buf*)
        *(.gpdbuf*)
        *(.drc_buf*)
        *(.avio_buf)
        *(.uart_buf)
        *(.eq_buf)
        *(.msc_buf)
        *(.wav_buf)
        *(.lfs_buf*)

        #if PWM_HW_EN
        *(.pwm_buf)
        #endif

        #if FFT_PROC_EN
        *(.fft_buf*)
        #endif

        #if RGB_SERIAL_EN
        *(.spi_led_buf)
        #endif

        #if (!MUSIC_MP3_LONG_BUFFER)
        *(.mav_buf)
        #endif

        #if SPI1_AUDIO_EN
        *(.spi_buf)
            #if SPI1_AUDIO_TEST_EN
            *(.spi_tx_buf*)
            *(.spi_recv_buf*)
            #endif
        #endif

        #if MATRIX_XY_KEY_EN
        *(.matrix_buf)
        #endif

        __bss1_end = .;
    } > data1
    __bss1_size = __bss1_end - __bss1_start;

    .aram_mp3 __aram_mp3_vma (NOLOAD) : {
        *(.mp3_decram_buf*)
        *(.mp3buf.dec)
    } > aram_mp3

    .aram_esbc __aram_esbc_vma (NOLOAD) : {
        *(.esbc_buf*)
    } > aram_esbc

    .aram_lsbc __aram_lsbc_vma (NOLOAD) : {
        *(.lsbc_buf*)
    } > aram

    .aram_upd __aram_upd_vma (NOLOAD) : {
        *(.upd_buf*)
        *(.uart_upd_buf*)
    } > aram

    .aram_midi __aram_midi_vma (NOLOAD) : {
        *(.midi_buf*)
        *(.gsf_buf*)
    } > aram_midi

    .aram_pitch __aram_pitch_vma (NOLOAD) : {
        *(.pitch_buf)
    } > aram_pitch
}

/* Calc the lma */
__bank_size = SIZEOF(.flash);
__comm_lma = LOADADDR(.comm);
__comm_size = SIZEOF(.comm);
__pdn_comm_lma = LOADADDR(.pdn_comm);
__pdn_comm_size = SIZEOF(.pdn_comm);
__aram_msc_comm_lma = LOADADDR(.aram_msc_comm);
__aram_msc_comm_size = SIZEOF(.aram_msc_comm);
__aram_mp3_comm_lma = LOADADDR(.aram_mp3_comm);
__aram_mp3_comm_size = SIZEOF(.aram_mp3_comm);
__aram_esbc_comm_lma = LOADADDR(.aram_esbc_comm);
__aram_esbc_comm_size = SIZEOF(.aram_esbc_comm);
__aram_lsbc_comm_lma = LOADADDR(.aram_lsbc_comm);
__aram_lsbc_comm_size = SIZEOF(.aram_lsbc_comm);
__aram_midi_comm_lma = LOADADDR(.aram_midi_comm);
__aram_midi_comm_size = SIZEOF(.aram_midi_comm);
