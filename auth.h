#ifndef AUTH_H
#define AUTH_H

#include <stdbool.h>

#define AUTH_KEY_VERSION 0x01

typedef struct _sAuth {
    unsigned char   devRand[16];
    unsigned char   serRand[16];
    unsigned char   prvMagic[4];
    unsigned char   sessionKeyHash[32];
    unsigned char   cmdMySerial[32];
    bool            isGetRandomBefore;
    bool            isChallengingOkay;
} sAuth;

typedef struct _sAuthResponse {
    unsigned char   response[128];
    unsigned char   len;
} sAuthResponse;

void            AuthInit                ( void );
bool            AuthNext                ( unsigned char* buf, unsigned char fistByteIndex, sAuthResponse* res );
bool            AuthIsOkay              ( void );
sAuth*          AuthGet                 ( void );
void            AuthStatus              ( sAuthResponse* res );
bool            AuthSetKey              ( sAuthResponse* res );
bool            AuthWriteAuthKeyToOpt   ( unsigned char* buf, unsigned char fistByteIndex, sAuthResponse* res );

#endif //! AUTH_H