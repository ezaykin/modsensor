#include <avr/interrupt.h>
#include "Globals.h"
#include "twi.h"

/****************************************************************************
  ��������� ���� TWI ������ 
****************************************************************************/

/*����� ��������� ���� */                    
#define TWI_START                  0x08  // ��������� START ������������ 
#define TWI_REP_START              0x10  // ��������� ��������� START ������������ 
#define TWI_ARB_LOST               0x38  // ��� ������� ��������� 

/*��������� ���� �������� �����������*/                
#define TWI_MTX_ADR_ACK            0x18  // ��� ������� ����� SLA+W � �������� �������������
#define TWI_MTX_ADR_NACK           0x20  // ��� ������� ���� SLA+W � �� �������� �������������
#define TWI_MTX_DATA_ACK           0x28  // ��� ������� ���� ������ � �������� �������������  
#define TWI_MTX_DATA_NACK          0x30  // ��� ������� ���� ������ � �� �������� �������������

/*��������� ���� �������� ���������*/ 
#define TWI_MRX_ADR_ACK            0x40  // ��� ������� ����� SLA+R � �������� ������������ 
#define TWI_MRX_ADR_NACK           0x48  // ��� ������� ����� SLA+R � �� �������� ������������� 
#define TWI_MRX_DATA_ACK           0x50  // ���� ������ ������ � �������� �������������  
#define TWI_MRX_DATA_NACK          0x58  // ��� ������ ���� ������ ��� �������������  

/*������ ��������� ����*/
#define TWI_BUS_ERROR              0x00  // ������ �� ���� ��-�� ����������� ��������� ����� ��� ����

/****************************************************************************
  ����������� ��������
****************************************************************************/
#define TWI_READ_BIT     0       // ������� R/W ���� � �������� ������
#define TWI_ADR_BITS     1       // ������� ������ � �������� ������
#define TWSR_MASK     0xfc  

volatile static uint8_t* pTwiBuf;
volatile static uint8_t twiMsgSize=0;

volatile static uint8_t ReadingStarted=0;
volatile static uint8_t ReadSize=0;
volatile static uint8_t RegBuf[2];
volatile static uint8_t* pReadBuf;


uint8_t TWI_MasterInit(uint16_t fr)
{
    /*������������ ��� ��������� �������� ������ twi ������*/
    const uint8_t PRE[4] = {2, 8, 32, 128};
    uint8_t i;
    uint16_t twbrValue;
    
    for(i = 0; i<4; i++){
        twbrValue = ((((F_CPU)/1000UL)/fr)-16)/PRE[i];
        if ((twbrValue > 0)&& (twbrValue < 256)){
            TWBR = (uint8_t)twbrValue;
            TWSR = i;
            TWDR = 0xFF;
            TWCR = (1<<TWEN);
            return 1;
        }
    }
    return 0;
}

void TwiStartWrite(void* pData, uint8_t Size)
{
    ReadingStarted=0;
    pTwiBuf=pData;
    pTwiBuf[0]&=~(0x01);
    twiMsgSize = Size;
    TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWSTA); //��������� ���������� � ��������� ��������� �����
}

void TwiStartRead(void* pData, uint8_t Register, uint8_t Size)
{
    pReadBuf=pData;					//��������� ��������� �� ����� ��� ������ � ������
    ReadSize=Size;
    ReadingStarted=1;
    RegBuf[0]=(((uint8_t*)pData)[0]&~(0x01));	//������� ����� ����� ���������� � ��������� �������
    RegBuf[1]=Register;
    pTwiBuf=RegBuf;
    twiMsgSize = 2;
    TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWSTA); //��������� ���������� � ��������� ��������� �����
}

ISR(TWI_vect)
{
  static uint8_t ptr;
  uint8_t stat = TWSR & TWSR_MASK;
  
  switch (stat){
    
    case TWI_START:                   // ��������� START ������������ 
    case TWI_REP_START:               // ��������� ��������� START ������������        
       ptr = 0;      

    case TWI_MTX_ADR_ACK:             // ��� ������� ����� SLA+W � �������� �������������
    case TWI_MTX_DATA_ACK:            // ��� ������� ���� ������ � �������� �������������  
       if (ptr < twiMsgSize){
          TWDR = pTwiBuf[ptr];                    //��������� � ������� ������ ��������� ����
          TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT); //���������� ���� TWINT    
          ptr++;
       }
       else{
          if (ReadingStarted)
          {
            ReadingStarted=0;		//���� ����������� �������� ������, �� �������� ���������� ������ �� ���������� � �����
            pTwiBuf=pReadBuf;
            pTwiBuf[0]|=0x01;       //������������� ��� ������
            twiMsgSize=ReadSize;    //��� ������ �� ���������� ��������� ����� ������� �� 1 �� ������ ���������
            ptr=1;
            TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWSTA); //��������� ���������� � ��������� ��������� �����
          }
          else                      //��� ��������� �������� ������
          {
            TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO)|(0<<TWIE); //��������� ��������� ����, ���������� ����, ��������� ����������
            g_nStatus |= EVENT_TWI;
          }
       }
       break;
     
    case TWI_MRX_DATA_ACK:          //���� ������ ������ � �������� �������������  
       pTwiBuf[ptr] = TWDR;
       ptr++;
    case TWI_MRX_ADR_ACK:           //��� ������� ����� SLA+R � �������� ������������  
      if (ptr < (twiMsgSize-1)){
        TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA);  //���� ��� �� ������������� �������� ����, ��������� �������������                             
      }
      else {
        TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT);            //���� ������� ������������� ����, ������������� �� ���������
      }    
      break; 
      
    case TWI_MRX_DATA_NACK:       //��� ������ ���� ������ ��� �������������      
      pTwiBuf[ptr] = TWDR;
      TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO); //��������� ��������� ����
      g_nStatus |= EVENT_TWI;
      break; 
     
    case TWI_ARB_LOST:          //��� ������� ��������� 
      TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWSTA); // ���������� ���� TWINT, ��������� ��������� �����
      break;
      
    case TWI_MTX_ADR_NACK:      // ��� ������� ���� SLA+W � �� �������� �������������
    case TWI_MRX_ADR_NACK:      // ��� ������� ����� SLA+R � �� �������� �������������    
    case TWI_MTX_DATA_NACK:     // ��� ������� ���� ������ � �� �������� �������������
    case TWI_BUS_ERROR:         // ������ �� ���� ��-�� ����������� ��������� ����� ��� ����
    default:     
      TWCR = (1<<TWEN)|(0<<TWIE)|(0<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC); //��������� ����������     
  }
}
