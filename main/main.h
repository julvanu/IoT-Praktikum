#pragma once


// #define EXAMPLE_ESP_WIFI_SSID      "CAPS-Seminar-Room"
// #define EXAMPLE_ESP_WIFI_PASS      "caps-schulz-seminar-room-wifi"
// #define SNTP_SERVER_NAME           "ntp1.in.tum.de"

#define EXAMPLE_ESP_WIFI_SSID      "Raum der Wuensche"
#define EXAMPLE_ESP_WIFI_PASS      "2Affenimboot"
#define SNTP_SERVER_NAME           "pool.ntp.org"

#define MQTT_BROKER                "192.168.178.41" // your RPi IP address (WLAN: .39, LAN: .41)

// DEVICE: corridor
// #define DEVICE_ID                  "1"
// #define DEVICE_TOPIC               "1/1/data"
// #define DEVICE_KEY                 "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE3NjI1NjExMDUsImlzcyI6ImlvdHBsYXRmb3JtIiwic3ViIjoiMS8xIn0.PKesAYBXCPU6j3AN1zyU5OssawPIPYuP6crShnkpIbtgiB-YSQym2bZJlU_dykUKTrW5REPY3RnNaKRzzmhDeZzof08mkZswisrDWVPuGdIFumCLS4fbKPk9ni5OHQ2uZZfRpdfvyRBa2Ut_9YPXMYDO39sExwrACYG9M6o0Hr-9juHVgo0hFpAg4zvGclMOzd7Vm1vrbumm72f5haWGhdvS-eWZyPi7tIBN_B9M2dzR1Els8lg4jz9jx6W6Dag6zLSPlQWhq7aK1ywaX7rGR3l7oBmfN6RpgkDu3s93-ESVkzvftpbd06uAzUd_7wfXjCeflG3NfqmMJR9-1gAXuha0BG9JWfS5TXO5yUEkpTzCPPQHkL8QKoobErIss1fPjaj1fS1ngeUzWk2vmhut8Kf9QL0zpgM2dOUmKN7koyVvKSYWdwn1tvtvYBfaEMi2OfA1rzKwbUli6PP7QHjyG9D0U6JggR1ER0TjcpWAhP3KFlDkcnbxLkIRgfa4qwv9NMTA8-w5SVpKcBQl6JmHh46udU0dnyc1SYntBTf6NipfszpAHM757Mj1egxbk6MGfePoT-fO98LdYwEdIMawL3siOTuox5eShWdlfPaCN2oB-1PIlo92xznFi5gjfHaSnGdZ-PCvQ9hl4BrbQAfll1Tz2voYKFsTq1lt4GzvOsA"

// DEVICE: bathroom
// #define DEVICE_ID                  "2"
// #define DEVICE_TOPIC               "1/2/data"
// #define DEVICE_KEY                 "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE3NjIxMTY5OTcsImlzcyI6ImlvdHBsYXRmb3JtIiwic3ViIjoiMS8yIn0.kKhJXe4CsNeeM4wi7DsfqKAlVewMAi6796eFZDf-vQ13vgwkVN7bXbQDov-WRxZTfq0ifligp53hoPEvei-h8hjgZ7G8FeaQVhaTpDZbLm_jwAXn2bBMj8Cti4B-Bh1VxKKqD2Tot-_4P0yEXoqztNAexZ9lYK4j2dcfXkG02mO9XHjf6s24eBuIONF8PQXn_SGh-ZNFlfAjm67FiXLl6LV6HOs1lPe8RcZhGNo00b1CtxxdJlV7vFLSNlaI6jm9xuDrN8IN89m1ZSKwistXHF8zfKmtKG6wYsDcTg3S9Q2SeYQuUYcj01u4EPlPmkDqXXT1LQv1retT8C31CSfiBfwlDygN16O82OWlVtNXHmoebaE2TbqgyLyLAFbL16JQKV0cObX3g1v-wASLalT3WDEriADPei_CK3GecAK_b_FSjW_QK6iUrC0uOjRnNA3blmKjrI_3nPubd3D0HW07gPst07PRaelUh57AUTbnRQkYxzgL9MpGrE0Bjfvm5aMyyh57co97IE4F36RxGpntIxkTwgZ6yLM6bVzwpi5CaCFyqgAWpI-I0S8tgL0M3I2VxP3cmqihei8h0FtogSlYsqs7yNl54t8ss6IAzPjgQzo3766pbqrTXrOERbo2XkPQ1DGO12HgVaJry-99lSawQLmWD3nPXEUd6443OD9oTow"

// DEVICE: kitchen
#define DEVICE_ID                  "4"
#define DEVICE_TOPIC               "1/4/data"
#define DEVICE_KEY                 "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE3NjI5NTcwMjEsImlzcyI6ImlvdHBsYXRmb3JtIiwic3ViIjoiMS80In0.oc3zFuR7uT0Nxyovr5EGdje3E9BeduQxzKMUsoAF1YSzPFAwZdvBA2A5H6hDdotuV1neBUPZJIUFHXrpqioGMCzwe3dEsxH3S-bdfDBfHi5-ebzsEJIa8a77QZ4m1Jbtwa1mz9BLIXgCN1q9HVG-DZQjSEmJXZMg3RdD74TX2cLtanBd3qwM6n3AjH6aea5sv6opFwdMp5ef955KAIYbe9vu3spCnIyrltPSbBILEqsx3nZtMexwr02TMvtda4MXxkXp7FszzXh6gF9BA-ZPQExVzv4PeO_ePi_FWiJk5jROW6gToeEVhlJh86sQl8PfoBpZmzBaDmSmAiWfYqDbf8qvnpNmiKIte6mPUeD6KoaI3Twsz0mxj814H6EBPLzzNCQHEsIdJyYBs28CBb8oPy9Wdchp3rOe9ivQ6fsqR86WtXXxQA-bAP5pgIpw-S_RZvpX6o7lKVSxWSsnKQm4MAepRsKFOtCKZHkZVXvmwZtnkjy_1gnSEW1WofzbC8gkvC_LG3FyPc1jwxbIDuBlN1SnkytpMra9HQ9xkGvu0qnaXgqWmQ9VonoHxuQ-LzMNbMXhRPcMxpsI48Sy4aV3YIETApdsWLSFyeNSz1Oiind7EdMy1ui9nVog3LMUCVoSmv9iNj_1n_497rPIoDL5z3d78eQEv3uky6BqoUGE7Jo"


#define PIR_PIN 27 
#define DOOR_PIN 32
#define PIN_MASK ((1ULL<<PIR_PIN)|(1ULL<<DOOR_PIN))

#define MAC_CORRIDOR "64:b7:08:6e:ae:bc"
#define MAC_BATHROOM "94:b9:7e:54:d3:00"
#define MAC_KITCHEN  "7c:9e:bd:d3:1b:38"
