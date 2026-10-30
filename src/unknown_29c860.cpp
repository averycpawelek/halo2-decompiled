#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_2c0d00.h"
#include "unknown_11cc90.h"
#include "path.h"
#include "normalize3d.h"
#include <math.h>
// @flags /O2 /arch:SSE /Gr

real function_30bf0(vector3f *arg_0);
real function_11cc90(vector2f const *a, vector2f const *b);

struct s_29c860
{
	byte field_0[0x30];
	real field_30;
	real field_34;
	real field_38;
};

__forceinline real function_29c8b1(real arg_0)
{
	if (-1.0f > arg_0)
		return -1.0f;
	return arg_0 > 1.0f ? 1.0f : arg_0;
}

__forceinline void function_29c9ee(vector3f const *arg_0, vector3f const *arg_1, real arg_2, vector3f *arg_3)
{
	vector3f const *local_4 = arg_1;
	real local_0 = (real)sin(arg_2);
	real local_1 = (real)cos(arg_2);
	real local_2 = (local_4->i * arg_0->i + arg_0->j * arg_1->j + arg_1->k * arg_0->k) * (1.0f - local_1);
	vector3f local_3;
	local_3.k = arg_0->i * arg_1->j - local_4->i * arg_0->j;
	local_3.j = local_4->i * arg_0->k - arg_1->k * arg_0->i;
	local_3.i = arg_1->k * arg_0->j - arg_1->j * arg_0->k;
	arg_3->i = local_4->i * local_2 + local_1 * arg_0->i - local_3.i * local_0;
	arg_3->j = local_1 * arg_0->j + local_2 * arg_1->j - local_3.j * local_0;
	arg_3->k = arg_1->k * local_2 + local_1 * arg_0->k - local_3.k * local_0;
}

