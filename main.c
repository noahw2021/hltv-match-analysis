//
//  main.c
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/14/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hltv/hltv.h"
#include "net/netapi.h"

int main(int argc, char** argv) {
    NetInit();
    
    printf("Please enter a player name: ");
    char* InputBuffer = malloc(128);
    fgets(InputBuffer, 128, stdin);
    if (strstr(InputBuffer, "\n")) {
        *(strstr(InputBuffer, "\n")) = 0x00;
    }
    
    PHLTV_SEARCH_LIST SearchResults = HltvSearchPlayer(InputBuffer);
    printf("Found %lu results.\n", SearchResults->EntryCount);
    for (int i = 0; i < SearchResults->EntryCount; i++) {
        PHLTV_SEARCH_ENTRY ThisEntry = &SearchResults->Entries[i];
        printf("%i. '%s' of %s\n", i + 1, ThisEntry->PlayerName, ThisEntry->Country);
    }
    
    printf("Please select which player: ");
    unsigned long InputBuffer2;
    scanf("%lu", &InputBuffer2);
    unsigned long RealSector = InputBuffer2 - 1;
    PHLTV_SEARCH_ENTRY Selected = &SearchResults->Entries[RealSector];
    
    printf("Would you like to apply any filters? [Y/N]: ");
    char InputBuffer3 = fgetc(stdin);
    
    // Filter setup
    unsigned long PlayerId;
    int MatchType;
    char StartDate[32];
    char EndDate[32];
    int RankingFilter;
    int WinType;
    
    if ((InputBuffer3 & ~0x20) == 'Y') {
        printf("Would you like to filter by Match Type? [Y/N]: ");
        InputBuffer3 = fgetc(stdin);
        if ((InputBuffer3 & ~0x20) == 'Y') {
            printf("Please select a match type: ");
            printf("1. Majors\n");
            printf("2. Big Events\n");
            printf("3. Online\n");
            printf("4. LAN\n");
            InputBuffer3 = fgetc(stdin);
            InputBuffer3 -= '0';
            
            switch (InputBuffer3) {
                case 1:
                    MatchType = HLTV_MT_MAJORS;
                    break;
                case 2:
                    MatchType = HLTV_MT_BIGEVNTS;
                    break;
                case 3:
                    MatchType = HLTV_MT_ONLINE;
                    break;
                case 4:
                    MatchType = HLTV_MT_LAN;
                    break;
                default:
                    MatchType = HLTV_MT_ALL;
                    break;
            }
        } else {
            MatchType = HLTV_MT_ALL;
        }
        
        printf("Would you like to filter by start date? [Y/N]: ");
        InputBuffer3 = fgetc(stdin);
        if ((InputBuffer3 & ~0x20) == 'Y') {
            
        }
    } else {
        PlayerId = Selected->PlayerId;
        MatchType = HLTV_MT_ALL;
        strcpy(StartDate, "NONE");
        strcpy(EndDate, "NONE");
        RankingFilter = HLTV_RF_NONE;
        WinType = HLTV_WT_ALL;
    }
    
    return 0;
}
