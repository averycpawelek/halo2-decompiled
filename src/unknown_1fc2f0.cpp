// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1FC2F0.CPP: a tracked point (an aim or look target) and its
   updates */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "slot_handler.h"
#include "unknown_1cec30.h"
#include "unknown_1946f0.h"
#include "object_default_placement.h"
#include <math.h>

void function_1d4360(s_havok_component *component, transform4x3f *volatile result);
void havok_component_rigid_body_linear_velocity_set(long rigid_body_index, s_havok_component *component, vector3f const *velocity);

static inline void local_vector3d_from_havok(vector3f *vector, hkVector4 const *havok)
{
	vector->i = (*havok)(0);
	vector->j = (*havok)(1);
	vector->k = (*havok)(2);
}

static inline void local_scale3d(vector3f const *vector, real scale, vector3f *result)
{
	result->i = vector->i;
	result->j = vector->j;
	result->k = vector->k;

	result->i *= scale;
	result->j *= scale;
	result->k *= scale;
}

static inline void local_rigid_body_linear_velocity_get(long rigid_body_index, s_havok_component *component, vector3f *velocity)
{
	hkRigidBody *rigid_body = havok_component_rigid_body_get(rigid_body_index, component);

	if (!rigid_body->m_fixed)
	{
		local_vector3d_from_havok(velocity, &rigid_body->m_motion->m_linear_velocity);
	}
	else
	{
		*velocity = *g_4687a4;
	}
}

/* where a point lies: two indices that the tracking copies together */
struct s_tracking_location
{
	short unknown0;
	short unknown2;
};

struct s_tracked_point
{
	point3f point;
	vector3f offset;
	vector3f velocity;
	s_tracking_location location;
	bool unknown28;
	bool unknown29;
};

// @retail 0x1fc2f0
void function_1fc2f0(s_tracked_point *tracked, point3f const *point, bool unknown)
{
	tracked->offset = *g_4687a4;
	tracked->point = *point;
	tracked->unknown29 = false;
	tracked->velocity = *g_4687a4;
	*(volatile bool *)&tracked->unknown28 = unknown;
	*(volatile short *)&tracked->location.unknown0 = NONE;
	*(volatile short *)&tracked->location.unknown2 = NONE;
}

/* Updates an object's position from Havok state and adjusts its rigid-body linear velocity. */
// @retail 0x1fc350
void function_1fc350(long object_index, s_tracked_point *tracked)
{
	s_havok_object *havok_object = havok_object_get(object_index);
	s_havok_component *component = havok_component_get(havok_object->havok_component_index);

	transform4x3f matrix;
	function_1d4360(component, &matrix);

	s_slot_object_view *object = object_get(object_index);
	if (object->parent_index == NONE)
		function_b75a0(object_index, &matrix.position, NULL, NULL, NULL, false);

	vector3f *velocity = &tracked->velocity;
	vector3f linear_velocity;
	real magnitude = function_30bf0(velocity);

	if (magnitude > 1.0f)
	{
		local_rigid_body_linear_velocity_get(0, component, &linear_velocity);

		real linear_i = linear_velocity.i;
		real linear_j = linear_velocity.j;
		real linear_k = linear_velocity.k;

		real dot =
			((vector3f volatile *)velocity)->k * linear_velocity.k +
			((vector3f volatile *)velocity)->j * linear_velocity.j +
			((vector3f volatile *)velocity)->i * linear_velocity.i;

		if (dot > 1.0f)
		{
			magnitude -= 1.0f;
			if (dot > magnitude)
				dot = magnitude;

			vector3f correction;
			local_scale3d(velocity, dot, &correction);

			linear_velocity.i -= correction.i;
			linear_velocity.j -= correction.j;
			linear_velocity.k -= correction.k;

			havok_component_rigid_body_linear_velocity_set(0, component, &linear_velocity);
		}
	}
}

struct s_tracking_source
{
	byte unknown00[0x28];
	bool unknown28;
};

struct s_tracking_target
{
	byte unknown00[0xe4];
	real unknowne4;
};

struct s_tracking_result
{
	long unknown0;
	long direction;
};

// @retail 0x1fc620
void function_1fc620(s_tracking_result *result, s_tracking_source const *source, s_tracking_target const *target)
{
	if (source->unknown28)
	{
		if (target->unknowne4 < 0.0f)
			result->direction = 0xb;
		else if (target->unknowne4 > 0.0f)
			result->direction = 0xa;
		else
			result->direction = 1;
	}
}

struct s_tracking_state
{
	byte unknown00[0x2c];
	point3f unknown2c;
	point3f unknown38;
	vector3f velocity;
	s_tracking_location location;
	byte unknown54[0xe8 - 0x54];
	vector3f unknowne8;
};

struct s_tracking_output
{
	dword flags;
	byte unknown04[0xc - 0x4];
	vector3f velocity;
	byte unknown18[0x54 - 0x18];
	bool unknown54;
	byte unknown55[3];
	point3f point;
};

// @retail 0x1fc660
void function_1fc660(s_tracked_point *tracked, s_tracking_state const *state, s_tracking_output *output)
{
	if (tracked->location.unknown0 == state->location.unknown0 && tracked->location.unknown2 == state->location.unknown2)
	{
		vector3f delta;

		vector3d_from_points3d(&state->unknown38, &state->unknown2c, &delta);
		if (length_sq3f(&delta) > 0.1f * 0.1f)
			output->flags |= 4;
	}
	if (tracked->unknown29)
	{
		output->point.x = state->unknowne8.i + tracked->offset.i;
		output->point.y = state->unknowne8.j + tracked->offset.j;
		output->point.z = state->unknowne8.k + tracked->offset.k;
		output->unknown54 = true;
		tracked->unknown29 = false;
	}
}

