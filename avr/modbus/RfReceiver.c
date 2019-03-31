#include <avr/interrupt.h>
#include "Globals.h"
#include "RfReceiver.h"
#include "Timers.h"

#define PULSE_LEN   600         //������������ ��������� ��������� 700 ���
#define HALFBIT_LEN 400         //������������ �������� ���� 700 ���
#define START_PULSE_COUNT 4     //����� 40 ������ ������ ���� 4 ��������� ��������

#define BUFFER_SIZE 8
#define PACKET_SIZE 40
#define CRC_POS 4

//�������� ��������� ��������� ��������
enum {				
    STATE_PAUSE,
    STATE_PULSE,
    STATE_WAIT_PAUSE,
    STATE_WAIT_PULSE,
    STATE_WAIT_START,
    STATE_BIT,
    STATE_WAIT_BIT
    };

static volatile uint8_t RfBuffer[BUFFER_SIZE];	//����� �������� ������
static volatile int nState=0;					//������� ��������� ��������� ��������
static volatile int nBitCount=0;				//������� �������� ���

static void Reset()
{
    RfTimerStop();
    nState=STATE_WAIT_PULSE;
    nBitCount=0;
}

static void StartReceive()
{
    //���������� ������� ����������
    EIFR |= (1<<INTF0);
    EIMSK |= (1<<INT0);
}

static void StopReceive()
{
    //������ ������� ����������
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

//������������� �������� �� ����������� ������
int DecodeSensorData(stSensorData_t* pSensorData)
{
    //  BALDR sensor data
    //  | ID | BAT | CHANNEL | TEMPERATURE | HUMIDITY | CRC |
    //  |  8 |  1  |  3bits  |    12bits   |  8bits   |  8  |

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
    // ���� �������� ���������� INT0 - ��� D2 ����� PORTD
    // �� ���������� ����������� ���������� �������
    if((PIND & (1<<PIND2))) {
        switch(nState) {
        case STATE_WAIT_PULSE:
            //1. ��� ������ ���� 4 �������� �� 700 ���. ��������� ������ �� 600 ��� ��� �������� ����������� ������������
            RfTimerStart(PULSE_LEN);
            nBitCount++;            //���� ��� ������ ������, ������� ��������� ��������
            nState=STATE_PULSE;
            break;
        case STATE_WAIT_BIT:
            //9. ���������� �������� ����
            nBitCount++;
            if (PACKET_SIZE==nBitCount) {
                //�� ���������� ���� � ������ ��������� ������ ��� ��������
                Send();
                StopReceive();
                break;
            }
            //���� ��� �� ��������� - ��������� � ������ ����������
        case STATE_WAIT_START:
            //5. ��������� ������ �� ���������� ������������ ����. �� ������ �� ������ ������������ ������� ��������
            RfTimerStart(HALFBIT_LEN);
            nState=STATE_BIT;
            break;
        default:
            //���� ��������� ������ ��������� ��� ���������� ���������, ���������� ��������� ��������
            RfTimerStart(PULSE_LEN);
            nBitCount=1;
            nState=STATE_PULSE;
        }
    }
    else {
        switch (nState) {
        case STATE_WAIT_PAUSE:
            //3. ��������� ����������� ������������ ����� ����� ���������� ����������
            RfTimerStart(PULSE_LEN);
            nState=STATE_PAUSE;
            break;
        case STATE_BIT:
            //7. ������� � 0 � ������ �������� ������� �������� ���� (�������� �������) - ���������� 0
            Put(0);
            break;
        case STATE_WAIT_BIT:
            //8. ������� � 0 �� ������ �������� ������� �������� ���� (������� �������) - ���������� 1
            Put(1);
            break;
        case STATE_WAIT_PULSE:
            break;
        default:
            //���� ��������� ������ ��������� ��� ���������� ���������, ���������� ��������� ��������
            Reset();
        }
    }    
}

ISR(TIMER2_COMPA_vect)
{
    RfTimerStop();
    switch(nState) {
    case STATE_PULSE:
        //2. ������ ��������� ������������ ������������ - ������ ������ ������ � ��������� 400 ���
        nState=STATE_WAIT_PAUSE;
        RfTimerStart(HALFBIT_LEN);
        break;
    case STATE_PAUSE:
        //4. ��������� ������������ ������������ ����� - �������� ������. � ��������� 400 ��� ������ ���� �������
        RfTimerStart(HALFBIT_LEN);
        if (nBitCount<START_PULSE_COUNT) {
            nState=STATE_WAIT_PULSE;
        }
        else {  //���� ��� ������� 4 �������� - ��������� ������������������ ���������, ��������� ������� ����� ������� �������� ������
            nState=STATE_WAIT_START;
            nBitCount=0;
        }
        break;
    case STATE_BIT:
        //6. ��������� �������� ���������� ����
        RfTimerStart(HALFBIT_LEN);
        nState=STATE_WAIT_BIT;
        break;
    default:
        //���� ������ �������� ��� ���������� ��������� - ���������� ��������� ��������
        Reset();
    }
}

