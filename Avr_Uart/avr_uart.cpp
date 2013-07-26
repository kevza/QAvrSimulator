#include "avr_uart.h"
#include <QApplication>

Avr_Uart::Avr_Uart(){
        readComplete = false;
        isOpen = false;
        writeComplete = false;
        this->lastUDR = 0;
         UDR = NULL;
        UCSRA = NULL;
        UCSRB = NULL;
        UCSRC = NULL;
        UCSRD = NULL;
        UBRRL = NULL;
        UBRRH = NULL;

        oldUCSRB = 0;
        oldUCSRC = 0;

}
Avr_Uart::~Avr_Uart(){
    serial.closeSerial();
}

QString Avr_Uart::getPluginName(){
    return "AVRUART";
}

/**
 * @brief Avr_Uart::getRegisterCount
 * @return The number of registers to set up
 */
int Avr_Uart::getRegisterCount(){
    return 7;
}

/**
 * @brief Avr_Uart::getRegisters
 * @return a QList of Register Names
 */
QStringList Avr_Uart::getRegisters(){
    return QStringList()    << "UDR" << "UCSRA"
                            << "UCSRB" << "UCSRC"
                            << "UCSRD" << "UBRRL"
                            << "UBRRH";
}

/**
 * @brief Avr_Uart::getInterruptCount
 * @return The number of Interrupts
 */
int Avr_Uart::getInterruptCount(){
    return 0;
}

/**
 * @brief Avr_Uart::getInterrupts
 * @return A QList of interrupt names
 */
QStringList Avr_Uart::getInterrupts(){
    return QStringList() << "nothing to see here";
}

/**
 * @brief Avr_Uart::bindRegister Binds a memory location to a register
 * @param n The Number of the Register
 * @param ptr The memory location to point to
 */
void Avr_Uart::bindRegister(QString reg, uint8_t *ptr){
    qDebug() << "Bind Register : " << reg;
    if (reg == "UDR"){
        UDR = ptr;
        this->lastUDR = *UDR;
    }
    if (reg == "UCSRA"){
        UCSRA = ptr;
        //Buffer Ready for Transmission
        *UCSRA = 0;
    }
    if (reg == "UCSRB"){
        UCSRB = ptr;
        oldUCSRB = 0;
        *UCSRB = 0;
    }
    if (reg == "UCSRC"){
        UCSRC = ptr;
        oldUCSRC = 0;
        *UCSRC = 0;
    }
    if (reg == "UCSRD"){
        UCSRD = ptr;
        *UCSRD = 0;
    }
    if (reg == "UBRRL"){
        UBRRL = ptr;
        *UBRRL = 0;
    }
    if (reg == "UBRRH"){
        UBRRH = ptr;
        *UBRRH = 0;
    }

}

/**
 * @brief Avr_Uart::update Runs an update cycle on the hard ware
 * @param cycles The number of cycles since the last update
 * @return Any relevant Interrupt Vector
 */
int Avr_Uart::update(int cycles){
    //Checks if a UART is Open and
    //ready to use
    if (!isOpen || oldUCSRB != *UCSRB || oldUCSRC != *UCSRC){
        //Check Reason for Being here
       if (oldUCSRB == *UCSRB && oldUCSRC == *UCSRC){
            return -1;
       }
       //Close old serial if state has changed
       if (isOpen){
           qDebug() << "Closing Serial Port";
            serial.closeSerial();            
       }
       if (!openUart())
           return -1;
       oldUCSRB = *UCSRB;
       oldUCSRC = *UCSRC;
   }

   //Checks if something was written to the UDR and
   //Needs to be sent.
   if ((*UCSRB) & (1 << 3) && reg->lastWrite == 0xce){
        //There is something to transmit
        qDebug() << "Transmit " << (int)*UDR;
        serial.writeSerial(*UDR);
        *UDR = lastUDR;
        //Set Bytes transmitted and
        //buffer is clear.
        *UCSRA |= (1 << 6);

        //Clear Last Write
        reg->lastWrite = 0;
        //flag Transmission complete
   }else{
       *UCSRA |= (1 << 5);
   }

    //Check if a Read is ready
   if ((*UCSRB) & (1 << 4)){
        serial.readSerial();
        //Check if data has been read
        //increment buffer if it has
        if (reg->lastRead == 0xce){
            serial.buffer.read = (serial.buffer.read + 1) % 2;
            *UCSRA &= ~(1 << 7);
            reg->lastRead = 0;
        }
        //Load new data from the buffer.
        if (serial.buffer.cleared[serial.buffer.read]){
            *UDR = serial.buffer.buffer[serial.buffer.read];
            lastUDR = *UDR;
            serial.buffer.cleared[serial.buffer.read] = 0;
            *UCSRA |= (1 << 7);
        }
   }

    if (serial.buffer.overrun){
        //Set Overflow flag
        *UCSRA |= (1 << 3);  //
        serial.buffer.overrun = 0;
    }
    return -1;
}

