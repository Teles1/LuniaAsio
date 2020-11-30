#pragma once
#ifndef LOG_H
#define LOG_H

#include <iostream>

using namespace std;

enum class typelog {
    DEBUG,
    INFO,
    WARN,
    EXCEPTION
};

struct structlog {
    bool headers = true;
    typelog level = typelog::WARN;
};

extern structlog LOGCFG;

class LOG {
public:
    LOG() {}
    LOG(typelog type) {
        msglevel = type;
        if (LOGCFG.headers) {
            operator << ("[" + getLabel(type) + "]");
        }
    }
    ~LOG() {
        if (opened) {
            cout << endl;
        }
        opened = false;
    }
    template<class T>
    LOG& operator<<(const T& msg) {
        if (msglevel >= LOGCFG.level) {
            cout << msg;
            opened = true;
        }
        return *this;
    }
    template<class T>
    void ALLM_WARNING(const T& msg) {
        msglevel = typelog::WARN;
        getHeader(msglevel);
        cout << msg;
        opened = true;
    }
private:
    bool opened = false;
    typelog msglevel = typelog::DEBUG;
    inline string getLabel(typelog type) {
        string label;
        switch (type) {
        case typelog::DEBUG: 
            label = "DEBUG"; 
            break;
        case typelog::INFO:  
            label = "INFO "; 
            break;
        case typelog::WARN:  
            label = "WARN "; 
            break;
        case typelog::EXCEPTION:
            label = "EXCEPTION"; 
            break;
        }
        return label;
    }
    inline void getHeader(typelog type) {
        if (LOGCFG.headers) {
            operator << ("[" + getLabel(type) + "]");
        }
    }
};

#endif  /* LOG_H */