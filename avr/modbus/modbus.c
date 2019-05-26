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

#define UART_BUFFER_SIZE    32

uint16_t CRC16(uint8_t *p, uint16_t len);


static struct {
    uint8_t InputBuffer[UART_BUFFER_SIZE];
    int nBytesCount;
} stUartData;

static struct {
    int16_t nInputRegisters[MODBUS_INPUTREG_COUNT];
    uint8_t nCoils [MODBUS_COILS_COUNT];
    uint8_t nInputs [MODBUS_INPUTS_COUNT];
} stModbusData;

static uint16_t htons(uint16_t val)
{
    return (val<<8) | (val>>8);
}

static int ReadCoilStatus(uint16_t nRegister, uint16_t nCount, uint8_t* pOutputBuffer, uint8_t* pErrorCode)
{
    int OutputLen=0;
    if((nCount>0) && ((nRegister+nCount)<=MODBUS_COILS_COUNT))
    {
        //эта версия поддерживает чтение не более 8 значений выходов, поэтому во фрейме фикс. длина 1 байт
        pOutputBuffer[OUTPUT_LEN_OFFSET] = 1;
        uint8_t CoilsValue=0;
        for (int i=0;i<nCount;++i)
        {
            if (stModbusData.nCoils[nRegister+i])
            {
                CoilsValue|=(1<<i);
            }
        }
        pOutputBuffer[OUTPUT_DATA_OFFSET]=CoilsValue;
        OutputLen=HEADER_LEN+pOutputBuffer[OUTPUT_LEN_OFFSET];
    }
    else
    {
        *pErrorCode = ILLEGAL_ADDRESS;
    }
    return OutputLen;
}

static int ReadInputStatus(uint16_t nRegister, uint16_t nCount, uint8_t* pOutputBuffer, uint8_t* pErrorCode)
{
    int OutputLen=0;
    if((nCount>0) && ((nRegister+nCount)<=MODBUS_INPUTS_COUNT))
    {
        //эта версия поддерживает чтение не более 8 значений входов, поэтому во фрейме фикс. длина 1 байт
        pOutputBuffer[OUTPUT_LEN_OFFSET] = 1;
        uint8_t InputsValue=0;
        for (int i=0;i<nCount;++i)
        {
            if (stModbusData.nInputs[nRegister+i])
            {
                InputsValue|=(1<<i);
            }
        }
        pOutputBuffer[OUTPUT_DATA_OFFSET]=InputsValue;
        OutputLen=HEADER_LEN+pOutputBuffer[OUTPUT_LEN_OFFSET];
    }
    else
    {
        *pErrorCode = ILLEGAL_ADDRESS;
    }
    return OutputLen;
}   
 
static int ReadInputRegisters(uint16_t nRegister, uint16_t nCount, uint8_t* pOutputBuffer, uint8_t* pErrorCode)
{
    int OutputLen=0;
    if((nCount>0) && ((nRegister+nCount)<=MODBUS_INPUTREG_COUNT))
    {
        pOutputBuffer[OUTPUT_LEN_OFFSET] = nCount * sizeof(uint16_t);
        uint16_t* pOutputRegister=(uint16_t*)(pOutputBuffer+OUTPUT_DATA_OFFSET);
        for (int i=0;i<nCount;++i)
        {
            *pOutputRegister++=htons(stModbusData.nInputRegisters[nRegister+i]);
        }
        OutputLen=HEADER_LEN+pOutputBuffer[OUTPUT_LEN_OFFSET];
    }
    else
    {
        *pErrorCode = ILLEGAL_ADDRESS;
    }
    return OutputLen;
}

static int ForceSingeCoil(uint16_t nRegister, uint16_t nValue, uint8_t* pOutputBuffer, uint8_t* pErrorCode)
{
    int OutputLen=0;
    if(nRegister<=MODBUS_COILS_COUNT)
    {
        if (nValue==COIL_ON)
        {
            stModbusData.nCoils[nRegister]=1;
        }
        else if(nValue==0){
            stModbusData.nCoils[nRegister]=0;
        }
        else {
            *pErrorCode=ILLEGAL_DATA_VALUE;
        }
        if (NO_ERRORS==*pErrorCode)
        {
            uint16_t* pOutputRegister=(uint16_t*)(pOutputBuffer+OUTPUT_FCDATA1_OFFSET);
            *pOutputRegister = htons(nRegister);
            uint16_t* pOutputValue=(uint16_t*)(pOutputBuffer+OUTPUT_FCDATA2_OFFSET);
            *pOutputValue=htons(nValue);
            OutputLen=FC05_LEN;
        }
    }
    else
    {
        *pErrorCode = ILLEGAL_ADDRESS;
    }
    return OutputLen;
}