void Avr_Uart::passSetting(QMap<QString, QString> setting){
    isOpen = false;
    if (setting.contains("PORT")){
        port = setting["PORT"];
    }
    if (setting.contains("BAUD")){
        baud = setting ["BAUD"];
    }
    //setup hardware Echo
    if (setting.contains("HECHO")){
        if (setting["HECHO"]=="True"){
            serial.hEcho = true;
        }else{
            serial.hEcho = false;
        }
    }
    //setup software Echo
    if (setting.contains("SECHO")){
        if (setting["SECHO"]=="True"){
            serial.sEcho = true;
        }else{
            serial.sEcho = false;
        }
    }
}

bool Avr_Uart::openUart(){
    //Check we have a port and a rate
    if (!port.isEmpty() && !baud.isEmpty()){
        //Get Data Size
        //UCSZn2

        if ((*UCSRB) & (1 << 2)){
            if ((*UCSRC) & (1 << 1) && (*UCSRC) & (1 << 2) ){
                //9 Bit Transmission -- Not used
                return false;
            }
            return false;
        }else{
            qDebug() << "UCSRC STATE " << (int)*UCSRC;
            switch (((*UCSRC)>>1) & 0x3){
                case 0:
                    //5 bit
                    serial.setDataBits(5);
                break;
                case 1:
                    //6 bit
                    serial.setDataBits(6);
                break;
                case 2:
                    //7Bit
                    serial.setDataBits(7);
                break;
                case 3:
                    //8 Bit
                    serial.setDataBits(8);
                break;
            }
        }
        //Stop Bits
        if ((*UCSRC) & (1 << 3)){
            serial.setStopBits(2);
        }else{
            serial.setStopBits(1);
        }



        //Get Parity
        switch (((*UCSRC) &(3 << 4))>> 4){
            case 0:
                serial.setParity(NO_PARITY);
            break;
            case 1:
                qDebug() << "Reserved";
            break;
            case 2:
                serial.setParity(EVEN_PARITY);
            break;
            case 3:
                serial.setParity(ODD_PARITY);
            break;
        }
        //Set the Baud Rate

        if (this->baud == "2400"){
                #ifdef __unix
                    serial.setBaudRate(B2400);
                #endif
                #ifdef __WIN32
                    serial.setBaudRate(2400); 
                #endif
         }

        if (this->baud == "4800"){
                #ifdef __unix
                serial.setBaudRate(B4800);
                #endif
                 #ifdef __WIN32
                    serial.setBaudRate(4800);
                #endif

        }

        if (this->baud == "9600"){
                #ifdef __unix
                serial.setBaudRate(B9600);
                #endif
                 #ifdef __WIN32
                    serial.setBaudRate(9600);
                #endif


        }

        if (this->baud == "19200"){
                #ifdef __unix
                serial.setBaudRate(B19200);
                #endif
                 #ifdef __WIN32
                    serial.setBaudRate(19200);
                #endif

        }

        if (this->baud == "38400"){
                #ifdef __unix
                serial.setBaudRate(B38400);
                #endif
                 #ifdef __WIN32
                    serial.setBaudRate(38400);
                #endif

        }

        if (this->baud == "57600"){
                #ifdef __unix
                serial.setBaudRate(B57600);
                #endif
                 #ifdef __WIN32
                    serial.setBaudRate(57600);
                #endif

        }

        if (this->baud == "115200"){
                #ifdef __unix
                serial.setBaudRate(B115200);
                #endif
                 #ifdef __WIN32
                    serial.setBaudRate(115200);
                #endif

        }
        serial.setPort(port);

        if (serial.openSerial()> 0){
            qDebug() << "Serial Port is Open";
            isOpen = true;
            return true;
        }else{
            qDebug() << "Something went wrong" ;
            exit(1);
        }

    }else{
        return false;
    }
    return false;
}

Q_EXPORT_PLUGIN2(avr_uart ,Avr_Uart)
