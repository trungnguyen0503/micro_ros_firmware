#ifndef ICM20948_H
#define ICM20948_H

#include "i2c.h"
#include "stm32f4xx_hal.h"

#define ICM20948_ADDR_L ((uint16_t)0xD0) // SDO(AD0) pin is low
#define ICM20948_ADDR_H ((uint16_t)0xD2) // SDO(AD0) pin is high

typedef struct ICM20948_reg {
    const char *name;
    const uint16_t address;
} ICM20948_reg;

/* ICM20948 USER UBANK SELECT REGISTER START */
/* BIT 	| NAME 		| DESC
 * 7:6 	| -			| reserved
 * 5:4	| USER_BANK	| value of user bank
 * 3:0	| -			| reserved
 * Reset Value: 0b01000000
 */
#define ICM20948_REG_UBANK_SEL ((ICM20948_reg){ "UBANK_SEL", 0x7F }) // A RW register but cannot be used in ICM20948_Reg_RW or ICM20948_reg_R functions
#define ICM20948_REG_UBANK_SEL_RESERVED_MASK ((uint8_t)0b11001111)
typedef enum ICM20948_user_bank {
    ICM20948_UBANK_0 = 0x0,
    ICM20948_UBANK_1 = 0x10,
    ICM20948_UBANK_2 = 0x20,
    ICM20948_UBANK_3 = 0x30
} ICM20948_user_bank;

typedef struct ICM20948_reg_R {
    const char *name;
    const uint16_t address;
    const ICM20948_user_bank bank;
} ICM20948_reg_R;

typedef struct ICM20948_Reg_RW {
    const char *name;
    const uint16_t address;
    const ICM20948_user_bank bank;
    const uint8_t reserved_mask;
} ICM20948_Reg_RW;

#define ICM20948_REG_WHO_AM_I ((ICM20948_reg_R){ "WHO_AM_I", 0x00, ICM20948_UBANK_0 })
#define ICM20948_REG_WHO_AM_I_VALUE ((uint8_t)0xEA) // Read Only Value

/* BIT 	| NAME 			| DESC
 * 7 	| DMP_EN		| 1 enables DMP features
 * 6	| FIFO_EN		| 1 enables FIFO operation mode
 * 5	| I2C_MST_EN	| 1 enables I2C master I/F module. isolates ES_DA/ES_SCL from SDA/SCL
 * 4	| I2C_IF_DIS	| 1 reset I2C and enter SPI mode only
 * 3	| DMP_RST		| 1 asynchronous reset of DMP module, bit clears after one clock cycle
 * 2	| SRAM_RST		| 1 asynchronous reset of SRAM module, bit clears after one clock cycle
 * 1	| I2C_MST_RST	| 1 asynchronous reset of I2C module, bit clears after one clock cycle
 * 0	| -				| reserved
 * Reset Value: 0
 */
#define ICM20948_REG_USER_CTRL ((ICM20948_Reg_RW){ "USER_CTRL", 0x03, ICM20948_UBANK_0, 0 })
typedef enum ICM20948_user_ctrl_opts {
    ICM20948_I2C_MST_RST = 0b00000010,
    ICM20948_SRAM_RST = 0b00000100,
    ICM20948_DMP_RST = 0b00001000,
    ICM20948_I2C_IF_DIS = 0b00010000,
    ICM20948_I2C_MST_EN = 0b00100000,
    ICM20948_FIFO_EN = 0b01000000,
    ICM20948_DMP_EN = 0b10000000
} ICM20948_user_ctrl_opts;

/* BIT 	| NAME 			| DESC
 * 7 	| -				| reserved
 * 6	| I2C_MST_CYCLE	| (See p68 for I2C_MST_ODR_CONFIG)
 * 5	| ACCEL_CYCLE	| 1 sets duty cycled mode
 * 4	| GYRO_CYCLE	| 1 sets duty cycled mode
 * 3:0	| -				| reserved
 * Reset Value: 0b01000000
 */
#define ICM20948_REG_LP_CONFIG ((ICM20948_Reg_RW){ "LP_CONFIG", 0x05, ICM20948_UBANK_0, 0b10001111 })
typedef enum ICM20948_lp_config_opts {
    ICM20948_GYRO_CYCLE = 0b00010000,
    ICM20948_ACCEL_CYCLE = 0b00100000,
    ICM20948_I2C_MST_CYCLE = 0b01000000
} ICM20948_lp_config_opts;

/* BIT 	| NAME 			| DESC
 * 7 	| DEVICE_RESET	| 1 resets all registers
 * 6	| Sleep			| 1 sets sleep mode
 * 5	| LP_EN			| 1 sets low power mode
 * 4	| -				| reserved
 * 3	| TEMP_DIS		| 1 disables temp sensor
 * 2:0	| CLKSEL		| 1-5 auto clock (DS p37)
 * Reset Value: 0b01000001
 */
#define ICM20948_REG_PWR_MGMT_1 ((ICM20948_Reg_RW){ "PWR_MGMT_1", 0x06, ICM20948_UBANK_0, 0b00010000 })
typedef enum ICM20948_pwr_mgmt_1_opts {
    ICM20948_CLKSEL_AUTO = 0b00000001,
    ICM20948_CLKSEL_INTERNAL = 0b00000110,
    ICM20948_CLKSEL_STOP = 0b00000111,
    ICM20948_TEMP_DIS = 0b00001000,
    ICM20948_LP_EN = 0b00100000,
    ICM20948_SLEEP = 0b01000000,
    ICM20948_DEVICE_RESET = 0b10000000
} ICM20948_pwr_mgmt_1_opts;

/* BIT 	| NAME 				| DESC
 * 7 	| INT1_ACTL			| the logic level for INT1 pin active is low (1) or high (0)
 * 6 	| INT1_OPEN			| INT1 pin is configured as open drain (1) or push-pull (0)
 * 5 	| INT1_LATCH__EN	| INT1 pin level held until interrupt status is cleared (1) or 50us have passed (0)
 * 4 	| INT_ANYRD_2CLEAR	| INT_STATUS is cleared if any read is performed (1) or only when INT_STATUS is read (0)
 * 3 	| ACTL_FSYNC		| logic level for FSYNC pin is active is low (1) or high (0)
 * 2	| FSYNC_INT_MODE_EN	| 1 enables the FSYNC pin to be used as an interrupt (DS p38)
 * 1	| BYPASS_EN			| 1 sets ES_CL and ES_DA pins to bypass mode when I2C master interface is disabled
 * 0	| -					| reserved
 * Reset Value: 0
 */