/* the tracked velocity, its change limited to half a unit per second */
// score 43 (was 44): the limit test is written the other way round, so the comparison matches
// retail's operand order and its branch, but the scheduler still computes the components in the
// order i, j, k where retail does j, i, k, so the tick conversion takes xmm4 where retail reuses
// xmm3, the deltas land in other registers and the limit is computed before the squared length.
// rate local with long ticks 64; the components multiplied in one expression 96; the same without
// the tick scale local 64; the components loaded first and multiplied in place 44.
// The three chains reordered to j, i, k 44 (the compiler schedules them itself, the code is
// unchanged); the chains as one expression each in that order 105; the chains as separate scalar
// locals in that order 93; the components as separate scalar locals 94 and without the velocity
// struct 106; the limit test the other way round 43.
// This round: the tick products reordered to j, i, k 43 (the code is unchanged); the three state
// loads reordered to j, i, k 47 (the chain order follows the loads, but the registers move with
// them, so retail's xmm0 = i, xmm1 = j is lost); the state velocity read into three locals and
// the chains written in j, i, k 87; the chains as one expression each without the separate loads
// 105 in that order and 104 in i, j, k (the loads then fold into the products and the code loses
// 18 bytes); the tick products written into a second vector 43 (the code is unchanged). The chain
// order follows the order of the first definitions, so it moves with the loads and cannot be
// changed on its own.
// extends the word into EDI, while ours writes the word with a 16-bit store
// to DI and loads the float into xmm0.
// retail's first 0x5a bytes (rate kept in xmm3, the three components loaded
// first) but the tick products land in fresh registers and the correction's
// last product moves late, one instruction long: rate local with long ticks 64;
// with the tick scale through local_scale3d 64; tick product and delta per
// component 64; ticks read first 64; the limit test as !(limit * limit > ...) 56.
// `long ticks` and a `rate` local give retail's first 21
// instructions (rate in xmm3, movsx edi, components loaded first) but the tick
// products go to new registers where retail scales in place 64 (369 bytes);
// limit from the rate local 68; real ticks at the top 103; real ticks after the
// rate 104; the same without the parameter reference 80.
// Round 95: the existing local_scale3d for the rate and tick stages 99 (377 bytes), for the tick
// stage only 43 (code unchanged), for the rate stage only 99; the velocity copied as a struct 132
// (394 bytes); the rate read from the global at each use 99 (375 bytes).
// @retail 0x1fc4b0
void function_1fc4b0(s_tracked_point *tracked, s_tracking_state const *state, s_tracking_output *output)
{
	vector3f velocity;
	vector3f delta;
	real limit;

	s_tracked_point *const *tracked_reference = &tracked;
	function_1fc660(*tracked_reference, state, output);
	real rate = g_510c54->rate;
	long ticks = g_510c54->field_2_3;
	real tick_scale = (real)ticks;
	velocity.i = state->velocity.i;
	velocity.j = state->velocity.j;
	velocity.k = state->velocity.k;
	velocity.i *= rate;
	velocity.i += state->unknown38.x;
	velocity.i -= state->unknown2c.x;
	velocity.j *= rate;
	velocity.j += state->unknown38.y;
	velocity.j -= state->unknown2c.y;
	velocity.k *= rate;
	velocity.k += state->unknown38.z;
	velocity.k -= state->unknown2c.z;
	velocity.i *= tick_scale;
	velocity.j *= tick_scale;
	velocity.k *= tick_scale;
	delta.i = velocity.i - state->velocity.i;
	delta.j = velocity.j - state->velocity.j;
	delta.k = velocity.k - state->velocity.k;
	limit = g_510c54->rate * 0.5f;
	if (!(limit * limit > length_sq3f(&delta)) &&
		tracked->location.unknown0 == state->location.unknown0 &&
		tracked->location.unknown2 == state->location.unknown2)
	{
		real scale = limit / ((real)sqrt(length_sq3f(&delta)) * g_510c54->rate);

		vector3f correction;
		local_scale3d(&delta, scale, &correction);
		output->velocity.i = state->velocity.i + correction.i;
		output->velocity.j = state->velocity.j + correction.j;
		output->velocity.k = state->velocity.k + correction.k;
	}
	else
	{
		output->velocity = velocity;
	}
	tracked->velocity = state->velocity;
	tracked->location = state->location;
}

struct s_actor_aim_unit
{
	byte unknown000[0x212];
	char field_212;
	byte unknown213[0x218 - 0x213];
	long weapons[4];
};

bool function_101b80(long weapon_index, short barrel_index, point3f *point);
void function_cafc0(long unit_index, point3f *position);

PRIVATE inline bool actor_weapon_position(long unit_index, point3f *position)
{
	s_actor_aim_unit *unit = (s_actor_aim_unit *)object_get(unit_index);
	short slot = unit->field_212;
	bool result = false;

	if (slot != NONE)
	{
		long weapon_index = unit->weapons[slot];
		if (weapon_index != NONE)
			result = function_101b80(weapon_index, 0, position);
	}
	return result;
}

/* Uses a weapon marker when available, otherwise the unit or actor origin. */
// @retail 0x1fc710
void function_1fc710(long actor_index, point3f *position)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown274 != NONE)
	{
		if (!actor_weapon_position(actor->unknown274, position))
			function_cafc0(actor->unknown018, position);
	}
	else if (!actor_weapon_position(actor->unknown018, position))
	{
		*position = *(point3f *)((byte *)actor + 0x22c);
	}
}
