#include <libssh/libssh.h>
#include <stdlib.h>
#include <string.h>
#include "Spatch.h"
#include "usrFile.h"
#include "cmdlist.h"

void cmd_createuser(char **cmd, sessionData* sesData)
{
    if (cmd[0] != NULL && cmd[1] != NULL && cmd[2] != NULL)
    {
        usrData *newuser = newUsrData();

        newuser->name = malloc(strlen(cmd[1]) + 1);
        newuser->pwd = malloc(strlen(cmd[2]) + 1);
        memset(newuser->name, 0, strlen(cmd[1]) + 1);
        memset(newuser->pwd, 0, strlen(cmd[2]) + 1);
        strcpy(newuser->name, cmd[1]);
        strcpy(newuser->pwd, cmd[2]);
        if (strncmp(cmd[3], "user", 4))
            newuser->spaAccess = USER;
        else if (strncmp(cmd[3], "admin", 5))
            newuser->spaAccess = ADMIN;
        else
        {
            FreeUsrData(newuser);
            ssh_channel_write(sesData->channel,
                              "usage \"createuser user pwd user|admin\"",
                              strlen("usage \"createuser user pwd user|admin\""));
        }
        SaveUsrData(newuser);
        FreeUsrData(newuser);
    }
    else
    {
        ssh_channel_write(sesData->channel,
                          "usage \"createuser user pwd user|admin\"",
                          strlen("usage \"createuser user pwd user|admin\""));
    }

}
