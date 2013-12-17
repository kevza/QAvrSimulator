#include "avr_core_builder.h"
#include <iostream>
#include <QDebug>
#include <QDir>
#define PLUGIN_PATH  "/sdcard/plugins/"

Avr_Core_Builder::Avr_Core_Builder(QObject *parent) : QObject(parent)
{  

}

/**
 * @brief Avr_Core_Builder::loadCore Loads the core described by the configuration file at the path mmcu
 * @param mmcu The path for the configuration file
 * @return
 */
Avr_Core* Avr_Core_Builder::loadCore(QString mmcu){
    //Loader to open plugins
    QPluginLoader loader;
    core = new Avr_Core();
    //Setup the Basics
    core->setMemory(new Avr_Memory);
    core->setRegisters(new Avr_Registers);

    //Load Configuration File
    string line;string id;string setting;

    QString path = PLUGIN_PATH + mmcu;
    ifstream configFile;
    configFile.open(path.toStdString().c_str());

    if (configFile.is_open()){
        //Process config file, there might be a problem
        //with the loop termination here
        while (!configFile.eof()){
            configFile >> line;
            if (line == "END"){
                break;
            }

            if (line[0]==';'){
                //Line is a comment skip
                continue;
            }
            int i = line.find(':');
            id = line.substr(0,i);
            setting = line.substr(i + 1, line.size() - i - 1);
            if (id == "RAMSIZE"){
                qDebug() << "Load Ram\n";
                core->mem->initRam(sizeToInt(setting) + 0xff);
                core->reg->setRam(core->mem->getRam());
				core->reg->setRamEnd(core->mem->getRamEnd());
            }else if (id == "FLASHSIZE"){
                qDebug() << "Load Flash\n";
                core->setFlash(new Avr_Flash(sizeToInt(setting)));

            }else if (id == "EPROMSIZE"){
                qDebug() << "Load Eprom\n";
                core->mem->initEprom(sizeToInt(setting));

            }else if (id == "SPL"){
                qDebug() << "Set SPL\n";
                core->reg->setStackPL(getRegPtr(setting));

            }else if (id == "SPH"){
                qDebug() << "Set SPH\n";
                core->reg->setStackPH(getRegPtr(setting));

            }else if (id == "SREG"){
                core->reg->setSREGP(getRegPtr(setting));

            }else if (id == "PLUGINLIB"){

                qDebug() << "Load Plugin " << QString(setting.c_str()) << "\n";
                loader.setFileName(PLUGIN_PATH +QString(setting.c_str()));
                loader.load();

                QObject *plugin = loader.instance();
                Avr_Hardware_Interface *h = qobject_cast<Avr_Hardware_Interface*>(plugin);
                h->attachRegister(core->reg);

                //Load Registers for the plugin
                for (int j = 0; j < h->getRegisterCount();j++){
                    configFile >> line;
                    if (line[0]==';'){
                        j -= 1;
                        continue;
                    }
                    i = line.find(':');
                    id = line.substr(0,i);
                    setting = line.substr(i + 1, line.size() - i - 1);
                    qDebug() << "Setting " << QString(setting.c_str()) << "\n";
                    h->bindRegister(QString(id.c_str()),getRegPtr(setting));
                    h->bindRegister(QString(id.c_str()),getRegLoc(setting));
                }
                //Load Interrupts



                core->hardware.push_back(h);
            }else if (id == "INTERFACE"){
                loader.setFileName(QString(setting.c_str()));
                loader.load();
                QList <QObject*> objects = loader.staticInstances();
                if (loader.isLoaded()){
                    std::cout << "Loaded\n";
                }else{
                    std::cout << "Object Count " << objects.size() <<"\n";
                }
                QObject *plugin = loader.instance();
                //Set the Interface;
                this->interface = qobject_cast<Avr_Hardware_Interface*>(plugin);
            }
        }
    }else{
        qDebug() << "Configuration file " << mmcu << " failed to load";
        exit(1);
    }
    return core;
}

/**
 * @brief Avr_Core_Builder::sizeToInt Converts a memory size to an integer number of bytes
 * @param size
 * @return
 */
int Avr_Core_Builder::sizeToInt(string size){
    int i = size.size() - 1;
    std::stringstream ss;
    int res;
    if (i > 0 && size.at(i) == 'K'){
        ss << size.substr(0,i);
        ss >> res;
        res = res * 1024;
    }else if (i > 0 && size.at(i) == 'M'){
        ss << size.substr(0,i);
        ss >> res;
        res = res * 1048576;

    }else {
        ss << size;
        ss >> res;
    }
    return res;
}

bool Avr_Core_Builder::loadProgram(string file){
    if (!core){
        return false;
    }else{
        return core->flash->loadHex(file);
    }
}

/**
 * @brief Avr_Core_Builder::getRegPtr Gets a pointer to a location in an array by the hexdecimal address (Assuming the array starts at 0x00)
 * @param loc
 * @return
 */
uint8_t* Avr_Core_Builder::getRegPtr(string loc){
    std::stringstream ss;
    int res;
    ss << std::hex << loc;
    ss >> res;
    uint8_t *ptr;
    ptr = core->mem->getRam();
    ptr += res;
    *ptr = 0;
    return   ptr;
}


int Avr_Core_Builder::getRegLoc(string loc){
    std::stringstream ss;
    int res;
    ss << std::hex << loc;
    ss >> res;
    return   res;
}