#define ICM20948_REG_INT_PIN_CFG ((ICM20948_Reg_RW){ "INT_PIN_CFG", 0x0F, ICM20948_UBANK_0, 0b00000001 })
typedef enum ICM20948_int_pin_cfg_opts {
    ICM20948_BYPASS_EN = 0b00000010,
    ICM20948_FSYNC_INT_MODE_EN = 0b00000100,
    ICM20948_ACTL_FSYNC = 0b00001000,
    ICM20948_INT_ANYRD_2CLEAR = 0b00010000,
    ICM20948_INT1_LATCH__EN = 0b00100000,
    ICM20948_INT1_OPEN = 0b01000000,
    ICM20948_INT1_ACTL = 0b10000000
} ICM20948_int_pin_cfg_opts;

/* BIT 	| NAME 				| DESC
 * 7 	| ICM20948_REG_WOF_EN		| 1 enables wake on FSYNC interrupt
 * 6:4 	| -					| reserved
 * 3 	| WOM_INT_EN		| 1 enables interrupt for wake on motion to propagate to interrupt pin 1
 * 2	| PLL_RDY_EN		| 1 enables PLL RDY interrupt to propagate to interrupt pin 1 (DS p39)
 * 1	| DMP_INT1_EN		| 1 enables DMP interrupt to propagate to interrupt pin 1
 * 0	| I2C_MST_INT_EN	| 1 enables I2C master interrupt to propagate to interrupt pin
 * Reset Value: 0
 */
#define ICM20948_REG_INT_ENABLE ((ICM20948_Reg_RW){ "INT_ENABLE", 0x10, ICM20948_UBANK_0, 0b01110000 })
typedef enum ICM20948_int_enable_opts {
    ICM20948_I2C_MST_INT_EN = 0b00000001,
    ICM20948_DMP_INT1_EN = 0b00000010,
    ICM20948_PLL_RDY_EN = 0b00000100,
    ICM20948_WOM_INT_EN = 0b00001000,
    ICM20948_ICM20948_REG_WOF_EN = 0b10000000
} ICM20948_int_enable_opts;

// Clears register on read
#define ICM20948_REG_I2C_MST_STATUS ((ICM20948_reg_R){ "I2C_MST_STATUS", 0x17, ICM20948_UBANK_0 })

#define ICM20948_REG_DELAY_TIMEH ((ICM20948_reg_R){ "DELAY_TIMEH", 0x28, ICM20948_UBANK_0 })
#define ICM20948_REG_DELAY_TIMEL ((ICM20948_reg_R){ "DELAY_TIMEL", 0x29, ICM20948_UBANK_0 })

#define ICM20948_REG_ACCEL_XOUT_H ((ICM20948_reg_R){ "ACCEL_XOUT_H", 0x2D, ICM20948_UBANK_0 })
#define ICM20948_REG_ACCEL_XOUT_L ((ICM20948_reg_R){ "ACCEL_XOUT_L", 0x2E, ICM20948_UBANK_0 })
#define ICM20948_REG_ACCEL_YOUT_H ((ICM20948_reg_R){ "ACCEL_YOUT_H", 0x2F, ICM20948_UBANK_0 })
#define ICM20948_REG_ACCEL_YOUT_L ((ICM20948_reg_R){ "ACCEL_YOUT_L", 0x30, ICM20948_UBANK_0 })
#define ICM20948_REG_ACCEL_ZOUT_H ((ICM20948_reg_R){ "ACCEL_ZOUT_H", 0x31, ICM20948_UBANK_0 })
#define ICM20948_REG_ACCEL_ZOUT_L ((ICM20948_reg_R){ "ACCEL_ZOUT_L", 0x32, ICM20948_UBANK_0 })

#define ICM20948_REG_GYRO_XOUT_H ((ICM20948_reg_R){ "GYRO_XOUT_H", 0x33, ICM20948_UBANK_0 })
#define ICM20948_REG_GYRO_XOUT_L ((ICM20948_reg_R){ "GYRO_XOUT_L", 0x34, ICM20948_UBANK_0 })
#define ICM20948_REG_GYRO_YOUT_H ((ICM20948_reg_R){ "GYRO_YOUT_H", 0x35, ICM20948_UBANK_0 })
#define ICM20948_REG_GYRO_YOUT_L ((ICM20948_reg_R){ "GYRO_YOUT_L", 0x36, ICM20948_UBANK_0 })
#define ICM20948_REG_GYRO_ZOUT_H ((ICM20948_reg_R){ "GYRO_ZOUT_H", 0x37, ICM20948_UBANK_0 })
#define ICM20948_REG_GYRO_ZOUT_L ((ICM20948_reg_R){ "GYRO_ZOUT_L", 0x38, ICM20948_UBANK_0 })

#define ICM20948_REG_TEMP_OUT_H ((ICM20948_reg_R){ "TEMP_OUT_H", 0x33, ICM20948_UBANK_0 })
#define ICM20948_REG_TEMP_OUT_L ((ICM20948_reg_R){ "TEMP_OUT_L", 0x34, ICM20948_UBANK_0 })

#define ICM20948_REG_EXT_SLV_SENS_DATA_00 ((ICM20948_reg_R){ "EXT_SLV_SENS_DATA_00", 0x3B, ICM20948_UBANK_0 })
#define ICM20948_REG_EXT_SLV_SENS_DATA_08 ((ICM20948_reg_R){ "EXT_SLV_SENS_DATA_08", 0x43, ICM20948_UBANK_0 })

/* BIT 	| NAME 			| DESC
 * 7:4 	| -				| reserved
 * 3 	| SLV_3_FIFO_EN	| write associated EXT_SENS_DATA registers to FIFO at sample rate
 * 2	| SLV_2_FIFO_EN	| write associated EXT_SENS_DATA registers to FIFO at sample rate
 * 1 	| SLV_1_FIFO_EN	| write associated EXT_SENS_DATA registers to FIFO at sample rate
 * 0 	| SLV_0_FIFO_EN	| write associated EXT_SENS_DATA registers to FIFO at sample rate
 */
