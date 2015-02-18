#ifndef CMDLIST_H
#define CMDLIST_H

#include <libssh/libssh.h>
#include "Spatch.h"

void cmd_createuser(char **cmd, sessionData* sesData);
void cmd_deleteuser(char **cmd, sessionData* sesData);
void cmd_lsserv(char** cmdtab, sessionData* sesData);
void cmd_lsuser(char** cmdtab, sessionData* sesData);
void cmd_lsaccess(char** cmdtab, sessionData* sesData);
void cmd_connectto(char** cmdtab, sessionData* sesData);
void cmd_disconnect(char** cmdtab, sessionData* sesData);
void cmd_createserver(char **cmd, sessionData* sesData);
void cmd_deleteserver(char **cmd, sessionData* sesData);
void cmd_createaccess(char **cmd, sessionData* sesData);
void cmd_deleteaccess(char **cmd, sessionData *sesData);

#endif // CMDLIST_H
