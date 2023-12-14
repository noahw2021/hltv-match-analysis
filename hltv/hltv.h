//
//  hltv.h
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/14/23.
//

#ifndef hltv_h
#define hltv_h

typedef struct _HLTV_MATCH_ENTRY {
    unsigned long MatchId;
    
    char Date[32];
    char PlayerTeam[32];
    char OpponentTeam[32];
    char Map[8];
    
    int Kills, Death;
    int KDDiff;
    
    float Rating;
}HLTV_MATCH_ENTRY, *PHLTV_MATCH_ENTRY;

#define HLTV_MT_MAJORS   0x01
#define HLTV_MT_BIGEVNTS 0x02
#define HLTV_MT_ONLINE   0x04
#define HLTV_MT_LAN      0x08
#define HLTV_MT_ALL      0x0F

#define HLTV_RF_TOP5   0x01
#define HLTV_RF_TOP10  0x02
#define HLTV_RF_TOP20  0x03
#define HLTV_RF_TOP30  0x04
#define HLTV_RF_TOP50  0x05
#define HLTV_RF_NONE   0x06

#define HLTV_WT_WIN  0x01
#define HLTV_WT_LOSE 0x02
#define HLTV_WT_TIE  0x04
#define HLTV_WT_ALL  0x07

typedef struct _HLTV_MATCH_LIST {
    unsigned long PlayerId;
    int MatchType;
    char StartDate[32];
    char EndDate[32];
    int RankingFilter;
    int WinType;
    
    PHLTV_MATCH_ENTRY Matches;
    unsigned long MatchCount;
    
    int MatchesWon;
    int MatchesLost;
    
    int Rounds;
    float KillsPerRound;
    float DeathsPerRound;
    int Kills, Death;
    float AverageRating;
    float AverageRatingWin;
    float AverageRatingLoss;
    float HighestRating;
    float LowestRating;
    float StandardDeviation;
    float WeightedConsistencyFactor;
    
    float Median;
    float Q1;
    float Q3;
}HLTV_MATCH_LIST, *PHLTV_MATCH_LIST;

typedef struct _HLTV_SEARCH_ENTRY {
    char PlayerName[64];
    char Country[64];
    unsigned long PlayerId;
}HLTV_SEARCH_ENTRY, *PHLTV_SEARCH_ENTRY;

typedef struct _HLTV_SEARCH_LIST {
    char Query[32];
    PHLTV_SEARCH_ENTRY Entries;
    unsigned long EntryCount;
}HLTV_SEARCH_LIST, *PHLTV_SEARCH_LIST;

PHLTV_SEARCH_LIST HltvSearchPlayer(const char* Name);
void HltvDestroySearch(PHLTV_SEARCH_LIST SearchList);

PHLTV_MATCH_LIST HltvGenerateMatchList(unsigned long PlayerId,
    int MatchType, char* StartDate, char* EndDate, int RankFilter,
    int WinType);

void HltvDestroyMatchList(PHLTV_MATCH_LIST MatchList);

#endif /* hltv_h */
