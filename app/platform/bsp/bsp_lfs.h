#ifndef _BSP_LFS_H
#define _BSP_LFS_H

#if SPIFLASH_MUSIC_SORT_AND_LOOP
// 配置
#define MAX_SORT_FILES 10  //参与排序文件数
#define SORT_KEY_LEN 5

typedef struct {
    char key[SORT_KEY_LEN]; // 前5字节或首字母键
    uint32_t file_index; // 文件在目录的序号（第几个音乐文件）
} sort_entry_t;

extern int sort_count;
extern sort_entry_t sort_list[MAX_SORT_FILES];

#endif
int exflash_lfs_mount(void);
void exflash_lfs_music_play(void);
void exflash_lfs_read(void* buf, u32 addr, u32 len);
void exflash_lfs_play_num_music(uchar playing_index);
void exflash_lfs_list_files(void);
#endif
