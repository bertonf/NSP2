#ifndef SENDCLIENTCMD_H
#define SENDCLIENTCMD_H

#include <libssh/libssh.h>

ssh_channel open_client_channel(ssh_session session);
int close_client_channel(ssh_channel channel);
int send_cmd_to_ssh(ssh_channel chanusr, ssh_channel chansvr, char *buff);


#endif // SENDCLIENTCMD_H
