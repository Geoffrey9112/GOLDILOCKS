#include <alloca.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

/* The child process. This sends the file descriptor. */
int childProcess(char * filename, int sock)
{
    int fd;
    struct iovec vector;        /* some data to pass w/ the fd */
    struct msghdr msg;          /* the complete message */
                                /* include the fd */

    /* Open the file whose descriptor will be passed. */
    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        perror("open");
        return 1;
    }

    /* Send the file name down the socket, including the trailing
       '\0' */
    vector.iov_base = filename;
    vector.iov_len = strlen(filename) + 1;

    /* Put together the first part of the message. Include the
       file name iovec */
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &vector;
    msg.msg_iovlen = 1;

#ifdef SCM_RIGHTS
    {
        struct cmsghdr * cmsg;      /* the control message, which will */
    
        /* Now for the control message. We have to allocate room for
           the file descriptor. */
        cmsg = alloca(sizeof(struct cmsghdr) + sizeof(fd));
        cmsg->cmsg_len = sizeof(struct cmsghdr) + sizeof(fd);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;

        /* copy the file descriptor onto the end of the control 
           message */
        memcpy(CMSG_DATA(cmsg), &fd, sizeof(fd));
    
        msg.msg_control = cmsg;
        msg.msg_controllen = cmsg->cmsg_len;
    }
#else
    /* Old BSD 4.3 way. */
    msg.msg_accrights = (void*)&fd;
    msg.msg_accrightslen = sizeof(fd);
#endif
  
    if (sendmsg(sock, &msg, 0) != vector.iov_len)
    {
        printf("sendmsg\n");
        exit(0);
    }

    return 0;
}

/* The parent process. This receives the file descriptor. */
int parentProcess(int sock)
{
    char buf[80];               /* space to read file name into */
    struct iovec vector;	/* file name from the child */
    struct msghdr msg;		/* full message */
    int fd;
    int bytes = 0;

    /* set up the iovec for the file name */
    vector.iov_base = buf;
    vector.iov_len = 80;

    /* the message we're expecting to receive */

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &vector;
    msg.msg_iovlen = 1;

#ifdef SCM_RIGHTS
    {
        struct cmsghdr * cmsg;      /* control message with the fd */
    
        /* dynamically allocate so we can leave room for the file
           descriptor */
        cmsg = alloca(sizeof(struct cmsghdr) + sizeof(fd));
        cmsg->cmsg_len = sizeof(struct cmsghdr) + sizeof(fd);
        msg.msg_control = cmsg;
        msg.msg_controllen = cmsg->cmsg_len;
    
        if (!recvmsg(sock, &msg, 0))
        {
            printf("failed to receive message\n");
            return 1;
        }

        /* grab the file descriptor from the control structure */
        memcpy(&fd, CMSG_DATA(cmsg), sizeof(fd));
    
        if( (cmsg->cmsg_len != (sizeof(struct cmsghdr) + sizeof(fd))) ||
            (cmsg->cmsg_level != SOL_SOCKET) ||
            (cmsg->cmsg_type != SCM_RIGHTS) )
        {
            printf( "but message control section is invalid ( len:%ld, level:%d, type:%d )\n",
                    cmsg->cmsg_len, 
                    cmsg->cmsg_level,
                    cmsg->cmsg_type);
            printf( "its seems to be ( len:%d, level:%d, type:%d )\n",
                    20, 
                    SOL_SOCKET,
                    SCM_RIGHTS);
            return 1;
        }
    }
#else
    /* Old BSD 4.3 way. Not tested. */
    msg.msg_accrights = (void*)&fd;
    msg.msg_accrightslen = sizeof(fd);
    
    if (!recvmsg(sock, &msg, 0))
    {
        printf("failed to receive message\n");
        return 1;
    }
#endif
    printf("got file descriptor for '%s'( fd:%d )\n", 
           (char *) vector.iov_base, fd);

    while(1)
    {
        bytes = read( fd, buf, 80 );

        if( bytes < 0 )
        {
            return 1;
        }

        if( bytes < 80 )
        {
            break;
        }

        write( 1, buf, bytes );
    }
    printf("fd : %d\n", fd );

    return 0;
}

int main(int argc, char ** argv)
{
    int socks[2];
    int status;

    if (argc != 2)
    {
        fprintf(stderr, "only a single argument is supported\n");
        return 1;
    }

    /* Create the sockets. The first is for the parent and the
       second is for the child (though we could reverse that
       if we liked. */
    if (socketpair(PF_UNIX, SOCK_STREAM, 0, socks))
    {
        printf("socketpair");
        exit(0);
    }

    if (!fork())
    {
        /* child */
        close(socks[0]);
        return childProcess(argv[1], socks[1]);
    }

    /* parent */
    close(socks[1]);
    parentProcess(socks[0]);

    /* reap the child */
    wait(&status);

    if (WEXITSTATUS(status))
    {
        fprintf(stderr, "child failed\n");
    }

    return 0;
}

