/*******************************************************************************
 * stnUnix.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef NETWORK_IO_H
#define NETWORK_IO_H

#include <stc.h>

/* System headers the network I/O library needs */
#if STC_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if STC_HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#if STC_HAVE_ERRNO_H
#include <errno.h>
#endif
#if STC_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#if STC_HAVE_UNISTD_H
#include <unistd.h>
#endif
#if STC_HAVE_STRING_H
#include <string.h>
#endif
#if STC_HAVE_NETINET_TCP_H
#include <netinet/tcp.h>
#endif
#if STC_HAVE_NETINET_SCTP_UIO_H
#include <netinet/sctp_uio.h>
#endif
#if STC_HAVE_NETINET_SCTP_H
#include <netinet/sctp.h>
#endif
#if STC_HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if STC_HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#if STC_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if STC_HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif
#if STC_HAVE_NETDB_H
#include <netdb.h>
#endif
#if STC_HAVE_FCNTL_H
#include <fcntl.h>
#endif
#if STC_HAVE_SYS_SENDFILE_H
#include <sys/sendfile.h>
#endif
#if STC_HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
/* End System Headers */

#include "stl.h"
#include "stlNetworkIo.h"
#include "stlError.h"

stlStatus stnInetNtop( stlInt32        aAf,
                       const void    * aSrc,
                       stlChar       * aDst,
                       stlSize         aSize,
                       stlErrorStack * aErrorStack);

stlStatus stnInetPton( stlInt32        aAf,
                       const stlChar * aSrc,
                       void          * aDst,
                       stlErrorStack * aErrorStack );

stlStatus stnGetSockName( stlSocket       aSock, 
                          stlSockAddr   * aSockAddr,
                          stlErrorStack * aErrorStack );

stlStatus stnGetPeerName( stlSocket       aSock, 
                          stlSockAddr   * aSockAddr,
                          stlErrorStack * aErrorStack );

stlStatus stnSockAddrVarsSet( stlSockAddr   * aAddr,
                              stlInt32        aFamily,
                              stlPort         aPort,
                              stlErrorStack * aErrorStack);

stlStatus stnWaitForIoOrTimeout( stlFile       * aFile,
                                 stlSocket     * aSock,
                                 stlInt32        aForRead,
                                 stlErrorStack * aErrorStack);


#endif  /* ! NETWORK_IO_H */

