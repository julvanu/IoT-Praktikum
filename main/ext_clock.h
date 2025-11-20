#pragma once
#include "time.h"
#include <i2cdev.h>

time_t get_time_ext_clock_no_init(i2c_dev_t dev);
time_t get_time_ext_clock(void);
i2c_dev_t init_ext_clock(void);
