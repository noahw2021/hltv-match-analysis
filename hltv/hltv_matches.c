//
//  hltv_matches.c
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/15/23.
//

#include "hltv.h"
#include "../net/netapi.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

PHLTV_MATCH_LIST HltvGenerateMatchList(unsigned long PlayerId,
    char* QString
) {
    PHLTV_MATCH_LIST MatchList = malloc(sizeof(HLTV_MATCH_LIST));
    memset(MatchList, 0, sizeof(HLTV_MATCH_LIST));
    MatchList->LowestRating = 10.0f;
    
    char* DlBuffer = malloc(512000);
    unsigned long DlBfrSize = 512000;
    char* StatsPage = malloc(512);
    char* QueryString = QString;
    if (strstr(QueryString, "\n"))
        strstr(QueryString, "\n")[0] = 0x00;
    
    snprintf(StatsPage, 512, "https://www.hltv.org/stats/players/matches/"
        "%lu/playa%s", PlayerId, QueryString);
    NetDownload(StatsPage, DlBuffer, DlBfrSize, &DlBfrSize);
    if (DlBfrSize > 512000) {
        free(DlBuffer);
        DlBuffer = malloc(100000 + DlBfrSize);
        NetDownload(StatsPage, DlBuffer, DlBfrSize + 100000, &DlBfrSize);
    }
    
    char* ThisMatch = DlBuffer;
    while (1) {
        ThisMatch = strstr(ThisMatch, "<td><a href=\"/stats/matches/mapstatsid/");
        if (!ThisMatch)
            break;
        ThisMatch += strlen("<td><a href=\"/stats/matches/mapstatsid/");
        
        ThisMatch = strstr(ThisMatch, "/img/static/flags/30x20");
        ThisMatch += strlen("/img/static/flags/30x20");
        
        MatchList->Matches = realloc(MatchList->Matches,
            sizeof(HLTV_MATCH_ENTRY) * (MatchList->MatchCount + 1));
        PHLTV_MATCH_ENTRY ThisEntry = &MatchList->Matches[MatchList->MatchCount];
        MatchList->MatchCount++;
        memset(ThisEntry, 0, sizeof(HLTV_MATCH_ENTRY));
        
        // Find our team name / rounds won
        while (*ThisMatch != '>')
            ThisMatch++;
        ThisMatch++;
        
        for (int i = 0; i < 32; i++) {
            if (*ThisMatch == '<')
                break;
            
            ThisEntry->PlayerTeam[i] = *ThisMatch;
            ThisMatch++;
        }
        
        while (*ThisMatch != '(')
            ThisMatch++;
        ThisMatch++;
        
        char NumBuf[5];
        memset(NumBuf, 0, 5);
        for (int i = 0; i < 5; i++) {
            if (*ThisMatch == ')')
                break;
            
            NumBuf[i] = *ThisMatch;
            ThisMatch++;
        }
        
        ThisEntry->RoundsWon = atoi(NumBuf);
        
        // Find their team name / rounds won
        ThisMatch = strstr(ThisMatch, "/img/static/flags/30x20");
        ThisMatch += strlen("/img/static/flags/30x20");
        while (*ThisMatch != '>')
            ThisMatch++;
        ThisMatch++;
        for (int i = 0; i < 32; i++) {
            if (*ThisMatch == '<')
                break;
            
            ThisEntry->OpponentTeam[i] = *ThisMatch;
            ThisMatch++;
        }
        
        while (*ThisMatch != '(')
            ThisMatch++;
        ThisMatch++;
        
        memset(NumBuf, 0, 5);
        for (int i = 0; i < 5; i++) {
            if (*ThisMatch == ')')
                break;
            
            NumBuf[i] = *ThisMatch;
            ThisMatch++;
        }
        ThisEntry->RoundsLost = atoi(NumBuf);
        
        // Find map
        ThisMatch = strstr(ThisMatch, "<td class=\"statsMapPlayed\">");
        ThisMatch += strlen("<td class=\"statsMapPlayed\">");
        for (int i = 0; i < 8; i++) {
            if (*ThisMatch == '<')
                break;
            
            ThisEntry->Map[i] = *ThisMatch;
            ThisMatch++;
        }
        
        // Find KD
        memset(NumBuf, 0, 5);
        ThisMatch = strstr(ThisMatch, "<td class=\"statsCenterText\">");
        ThisMatch += strlen("<td class=\"statsCenterText\">");
        for (int i = 0; i < 5; i++) {
            if (*ThisMatch == ' ')
                break;
            
            NumBuf[i] = *ThisMatch;
            ThisMatch++;
        }
        ThisMatch += 3;
        ThisEntry->Kills = atoi(NumBuf);
        memset(NumBuf, 0, 5);
        for (int i = 0; i < 5; i++) {
            if (*ThisMatch == '<')
                break;
            
            NumBuf[i] = *ThisMatch;
            ThisMatch++;
        }
        ThisEntry->Death = atoi(NumBuf);
        
        // Find Rating
        memset(NumBuf, 0, 5);
        ThisMatch = strstr(ThisMatch, "<td class=\"match");
        ThisMatch += strlen("<td class=\"match");
        while (*ThisMatch != '>')
            ThisMatch++;
        ThisMatch++;
        for (int i = 0; i < 5; i++) {
            if (*ThisMatch == '<')
                break;
            
            NumBuf[i] = *ThisMatch;
            ThisMatch++;
        }
        ThisEntry->Rating = atof(NumBuf);
        
        if (ThisEntry->RoundsWon > ThisEntry->RoundsLost) {
            ThisEntry->DidTeamWin = 1;
        } else if (ThisEntry->RoundsWon == ThisEntry->RoundsLost) {
            ThisEntry->DidTeamTie = 1;
        }
    }
    
    // Find statistics
    for (int i = 0; i < MatchList->MatchCount; i++) {
        PHLTV_MATCH_ENTRY ThisMatch = &MatchList->Matches[i];
        
        if (ThisMatch->DidTeamWin) {
            MatchList->MatchesWon++;
            MatchList->RatingSumWon += ThisMatch->Rating;
        } else if (ThisMatch->DidTeamTie) {
            MatchList->MatchesTied++;
            MatchList->RatingSumTIe += ThisMatch->Rating;
        } else {
            MatchList->MatchesLost++;
            MatchList->RatingSumLost += ThisMatch->Rating;
        }
        
        MatchList->Rounds += ThisMatch->RoundsWon;
        MatchList->Rounds += ThisMatch->RoundsLost;
        
        MatchList->Kills += ThisMatch->Kills;
        MatchList->Death += ThisMatch->Death;
        
        if (ThisMatch->Rating > MatchList->HighestRating)
            MatchList->HighestRating = ThisMatch->Rating;
        
        if (ThisMatch->Rating < MatchList->LowestRating)
            MatchList->LowestRating = ThisMatch->Rating;
        
        MatchList->RatingSum += ThisMatch->Rating;
    }
    
    // calculate all statistics after data processed
    MatchList->KillsPerRound = ((float)MatchList->Kills /
        (float)MatchList->Rounds);
    MatchList->DeathsPerRound = ((float)MatchList->Death /
        (float)MatchList->Rounds);
    MatchList->AverageRating = MatchList->RatingSum / MatchList->MatchCount;
    MatchList->AverageRatingTie = MatchList->RatingSumTIe / MatchList->MatchesTied;
    MatchList->AverageRatingWin = MatchList->RatingSumWon / MatchList->MatchesWon;
    MatchList->AverageRatingLoss = MatchList->RatingSumLost / MatchList->MatchesLost;
    
    // calculate stdev
    float Starg0 = 0.f;
    for (int i = 0; i < MatchList->MatchCount; i++) {
        PHLTV_MATCH_ENTRY ThisMatch = &MatchList->Matches[i];
        if (ThisMatch->Rating < MatchList->AverageRating)
            Starg0 += pow((ThisMatch->Rating - MatchList->AverageRating), 2);
    }
    
    Starg0 /= MatchList->MatchCount;
    MatchList->StandardDeviation = sqrt(Starg0);
    MatchList->WeightedConsistencyFactor = ((MatchList->AverageRating - 1) /
        (MatchList->StandardDeviation + 1) * 1.4f) + 1;
    
    return MatchList;
}
