//
//  hltv_search.c
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/14/23.
//

#include "hltv.h"
#include "../net/netapi.h"

#include <stdlib.h>
#include <string.h>

PHLTV_SEARCH_LIST HltvSearchPlayer(const char* Name) {
    PHLTV_SEARCH_LIST SearchList = malloc(sizeof(HLTV_SEARCH_LIST));
    memset(SearchList, 0, sizeof(HLTV_SEARCH_LIST));
    
    char* SearchQuery = malloc(768);
    snprintf(SearchQuery, 768, "https://www.hltv.org/search?query=%s", Name);
    char* SearchBuffer = malloc(250000);
    unsigned long BufferSize = 250000;
    
    NetDownload(SearchQuery, SearchBuffer, 250000, &BufferSize);
    if (BufferSize > 250000) {
        free(SearchBuffer);
        SearchBuffer = malloc(250000 + BufferSize);
        NetDownload(SearchQuery, SearchBuffer, BufferSize + 250000, &BufferSize);
    }
    free(SearchQuery);
    char* FirstPlayerEntry = SearchBuffer;
    
    while (1) {
        FirstPlayerEntry = strstr(FirstPlayerEntry, "<td class=\"\"><a href=\"/player/");
        if (!FirstPlayerEntry)
            break;
        FirstPlayerEntry += strlen("<td class=\"\"><a href=\"/player/");
        
        SearchList->Entries = realloc(SearchList->Entries, sizeof(HLTV_SEARCH_ENTRY) *
            (SearchList->EntryCount + 1));
        PHLTV_SEARCH_ENTRY SearchEntry = &SearchList->Entries[SearchList->EntryCount];
        SearchList->EntryCount++;
        memset(SearchEntry, 0, sizeof(HLTV_SEARCH_ENTRY));
        
        SearchEntry->PlayerId = strtoul(FirstPlayerEntry, NULL, 10);
        FirstPlayerEntry = strstr(FirstPlayerEntry, "title=\"");
        FirstPlayerEntry += strlen("title=\"");
        
        for (int i = 0; i < 63; i++) {
            if (*FirstPlayerEntry == '\"')
                break;
            SearchEntry->Country[i] = *FirstPlayerEntry++;
        }
        
        while (*FirstPlayerEntry != '\'')
            FirstPlayerEntry++;
        FirstPlayerEntry++;
        
        for (int i = 0; i < 63; i++) {
            if (*FirstPlayerEntry == '\'')
                break;
            SearchEntry->PlayerName[i] = *FirstPlayerEntry++;
        }
    }
    
    free(SearchBuffer);
    strcpy(SearchList->Query, Name);
    return SearchList;
}

void HltvDestroySearch(PHLTV_SEARCH_LIST SearchList) {
    if (!SearchList)
        return;
    if (SearchList->Entries)
        free(SearchList->Entries);
    free(SearchList);
    return;
}
