#include "../Globals.h"
#include "twi.h"
#include <avr/interrupt.h>

/****************************************************************************
  Статусные коды TWI модуля 
****************************************************************************/

/*Общие статусные коды */                    
#define TWI_START                  0x08  // состояние START сформировано 
#define TWI_REP_START              0x10  // состояние повторный START сформировано 
#define TWI_ARB_LOST               0x38  // был потерян приоритет 

/*Статусные коды ведущего передатчика*/                
#define TWI_MTX_ADR_ACK            0x18  // был передан пакет SLA+W и получено подтверждение
#define TWI_MTX_ADR_NACK           0x20  // был передан пает SLA+W и не получено подтверждение
#define TWI_MTX_DATA_ACK           0x28  // был передан байт данных и получено подтверждение  
#define TWI_MTX_DATA_NACK          0x30  // был передан байт данных и не получено подтверждение

/*Статусные коды ведущего приемника*/ 
#define TWI_MRX_ADR_ACK            0x40  // был передан пакет SLA+R и получено подтвеждение 
#define TWI_MRX_ADR_NACK           0x48  // был передан пакет SLA+R и не получено подтверждение 
#define TWI_MRX_DATA_ACK           0x50  // байт данных принят и передано подтверждение  
#define TWI_MRX_DATA_NACK          0x58  // был принят байт данных без подтверждения  

/*Другие статусные коды*/
#define TWI_BUS_ERROR              0x00  // ошибка на шине из-за некоректных состояний СТАРТ или СТОП

/****************************************************************************
  Определения констант
****************************************************************************/
#define TWI_READ_BIT     0       // позиция R/W бита в адресном пакете
#define TWI_ADR_BITS     1       // позиция адреса в адресном пакете
#define TWSR_MASK     0xfc  

volatile static uint8_t* pTwiBuf;
volatile static uint8_t twiMsgSize = 0;

volatile static uint8_t ReadingStarted = 0;
volatile static uint8_t ReadSize = 0;
volatile static uint8_t RegBuf[2];
volatile static uint8_t* pReadBuf;

static void (*pTwiIsrHandler)(void)=0;

uint8_t TWI_MasterInit(void (*IsrHandler)(), uint16_t fr)
{
    pTwiIsrHandler = IsrHandler;
    
    /*предделители для установки скорости обмена twi модуля*/
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
    TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWSTA); //разрешаем прерывание и формируем состояние старт
}

void TwiStartRead(void* pData, uint8_t Register, uint8_t Size)
{
    pReadBuf=pData;					//сохраняем указатель на буфер для чтения и размер
    ReadSize=Size;
    ReadingStarted=1;
    RegBuf[0]=(((uint8_t*)pData)[0]&~(0x01));	//сначала пишем адрес устройства и начальный регистр
    RegBuf[1]=Register;
    pTwiBuf=RegBuf;
    twiMsgSize = 2;
    TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWSTA); //разрешаем прерывание и формируем состояние старт
}

ISR(TWI_vect)
{
  static uint8_t ptr;
  uint8_t stat = TWSR & TWSR_MASK;
  
  switch (stat){
    
    case TWI_START:                   // состояние START сформировано 
    case TWI_REP_START:               // состояние повторный START сформировано        
       ptr = 0;      

    case TWI_MTX_ADR_ACK:             // был передан пакет SLA+W и получено подтверждение
    case TWI_MTX_DATA_ACK:            // был передан байт данных и получено подтверждение  
       if (ptr < twiMsgSize){
          TWDR = pTwiBuf[ptr];                    //загружаем в регистр данных следующий байт
          TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT); //сбрасываем флаг TWINT    
          ptr++;
       }
       else{
          if (ReadingStarted)
          {
            ReadingStarted = 0;		//если выполняется операция чтения, то начинаем вычитывать данные из устройства в буфер
            pTwiBuf = pReadBuf;
            pTwiBuf[0] |= 0x01;       //устанавливаем бит чтения
            twiMsgSize = ReadSize;    //при чтении из устройства указатель будет сдвинут на 1 от начала структуры
            ptr=1;
            TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWSTA); //разрешаем прерывание и формируем состояние старт
          }
          else                      //это окончание операции записи
          {
            TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO)|(0<<TWIE); //формируем состояние СТОП, сбрасываем флаг, запрещаем прерывания
            if(pTwiIsrHandler) {
                (*pTwiIsrHandler)();
            }
          }
       }
       break;
     
    case TWI_MRX_DATA_ACK:          //байт данных принят и передано подтверждение  
       pTwiBuf[ptr] = TWDR;
       ptr++;
    case TWI_MRX_ADR_ACK:           //был передан пакет SLA+R и получено подтвеждение  
      if (ptr < (twiMsgSize-1)){
        TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA);  //если это не предпоследний принятый байт, формируем подтверждение                             
      }
      else {
        TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT);            //если приняли предпоследний байт, подтверждение не формируем
      }    
      break; 
      
    case TWI_MRX_DATA_NACK:       //был принят байт данных без подтверждения      
      pTwiBuf[ptr] = TWDR;
      TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO); //формируем состояние стоп
      if (pTwiIsrHandler)
      {
          (*pTwiIsrHandler)();
      }
      break; 
     
    case TWI_ARB_LOST:          //был потерян приоритет 
      TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWSTA); // сбрасываем флаг TWINT, формируем повторный СТАРТ
      break;
      
    case TWI_MTX_ADR_NACK:      // был передан пает SLA+W и не получено подтверждение
    case TWI_MRX_ADR_NACK:      // был передан пакет SLA+R и не получено подтверждение    
    case TWI_MTX_DATA_NACK:     // был передан байт данных и не получено подтверждение
    case TWI_BUS_ERROR:         // ошибка на шине из-за некоректных состояний СТАРТ или СТОП
    default:     
      TWCR = (1<<TWEN)|(0<<TWIE)|(0<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC); //запретить прерывание     
  }
}
