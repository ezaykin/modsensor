/*
 * modbus.c
 *
 * Created: 27.10.2018 17:13:34
 *  Author: Evgeny
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "Modbus.h"
#include "Timers.h"
#include "Uart.h"

#define ADDR_OFFSET     0
#define FUNC_OFFSET     1
#define DATA_OFFSET     2
#define COUNT_OFFSET    4
#define FC_COILS_OFFSET 7

#define OUTPUT_LEN_OFFSET       2
#define OUTPUT_DATA_OFFSET      3
#define OUTPUT_FCDATA1_OFFSET   2
#define OUTPUT_FCDATA2_OFFSET   4

#define READ_COIL_STATUS        0x01
#define READ_INPUT_STATUS       0x02
#define READ_INPUT_REGISTERS    0x04
#define FORCE_SINGLE_COIL       0x05
#define FORCE_MULTIPLE_COILS    0x0F

#define ERROR_FLAG          0x80
#define NO_ERRORS           0x00
#define ILLEGAL_FUNCTION    0x01
#define ILLEGAL_ADDRESS     0x02
#define ILLEGAL_DATA_VALUE  0x03


#define ntohs(a)            htons(a)
#define OUTPUT_BUFFER_SIZE (MODBUS_INPUTREG_COUNT*2+8)

#define HEADER_LEN  3
#define FC05_LEN    6
#define COIL_ON     0xFF00

extern uint8_t          g_UartBuffer[];
extern volatile int     g_nUartBytesCount;


uint16_t CRC16(uint8_t *p, uint16_t len);
void UartInit();

static int16_t nInputRegisters[MODBUS_INPUTREG_COUNT];
static uint8_t nCoils [MODBUS_COILS_COUNT];
static uint8_t nInputs [MODBUS_INPUTS_COUNT];

static uint16_t htons(uint16_t val)
{
    return (val<<8) | (val>>8);
}

void ModbusInit()
{
    memset(nInputRegisters,0,sizeof(nInputRegisters));
    memset(nCoils,0,sizeof(nCoils));
    memset(nInputs,0,sizeof(nInputs));
    ModbusTimerInit();
    UartInit();
    UartStart();
}

void SetInputRegister(int nRegister, uint16_t nValue)
{
    if (nRegister<MODBUS_INPUTREG_COUNT)
    {
        nInputRegisters[nRegister]=nValue;
    }
}

void SetDiscreteInput(int nInput, uint8_t nValue)
{
    if (nInput<MODBUS_INPUTREG_COUNT)
    {
        nInputs[nInput]=nValue;
    }
}

uint8_t GetCoilValue(int nCoil)
{
    uint8_t Result=0;
    if (nCoil<MODBUS_COILS_COUNT)
    {
        Result=nCoils[nCoil];
    }
    return Result;
}

void ModbusHandler()
{
    uint8_t nErrorCode=NO_ERRORS;
    uint8_t OuputBuffer[OUTPUT_BUFFER_SIZE];
    int OutputLen=0;
    if (g_UartBuffer[ADDR_OFFSET]==MODBUS_ADDR)
    {
        uint16_t* pCRC=(uint16_t*)(g_UartBuffer+g_nUartBytesCount-2);
        if (CRC16(g_UartBuffer, g_nUartBytesCount-2)==ntohs(*pCRC))
        {
            OuputBuffer[ADDR_OFFSET]=MODBUS_ADDR;
            OuputBuffer[FUNC_OFFSET]=g_UartBuffer[FUNC_OFFSET];
            uint16_t* pRegister = (uint16_t*)(g_UartBuffer+DATA_OFFSET);
            uint16_t* pCount = (uint16_t*)(g_UartBuffer+COUNT_OFFSET);
            uint16_t nRegister = ntohs(*pRegister);
            uint16_t nCount = ntohs(*pCount);
            nErrorCode=NO_ERRORS;
            switch(g_UartBuffer[FUNC_OFFSET])
            {
                case READ_COIL_STATUS:
                    if((nCount>0) && ((nRegister+nCount)<=MODBUS_COILS_COUNT))
                    {
                        //эта версия поддерживает чтение не более 8 значений выходов, поэтому во фрейме фикс. длина 1 байт
                        OuputBuffer[OUTPUT_LEN_OFFSET] = 1;
                        uint8_t CoilsValue=0;
                        for (int i=0;i<nCount;++i)
                        {
                            if (nCoils[nRegister+i])
                            {
                                CoilsValue|=(1<<i);
                            }
                        }
                        OuputBuffer[OUTPUT_DATA_OFFSET]=CoilsValue;
                        OutputLen=HEADER_LEN+OuputBuffer[OUTPUT_LEN_OFFSET];
                    }
                    else
                    {
                        nErrorCode=ILLEGAL_ADDRESS;
                    }
                break;
                case READ_INPUT_STATUS:
                if((nCount>0) && ((nRegister+nCount)<=MODBUS_INPUTS_COUNT))
                {
                    //эта версия поддерживает чтение не более 8 значений входов, поэтому во фрейме фикс. длина 1 байт
                    OuputBuffer[OUTPUT_LEN_OFFSET] = 1;
                    uint8_t InputsValue=0;
                    for (int i=0;i<nCount;++i)
                    {
                        if (nInputs[nRegister+i])
                        {
                            InputsValue|=(1<<i);
                        }
                    }
                    OuputBuffer[OUTPUT_DATA_OFFSET]=InputsValue;
                    OutputLen=HEADER_LEN+OuputBuffer[OUTPUT_LEN_OFFSET];
                }
                else
                {
                    nErrorCode=ILLEGAL_ADDRESS;
                }
                break;
                case READ_INPUT_REGISTERS:
                    if((nCount>0) && ((nRegister+nCount)<=MODBUS_INPUTREG_COUNT))
                    {
                        OuputBuffer[OUTPUT_LEN_OFFSET] = nCount * sizeof(uint16_t);
                        pRegister=(uint16_t*)(OuputBuffer+OUTPUT_DATA_OFFSET);
                        for (int i=0;i<nCount;++i)
                        {
                            *pRegister++=htons(nInputRegisters[nRegister+i]);
                        }
                        OutputLen=HEADER_LEN+OuputBuffer[OUTPUT_LEN_OFFSET];
                    }
                    else
                    {
                        nErrorCode=ILLEGAL_ADDRESS;
                    }
                break;
                case FORCE_SINGLE_COIL:
                    if(nRegister<=MODBUS_COILS_COUNT)
                    {
                        if (nCount==COIL_ON)
                        {
                            nCoils[nRegister]=1;
                        }
                        else if(nCount==0){
                            nCoils[nRegister]=0;
                        }
                        else {
                            nErrorCode=ILLEGAL_DATA_VALUE;
                        }
                        if (NO_ERRORS==nErrorCode)
                        {
                            pRegister=(uint16_t*)(OuputBuffer+OUTPUT_FCDATA1_OFFSET);
                            *pRegister = htons(nRegister);
                            pCount=(uint16_t*)(OuputBuffer+OUTPUT_FCDATA2_OFFSET);
                            *pCount=htons(nCount);
                            OutputLen=FC05_LEN;
                        }
                    }
                    else
                    {
                        nErrorCode=ILLEGAL_ADDRESS;
                    }
                break;
                case FORCE_MULTIPLE_COILS:
                    if((nCount>0) && ((nRegister+nCount)<=MODBUS_COILS_COUNT))
                    {
                        uint8_t CoilsData=g_UartBuffer[FC_COILS_OFFSET];
                        for (int i=0;i<nCount;++i)
                        {
                            if (CoilsData & (1<<i))
                            {
                                nCoils[nRegister+i]=1;
                            } 
                            else
                            {
                                nCoils[nRegister+i]=0;
                            }
                        }
                        pRegister=(uint16_t*)(OuputBuffer+OUTPUT_FCDATA1_OFFSET);
                        *pRegister = htons(nRegister);
                        pCount=(uint16_t*)(OuputBuffer+OUTPUT_FCDATA2_OFFSET);
                        *pCount=htons(nCount);
                        OutputLen=FC05_LEN;
                    }
                    else
                    {
                        nErrorCode=ILLEGAL_ADDRESS;
                    }
                break;
                default:
                    nErrorCode=ILLEGAL_FUNCTION;
            }
            if (NO_ERRORS!=nErrorCode)
            {
                OuputBuffer[FUNC_OFFSET]|=ERROR_FLAG;
                OuputBuffer[DATA_OFFSET]=nErrorCode;
                OutputLen=HEADER_LEN;
            }
        }
        pCRC = (uint16_t*)(OuputBuffer+OutputLen);
        *pCRC = htons(CRC16(OuputBuffer,OutputLen));
        OutputLen+=sizeof(uint16_t);
    }
    UartStart();
    UartWrite(OuputBuffer, OutputLen);
}

