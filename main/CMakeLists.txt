idf_component_register(
    SRCS main.c mqtt.c wifi.c
        hardware/ble_tag.c hardware/device_handling.c hardware/gauge.c hardware/identify_device.c
        init/configPM.c init/flash_init.c init/initialize.c init/sensor_setup.c init/watchdog.c
        time/ext_clock.c time/periodic_wakeup_timer.c time/sleep_minutes.c time/sntp.c
    EMBED_TXTFILES 
    INCLUDE_DIRS "." "hardware" "init" "time"
    REQUIRES esp_event esp_wifi mqtt nvs_flash driver ds3231 lc709203f bt
)
