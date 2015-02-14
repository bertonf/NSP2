#include "Spatch.h"
#include "usrFile.h"


void mainTestUsrData(void)
{
    usrData* usr;

    usr = GetUsrData("bertonf");
    DebugTraceUsrData(usr);
    strcpy(usr->name, "bertons");
    SaveUsrData(usr);
    DebugTraceUsrData(usr);
    FreeUsrData(usr);
}



int main(void)
{
    spatch();
    //connect_ssh ("88.188.248.11", "ubuntu01", 5656,  0);


    return 0;
}

