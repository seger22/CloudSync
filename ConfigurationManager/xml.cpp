#include "xml.h"

XML::XML()
{
    result = doc.load_file("Config.xml");
}



string XML::getValue(string nodeName){
  string temp= doc.child(nodeName.c_str()).text().get();
  return temp;
}
