#ifndef SERIAL_H
#define SERIAL_H

#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <QString>
#include <QDebug>

enum {NO_PARITY,EVEN_PARITY,ODD_PARITY};
struct Buffer{
    int write;
    int read;
    int overrun;
    int cleared[2];
    unsigned char buffer[2];
};

class Serial
{
    public:
        Serial();
        ~Serial();
        void setDataBits(int n);
        void setParity(int n);
        void setStopBits(int n);
        void setBaudRate(speed_t speed);
        void setPort(QString p);
        bool openSerial();
        void closeSerial();
        int readSerial();
        void writeSerial(unsigned char c);
        unsigned char data;
        Buffer buffer;

    private:
        struct termios tio;
        struct termios stdio;
        struct termios old_stdio;
        struct sigaction saio;
        int tty_fd;
        int parity;
        int dataBits;
        int stopBits;
        speed_t baud;
        QString port;

        //Signal handler for uart

};

#endif // SERIAL_H
