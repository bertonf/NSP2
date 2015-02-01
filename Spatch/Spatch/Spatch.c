/* This is a sample implementation of a libssh based SSH server */
/*
Copyright 2003-2011 Aris Adamantiadis

This file is part of the SSH Library

You are free to copy this file, modify it in any way, consider it being public
domain. This does not apply to the rest of the library though, but it is
allowed to cut-and-paste working code from this file to any license of
program.
The goal is to show the API in action. It's not a reference on how terminal
clients must be made or how a client should react.
*/

//#include "config.h"

#include "Spatch.h"
#include "SessionLoop.h"

static sthreadList* newNodeList(sthreadList* list, ssh_session session)
{
    sthreadList *new;
    new = (sthreadList*)malloc(sizeof(sthreadList));
    new->session = session;
    new->next = list;
    return (new);
}

static void cleanList(sthreadList* list)
{
    sthreadList* tmp = NULL;
    do {
        ssh_disconnect(list->session);
        tmp = list;
        list = tmp->next;
        free(tmp);
        tmp = NULL;
    }while (list != NULL);

}

int spatch()
{
    ssh_session session = NULL;
    ssh_bind sshbind = NULL;
    sthreadList* list = NULL;

    int r;
    int port = 25000;

    sshbind=ssh_bind_new();
    //session=ssh_new();

    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT, &port);
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_DSAKEY,
                                            KEYS_FOLDER "ssh_host_dsa_key");
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY,
                                            KEYS_FOLDER "ssh_host_rsa_key");



    if(ssh_bind_listen(sshbind)<0){
        printf("Error listening to socket: %s\n", ssh_get_error(sshbind));
        return 1;
    }
    printf("Started sample libssh sshd on port %d\n", port);
    printf("You can login as the user %s with the password %s\n", SSHD_USER,
                                                            SSHD_PASSWORD);
    while(1)
    {
        session=ssh_new();
        r = ssh_bind_accept(sshbind, session);
        if(r==SSH_ERROR){
          printf("Error accepting a connection: %s\n", ssh_get_error(sshbind));
          return 1;
        }
    //ADD SESSION TO LIST
        list = newNodeList(list, session);
        pthread_create(&(list->thread), NULL, NewSessionLoop, (void*)session);
    }
    
    /*if (session != NULL)
        ssh_disconnect(session);*/
    cleanList(list);
    ssh_bind_free(sshbind);
    ssh_finalize();
    return 0;
}

