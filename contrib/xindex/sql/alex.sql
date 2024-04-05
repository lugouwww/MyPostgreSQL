CREATE EXTENSION alex;

CREATE TABLE tst (
	i	int4,
    t	text
);

-- INSERT INTO tst SELECT i%1000000, '1' FROM generate_series(1,10000000) i;
INSERT INTO tst SELECT FLOOR(random() * 1000000)::int, '1' FROM generate_series(1,10000) i;
CREATE INDEX alexidx ON tst USING alex (i);

SET enable_seqscan=on;
SET enable_bitmapscan=off;
SET enable_indexscan=off;

EXPLAIN (COSTS OFF) SELECT count(*) FROM tst WHERE i = 7;
SELECT count(*) FROM tst WHERE i = 7;
SELECT * FROM tst WHERE i = 7;

SET enable_seqscan=off;
SET enable_bitmapscan=on;
SET enable_indexscan=on;

EXPLAIN (COSTS OFF) SELECT count(*) FROM tst WHERE i = 7;
SELECT count(*) FROM tst WHERE i = 7;
SELECT * FROM tst WHERE i = 7;
