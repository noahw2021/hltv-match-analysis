//
//  netapi.h
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/14/23.
//

#include "netapi.h"
#include <stdlib.h>
#include <string.h>

PNET_CTX NetCtx;

void NetInit(void) {
    NetCtx = malloc(sizeof(NET_CTX));
    memset(NetCtx, 0, sizeof(NET_CTX));
    
    NetCtx->ThisCurlCtx = curl_easy_init();
    return;
}

void NetShutdown(void) {
    if (!NetCtx)
        return;
    
    if (NetCtx->ThisCurlCtx)
        curl_easy_cleanup(NetCtx->ThisCurlCtx);
    
    free(NetCtx);
    return;
}
