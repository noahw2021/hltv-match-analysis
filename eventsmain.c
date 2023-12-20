//
//  eventsmain.c
//  hltv-match-analysis
//
//  Created by Noah Wooten on 12/20/23.
//

#include "hltv/hltv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void emain(void) {
    int EventCount = 0;
    char* InputBuffer = malloc(128);
    char** Events = malloc(sizeof(char*) * 128);
    memset(Events, 0, sizeof(char*) * 128);
    unsigned long* EventIDs = malloc(sizeof(unsigned long) * 128);
    printf("Please enter event IDs, and ':q' to stop.\n");
    
    for (int i = 0; i < 128; i++) {
        fgets(InputBuffer, 128, stdin);
        if (strstr(InputBuffer, ":q")) {
            EventCount = i;
            break;
        }
        
        Events[i] = malloc(strlen(InputBuffer) + 1);
        EventIDs[i] = strtoul(Events[i], NULL, 10);
        strcpy(Events[i], InputBuffer);
    }
    
    
    
    for (int i = 0; i < 128; i++) {
        if (Events[i])
            free(Events[i]);
    }
    free(Events);
}
