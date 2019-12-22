#include "Globals.h"
#include "RfReceiver.h"
#include "HAL/Timers.h"
#include "HAL/PIO.h"
#include <string.h>


static const int PULSE_LEN   = 600;         //длительность стартовых импульсов 700 мкс
static const int HALFBIT_LEN = 400;         //длительность передачи бита 700 мкс
static const int START_PULSE_COUNT = 4;     //перед 40 битами данных идут 4 стартовых импульса
static const int CRC_POS     = 4;
static const int PACKET_SIZE = 40;

#define BUFFER_SIZE 8
#define MAX_PACKET_COUNT 20

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

static uint8_t RfBuffer[MAX_PACKET_COUNT][BUFFER_SIZE]; //буфер принятых данных
static uint8_t unState = 0;                 //текущее состояние конечного автомата
static uint8_t unBitCount = 0;              //счетчик принятых бит в принимаемом пакете
static uint8_t unPacketCount = 0;           //счетчик принятых пакетов
static uint8_t unPacketIndex = 0;           //индекс принимаемого пакета

static void Reset()
{
    TimerRf_Stop();
    unState = STATE_WAIT_PULSE;
    unBitCount = 0;
}

static uint8_t GetCRC(uint8_t unIndex)
{
    uint8_t result = 0;
    for (int i = 0; i < CRC_POS; ++i) {
        result += RfBuffer[unIndex][i];
    }
    return result;
}

static void IncPacketIndex()
{
    unPacketIndex++;
    if (unPacketIndex >= MAX_PACKET_COUNT) {
        unPacketIndex = 0;
    }
    if (unPacketCount < MAX_PACKET_COUNT) {
        unPacketCount++;
    }
}

static inline void Send()
{
    g_nStatus |= EVENT_RF;
    Reset();
}


static void OnRfTimer()
{
    TimerRf_Stop();

    switch (unState) {
    case STATE_PULSE:
        //2. теперь проверяем максимальную длительность - сигнал должен упасть в интервале 400 мкс
        unState = STATE_WAIT_PAUSE;
        TimerRf_Start(HALFBIT_LEN);
        break;

    case STATE_PAUSE:
        //4. проверяем максимальную длительность паузы - включаем таймер. В интервале 400 мкс должен быть импульс
        TimerRf_Start(HALFBIT_LEN);
        if (unBitCount < START_PULSE_COUNT) {
            unState = STATE_WAIT_PULSE;
        }
        else {  //если уже принято 4 импульса - стартовая последовательность закончена, следующий импульс будет началом передачи данных
            unState = STATE_WAIT_START;
            unBitCount = 0;
        }
        break;

    case STATE_BIT:
        //6. запускаем ожидание следующего бита
        TimerRf_Start(HALFBIT_LEN);
        unState = STATE_WAIT_BIT;
        break;

    default:
        //если таймер сработал вне ожидаемого состояния - сбрасываем состояние автомата
        Reset();
        //через 700 мкс после приема последнего пакета сработает таймаут ожидания стартового импульса.
        //Если к этому моменту есть данные - отправляем
        if (unPacketCount > 0) {
            ExternalInt_Disable();
            Send();
        }
    }
}

static void Put(int nValue)
{
    uint8_t ByteIndex = unBitCount / 8;
    RfBuffer[unPacketIndex][ByteIndex] <<= 1;
    RfBuffer[unPacketIndex][ByteIndex] |= nValue;
}

