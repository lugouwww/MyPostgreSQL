//
// Created by 唐雨露 on 2024/3/9.
//

#ifndef MYALEX_GLOBAL_MAP_H
#define MYALEX_GLOBAL_MAP_H

#include <unordered_map>

int modelBlockCount = 0;
std::unordered_map<void *, BlockNumber> nodeBlockNumberMapper;
std::unordered_map<BlockNumber, void *> nodesRead;
std::vector <std::pair<int, ItemPointerData>> callbackTemp;
/* Kind of relation options for bloom index */
relopt_kind bl_relopt_kind;
/* parse table for fillRelOptions */
relopt_parse_elt bl_relopt_tab[INDEX_MAX_KEYS + 1];


/* Support procedures numbers */
#define BLOOM_HASH_PROC            1
#define BLOOM_OPTIONS_PROC        2
#define BLOOM_NPROC                2

/* Scan strategies */
#define BLOOM_EQUAL_STRATEGY    1
#define BLOOM_NSTRATEGIES        1

/* Opaque for bloom pages */
// page layout的special部分
typedef struct BloomPageOpaqueData {
    // todo 有没有用不知道，先写在这吧
    OffsetNumber firstOffset;   // 数据页中第一个pair的起始位置
    OffsetNumber lastOffset;    // 数据页中最后一个pair的起始位置
    BlockNumber nextBlockNumber;    // 如果该数据页放不下所有的pair，指向下一个数据页
    uint16 flags;            /* see bit definitions below */
    uint16 bloom_page_id;    /* for identification of BLOOM indexes */
} BloomPageOpaqueData;

typedef BloomPageOpaqueData *BloomPageOpaque;

/* Bloom page flags */
// 是放在每个 page 的 special 部分里的
#define BLOOM_META        (1)
#define BLOOM_DELETED    (2)
#define ALEX_MODEL_NODE    (3)
#define ALEX_DATA_NODE    (4)

/*
 * The page ID is for the convenience of pg_filedump and similar utilities,
 * which otherwise would have a hard time telling pages of different index
 * types apart.  It should be the last 2 bytes on the page.  This is more or
 * less "free" due to alignment considerations.
 *
 * See comments above GinPageOpaqueData.
 */
#define BLOOM_PAGE_ID        0xFF93

/* Macros for accessing bloom page structures */
#define BloomPageGetOpaque(page) ((BloomPageOpaque) PageGetSpecialPointer(page))
#define BloomPageGetMaxOffset(page) (BloomPageGetOpaque(page)->maxoff)
#define BloomPageIsMeta(page) \
    ((BloomPageGetOpaque(page)->flags & BLOOM_META) != 0)
#define BloomPageIsDeleted(page) \
    ((BloomPageGetOpaque(page)->flags & BLOOM_DELETED) != 0)
#define BloomPageSetDeleted(page) \
    (BloomPageGetOpaque(page)->flags |= BLOOM_DELETED)
#define BloomPageSetNonDeleted(page) \
    (BloomPageGetOpaque(page)->flags &= ~BLOOM_DELETED)
// todo PageHeaderData之后的部分？
#define BloomPageGetData(page)        ((BloomTuple *)PageGetContents(page))
// state是为了获取每个tuple的大小，page是为了得到content的起始位置，offset是指明要第几个tuple
#define BloomPageGetTuple(state, page, offset) \
    ((BloomTuple *)(PageGetContents(page) \
        + (state)->sizeOfBloomTuple * ((offset) - 1)))
#define BloomPageGetNextTuple(state, tuple) \
    ((BloomTuple *)((Pointer)(tuple) + (state)->sizeOfBloomTuple))

/* Preserved page numbers */
#define BLOOM_METAPAGE_BLKNO    (0)
#define BLOOM_SUPER_ROOT_BLKNO  (1)

/*
 * We store Bloom signatures as arrays of uint16 words.
 */
// myfind BloomSignatureWord
typedef uint16 BloomSignatureWord;

#define SIGNWORDBITS ((int) (BITS_PER_BYTE * sizeof(BloomSignatureWord)))

/*
 * Default and maximum Bloom signature length in bits.
 */
#define DEFAULT_BLOOM_LENGTH    (5 * SIGNWORDBITS)
#define MAX_BLOOM_LENGTH        (256 * SIGNWORDBITS)

