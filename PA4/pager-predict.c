/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains a predictive pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

/* struct to hold data for each process, namely the transition matrices */
typedef struct Transitions {
    double probability[MAXPROCPAGES][MAXPROCPAGES]; // chance to jump from page i to page j
    int between[MAXPROCPAGES][MAXPROCPAGES];        // num transitions made between pages
    int total[MAXPROCPAGES];                        // total transitions made by each page
    int beingpaged[MAXPROCPAGES];                   // pages which are currently paging
    int last_page;                                  // last page process was on
} Transitions;

/* initialize probabilities, totals, etc for Transitions */
void init_transition(Transitions* transition) {
    for (int i = 0; i < MAXPROCPAGES; i++) {
        transition->total[i] = 0;
        transition->beingpaged[i] = 0;
        for (int j = 0; j < MAXPROCPAGES; j++) {
            transition->probability[i][j] = 0;
            transition->between[i][j] = 0;
        }
    }

    transition->last_page = -1;
}

/* gets page with highest prob of being next given a page number */ 
int get_next(Transitions transition, int pageNumber) {
    int maxIndex = (!pageNumber) ? 1 : 0;
    double max = transition.probability[pageNumber][0];
    for (int i = 1; i < MAXPROCPAGES; i++) {
        if (transition.probability[pageNumber][i] > max && i != pageNumber) {
            max = transition.probability[pageNumber][i];
            maxIndex = i;
        }
    }
    return maxIndex;
}

/* Predictive pageit algorithm using dynamic transition matrices */
void pageit(Pentry q[MAXPROCESSES]) { 
    /* Local vars */
    int proctmp;
    int pagetmp;

    /* Static vars */
    static int initialized = 0;
    static Transitions transitions[MAXPROCESSES];
    
    /* Initialize transisiton matrices */
    if (!initialized) {
        for (int i = 0; i < MAXPROCESSES; i++) {
            init_transition(&transitions[i]);
        }
        initialized = 1;
    }

    /* loop through processes */
    for (proctmp = 0; proctmp < MAXPROCESSES; proctmp++) {
        /* check if process is active */
        if (q[proctmp].active) {
            /* init predictions w/ 0,9 & set current page */
            int predictions[MAXPROCPAGES] = { 1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0 };
            pagetmp = q[proctmp].pc / PAGESIZE;
        
            /* add this page & next to prediction */
            predictions[pagetmp] = 1;
            predictions[pagetmp+1] = 1;

            /* if this isnt the first page, update the transition matrix */
            if (transitions[proctmp].last_page != -1) {
                /* update probabilities */
                transitions[proctmp].total[pagetmp]++;
                transitions[proctmp].between[transitions[proctmp].last_page][pagetmp]++;   
                transitions[proctmp].probability[transitions[proctmp].last_page][pagetmp] = (double)transitions[proctmp].between[transitions[proctmp].last_page][pagetmp] / (double)transitions[proctmp].total[pagetmp];
                
                /* add page w/ highest prob of being next to prediction */ 
                predictions[get_next(transitions[proctmp], pagetmp)] = 1;
            }

            /* page in predictions, and out everything else */
            for (int i = 0; i < MAXPROCPAGES; i++) {
                if (predictions[i] && !q[proctmp].pages[i]) pagein(proctmp, i);
                else if (!predictions[i] && q[proctmp].pages[i]) pageout(proctmp, i);
            }

            /* cache this page as the last one */
            transitions[proctmp].last_page = pagetmp;
        }
    }
} 