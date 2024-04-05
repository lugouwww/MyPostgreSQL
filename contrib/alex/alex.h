/*-------------------------------------------------------------------------
 *
 * alex.h
 *	  Header for alex index.
 *
 * Copyright (c) 2016-2021, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  contrib/alex/alex.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef _ALEX_H_
#define _ALEX_H_

extern "C" {
#include "access/amapi.h"
#include "access/generic_xlog.h"
#include "access/itup.h"
#include "access/xlog.h"
#include "fmgr.h"
#include "nodes/pathnodes.h"
};

/* index access method interface functions */
extern "C" {
bool alinsert(Relation index, Datum *values, bool *isnull,
              ItemPointer ht_ctid, Relation heapRel,
              IndexUniqueCheck checkUnique,
              bool indexUnchanged,
              struct IndexInfo *indexInfo);
IndexScanDesc albeginscan(Relation r, int nkeys, int norderbys);
int64 algetbitmap(IndexScanDesc scan, TIDBitmap *tbm);
void alrescan(IndexScanDesc scan, ScanKey scankey, int nscankeys,
              ScanKey orderbys, int norderbys);
void alendscan(IndexScanDesc scan);
IndexBuildResult *albuild(Relation heap, Relation index,
                          struct IndexInfo *indexInfo);
bytea *aloptions(Datum reloptions, bool validate);
void alcostestimate(PlannerInfo *root, IndexPath *path,
                    double loop_count, Cost *indexStartupCost,
                    Cost *indexTotalCost, Selectivity *indexSelectivity,
                    double *indexCorrelation, double *indexPages);

}

#endif