/*
 * Default and maximum signature bits generated per index key.
 */
#define DEFAULT_BLOOM_BITS        2
#define MAX_BLOOM_BITS            (MAX_BLOOM_LENGTH - 1)

/* Bloom index options */
// myfind BloomOptions
typedef struct BloomOptions {
    int32 vl_len_;        /* varlena header (do not touch directly!) */
} BloomOptions;

/*
 * FreeBlockNumberArray - array of block numbers sized so that metadata fill
 * all space in metapage.
 */
typedef BlockNumber FreeBlockNumberArray[
        MAXALIGN_DOWN(
                BLCKSZ - SizeOfPageHeaderData - MAXALIGN(sizeof(BloomPageOpaqueData))
                - MAXALIGN(sizeof(uint16) * 2 + sizeof(uint32) + sizeof(BloomOptions))
        ) / sizeof(BlockNumber)
];

/* Metadata of bloom index */
// bloom index中meta page的content部分
typedef struct BloomMetaPageData {
    uint32 magickNumber;
    int key_domain_min_;
    int key_domain_max_;
    int num_keys_above_key_domain;
    int num_keys_below_key_domain;
    int num_keys_at_last_right_domain_resize;
    int num_keys_at_last_left_domain_resize;
    BloomOptions opts;
} BloomMetaPageData;

typedef struct DataSlotItem {
    int key;
    ItemPointerData value;
} DataSlotItem;

// alex index 中 data node page 的 content 部分
typedef struct AlexDataNodePageData {
    bool is_leaf_;
    uint8_t duplication_factor_;
    short level_;
    double model_a;
    double model_b;
    double cost_;
    BlockNumber next_leaf_;
    BlockNumber prev_leaf_;
    int data_capacity_;
    int num_keys_;
    int bitmap_size_;
    double expansion_threshold_;
    double contraction_threshold_;
    int max_slots_;
    long long num_shifts_;
    long long num_exp_search_iterations_;
    int num_lookups_;
    int num_inserts_;
    int num_resizes_;
    int max_key_;
    int min_key_;
    int num_right_out_of_bounds_inserts_;
    int num_left_out_of_bounds_inserts_;
    double expected_avg_exp_search_iterations_;
    double expected_avg_shifts_;
    // data_slots_ 开新地方存
    BlockNumber data_slots_;
    int compressed_bitmap_size_;
    // bitmap_ 开新地方存
    BlockNumber bitmap_;
} AlexDataNodePageData;

// alex index 中 model node page 的 content 部分
typedef struct AlexModelNodePageData {
    bool is_leaf_;
    uint8_t duplication_factor_;
    short level_;
    double model_a;
    double model_b;
    double cost_;
    int num_children_;
    // children_ 开新地方存
    BlockNumber children_;
} AlexModelNodePageData;

/* Magic number to distinguish bloom pages among anothers */
#define BLOOM_MAGICK_NUMBER (0xDBBC0DED)

/* Number of blocks numbers fit in BloomMetaPageData */
#define BloomMetaBlockN        (sizeof(FreeBlockNumberArray) / sizeof(BlockNumber))

#define BloomPageGetMeta(page)    ((BloomMetaPageData *) PageGetContents(page))

#define AlexGetModelPageContent(page)    ((AlexModelNodePageData *) PageGetContents(page))

#define AlexGetDataPageContent(page)    ((AlexDataNodePageData *) PageGetContents(page))

#define AlexGetGeneralContent(page)    ((uint8_t *) PageGetContents(page))

#define BloomPageGetOpaque(page) ((BloomPageOpaque) PageGetSpecialPointer(page))

#define MaxBlockNumberPerBlock ((BLCKSZ - SizeOfPageHeaderData - MAXALIGN(sizeof(BloomPageOpaqueData))) / sizeof(BlockNumber))
#define MaxDataSlotPerBlock ((BLCKSZ - SizeOfPageHeaderData - MAXALIGN(sizeof(BloomPageOpaqueData))) / sizeof(DataSlotItem))
#define MaxUint64PerBlock ((BLCKSZ - SizeOfPageHeaderData - MAXALIGN(sizeof(BloomPageOpaqueData))) / sizeof(uint64_t))
#define MaxCharPerBlock ((BLCKSZ - SizeOfPageHeaderData - MAXALIGN(sizeof(BloomPageOpaqueData))) / sizeof(unsigned char))

