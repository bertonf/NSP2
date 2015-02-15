#include <libssh/libssh.h>
#include "usrFile.h"

void cmd_createuser(char **cmd, ssh_channel channel)
{
        if (cmd[0] != NULL && cmd[1] != NULL && cmd[2] != NULL)
        {
            usrData *newuser = newUsrData();
            newuser->name = malloc(strlen(cmd[1]);
            newuser->pwd = malloc(strlen(cmd[2]);


            strcpy(newuser->name, cmd[1]);
            strcpy(newuser->pwd, cmd[2]);
            if (strncmp(cmd[3], "USER", 4))
                newuser->spaAccess = USER;
            else if (strncmp(cmd[3], "ADMIN", 5))
                newuser->spaAccess = ADMIN;
            else
            {
                FreeUsrData(newuser);
                ssh_channel_write(channel, ("usage \"cmd user pwd\"", strlen(("usage \"cmd user pwd\""));
            }
            saveusrdata(newuser);
        }
            else
        {
            ssh_channel_write(channel, ("usage \"cmd user pwd\"", strlen(("usage \"cmd user pwd\""));
        }
}
