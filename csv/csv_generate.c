//
//  csv_generate.c
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/15/23.
//

#include <stdio.h>
#include "csv.h"

void CsvGenerate(WORD32 Table) {
    if (!CsvCtx)
        return;
    if (Table == 0xFFFFFFFF ||
        Table >= CsvCtx->TableCount
    ) {
        return;
    }
    
    PCSV_TABLE ThisTable = &CsvCtx->Tables[Table];
    if (!ThisTable->TableFile)
        return;
    
    for (int i = 0; i < ThisTable->EntryCount; i++) {
        PCSV_ENTRY ThisEntry = &ThisTable->Entries[i];
        
        for (int v = 0; v < ThisEntry->ValueCount; v++) {
            fputs(ThisEntry->Values[v], ThisTable->TableFile);
            if (v != (ThisEntry->ValueCount - 1))
                fputs(",", ThisTable->TableFile);
        }
        
        fputs("\n", ThisTable->TableFile);
    }
    
    fflush(ThisTable->TableFile);
    return;
}