typedef struct BloomState {
    BloomOptions opts;            /* copy of options on index's metapage */
} BloomState;

#define BloomPageGetFreeSpace(state, page) \
    (BLCKSZ - MAXALIGN(SizeOfPageHeaderData) \
        - BloomPageGetMaxOffset(page) * (state)->sizeOfBloomTuple \
        - MAXALIGN(sizeof(BloomPageOpaqueData)))

/*
 * Tuples are very different from all other relations
 */
// myfind BloomTuple
// 在bloom index中，表中的每个数据行都会有一个对应的bit数组
typedef struct BloomTuple {
    ItemPointerData heapPtr;
    BloomSignatureWord sign[FLEXIBLE_ARRAY_MEMBER];
} BloomTuple;

#define BLOOMTUPLEHDRSZ offsetof(BloomTuple, sign)

/* Opaque data structure for bloom index scan */
typedef struct BloomScanOpaqueData {
    BloomSignatureWord *sign;    /* Scan signature */
    BloomState state;
} BloomScanOpaqueData;

typedef BloomScanOpaqueData *BloomScanOpaque;

/*
* State of bloom index build.  We accumulate one page data here before
* flushing it to buffer manager.
*/
typedef struct {
    BloomState blstate;        /* bloom index state */
} BloomBuildState;

/* Signature dealing macros - note i is assumed to be of type int */
#define GETWORD(x, i) ( *( (BloomSignatureWord *)(x) + ( (i) / SIGNWORDBITS ) ) )
#define CLRBIT(x, i)   GETWORD(x,i) &= ~( 0x01 << ( (i) % SIGNWORDBITS ) )
#define SETBIT(x, i)   GETWORD(x,i) |=  ( 0x01 << ( (i) % SIGNWORDBITS ) )
#define GETBIT(x, i) ( (GETWORD(x,i) >> ( (i) % SIGNWORDBITS )) & 0x01 )

PG_FUNCTION_INFO_V1(blhandler);

/*
 * Initialize any page of a bloom index.
 */
void BloomInitPage(Page page, uint16 flags, BlockNumber nextBlockNumber = InvalidBlockNumber) {

//    elog(INFO, "==================BloomInitPage=================");

    BloomPageOpaque opaque;

    PageInit(page, BLCKSZ, sizeof(BloomPageOpaqueData));

    opaque = BloomPageGetOpaque(page);
    opaque->firstOffset = InvalidOffsetNumber;
    opaque->lastOffset = InvalidOffsetNumber;
    opaque->nextBlockNumber = nextBlockNumber;
    opaque->flags = flags;
    opaque->bloom_page_id = BLOOM_PAGE_ID;

}

/*
 * Per-tuple callback for table_index_build_scan.
 */
void bloomBuildCallback(Relation index, ItemPointer tid, Datum *values,
                        bool *isnull, bool tupleIsAlive, void *state) {

//    elog(INFO, "==================bloomBuildCallback=================");
//    std::cout << "==================bloomBuildCallback=================" << std::endl;

//    // 8
//    std::cout << "sizeof(tid) = " << sizeof(tid) << std::endl;
//    // 6
//    std::cout << "sizeof(*tid) = " << sizeof(*tid) << std::endl;
//    std::cout << (*tid).ip_blkid.bi_hi << std::endl;
//    std::cout << (*tid).ip_blkid.bi_lo << std::endl;
//    std::cout << (*tid).ip_posid << std::endl;

    // key 为建索引的那个字段的值，value 为对应数据行
    std::pair<int, ItemPointerData> pair;
    pair.first = values[0];
    pair.second = *tid;
    callbackTemp.push_back(pair);

}

/*
* Fill BloomState structure for particular index.
*/
void initBloomState(BloomState *state, Relation index) {

//    elog(INFO, "==================initBloomState=================");
//    std::cout << "==================initBloomState=================" << std::endl;

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

//    elog(INFO, "==================BloomNewBuffer=================");

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
//    elog(INFO, "==================makeDefaultBloomOptions=================");
    BloomOptions *opts;
    opts = (BloomOptions *) palloc0(sizeof(BloomOptions));
    SET_VARSIZE(opts, sizeof(BloomOptions));
    return opts;
}

#endif //MYALEX_GLOBAL_MAP_H
