#include "include.h"

AT(.rodata.func.table)
const u8 func_sort_table[] = {
#if EX_SPIFLASH_SUPPORT
    FUNC_EXSPIFLASH_MUSIC,
#endif

#if FUNC_CLOCK_EN
    FUNC_CLOCK,
#endif

#if FUNC_IDLE_EN
    FUNC_IDLE,
#endif // FUNC_IDLE_EN

#if FUNC_MUSIC_EN
    FUNC_MUSIC,
#endif // FUNC_MUSIC_EN
};

AT(.text.func)
u8 get_funcs_total(void)
{
    return sizeof(func_sort_table);
}

#if VDDIO_FOLLOW_VBAT_EN
AT(.com_text.vddio)
#endif
u32 getcfg_vddio_sel(void)
{
    return xcfg_cb.vddio_sel;
}

///获取当前vddio电压，单位mV，与实际输出vddio偏差0.01v左右
u32 get_vddio_voltage(void)
{
    return (1500 + xcfg_cb.vddio_sel * 75);
}

#if VDDIO_FOLLOW_VBAT_EN
AT(.com_text.vddio)
#endif
u32 getcfg_reset_sel(void)
{
    return xcfg_cb.reset_sel;
}

u16 get_wav_fix_spr(void)
{
#if WAV_FIX_SPR_EN
    return WAV_FIX_SPR_VAL;
#endif
    return 0;
}
