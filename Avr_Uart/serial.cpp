#include "serial.h"
#include <stdlib.h>


//This file reaks of platform dependent code and it desperately needs fixing
//Signal Handler
bool readyRead;
#ifdef __unix
void signal_handler_IO(int mask){ 
    readyRead = true;
}
#endif
#ifdef __WIN32
bool abContinue = true;
unsigned char winReceived;
HANDLE hCommPort;
//Keep Checking for Data
DWORD WINAPI winWatcher(void *arg){
    qDebug() << "Thread Active";
        while (abContinue){
                if (!readyRead){
                    DWORD dwBytesRead = 0;
                    ReadFile(hCommPort, &winReceived, 1, &dwBytesRead, NULL);
                    int r = dwBytesRead;
                    if (r > 0){
                        qDebug() << "Byte Recieved set readyread flag";
                       readyRead = true;
                    }
                }
                Sleep(100);
        }
    return 0;
}


#endif



Serial::Serial()
{
    data = 'd';
    readyRead = false;
    buffer.read = 0;
    buffer.write = 0;
    buffer.overrun = 0;
    buffer.cleared[0] =0;
    buffer.cleared[1] = 0;
    buffer.buffer[0] = 0;
    buffer.buffer[1] = 0;
}

Serial::~Serial(){
    this->closeSerial();
}

void Serial::setDataBits(int n){
        qDebug() << "Setting Data Bits to : " << n;
        this->dataBits =n;
}
void Serial::setParity(int n){
    this->parity = n;
}
void Serial::setStopBits(int n){
    this->stopBits = n;
}
void Serial::setBaudRate(unsigned int speed){
    this->baud = speed;

}
void Serial::setPort(QString p){
    this->port = p;
}


