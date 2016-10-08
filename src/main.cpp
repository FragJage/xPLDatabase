#include <iostream>
#include <exception>
#include "xPLDatabase.h"

int main(int argc, char* argv[])
{
  int res = 0;

  try
  {
    xPLDatabase xPLDev;

  	Service* pService = Service::Create("xPLDatabase", "Log xPL message values into a database", &xPLDev);
  	res = pService->Start(argc, argv);
  	Service::Destroy();
  }
  catch(const exception &e)
  {
      std::cout << e.what();
  }
	return res;
}
