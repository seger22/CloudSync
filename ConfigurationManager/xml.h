#ifndef XML_H
#define XML_H
#include <iostream>
#include "pugixml.hpp"
using namespace std;
class XML
{
public:
    XML();
     string getValue(string nodeName);
     string N_WINDOW="window";
     string N_DBUSRNAME="dbuser";
     string N_DBPASS="dbpass";
     string N_TMPFILE="tmpfile";
     string N_USRDESTDIR="destusrdir";
     string N_ALLUSRDIR="allusrdir";


 private:
    pugi::xml_document doc;
    pugi::xml_parse_result result ;

};

#endif // XML_H
