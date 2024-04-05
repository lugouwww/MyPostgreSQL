/*-------------------------------------------------------------------------
 *
 * blscan.c
 *		Bloom index scan functions.
 *
 * Copyright (c) 2016-2021, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  contrib/bloom/blscan.c
 *
 *-------------------------------------------------------------------------
 */
extern "C" {
#include "postgres.h"

#include "access/relscan.h"
#include "miscadmin.h"
#include "pgstat.h"
#include "storage/bufmgr.h"
#include "storage/lmgr.h"
#include "utils/memutils.h"
#include "utils/rel.h"
};

#include "alex.h"
#include <iostream>
