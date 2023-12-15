//
//  csv.h
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/15/23.
//

#ifndef csv_h
#define csv_h

#include <stdio.h>

typedef unsigned long WORD32;

void CsvInit(void);
void CsvShutdown(void);

typedef struct _CSV_ENTRY {
    WORD32 ValueCount;
    char** Values;
    WORD32 LineId;
}CSV_ENTRY, *PCSV_ENTRY;

typedef struct _CSV_TABLE {
    FILE* TableFile;
    WORD32 LineCount;
    
    PCSV_ENTRY Entries;
    WORD32 EntryCount;
}CSV_TABLE, *PCSV_TABLE;

typedef struct _CSV_CTX {
    PCSV_TABLE Tables;
    WORD32 TableCount;
}CSV_CTX, *PCSV_CTX;
extern PCSV_CTX CsvCtx;

WORD32   CsvCreateTable(const char* TableName);
WORD32 CsvCreateEntry(WORD32 Table, WORD32 LineNumber);
void   CsvEntryAddMember(WORD32 Table, WORD32 Entry, char* Field);
void   CsvGenerate(WORD32 Table);

#endif /* csv_h */
