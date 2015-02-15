#ifndef CMDANALYSER_H
#define CMDANALYSER_H

#include <libssh/libssh.h>
#include "Spatch.h"
#include "usrFile.h"

typedef struct s_CmdData
{
    char* name;
    void (*func)(char**, sessionData*);
    eaccess access;
}CmdData;

void CmdAnalyserInit();
void CmdAnalyser(char* cmd, sessionData* sesData);


#endif // CMDANALYSER_H
