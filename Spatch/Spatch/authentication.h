#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <libssh/libssh.h>

int authenticate_kbdint(ssh_session session, const char *password);
int authenticate_console(ssh_session session, char *password);

#endif // AUTHENTICATION_H
