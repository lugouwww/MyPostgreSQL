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
#include "postgres.h"

#include "access/relscan.h"
#include "bloom.h"
#include "miscadmin.h"
#include "pgstat.h"
#include "storage/bufmgr.h"
#include "storage/lmgr.h"
#include "utils/memutils.h"
#include "utils/rel.h"

/*
 * Begin scan of bloom index.
 */
// Prepare for an index scan. The nkeys and norderbys parameters indicate the number of quals
// and ordering operators that will be used in the scan; these may be useful for space allocation purposes.
// Note that the actual values of the scan keys aren't provided yet. The result must be a palloc'd struct.
// For implementation reasons the index access method must create this struct by calling RelationGetIndexScan().
// In most cases ambeginscan does little beyond making that call and perhaps acquiring locks;
// the interesting parts of index-scan startup are in amrescan.
// nkeys指的是where子句后面条件语句的个数，norderbys指的是order by的个数
IndexScanDesc
blbeginscan(Relation r, int nkeys, int norderbys)
{
    elog(INFO, "==================blbeginscan=================");
	IndexScanDesc scan;
	BloomScanOpaque so;

	scan = RelationGetIndexScan(r, nkeys, norderbys);

	so = (BloomScanOpaque) palloc(sizeof(BloomScanOpaqueData));
	initBloomState(&so->state, scan->indexRelation);
	so->sign = NULL;

	scan->opaque = so;

	return scan;
}

/*
 * Rescan a bloom index.
 */
// Start or restart an index scan, possibly with new scan keys.
// (To restart using previously-passed keys, NULL is passed for keys and/or orderbys.)
// Note that it is not allowed for the number of keys or order-by operators to be larger than what was passed to ambeginscan.
// In practice the restart feature is used when a new outer tuple is
// selected by a nested-loop join and so a new key comparison value is needed,
// but the scan key structure remains the same.
void
blrescan(IndexScanDesc scan, ScanKey scankey, int nscankeys,
		 ScanKey orderbys, int norderbys)
{
    elog(INFO, "==================blrescan=================");
	BloomScanOpaque so = (BloomScanOpaque) scan->opaque;

	if (so->sign)
		pfree(so->sign);
	so->sign = NULL;

	if (scankey && scan->numberOfKeys > 0)
	{
		memmove(scan->keyData, scankey,
				scan->numberOfKeys * sizeof(ScanKeyData));
	}
}

/*
 * End scan of bloom index.
 */
// End a scan and release resources.
// The scan struct itself should not be freed,
// but any locks or pins taken internally by the access method must be released,
// as well as any other memory allocated by ambeginscan and other scan-related functions.
void
blendscan(IndexScanDesc scan)
{
    elog(INFO, "==================blendscan=================");
	BloomScanOpaque so = (BloomScanOpaque) scan->opaque;

	if (so->sign)
		pfree(so->sign);
	so->sign = NULL;
}

/*
 * Insert all matching tuples into a bitmap.
 */
// Fetch all tuples in the given scan and add them to the caller-supplied TIDBitmap
// (that is, OR the set of tuple IDs into whatever set is already in the bitmap).
// The number of tuples fetched is returned (this might be just an approximate count,
// for instance some AMs do not detect duplicates).
// While inserting tuple IDs into the bitmap, amgetbitmap can indicate that rechecking of
// the scan conditions is required for specific tuple IDs.
// This is analogous to the xs_recheck output parameter of amgettuple.
// Note: in the current implementation, support for this feature
// is conflated with support for lossy storage of the bitmap itself,
// and therefore callers recheck both the scan conditions and
// the partial index predicate (if any) for recheckable tuples.
// That might not always be true, however. amgetbitmap and amgettuple cannot be used in the same index scan;
// there are other restrictions too when using amgetbitmap, as explained in Section 62.3.
// The amgetbitmap function need only be provided if the access method supports “bitmap” index scans.
// If it doesn't, the amgetbitmap field in its IndexAmRoutine struct must be set to NULL.
int64
blgetbitmap(IndexScanDesc scan, TIDBitmap *tbm)
{
    elog(INFO, "==================blgetbitmap=================");
	int64		ntids = 0;
	BlockNumber blkno = BLOOM_HEAD_BLKNO,
				npages;
	int			i;
	BufferAccessStrategy bas;
	BloomScanOpaque so = (BloomScanOpaque) scan->opaque;

	if (so->sign == NULL)
	{
		/* New search: have to calculate search signature */
        // ScanKey是ScanKeyData的结构体数组
		ScanKey		skey = scan->keyData;

		so->sign = palloc0(sizeof(BloomSignatureWord) * so->state.opts.bloomLength);

		for (i = 0; i < scan->numberOfKeys; i++)
		{
			/*
			 * Assume bloom-indexable operators to be strict, so nothing could
			 * be found for NULL key.
			 */
			if (skey->sk_flags & SK_ISNULL)
			{
				pfree(so->sign);
				so->sign = NULL;
				return 0;
			}

			/* Add next value to the signature */
			signValue(&so->state, so->sign, skey->sk_argument,
					  skey->sk_attno - 1);

			skey++;
		}
	}

	/*
	 * We're going to read the whole index. This is why we use appropriate
	 * buffer access strategy.
	 */
	bas = GetAccessStrategy(BAS_BULKREAD);
	npages = RelationGetNumberOfBlocks(scan->indexRelation);

	for (blkno = BLOOM_HEAD_BLKNO; blkno < npages; blkno++)
	{
		Buffer		buffer;
		Page		page;

		buffer = ReadBufferExtended(scan->indexRelation, MAIN_FORKNUM,
									blkno, RBM_NORMAL, bas);

		LockBuffer(buffer, BUFFER_LOCK_SHARE);
		page = BufferGetPage(buffer);
		TestForOldSnapshot(scan->xs_snapshot, scan->indexRelation, page);

		if (!PageIsNew(page) && !BloomPageIsDeleted(page))
		{
			OffsetNumber offset,
						maxOffset = BloomPageGetMaxOffset(page);

			for (offset = 1; offset <= maxOffset; offset++)
			{
				BloomTuple *itup = BloomPageGetTuple(&so->state, page, offset);
				bool		res = true;

				/* Check index signature with scan signature */
				for (i = 0; i < so->state.opts.bloomLength; i++)
				{
					if ((itup->sign[i] & so->sign[i]) != so->sign[i])
					{
						res = false;
						break;
					}
				}

				/* Add matching tuples to bitmap */
				if (res)
				{
					tbm_add_tuples(tbm, &itup->heapPtr, 1, true);
					ntids++;
				}
			}
		}

		UnlockReleaseBuffer(buffer);
		CHECK_FOR_INTERRUPTS();
	}
	FreeAccessStrategy(bas);

	return ntids;
}
