// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_10AF20.CPP: object changes the scripts make: attaching an object to
   another's marker, scaling an object and its children over time, and
   setting an object's velocity in its own frame */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"

/* the object (a view of the object data) */
struct s_scripted_object
{
	long definition_index;
	byte unknown004[0xc - 4];
	long next_object_index;
	long first_child_index;
	long parent_index;
	byte unknown018[0x70 - 0x18];
	vector3f forward;
	vector3f up;
	byte unknown088[0xaa - 0x88];
	byte type;
	byte unknown0ab[0x12c - 0xab];
	dword flags_12c;
};

struct s_scripted_object_header
{
	byte unknown00[8];
	s_scripted_object *object;
};

#define SCRIPTED_OBJECT_GET(index) (((s_scripted_object_header *)g_4e0300->data)[(index) & 0xffff].object)

void __stdcall function_b8ee0(long parent_index, long marker_name, long object_index, long a);
void function_b7680(long object_index, real scale, real seconds);
void function_b7740(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity,
	bool unknown);
void function_1c4b00(long object_index, void *a, void *b, long c);
void __stdcall function_b9b90(long object_index, bool disable);
void function_b7360(long object_index);
void function_bba20(long object_index);

/* attaches an object that has no parent to a marker of another object */
// @retail 0x10af20
void function_10af20(long object_index, long marker_name, long other_object_index, long other_marker_name)
{
	if (object_index != NONE && other_object_index != NONE)
	{
		s_scripted_object *other = SCRIPTED_OBJECT_GET(other_object_index);

		if (other->parent_index == NONE && (!((1 << other->type) & 0x1c) || !(other->flags_12c & 1)))
			function_b8ee0(object_index, marker_name, other_object_index, other_marker_name);
	}
}

/* scales an object and its children over a number of ticks */
// @retail 0x10af80
void __stdcall function_10af80(long object_index, real scale, short ticks)
{
	if (object_index != NONE)
	{
		long child_index = SCRIPTED_OBJECT_GET(object_index)->first_child_index;

		function_b7680(object_index, scale, (real)ticks * (1.0f / 30.0f));
		while (child_index != NONE)
		{
			s_scripted_object *child = SCRIPTED_OBJECT_GET(child_index);

			function_10af80(child_index, scale, ticks);
			child_index = child->next_object_index;
		}
	}
}

PRIVATE __forceinline void scripted_cross_product(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = a->j * b->k - a->k * b->j;
	result->j = a->k * b->i - a->i * b->k;
	result->k = a->i * b->j - a->j * b->i;
}

PRIVATE __forceinline void scripted_vector_scale(vector3f const *vector, real scale, vector3f *result)
{
	result->i = vector->i * scale;
	result->j = vector->j * scale;
	result->k = vector->k * scale;
}

PRIVATE __forceinline void scripted_vector_add_scaled(vector3f const *a, vector3f const *b,
	real scale, vector3f *result)
{
	result->i = a->i + b->i * scale;
	result->j = a->j + b->j * scale;
	result->k = a->k + b->k * scale;
}

/* sets an object's linear velocity from speeds along its forward, left and up
   axes */
// score 76 (was 76): retail keeps the three components of the velocity in
// registers and stores them only just before the two calls, so its frame is
// twelve bytes smaller than ours, and it emits the scaled products in a
// different order. Round 81 (score 76): `forward`/`up` pointer locals for the cross product
// alone 74 (retail's product operand order); velocity written directly without the
// scaled copy 80 (retail's 0xc frame); left_part/up_part vectors 79-83; left scaled in
// place 83; velocity = left * left_speed first 85-87 (forward_speed leaves xmm6)
// Round 197 fast trials: Existing cross helper with up/forward/both scalar inputs, output
// last/first and actual vector return 84, 72, 95, 83, 72, 95, 156, 153.
// Forward scalar inputs with up by pointer: 72 with output first or last; four-point lead reverted.
// Round 77 (plain): the scale/add/add helper chain straight into velocity 84 (0xc frame, but each
// sum commutes: retail accumulates in the forward product's register); chain into the working
// vector with the last step writing velocity 86; per-component three-term sums 93 and through
// vector3f_assign 93 (both need the two helpers as plain definitions, which 0x1c1e70 accepts);
// forward and left summed per component, up through add-scaled 93. Retail stores nothing of
// velocity until all three sums are done and loads up.j early (xmm7), spilling two left products.
// @retail 0x10b010
void function_10b010(long object_index, real forward_speed, real left_speed, real up_speed)
{
	if (object_index != NONE)
	{
		s_scripted_object *object = SCRIPTED_OBJECT_GET(object_index);
		vector3f left;
		vector3f scaled;
		vector3f velocity;

		scripted_cross_product(&object->up, &object->forward, &left);
		scripted_vector_scale(&object->forward, forward_speed, &scaled);
		scaled.j = scaled.j + left.j * left_speed;
		scaled.k = scaled.k + left.k * left_speed;
		scaled.i = scaled.i + left.i * left_speed;
		scripted_vector_add_scaled(&scaled, &object->up, up_speed, &scaled);
		velocity.i = scaled.i;
		velocity.j = scaled.j;
		velocity.k = scaled.k;
		function_b7740(object_index, &velocity, NULL, false);
		function_1c4b00(object_index, &velocity, NULL, true);
		if (velocity.i * velocity.i + velocity.j * velocity.j + velocity.k * velocity.k > 0.0001f)
		{
			function_b9b90(object_index, false);
			function_b7360(object_index);
			function_bba20(object_index);
		}
	}
}
