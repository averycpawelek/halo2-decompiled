// @flags /O2 /Gr
/* UNKNOWN_26BFA0.CPP: the locations of objects and actors (lane I's outside
   functions; 0x26c180 has about 30 callers among the slot handlers) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"

/* the object as 0x26bfa0 reads it */
struct s_location_object
{
	byte unknown00[0x14];
	long parent_index;
	byte unknown18[0x30 - 0x18];
	point3f center;
	byte unknown3c[0xaa - 0x3c];
	char type;
};

struct s_location_object_header
{
	byte unknown0[8];
	s_location_object *object;
};

/* the structure bsp (g_4e0348) and its pathfinding data at +0xc4 */
struct s_location_structure_view
{
	byte unknown00[0xc4];
	long pathfinding_count;
	void *pathfinding;
};

#define LOCATION_OBJECT(index) (((s_location_object_header *)g_4e0300->data)[(index) & 0xffff].object)

void function_dfdb0(long object_index, long *unknown, long *location_index, point3f *point, long *a, long *b);
void function_f1070(long object_index, long *unknown, long *location_index, point3f *point, long *a, long *b);
bool function_210420(point3f const *point, long object_index, long marker, s_type_c3b527 *node_point); /* unknown_210420.cpp */
long function_26d100(vector3f const *up, long *unknown, s_collision_result_1697c0 *collision, point3f const *point);

/* finds the location of the object's root: a unit's (0), a vehicle's (1),
   otherwise the pathfinding location under its center */
// score 89 (was 89): Object receiver, pre-dispatch defaults, query homes and case/copy scheduling remain. Per-
// use/default-only receiver 138/102; default-only with whole center 106; early independent index
// with per-use/default-only 135/95; early marker equivalents 133/95; early index plus whole center
// 109. Original observations and case bodies restored.
// Earlier: query homes, case-tail and point-copy scheduling still differ.
// case-tail and point-copy scheduling differ; nine excess bytes remain.
// Query index captured after the location guard: kind zero/one/both 150/158/155;
// null-output-first counterparts 167/158/155; result assigned per dispatch arm 140/140.
// The first eight query forms were restored; the inherited parameter-slot read is unchanged.
// Dispatch-assigned result alone 140; separate case query outputs, late/after-guard/early result 127/94/94.
// Separate case query outputs and a dispatch-assigned result kept: plain 94 at 383/374;
// retail opening allocation agrees through +0x45. Case tail/copy schedule and 9 bytes remain.
// Round 246: Actual result output/shared point/chained defaults, seven combinations: 117/94/117/94/117/94/117/94.
// Eighth form uses distinct first/second query scalar names.
// Common scalar outputs before/after root lookup; after lookup with shared point/whole center: 143/140/140/114.
// All trial forms restored; no candidate qualifies.
// Round 248: Shared index/object/both/marker/index-marker/object-marker query outputs
// 100/101/130/101/130/130; early shared index-object/index-marker 136/136.
// Original body restored; pre-dispatch initialization, output homes and case tails remain.
// Round 251: independent result/owner/marker query-home names, all seven subsets 89 each; complete point-home names 89. Original independent case outputs restored. Earlier location-slot volatile read 94 was dropped in review.
// Round 253: Independent early per-case scalar roles
// index/object/index-object/marker/index-marker/object-marker/all 82/82/85/83/84/83/124; all with
// independent early points 124. Early indices with native center copy/point setter 102/82.
// Seven-point forms excluded; original complete body restored.
// @retail 0x26bfa0
void function_26bfa0(long object_index, long *location_index, s_location_view *location)
{
	long result;
	long root = NONE;

	while (object_index != NONE)
	{
		root = object_index;
		object_index = LOCATION_OBJECT(object_index)->parent_index;
	}

	s_location_object *object = LOCATION_OBJECT(root);

	switch (object->type)
	{
	case 0:
	{
		long query_index = NONE;
		long query_object = NONE;
		long query_marker = NONE;
		point3f query_point;
		function_dfdb0(root, 0, &query_index, &query_point, &query_object, &query_marker);
		result = query_index;
		if (location)
		{
			if (result != NONE)
			{
				function_210420(&query_point, query_object, query_marker, &location->point);
			}
			else
			{
				location->point.point = query_point;
				location->point.output_index = NONE;
			}
		}
		break;
	}
	case 1:
	{
		long query_index = NONE;
		long query_object = NONE;
		long query_marker = NONE;
		point3f query_point;
		function_f1070(root, 0, &query_index, &query_point, &query_object, &query_marker);
		result = query_index;
		if (location)
		{
			if (result != NONE)
			{
				function_210420(&query_point, query_object, query_marker, &location->point);
			}
			else
			{
				location->point.point = query_point;
				location->point.output_index = NONE;
			}
		}
		break;
	}
	default:
		{
			result = NONE;
			point3f query_point;
			s_location_structure_view *structure = (s_location_structure_view *)g_4e0348;

			if (structure->pathfinding_count > 0 && structure->pathfinding)
			{
				struct { s_collision_result_1697c0 collision; long field_4c, field_50, field_54, field_58; } contact;

				query_point.x = object->center.x;
				query_point.y = object->center.y;
				query_point.z = object->center.z;
				contact.collision.unknown24 = NONE;
				result = function_26d100(g_4687b0, (long *)location, &contact.collision, &query_point);
			}
		}
		break;
	}
	if (location_index)
	{
		*location_index = result;
	}
}