static inline void OnExternalInt(uint8_t nLevel)
{
    // вход внешнего прерывания INT0 - пин D2 порта PORTD
    // по прерыванию анализируем логический уровень
    if (nLevel) {
        switch (unState) {
        case STATE_WAIT_PULSE:
            //1. при старте ждем 4 импульса по 700 мкс. Запускаем таймер на 600 мкс для проверки минимальной длительности
            TimerRf_Start(PULSE_LEN);
            unBitCount++;            //пока нет приема данных, считаем стартовые импульсы
            unState = STATE_PULSE;
            break;

        case STATE_WAIT_BIT:
            //9. завершение передачи бита
            unBitCount++;

        case STATE_WAIT_START:
            //5. запускаем таймер на половинную длительность бита. По уровню на момент срабатывания смотрим значение
            TimerRf_Start(HALFBIT_LEN);
            //если бит последний - переходим к приему следующего пакета
            if (PACKET_SIZE == unBitCount) {
                unBitCount = 0;
                IncPacketIndex();
                //10. проверяем длительность последнего импульса. Если все нормально - подготавливаем ожидание паузы.
                unState = STATE_WAIT_PAUSE;
                break;
            }
            unState = STATE_BIT;
            break;

        default:
            //если изменение уровня произошло вне ожидаемого состояния, сбрасываем состояние автомата
            TimerRf_Start(PULSE_LEN);
            unBitCount = 1;
            unState = STATE_PULSE;
        }
    }
    else {
        switch (unState) {
        case STATE_WAIT_PAUSE:
            //3. проверяем минимальную длительность паузы между стартовыми импульсами
            TimerRf_Start(PULSE_LEN);
            unState = STATE_PAUSE;
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

void ReceiverInit()
{
    TimerRf_Init(&OnRfTimer);
    Reset();
    ExternalInt_Init(&OnExternalInt);
    ExternalInt_Enable();
}

//декодирование принятых по радиоканалу данных
int DecodeSensorData(stSensorData_t* pSensorData)
{
    //  BALDR sensor data
    //  | ID | BATTERY| BUTTON | CHANNEL | TEMPERATURE | HUMIDITY | CRC |
    //  |  8 |    1   |    1   |  2bits  |    12bits   |  8bits   |  8  |

    int result = 0;
    //фильтруем пакеты с неверной CRC
    for (uint8_t i = 0; i < unPacketCount; ++i) {
        //если CRC неправильное, выкидываем пакет из массива, сдвигая остальные данные
        if (GetCRC(i) != RfBuffer[i][CRC_POS]) {
            unPacketCount--;

            if (unPacketCount > 0) {
                memcpy(RfBuffer[i], RfBuffer[i + 1], BUFFER_SIZE * (unPacketCount - i));
            }
        }
    }
    //считаем количество одинаковых пакетов
    uint8_t MaxCount = 0;
    uint8_t CmpCount = 0;
    for (uint8_t i = 0; i < unPacketCount - 1; ++i) {
        CmpCount = 0;
        for (uint8_t j = i+1; j < unPacketCount; ++j) {
            if (!memcmp(RfBuffer[i], RfBuffer[j], BUFFER_SIZE)) {
                CmpCount++;
            }
        }
        //запоминаем индекс наибольшего количества одинаковых элементов
        if (CmpCount > MaxCount) {
            MaxCount = CmpCount;
            unPacketIndex = i;
        }
        //если количество совпадений текущего элемента совпадает с оставшимся количеством элемента в массиве, 
        //значит последующие элементы одинаковые, проверять дальше нет смысла
        if (CmpCount == (unPacketCount-i-1)) {
            break;
        }
    }

    if (MaxCount > 1)  {
        pSensorData->nId = RfBuffer[unPacketIndex][0];
        pSensorData->bBattery = (RfBuffer[unPacketIndex][1] >> 7) & 0x01;
        pSensorData->unChannel = (RfBuffer[unPacketIndex][1] >> 4) & 0x03;
        int16_t Farengate = RfBuffer[unPacketIndex][1] & 0x0F;
        Farengate = (Farengate << 8) | RfBuffer[unPacketIndex][2];
        pSensorData->nTemperature = (Farengate - 320) * 5 / 9 - 500;
        pSensorData->nHumidity = RfBuffer[unPacketIndex][3];
        result = 1;
    }

    unPacketIndex = 0;
    unPacketCount = 0;
    memset(RfBuffer, 0, BUFFER_SIZE * MAX_PACKET_COUNT);
    ExternalInt_Enable();
    return result;
}
