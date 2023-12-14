//
//  netapi.h
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/14/23.
//

#ifndef netapi_h
#define netapi_h
#include <curl/curl.h>

void NetInit(void);
void NetShutdown(void);

void NetDownload(const char* Url, char* Buffer,
    unsigned long BufferSize, unsigned long* RequiredBufferSize);
unsigned char NetDidSucceed(void);

typedef struct _NET_CTX {
    CURL* ThisCurlCtx;
    unsigned char DidSucceed;
}NET_CTX, *PNET_CTX;
extern PNET_CTX NetCtx;

#endif /* netapi_h */
