﻿#include <avr/interrupt.h>
#include "Globals.h"
#include "RfReceiver.h"
#include "Timers.h"

#define PULSE_LEN           600         //длительность стартовых импульсов 700 мкс
#define HALFBIT_LEN         400         //длительность передачи бита 700 мкс
#define START_PULSE_COUNT   4            //перед 40 битами данных идут 4 стартовых импульса

#define BUFFER_SIZE 8
#define PACKET_SIZE 40
#define CRC_POS     4

//перечень состояний конечного автомата
enum {				
    STATE_PAUSE,
    STATE_PULSE,
    STATE_WAIT_PAUSE,
    STATE_WAIT_PULSE,
    STATE_WAIT_START,
    STATE_BIT,
    STATE_WAIT_BIT
    };

static volatile uint8_t RfBuffer[BUFFER_SIZE];	//буфер принятых данных
static volatile int nState=0;					//текущее состояние конечного автомата
static volatile int nBitCount=0;				//счетчик принятых бит

static void Reset()
{
    RfTimerStop();
    nState=STATE_WAIT_PULSE;
    nBitCount=0;
}

static void StartReceive()
{
    //разрешение внешних прерываний
    EIFR |= (1<<INTF0);
    EIMSK |= (1<<INT0);
}

static void StopReceive()
{
    //запрет внешних прерываний
    EIMSK &= ~(1<<INT0);
    EIFR |= (1<<INTF0);
}

static uint8_t GetCRC()
{
    uint8_t result=0;
    for (int i=0;i<CRC_POS;++i) {
        result+=RfBuffer[i];
    }
    return result;
}

//декодирование принятых по радиоканалу данных
int DecodeSensorData(stSensorData_t* pSensorData)
{
    //  BALDR sensor data
    //  | ID | BATTERY| BUTTON | CHANNEL | TEMPERATURE | HUMIDITY | CRC |
    //  |  8 |    1   |    1   |  3bits  |    12bits   |  8bits   |  8  |

    int result=0;
    if (GetCRC()==RfBuffer[CRC_POS]) {
        pSensorData->nId = RfBuffer[0];
        pSensorData->bBattery = (RfBuffer[1]>>7)&0x01;
        pSensorData->nChannel = (RfBuffer[1]>>4)&0x03;
        int16_t Farengate = RfBuffer[1]&0x0F;
        Farengate=(Farengate<<8)|RfBuffer[2];
        pSensorData->nTemperature=(Farengate-320)*5/9-500;
        pSensorData->nHumidity = RfBuffer[3];
        
        result=1;
    }
    StartReceive();
    return result;
}

void ReceiverInit()
{
    RfTimerInit();
    Reset();
    EICRA = (1<<ISC00); //Any logical change on INT0 generates an interrupt request.
    StartReceive();
}

static void Send()
{
    g_nStatus |= EVENT_RF;
    Reset();
}

static void Put(int nValue)
{
  int ByteIndex=nBitCount/8;
  RfBuffer[ByteIndex]<<=1;
  RfBuffer[ByteIndex]|=nValue;
}

ISR(INT0_vect)
{
    // вход внешнего прерывания INT0 - пин D2 порта PORTD
    // по прерыванию анализируем логический уровень
    if((PIND & (1<<PIND2))) {
        switch(nState) {
        case STATE_WAIT_PULSE:
            //1. при старте ждем 4 импульса по 700 мкс. Запускаем таймер на 600 мкс для проверки минимальной длительности
            RfTimerStart(PULSE_LEN);
            nBitCount++;            //пока нет приема данных, считаем стартовые импульсы
            nState=STATE_PULSE;
            break;
        case STATE_WAIT_BIT:
            //9. завершение передачи бита
            nBitCount++;
            if (PACKET_SIZE==nBitCount) {
                //по последнему биту в пакете формируем сигнал для декодера
                Send();
                StopReceive();
                break;
            }
            //если бит не последний - переходим к приему следующего
        case STATE_WAIT_START:
            //5. запускаем таймер на половинную длительность бита. По уровню на момент срабатывания смотрим значение
            RfTimerStart(HALFBIT_LEN);
            nState=STATE_BIT;
            break;
        default:
            //если изменение уровня произошло вне ожидаемого состояния, сбрасываем состояние автомата
            RfTimerStart(PULSE_LEN);
            nBitCount=1;
            nState=STATE_PULSE;
        }
    }
    else {
        switch (nState) {
        case STATE_WAIT_PAUSE:
            //3. проверяем минимальную длительность паузы между стартовыми импульсами
            RfTimerStart(PULSE_LEN);
            nState=STATE_PAUSE;
            break;
        case STATE_BIT:
            //7. переход в 0 в первую половину периода передачи бита (короткий импульс) - передается 0
            Put(0);
            break;
        case STATE_WAIT_BIT:
            //8. переход в 0 во вторую половину периода передачи бита (длинный импульс) - передается 1
            Put(1);
            break;
        case STATE_WAIT_PULSE:
            break;
        default:
            //если изменение уровня произошло вне ожидаемого состояния, сбрасываем состояние автомата
            Reset();
        }
    }    
}

ISR(TIMER2_COMPA_vect)
{
    RfTimerStop();
    switch(nState) {
    case STATE_PULSE:
        //2. теперь проверяем максимальную длительность - сигнал должен упасть в интервале 400 мкс
        nState=STATE_WAIT_PAUSE;
        RfTimerStart(HALFBIT_LEN);
        break;
    case STATE_PAUSE:
        //4. проверяем максимальную длительность паузы - включаем таймер. В интервале 400 мкс должен быть импульс
        RfTimerStart(HALFBIT_LEN);
        if (nBitCount<START_PULSE_COUNT) {
            nState=STATE_WAIT_PULSE;
        }
        else {  //если уже принято 4 импульса - стартовая последовательность закончена, следующий импульс будет началом передачи данных
            nState=STATE_WAIT_START;
            nBitCount=0;
        }
        break;
    case STATE_BIT:
        //6. запускаем ожидание следующего бита
        RfTimerStart(HALFBIT_LEN);
        nState=STATE_WAIT_BIT;
        break;
    default:
        //если таймер сработал вне ожидаемого состояния - сбрасываем состояние автомата
        Reset();
    }
}

