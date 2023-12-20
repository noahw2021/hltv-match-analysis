//
//  hltv_events.c
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/20/23.
//

#include "hltv.h"
#include "../net/netapi.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

unsigned char InEvent;
PHLTV_EVENT_PLAYER EventList;
int EventListCnt;

void HltvStartEventAnalysis(void) {
    if (InEvent)
        return;
    InEvent = 1;
    
    EventListCnt = 0;
    EventList = NULL;
}

void HltvAnalyzeEvent(unsigned long EventId) {
    if (!InEvent)
        return;
    
    char* CraftedURL = malloc(128);
    snprintf(CraftedURL, 128, "https://www.hltv.org/events/%lu/hma", EventId);
    
    unsigned long EventBufferSz = 500000;
    char* EventBuffer = malloc(EventBufferSz);
    
    NetDownload(CraftedURL, EventBuffer, EventBufferSz, &EventBufferSz);
    if (EventBufferSz > 500000) {
        free(EventBuffer);
        EventBuffer = malloc(200000 + EventBufferSz);
        NetDownload(CraftedURL, EventBuffer, 200000 + EventBufferSz, &EventBufferSz);
    }
    
    char* Playoffs = strstr(EventBuffer, "<div class=\"section-header brackets\"><span id=\"Brackets\">Playoffs</span>");
    if (!Playoffs) {
        free(EventBuffer);
        return;
    }
    
    Playoffs += strlen("<div class=\"section-header brackets\"><span id=\"Brackets\">Playoffs</span>");
    char* Groups = strstr(Playoffs, "section-header brackets");
    
    // Calculate for playoffs
    while (Playoffs < Groups) {
        Playoffs = strstr(Playoffs, "nick&quot;:&quot;");
        Playoffs += strlen("nick&quot;:&quot;");
        
        char PlayerName[64];
        memset(PlayerName, 0, 64);
        for (int i = 0; i < 64; i++) {
            if (*Playoffs == '&')
                break;
            if (*Playoffs == 0x00)
                break;
                
            PlayerName[i] = *Playoffs;
            Playoffs++;
        }
        
        Playoffs = strstr(Playoffs, "rating&quot;:&quot;");
        Playoffs += strlen("rating&quot;:&quot;");
        char RatingBfr[16];
        memset(RatingBfr, 0, 16);
        for (int i = 0; i < 16; i++) {
            if (*Playoffs == '&')
                break;
            
            RatingBfr[i] = *Playoffs;
            Playoffs++;
        }
        float Rating = atof(RatingBfr);
        
        unsigned char PlayerFound = 0;
        printf("n: %s\n", PlayerName);
        
        for (int i = 0; i < EventListCnt; i++) {
            PHLTV_EVENT_PLAYER ThisPlaya = &EventList[i];
            if (strstr(ThisPlaya->PlayerName, PlayerName)) {
                ThisPlaya->MatchCount[1]++;
                ThisPlaya->PlayerRatingSum[1] += Rating;
                PlayerFound = 1;
                break;
            }
        }
        
        if (!PlayerFound) {
            EventList = realloc(EventList,
                sizeof(HLTV_EVENT_PLAYER) * (EventListCnt + 1));
            PHLTV_EVENT_PLAYER ThisPlayer = &EventList[EventListCnt];
            EventListCnt++;
            memset(ThisPlayer, 0, sizeof(HLTV_EVENT_PLAYER));
            
            strcpy(ThisPlayer->PlayerName, PlayerName);
            ThisPlayer->PlayerRatingSum[1] = Rating;
            ThisPlayer->MatchCount[1] = 1;
        }
    }
    
    // Calculate for groups
    while (1) {
        Playoffs = strstr(Playoffs, "nick&quot;:&quot;");
        if (!Playoffs)
            break;
        Playoffs += strlen("nick&quot;:&quot;");
        
        char PlayerName[64];
        memset(PlayerName, 0, 64);
        for (int i = 0; i < 64; i++) {
            if (*Playoffs == '&')
                break;
            
            PlayerName[i] = *Playoffs;
            Playoffs++;
        }
        
        Playoffs = strstr(Playoffs, "rating&quot;:&quot;");
        Playoffs += strlen("rating&quot;:&quot;");
        char RatingBfr[16];
        memset(RatingBfr, 0, 16);
        for (int i = 0; i < 16; i++) {
            if (*Playoffs == '&')
                break;
            
            RatingBfr[i] = *Playoffs;
            Playoffs++;
        }
        float Rating = atof(RatingBfr);
        
        unsigned char PlayerFound = 0;
        for (int i = 0; i < EventListCnt; i++) {
            PHLTV_EVENT_PLAYER ThisPlaya = &EventList[i];
            if (!strcmp(ThisPlaya->PlayerName, PlayerName)) {
                ThisPlaya->MatchCount[0]++;
                ThisPlaya->PlayerRatingSum[0] += Rating;
                PlayerFound = 1;
                break;
            }
        }
        
        if (!PlayerFound) {
            EventList = realloc(EventList,
                sizeof(HLTV_EVENT_PLAYER) * (EventListCnt + 1));
            memset(&EventList[EventListCnt], 0, sizeof(HLTV_EVENT_PLAYER));
            PHLTV_EVENT_PLAYER ThisPlayer = &EventList[EventListCnt++];
            
            strcpy(ThisPlayer->PlayerName, PlayerName);
            ThisPlayer->PlayerRatingSum[0] = Rating;
            ThisPlayer->MatchCount[0] = 1;
        }
    }
    
    free(EventBuffer);
    return;
}

PHLTV_EVENT_PLAYER HltvGetEventsPlayerList(int* PlayerCount) {
    if (!InEvent)
        return NULL;
    
    *PlayerCount = EventListCnt;
    return EventList;
}

void HltvDestroyEventAnalysis(void) {
    if (!InEvent)
        return;
    
    free(EventList);
    InEvent = 0;
    return;
}
