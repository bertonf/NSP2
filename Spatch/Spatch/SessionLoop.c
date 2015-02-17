#include <libssh/libssh.h>
#include <libssh/server.h>
#include <libssh/callbacks.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <poll.h>
#include <pty.h>

#include "Spatch.h"
#include "CmdAnalyser.h"
#include "usrFile.h"

static int auth_password(const char *user, const char *password, sessionData* sesData)
{
    usrData* uData;

    uData = GetUsrData((char*)user);
    if (uData == NULL)
        return (0);
    if (strcmp(password, uData->pwd))
        return (0);
    sesData->uData = uData;
    return (1);

    /*
    if(strcmp(user, SSHD_USER))
        return 0;
    if(strcmp(password, SSHD_PASSWORD))
        return 0;
    return 1; // authenticated*/
}

static int authenticate(sessionData* sesData)
{
    ssh_message message;

    do {
        message=ssh_message_get(sesData->session);
        if(!message)
            break;
        switch(ssh_message_type(message)){
            case SSH_REQUEST_AUTH:
                switch(ssh_message_subtype(message)){
                    case SSH_AUTH_METHOD_PASSWORD:
                        printf("User %s wants to auth with pass %s\n",
                               ssh_message_auth_user(message),
                               ssh_message_auth_password(message));
                        if(auth_password(ssh_message_auth_user(message),
                           ssh_message_auth_password(message), sesData)){
                               ssh_message_auth_reply_success(message,0);
                               ssh_message_free(message);
                               return 1;
                           }
                        ssh_message_auth_set_methods(message,
                                                SSH_AUTH_METHOD_PASSWORD |
                                                SSH_AUTH_METHOD_INTERACTIVE);
                        // not authenticated, send default message
                        ssh_message_reply_default(message);
                        break;

                    case SSH_AUTH_METHOD_NONE:
                    default:
                        printf("User %s wants to auth with unknown auth %d\n",
                               ssh_message_auth_user(message),
                               ssh_message_subtype(message));
                        ssh_message_auth_set_methods(message,
                                                SSH_AUTH_METHOD_PASSWORD |
                                                SSH_AUTH_METHOD_INTERACTIVE);
                        ssh_message_reply_default(message);
                        break;
                }
                break;
            default:
                ssh_message_auth_set_methods(message,
                                                SSH_AUTH_METHOD_PASSWORD |
                                                SSH_AUTH_METHOD_INTERACTIVE);
                ssh_message_reply_default(message);
        }
        ssh_message_free(message);
    } while (1);
    return 0;
}

void* NewSessionLoop(sessionData* sesData)
{
    ssh_session session = sesData->session;
    ssh_message message;
    int auth=0;
    int shell=0;

    if (ssh_handle_key_exchange(session)) {
        printf("ssh_handle_key_exchange: %s\n", ssh_get_error(session));
        return (NULL);
    }

    /* proceed to authentication */
    auth = authenticate(sesData);
    if(!auth){
        printf("Authentication error: %s\n", ssh_get_error(session));
        ssh_disconnect(session);
        return (NULL);
    }


    /* wait for a channel session */
    do {
        message = ssh_message_get(session);
        if(message){
            if(ssh_message_type(message) == SSH_REQUEST_CHANNEL_OPEN &&
                    ssh_message_subtype(message) == SSH_CHANNEL_SESSION) {
                sesData->channel = ssh_message_channel_request_open_reply_accept(message);
                ssh_message_free(message);
                break;
            } else {
                ssh_message_reply_default(message);
                ssh_message_free(message);
            }
        } else {
            break;
        }
    } while(!sesData->channel);

    if(!sesData->channel) {
        printf("Error: client did not ask for a channel session (%s)\n",
                                                    ssh_get_error(session));
        ssh_finalize();
        return (NULL);
    }


    /* wait for a shell */
    do {
        message = ssh_message_get(session);
        if(message != NULL) {
            if(ssh_message_type(message) == SSH_REQUEST_CHANNEL) {
                if(ssh_message_subtype(message) == SSH_CHANNEL_REQUEST_SHELL) {
                    shell = 1;
                    ssh_message_channel_request_reply_success(message);
                    ssh_message_free(message);
                    break;
                }
                /*else if(ssh_message_subtype(message) == SSH_CHANNEL_REQUEST_PTY) {
                    ssh_message_channel_request_
                    ssh_message_channel_request_reply_success(message);
                    ssh_message_free(message);
                    continue;
                }*/
            }
            ssh_message_reply_default(message);
            ssh_message_free(message);
        } else {
            break;
        }
    } while(!shell);

    if(!shell) {
        printf("Error: No shell requested (%s)\n", ssh_get_error(session));
        return (NULL);
    }

    printf("it works !\n");

    int i ;
    char buf[2048];

    do {
        memset(buf, 0, 2048);
        ssh_channel_write(sesData->channel, "\r\nSpatch#", 9);
        i = ssh_channel_read(sesData->channel, buf, 2048, 0);
        if(i>0)
        {
            if(i==1 && *buf=='\r')
                ssh_channel_write(sesData->channel, "\r\n", 2);
            else
                CmdAnalyser(buf, sesData);

//                ssh_channel_write(chan, buf, i);
/*            if(write(1,buf,i)<0)
                printf("error writting to buffer\n");
                return NULL;*/
        }
    } while(!ssh_channel_is_closed(sesData->channel));

    //main_loop(chan);
    return (NULL);
}
