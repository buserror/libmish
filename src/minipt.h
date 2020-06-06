/*
 * minipt.h
 *
 * Created on: 1 Apr 2018
 * Copyright (C) 2020 Michel Pollet <buserror@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MPTOOLS_INCLUDE_MINIPT_H_
#define MPTOOLS_INCLUDE_MINIPT_H_

/*
 * Mini Protothread.
 *
 * A thread or a coroutine would use a stack; this won't,
 * Use an old gcc trick of being able to goto and indirect label.
 * There are a few caveats: no persistent local variables, as you can't
 * have a consistent stack frame. It's easy to work around tho.
 */
#define _CONCAT2(s1, s2) s1##s2
#define _CONCAT(s1, s2) _CONCAT2(s1, s2)

#define pt_start(_pt) do { \
		if (_pt) goto *_pt; \
	} while (0);
#define pt_end(_pt) do { _pt = NULL; return; } while(0);
#define pt_yield(_pt) do { \
		_pt = &&_CONCAT(_label, __LINE__);\
		return;\
		_CONCAT(_label, __LINE__): ; \
	} while (0);
#define pt_wait(_pt, _condition) do { \
		while (!(_condition)) \
			pt_yield(_pt); \
	} while (0);

#endif /* MPTOOLS_INCLUDE_MINIPT_H_ */
