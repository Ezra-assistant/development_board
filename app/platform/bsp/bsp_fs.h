#ifndef _BSP_FS_H
#define _BSP_FS_H

#if MUSIC_FILE_SORT_CHAR_EN
typedef struct{
    u8 dir_cur_num;
    u16 file_01_offset;//01文件夹的目录项编号
    u16 file_02_offset;
    u16 file_03_offset;

    u8 dir_01_save_num;
    u8 dir_02_save_num;
}file_sort_var_t;

typedef struct{
    u16 file_count;
    char fname[5];
}file_sort_t;
#endif

u8 music_file_filter(void);
u8 music_dir_filter(void);
u8 music_num_file_filter(void);
u8 music_only_record_file_filter(void);                     //只扫描录音文件
u8 music_only_record_dir_filter(void);                      //只扫描录音文件
u8 music_rm_record_dir_filter(void);                        //不扫描录音文件
u8 music_only_name_file_filter(void);                       //只扫描特定文件
void bsp_update_final_rec_file_num(void);

int stream_read(void *buf, unsigned int size);
bool stream_seek(unsigned int ofs, int whence);
void str_tolower(char *str, uint len);

void music_mp3_loopback_size(u32 start_byte_offset, u32 end_byte_offset);

#endif //_BSP_FS_H