/* the actor (0x888 bytes) as 0x26c180 reads it */
struct s_actor_location_view
{
	byte unknown000[0x18];
	long unit_index;
	byte unknown01c[0x229 - 0x1c];
	bool unknown229;
	byte unknown22a[0x238 - 0x22a];
	point3f position;
	byte unknown244[0x26c - 0x244];
	long unknown26c;
	byte unknown270[0x278 - 0x270];
	bool location_valid;
	byte unknown279[0x27c - 0x279];
	s_location_view location;
};

void function_1caa40(long object_index, point3f *position);
bool function_26be90(long object_index);

/* computes the actor's location once per update */
// @retail 0x26c180
void function_26c180(long actor_index)
{
	s_actor_location_view *actor = (s_actor_location_view *)actor_get(actor_index);

	if (!actor->location_valid)
	{
		actor->location.point.point = actor->position;
		actor->location_valid = true;
		actor->location.point.output_index = NONE;
		actor->location.unknown10 = NONE;
		if (actor->unknown229)
		{
			long object_index = actor->unknown26c;

			if (object_index == NONE)
			{
				object_index = actor->unit_index;
			}
			function_1caa40(object_index, &actor->location.point.point);
		}
		else if (actor->unknown26c != NONE)
		{
			function_26bfa0(actor->unknown26c, &actor->location.unknown10, &actor->location);
		}
		else
		{
			long unit_index = actor->unit_index;

			if (unit_index != NONE && function_26be90(unit_index))
			{
				function_26bfa0(unit_index, &actor->location.unknown10, &actor->location);
			}
		}
	}
}

bool function_26be60(long object_index);

// @retail 0x26c120
bool function_26c120(long actor_index)
{
	s_actor_location_view *actor = (s_actor_location_view *)actor_get(actor_index);
	if (!actor->unknown229 && actor->unknown26c == NONE)
	{
		long object_index = actor->unit_index;
		if (object_index != NONE && ((s_object_header_view *)g_4e0300->data)[object_index & 0xffff].type == 0)
			return function_26be60(object_index);
	}
	return true;
}

struct s_location_target_view
{
	byte unknown00[0x3c];
	long object_index;
	byte unknown40[4];
	long location_index;
	s_location_view location;
};

long function_1e3480(long object_index);

// @retail 0x26c240
void function_26c240(long object_index, s_location_target_view *target)
{
	long actor_index = function_1e3480(object_index);
	if (actor_index != NONE)
	{
		s_actor_location_view *actor = (s_actor_location_view *)actor_get(actor_index);
		function_26c180(actor_index);
		target->location_index = actor->location.unknown10;
		target->location.point = actor->location.point;
	}
	else if (target->object_index != NONE)
	{
		function_26bfa0(target->object_index, &target->location_index, &target->location);
	}
	else
	{
		function_26bfa0(object_index, &target->location_index, &target->location);
	}
}


// @retail 0x26c2d0
void __stdcall function_26c2d0(long prop_index)
{
 s_prop_node_view *node = (s_prop_node_view *)(g_502418->data + (prop_index & 0xffff) * 0x3c);
 struct s_prop_location_cache_view
 {
  long unknown00;
  point3f position;
  byte unknown10[0x44 - 0x10];
  long location_index;
  s_type_c3b527 location;
  bool valid;
 };
 s_prop_location_cache_view *state = (s_prop_location_cache_view *)function_25d690((s_prop_datum *)node);
 short kind = *(short *)((byte *)node + 0x24);
 if (kind >= 1 && kind <= 2)
 {
  long object_index = *(long *)((byte *)node + 0x20);
  if (function_26be90(object_index) || !state->valid)
  {
   function_26c240(object_index, (s_location_target_view *)state);
   state->valid = true;
  }
 }
 else if (!state->valid)
 {
  s_location_structure_view *structure = (s_location_structure_view *)g_4e0348;
  struct
  {
   void *pathfinding;
   s_collision_result_1697c0 collision;
   byte unknown50[0x60 - 0x50];
  } request;
  request.pathfinding = 0;
  if (structure->pathfinding_count > 0)
   request.pathfinding = structure->pathfinding;
  request.collision.unknown24 = NONE;
  state->location_index = function_26d100(g_4687b0, (long *)&state->location, &request.collision, &state->position);
  state->valid = true;
 }
}

