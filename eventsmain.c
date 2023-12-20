//
//  eventsmain.c
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/20/23.
//

#include "hltv/hltv.h"
#include "csv/csv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>6

void emain(void) {
    int EventCount = 0;
    char* InputBuffer = malloc(128);
    char** Events = malloc(sizeof(char*) * 128);
    memset(Events, 0, sizeof(char*) * 128);
    unsigned long* EventIDs = malloc(sizeof(unsigned long) * 128);
    printf("Please enter event IDs, and ':q' to stop.\n");
    
    for (int i = 0; i < 128; i++) {
        fgets(InputBuffer, 128, stdin);
        if (strstr(InputBuffer, ":q")) {
            EventCount = i;
            break;
        }
        
        Events[i] = malloc(strlen(InputBuffer) + 1);
        strcpy(Events[i], InputBuffer);
        EventIDs[i] = strtoul(Events[i], NULL, 10);
    }
    
    HltvStartEventAnalysis();
    
    for (int i = 0; i < EventCount; i++) {
        HltvAnalyzeEvent(EventIDs[i]);
        usleep(1000000);
    }
    
    int EventPlayersCount;
    PHLTV_EVENT_PLAYER EventPlayers = HltvGetEventsPlayerList(
        &EventPlayersCount);
    
    unsigned long Merchants = CsvCreateTable("merchants.csv");
    unsigned long Header = CsvCreateEntry(Merchants, 1);
    CsvEntryAddMember(Merchants, Header, "Player Name");
    CsvEntryAddMember(Merchants, Header, "Matches Played");
    CsvEntryAddMember(Merchants, Header, "Average Rating");
    CsvEntryAddMember(Merchants, Header, "Average Playoffs Rating");
    CsvEntryAddMember(Merchants, Header, "Average Groups Rating");
    CsvEntryAddMember(Merchants, Header, "Playoffs Match Count");
    CsvEntryAddMember(Merchants, Header, "Groups Match Count");
    CsvEntryAddMember(Merchants, Header, "Absolute Change");
    CsvEntryAddMember(Merchants, Header, "Percent Change");
    
    for (int i = 0; i < EventPlayersCount; i++) {
        PHLTV_EVENT_PLAYER ThisPlayer = &EventPlayers[i];
        if (ThisPlayer->MatchCount[1] == 0)
            continue;
        
        char* ThisBuffer = malloc(128);
        unsigned long ThisEntry = CsvCreateEntry(Merchants, 1 + i);
        
        int MatchesPlayed = ThisPlayer->MatchCount[0] + ThisPlayer->MatchCount[1];
        float SumRating = ThisPlayer->PlayerRatingSum[0] + ThisPlayer->PlayerRatingSum[1];
        float AverageRating = SumRating / (float)MatchesPlayed;
        float PlayoffsRating = ThisPlayer->PlayerRatingSum[1] / ThisPlayer->MatchCount[1];
        float GroupsRating = ThisPlayer->PlayerRatingSum[0] / ThisPlayer->MatchCount[0];
        float AbsoluteChange = PlayoffsRating - GroupsRating;
        float PercentChange = PlayoffsRating / GroupsRating;
        
        sprintf(ThisBuffer, "%s", ThisPlayer->PlayerName);
        CsvEntryAddMember(Merchants, ThisEntry, ThisBuffer);
        
        snprintf(ThisBuffer, 128, "%i", MatchesPlayed / 2);
        CsvEntryAddMember(Merchants, ThisEntry, ThisBuffer);
        
        snprintf(ThisBuffer, 128, "%0.2f", AverageRating);
        CsvEntryAddMember(Merchants, ThisEntry, ThisBuffer);
        
        snprintf(ThisBuffer, 128, "%0.2f", PlayoffsRating);
        CsvEntryAddMember(Merchants, ThisEntry, ThisBuffer);
        
        snprintf(ThisBuffer, 128, "%0.2f", GroupsRating);
        CsvEntryAddMember(Merchants, ThisEntry, ThisBuffer);
        
        snprintf(ThisBuffer, 128, "%i", ThisPlayer->MatchCount[1] / 2);
        CsvEntryAddMember(Merchants, ThisEntry, ThisBuffer);
        
        snprintf(ThisBuffer, 128, "%i", ThisPlayer->MatchCount[0] / 2);
        CsvEntryAddMember(Merchants, ThisEntry, ThisBuffer);
        
        snprintf(ThisBuffer, 128, "%0.2f", AbsoluteChange);
        CsvEntryAddMember(Merchants, ThisEntry, ThisBuffer);
        
        snprintf(ThisBuffer, 128, "%0.2f%%", (PercentChange - 1.0f) * 100.f);
        CsvEntryAddMember(Merchants, ThisEntry, ThisBuffer);
        
        free(ThisBuffer);
    }
    
    CsvGenerate(Merchants);
    HltvDestroyEventAnalysis();
    
    for (int i = 0; i < 128; i++) {
        if (Events[i])
            free(Events[i]);
    }
    free(Events);
}
