/*
 * bmp280.c
 *
 * Created: 29.10.2018 20:16:46
 *  Author: Evgeny
 */ 
#include "Globals.h"
#include "HAL/twi.h"

#include <util/delay.h>
#include <string.h>

static const int BMP_ADR        = 0x76;
static const int BMP_ID         = 0x58;
static const int BMP_ID_REG     = 0xD0;
static const int BMP_CONFIG_REG = 0xF4;
static const int BMP_CALIBR_REG = 0x88;
static const int BMP_DATA_REG   = 0xF7;

static const int BMP_CTRL_MEAS =  ((0x05<<5)|(0x05<<2)|(0x03));    //ctrl_meas: mode normal, osrs_p=101, osrs_t=101 
static const int BMP_CONFIG    =  ((0x07<<5)|(0x04<<2));           //config: filter=100, t_sb=111 (4000ms)

static const int TWI_TIMEOUT_MKS    = 30000;
static const int BMP_RETRY_INTERVAL = 10;
static const int MAX_RETRY_COUNT    = 5;
static const int TWI_BAUD           = 400;

#define BMP_CONFIG_SIZE 4

static struct __attribute__ ((packed))
{
    uint8_t DeviceAddr;
    uint16_t T1;
    int16_t T2;
    int16_t T3;
    uint16_t P1;
    int16_t P2;
    int16_t P3;
    int16_t P4;
    int16_t P5;
    int16_t P6;
    int16_t P7;
    int16_t P8;
    int16_t P9;
} stBmpCalibrData;

static struct __attribute__ ((packed))
{
    uint8_t DeviceAddr;
    uint8_t press[3];
    uint8_t temp[3];
} stBmpSensorData;

/*(from BMP280 datasheet)*/
static int32_t t_fine = 0;
/* Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC. */
static int32_t bmp280_compensate_T_int32() {
    int32_t var1, var2, T;
    int32_t adc_T = stBmpSensorData.temp[0];
    adc_T = (adc_T<<8)|stBmpSensorData.temp[1];
    adc_T = (adc_T<<4)|(stBmpSensorData.temp[2]>>4);

    var1 = ((((adc_T >> 3) - ((int32_t)stBmpCalibrData.T1 << 1))) * ((int32_t)stBmpCalibrData.T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)stBmpCalibrData.T1)) * ((adc_T >> 4) - ((int32_t)stBmpCalibrData.T1))) >> 12) *
    ((int32_t)stBmpCalibrData.T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

/* Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa */
static uint32_t bmp280_compensate_P_int32() {
    int32_t var1, var2;
    uint32_t x;
    int32_t adc_P = stBmpSensorData.press[0];
    adc_P=(adc_P<<8)|stBmpSensorData.press[1];
    adc_P=(adc_P<<4)|(stBmpSensorData.press[2]>>4); // 3852
    var1 = (((int32_t) t_fine) >> 1) - (int32_t) 64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t) stBmpCalibrData.P6);
    var2 = var2 + ((var1 * ((int32_t) stBmpCalibrData.P5)) << 1);
    var2 = (var2 >> 2) + (((int32_t) stBmpCalibrData.P4) << 16);
    var1 = (((stBmpCalibrData.P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t) stBmpCalibrData.P2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((int32_t) stBmpCalibrData.P1)) >> 15);
    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    x = (((uint32_t)(((int32_t) 1048576) - adc_P) - (var2 >> 12)))	* 3125;
    if (x < 0x80000000) {
        x = (x << 1) / ((uint32_t) var1);
        } else {
        x = (x / (uint32_t) var1) * 2;
    }
    var1 = (((int32_t) stBmpCalibrData.P9) * ((int32_t) (((x >> 3) * (x >> 3)) >> 13))) >> 12;
    var2 = (((int32_t) (x >> 2)) * ((int32_t) stBmpCalibrData.P8)) >> 13;
    x = (uint32_t)((int32_t) x + ((var1 + var2 + stBmpCalibrData.P7) >> 4));
    return x;
}

static void OnTwiIsr()
{
    g_nStatus |= EVENT_TWI;
}

static int WaitTwiEvent()
{
    for(int i=0; i<TWI_TIMEOUT_MKS; ++i) {
        if (g_nStatus & EVENT_TWI) {
            g_nStatus &= ~EVENT_TWI;
            return 1;
        }
        _delay_us(1);
    }
    return 0;
}

int BmpInit()
{
    memset(&stBmpCalibrData, 0, sizeof(stBmpCalibrData));
    memset(&stBmpSensorData, 0, sizeof(stBmpSensorData));
    stBmpCalibrData.DeviceAddr = (BMP_ADR<<1);
    stBmpSensorData.DeviceAddr = (BMP_ADR<<1);

    if(TWI_MasterInit(&OnTwiIsr, TWI_BAUD))
    {
        for (int i=0; i<MAX_RETRY_COUNT; ++i)
        {
            //для настройки BMP280 используются 3 регистра + 1 регистр адреса
            uint8_t ConfigBuffer[BMP_CONFIG_SIZE];
            ConfigBuffer[0] = (BMP_ADR<<1);
            //сначала прочитаем ID устройства
            TwiStartRead(ConfigBuffer, BMP_ID_REG, 2);
            if(WaitTwiEvent()) {
                if (BMP_ID == ConfigBuffer[1])
                {
                    //прочитаем калибровочные данные
                    TwiStartRead(&stBmpCalibrData, BMP_CALIBR_REG, sizeof(stBmpCalibrData));
                    if(WaitTwiEvent()) {
                        //запишем настройки в датчик
                        ConfigBuffer[1] = BMP_CONFIG_REG;
                        ConfigBuffer[2] = BMP_CTRL_MEAS;
                        ConfigBuffer[3] = BMP_CONFIG;
                        TwiStartWrite(ConfigBuffer, BMP_CONFIG_SIZE);
                        if(WaitTwiEvent()) {
                            return 1;
                        }
                    }
                }
                _delay_ms(BMP_RETRY_INTERVAL);
            }
        }
    }	
    return 0;
}

void BmpStart()
{
    TwiStartRead(&stBmpSensorData, BMP_DATA_REG, sizeof(stBmpSensorData));
}

int16_t BmpGetTemperature()
{
    return bmp280_compensate_T_int32()/10l;		//возвращаем температуру в десятых долях градуса
}

int16_t BmpGetPressure()
{
    return bmp280_compensate_P_int32()/10ul;
}
