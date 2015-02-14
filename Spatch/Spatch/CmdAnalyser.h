#ifndef CMDANALYSER_H
#define CMDANALYSER_H

#include "usrFile.h"

typedef struct s_CmdData
{
    char* name;
    void (*func)();
    eaccess access;
}CmdData;

#endif // CMDANALYSER_H
