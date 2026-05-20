#ifndef FUNC_USER_MUSIC_H
#define FUNC_USER_MUSIC_H

#include "spiflash/spiflash1.h"

#define USER_READ_ADDR  (UFLASH_BIN_ADDR + 0x20000000)
#define USER_MUSIC_HEAD_ADDR  (USER_READ_ADDR + 0x10)
#define USER_MUSIC_ADDR  (USER_READ_ADDR + 0x20)

enum{
    USE_WAV  = BIT(0),
    USE_MP3  = BIT(1),
    USE_VSBC = BIT(2),
    USE_MID  = BIT(3),
    USE_VMP3 = BIT(4),
    USE_ESBC = BIT(5),
    USE_LSBC = BIT(6),
};

typedef struct{
    u8  music_type;

    u32 wav_chunk_addr;     //wav块起始地址
    u8  wav_file_num;        //wav文件数

    u32 mp3_chunk_addr;
    u8  mp3_file_num;

    u32 vmp3_chunk_addr;
    u8  vmp3_file_num;

    u32 esbc_chunk_addr;
    u8  esbc_file_num;

    u32 lsbc_chunk_addr;
    u8  lsbc_file_num;
}user_msc_t;
extern user_msc_t user_msc AT(.user_msc);

bool user_music_read_head(void);
bool get_music_info(u32* addr, u32* len, u8 type, u8 num);


#endif