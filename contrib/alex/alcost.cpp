/*-------------------------------------------------------------------------
 *
 * blcost.c
 *		Cost estimate function for bloom indexes.
 *
 * Copyright (c) 2016-2021, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  contrib/bloom/blcost.c
 *
 *-------------------------------------------------------------------------
 */
extern "C" {
#include "postgres.h"
#include "fmgr.h"
#include "utils/selfuncs.h"
}

#include "alex.h"
#include <iostream>

/*
 * Estimate cost of alex index scan.
 */
void
alcostestimate(PlannerInfo *root, IndexPath *path, double loop_count,
               Cost *indexStartupCost, Cost *indexTotalCost,
               Selectivity *indexSelectivity, double *indexCorrelation,
               double *indexPages) {

//    elog(INFO, "===========alcostestimate===========");
//    std::cout << "===========alcostestimate===========" << std::endl;

}
