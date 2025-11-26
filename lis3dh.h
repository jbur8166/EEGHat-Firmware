#ifndef LIS3DH_H
#define LIS3DH_H

#include <stdint.h>

void lis3dh_spi_init(void);
void lis3dh_init(void);
void lis3dh_read_raw(int16_t *x, int16_t *y, int16_t *z);

#endif // LIS3DH_H