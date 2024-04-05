/*-------------------------------------------------------------------------
 *
 * blutils.c
 *		Alex index utilities.
 *
 * Portions Copyright (c) 2016-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1990-1993, Regents of the University of California
 *
 * IDENTIFICATION
 *	  contrib/bloom/blutils.c
 *
 *-------------------------------------------------------------------------
 */
extern "C" {
#include "postgres.h"

#include "access/amapi.h"
#include "access/generic_xlog.h"
#include "access/reloptions.h"
#include "catalog/index.h"
#include "commands/vacuum.h"
#include "miscadmin.h"
#include "storage/bufmgr.h"
#include "storage/freespace.h"
#include "storage/indexfsm.h"
#include "storage/lmgr.h"
#include "utils/memutils.h"
PG_FUNCTION_INFO_V1(alhandler);
};

#include "alex.h"

/*
 * Module initialize function: initialize info about Alex relation options.
 */
void
_PG_init(void) {
//    elog(INFO, "==================_PG_init=================");
}


/*
 * Alex handler function: return IndexAmRoutine with access method parameters
 * and callbacks.
 */
Datum
alhandler(PG_FUNCTION_ARGS) {

    IndexAmRoutine *amroutine = makeNode(IndexAmRoutine);

    amroutine->amstrategies = 1;
    amroutine->amsupport = 2;
    amroutine->amoptsprocnum = 2;
    amroutine->amcanorder = false;
    amroutine->amcanorderbyop = false;
    amroutine->amcanbackward = false;
    amroutine->amcanunique = false;
    amroutine->amcanmulticol = true;
    amroutine->amoptionalkey = true;
    amroutine->amsearcharray = false;
    amroutine->amsearchnulls = false;
    amroutine->amstorage = false;
    amroutine->amclusterable = false;
    amroutine->ampredlocks = false;
    amroutine->amcanparallel = false;
    amroutine->amcaninclude = false;
    amroutine->amusemaintenanceworkmem = false;
    amroutine->amparallelvacuumoptions =
            VACUUM_OPTION_PARALLEL_BULKDEL | VACUUM_OPTION_PARALLEL_CLEANUP;
    amroutine->amkeytype = InvalidOid;

    amroutine->ambuild = albuild;
    amroutine->ambuildempty = NULL;
    amroutine->aminsert = alinsert;
    amroutine->ambulkdelete = NULL;
    amroutine->amvacuumcleanup = NULL;
    amroutine->amcanreturn = NULL;
    amroutine->amcostestimate = alcostestimate;
    amroutine->amoptions = NULL;
    amroutine->amproperty = NULL;
    amroutine->ambuildphasename = NULL;
    amroutine->amvalidate = NULL;
    amroutine->amadjustmembers = NULL;
    amroutine->ambeginscan = albeginscan;
    amroutine->amrescan = alrescan;
    amroutine->amgettuple = NULL;
    amroutine->amgetbitmap = algetbitmap;
    amroutine->amendscan = alendscan;
    amroutine->ammarkpos = NULL;
    amroutine->amrestrpos = NULL;
    amroutine->amestimateparallelscan = NULL;
    amroutine->aminitparallelscan = NULL;
    amroutine->amparallelrescan = NULL;

    PG_RETURN_POINTER(amroutine);

}





