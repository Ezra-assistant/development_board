#include "include.h"

#if EX_FLASH_FS_EN

// variables used by the filesystem
lfs_t lfs_nor AT(.lfs_buf.nor);
lfs_file_t lfs_file_nor AT(.lfs_buf.nor);

u8 read_buffer[256] AT(.lfs_buf.nor);
u8 prog_buffer[256] AT(.lfs_buf.nor);
u8 lookahead_buffer[16] AT(.lfs_buf.nor);
u8 lfs_file_buffer[512] AT(.lfs_buf.mem);

// configuration of the filesystem is provided by this struct
const struct lfs_config lfs_cfg_nor = {
    .context = NULL,

    // block device operations
    .read  = lfs_block_flash_read,
    .prog  = lfs_block_flash_prog,
    .erase = lfs_block_flash_erase,
    .sync  = lfs_block_flash_sync,

    // block device configuration
    .read_size = 16,                        //flash page read size
    .prog_size = EX_FLASH_LFS_W_SIZE,       //flash page write size
    .block_size = EX_FLASH_LFS_E_SIZE,      //flash page erase size
    .block_count = EX_FLASH_LFS_BLK_CNT,    //flash max size(block_count*block_size)
    .block_cycles = 500,                    //100~1000
    .cache_size = 256,                      //(block_size/4 or block_size/8)
    .lookahead_size = 16,                   //(block_count/8)
    .compact_thresh = 50,                   //0~100 -> 0%~100%

    .read_buffer = read_buffer,
    .prog_buffer = prog_buffer,
    .lookahead_buffer = lookahead_buffer,

    .name_max = LFS_NAME_MAX,
    .file_max = LFS_FILE_MAX,
    .attr_max = LFS_ATTR_MAX,
    .metadata_max = 1024,           //(block_size/n)
    .inline_max = 256,              //(prog_size*n)
};

const struct lfs_file_config file_cfg_nor = {
    .buffer = lfs_file_buffer,
    .attrs = NULL,
    .attr_count = 0,
};

//lfs挂载
int exflash_lfs_mount(void)
{
    memset(&lfs_nor, 0, sizeof(lfs_t));
    memset(&lfs_file_nor, 0, sizeof(lfs_file_t));
    memset(&read_buffer, 0, sizeof(read_buffer));
    memset(&prog_buffer, 0, sizeof(prog_buffer));
    memset(&lookahead_buffer, 0, sizeof(lookahead_buffer));
    int err = lfs_mount(&lfs_nor, &lfs_cfg_nor);
    if(err){
        //fisrt boot
        lfs_format(&lfs_nor, &lfs_cfg_nor);
        err = lfs_mount(&lfs_nor, &lfs_cfg_nor);
        if(err){
            printf("lfs mount fail\n");
        }
    }
    return err;
}

//lfs读写数据测试
void exflash_lfs_rw_test(void)
{
    int err = lfs_file_opencfg(&lfs_nor, &lfs_file_nor, "data.txt", LFS_O_RDWR | LFS_O_CREAT, &file_cfg_nor);
    if(err){
        my_printf("open err: %d\n", err);
    }

    err = lfs_file_write(&lfs_nor, &lfs_file_nor, "hello", 5);
    my_printf("w err: %d\n", err);

    lfs_file_sync(&lfs_nor, &lfs_file_nor);

    u8 read_buf[5];
    memset(read_buf, 0,sizeof(read_buf));
    lfs_file_rewind(&lfs_nor, &lfs_file_nor);
    err = lfs_file_read(&lfs_nor, &lfs_file_nor, read_buf, 5);
    my_printf("r err: %d\n", err);
    my_print_r(read_buf, 5);

    lfs_file_close(&lfs_nor, &lfs_file_nor);
    lfs_unmount(&lfs_nor);
}

//lfs写文件测试
void exflash_lfs_file_test(void)
{
    int err = lfs_file_opencfg(&lfs_nor, &lfs_file_nor, "gq01a.lsbc", LFS_O_RDWR | LFS_O_CREAT, &file_cfg_nor);
    my_printf("open err: %d\n", err);

#if 0   //需自行修改RES_BUF_GQ01B_MP3和RES_LEN_GQ01B_MP3路径, 再打开进行读写测试
    err = lfs_file_write(&lfs_nor, &lfs_file_nor, (const void *)RES_BUF_GQ01A_LSBC, RES_LEN_GQ01A_LSBC);
    my_printf("w err: %d\n", err);
#endif

    lfs_file_sync(&lfs_nor, &lfs_file_nor);
    lfs_file_close(&lfs_nor, &lfs_file_nor);

    err = lfs_file_opencfg(&lfs_nor, &lfs_file_nor, "gq01a.lsbc", LFS_O_RDONLY , &file_cfg_nor);
    my_printf("open err: %d\n", err);

    u8 read_buf[32];
    memset(read_buf, 0,sizeof(read_buf));
    err = lfs_file_read(&lfs_nor, &lfs_file_nor, read_buf, 32);
    my_printf("r err: %d\n", err);
    my_print_r(read_buf, 32);

    lfs_file_close(&lfs_nor, &lfs_file_nor);
    // lfs_unmount(&lfs_nor);
}