#define ICM20948_REG_FIFO_EN_1 ((ICM20948_Reg_RW){ "FIFO_EN_1", 0x66, ICM20948_UBANK_0, 0b11110000 })
typedef enum ICM20948_fifo_en_1_opts {
    ICM20948_SLV_0_FIFO_EN = 0b00000001,
    ICM20948_SLV_1_FIFO_EN = 0b00000010,
    ICM20948_SLV_2_FIFO_EN = 0b00000100,
    ICM20948_SLV_3_FIFO_EN = 0b00001000
} ICM20948_fifo_en_1_opts;

/* BIT 	| NAME 			| DESC
 * 7:0 	| XA_OFFS[14:7]	| Upper bits of X accel offset cancellation
 * Reset Value: Trimmed on a per-part basis for optimal performance
 */
#define ICM20948_REG_XA_OFFS_H ((ICM20948_Reg_RW){ "XA_OFFS_H", 0x14, ICM20948_UBANK_1, 0 })

/* BIT 	| NAME 			| DESC
 * 7:1 	| XA_OFFS[6:0]	| Lower bits of X accel offset cancellation
 * 0 	| -				| reserved
 * Reset Value: Trimmed on a per-part basis for optimal performance
 */
#define ICM20948_REG_XA_OFFS_L ((ICM20948_Reg_RW){ "XA_OFFS_L", 0x15, ICM20948_UBANK_1, 0b00000001 })

/* BIT 	| NAME 			| DESC
 * 7:0 	| YA_OFFS[14:7]	| Upper bits of Y accel offset cancellation
 * Reset Value: Trimmed on a per-part basis for optimal performance
 */
#define ICM20948_REG_YA_OFFS_H ((ICM20948_Reg_RW){ "YA_OFFS_H", 0x17, ICM20948_UBANK_1, 0 })

/* BIT 	| NAME 			| DESC
 * 7:1 	| YA_OFFS[6:0]	| Lower bits of Y accel offset cancellation
 * 0 	| -				| reserved
 * Reset Value: Trimmed on a per-part basis for optimal performance
 */
#define ICM20948_REG_YA_OFFS_L ((ICM20948_Reg_RW){ "YA_OFFS_L", 0x18, ICM20948_UBANK_1, 0b00000001 })

/* BIT 	| NAME 			| DESC
 * 7:0 	| ZA_OFFS[14:7]	| Upper bits of Z accel offset cancellation
 * Reset Value: Trimmed on a per-part basis for optimal performance
 */
#define ICM20948_REG_ZA_OFFS_H ((ICM20948_Reg_RW){ "ZA_OFFS_H", 0x1A, ICM20948_UBANK_1, 0 })

/* BIT 	| NAME 			| DESC
 * 7:1 	| ZA_OFFS[6:0]	| Lower bits of Z accel offset cancellation
 * 0 	| -				| reserved
 * Reset Value: Trimmed on a per-part basis for optimal performance
 */
#define ICM20948_REG_ZA_OFFS_L ((ICM20948_Reg_RW){ "ZA_OFFS_L", 0x1B, ICM20948_UBANK_1, 0b00000001 })

/* BIT 	| NAME 				| DESC
 * 7:0 	| GYRO_SMPLRT_DIV	| sample rate dividor 1.1kHz/(1 + GYRO_SMPLRT_DIV)
 * Reset Value: 0
 */
#define ICM20948_REG_GYRO_SMPLRT_DIV ((ICM20948_Reg_RW){ "GYRO_SMPLRT_DIV", 0x00, ICM20948_UBANK_2, 0 })

/* BIT 	| NAME 			| DESC
 * 7:6 	| -				| reserved
 * 5:3	| GYRO_DLPFCFG	| low pass filter config (DS p60)
 * 2:1	| GYRO_FS_SEL	| full scale select (DS p59)
 * 0	| GYRO_FCHOICE	| 1 enable DLPF
 * Reset Value: 0
 */
#define ICM20948_REG_GYRO_CONFIG_1 ((ICM20948_Reg_RW){ "GYRO_CONFIG_1", 0x01, ICM20948_UBANK_2, 0b11000000 })
typedef enum ICM20948_gyro_config_1_opts {
    ICM20948_GYRO_FCHOICE = 0b00000001,
    // GYRO_FS_SEL_250 by default
    ICM20948_GYRO_FS_SEL_500 = 0b00000010,
    ICM20948_GYRO_FS_SEL_1000 = 0b00000100,
    ICM20948_GYRO_FS_SEL_2000 = 0b00000110,
    // GYRO_DLPFCFG_0 by default
    ICM20948_GYRO_DLPFCFG_1 = 0b00001000,
    ICM20948_GYRO_DLPFCFG_2 = 0b00010000,
    ICM20948_GYRO_DLPFCFG_3 = 0b00011000,
    ICM20948_GYRO_DLPFCFG_4 = 0b00100000,
    ICM20948_GYRO_DLPFCFG_5 = 0b00101000,
    ICM20948_GYRO_DLPFCFG_6 = 0b00110000,
    ICM20948_GYRO_DLPFCFG_7 = 0b00111000
} ICM20948_gyro_config_1_opts;

/* BIT 	| NAME 			| DESC
 * 7:6 	| -				| reserved
 * 5	| XGYRO_CTEN	| 1 enable x self test
 * 4	| YGYRO_CTEN	| 1 enable y self test
 * 3	| ZGYRO_CTEN	| 1 enable z self test
 * 2:0	| GYRO_AVGCFG	| average lp-mode filter config (DS p60)
 * Reset Value: 0
 */
#define ICM20948_REG_GYRO_CONFIG_2 ((ICM20948_Reg_RW){ "GYRO_CONFIG_2", 0x02, ICM20948_UBANK_2, 0b11000000 })
typedef enum ICM20948_gyro_config_2_opts {
    // GYRO_AVGCFG_1x by default
    ICM20948_GYRO_AVGCFG_2x = 0b00000001,
    ICM20948_GYRO_AVGCFG_4x = 0b00000010,
    ICM20948_GYRO_AVGCFG_8x = 0b00000011,
    ICM20948_GYRO_AVGCFG_16x = 0b00000100,
    ICM20948_GYRO_AVGCFG_32x = 0b00000101,
    ICM20948_GYRO_AVGCFG_64x = 0b00000110,
    ICM20948_GYRO_AVGCFG_128x = 0b00000111,
    ICM20948_ZGYRO_CTEN = 0b00001000,
    ICM20948_YGYRO_CTEN = 0b00010000,
    ICM20948_XGYRO_CTEN = 0b00100000
} ICM20948_gyro_config_2_opts;

