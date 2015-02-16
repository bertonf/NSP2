#ifndef CMDLIST_H
#define CMDLIST_H

#include <libssh/libssh.h>
#include "Spatch.h"

void cmd_createuser(char **cmd, sessionData* sesData);
void cmd_lsserv(char** cmdtab, sessionData* sesData);
void cmd_lsuser(char** cmdtab, sessionData* sesData);
void cmd_lsaccess(char** cmdtab, sessionData* sesData);
void cmd_connectto(char** cmdtab, sessionData* sesData);

#endif // CMDLIST_H