// 自定义strrchr函数实现
char *strrchr(const char *s, int c) {
    char *last = NULL;

    // 如果字符串为空，返回NULL
    // if (s == NULL) {
    //     return NULL;
    // }

    // 遍历字符串，记录最后一次匹配的位置
    while (*s != '\0') {
        if (*s == (char)c) {
            last = (char *)s;
        }
        s++;
    }

    // 如果查找的是空字符('\0')，返回字符串末尾
    if (c == '\0') {
        return (char *)s;
    }

    return last;
}

// 添加文件列表功能
void exflash_lfs_list_files(void)
{
    lfs_dir_t dir;
    struct lfs_info info;

    int err = lfs_dir_open(&lfs_nor, &dir, "/");
    if (err) {
        printf("Failed to open directory: %d\n", err);
        return;
    }

    printf("Files in LFS:\n");
    while (lfs_dir_read(&lfs_nor, &dir, &info) > 0) {
        if (info.type == LFS_TYPE_REG) {  // 只显示普通文件
            printf("  %s (size: %d)\n", info.name, info.size);
        }
    }

    lfs_dir_close(&lfs_nor, &dir);
}

// 检查LFS文件系统中是否有音乐文件
bool exflash_lfs_has_music_files(void)
{
    lfs_dir_t dir;
    struct lfs_info info;
    bool has_music = false;

    int err = lfs_dir_open(&lfs_nor, &dir, "/");
    if (err) {
        printf("Failed to open directory: %d\n", err);
        return false;
    }

    while (lfs_dir_read(&lfs_nor, &dir, &info) > 0) {
        if (info.type == LFS_TYPE_REG) {
            // 检查文件扩展名
            char *ext = strrchr(info.name, '.');
            if (ext && (strcmp(ext, ".mp3") == 0 || strcmp(ext, ".lsbc") == 0)) {
                has_music = true;
                break;
            }
        }
    }

    lfs_dir_close(&lfs_nor, &dir);
    return has_music;
}

// 获取第一个音乐文件
bool exflash_lfs_get_first_music_file(char *filename, int max_len)
{
    lfs_dir_t dir;
    struct lfs_info info;
    bool found = false;

    int err = lfs_dir_open(&lfs_nor, &dir, "/");
    if (err) {
        printf("Failed to open directory: %d\n", err);
        return false;
    }

    while (lfs_dir_read(&lfs_nor, &dir, &info) > 0) {
        if (info.type == LFS_TYPE_REG) {
            // 检查文件扩展名
            char *ext = strrchr(info.name, '.');
            if (ext && (strcmp(ext, ".mp3") == 0 || strcmp(ext, ".lsbc") == 0)) {
                strncpy(filename, info.name, max_len - 1);
                filename[max_len - 1] = '\0';
                found = true;
                break;
            }
        }
    }

    lfs_dir_close(&lfs_nor, &dir);
    return found;
}

// 修改播放函数，使其能够播放指定的音乐文件
void exflash_lfs_play_music_file(const char *filename)
{
    int err = lfs_file_opencfg(&lfs_nor, &lfs_file_nor, filename, LFS_O_RDONLY, &file_cfg_nor);
    // printf("Opening file '%s', err: %d\n", filename, err);

    if (err != 0) {
        printf("Failed to open file '%s': %d\n", filename, err);
        return;
    }

    lfs_soff_t size = lfs_file_size(&lfs_nor, &lfs_file_nor);
    printf("File size: 0x%X (%d bytes)\n", size, size);

    if (size > 0) {
        // 根据文件扩展名选择播放方式
        char *ext = strrchr(filename, '.');
        if (ext) {
            if (strcmp(ext, ".mp3") == 0) {
                mp3_song_play(0, size, 0);
            } else if (strcmp(ext, ".lsbc") == 0) {
                lsbc_song_play(0, size, 0);
            } else {
                printf("Unsupported file format: %s\n", ext);
            }
        }
    }

    // 注意：不要在这里关闭文件，因为播放过程中还需要读取
}