// score 67 (was 67): the SSE registers differ (retail xmm1/xmm4/xmm6, ours
// xmm0/xmm4/xmm1). Inline depths one/two 67; global optimization off 331;
// frame pointer 174; rotation source/axis/both by value 163/144/222;
// planar source contract 83. Cross assignment order j/k/i 66; dot with the
// k term first 66; split source-vector init 66 (alone and with j/k/i);
// crosses as three scalars 91; crosses before the dot 70; axis reads
// without the pointer copy 88; field store before the rotation 125;
// signed-angle call with swapped arguments and negation 136; fused gain
// add 66; dot as accumulate statements 77; the field subtract after the
// second limit 77. Retail preloads one axis component across fsin and
// reloads the rest from [eax] each use; ours keeps one more component
// live, so the zero lands in xmm6 and cos in xmm4 (retail xmm4/xmm3).
// Round 76: the first difference is the inlined 0x11cc90's cross product (retail loads b.j and
// then a.j first; ours, like the out-of-line 0x11cc90, b.j and b.i). The two point parameters
// swapped at the definition and both calls 71 (0x29cb10 still matches; flips the other product
// and the dot); the rotation helper's parameters as (axis, vector, ...), (out, vector, axis,
// angle) or (angle, vector, axis, out) 67 each; named facing/target pointer locals for the
// call and the source vector 71. A compile-only harness reproduces the 0x11cc90 part but not
// the rotation (the points arrive in registers only under LTCG).
// Shared dot/cross with cosine/projection/turn scale phases: single 81/100/84, pairs 105/84/80,
// all 91, native output loop 122; basic product controls 82/80/84. Zero/axis/trig homes remain.
// Native vector return with component output, original/dot/cross/both 78/74/82/89;
// aggregate output counterparts 78/74/82/89. Original helper and all game-call inputs restored.
// The first difference is in the inlined 0x11cc90: retail loads a->j and
// folds b->i in the cross term (the standalone 0x11cc90 and ours do the
// reverse). Rotation in place on the source vector 67; the helper in
// 0x11d3b0's clean form 101; sine/cosine as helper arguments 67; source
// vector initialised at the top 194; the two 2-D vector parameters swapped
// at the definition and both calls 71. Round 29: the source vector read through the
// 2-D vector type 67; the cross vector declared before the dot 67; a separate result
// vector copied out 78; the rotation straight into the output 97; one named 2-D view of
// the source point for both uses 78.
// at the definition and both calls 71.
// Shared dot/cross with cosine/projection/turn scale phases: single 81/100/84, pairs 105/84/80,
// all 91, native output loop 122; basic product controls 82/80/84. Zero/axis/trig homes remain.
// Native vector return with component output, original/dot/cross/both 78/74/82/89;
// aggregate output counterparts 78/74/82/89. Original helper and all game-call inputs restored.
// Round 255: two-use previous-angle snapshot with gain/both-bound/native-delta subsets 104/112/114/112/104/111/114/108. All restored; original observations, signed-angle call, rotation helper and callers retained.
// @retail 0x29c860
void function_29c860(long arg_0, long arg_1, point2f const *arg_2,
	point2f const *arg_3, real arg_4, vector3f *arg_5)
{
	s_actor_view *local_0 = actor_get(arg_0);
	s_29c860 const *local_1 = (s_29c860 const *)function_1e5450(arg_0, *(long *)object_get(arg_1));
	real local_3 = function_11cc90((vector2f const *)arg_2, (vector2f const *)arg_3);
	local_3 += arg_4;
	if (local_1->field_38 > 0.0f)
		local_3 *= local_1->field_38;
	real local_4 = local_1->field_30 > 0.0f ? local_1->field_30 * 0.01745329238474369f : 1.5707963705062866f;
	if (-local_4 > local_3)
		local_3 = -local_4;
	else if (local_3 > local_4)
		local_3 = local_4;
	arg_4 = local_3 - *(real *)((byte *)local_0 + 0x654);
	local_4 = 6.2831854820251465f;
	if (local_1->field_34 > 0.0f)
		local_4 = local_1->field_34 * 0.01745329238474369f;
	if (-local_4 > arg_4)
		arg_4 = -local_4;
	else if (arg_4 > local_4)
		arg_4 = local_4;
	arg_4 += *(real *)((byte *)local_0 + 0x654);
	vector3f local_7;
	vector3f local_8 = { arg_2->x, arg_2->y, 0.0f };
	function_29c9ee(&local_8, g_4687b0, arg_4, &local_7);
	*(real *)((byte *)local_0 + 0x65c) = arg_4;
	*arg_5 = local_7;
}

// @retail 0x29cb10
void function_29cb10(long arg_0, long arg_1, point2f const *arg_2, vector3f *arg_3)
{
	s_actor_view *local_0 = actor_get(arg_0);
	vector3f local_1;
	if (*(bool *)((byte *)local_0 + 0x5d1))
	{
		local_1 = *(vector3f *)((byte *)local_0 + 0x5f8);
		local_1.k = 0.0f;
		if (function_30bf0(&local_1) != 0.0f)
			goto local_2;
	}
	local_1 = *(vector3f *)((byte *)local_0 + 0x290);
local_2:
	function_29c860(arg_0, arg_1, arg_2, (point2f *)&local_1, 0.0f, arg_3);
}

struct s_298c30;
struct s_29946c;
struct s_298c46
{
	byte field_0;
	bool field_1;
	byte field_2[2];
	real field_4;
	real field_8;
	point2f field_c;
	byte field_14[4];
	short field_18;
	byte field_1a[0xe];
};

point3f *function_b9dd0(long arg_0, point3f *arg_1);
void function_26c180(long arg_0);
bool function_1f9760(long arg_0, long arg_1, point3f const *arg_2, real arg_3,
	bool *arg_4, bool *arg_5, vector3f *arg_6);
bool function_1f9240(long arg_0, s_path_settings *arg_1);
void obstacle_list_group(s_obstacle_list *arg_0, real arg_1);
void __stdcall function_2c0d60(vector3f const *arg_0, long arg_1, byte arg_2, s_path_settings const *arg_3,
	s_obstacle_list *arg_4, s_obstacle_list *arg_5, point3f const *arg_6, real arg_7, vector3f const *arg_8,
	long arg_9, long arg_10);
