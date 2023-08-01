#include"Webserver.h"
#include"Webserver.cpp"
#include "Httpconn.cpp"
#include "Epoller.cpp"
#include"Log.cpp"
#include "Timer.cpp"
#include"Buffer.cpp"
using namespace std;
int main()
{
Webserver w;
w.Initsocket();
w.Start();
return 0;
}