//lfs播放音频示例
void exflash_lfs_file_kick(void)
{
    my_printf("%s %d\n", __func__, __LINE__);

    // 首先列出所有文件
    // exflash_lfs_list_files();

    // 检查是否有音乐文件
    if (!exflash_lfs_has_music_files()) {
        printf("No music files found in LFS!\n");
        return;
    }

    // 获取第一个音乐文件并播放
    char filename[64];
    if (exflash_lfs_get_first_music_file(filename, sizeof(filename))) {
        // printf("Playing first music file: %s\n", filename);
        exflash_lfs_play_music_file(filename);
    }

#if 0   //mp3测试
    int err = lfs_file_opencfg(&lfs_nor, &lfs_file_nor, "003.mp3", LFS_O_RDONLY , &file_cfg_nor);
    my_printf("open err: %d\n", err);

    lfs_soff_t size = lfs_file_size(&lfs_nor, &lfs_file_nor);
    my_printf("size: 0x%X\n", size);
    if(size > 0){
        mp3_song_play(0, size, 0);
    }
#endif
#if 0   //lsbc测试
    int err = lfs_file_opencfg(&lfs_nor, &lfs_file_nor, "gq01a.lsbc", LFS_O_RDONLY , &file_cfg_nor);
    my_printf("open err: %d\n", err);

    lfs_soff_t size = lfs_file_size(&lfs_nor, &lfs_file_nor);
    my_printf("size: 0x%X\n", size);
    if(size > 0){
        lsbc_song_play(0, size, 0);
    }
#endif
}
#if SPIFLASH_MUSIC_SORT_AND_LOOP
sort_entry_t sort_list[MAX_SORT_FILES];
int sort_count = 0;
 static int strcasecmp_local(const char* s1, const char* s2)
 {
     while (*s1 && *s2) {
         char c1 = (*s1 >= 'A' && *s1 <= 'Z') ? *s1 + 32 : *s1;
         char c2 = (*s2 >= 'A' && *s2 <= 'Z') ? *s2 + 32 : *s2;
         if (c1 != c2)
             return c1 - c2;
         s1++;
         s2++;
     }
     return *s1 - *s2;
 }

static void insert_sorted(const char* filename, uint32_t index)
{
    if (sort_count >= MAX_SORT_FILES){
        return;
	}
    sort_entry_t new_entry;
    memset(&new_entry, 0, sizeof(new_entry));
    new_entry.file_index = index;

    // 中文首字母映射简化: 如果首字节>=0x80，就统一用 'H'
    if ((unsigned char)filename[0] >= 0x80) {
        new_entry.key[0] = 'H'; // 占位，后续可扩展为完整汉字首字母映射
        new_entry.key[1] = '\0';
    } else {
        strncpy(new_entry.key, filename, SORT_KEY_LEN);
        new_entry.key[SORT_KEY_LEN] = '\0'; // 防止无终止符
    }

    int i = sort_count;
    while (i > 0) {
        if (strcasecmp_local(new_entry.key, sort_list[i - 1].key) < 0) {
            sort_list[i] = sort_list[i - 1];
            i--;
        } else {
            break;
        }
    }
    sort_list[i] = new_entry;
    sort_count++;

    // // === 调试打印排序结果 ===
    // printf("Current sorted list (%d):\n", sort_count);
    // for (int j = 0; j < sort_count; j++) {
    //     printf("[%d] idx=%u key=%s\n",
    //         j,
    //         sort_list[j].file_index,
    //         sort_list[j].key);
    // }
}
//排序
void exflash_lfs_sort_music(void)
{
    lfs_dir_t dir;
    struct lfs_info info;
    uint32_t music_file_index = 0;

    if (lfs_dir_open(&lfs_nor, &dir, "/") != 0) {
        printf("open dir fail\n");
        return;
    }
    while (lfs_dir_read(&lfs_nor, &dir, &info) > 0) {
        if (info.type == LFS_TYPE_REG) {
            const char* ext = strrchr(info.name, '.');
            if (ext && (strcasecmp_local(ext, ".mp3") == 0 || strcasecmp_local(ext, ".wav") == 0)) {
                insert_sorted(info.name, music_file_index);
                music_file_index++;
            }
        }
    }
    exspi_msc.total_num = sort_count;
    lfs_dir_close(&lfs_nor, &dir);
}

// ==== 主流程 ====
void exflash_lfs_play_num_music(uchar playing_index)
{
    lfs_dir_t dir;
    struct lfs_info info;
    char filename_buf[64] = { 0 };
    char music_idx = 0;

    if (lfs_dir_open(&lfs_nor, &dir, "/") != 0) {
        printf("open dir fail\n");
        return;
    }
    while (lfs_dir_read(&lfs_nor, &dir, &info) > 0) {
        if (info.type == LFS_TYPE_REG) {
            const char* ext = strrchr(info.name, '.');
            if (ext && (!strcasecmp_local(ext, ".mp3") || !strcasecmp_local(ext, ".wav"))) {
                if (music_idx == sort_list[playing_index].file_index) {
                    strncpy(filename_buf, info.name, sizeof(filename_buf) - 1);
                    filename_buf[sizeof(filename_buf) - 1] = '\0';
                    lfs_dir_close(&lfs_nor, &dir);
                    break; // success
                    }
                music_idx++;
            }
        }
    }
    printf("Play: %s\n", filename_buf);
    exflash_lfs_play_music_file(filename_buf);
}
#endif


void exflash_lfs_read(void *buf, u32 addr, u32 len)
{
    lfs_file_read(&lfs_nor, &lfs_file_nor, buf, len);
}

bool exflash_lfs_music_chk(void)
{
    bool bo_ret = false;
    return bo_ret;
}

void exflash_lfs_music_play(void)
{
    set_sys_clk(SYS_120M);
    register_spi_read_function(exflash_lfs_read);
    // exflash_lfs_file_kick();
#if SPIFLASH_MUSIC_SORT_AND_LOOP
    exflash_lfs_sort_music();    //排序
#endif
    // exflash_lfs_play_num_music(0);  //播放排序后的第一首歌
}
#endif
