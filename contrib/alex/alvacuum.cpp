/*-------------------------------------------------------------------------
 *
 * blvacuum.c
 *		Bloom VACUUM functions.
 *
 * Copyright (c) 2016-2021, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  contrib/bloom/blvacuum.c
 *
 *-------------------------------------------------------------------------
 */
extern "C" {
#include "postgres.h"

#include "access/genam.h"
#include "catalog/storage.h"
#include "commands/vacuum.h"
#include "miscadmin.h"
#include "postmaster/autovacuum.h"
#include "storage/bufmgr.h"
#include "storage/indexfsm.h"
#include "storage/lmgr.h"
};

#include "alex.h"


/*
 * Bulk deletion of all index entries pointing to a set of heap tuples.
 * The set of target tuples is specified via a callback routine that tells
 * whether any given heap tuple (identified by ItemPointer) is being deleted.
 *
 * Result: a palloc'd struct containing statistical info for VACUUM displays.
 */
IndexBulkDeleteResult *
albulkdelete(IndexVacuumInfo *info, IndexBulkDeleteResult *stats,
             IndexBulkDeleteCallback callback, void *callback_state) {
    elog(INFO, "===========albulkdelete===========");
    return NULL;
}

/*
 * Post-VACUUM cleanup.
 *
 * Result: a palloc'd struct containing statistical info for VACUUM displays.
 */
IndexBulkDeleteResult *
alvacuumcleanup(IndexVacuumInfo *info, IndexBulkDeleteResult *stats) {
    elog(INFO, "===========alvacuumcleanup===========");
    return NULL;
}