static int ForceMultipleCoils(uint16_t nRegister, uint16_t nCount, uint8_t* pOutputBuffer, uint8_t nCoilsData, uint8_t* pErrorCode)
{
    int OutputLen=0;
    if((nCount>0) && ((nRegister+nCount)<=MODBUS_COILS_COUNT))
    {
        //будет работать только до 8 выходов
        for (int i=0;i<nCount;++i)
        {
            if (nCoilsData & (1<<i))
            {
                stModbusData.nCoils[nRegister+i]=1;
            }
            else
            {
                stModbusData.nCoils[nRegister+i]=0;
            }
        }
        uint16_t* pOutputRegister=(uint16_t*)(pOutputBuffer+OUTPUT_FCDATA1_OFFSET);
        *pOutputRegister = htons(nRegister);
        uint16_t* pOutputCount=(uint16_t*)(pOutputBuffer+OUTPUT_FCDATA2_OFFSET);
        *pOutputCount=htons(nCount);
        OutputLen=FC05_LEN;
    }    
    else
    {
        *pErrorCode = ILLEGAL_ADDRESS;
    }
    return OutputLen;
}

static int DecodeModbusData(uint8_t* pInputData, int nInputSize, uint8_t* pOutputBuffer)
{
    uint8_t nErrorCode=NO_ERRORS;
    int OutputLen=0;
    if (pInputData[ADDR_OFFSET]==MODBUS_ADDR)
    {
        uint16_t* pCRC=(uint16_t*)(pInputData+nInputSize-2);
        if (CRC16(pInputData, nInputSize-2)==ntohs(*pCRC))
        {
            pOutputBuffer[ADDR_OFFSET]=MODBUS_ADDR;
            pOutputBuffer[FUNC_OFFSET]=pInputData[FUNC_OFFSET];
            uint16_t* pRegister = (uint16_t*)(pInputData+DATA_OFFSET);
            uint16_t nRegister=htons(*pRegister);
            uint16_t* pCount = (uint16_t*)(pInputData+COUNT_OFFSET);
            uint16_t nCount = ntohs(*pCount);
            nErrorCode=NO_ERRORS;
            switch(pInputData[FUNC_OFFSET])
            {
                case READ_COIL_STATUS:
                    OutputLen = ReadCoilStatus(nRegister,nCount,pOutputBuffer,&nErrorCode);
                break;
                case READ_INPUT_STATUS:
                    OutputLen = ReadInputStatus(nRegister,nCount,pOutputBuffer,&nErrorCode);
                break;
                case READ_INPUT_REGISTERS:
                    OutputLen = ReadInputRegisters(nRegister,nCount,pOutputBuffer,&nErrorCode);
                break;
                case FORCE_SINGLE_COIL:
                    OutputLen = ForceSingeCoil(nRegister,nCount,pOutputBuffer,&nErrorCode);
                break;
                case FORCE_MULTIPLE_COILS:
                    OutputLen = ForceMultipleCoils(nRegister,nCount,pOutputBuffer,pInputData[FC_COILS_OFFSET],&nErrorCode);
                break;
                default:
                    nErrorCode=ILLEGAL_FUNCTION;
            }
            if (NO_ERRORS!=nErrorCode)
            {
                pOutputBuffer[FUNC_OFFSET]|=ERROR_FLAG;
                pOutputBuffer[DATA_OFFSET]=nErrorCode;
                OutputLen=HEADER_LEN;
            }
            pCRC = (uint16_t*)(pOutputBuffer+OutputLen);
            *pCRC = htons(CRC16(pOutputBuffer,OutputLen));
            OutputLen+=sizeof(uint16_t);
        }
    }
    return OutputLen;
}

static void OnModbusReceiveByte(uint8_t nData)
{
    if (stUartData.nBytesCount<UART_BUFFER_SIZE)
    {
        stUartData.InputBuffer[stUartData.nBytesCount++]=nData;
    }
    ModbusTimerStart();
}

void ModbusInit()
{
    memset(&stUartData,0,sizeof(stUartData));
    memset(&stModbusData,0,sizeof(stModbusData));
    
    ModbusTimerInit();
    UartInit();
    SetUartIsrHandler(&OnModbusReceiveByte);
    UartStart();
}

void SetInputRegister(int nRegister, uint16_t nValue)
{
    if (nRegister<MODBUS_INPUTREG_COUNT)
    {
        stModbusData.nInputRegisters[nRegister]=nValue;
    }
}

void SetDiscreteInput(int nInput, uint8_t nValue)
{
    if (nInput<MODBUS_INPUTREG_COUNT)
    {
        stModbusData.nInputs[nInput]=nValue;
    }
}

uint8_t GetCoilValue(int nCoil)
{
    uint8_t Result=0;
    if (nCoil<MODBUS_COILS_COUNT)
    {
        Result=stModbusData.nCoils[nCoil];
    }
    return Result;
}

void ModbusHandler()
{
    uint8_t pOutputBuffer[OUTPUT_BUFFER_SIZE];
    int OutputSize = DecodeModbusData(stUartData.InputBuffer, stUartData.nBytesCount, pOutputBuffer);
    stUartData.nBytesCount=0;
    UartStart();
    UartWrite(pOutputBuffer, OutputSize);
}