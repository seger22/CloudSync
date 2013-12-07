#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H
#include<iostream>
#include<string>
#include <QDir>
#include <stdlib.h> // to get path of home dir
#include "pugixml.hpp"
#include "xml.h"

using namespace std;

class ConfigurationManager
{

public:
    ConfigurationManager();
    static string getValue();
    static string getLocation();
    static int getBlockSize();
    static string getTempFileLocation();
    static string getAllUserFileLocation();


};

#endif // CONFIGURATIONMANAGER_H
