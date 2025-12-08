idf_component_register(SRCS main.c wifi.c sntp.c mqtt.c gauge.c controlflow.c identify_device.c flash_init.c 
                    initialize.c sensor_setup.c device_handling.c configPM.c ext_clock.c ble_tag.c periodic_wakeup_timer.c
					EMBED_TXTFILES 
                    INCLUDE_DIRS "."
                    REQUIRES esp_event esp_wifi mqtt nvs_flash driver ds3231 lc709203f bt) 