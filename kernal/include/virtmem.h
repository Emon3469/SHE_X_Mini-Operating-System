#ifndef VIRTMEM_H
#define VIRTMEM_H

#include <stdint.h>
#include <stdbool.h>

bool vmmngr_init();
bool map_page(uint32_t vir, uint32_t phy);
void remove_identity_map();

#endif