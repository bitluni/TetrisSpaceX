namespace music1
{
#include "music1.h"
}

#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

const int sampleRate = 22050;

bool playing = true;
int sample = 0;

void IRAM_ATTR timer_isr(void *para)
{
    int timer_idx = (int) para;
    uint32_t intr_status = TIMERG0.int_st_timers.val;
    if((intr_status & BIT(timer_idx)) && timer_idx == TIMER_0) 
    {
        TIMERG0.hw_timer[timer_idx].update = 1;
        TIMERG0.int_clr_timers.t0 = 1;
        TIMERG0.hw_timer[timer_idx].config.alarm_en = 1;

        if(playing)
        {
          WRITE_PERI_REG(I2S_CONF_SIGLE_DATA_REG(0), (128 + music1::samples[sample]) << 24);
          sample++;
          if(sample >= music1::sampleCount)
            sample = 0;        
        }
    }
}

void initSound()
{
  timer_group_t timer_group = (timer_group_t)TIMER_GROUP_0;
  timer_idx_t timer_idx = (timer_idx_t)TIMER_0;
  timer_config_t config;
  config.alarm_en = 1;
  config.auto_reload = 1;
  config.counter_dir = TIMER_COUNT_UP;
  config.divider = 16;
  config.intr_type = TIMER_INTR_LEVEL;
  config.counter_en = TIMER_PAUSE;
  timer_init(timer_group, timer_idx, &config);
  timer_pause(timer_group, timer_idx);
  timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
  timer_set_alarm_value(timer_group, timer_idx, 1/22050.0 * TIMER_BASE_CLK / config.divider);
  timer_enable_intr(timer_group, timer_idx);
  timer_isr_register(timer_group, timer_idx, timer_isr, (void*) timer_idx, ESP_INTR_FLAG_IRAM, NULL);
  timer_start(timer_group, timer_idx);
}