real function_1f99d0(long arg_0, long arg_1, short arg_2, point2f const *arg_3,
	point2f const *arg_4, point2f const *arg_5, s_obstacle_list const *arg_6);
real function_1f9e70(long arg_0, long arg_1, point2f const *arg_2, vector2f const *arg_3,
	vector2f const *arg_4, vector2f const *arg_5, vector2f const *arg_6, real arg_7, bool arg_8);
real function_11cd20(vector2f const *arg_0, vector2f const *arg_1);
bool function_299460(long arg_0, s_298c46 *arg_1, s_298c30 const *arg_2,
	point3f const *arg_3, vector3f const *arg_4, vector3f const *arg_5,
	real arg_6, s_29946c const *arg_7);

void function_1e4260(long arg_0);
void function_1e4290(long arg_0, bool arg_1);

__forceinline real function_29cc7e(vector3f *arg_0)
{
	arg_0->k = 0.0f;
	return normalize3d(arg_0);
}

// score 300 (was 360): the actor flag updates are the existing setters 0x1e4290 (clear 0x800)
// and 0x1e4260 (set 2), which retail inlines here (they are `inline` at their definitions), and the
// side test reads the left vector, which is initialised from local_8 as sibling 0x29a190 writes it
// (retail reads the left vector's home there and the scalar's home in the two direction sums).
// Left: the left vector and the avoidance vector (local_14) swap their 12-byte slots (ours 0x78/0x84,
// retail 0x84/0x78): VC orders them by reference count and they tie here (one avoidance store more,
// or one left-vector store fewer, gives retail's slots as a diagnostic); the steering block's sums,
// the second normalize's j product, the 0x5d0 flag store and the local_16 dot's x87 order and early
// field_44 load still differ. Tried: the dot sums swapped 360; the velocity dot written i/j/k, j/k/i
// and through dot3f either way 360 each; the projection test's terms swapped 360; the flags written
// through the actor struct field 365. Round 148: the left vector stored before the local_11 copy 297
// (retail's store order); also before local_12 297; left_i after the vector 300; the local_16 dot in
// three other operand orders 300 each; direction.i/direction.j operands reordered 300-322 (canonical);
// the side test as the full 2-D dot with the left vector 328; the left vector through vector3f_assign
// 321; declared in the steering block 377; the avoidance vector at function scope 322; a separate
// velocity temporary for the early vehicle block 415.
// @retail 0x29cbb0
void function_29cbb0(long arg_0, long arg_1, vector3f const *arg_2, vector3f *arg_3,
	short *arg_4, vector3f *arg_5, bool *arg_6, bool *arg_7, short arg_8)
{
	byte *local_0 = (byte *)actor_get(arg_0);
	s_slot_object_view *local_1 = object_get(arg_1);
	byte const *local_2 = (byte const *)function_1e5450(arg_0, *(long *)local_1);
	bool local_3 = false;
	bool local_4 = false;
	bool local_5 = true;
	vector3f direction;
	*(bool *)(local_0 + 0x6d1) = false;
	if (arg_8 == 0)
	{
		*(real *)(local_0 + 0x654) = *(real *)(local_0 + 0x65c);
		*(real *)(local_0 + 0x658) = *(real *)(local_0 + 0x660);
	}
	if (local_1->type == 1)
	{
		if (*(byte *)((byte *)local_1 + 0x34c) > 0)
		{
			local_3 = true;
			*(bool *)(local_0 + 0x5d0) = false;
		}
		else if (0.7f > *(real *)((byte *)local_1 + 0x370))
		{
			local_3 = true;
			if (0.8f > *(real *)((byte *)local_1 + 0x84))
			{
				direction = *(vector3f *)((byte *)local_1 + 0x7c);
				direction.k = 0.0f;
				if (function_30bf0(&direction) > 0.0f)
				{
					*(bool *)(local_0 + 0x5d0) = true;
					((vector3f *)(local_0 + 0x5ec))->i = direction.i * 3.0f;
					((vector3f *)(local_0 + 0x5ec))->j = direction.j * 3.0f;
					((vector3f *)(local_0 + 0x5ec))->k = direction.k * 3.0f;
				}
				else
					*(bool *)(local_0 + 0x5d0) = false;
			}
		}
	}
	point3f local_7;
	function_b9dd0(arg_1, &local_7);
	vector3f local_8 = *(vector3f *)(local_0 + 0x290);
	if (function_29cc7e(&local_8) == 0.0f)
		local_8 = *g_4687a8;
	if (*(bool *)(local_0 + 0x44d))
		*(bool *)(local_0 + 0x6d2) = true;
	vector3f local_9 = *arg_2;
	if (function_29cc7e(&local_9) == 0.0f)
		*(bool *)(local_0 + 0x5d0) = false;
	real local_10 = (real)sqrt(arg_2->i * arg_2->i + arg_2->j * arg_2->j + arg_2->k * arg_2->k);
	vector3f local_12 = *g_4687a4;
	vector3f local_11 = local_8;
	real left_i = -local_8.j;
	vector3f local_13 = {-local_8.j, local_8.i, 0.0f};
	if (!local_2)
		*(real *)(local_0 + 0x660) = 0.0f;
	else if (!*(bool *)(local_0 + 0x5d0) || *(bool *)(local_0 + 0x5d2))
	{
		*(real *)(local_0 + 0x660) = 0.0f;
		function_29cb10(arg_0, arg_1, (point2f *)&local_8, &local_11);
	}
	else
	{
		vector3f local_14;
		if (function_1f9760(arg_0, arg_1, &local_7, local_10, &local_5, &local_4, &local_14))
		{
			*(real *)(local_0 + 0x660) = 0.0f;
			function_29cb10(arg_0, arg_1, (point2f *)&local_8, &local_11);
			*arg_7 = true;
			goto local_15;
		}
		if (local_4)
		{
			local_14.k = 0.0f;
			if (function_30bf0(&local_14) == 0.0f)
				local_4 = false;
		}
		bool local_16 = *(bool *)(local_0 + 0x5d1);
		if (local_16 && (((vector3f *)(local_0 + 0x5f8))->j * arg_2->j + arg_2->i * ((vector3f *)(local_0 + 0x5f8))->i) / local_10 > 0.5)
			local_16 = false;
		short local_17;
		if (!local_16 && !(*(real *)(local_2 + 0x44) > local_10 && *(bool *)(local_0 + 0x4d5)))
		{
			local_11 = *arg_2;
			local_11.k = 0.0f;
			if (function_30bf0(&local_11) == 0.0f)
				local_11 = *(vector3f *)(local_0 + 0x290);
			local_17 = 1;
		}
		else
		{
			direction = *arg_2;
			local_17 = 0;
			if (local_16)
				local_11 = *(vector3f *)(local_0 + 0x5f8);
			else if (*(bool *)(local_0 + 0x4d5))
				local_11 = *(vector3f *)(local_0 + 0x4d8);
			direction.k = 0.0f;
			local_11.k = 0.0f;
			if (function_30bf0(&local_11) == 0.0f)
				local_11 = *(vector3f *)(local_0 + 0x290);
			if (function_30bf0(&direction) == 0.0f)
				direction = local_11;
			real local_19 = direction.j * local_8.j + direction.i * local_8.i;
			real local_20 = left_i * direction.i + direction.j * local_8.i;
			direction.i = local_19;
			direction.j = local_20;
			direction.k = 0.0f;
			function_30bf0(&direction);
		}
		s_obstacle_list local_21;
		s_obstacle_list *local_22 = &local_21;
		union
		{
			s_path_settings local_23;
			s_298c46 local_24;
		};
		if (*(bool *)(local_0 + 0x478))
			local_22 = NULL;
		else
		{
			function_1f9240(arg_0, &local_23);
			local_21.group_count = 0;
			local_21.count = 0;
			local_21.flag0_count = 0;
			local_21.flag3_count = 0;
			*(short *)local_21.unknown08 = 0;
			function_2c0d60(0, arg_0, true, &local_23, &local_21, 0, &local_7, 20.0f, arg_2, arg_1, NONE);
			obstacle_list_group(&local_21, *(real *)(local_2 + 0x14));
		}
		if (local_17 == 1)
		{
			function_26c180(arg_0);
			if (*(bool *)(local_0 + 0x278) && *(long *)(local_0 + 0x28c) != NONE)
			{
				local_24.field_4 = function_11cd20((vector2f *)&local_8, (vector2f *)&local_9);
				real local_25 = local_9.j * local_8.i;
				local_24.field_0 = local_9.i * local_13.i + local_25 > 0.0f;
				local_24.field_1 = false;
				local_24.field_8 = 1.0f;
				local_24.field_1a[10] = true;
				function_299460(arg_0, &local_24, (s_298c30 const *)local_2, &local_7, &local_8, &local_13,
					*(real *)(local_2 + 0x20), (s_29946c const *)local_22);
				if (0.9f > local_24.field_8)
				{
					direction.j = left_i * local_9.i + local_25;
					direction.i = local_9.j * local_8.j + local_9.i * local_8.i;
					direction.k = 0.0f;
					local_17 = 0;
					function_30bf0(&direction);
					function_1e4290(arg_0, false);
				}
			}
		}
		real local_26 = 0.0f;
		if (local_4 && local_17 == 1)
			local_26 = function_1f99d0(arg_0, arg_1, (short)!local_5,
				(point2f const *)(local_0 + 0x548 + *(char *)(local_0 + 0x53a) * 0x1c),
				(point2f const *)&local_7, (point2f const *)&local_14, local_22);
		function_29c860(arg_0, arg_1, (point2f *)&local_8, (point2f *)&local_11, local_26, &local_11);
		real local_27 = 1.0f;
		if (local_17 == 0)
			local_12 = direction;
		else
		{
			real local_28 = function_1f9e70(arg_0, arg_1, (point2f *)&local_7, (vector2f *)&local_9,
				(vector2f *)&local_11, (vector2f *)&local_8, local_4 ? (vector2f *)&local_14 : NULL, local_10, local_17 != 1);
			real local_29 = *(real *)((byte *)local_1 + 0x90) * local_11.k + *(real *)((byte *)local_1 + 0x8c) * local_11.j + *(real *)((byte *)local_1 + 0x88) * local_11.i;
			real local_30 = *(real *)(local_2 + 0x68);
			real local_31;
			if (local_30 > 0.0f)
				local_31 = *(real *)(local_2 + 0x6c);
			else
			{
				local_31 = 4.019999980926514f;
				local_30 = 6.0f;
			}
			local_27 = (local_28 - local_29 / local_30) * local_31 + local_28;
			if (local_27 > 0.0f && *(real *)(local_2 + 0x60) > 0.0f)
			{
				real local_33 = local_27 - *(real *)(local_0 + 0x658);
				if (local_33 > *(real *)(local_2 + 0x60))
					local_33 = *(real *)(local_2 + 0x60);
				local_27 = *(real *)(local_0 + 0x658) + local_33;
			}
			*(real *)(local_0 + 0x660) = 0.0f > local_27 ? 0.0f : local_27;
			if (-*(real *)(local_2 + 0x50) > local_27)
				local_27 = -*(real *)(local_2 + 0x50);
			else if (local_27 > *(real *)(local_2 + 0x50))
				local_27 = *(real *)(local_2 + 0x50);
			local_12.i = 1.0f;
		}
		local_12.i *= local_27;
		local_12.j *= local_27;
		local_12.k *= local_27;
		local_12.i = function_29c8b1(local_12.i);
		local_12.j = function_29c8b1(local_12.j);
		local_12.k = function_29c8b1(local_12.k);
		*arg_6 = false;
		*(short *)(local_0 + 0x664) = local_17;
	}
local_15:
	*arg_4 = 0;
	*arg_3 = local_11;
	*arg_5 = local_12;
	if (*arg_6 || !*(bool *)(local_0 + 0x5d0))
		function_1e4290(arg_0, false);
	if (local_3)
		function_1e4260(arg_0);
}
