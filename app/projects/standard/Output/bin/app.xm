#include "config.h"
setid(11111111-1111-1111-1111-111111111111);
setflash(1, FLASH_SIZE, FLASH_ERASE_4K, FLASH_DUAL_READ, FLASH_QUAD_READ);
setspace(0x4000);
#if UFLASH_BIN_EN
    setuserbin(UFLASH_BIN_ADDR, UFLASH_BIN_LEN, user.bin);
#endif
#if (MIDI_EN)
    setuserbin(VOICE_BIN_ADDR, VOICE_BIN_LEN, voice.bin);
#endif
make(dcf_buf, header.bin, app.bin, res.bin, xcfg.bin, updater.bin);
save(dcf_buf, app.dcf);
