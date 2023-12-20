//
//  hltv.h
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/14/23.
//

#ifndef hltv_h
#define hltv_h

typedef struct _HLTV_MATCH_ENTRY {
    char PlayerTeam[32];
    char OpponentTeam[32];
    char Map[8];
    
    int RoundsWon;
    int RoundsLost;
    unsigned char DidTeamWin;
    unsigned char DidTeamTie;
    int Kills, Death;
    
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
    
    // determined thru loop
    int MatchesWon;
    int MatchesLost;
    int MatchesTied;
    int Rounds;
    int Kills, Death;
    float HighestRating;
    float LowestRating;
    float RatingSum;
    float RatingSumWon;
    float RatingSumLost;
    float RatingSumTIe;
    
    // things that must be determined not in loop
    float KillsPerRound;
    float DeathsPerRound;
    float AverageRating;
    float AverageRatingWin;
    float AverageRatingLoss;
    float AverageRatingTie;
    float StandardDeviation;
    float WeightedConsistencyFactor;
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
    char* QString);

void HltvDestroyMatchList(PHLTV_MATCH_LIST MatchList);

typedef struct _HLTV_EVENT_PLAYER {
    char PlayerName[64];
    
    // 0 = groups, 1 = playoffs!
    float PlayerRatingSum[2];
    int MatchCount[2];
}HLTV_EVENT_PLAYER, *PHLTV_EVENT_PLAYER;

void HltvStartEventAnalysis(void);
void HltvAnalyzeEvent(unsigned long EventId);
PHLTV_EVENT_PLAYER HltvGetEventsPlayerList(int* PlayerCount);
void HltvDestroyEventAnalysis(void);

#endif /* hltv_h */
