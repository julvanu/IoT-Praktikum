#pragma once

void addr_to_str(const uint8_t *addr, char *out, size_t out_len);
int identify_device();
char* get_device_name_by_id(int device_id);