#ifndef SPATCH_H
#define SPATCH_H

#define SSHD_USER "libssh"
#define SSHD_PASSWORD "libssh"

#define KEYS_FOLDER "/etc/ssh/"
#define SERVER_PORT 25000


#include <libssh/libssh.h>
#include <libssh/server.h>
#include <libssh/callbacks.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <poll.h>
#include <pty.h>
#include <pthread.h>
#include <usrFile.h>

typedef struct s_sessionData
{
    ssh_session session;
    ssh_channel channel;
    ssh_session clientsession;
    ssh_channel clientchannel;
    usrData*    uData;
}sessionData;

typedef struct s_threadList
{
    pthread_t thread;
    sessionData sesData;
    struct s_threadList* next;
} sthreadList;

int spatch();

#endif // SPATCH_H
