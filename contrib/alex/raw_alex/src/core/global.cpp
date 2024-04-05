#include "global.h"

std::unordered_map<void *, BlockNumber> *nodeBlockNumberMapper;

/* Kind of relation options for bloom index */
relopt_kind bl_relopt_kind;

/* parse table for fillRelOptions */
relopt_parse_elt bl_relopt_tab[INDEX_MAX_KEYS + 1];

/*
 * Initialize any page of a bloom index.
 */
void BloomInitPage(Page page, uint16 flags) {

    elog(INFO, "==================BloomInitPage=================");

    BloomPageOpaque opaque;

    PageInit(page, BLCKSZ, sizeof(BloomPageOpaqueData));

    opaque = BloomPageGetOpaque(page);
    opaque->firstOffset = InvalidOffsetNumber;
    opaque->lastOffset = InvalidOffsetNumber;
    opaque->nextBlockNumber = InvalidBlockNumber;
    opaque->flags = flags;
    opaque->bloom_page_id = BLOOM_PAGE_ID;

}

std::vector <std::pair<int, ItemPointer>> callbackTemp;

/*
 * Per-tuple callback for table_index_build_scan.
 */
void bloomBuildCallback(Relation index, ItemPointer tid, Datum *values,
                        bool *isnull, bool tupleIsAlive, void *state) {

    elog(INFO, "==================bloomBuildCallback=================");
    std::cout << "==================bloomBuildCallback=================" << std::endl;

    // key 为建索引的那个字段的值，value 为对应数据行
    std::pair<int, ItemPointer> pair;
    pair.first = values[0];
    pair.second = tid;
    callbackTemp.push_back(pair);

}

/*
* Fill BloomState structure for particular index.
*/
void initBloomState(BloomState *state, Relation index) {

    elog(INFO, "==================initBloomState=================");

    /* Initialize amcache if needed with options from metapage */
    if (!index->rd_amcache) {

        Buffer buffer;
        Page page;
        BloomMetaPageData *meta;
        BloomOptions *opts;

        opts = static_cast<BloomOptions *>(MemoryContextAlloc(index->rd_indexcxt, sizeof(BloomOptions)));

        buffer = ReadBuffer(index, BLOOM_METAPAGE_BLKNO);
        LockBuffer(buffer, BUFFER_LOCK_SHARE);

        page = BufferGetPage(buffer);

        if (!BloomPageIsMeta(page))
            elog(ERROR, "Relation is not a bloom index");
        meta = BloomPageGetMeta(BufferGetPage(buffer));

        if (meta->magickNumber != BLOOM_MAGICK_NUMBER)
            elog(ERROR, "Relation is not a bloom index");

        *opts = meta->opts;

        UnlockReleaseBuffer(buffer);

        index->rd_amcache = (void *) opts;

    }

    memcpy(&state->opts, index->rd_amcache, sizeof(state->opts));

}

/*
         * Allocate a new page (either by recycling, or by extending the index file)
         * The returned buffer is already pinned and exclusive-locked
         * Caller is responsible for initializing the page by calling BloomInitPage
         */
Buffer BloomNewBuffer(Relation index) {

    elog(INFO, "==================BloomNewBuffer=================");

    Buffer buffer;
    bool needLock;

    /* First, try to get a page from FSM */
    for (;;) {

        BlockNumber blkno = GetFreeIndexPage(index);

        if (blkno == InvalidBlockNumber)
            break;

        buffer = ReadBuffer(index, blkno);

        /*
         * We have to guard against the possibility that someone else already
         * recycled this page; the buffer may be locked if so.
         */
        if (ConditionalLockBuffer(buffer)) {

            Page page = BufferGetPage(buffer);

            if (PageIsNew(page))
                return buffer;    /* OK to use, if never initialized */

            if (BloomPageIsDeleted(page))
                return buffer;    /* OK to use */

            LockBuffer(buffer, BUFFER_LOCK_UNLOCK);

        }

        /* Can't use it, so release buffer and try again */
        ReleaseBuffer(buffer);

    }

    /* Must extend the file */
    needLock = !RELATION_IS_LOCAL(index);
    if (needLock)
        LockRelationForExtension(index, ExclusiveLock);

    buffer = ReadBuffer(index, P_NEW);
    LockBuffer(buffer, BUFFER_LOCK_EXCLUSIVE);

    if (needLock)
        UnlockRelationForExtension(index, ExclusiveLock);

    return buffer;

}

/*
 * Construct a default set of Bloom options.
 */
BloomOptions *makeDefaultBloomOptions(void) {
    elog(INFO, "==================makeDefaultBloomOptions=================");
    BloomOptions *opts;
    opts = (BloomOptions *) palloc0(sizeof(BloomOptions));
    SET_VARSIZE(opts, sizeof(BloomOptions));
    return opts;
}
