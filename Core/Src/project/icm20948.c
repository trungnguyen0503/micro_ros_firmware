#include "project/icm20948.h"
#include "stm32f4xx_hal_def.h"

#define NUMBER_ONBOARD_SENSOR_REGISTERS 12     // accel and gyro each have 2 registers for 3 dof
#define NUMBER_ONBOARD_SENSOR_REGISTERS_HALF 6 // accel or gyro each have 2 registers for 3 dof

static uint16_t ICM20948_addr;
static I2C_HandleTypeDef *ICM20948_hi2c;
static ICM20948_user_bank ICM20948_current_user_bank;

static uint8_t AK09916_last_addr = 0;

static uint16_t CombineRegisters(uint8_t high, uint8_t low) {
    return ((uint16_t)high << 8) | low;
}

static HAL_StatusTypeDef CheckUserRegister(const ICM20948_reg_R *regi) {
    if (regi->bank != ICM20948_current_user_bank) {
        return ICM20948_ChangeUserBank(regi->bank);
    }
    return HAL_OK;
}

HAL_StatusTypeDef ICM20948_Init(I2C_HandleTypeDef *hi2c, ICM20948_SDO_pinouts SDO_pinout) {
    HAL_Delay(ICM20948_STARTUP_DELAY);

    ICM20948_hi2c = hi2c;
    if (SDO_pinout == ICM20948_SDO_LOW) {
        ICM20948_addr = ICM20948_ADDR_L;
    } else {
        ICM20948_addr = ICM20948_ADDR_H;
    }

    HAL_StatusTypeDef status = HAL_ERROR;
    uint8_t data = 0;

    status = ICM20948_ReadUserBank(&data);
    if (status != HAL_OK) {
        return status;
    }
    ICM20948_current_user_bank = data;

    status = ICM20948_ReadRegister(&ICM20948_REG_WHO_AM_I, &data);
    if (status != HAL_OK) {
        return status;
    }
    if (data != ICM20948_REG_WHO_AM_I_VALUE) {
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_StatusTypeDef ICM20948_ReadUserBank(uint8_t *data) {
    return HAL_I2C_Mem_Read(
        ICM20948_hi2c, ICM20948_addr, ICM20948_REG_UBANK_SEL.address,
        I2C_MEMADD_SIZE_8BIT, data, 1, ICM20948_MAXIMUM_TIMEOUT
    );
}

HAL_StatusTypeDef ICM20948_ChangeUserBank(ICM20948_user_bank ubank) {
    uint8_t data = 0;
    HAL_StatusTypeDef status = ICM20948_ReadUserBank(&data);
    if (status != HAL_OK) {
        return status;
    }

    uint8_t i_ubank = ((uint8_t)ubank) + (data & ICM20948_REG_UBANK_SEL_RESERVED_MASK);
    status = HAL_I2C_Mem_Write(
        ICM20948_hi2c, ICM20948_addr, ICM20948_REG_UBANK_SEL.address, I2C_MEMADD_SIZE_8BIT,
        &i_ubank, 1, ICM20948_MAXIMUM_TIMEOUT
    );
    if (status == HAL_OK) {
        ICM20948_current_user_bank = ubank;
    }
    return status;
}

HAL_StatusTypeDef ICM20948_ReadRegister(const ICM20948_reg_R *regi, uint8_t *data) {
    return ICM20948_ReadRegisters(regi, data, 1);
}

HAL_StatusTypeDef ICM20948_ReadRegisters(const ICM20948_reg_R *regi, uint8_t *data, uint8_t size) {
    HAL_StatusTypeDef status = CheckUserRegister(regi);
    if (status != HAL_OK) {
        return status;
    }
    return HAL_I2C_Mem_Read(
        ICM20948_hi2c, ICM20948_addr, regi->address,
        I2C_MEMADD_SIZE_8BIT, data, size, ICM20948_MAXIMUM_TIMEOUT
    );
}

HAL_StatusTypeDef ICM20948_WriteRegister(const ICM20948_Reg_RW *regi, uint8_t data) {
    if (regi->reserved_mask != 0) {
        uint8_t data_read = 0;
        HAL_StatusTypeDef status = ICM20948_ReadRegister((ICM20948_reg_R *)regi, &data_read);
        if (status != HAL_OK) {
            return status;
        }

        data = data & ~regi->reserved_mask;      // Make sure we aren't writing to reserved bits
        data += data_read & regi->reserved_mask; // Copy the value of reserved bits into data
    } else {                                     // ICM20948_ReadRegister already checks the userbank so we only do so if we don't read first
        HAL_StatusTypeDef status = CheckUserRegister((ICM20948_reg_R *)regi);
        if (status != HAL_OK) {
            return status;
        }
    }

    return HAL_I2C_Mem_Write(
        ICM20948_hi2c, ICM20948_addr, regi->address,
        I2C_MEMADD_SIZE_8BIT, &data, 1, ICM20948_MAXIMUM_TIMEOUT
    );
}

HAL_StatusTypeDef ICM20948_WriteRegisterEnables(const ICM20948_Reg_RW *regi, uint8_t data) {
    uint8_t data_read = 0;
    HAL_StatusTypeDef status = ICM20948_ReadRegister((ICM20948_reg_R *)regi, &data_read);
    if (status != HAL_OK) {
        return status;
    }

    data &= ~regi->reserved_mask; // Make sure we aren't writing to reserved bits
    data |= data_read;

    return HAL_I2C_Mem_Write(
        ICM20948_hi2c, ICM20948_addr, regi->address,
        I2C_MEMADD_SIZE_8BIT, &data, 1, ICM20948_MAXIMUM_TIMEOUT
    );
}

// data_en overrides data_dis
HAL_StatusTypeDef ICM20948_WriteRegisterEnDisables(
    const ICM20948_Reg_RW *regi,
    uint8_t data_en,
    uint8_t data_dis
) {
    uint8_t data_read = 0;
    HAL_StatusTypeDef status = ICM20948_ReadRegister((ICM20948_reg_R *)regi, &data_read);
    if (status != HAL_OK) {
        return status;
    }

    data_dis = ~data_dis | regi->reserved_mask; // Make sure we aren't writing to reserved bits
    data_dis &= data_read;

    data_en &= ~regi->reserved_mask; // Make sure we aren't writing to reserved bits
    data_en |= data_dis;

    return HAL_I2C_Mem_Write(
        ICM20948_hi2c, ICM20948_addr, regi->address,
        I2C_MEMADD_SIZE_8BIT, &data_en, 1, ICM20948_MAXIMUM_TIMEOUT
    );
}

HAL_StatusTypeDef ICM20948_WriteRegisterDisables(const ICM20948_Reg_RW *regi, uint8_t data) {
    uint8_t data_read = 0;
    HAL_StatusTypeDef status = ICM20948_ReadRegister((ICM20948_reg_R *)regi, &data_read);
    if (status != HAL_OK) {
        return status;
    }

    data = ~data | regi->reserved_mask; // Make sure we aren't writing to reserved bits
    data &= data_read;

    return HAL_I2C_Mem_Write(
        ICM20948_hi2c, ICM20948_addr, regi->address,
        I2C_MEMADD_SIZE_8BIT, &data, 1, ICM20948_MAXIMUM_TIMEOUT
    );
}

HAL_StatusTypeDef ICM20948_ReadAccelRegisters(ICM20948_int16_vector3 *accel) {
    uint8_t data[NUMBER_ONBOARD_SENSOR_REGISTERS_HALF];
    HAL_StatusTypeDef status = ICM20948_ReadRegisters(
        &ICM20948_REG_ACCEL_XOUT_H, data, NUMBER_ONBOARD_SENSOR_REGISTERS_HALF
    );
    if (status != HAL_OK) {
        return status;
    }

    accel->x = (int16_t)CombineRegisters(data[0], data[1]);
    accel->y = (int16_t)CombineRegisters(data[2], data[3]);
    accel->z = (int16_t)CombineRegisters(data[4], data[5]);
    return HAL_OK;
}

HAL_StatusTypeDef ICM20948_ReadMagRegisters(ICM20948_int16_vector3 *mag) {
    uint8_t data[NUMBER_ONBOARD_SENSOR_REGISTERS_HALF];
    HAL_StatusTypeDef status = ICM20948_ReadRegisters(
        &ICM20948_REG_EXT_SLV_SENS_DATA_00, data, NUMBER_ONBOARD_SENSOR_REGISTERS_HALF
    );
    if (status != HAL_OK) {
        return status;
    }

    mag->x = (int16_t)CombineRegisters(data[1], data[0]);
    mag->y = (int16_t)CombineRegisters(data[3], data[2]);
    mag->z = (int16_t)CombineRegisters(data[5], data[4]);
    return HAL_OK;
}

HAL_StatusTypeDef ICM20948_ReadGyroRegisters(ICM20948_int16_vector3 *gyro) {
    uint8_t data[NUMBER_ONBOARD_SENSOR_REGISTERS_HALF];
    HAL_StatusTypeDef status = ICM20948_ReadRegisters(
        &ICM20948_REG_GYRO_XOUT_H, data, NUMBER_ONBOARD_SENSOR_REGISTERS_HALF
    );
    if (status != HAL_OK) {
        return status;
    }

    gyro->x = (int16_t)CombineRegisters(data[0], data[1]);
    gyro->y = (int16_t)CombineRegisters(data[2], data[3]);
    gyro->z = (int16_t)CombineRegisters(data[4], data[5]);
    return HAL_OK;
}

HAL_StatusTypeDef ICM20948_ReadAccelGryoRegisters(
    ICM20948_int16_vector3 *accel,
    ICM20948_int16_vector3 *gyro
) {
    uint8_t data[NUMBER_ONBOARD_SENSOR_REGISTERS];
    HAL_StatusTypeDef status = ICM20948_ReadRegisters(
        &ICM20948_REG_ACCEL_XOUT_H, data, NUMBER_ONBOARD_SENSOR_REGISTERS
    );
    if (status != HAL_OK) {
        return status;
    }

    accel->x = (int16_t)CombineRegisters(data[0], data[1]);
    accel->y = (int16_t)CombineRegisters(data[2], data[3]);
    accel->z = (int16_t)CombineRegisters(data[4], data[5]);
    gyro->x = (int16_t)CombineRegisters(data[6], data[7]);
    gyro->y = (int16_t)CombineRegisters(data[8], data[9]);
    gyro->z = (int16_t)CombineRegisters(data[10], data[11]);
    return HAL_OK;
}

ICM20948_float_vector3 ICM20948_ScaleSensorVector(ICM20948_int16_vector3 *sensor_v, float scale_factor) {
    ICM20948_float_vector3 ret;
    ret.x = (float)sensor_v->x * scale_factor;
    ret.y = (float)sensor_v->y * scale_factor;
    ret.z = (float)sensor_v->z * scale_factor;
    return ret;
}

HAL_StatusTypeDef ICM20948_MeasureGyroOffset(
    uint32_t ticks,
    ICM20948_int16_vector3 *gyro_offset,
    uint32_t gyro_update_period_ms
) {
    int64_t gyro_sum_x = 0;
    int64_t gyro_sum_y = 0;
    int64_t gyro_sum_z = 0;

    for (uint32_t i = 0; i < ticks; i++) {
        uint32_t start_time = HAL_GetTick();

        HAL_StatusTypeDef status = ICM20948_ReadGyroRegisters(gyro_offset);
        if (status != HAL_OK) {
            return status;
        }

        gyro_sum_x += gyro_offset->x;
        gyro_sum_y += gyro_offset->y;
        gyro_sum_z += gyro_offset->z;

        int64_t wait_time = gyro_update_period_ms + start_time - (int64_t)HAL_GetTick();
        if (wait_time > 0) {
            HAL_Delay((uint32_t)wait_time);
        }
    }

    gyro_offset->x = (int16_t)(gyro_sum_x / ticks);
    gyro_offset->y = (int16_t)(gyro_sum_y / ticks);
    gyro_offset->z = (int16_t)(gyro_sum_z / ticks);

    return HAL_OK;
}

HAL_StatusTypeDef ICM20948_WriteGyroOffsetRegisters(const ICM20948_int16_vector3 *gyro_offset) {
    HAL_StatusTypeDef status = HAL_ERROR;

    status = ICM20948_WriteRegister(&ICM20948_REG_XG_OFFS_USRH, (uint8_t)(gyro_offset->x >> 8));
    if (status != HAL_OK) {
        return status;
    }
    status = ICM20948_WriteRegister(&ICM20948_REG_XG_OFFS_USRL, (uint8_t)gyro_offset->x);
    if (status != HAL_OK) {
        return status;
    }
    status = ICM20948_WriteRegister(&ICM20948_REG_YG_OFFS_USRH, (uint8_t)(gyro_offset->y >> 8));
    if (status != HAL_OK) {
        return status;
    }
    status = ICM20948_WriteRegister(&ICM20948_REG_YG_OFFS_USRL, (uint8_t)gyro_offset->y);
    if (status != HAL_OK) {
        return status;
    }
    status = ICM20948_WriteRegister(&ICM20948_REG_ZG_OFFS_USRH, (uint8_t)(gyro_offset->z >> 8));
    if (status != HAL_OK) {
        return status;
    }
    status = ICM20948_WriteRegister(&ICM20948_REG_ZG_OFFS_USRL, (uint8_t)gyro_offset->z);
    if (status != HAL_OK) {
        return status;
    }
    return HAL_OK;
}

HAL_StatusTypeDef ICM20948_Wake() {
    HAL_StatusTypeDef status = ICM20948_WriteRegisterDisables(
        &ICM20948_REG_PWR_MGMT_1, ICM20948_SLEEP
    );
    if (status == HAL_OK) {
        HAL_Delay(ICM20948_WAKE_DELAY);
    }
    return status;
}

HAL_StatusTypeDef ICM20948_Sleep() {
    return ICM20948_WriteRegisterEnables(&ICM20948_REG_PWR_MGMT_1, ICM20948_SLEEP);
}

HAL_StatusTypeDef ICM20948_Reset() {
    HAL_StatusTypeDef status = ICM20948_WriteRegisterEnables(
        &ICM20948_REG_PWR_MGMT_1, ICM20948_DEVICE_RESET
    );
    if (status == HAL_OK) {
        HAL_Delay(ICM20948_STARTUP_DELAY);
    }
    return status;
}

HAL_StatusTypeDef AK09916_Init(AK09916_cntl2_modes mode) {
    HAL_StatusTypeDef status = HAL_ERROR;

    status = ICM20948_WriteRegisterEnables(&ICM20948_REG_USER_CTRL, ICM20948_I2C_MST_RST);
    if (status != HAL_OK) {
        return status;
    }
    HAL_Delay(ICM20948_STARTUP_DELAY);
    status = ICM20948_WriteRegisterEnables(&ICM20948_REG_USER_CTRL, ICM20948_I2C_MST_EN);
    if (status != HAL_OK) {
        return status;
    }
    status = ICM20948_WriteRegisterEnables(
        &ICM20948_REG_I2C_MST_CTRL,
        (mode == AK09916_CNTL2_SINGLE_MEASURE)
            ? ICM20948_I2C_MST_P_NSR | ICM20948_I2C_MST_CLK_7
            : ICM20948_I2C_MST_CLK_7
    );
    if (status != HAL_OK) {
        return status;
    }
    status = AK09916_Reset();
    if (status != HAL_OK) {
        return status;
    }
    if (mode == AK09916_CNTL2_POWER_DOWN || mode == AK09916_CNTL2_SELF_TEST) {
        return AK09916_SetCNTL2(mode);
    }
    status = AK09916_ReadRegisters(AK09916_WIA, ICM20948_I2C_SLV_LENG_1);
    if (status != HAL_OK) {
        return status;
    }
    HAL_Delay(ICM20948_MAXIMUM_TIMEOUT);
    uint8_t data = 0;
    status = ICM20948_ReadRegisters(
        (ICM20948_reg_R *)&ICM20948_REG_EXT_SLV_SENS_DATA_00, &data, ICM20948_I2C_SLV_LENG_1
    );
    if (status != HAL_OK) {
        return status;
    }
    if (data != AK09916_WIA_VALUE) {
        return HAL_ERROR;
    }
    status = AK09916_SetCNTL2(mode);
    if (status != HAL_OK) {
        return status;
    }
    HAL_Delay(ICM20948_MAXIMUM_TIMEOUT);
    return AK09916_ReadRegisters(AK09916_HXL, ICM20948_I2C_SLV_LENG_8);
}

HAL_StatusTypeDef AK09916_ReadRegisters(AK09916_register regi, uint8_t size) {
    HAL_StatusTypeDef status = HAL_ERROR;

    if (size > ICM20948_I2C_SLV_LENG_15 || size == 0) {
        return HAL_ERROR;
    }
    if (AK09916_last_addr != AK09916_ADDR_READ) {
        status = ICM20948_WriteRegister(&ICM20948_REG_I2C_SLV0_ADDR, AK09916_ADDR_READ);
        if (status != HAL_OK) {
            return status;
        }
        AK09916_last_addr = AK09916_ADDR_READ;
    }
    status = ICM20948_WriteRegister(&ICM20948_REG_I2C_SLV0_REG, regi);
    if (status != HAL_OK) {
        return status;
    }
    status = ICM20948_WriteRegisterEnDisables(
        &ICM20948_REG_I2C_SLV0_CTRL, ICM20948_I2C_SLV_EN | size, ICM20948_I2C_SLV_LENG_15
    );
    return status;
}

// Unsafe local write function for the two valid AK09916 registers
HAL_StatusTypeDef AK09916_WriteRegister(uint8_t regi, uint8_t data) {
    HAL_StatusTypeDef status = HAL_ERROR;

    if (AK09916_last_addr != AK09916_ADDR_WRITE) {
        status = ICM20948_WriteRegister(&ICM20948_REG_I2C_SLV0_ADDR, AK09916_ADDR_WRITE);
        if (status != HAL_OK) {
            return status;
        }
        AK09916_last_addr = AK09916_ADDR_WRITE;
    }
    status = ICM20948_WriteRegister(&ICM20948_REG_I2C_SLV0_REG, regi);
    if (status != HAL_OK) {
        return status;
    }
    status = ICM20948_WriteRegister(&ICM20948_REG_I2C_SLV0_DO, data);
    if (status != HAL_OK) {
        return status;
    }
    return ICM20948_WriteRegisterEnDisables(
        &ICM20948_REG_I2C_SLV0_CTRL,
        ICM20948_I2C_SLV_EN | ICM20948_I2C_SLV_LENG_1, ICM20948_I2C_SLV_LENG_15
    );
}

HAL_StatusTypeDef AK09916_SetCNTL2(AK09916_cntl2_modes mode) {
    return AK09916_WriteRegister(AK09916_CNTL2, mode);
}

HAL_StatusTypeDef AK09916_Reset() {
    HAL_StatusTypeDef status = AK09916_WriteRegister(AK09916_CNTL3, AK09916_CNTL3_SOFT_RESET);
    if (status == HAL_OK) {
        HAL_Delay(ICM20948_STARTUP_DELAY);
    }
    return status;
}