/* BIT 	| NAME 					| DESC
 * 7:0 	| XG_OFFS_USER[15:8]	| Upper bits of X gyro offset cancellation
 * Reset Value: 0
 */
#define ICM20948_REG_XG_OFFS_USRH ((ICM20948_Reg_RW){ "XG_OFFS_USRH", 0x03, ICM20948_UBANK_2, 0 })

/* BIT 	| NAME 				| DESC
 * 7:0 	| XG_OFFS_USER[7:0]	| Lower bits of X gyro offset cancellation
 * Reset Value: 0
 */
#define ICM20948_REG_XG_OFFS_USRL ((ICM20948_Reg_RW){ "XG_OFFS_USRL", 0x04, ICM20948_UBANK_2, 0 })

/* BIT 	| NAME 					| DESC
 * 7:0 	| YG_OFFS_USER[15:8]	| Upper bits of Y gyro offset cancellation
 * Reset Value: 0
 */
#define ICM20948_REG_YG_OFFS_USRH ((ICM20948_Reg_RW){ "YG_OFFS_USRH", 0x05, ICM20948_UBANK_2, 0 })

/* BIT 	| NAME 				| DESC
 * 7:0 	| YG_OFFS_USER[7:0]	| Lower bits of Y gyro offset cancellation
 * Reset Value: 0
 */
#define ICM20948_REG_YG_OFFS_USRL ((ICM20948_Reg_RW){ "YG_OFFS_USRL", 0x06, ICM20948_UBANK_2, 0 })

/* BIT 	| NAME 					| DESC
 * 7:0 	| ZG_OFFS_USER[15:8]	| Upper bits of Z gyro offset cancellation
 * Reset Value: 0
 */
#define ICM20948_REG_ZG_OFFS_USRH ((ICM20948_Reg_RW){ "ZG_OFFS_USRH", 0x07, ICM20948_UBANK_2, 0 })

/* BIT 	| NAME 				| DESC
 * 7:0 	| ZG_OFFS_USER[7:0]	| Lower bits of Z gyro offset cancellation
 * Reset Value: 0
 */
#define ICM20948_REG_ZG_OFFS_USRL ((ICM20948_Reg_RW){ "ZG_OFFS_USRL", 0x08, ICM20948_UBANK_2, 0 })

/* BIT 	| NAME 			| DESC
 * 7:1 	| -				| reserved
 * 0	| ODR_ALIGN_EN	| sensor start time alignment if and smprt set
 * Reset Value: 0
 */
#define ICM20948_REG_ODR_ALIGN_EN ((ICM20948_Reg_RW){ "ODR_ALIGN_EN", 0x09, ICM20948_UBANK_2, 0b11111110 })
typedef enum ICM20948_odr_align_opts {
    ICM20948_ODR_ALIGN_EN = 0b00000001
} ICM20948_odr_align_opts;

/* BIT 	| NAME 				| DESC
 * 7:4 	| -					| reserved
 * 3:0	| ACCEL_SMPLRT_DIV	| MSB for ACCEL_SMPLRT_DIV
 * Reset Value: 0
 */
#define ICM20948_REG_ACCEL_SMPLRT_DIV_1 ((ICM20948_Reg_RW){ "ACCEL_SMPLRT_DIV_1", 0x10, ICM20948_UBANK_2, 0b11110000 })

/* BIT 	| NAME 				| DESC
 * 7:0 	| ACCEL_SMPLRT_DIV	| LSB for sample rate dividor 1.125kHz/(1 + ACCEL_SMPLRT_DIV[11:0])
 * Reset Value: 0
 */
#define ICM20948_REG_ACCEL_SMPLRT_DIV_2 ((ICM20948_Reg_RW){ "ACCEL_SMPLRT_DIV_2", 0x11, ICM20948_UBANK_2, 0 })

/* BIT 	| NAME 			| DESC
 * 7:6 	| -				| reserved
 * 5:3	| ACCEL_DLPFCFG	| low pass filter config (DS p64)
 * 2:1	| ACCEL_FS_SEL	| full scale select (DS p64)
 * 0	| ACCEL_FCHOICE	| 1 enable DLPF
 * Reset Value: 0b00000001
 */
#define ICM20948_REG_ACCEL_CONFIG_1 ((ICM20948_Reg_RW){ "ACCEL_CONFIG_1", 0x14, ICM20948_UBANK_2, 0b11000000 })
typedef enum ICM20948_accel_config_1_opts {
    ICM20948_ACCEL_FCHOICE = 0b00000001,
    // ACCEL_FS_SEL_2g by default
    ICM20948_ACCEL_FS_SEL_4g = 0b00000010,
    ICM20948_ACCEL_FS_SEL_8g = 0b00000100,
    ICM20948_ACCEL_FS_SEL_16g = 0b00000110,
    // ACCEL_DLPFCFG_0 by default
    ICM20948_ACCEL_DLPFCFG_1 = 0b00001000,
    ICM20948_ACCEL_DLPFCFG_2 = 0b00010000,
    ICM20948_ACCEL_DLPFCFG_3 = 0b00011000,
    ICM20948_ACCEL_DLPFCFG_4 = 0b00100000,
    ICM20948_ACCEL_DLPFCFG_5 = 0b00101000,
    ICM20948_ACCEL_DLPFCFG_6 = 0b00110000,
    ICM20948_ACCEL_DLPFCFG_7 = 0b00111000
} ICM20948_accel_config_1_opts;

/* BIT 	| NAME 			| DESC
 * 7:5 	| -				| reserved
 * 4	| AX_ST_EN_REG	| 1 enable x self test
 * 3	| AY_ST_EN_REG	| 1 enable y self test
 * 2	| AZ_ST_EN_REG	| 1 enable z self test
 * 1:0	| DEC3_CFG		| controls samples averaged in devimator (DS p65)
 * Reset Value: 0
 */
