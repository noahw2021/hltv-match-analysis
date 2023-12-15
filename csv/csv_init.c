//
//  csv_init.c
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/15/23.
//

#include "csv.h"
#include <stdlib.h>
#include <string.h>
PCSV_CTX CsvCtx;

void CsvInit(void) {
    CsvCtx = malloc(sizeof(CSV_CTX));
    memset(CsvCtx, 0, sizeof(CSV_CTX));
    
    return;
}

void CsvShutdown(void) {
    if (!CsvCtx)
        return;
    
    for (int i = 0; i < CsvCtx->TableCount; i++) {
        PCSV_TABLE ThisTable = &CsvCtx->Tables[i];
        
        fclose(ThisTable->TableFile);
        
        for (int e = 0; e < ThisTable->EntryCount; e++) {
            PCSV_ENTRY ThisEntry = &ThisTable->Entries[e];
            
            for (int v = 0; v < ThisEntry->ValueCount; v++)
                free(ThisEntry->Values[v]);
            free(ThisEntry->Values);
        }
        
        free(ThisTable->Entries);
    }
    
    if (CsvCtx->Tables)
        free(CsvCtx->Tables);
    free(CsvCtx);
    
    return;
}
