//
//  main.c
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/14/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hltv/hltv.h"
#include "net/netapi.h"
#include "csv/csv.h"

int main(int argc, char** argv) {
    NetInit();
    CsvInit();
    
    int PlayerCount = 0;
    char* InputBuffer = malloc(128);
    char** Players = malloc(sizeof(char*) * 128);
    unsigned long* PlayerIds = malloc(sizeof(unsigned long) * 128);
    printf("Please enter player names, and ':q' to stop.\n");
    
    FILE* Playalist = fopen("playerlist.txt", "r");
    if (!Playalist)
        Playalist = stdin;
    
    for (int i = 0; i < 128; i++) {
        fgets(InputBuffer, 128, Playalist);
        if (strstr(InputBuffer, ":q")) {
            PlayerCount = i;
            break;
        }
        
        Players[i] = malloc(strlen(InputBuffer) + 1);
        strcpy(Players[i], InputBuffer);
    }
    
    for (int i = 0; i < PlayerCount; i++) {
        if (strstr(Players[i], "\n"))
            strstr(Players[i], "\n")[0] = 0x00;
        PHLTV_SEARCH_LIST SearchList = HltvSearchPlayer(Players[i]);
        usleep(1000000 + (rand() % 100000));
        if (SearchList->EntryCount == 0) {
            printf("No player with name '%s' found.\n", Players[i]);
            for (int j = 0; j < PlayerCount; j++)
                free(Players[j]);
            free(Players);
            free(InputBuffer);
            free(PlayerIds);
            HltvDestroySearch(SearchList);
            NetShutdown();
            CsvShutdown();
            return - 1;
        } else if (SearchList->EntryCount == 1) {
            PlayerIds[i] = SearchList->Entries[0].PlayerId;
            strcpy(Players[i], SearchList->Entries[0].PlayerName);
            HltvDestroySearch(SearchList);
            printf("Found player: %s\n", Players[i]);
        } else {
            printf("%lu players with name '%s' were found, please select one: \n", SearchList->EntryCount, Players[i]);
            for (int j = 0; j < SearchList->EntryCount; j++)
                printf("%i. %s of %s\n", j + 1, SearchList->Entries[j].PlayerName,
                    SearchList->Entries[j].Country);
            printf("\nPlayer: ");
            int Selector;
            scanf("%i", &Selector);
            PlayerIds[i] = SearchList->Entries[Selector - 1].PlayerId;
            strcpy(Players[i], SearchList->Entries[Selector - 1].PlayerName);
            HltvDestroySearch(SearchList);
            usleep(rand() % 100000);
        }
    }
    
    fgetc(stdin);
    printf("Please type a match query string, or blank:\n");
    fgets(InputBuffer, 128, stdin);
    
    WORD32 PlayerTable = CsvCreateTable("player.csv");
    WORD32 Header = CsvCreateEntry(PlayerTable, 1);
    CsvEntryAddMember(PlayerTable, Header, "Player Name");
    CsvEntryAddMember(PlayerTable, Header, "Matches Count");
    CsvEntryAddMember(PlayerTable, Header, "Matches Won");
    CsvEntryAddMember(PlayerTable, Header, "Matches Lost");
    CsvEntryAddMember(PlayerTable, Header, "Matches Tied");
    CsvEntryAddMember(PlayerTable, Header, "Rounds");
    CsvEntryAddMember(PlayerTable, Header, "Kills");
    CsvEntryAddMember(PlayerTable, Header, "Deaths");
    CsvEntryAddMember(PlayerTable, Header, "Lowest Rating");
    CsvEntryAddMember(PlayerTable, Header, "Highest Rating");
    CsvEntryAddMember(PlayerTable, Header, "Kills Per Round");
    CsvEntryAddMember(PlayerTable, Header, "Deaths Per Round");
    CsvEntryAddMember(PlayerTable, Header, "Average Rating");
    CsvEntryAddMember(PlayerTable, Header, "Average Rating (Loss)");
    CsvEntryAddMember(PlayerTable, Header, "Average Rating (Win)");
    CsvEntryAddMember(PlayerTable, Header, "Average Rating (Tie)");
    CsvEntryAddMember(PlayerTable, Header, "Standard Deviation");
    CsvEntryAddMember(PlayerTable, Header, "Consistency Factor");
    
    for (int i = 0; i < PlayerCount; i++) {
        usleep(1000000);
        PHLTV_MATCH_LIST MatchList = HltvGenerateMatchList(PlayerIds[i],
            InputBuffer);
        
        printf("%lu matches found.\n", MatchList->MatchCount);
        
        WORD32 Player = CsvCreateEntry(PlayerTable, 2 + i);
        
        PHLTV_MATCH_LIST PlayerList = MatchList;
        CsvEntryAddMember(PlayerTable, Player, Players[i]);
        
        char* Bfr = malloc(128);
        
        sprintf(Bfr, "%lu", PlayerList->MatchCount);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%d", PlayerList->MatchesWon);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%d", PlayerList->MatchesLost);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%d", PlayerList->MatchesTied);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%d", PlayerList->Rounds);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%d", PlayerList->Kills);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%d", PlayerList->Death);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%.2f", PlayerList->LowestRating);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%.2f", PlayerList->HighestRating);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%.2f", PlayerList->KillsPerRound);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%.2f", PlayerList->DeathsPerRound);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%.2f", PlayerList->AverageRating);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%.2f", PlayerList->AverageRatingLoss);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%.2f", PlayerList->AverageRatingWin);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%.2f", PlayerList->AverageRatingTie);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%.2f", PlayerList->StandardDeviation);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
        sprintf(Bfr, "%.2f", PlayerList->WeightedConsistencyFactor);
        CsvEntryAddMember(PlayerTable, Player, Bfr);
    }
    
    CsvGenerate(PlayerTable);
    
    NetShutdown();
    CsvShutdown();
    
    return 0;
}
