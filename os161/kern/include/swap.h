#ifndef _SWAP_H_
#define _SWAP_H_

#include <types.h>

#define SWAP_DEVICE "lhd0raw:" 

extern struct disk_entry * swap_map;

void swap_bootstrap(void);
int
swap_pagein( u_int32_t paddr, int rw, u_int32_t mem, u_int32_t file, u_int32_t offset, int swap, int swap_index);

int swap_out(u_int32_t paddr, int swap_index);
#endif
