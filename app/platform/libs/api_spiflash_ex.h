#ifndef _API_SPIFLASH_EX_H_
#define _API_SPIFLASH_EX_H_

//及镜像文件播放相关函数
u32 exspiflash_music_get_file_total(void);      //获取SPI1 flash文件总数
void exspiflash_music_num_kick_do(u32 num);        //播放SPI1 flash当前num号的音频
u32 exspiflash_music_get_cur_file_len(void);    //获取SPI1 flash当前num号音频长度
u8 exspiflash_msc_version(void);                //获取SPI1 flash打包版本标志

//外接SPIFLASH录音相关函数
void cycrec_fs_init(u32 begin_addr, u32 end_addr, void(*read)(void *buf, u32 addr, u32 len),void(*write)(void *buf, u32 addr, u32 len),void (*erase)(u32 addr) );
u8 spiflash_rec_open_last_file(u32 *addr, u32 *len);
u8 spiflash_rec_open_prev_file(u32 *addr, u32 *len);
u8 spiflash_rec_open_next_file(u32 *addr, u32 *len);
void spiflash_rec_delete_curplay_file(void);
u32 spiflash_rec_get_total_file(void);
u8 spiflash_rec_creat_file(void);
u8 spiflash_rec_write_file(void *buf, u32 len);
u8 spiflash_rec_close_file(void);

#endif
