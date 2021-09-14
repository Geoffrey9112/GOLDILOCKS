#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define _PATH_PROCNET_IFINET6		"/proc/net/if_inet6"

int main()
{
    struct if_nameindex * sInterfaaceList;
    struct if_nameindex * sInterface;
    char                  sBuffer[80] = {0x00, };

    /*
     * IPv4
     */
    int                  sSockFd;
    struct ifreq         sInterfaceReq;
    struct sockaddr_in * sSockAddrIn;

    /*
     * IPv6
     */
    FILE               * sFile;
    char                 sAddr6Str[40];
    char                 sDevName[20];
    unsigned int         sPlen;
    unsigned int         sScope;
    unsigned int         sStatus;
    unsigned int         sIndex;
    char                 sAddr6Element[8][5];
    struct sockaddr_in6  sSockAddrIn6;

    /*
     * 소켓 생성
     */
    sSockFd = socket(AF_INET, SOCK_STREAM, 0 );

    /*
     * 네트워크 인터페이스의 인덱스와 이름 정보를 가져온다.
     */
    sInterfaaceList = if_nameindex();
    for(sInterface = sInterfaaceList; sInterface->if_name != NULL; sInterface++)
    {
        /*
         * IPv4
         */

        /*
         * ioctl()의 SIOCGIFADDR 옵션을 통해 각 네트워크 인터페이스의 IP adress를 얻는다.
         */
        memset( &sInterfaceReq, 0x00, sizeof(struct ifreq));

        strcpy( sInterfaceReq.ifr_name, sInterface->if_name );
        sInterfaceReq.ifr_addr.sa_family = AF_INET;

        if( ioctl(sSockFd, SIOCGIFADDR, &sInterfaceReq) != 0 )
        {
            continue;
        }

        sSockAddrIn = (struct sockaddr_in *)&sInterfaceReq.ifr_addr;
        inet_ntop( AF_INET, &sSockAddrIn->sin_addr, sBuffer, sizeof(sBuffer) );
        
        printf("name    : %s\n", sInterface->if_name);
        printf("index   : %u\n", sInterface->if_index);
        printf("address : %s\n", sBuffer );

        /*
         * IPv6
         */

        /*
         * /proc/net/if_inet6 파일에서 각 네트워크 인터페이스의 IP adress를 얻는다.
         */
        sFile = fopen( _PATH_PROCNET_IFINET6, "r" );
        if( sFile != NULL )
        {
            while( fscanf(sFile,
                          "%4s%4s%4s%4s%4s%4s%4s%4s %02x %02x %02x %02x %20s\n",
                          sAddr6Element[0], sAddr6Element[1], sAddr6Element[2], sAddr6Element[3],
                          sAddr6Element[4], sAddr6Element[5], sAddr6Element[6], sAddr6Element[7],
                          &sIndex, &sPlen, &sScope, &sStatus, sDevName) != EOF )
            {
                if( strcmp(sDevName, sInterface->if_name) == 0 )
                {
                    (void)snprintf(sAddr6Str, 40, "%s:%s:%s:%s:%s:%s:%s:%s",
                                   sAddr6Element[0], sAddr6Element[1], sAddr6Element[2], sAddr6Element[3],
                                   sAddr6Element[4], sAddr6Element[5], sAddr6Element[6], sAddr6Element[7]);

                    sSockAddrIn6.sin6_family = AF_INET6;
                    sSockAddrIn6.sin6_port   = 0;

                    (void)inet_pton(AF_INET6, sAddr6Str, sSockAddrIn6.sin6_addr.s6_addr);
                    (void)inet_ntop(AF_INET6, &sSockAddrIn6.sin6_addr, sBuffer, sizeof(sBuffer));

                    printf("address : %s\n", sBuffer );
                    break;
                }
            }

            (void)fclose(sFile);
        }

        printf("\n");
    }

    if_freenameindex( sInterfaaceList );
    (void)close( sSockFd );

    return 0;
}