bool Serial::openSerial(){
    //Convert port using super user permissions
    //android
    qDebug() << "Set serial port to " << this->port;
    char permissionsChange[500];
    qDebug() << sprintf(permissionsChange,"su -c \"chmod 666 %s\"", this->port.toStdString().c_str());
    //qDebug() << QString(permissionsChange);
    if (system(permissionsChange)){
        qDebug() << "Permissions change failed";
        return false;
    }

    qDebug() << "Finished permissions updates";

//Only build this part for unix/linux systems
#ifdef __unix
    tty_fd = open(this->port.toStdString().c_str(), O_RDWR |O_NOCTTY | O_NONBLOCK);
    if (tty_fd == -1){
        qDebug() << "Error Modem Device";
        return false;
        //exit(1);
    }
    //Set up callback
    saio.sa_handler = signal_handler_IO;
    sigemptyset(&saio.sa_mask);
    sigaddset(&saio.sa_mask, SIGINT);
    saio.sa_flags = 0;
    saio.sa_restorer = NULL;
    sigaction(SIGIO,&saio,NULL);

    /* allow the process to receive SIGIO */
    fcntl(tty_fd, F_SETOWN, getpid());
    /* Make the file descriptor asynchronous (the manual page says only
    O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
    fcntl(tty_fd, F_SETFL, FASYNC);

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking

    memset(&tio,0,sizeof(tio));
    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_lflag=0;
    //Set Character Size
    switch (this->dataBits){
        case 5:
            tio.c_cflag |= CS5;
            qDebug() << "Set 5 Bit";
        break;
        case 6:
            tio.c_cflag |= CS6;
            qDebug() << "Set 6 Bit";
        break;
        case 7:
            tio.c_cflag |= CS7;
            qDebug() << "Set 7 Bit";
        break;
        case 8:
            tio.c_cflag |= CS8;
            qDebug() << "Set 8 Bit";
        break;
    }
    //Set Stop Bits
    if (this->stopBits == 2){
        tio.c_cflag |= CSTOPB;
        qDebug() << "Set 2 Stop Bits";
    }

    if (this->parity == EVEN_PARITY){
        tio.c_cflag |= PARENB;
        qDebug() << "Set Even Parity";
    }
    if (this->parity == ODD_PARITY){
        tio.c_cflag |= PARENB;
        tio.c_cflag |= PARODD;
        qDebug() << "Set Odd Parity";
    }
    tio.c_cflag |= CREAD|CLOCAL;   //Set Read and Ignore Control Lines

    tio.c_cc[VMIN]=0;
    tio.c_cc[VTIME]=0;
    cfsetospeed(&tio,this->baud);            // 115200 baud
    cfsetispeed(&tio,this->baud);            // 115200 baud
    //tcflush(tty_fd, TCIFLUSH);
    tcsetattr(tty_fd,TCSANOW,&tio);
#endif
#ifdef __WIN32
    //Create the windows serial file handle
    wchar_t wtext[20];
    mbstowcs(wtext, this->port.toStdString().c_str(), strlen(this->port.toStdString().c_str())+1);//Plus null
    LPWSTR gszPort = wtext;
    tty_fd = CreateFile( gszPort,
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        0,
                        OPEN_EXISTING,
                        0,
                        0);
    if (tty_fd == INVALID_HANDLE_VALUE){
        qDebug() << this->port << " failed to open." ;
        return false;
    }else{
        qDebug() << this->port << " successfully opened.";
    }
    //Apply Settings
    DCB dcb;
    if (!GetCommState(tty_fd, &dcb)){
        //Cant setup serial
        return false;
    }
    //Setup Settings
    FillMemory(&dcb, sizeof(dcb), 0);
    dcb.DCBlength = sizeof(dcb);
    //Create Serial String
    //Build Parity Settings
    if (this->parity == EVEN_PARITY){
        dcb.Parity = 2;
    }else if (this->parity == ODD_PARITY){
        dcb.Parity = 1;
    }else{
        dcb.Parity = 0;
    }
    dcb.BaudRate = this->baud;
    dcb.ByteSize = this->dataBits;
    if (this->stopBits == 1){
        dcb.StopBits = 0;
    }else{
        dcb.StopBits = 2;
    }

    if (!SetCommState(tty_fd,&dcb)){
        qDebug() << "Set Comms state failed : " << GetLastError();
        return false;
    }

    qDebug() << "Baud Rate Set to : " << dcb.BaudRate;
    qDebug() << "Byte Size Set to : " << dcb.ByteSize;
    qDebug() << "Stop Bits Set to : " << dcb.StopBits;

    COMMTIMEOUTS timeouts={0};
    timeouts.ReadIntervalTimeout=1;
    timeouts.ReadTotalTimeoutConstant=1;
    timeouts.ReadTotalTimeoutMultiplier=1;
    timeouts.WriteTotalTimeoutConstant=5;
    timeouts.WriteTotalTimeoutMultiplier=1;
    if(!SetCommTimeouts(tty_fd, &timeouts)){
        //handle error
        return false;
    }


    DWORD dwThreadId;
   //Start Watcher Thread
   //Set handle
   hCommPort = tty_fd;
   //Thread to run
   abContinue = true;
   readThread =  CreateThread(NULL,0,winWatcher,(void*)&tty_fd,0,&dwThreadId);
   if (readThread == NULL){
       qDebug() << "Create thread failed";
       exit(2);
   }



    qDebug() << "Settings applied seial port is ready to go";
#endif

    return true;
   }

void Serial::closeSerial(){
#ifdef __unix
    close(tty_fd);
#endif
#ifdef __WIN32
    abContinue = false;
    CloseHandle(tty_fd);
    CloseHandle(readThread);
    qDebug() << "Close Handle";
#endif
}


int Serial::readSerial(){
    #ifdef __unix
    while (readyRead){
        int r = read(tty_fd,&data,1);
        if (r > 0){

    #endif
    #ifdef __WIN32
    if (readyRead){
       data = winReceived;
    #endif
            qDebug() << "Read " << data<< " to "<< buffer.write ;
            buffer.buffer[buffer.write] = data;
            //Check if said location is cleared
            if (buffer.cleared[buffer.write] == 1)
                buffer.overrun = 1;
            //Flag Written
            buffer.cleared[buffer.write] = 1;
            buffer.write = (buffer.write + 1) % 2;
            //Check Hardware Echo
            //Send signal back to real board if engaged
            if (this->hEcho == true){
                //Save software echo state
                bool oldSEcho = this->sEcho;
                this->sEcho = false;
                this->writeSerial(data);
                this->sEcho = oldSEcho;
            }
            #ifdef __WIN32
            readyRead = false;
            #endif
        #ifdef __unix
        }else{
            readyRead = false;
        }
        #endif
    }
    return -1;
}

void Serial::writeSerial(unsigned char c){
#ifdef __unix
    write(tty_fd, &c,1);
#endif
#ifdef __WIN32
    DWORD dwBytesRead = 0;
    //Do something better here
    qDebug() << "Writting to Serial";
    WriteFile(tty_fd, &c, 1, &dwBytesRead, NULL);
    qDebug() << "Write Complete";
#endif
    //If software echo add sent char to receive buffer
    if (this->sEcho == true){
        qDebug() << "Run Software Echo";
        //Simulate UCFK send behaviour
        //echo the send character
        //into the recieve buffer.
        buffer.buffer[buffer.write] = c;
        //Check if said location is cleared
        if (buffer.cleared[buffer.write] == 1)
        buffer.overrun = 1;
        //Flag Written
        buffer.cleared[buffer.write] = 1;
        buffer.write = (buffer.write + 1) % 2;
    }
}

