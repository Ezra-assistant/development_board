#ifndef _SPIFLASH1_H
#define _SPIFLASH1_H

typedef struct{
    u32 flash_id;
    u32 flash_capacity;
    u8  mode_4byte_en;
}spiflash1_cb_t;

void spiflash1_init(void);
void spiflash1_read(void *buf, u32 addr, u32 len);
void spiflash1_write(const void *buf, u32 addr, u32 len);
u32 spiflash1_id_read(void);
void spiflash1_erase(u32 addr);
void spiflash1_erase_block(u32 addr);
bool is_exspiflash_online(void);
void spiflash1_power_down(void);
void spiflash1_release_pd(void);
void spiflash1_enter_4byte_mode(void);
void spiflash1_exit_4byte_mode(void);

bool exspiflash_init(void);
void write_music_bin_to_spiflash(void);
extern void register_spi_read_function(void (* read_func)(void *buf, u32 addr, u32 len));
extern void register_spi_wav_read_function(void (* read_func)(void *buf, u32 addr, u32 len));

#endif // _SPIFLASH_H
