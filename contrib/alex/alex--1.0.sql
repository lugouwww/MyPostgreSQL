/* contrib/bloom/alex--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION alex" to load this file. \quit

CREATE FUNCTION alhandler(internal)
RETURNS index_am_handler
AS 'MODULE_PATHNAME'
LANGUAGE C;

-- Access method
CREATE ACCESS METHOD alex TYPE INDEX HANDLER alhandler;
COMMENT ON ACCESS METHOD alex IS 'alex index access method';

-- Opclasses

CREATE OPERATOR CLASS int4_ops
DEFAULT FOR TYPE int4 USING alex AS
	OPERATOR	1	=(int4, int4),
	FUNCTION	1	hashint4(int4);
