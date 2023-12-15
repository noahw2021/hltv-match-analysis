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
    int MatchType, char* StartDate, char* EndDate, int RankFilter,
    int WinType
) {
    PHLTV_MATCH_LIST MatchList = malloc(sizeof(HLTV_MATCH_LIST));
    memset(MatchList, 0, sizeof(HLTV_MATCH_LIST));
    MatchList->LowestRating = 10.0f;
    
    char* DlBuffer = malloc(512000);
    unsigned long DlBfrSize = 512000;
    char* StatsPage = malloc(512);
    char* QueryString = malloc(512);
    memset(QueryString, 0, 512);
    unsigned char NeedsStrings[2] = {1, 0};
    
    if (MatchType != HLTV_MT_ALL) {
        if (NeedsStrings[0]) {
            strcat(QueryString, "?");
            NeedsStrings[0] = 0;
        }
        if (NeedsStrings[1]) {
            strcat(QueryString, "&");
            NeedsStrings[1] = 0;
        }
        
        switch (MatchType) {
            case HLTV_MT_MAJORS:
                strcat(QueryString, "matchType=Majors");
                break;
            case HLTV_MT_BIGEVNTS:
                strcat(QueryString, "matchType=BigEvents");
                break;
            case HLTV_MT_ONLINE:
                strcat(QueryString, "matchType=BigEvents");
                break;
            case HLTV_MT_LAN:
                strcat(QueryString, "matchType=Lan");
                break;
        }
        
        NeedsStrings[1] = 1;
    }
    
    if (!strstr(StartDate, "NONE")) {
        if (NeedsStrings[0]) {
            strcat(QueryString, "?");
            NeedsStrings[0] = 0;
        }
        if (NeedsStrings[1]) {
            strcat(QueryString, "&");
            NeedsStrings[1] = 0;
        }
        
        strcat(QueryString, "startDate=");
        strcat(QueryString, StartDate);
        
        if (strstr(EndDate, "NONE")) {
            time_t CurTime;
            time(&CurTime);
            struct tm *LocalTime;
            LocalTime = localtime(&CurTime);
            
            char _EndDate[32];
            snprintf(_EndDate, 32, "%i-%i-%i", LocalTime->tm_year,
                LocalTime->tm_mon + 1, LocalTime->tm_mday + 1);
            
            strcat(QueryString, "&endDate=");
            strcat(QueryString, _EndDate);
        }
        
        NeedsStrings[1] = 1;
    }
    
    if (!strstr(EndDate, "NONE")) {
        if (NeedsStrings[0]) {
            strcat(QueryString, "?");
            NeedsStrings[0] = 0;
        }
        if (NeedsStrings[1]) {
            strcat(QueryString, "&");
            NeedsStrings[1] = 0;
        }
        
        strcat(QueryString, "endDate=");
        strcat(QueryString, EndDate);
        
        if (!strstr(StartDate, "NONE")) {
            time_t CurTime;
            time(&CurTime);
            struct tm *LocalTime;
            LocalTime = localtime(&CurTime);
            
            char _StartDate[32];
            snprintf(_StartDate, 32, "%i-%i-%i", LocalTime->tm_year,
                LocalTime->tm_mon + 1, LocalTime->tm_mday + 1);
            
            strcat(QueryString, "&startDate=");
            strcat(QueryString, _StartDate);
        }
        
        NeedsStrings[1] = 1;
    }
    
    if (RankFilter != HLTV_RF_NONE) {
        if (NeedsStrings[0]) {
            strcat(QueryString, "?");
            NeedsStrings[0] = 0;
        }
        if (NeedsStrings[1]) {
            strcat(QueryString, "&");
            NeedsStrings[1] = 0;
        }
        
        strcat(QueryString, "rankingFilter=");
        
        switch (RankFilter) {
            case HLTV_RF_TOP5:
                strcat(QueryString, "Top5");
                break;
            case HLTV_RF_TOP10:
                strcat(QueryString, "Top10");
                break;
            case HLTV_RF_TOP20:
                strcat(QueryString, "Top20");
                break;
            case HLTV_RF_TOP30:
                strcat(QueryString, "Top30");
                break;
            case HLTV_RF_TOP50:
                strcat(QueryString, "Top50");
                break;
        }
        
        NeedsStrings[1] = 1;
    }
    
    snprintf(StatsPage, 512, "https://www.hltv.org/stats/players/matches/"
        "%lu/playa%s", PlayerId, QueryString);
    free(QueryString);
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
        Starg0 += pow((ThisMatch->Rating - MatchList->AverageRating), 2);
    }
    Starg0 /= MatchList->MatchCount;
    MatchList->StandardDeviation = sqrt(Starg0);
    MatchList->WeightedConsistencyFactor =
        ((pow(MatchList->AverageRating, 1.4f) +
        ((1.0f - MatchList->StandardDeviation) * 2.f)) / 2.f);
    
    /*
     The weighted consistency factor is a measure of
     a players consistency, with a heavy bias towards consistent
     1.0+ players.
     
     As a base for the unit, a 1.0 WCF is equivillent to a player
     who has an 100% 1.0 rating.
     
     examples:
     2.00 Rating,  50% Consistency: 1.32 Rating
     1.30 Rating,  85% Consistency: 1.23 Rating
     1.15 Rating, 100% Consistency: 1.21 Rating
     1.00 Rating, 100% Consistency: 1.00 Rating
     0.95 Rating, 100% Consistency: 0.93 Rating
     0.95 Rating,  95% Consistency: 0.88 Rating
     
     */
    
    return MatchList;
}
