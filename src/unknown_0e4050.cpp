// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "engine_peer.h"
#include "effects.h"
#include <math.h>

struct s_e4050_object
{
	byte unknown00[0x14];
	long parent_index;
	byte unknown18[0xa9];
	byte flag_c1_0 : 1;
	byte unknownc1 : 7;
	byte unknownc2[0x286];
	byte flag_348_0 : 1;
	byte unknown348 : 7;
	byte unknown349[0x50];
	char value_399;
	byte unknown39a[0x42];
	byte state_3dc;
};

struct s_e4050_object_header
{
	byte unknown00[8];
	s_e4050_object *object;
};

// score 8 (was 8): Full-word truth/return versus byte truth/return remains; restore the entire opening executable body.
// Earlier: Shared truth and return use DL/AL rather than retail EDX/EAX.
// Earlier long result 18; bool result 28; return 1 / 0 30; word/dword/long flag views 30 (18 with the long
// result).
// Byte/full truth and return registers remain (DL/AL versus EDX/EAX). Actual-admission bool/long/word/dword
// 8/36/38/36; native byte fields with those types 8/36/38/36; direct terminal predicate with long/word/bool
// and native long 11 each. Keep the Boolean admission result with original fields; plain full check preserves
// every baseline match.
// Round 191: Native byte/signed-char/short admission 25/25/26; bitwise bool/long/short/byte admission 26/52/56/37.
// Round 191: Complete failure clear 30; separate Boolean failures 30; nested terminal result 17.
// Round 191: Long/short/byte failure-clear 11/11/17; separate wide failures 23/23/27. All restored; no qualifying gain.
// @retail 0xe4050
bool function_e4050(long object_index)
{
	s_e4050_object *object = ((s_e4050_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	bool result = !TEST_FIELD_BIT(object->flag_c1_0) && TEST_FIELD_BIT(object->flag_348_0);
	if (result)
	{
		real scaled = g_510c54->field_2_3 * 0.18f;
		long ticks;

		__asm
		{
			fld scaled
			fistp ticks
		}
		result = object->value_399 >= ticks &&
			(object->state_3dc == 1 || object->state_3dc == 3) &&
			object->parent_index == NONE;
	}
	return result;
}

/* inline keeps it out of the callers' register tracking: retail's callers
   treat every scratch register (edx, the xmm registers) as clobbered by it
   (0x21e230 keeps `this` in esi across the call); noinline keeps the call */
inline __declspec(noinline) real function_12aff0(real a, real b, real c, bool flag);

// @retail 0x12aff0
real function_12aff0(real a, real b, real c, bool flag)
{
	if (!(0.0001f > fabs(a - b)))
	{
		real t = (c - a) / (b - a);
		if (t < 0.0f)
			t = 0.0f;
		else if (t > 1.0f)
			t = 1.0f;
		return t;
	}

	word above = a > c;
	return above == flag ? 0.0f : 1.0f;
}

/* the particle system objects at 0x479868 and 0x479874, picked by the group
   of a tag (c_type_4e7709 is in effects.h) */
c_type_4e7709 g_479868;
c_type_4e7709 g_479874;

// @retail 0x137bd0
c_type_4e7709 *function_137bd0(long tag_index)
{
	return particle_object_for_tag(tag_index);
}

// @retail 0x15e020
bool function_15e020(short a, short b)
{
	bool result = false;
	c_engine_peer *engine = g_55e4d0[g_4e9ae8->engine_index];

	if (engine)
		result = engine->p27(a, b);
	return result;
}