#define ICM20948_REG_ACCEL_CONFIG_2 ((ICM20948_Reg_RW){ "ACCEL_CONFIG_2", 0x15, ICM20948_UBANK_2, 0b11100000 })
typedef enum ICM20948_accel_config_2_opts {
    // DEC3_CFG_1 or DEC3_CFG_4 by default (DS p65)
    ICM20948_DEC3_CFG_8 = 0b00000001,
    ICM20948_DEC3_CFG_16 = 0b00000010,
    ICM20948_DEC3_CFG_32 = 0b00000011,
    ICM20948_AZ_ST_EN_REG = 0b00000100,
    ICM20948_AY_ST_EN_REG = 0b00001000,
    ICM20948_AX_ST_EN_REG = 0b00010000
} ICM20948_accel_config_2_opts;

/* BIT 	| NAME 				| DESC
 * 7 	| DELAY_TIME_EN		| 1 enables delay time measurement between FSYNC event and the first ODR event after FSYNC event
 * 6	| -					| reserved
 * 5	| WOF_DEGLITCH_EN	| 1 enables digital deglitching of FSYNC input for wake on FSYNC
 * 4	| WOF_EDGE_INT		| wake on FSYNC is an edge (1) or level (0) interrupt
 * 3:0	| EXT_SYNC_SET		| enables the FSYNC pin data to be sampled for the chosen bit sync location (DS p66)
 * Reset Value: 0
 */
#define ICM20948_REG_FSYNC_CONFIG ((ICM20948_Reg_RW){ "FSYNC_CONFIG", 0x52, ICM20948_UBANK_2, 0b01000000 })
typedef enum ICM20948_fsync_config_opts {
    // EXT_SYNC_DISABLED by default
    ICM20948_EXT_SYNC_TEMP_OUT_L = 0b00000001,
    ICM20948_EXT_SYNC_GYRO_XOUT_L = 0b00000010,
    ICM20948_EXT_SYNC_GYRO_YOUT_L = 0b00000011,
    ICM20948_EXT_SYNC_GYRO_ZOUT_L = 0b00000100,
    ICM20948_EXT_SYNC_ACCEL_XOUT_L = 0b00000101,
    ICM20948_EXT_SYNC_ACCEL_YOUT_L = 0b00000110,
    ICM20948_EXT_SYNC_ACCEL_ZOUT_L = 0b00000111,
    ICM20948_WOF_EDGE_INT = 0b00010000,
    ICM20948_WOF_DEGLITCH_EN = 0b00100000,
    ICM20948_DELAY_TIME_EN = 0b10000000
} ICM20948_fsync_config_opts;

/* BIT 	| NAME 			| DESC
 * 7:3 	| -				| unused (not labeled reserved)
 * 2:0	| TEMP_DLPFCFG	| low pass filter config (DS p67)
 * Reset Value: 0
 */
#define ICM20948_REG_TEMP_CONFIG ((ICM20948_Reg_RW){ "TEMP_CONFIG", 0x53, ICM20948_UBANK_2, 0 })
typedef enum ICM20948_temp_config_opts {
    // TEMP_DLPFCFG_0 by default
    ICM20948_TEMP_DLPFCFG_1 = 0b00000001,
    ICM20948_TEMP_DLPFCFG_2 = 0b00000010,
    ICM20948_TEMP_DLPFCFG_3 = 0b00000011,
    ICM20948_TEMP_DLPFCFG_4 = 0b00000100,
    ICM20948_TEMP_DLPFCFG_5 = 0b00000101,
    ICM20948_TEMP_DLPFCFG_6 = 0b00000110,
    ICM20948_TEMP_DLPFCFG_7 = 0b00000111
} ICM20948_temp_config_opts;

