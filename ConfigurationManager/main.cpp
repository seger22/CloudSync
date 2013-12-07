#include <iostream>
#include "pugixml.hpp"
#include <xml.h>
using namespace std;

int main()
{
  XML xml;
  cout<< xml.getValue(xml.N_WINDOW)<<endl;

    return 0;
}


