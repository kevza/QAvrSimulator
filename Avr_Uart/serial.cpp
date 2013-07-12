#include "serial.h"

//Signal Handler
bool readyRead;

void signal_handler_IO(int mask){ 
    readyRead = true;
}


Serial::Serial()
{
    data = 'd';
    readyRead = false;
    buffer.read = 0;
    buffer.write = 0;
    buffer.overrun = 0;
}

Serial::~Serial(){
    this->closeSerial();

}

void Serial::setDataBits(int n){
        this->dataBits =n;
}
void Serial::setParity(int n){
    this->parity = n;
}
void Serial::setStopBits(int n){
    this->stopBits = n;
}
void Serial::setBaudRate(speed_t speed){
    this->baud = speed;

}
void Serial::setPort(QString p){
    this->port = p;
}


bool Serial::openSerial(){

    qDebug() << this->port;
    tty_fd = open(this->port.toStdString().c_str(), O_RDWR |O_NOCTTY | O_NONBLOCK);
    if (tty_fd == -1){
        qDebug() << "Error Modem Device";
        exit(1);
    }
    tcgetattr(STDOUT_FILENO,&old_stdio);

    memset(&stdio,0,sizeof(stdio));
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

    //Set Flags
    stdio.c_iflag=0;
    stdio.c_oflag=0;
    stdio.c_cflag=0;
    stdio.c_lflag=0;
    stdio.c_cc[VTIME]=0;
    tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
    tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking

    memset(&tio,0,sizeof(tio));
    tio.c_iflag=0;
    tio.c_oflag=0;

    //Set Character Size
    switch (this->dataBits){
        case 5:
            tio.c_cflag |= CS5;
        break;
        case 6:
            tio.c_cflag |= CS6;
        break;
        case 7:
            tio.c_cflag |= CS7;
        break;
        case 8:
            tio.c_cflag |= CS8;
        break;
    }
    //Set Stop Bits
    if (this->stopBits == 2)
        tio.c_cflag |= CSTOPB;

    if (this->parity == EVEN_PARITY){
        tio.c_cflag |= PARENB;
    }
    if (this->parity == ODD_PARITY){
        tio.c_cflag |= PARENB;
        tio.c_cflag |= PARODD;
    }
    tio.c_cflag |= CREAD|CLOCAL;   //Set Read and Ignore Control Lines 
    tio.c_iflag = IGNPAR | ICRNL;

    stdio.c_cc[VMIN]=1;
    tio.c_cc[VMIN]=0;
    tio.c_cc[VTIME]=0;
    cfsetospeed(&tio,this->baud);            // 115200 baud
    cfsetispeed(&tio,this->baud);            // 115200 baud
    //tcflush(tty_fd, TCIFLUSH);
    tcsetattr(tty_fd,TCSANOW,&tio);

    return true;
   }

void Serial::closeSerial(){
    close(tty_fd);
    tcsetattr(STDOUT_FILENO,TCSANOW,&old_stdio);
}


int Serial::readSerial(){
    if (readyRead){
        int r = read(tty_fd,&data,1);
        if (r == 1){
            qDebug() << "Read " << data<< " to "<< buffer.write ;
            buffer.buffer[buffer.write] = data;
            //Check if said location is cleared
            if (buffer.cleared[buffer.write] == 1)
                buffer.overrun = 1;
            //Flag Written
            buffer.cleared[buffer.write] = 1;
            buffer.write = (buffer.write + 1) % 2;
        }else{
            readyRead = false;
        }
        return r;
    }
    return -1;
}


void Serial::writeSerial(unsigned char c){
    write(tty_fd, &c,1);

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

