#include "configurationmanager.h"


ConfigurationManager::ConfigurationManager()
{


}

string ConfigurationManager::getLocation(){
    XML xml;
    char* pPath;
     pPath = getenv ("HOME");
     QString path(pPath);
     if (pPath!=NULL){
         path = path +QString(xml.getValue(xml.N_USRDESTDIR).c_str());
         QDir dir(path);
         if (!dir.exists()) {
             dir.mkpath(".");
         }
     }
    return path.toStdString();
}

int ConfigurationManager::getBlockSize(){
    XML xml;
    return atoi(xml.getValue(xml.N_WINDOW).c_str());
}

string ConfigurationManager::getTempFileLocation(){
    XML xml;
    return xml.getValue(xml.N_TMPFILE);
}

string ConfigurationManager::getAllUserFileLocation(){
    XML xml;
    return xml.getValue(xml.N_ALLUSRDIR);
}


