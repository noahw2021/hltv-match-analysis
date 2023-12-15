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
#include "csv/csv.h"

int main(int argc, char** argv) {
    NetInit();
    CsvInit();
    
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
    unsigned long PlayerId = Selected->PlayerId;
    
    fgetc(stdin);
    printf("Please type a match query string, or blank:\n");
    fgets(InputBuffer, 128, stdin);
    
    
    PHLTV_MATCH_LIST MatchList = HltvGenerateMatchList(PlayerId,
        InputBuffer);
    HltvDestroySearch(SearchResults);
    
    printf("\n%lu matches found.\n", MatchList->MatchCount);
    
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
    
    WORD32 Player = CsvCreateEntry(PlayerTable, 2);
    
    PHLTV_MATCH_LIST PlayerList = MatchList;
    CsvEntryAddMember(PlayerTable, Player, InputBuffer);
    
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
    
    CsvGenerate(PlayerTable);
    
    NetShutdown();
    CsvShutdown();
    
    return 0;
}
