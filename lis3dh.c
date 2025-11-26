#include "lis3dh.h"

#include "nrf_drv_spi.h"
#include "app_error.h"
#include "nrf_delay.h"

#include "nrf_log.h"

// --- SPI instance and pins (use the same as your working test) ---
#define LIS3DH_SPI_INSTANCE   0
static const nrf_drv_spi_t m_spi = NRF_DRV_SPI_INSTANCE(LIS3DH_SPI_INSTANCE);

// Your pin mapping:
#define LIS3DH_MISO_PIN   14   // P0.14
#define LIS3DH_MOSI_PIN   15   // P0.15
#define LIS3DH_SCK_PIN    13   // P0.13
#define LIS3DH_CS_PIN     17   // P0.17

// LIS3DH registers
#define LIS3DH_REG_WHO_AM_I   0x0F
#define LIS3DH_REG_CTRL1      0x20
#define LIS3DH_REG_CTRL4      0x23
#define LIS3DH_REG_OUT_X_L    0x28

#define LIS3DH_WHO_AM_I_VAL   0x33

static void lis3dh_write_reg(uint8_t reg, uint8_t val)
{
    uint8_t tx_buf[2] = { reg & 0x7F, val }; // MSB=0 for write
    uint8_t rx_dummy[2];

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&m_spi, tx_buf, sizeof(tx_buf),
                                         rx_dummy, sizeof(rx_dummy)));
}

static void lis3dh_read_regs(uint8_t start_reg, uint8_t *p_data, uint8_t len)
{
    // MSB=1 for read, if multibyte also set bit 6 (auto-increment)
    uint8_t tx_buf[1] = { (uint8_t)(start_reg | 0x80 | 0x40) };
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&m_spi, tx_buf, 1, p_data, len));
}

void lis3dh_spi_init(void)
{
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = LIS3DH_CS_PIN;
    spi_config.miso_pin = LIS3DH_MISO_PIN;
    spi_config.mosi_pin = LIS3DH_MOSI_PIN;
    spi_config.sck_pin  = LIS3DH_SCK_PIN;
    spi_config.frequency = NRF_DRV_SPI_FREQ_4M;
    spi_config.mode      = NRF_DRV_SPI_MODE_3;
    spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;

    APP_ERROR_CHECK(nrf_drv_spi_init(&m_spi, &spi_config, NULL, NULL));
}

void lis3dh_init(void)
{
    uint8_t whoami = 0;
    lis3dh_read_regs(LIS3DH_REG_WHO_AM_I, &whoami, 1);
    NRF_LOG_INFO("LIS3DH WHO_AM_I = 0x%02x", whoami);

    // 100 Hz, all axes enabled
    lis3dh_write_reg(LIS3DH_REG_CTRL1, 0x57);

    // ±2 g, high-res
    lis3dh_write_reg(LIS3DH_REG_CTRL4, 0x08);

    nrf_delay_ms(10);
}


/* Read 3-axis high-res data, return right-shifted 12-bit values */
void lis3dh_read_raw(int16_t *x, int16_t *y, int16_t *z)
{
    // Command byte: RW=1, auto-increment=1, starting at OUT_X_L
    uint8_t tx[7];
    uint8_t rx[7];

    tx[0] = (uint8_t)(0x80 | 0x40 | (LIS3DH_REG_OUT_X_L & 0x3F));  // read + auto-inc
    for (int i = 1; i < 7; i++) {
        tx[i] = 0x00;   // dummy bytes
    }

    // *** THIS WAS MISSING ***
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&m_spi,
                                         tx, sizeof(tx),
                                         rx, sizeof(rx)));
    // ************************

    // rx[0] = dummy/status, rx[1..6] = X_L, X_H, Y_L, Y_H, Z_L, Z_H
    int16_t raw_x = (int16_t)((rx[2] << 8) | rx[1]);
    int16_t raw_y = (int16_t)((rx[4] << 8) | rx[3]);
    int16_t raw_z = (int16_t)((rx[6] << 8) | rx[5]);

    // High-res mode: 12-bit left-justified -> shift down 4
    *x = raw_x >> 4;
    *y = raw_y >> 4;
    *z = raw_z >> 4;
}
