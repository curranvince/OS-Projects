/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    int proctmp;
    int pagetmp;
    int old_time;
    int old_page;

    /* initialize static vars on first run */
    if(!initialized){
	    for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	        for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
		        timestamps[proctmp][pagetmp] = 0; 
	        }
	    }
	    initialized = 1;
    }

    /* loop through processes */
    for (proctmp = 0; proctmp < MAXPROCESSES; proctmp++) {
        /* check if process is active, then if pages are swapped in */
        if (q[proctmp].active) { 
            pagetmp = q[proctmp].pc/PAGESIZE; // get current page
            
            if (!q[proctmp].pages[pagetmp] && !pagein(proctmp,pagetmp)) {
                old_time = tick; // set old time to now so anything before gets set as older
                
                /* loop through pages, setting older pages as the one to remove */
                for (int i = 0; i < MAXPROCPAGES; i++) {
                    if (q[proctmp].pages[i] && timestamps[proctmp][i] < old_time) {
                        old_time = timestamps[proctmp][i]; // cache time of oldest page
                        old_page = i;                      // cache oldest page
                    }
                }
                pageout(proctmp, old_page);     // remove page
                break;
            }
            timestamps[proctmp][pagetmp] = tick; // set current proccess to now
        }
    }

    /* advance time for next pageit iteration */
    tick++;
} 
