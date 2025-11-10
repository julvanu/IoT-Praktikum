idf_component_register(SRCS main.c wifi.c sntp.c mqtt.c gauge.c controlflow.c identify_device.c flash_init.c 
                    initialize.c sensor_setup.c device_handling.c
					EMBED_TXTFILES 
                    INCLUDE_DIRS "."
                    REQUIRES esp_event esp_wifi mqtt nvs_flash driver lc709203f) 