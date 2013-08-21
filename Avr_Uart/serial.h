#ifndef SERIAL_H
#define SERIAL_H

#include <stdio.h>
#ifdef __unix
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
    #include <signal.h>
    #include <sys/types.h>
#endif
#ifdef __WIN32
    #include <windows.h>
    #include <process.h>
#endif
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
        void setBaudRate(unsigned int speed);
        void setPort(QString p);
        bool openSerial();
        void closeSerial();
        int readSerial();
#ifdef __WIN32
        char readWinSerial(HANDLE tty);

#endif
        void writeSerial(unsigned char c);
        unsigned char data;
        Buffer buffer;
        bool sEcho;
        bool hEcho;

    private:
    #ifdef __unix
        struct termios tio;
        struct termios stdio;
        struct termios old_stdio;
        struct sigaction saio;
        int tty_fd;
    #endif
    #ifdef __WIN32
        HANDLE tty_fd;
        HANDLE readThread;
    #endif
        int parity;
        int dataBits;
        int stopBits;
        unsigned int baud;
        QString port;


};

#endif // SERIAL_H
