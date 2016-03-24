/// @file   Odb2Odb1Main.cc
/// @author Anne Fouilloux

#include <iostream>
#include <string>
#include <unistd.h>
#include "odb_api/odbcapi.h"

#include "Odb2Odb1.h"

using namespace std;

int
main(int argc, char *argv[])
{
 odb_start();

 odb::tool::Odb2Odb1 sodb(argc, argv);

 if (sodb.prefixInputfile() != "") {
   cout << " number of pools " << sodb.npools() << endl;;
   sodb.create();
 }
}
