/*
 * mish_line.c
 *
 * Copyright (C) 2020 Michel Pollet <buserror@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "mish_priv.h"
#include "mish_priv_line.h"

/*
 * Here we duplicate length bytes of the line, queue that, and reset
 * the original line with what remains.
 * Did that so the input line 'grows' one way, and stays grown, while
 * the one we queue gets 'trimmed' down to size.
 */
mish_line_p
_mish_line_split(
		mish_line_queue_t * q,
		mish_line_p line,
		size_t length )
{
	static const mish_line_t zero = {};
	size_t l = line->len + 1;
	mish_line_p nl = (mish_line_p)malloc(sizeof(*nl) + l);
	*nl = zero;
	nl->size = l;
	nl->len = l - 1;
	memcpy(nl->line, line->line, l);
	nl->line[l] = 0;
	nl->stamp = _mish_stamp_ms();
	TAILQ_INSERT_TAIL(q, nl, self);
	line->len = 0;
	return line;
}

/*
 * Make sure the size of a line doesn't grow over MISH_MAX_LINE_SIZE
 */
mish_line_p
_mish_line_reserve_or_split(
		mish_line_queue_t * q,
		mish_line_p l,
		size_t size )
{
	if (l && (l->size - l->len) >= size)
		return l;
	if (!l || ((l->size + size) < MISH_MAX_LINE_SIZE))
		return _mish_line_reserve(l, size);

	return _mish_line_split(q, l, l->len);
}

mish_line_p
_mish_line_reserve(
		mish_line_p line,
		uint32_t count)
{
	if (count < 40)
		count = 40;
	if (!line) {
		line = (mish_line_p)calloc(1, sizeof(mish_line_t) + count);
		line->size = count;
	}
	if (line->size - line->len < count) {
		line = (mish_line_p)realloc(line,
							sizeof(mish_line_t) + line->size + count);
		line->size += count;
	}
	return line;
}