/* BIT 	| NAME 					| DESC
 * 7:4 	| -						| reserved
 * 3:0	| I2C_MST_ODR_CONFIG	| sample rate dividor 1.1kHz/(2^I2C_MST_ODR_CONFIG)
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_MST_ODR_CONFIG ((ICM20948_Reg_RW){ "I2C_MST_ODR_CONFIG", 0x00, ICM20948_UBANK_3, 0b11110000 })

/* BIT 	| NAME 			| DESC
 * 7 	| MULT_MST_EN	| enables multi-master capability
 * 6:5	| -				| reserved
 * 4	| I2C_MST_P_NSR	| 1/0 sets transition btwn reads to stop/restart
 * 3:0	| I2C_MST_CLK	| sets I2C master clock frequency (DS p68)
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_MST_CTRL ((ICM20948_Reg_RW){ "I2C_MST_CTRL", 0x01, ICM20948_UBANK_3, 0b01100000 })
typedef enum ICM20948_i2c_mst_ctrl_opts {
    // I2C_MST_CLK_0 by default (DS p81)
    // I2C_MST_CLK_1 is undefined
    // I2C_MST_CLK_2 == I2C_MST_CLK_0
    ICM20948_I2C_MST_CLK_3 = 0b00000011,
    ICM20948_I2C_MST_CLK_4 = 0b00000100,
    // I2C_MST_CLK_5 == I2C_MST_CLK_3
    ICM20948_I2C_MST_CLK_6 = 0b00000110,
    ICM20948_I2C_MST_CLK_7 = 0b00000111,
    ICM20948_I2C_MST_CLK_8 = 0b00001000,
    // I2C_MST_CLK_9 == I2C_MST_CLK_3
    ICM20948_I2C_MST_CLK_10 = 0b00001010,
    // I2C_MST_CLK_11 == I2C_MST_CLK_10
    ICM20948_I2C_MST_CLK_12 = 0b00001100,
    // I2C_MST_CLK_13 == I2C_MST_CLK_3
    // I2C_MST_CLK_14 == I2C_MST_CLK_7
    // I2C_MST_CLK_15 == I2C_MST_CLK_7
    ICM20948_I2C_MST_P_NSR = 0b00010000,
    ICM20948_MULT_MST_EN = 0b10000000
} ICM20948_i2c_mst_ctrl_opts;

/* BIT 	| NAME 				| DESC
 * 7 	| DELAY_ES_SHADOW	| Delays shadowing of external sensor data until all data is received
 * 6:5	| -					| reserved
 * 4	| I2C_SLV4_DELAY_EN	| 1 sets slave 4 to only be accessed 1/(1+I2C_SLC4_DLY) samples
 * 3	| I2C_SLV3_DELAY_EN	| 1 sets slave 3 to only be accessed 1/(1+I2C_SLC4_DLY) samples
 * 2	| I2C_SLV2_DELAY_EN	| 1 sets slave 2 to only be accessed 1/(1+I2C_SLC4_DLY) samples
 * 1	| I2C_SLV1_DELAY_EN	| 1 sets slave 1 to only be accessed 1/(1+I2C_SLC4_DLY) samples
 * 0	| I2C_SLV0_DELAY_EN	| 1 sets slave 0 to only be accessed 1/(1+I2C_SLC4_DLY) samples
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_MST_DELAY_CTRL ((ICM20948_Reg_RW){ "I2C_MST_DELAY_CTRL", 0x02, ICM20948_UBANK_3, 0b01100000 })
typedef enum ICM20948_i2c_mst_delay_ctrl_opts {
    ICM20948_I2C_SLV0_DELAY_EN = 0b00000001,
    ICM20948_I2C_SLV1_DELAY_EN = 0b00000010,
    ICM20948_I2C_SLV2_DELAY_EN = 0b00000100,
    ICM20948_I2C_SLV3_DELAY_EN = 0b00001000,
    ICM20948_I2C_SLV4_DELAY_EN = 0b00010000,
    ICM20948_DELAY_ES_SHADOW = 0b10000000
} ICM20948_i2c_mst_delay_ctrl_opts;

/* BIT 	| NAME 			| DESC
 * 7 	| I2C_SLV0_RNW	| 1/0 – Transfer is a read/write
 * 6:0	| I2C_ID_0		| physical address of I2C slave 0
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV0_ADDR ((ICM20948_Reg_RW){ "I2C_SLV0_ADDR", 0x03, ICM20948_UBANK_3, 0 })
typedef enum ICM20948_i2c_slv_addr_opts { // same value for all I2C_SLVX_ADDR
    ICM20948_I2C_SLV_RNW = 0b10000000
} ICM20948_i2c_slv_addr_opts;

/* BIT 	| NAME 			| DESC
 * 7:0 	| I2C_SLV0_REG	| register address from where to begin data transfer
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV0_REG ((ICM20948_Reg_RW){ "I2C_SLV0_REG", 0x04, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 				| DESC
 * 7 	| I2C_SLV0_EN		| 1 reads data at the sample rate and stores it at the first available EXT_SENS_DATA register
 * 6 	| I2C_SLV0_BYTE_SW	| 1 enables swapping bytes when reading both the low and high byte of a word (DS p70)
 * 5 	| I2C_SLV0_REG_DIS	| 1 the transaction does not write a register value, it will only read data, or write data
 * 4 	| I2C_SLV0_GRP		| sets wether groups of bytes swapped end in odd (0) or even (1) registers
 * 3:0 	| I2C_SLV0_LENG		| number of bytes to be read from I2C slave 0
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV0_CTRL ((ICM20948_Reg_RW){ "I2C_SLV0_CTRL", 0x05, ICM20948_UBANK_3, 0 })
typedef enum ICM20948_i2c_slv_ctrl_opts { // same value for all I2C_SLVX_ADDR
                                          // I2C_SLV_LENG_0 by default
    ICM20948_I2C_SLV_LENG_1 = 0b00000001,
    ICM20948_I2C_SLV_LENG_2 = 0b00000010,
    ICM20948_I2C_SLV_LENG_3 = 0b00000011,
    ICM20948_I2C_SLV_LENG_4 = 0b00000100,
    ICM20948_I2C_SLV_LENG_5 = 0b00000101,
    ICM20948_I2C_SLV_LENG_6 = 0b00000110,
    ICM20948_I2C_SLV_LENG_7 = 0b00000111,
    ICM20948_I2C_SLV_LENG_8 = 0b00001000,
    ICM20948_I2C_SLV_LENG_9 = 0b00001001,
    ICM20948_I2C_SLV_LENG_10 = 0b00001010,
    ICM20948_I2C_SLV_LENG_11 = 0b00001011,
    ICM20948_I2C_SLV_LENG_12 = 0b00001100,
    ICM20948_I2C_SLV_LENG_13 = 0b00001101,
    ICM20948_I2C_SLV_LENG_14 = 0b00001110,
    ICM20948_I2C_SLV_LENG_15 = 0b00001111,
    ICM20948_I2C_SLV_GRP = 0b00010000,
    ICM20948_I2C_SLV_REG_DIS = 0b00100000,
    ICM20948_I2C_SLV_BYTE_SW = 0b01000000,
    ICM20948_I2C_SLV_EN = 0b10000000
} ICM20948_i2c_slv_ctrl_opts;

/* BIT 	| NAME 			| DESC
 * 7:0 	| I2C_SLV0_DO	| data out when slave 0 is set to write
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV0_DO ((ICM20948_Reg_RW){ "I2C_SLV0_DO", 0x06, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 			| DESC
 * 7 	| I2C_SLV1_RNW	| 1/0 – Transfer is a read/write
 * 6:0	| I2C_ID_1		| physical address of I2C slave 1
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV1_ADDR ((ICM20948_Reg_RW){ "I2C_SLV1_ADDR", 0x07, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 			| DESC
 * 7:0 	| I2C_SLV1_REG	| register address from where to begin data transfer
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV1_REG ((ICM20948_Reg_RW){ "I2C_SLV1_REG", 0x08, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 				| DESC
 * 7 	| I2C_SLV1_EN		| 1 reads data at the sample rate and stores it at the first available EXT_SENS_DATA register
 * 6 	| I2C_SLV1_BYTE_SW	| 1 enables swapping bytes when reading both the low and high byte of a word (DS p70)
 * 5 	| I2C_SLV1_REG_DIS	| 1 the transaction does not write a register value, it will only read data, or write data
 * 4 	| I2C_SLV1_GRP		| sets wether groups of bytes swapped end in odd (0) or even (1) registers
 * 3:0 	| I2C_SLV1_LENG		| number of bytes to be read from I2C slave 1
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV1_CTRL ((ICM20948_Reg_RW){ "I2C_SLV1_CTRL", 0x09, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 			| DESC
 * 7:0 	| I2C_SLV1_DO	| data out when slave 1 is set to write
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV1_DO ((ICM20948_Reg_RW){ "I2C_SLV1_DO", 0x0A, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 			| DESC
 * 7 	| I2C_SLV1_RNW	| 1/0 – Transfer is a read/write
 * 6:0	| I2C_ID_1		| physical address of I2C slave 2
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV2_ADDR ((ICM20948_Reg_RW){ "I2C_SLV2_ADDR", 0x0B, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 			| DESC
 * 7:0 	| I2C_SLV1_REG	| register address from where to begin data transfer
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV2_REG ((ICM20948_Reg_RW){ "I2C_SLV2_REG", 0x0C, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 				| DESC
 * 7 	| I2C_SLV1_EN		| 1 reads data at the sample rate and stores it at the first available EXT_SENS_DATA register
 * 6 	| I2C_SLV1_BYTE_SW	| 1 enables swapping bytes when reading both the low and high byte of a word (DS p70)
 * 5 	| I2C_SLV1_REG_DIS	| 1 the transaction does not write a register value, it will only read data, or write data
 * 4 	| I2C_SLV1_GRP		| sets wether groups of bytes swapped end in odd (0) or even (1) registers
 * 3:0 	| I2C_SLV1_LENG		| number of bytes to be read from I2C slave 2
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV2_CTRL ((ICM20948_Reg_RW){ "I2C_SLV2_CTRL", 0x0D, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 			| DESC
 * 7:0 	| I2C_SLV1_DO	| data out when slave 2 is set to write
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV2_DO ((ICM20948_Reg_RW){ "I2C_SLV2_DO", 0x0E, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 			| DESC
 * 7 	| I2C_SLV1_RNW	| 1/0 – Transfer is a read/write
 * 6:0	| I2C_ID_1		| physical address of I2C slave 3
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV3_ADDR ((ICM20948_Reg_RW){ "I2C_SLV3_ADDR", 0x0F, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 			| DESC
 * 7:0 	| I2C_SLV1_REG	| register address from where to begin data transfer
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV3_REG ((ICM20948_Reg_RW){ "I2C_SLV3_REG", 0x10, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 				| DESC
 * 7 	| I2C_SLV1_EN		| 1 reads data at the sample rate and stores it at the first available EXT_SENS_DATA register
 * 6 	| I2C_SLV1_BYTE_SW	| 1 enables swapping bytes when reading both the low and high byte of a word (DS p70)
 * 5 	| I2C_SLV1_REG_DIS	| 1 the transaction does not write a register value, it will only read data, or write data
 * 4 	| I2C_SLV1_GRP		| sets wether groups of bytes swapped end in odd (0) or even (1) registers
 * 3:0 	| I2C_SLV1_LENG		| number of bytes to be read from I2C slave 3
 * Reset Value: 0
 */
