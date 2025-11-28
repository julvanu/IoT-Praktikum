#pragma once
#include "time.h"
#include <i2cdev.h>

i2c_dev_t init_ext_clock(void);
time_t get_time_ext_clock(void);
void set_time_ext_clock(struct tm *time);
float get_temp_ext_clock(void);
