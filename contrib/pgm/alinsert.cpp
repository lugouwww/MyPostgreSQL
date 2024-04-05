/*-------------------------------------------------------------------------
 *
 * blinsert.c
 *		Bloom index build and insert functions.
 *
 * Copyright (c) 2016-2021, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  contrib/bloom/blinsert.c
 *
 *-------------------------------------------------------------------------
 */
extern "C" {
#include "postgres.h"

#include "access/genam.h"
#include "access/generic_xlog.h"
#include "access/tableam.h"
#include "catalog/index.h"
#include "miscadmin.h"
#include "storage/bufmgr.h"
#include "storage/indexfsm.h"
#include "storage/smgr.h"
#include "utils/memutils.h"
#include "utils/rel.h"

PG_MODULE_MAGIC;
};

#include "alex.h"
#include "raw_alex/src/core/alex.h"
#include "raw_alex/src/core/global.h"
#include <iostream>


/*
 * Build a new alex index.
 */
IndexBuildResult *albuild(Relation heap, Relation index, IndexInfo *indexInfo) {

//    elog(INFO, "==================albuild=================");
//    std::cout << "===========albuild===========" << std::endl;

    IndexBuildResult *result = (IndexBuildResult *) palloc(sizeof(IndexBuildResult));

    alex::Alex<int, ItemPointerData> idx(heap, index, indexInfo, result);

    return result;

}

/*
 * Build an empty bloom index in the initialization fork.
 */
void albuildempty(Relation index) {
    elog(INFO, "===========albuildempty===========");
}

/*
 * Insert new tuple to the bloom index.
 */
bool alinsert(Relation index, Datum *values, bool *isnull,
              ItemPointer ht_ctid, Relation heapRel,
              IndexUniqueCheck checkUnique,
              bool indexUnchanged,
              IndexInfo *indexInfo) {
//    elog(INFO, "===========alinsert===========");
//    std::cout << "===========alinsert===========" << std::endl;

    return false;
}

/*
 * Begin scan of bloom index.
 */
IndexScanDesc
albeginscan(Relation r, int nkeys, int norderbys) {

//    elog(INFO, "===========albeginscan===========");
//    std::cout << "===========albeginscan===========" << std::endl;

    IndexScanDesc scan = RelationGetIndexScan(r, nkeys, norderbys);

    scan->opaque = nullptr;

    return scan;

}

/*
 * Rescan a bloom index.
 */
void
alrescan(IndexScanDesc scan, ScanKey scankey, int nscankeys,
         ScanKey orderbys, int norderbys) {

//    elog(INFO, "===========alrescan===========");
//    std::cout << "===========alrescan===========" << std::endl;

    if (scankey && scan->numberOfKeys > 0) {
        memmove(scan->keyData, scankey, scan->numberOfKeys * sizeof(ScanKeyData));
    }

}

/*
 * End scan of bloom index.
 */
void
alendscan(IndexScanDesc scan) {
//    elog(INFO, "===========alendscan===========");
//    std::cout << "===========alendscan===========" << std::endl;

}

/*
 * Insert all matching tuples into a bitmap.
 */
int64 algetbitmap(IndexScanDesc scan, TIDBitmap *tbm) {

//    elog(INFO, "===========algetbitmap===========");
//    std::cout << "===========algetbitmap===========" << std::endl;

    int64 ntids = 0;
    alex::Alex<int, ItemPointerData> idx(scan, tbm, &ntids);

//    std::cout << "ntids: " << ntids << std::endl;

    return ntids;

}
