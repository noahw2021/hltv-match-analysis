//
//  csv_manip.c
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/15/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv.h"

WORD32 CsvCreateTable(const char* TableName) {
    if (!CsvCtx)
        return 0xFFFFFFFF;
    
    if (!CsvCtx->Tables) {
        CsvCtx->Tables = malloc(sizeof(CSV_TABLE));
    } else {
        CsvCtx->Tables = realloc(CsvCtx->Tables,
            sizeof(CSV_TABLE) * (CsvCtx->TableCount + 1));
    }
    
    PCSV_TABLE NewTable = &CsvCtx->Tables[CsvCtx->TableCount++];
    memset(NewTable, 0, sizeof(CSV_TABLE));
    
    NewTable->TableFile = fopen(TableName, "wb");
    if (!NewTable->TableFile)
        return 0xFFFFFFFF;
    
    return CsvCtx->TableCount - 1;
}

WORD32 CsvCreateEntry(WORD32 Table, WORD32 LineNumber) {
    if (!CsvCtx)
        return 0xFFFFFFFF;
    if (Table == 0xFFFFFFFF ||
        Table >= CsvCtx->TableCount
        ) {
        return 0xFFFFFFFF;
    }
    
    PCSV_TABLE ThisTable = &CsvCtx->Tables[Table];
    
    if (!ThisTable->Entries) {
        ThisTable->Entries = malloc(sizeof(CSV_TABLE));
    } else {
        ThisTable->Entries = realloc(ThisTable->Entries,
            sizeof(CSV_TABLE) * (ThisTable->EntryCount + 1));
    }
    
    PCSV_ENTRY NewEntry = &ThisTable->Entries[ThisTable->EntryCount++];
    memset(NewEntry, 0, sizeof(CSV_ENTRY));
    NewEntry->LineId = LineNumber;
    
    return ThisTable->EntryCount - 1;
}

void CsvEntryAddMember(WORD32 Table, WORD32 Entry, char* Field) {
    if (!CsvCtx)
        return;
    if (Table == 0xFFFFFFFF ||
        Table >= CsvCtx->TableCount
        ) {
        return;
    }
    
    PCSV_TABLE ThisTable = &CsvCtx->Tables[Table];
    if (Entry >= ThisTable->EntryCount)
        return;
    
    PCSV_ENTRY ThisEntry = &ThisTable->Entries[Entry];
    if (!ThisEntry->Values) {
        ThisEntry->Values = malloc(sizeof(char*));
    } else {
        ThisEntry->Values = realloc(ThisEntry->Values,
            sizeof(char*) * (ThisEntry->ValueCount + 1));
    }
    
    char** ValuePtr = &ThisEntry->Values[ThisEntry->ValueCount++];
    *ValuePtr = malloc(strlen(Field) + 1);
    strcpy(*ValuePtr, Field);
    
    return;
}
