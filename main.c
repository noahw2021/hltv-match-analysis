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
            printf("Please select a match type: \n");
            printf("1. Majors\n");
            printf("2. Big Events\n");
            printf("3. Online\n");
            printf("4. LAN\n");
            printf("Type: ");
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
            printf("Please enter a date in the format of '2023-11-12'.\n");
            printf("Start Date: ");
            fgets(InputBuffer, 128, stdin);
            strncpy(StartDate, InputBuffer, 32);
        }
        
        printf("Would you like to filter by end date? [Y/N]: ");
        InputBuffer3 = fgetc(stdin);
        if ((InputBuffer3 & ~0x20) == 'Y') {
            printf("Please enter a date in the format of '2023-11-12'.\n");
            printf("End Date: ");
            fgets(InputBuffer, 128, stdin);
            strncpy(EndDate, InputBuffer, 32);
        }
        
        printf("Would you like to filter by ranking? [Y/N]: ");
        InputBuffer3 = fgetc(stdin);
        if ((InputBuffer3 & ~0x20) == 'Y') {
            printf("Please select a ranking filter: \n");
            printf("1. Top 5\n");
            printf("2. Top 10\n");
            printf("3. Top 20\n");
            printf("4. Top 30\n");
            printf("5. Top 50\n");
            printf("Filter: ");
            InputBuffer3 = fgetc(stdin);
            InputBuffer3 -= '0';
            switch (InputBuffer3) {
                case 1:
                    RankingFilter = HLTV_RF_TOP5;
                    break;
                case 2:
                    RankingFilter = HLTV_RF_TOP10;
                    break;
                case 3:
                    RankingFilter = HLTV_RF_TOP20;
                    break;
                case 4:
                    RankingFilter = HLTV_RF_TOP30;
                    break;
                case 5:
                    RankingFilter = HLTV_RF_TOP50;
                    break;
                default:
                    RankingFilter = HLTV_RF_NONE;
                    break;
            }
            
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