#define ICM20948_REG_I3C_SLV1_CTRL ((ICM20948_Reg_RW){ "I2C_SLV3_CTRL", 0x11, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 			| DESC
 * 7:0 	| I2C_SLV1_DO	| data out when slave 1 is set to write
 * Reset Value: 0
 */
#define ICM20948_REG_I3C_SLV1_DO ((ICM20948_Reg_RW){ "I2C_SLV3_DO", 0x12, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 			| DESC
 * 7 	| I2C_SLV4_RNW	| 1/0 – Transfer is a read/write
 * 6:0	| I2C_ID_4		| physical address of I2C slave 4
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV4_ADDR ((ICM20948_Reg_RW){ "I2C_SLV4_ADDR", 0x13, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 			| DESC
 * 7:0 	| I2C_SLV4_REG	| register address from where to begin data transfer
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV4_REG ((ICM20948_Reg_RW){ "I2C_SLV4_REG", 0x14, ICM20948_UBANK_3, 0 })

/* BIT 	| NAME 				| DESC
 * 7 	| I2C_SLV4_EN		| 1 data transfer at the sample rate and stores read data in I2C_SLV4_DI, bit clears afterwards
 * 6 	| I2C_SLV4_INT_EN	| 1 enables the completion of the I2C slave 4 data transfer to cause an interrupt
 * 5 	| I2C_SLV4_REG_DIS	| 1 the transaction does not write a register value, it will only read data, or write data
 * 4:0 	| I2C_SLV4_DLY		| when enabled via the I2C_MST_DELAY_CTRL, those slaves will only be enabled every 1/(1+I2C_SLV4_DLY)
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV4_CTRL ((ICM20948_Reg_RW){ "I2C_SLV4_CTRL", 0x15, ICM20948_UBANK_3, 0 })
typedef enum ICM20948_i2c_slv4_ctrl_opts {
    ICM20948_I2C_SLV4_DLY_1 = 0b00000001,
    ICM20948_I2C_SLV4_DLY_2 = 0b00000010,
    ICM20948_I2C_SLV4_DLY_3 = 0b00000011,
    ICM20948_I2C_SLV4_DLY_4 = 0b00000100,
    ICM20948_I2C_SLV4_DLY_5 = 0b00000101,
    ICM20948_I2C_SLV4_DLY_6 = 0b00000110,
    ICM20948_I2C_SLV4_DLY_7 = 0b00000111,
    ICM20948_I2C_SLV4_DLY_8 = 0b00001000,
    ICM20948_I2C_SLV4_DLY_9 = 0b00001001,
    ICM20948_I2C_SLV4_DLY_10 = 0b00001010,
    ICM20948_I2C_SLV4_DLY_11 = 0b00001011,
    ICM20948_I2C_SLV4_DLY_12 = 0b00001100,
    ICM20948_I2C_SLV4_DLY_13 = 0b00001101,
    ICM20948_I2C_SLV4_DLY_14 = 0b00001110,
    ICM20948_I2C_SLV4_DLY_15 = 0b00001111,
    ICM20948_I2C_SLV4_DLY_16 = 0b00010000,
    ICM20948_I2C_SLV4_DLY_17 = 0b00010001,
    ICM20948_I2C_SLV4_DLY_18 = 0b00010010,
    ICM20948_I2C_SLV4_DLY_19 = 0b00010011,
    ICM20948_I2C_SLV4_DLY_20 = 0b00010100,
    ICM20948_I2C_SLV4_DLY_21 = 0b00010101,
    ICM20948_I2C_SLV4_DLY_22 = 0b00010110,
    ICM20948_I2C_SLV4_DLY_23 = 0b00010111,
    ICM20948_I2C_SLV4_DLY_24 = 0b00011000,
    ICM20948_I2C_SLV4_DLY_25 = 0b00011001,
    ICM20948_I2C_SLV4_DLY_26 = 0b00011010,
    ICM20948_I2C_SLV4_DLY_27 = 0b00011011,
    ICM20948_I2C_SLV4_DLY_28 = 0b00011100,
    ICM20948_I2C_SLV4_DLY_29 = 0b00011101,
    ICM20948_I2C_SLV4_DLY_30 = 0b00011110,
    ICM20948_I2C_SLV4_DLY_31 = 0b00011111,
    ICM20948_I2C_SLV4_INT_EN = 0b01000000,
} ICM20948_i2c_slv4_ctrl_opts;

/* BIT 	| NAME 			| DESC
 * 7:0 	| I2C_SLV4_DO	| data out when slave 4 is set to write
 * Reset Value: 0
 */
#define ICM20948_REG_I2C_SLV4_DO ((ICM20948_Reg_RW){ "I2C_SLV4_DO", 0x16, ICM20948_UBANK_3, 0 })
#define ICM20948_REG_I2C_SLV4_DI ((ICM20948_reg_R){ "I2C_SLV4_DI", 0x17, ICM20948_UBANK_3 })

typedef struct ICM20948_int16_vector3 {
    int16_t x;
    int16_t y;
    int16_t z;
} ICM20948_int16_vector3;

typedef struct ICM20948_float_vector3 {
    float x;
    float y;
    float z;
} ICM20948_float_vector3;

// Start-up time for register read/write from power-up or reset
#define ICM20948_STARTUP_DELAY 100

/* Start-up time for sensor output from sleep
 * ACC ~ 30ms
 * GYR ~ 35ms
 * MAG ~ 8ms
 */
#define ICM20948_WAKE_DELAY 45

#define ICM20948_MAXIMUM_TIMEOUT 20

typedef enum ICM20948_SDO_pinouts {
    ICM20948_SDO_LOW,
    ICM20948_SDO_HIGH
} ICM20948_SDO_pinouts;

#define AK09916_ADDR_READ 0x8C
#define AK09916_ADDR_WRITE 0x0C
#define AK09916_WIA_VALUE 0x09

typedef enum cntl2_modes : uint8_t {
    AK09916_CNTL2_POWER_DOWN = 0,
    AK09916_CNTL2_SINGLE_MEASURE = 0b00000001,
    AK09916_CNTL2_CONT_MEASURE_1 = 0b00000010,
    AK09916_CNTL2_CONT_MEASURE_2 = 0b00000100,
    AK09916_CNTL2_CONT_MEASURE_3 = 0b00000110,
    AK09916_CNTL2_CONT_MEASURE_4 = 0b00001000,
    AK09916_CNTL2_SELF_TEST = 0b00010000
} AK09916_cntl2_modes;

typedef enum AK09916_cntl3_modes {
    AK09916_CNTL3_SOFT_RESET = 0b000001
} AK09916_cntl3_modes;

typedef enum AK09916_register : uint8_t {
    AK09916_WIA = 0x01,
    AK09916_ST1 = 0x10,
    AK09916_HXL = 0x11,
    AK09916_HXH = 0x12,
    AK09916_HYL = 0x13,
    AK09916_HYH = 0x14,
    AK09916_HZL = 0x15,
    AK09916_HZH = 0x16,
    AK09916_ST2 = 0x18,
    AK09916_CNTL2 = 0x31,
    AK09916_CNTL3 = 0x32
} AK09916_register;

HAL_StatusTypeDef ICM20948_Init(I2C_HandleTypeDef *hi2c, ICM20948_SDO_pinouts pinout);

HAL_StatusTypeDef ICM20948_ReadUserBank(uint8_t *data);
HAL_StatusTypeDef ICM20948_ChangeUserBank(ICM20948_user_bank ubank);

HAL_StatusTypeDef ICM20948_ReadRegister(const ICM20948_reg_R *regi, uint8_t *data);
HAL_StatusTypeDef ICM20948_ReadRegisters(const ICM20948_reg_R *regi, uint8_t *data, uint8_t size);
HAL_StatusTypeDef ICM20948_WriteRegister(const ICM20948_Reg_RW *regi, uint8_t data);
HAL_StatusTypeDef ICM20948_WriteRegisterEnables(const ICM20948_Reg_RW *regi, uint8_t data);
HAL_StatusTypeDef ICM20948_WriteRegisterEnDisables(const ICM20948_Reg_RW *regi, uint8_t data_en, uint8_t data_dis);
HAL_StatusTypeDef ICM20948_WriteRegisterDisables(const ICM20948_Reg_RW *regi, uint8_t data);

HAL_StatusTypeDef ICM20948_ReadAccelRegisters(ICM20948_int16_vector3 *accel);
HAL_StatusTypeDef ICM20948_ReadMagRegisters(ICM20948_int16_vector3 *mag);
HAL_StatusTypeDef ICM20948_ReadGyroRegisters(ICM20948_int16_vector3 *gyro);
HAL_StatusTypeDef ICM20948_ReadAccelGryoRegisters(ICM20948_int16_vector3 *accel, ICM20948_int16_vector3 *gyro);
ICM20948_float_vector3 ICM20948_ScaleSensorVector(ICM20948_int16_vector3 *sensor_v, float scale_factor);

HAL_StatusTypeDef ICM20948_MeasureGyroOffset(uint32_t ticks, ICM20948_int16_vector3 *gyro, uint32_t gyro_update_period_ms);
HAL_StatusTypeDef ICM20948_WriteGyroOffsetRegisters(const ICM20948_int16_vector3 *gyro_offset);

HAL_StatusTypeDef ICM20948_Wake();
HAL_StatusTypeDef ICM20948_Sleep();
HAL_StatusTypeDef ICM20948_Reset();

HAL_StatusTypeDef AK09916_Init(AK09916_cntl2_modes mode);
HAL_StatusTypeDef AK09916_ReadRegisters(AK09916_register regi, uint8_t size);
HAL_StatusTypeDef AK09916_SetCNTL2(AK09916_cntl2_modes mode);
HAL_StatusTypeDef AK09916_Reset();

#endif