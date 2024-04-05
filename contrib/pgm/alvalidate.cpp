/*-------------------------------------------------------------------------
 *
 * blvalidate.c
 *	  Opclass validator for bloom.
 *
 * Copyright (c) 2016-2021, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  contrib/bloom/blvalidate.c
 *
 *-------------------------------------------------------------------------
 */
extern "C" {
#include "postgres.h"

#include "access/amvalidate.h"
#include "access/htup_details.h"
#include "catalog/pg_amop.h"
#include "catalog/pg_amproc.h"
#include "catalog/pg_opclass.h"
#include "catalog/pg_opfamily.h"
#include "catalog/pg_type.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"
#include "utils/regproc.h"
#include "utils/syscache.h"
};

#include "alex.h"


/*
 * Validator for a bloom opclass.
 */
bool
alvalidate(Oid opclassoid) {
    elog(INFO, "===========alvalidate===========");
    return false;
}
