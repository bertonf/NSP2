#ifndef CMDLIST_H
#define CMDLIST_H

#include <libssh/libssh.h>

void cmd_createuser(char **cmd, ssh_channel channel);

#endif // CMDLIST_H
