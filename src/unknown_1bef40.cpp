// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259d0.h"
#include "unknown_1fb7e0.h"
#include "unknown_2605d0.h"
#include "unknown_2626b0.h"
#include "unknown_20fe20.h"
#include "unknown_1e1f20.h"

extern "C" void _ReadWriteBarrier(void);

/* slot types 0xa and 0xb */

struct s_slot_0a
{
	s_slot_header header;
	s_reference reference;
	short unknown10;
	short unknown12;
	byte unknown14[0x40 - 0x14];
};

struct s_slot_0a_data
{
	s_reference reference;
	short unknown04;
	short unknown06;
	byte unknown08[0x34 - 0x08];
};

struct s_slot_0b
{
	s_slot_header header;
	word unknown0c;
	byte unknown0e[2];
	long unknown10;
	byte unknown14[0x40 - 0x14];
};

short actor_behavior_find_level(long owner_index, short type);
short __stdcall function_1b7920(long actor_index);

short __stdcall function_1bef40(long actor_index);
short __stdcall function_1bf0f0(long actor_index, s_slot *slot, bool active);
bool __stdcall function_1bf230(long actor_index, s_slot *slot);
bool __stdcall function_1bf4e0(long actor_index, s_slot *slot);
void __stdcall function_1c1520(long actor_index, s_slot *slot, long index);
void __stdcall function_1bf5c0(long actor_index, s_slot *slot);

/* the block of the actor's character tag function_1e4d10 returns */
struct s_character_d10
{
	byte unknown00[0x2c];
	real unknown2c;
};

void *function_1e4d10(long actor_index);
void function_1f86a0(long index);

short function_2684f0(s_actor_view *actor);
short *function_e70b0(long unit_index, short *value);
long function_1469f0(real seconds);

#pragma intrinsic(_ReadWriteBarrier)
// score 13 (was 13): retail reads the slot reference before actor arithmetic and the object table
// after index arithmetic; ours has the opposite order in each case.
// Slot data block 13; without the barrier 90; action helper result 13; actor/state/result 17;
// result/state/actor 13; state/result/actor 13; result/actor/state 17; actor/result/state 17.
// Packed reference field view/local names/entry declarations/actor-result scope/action lifetime 13;
// presence-first branch 91; branch-local actor lifetime 96; shared entry exit 13.
// Native field view and all scope forms restored; both remaining load regions still differ.
// Early reference predicate/default-first/wide-result/presence predicate 109/108/111/109;
// complete current/both reference values 100/104; native word equality logical/bitwise 222/130.
// @retail 0x1bf0f0
short __stdcall function_1bf0f0(long actor_index, s_slot *slot, bool active)
{
	s_slot_0a_data *state = (s_slot_0a_data *)slot->unknownc;
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe8;
	_ReadWriteBarrier();
	if (REFERENCE_EQUAL(state->reference, g_470fa0))
	{
		function_1f86a0(actor_index);
	}
	else if (actor->unknown504 == 2)
	{
		short action;
		if (*function_e70b0(actor->unknown018, &action) == 5)
		{
			if (state->unknown06 > 0)
				state->unknown06--;
			else if (function_2684f0(actor) >= 10)
			{
				real delay = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 3.0f, 5.0f);
				real wait = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 0.0f, 0.5f);
				state->unknown04 = (short)function_1469f0(delay);
				state->unknown06 = (short)function_1469f0(wait);
				function_262800(actor_index, state->reference, false);
				function_1f86a0(actor_index);
				state->reference = g_470fa0;
			}
			else if (--state->unknown04 <= 0)
			{
				g_46eeb8[0x2b]->unknown8 = g_46f348;
				g_46eeb8[0xa]->unknown8 = g_46f348;
				result = g_46fbe4;
			}
		}
		else
		{
			result = g_46fbe4;
		}
	}
	return result;
}
#pragma function(_ReadWriteBarrier)

// @retail 0x1bef40
short __stdcall function_1bef40(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (actor->prop_index != NONE)
	{
		s_character_d10 *character = (s_character_d10 *)function_1e4d10(actor_index);

		if (character)
		{
			s_prop_node_view *node = prop_node_get(actor->prop_index);

			if (node->unknown24 >= 1 && node->unknown24 <= 2 && character->unknown2c * 2.0f > node->unknown28)
				result = 3;
		}
	}
	return result;
}

PRIVATE __forceinline void function_1befd1(long arg_0)
{
    s_actor_view *local_0 = actor_get(arg_0);
    *(bool volatile *)&local_0->unknown50c = false;
    *(short volatile *)&local_0->unknown5b4 = 0;
    *(short volatile *)&local_0->unknown5b6 = 0;
    *(long volatile *)&local_0->unknown5ac = NONE;
    *(short volatile *)&local_0->unknown5b0 = NONE;
    local_0->unknown4ac = 0;
    local_0->unknown504 = 0;
}

static __forceinline long real_to_long(real value);

// score 35 (was 35): our frame reserves eight bytes where retail reserves four, so the two temporaries
// sit four bytes higher; retail also stores the actor's 0x5b0 field before its 0x4ac and 0x504 fields
// and loads the tick source after the slot reference. Retail's x87 order is the inlined seconds-to-ticks
// body on each draw: the first draw finished before the second starts and kept on the FPU (fild ticks;
// fmul st(1); fstp st(0)), the second scaled late and spilled to the frame's one slot just before the
// first fld/fistp. Named reals give retail's frame but finish the first draw late and spill the second
// unscaled right after it is drawn (then fimul): reals 53, doubles 39, double/real 49, real/double 52;
// the header random-range helper 55 (57 in the pair); the reset without its volatile casts 43 in either
// store order (40 now); named reals through a seconds-to-ticks inline 53 (also with the parameter scaled
// in place as 0x1469f0 does, 53); the existing actor_reset_state for the reset 40 (0x5ac/0x5b0 stores
// after the zero stores), with named reals 55, doubles 41, double/real 51, real/double 54; named reals
// declared first/at the top/wait first 55; product operands swapped, an explicit (real) tick conversion
// or the seconds-to-ticks inline 55 each; the slot data block through slot->unknownc 80 (retail folds
// +0xc into each access); the existing out-of-line reset called 115 (its file is /Ob1, so it stays a
// call); named reals with the reset's plain stores in the out-of-line order 55, with the products
// written seconds first 53; existing reset call with native payload and original/scalar delay storage,
// ordinary/forced reset declaration 121/126/121/126; whole slot 115/125/115/125 (219/211 or 213/205
// bytes); the aggregate draws through a seconds-to-ticks inline (parameter scaled in place) 35
// (identical code); an in-file forceinline/inline copy of the range helper with named reals 53 (55
// with actor_reset_state). Lead, not kept: named reals drawn as `(real)(3.0 + 2.0 * <random>)` and
// `(real)(0.5 * <random>)` from the raw generator give retail's first-draw x87 variable and frame
// (26; 24 with actor_reset_state or the reset's plain stores, which also drops the draw struct and the
// volatile casts), but their double constants load as qword where retail's are dword, and the second
// draw is scaled and spilled before the reset (retail: just before the first fld/fistp); float
// constants (`3.0f + 2.0f * <random>`, also `(5.0f - 3.0f)`) fold or substitute: 49 (270 bytes),
// `(double)` on the random alone 26 (still qword).
// our frame reserves eight bytes where retail reserves four, so the two
// temporaries sit four bytes higher; retail also scales the first draw before it converts the
// second one, loads the tick source after the slot reference (which puts the reference in ECX, not
// EDX; ours hoists that global above the reference store) and stores the actor's 0x5b0 field
// before its 0x4ac and 0x504 fields. Named reals give retail's frame but finish both range products
// late (second draw spilled unscaled): reals 53, doubles 39, double/real 49, real/double 52; the
// header random-range helper 55 (57 in the pair); the reset without its volatile casts 43 in either
// store order; named reals through a seconds-to-ticks inline 53. Existing reset call with native
// payload and original/scalar delay storage, ordinary/forced reset declaration: 121/126/121/126;
// whole slot 115/125/115/125 (219/211 or 213/205 bytes). Round 74: the slot data block through
// slot->unknownc 80 (retail folds +0xc into each access); the existing out-of-line reset called 115
// (its file is /Ob1, so it stays a call); named reals 53, with the reset's plain stores in the
// out-of-line order 55, with the products written seconds first 53. The named reals' first draw
// sinks to the reset (fimul), where the aggregate field keeps it early. Round 5: the record's two
// fields declared the other way round 39 (284 bytes); the time globals in a local used twice 35;
// the products written value first 35; the reference stored before the reset 71 (290 bytes); the
// 0x5b0 store without its volatile cast 36, the 0x5ac and 0x5b0 stores without theirs 38.
// our frame reserves eight bytes where retail reserves four, so the two temporaries
// sit four bytes higher; retail also scales the first draw before it converts the second one, loads
// the tick source after the slot reference (which puts the reference in ECX, not EDX) and stores the
// actor's 0x5b0 field before its 0x4ac and 0x504 fields. Named reals give retail's frame but finish
// both range products late (second draw spilled unscaled): reals 53, doubles 39, double/real 49,
// real/double 52; the header random-range helper 55 (57 in the pair); the reset without its volatile
// casts 43 in either store order; named reals through a seconds-to-ticks inline 53. Round 74: the slot
// data block through slot->unknownc 80 (retail folds +0xc into each access); the existing out-of-line
// reset called 115 (its file is /Ob1, so it stays a call); named reals 53, with the reset's plain
// stores in the out-of-line order 55, with the products written seconds first 53. The named reals'
// first draw sinks to the reset (fimul), where the aggregate field keeps it early.
// our frame reserves eight bytes where retail reserves
// four, so the two temporaries sit four bytes higher; retail also scales the
// first draw before it converts the second one, loads the tick source after
// the slot reference (which puts the reference in ECX, not EDX) and stores the
// actor's 0x5b0 field before its 0x4ac and 0x504 fields. Named reals give
// retail's frame but finish both range products late (second draw spilled
// unscaled): reals 53, doubles 39, double/real 49, real/double 52; the header
// random-range helper 55 (57 in the pair); the reset without its volatile
// casts 43 in either store order; named reals through a seconds-to-ticks
// inline 53; the existing actor_reset_state for the reset 40 (0x5ac/0x5b0
// stores after the zero stores); with named reals 55, doubles 41, double/real
// 51, real/double 54; named reals declared first/at the top/wait first 55;
// product operands swapped, an explicit (real) tick conversion or the
// seconds-to-ticks inline 55 each. Named reals spill the second draw before
// the reset where retail keeps both draws on the FPU stack until the first
// conversion.
// inline 53.
// Existing reset call with native payload and original/scalar delay storage, ordinary/forced
// reset declaration: 121/126/121/126; whole slot 115/125/115/125 (219/211 or 213/205 bytes).
// The reset stays out of line; every existing reset store and the opening body are restored.
// our frame reserves eight bytes where retail reserves four, so the two temporaries
// our frame reserves eight bytes where retail reserves four, so the two
// temporaries sit four bytes higher; retail also scales the first draw before it converts the
// second one, loads the tick source after the slot reference (which puts the reference in ECX, not
// EDX; ours hoists that global above the reference store) and stores the actor's 0x5b0 field
// before its 0x4ac and 0x504 fields. Named reals give retail's frame but finish both range products
// late (second draw spilled unscaled): reals 53, doubles 39, double/real 49, real/double 52; the
// header random-range helper 55 (57 in the pair); the reset without its volatile casts 43 in either
// store order; named reals through a seconds-to-ticks inline 53. Existing reset call with native
// payload and original/scalar delay storage, ordinary/forced reset declaration: 121/126/121/126;
// whole slot 115/125/115/125 (219/211 or 213/205 bytes). Round 74: the slot data block through
// slot->unknownc 80 (retail folds +0xc into each access); the existing out-of-line reset called 115
// (its file is /Ob1, so it stays a call); named reals 53, with the reset's plain stores in the
// out-of-line order 55, with the products written seconds first 53. The named reals' first draw
// sinks to the reset (fimul), where the aggregate field keeps it early. Round 5: the record's two
// fields declared the other way round 39 (284 bytes); the time globals in a local used twice 35;
// the products written value first 35; the reference stored before the reset 71 (290 bytes); the
// 0x5b0 store without its volatile cast 36, the 0x5ac and 0x5b0 stores without theirs 38.
// @retail 0x1befd0
bool __stdcall function_1befd0(long actor_index, s_slot *slot)
{
	struct s_1befd4 { real field_0; real field_4; } local_0;
	s_slot_0a *state = (s_slot_0a *)slot;
	local_0.field_4 = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 3.0f, 5.0f);
	local_0.field_0 = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 0.0f, 0.5f);

	function_1befd1(actor_index);
	state->reference = g_470fa0;

	state->unknown10 = (short)real_to_long(g_510c54->field_2_3 * local_0.field_4);
	state->unknown12 = (short)real_to_long(g_510c54->field_2_3 * local_0.field_0);
	return true;
}

// @retail 0x1bf230
bool __stdcall function_1bf230(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_0a *state = (s_slot_0a *)slot;
	bool result = true;

	if (!REFERENCE_EQUAL(state->reference, g_470fa0) && actor->unknown5b4 > 0)
		state->reference = g_470fa0;
	if (actor->unknown040)
	{
		long other_index = NONE;

		result = false;
		if (REFERENCE_EQUAL(state->reference, g_470fa0))
		{
			byte *scratch = ai_scratch_buffer_get();
			s_2605d0_request request;
			bool unknown;

			memset(&request, 0, sizeof(request));
			unknown = false;
			request.type = 7;
			request.unknown008 = 5.0f;
			request.unknown00c = 5.0f;
			request.unknown056 = true;
			request.unknown057 = true;
			request.unknown010 = 10.0f;
			state->reference = function_2605d0(actor_index, &request, 0, (long)&other_index, scratch, &unknown);
			ai_scratch_buffer_release(scratch);
			if (REFERENCE_EQUAL(state->reference, g_470fa0))
				goto done;
		}
		state->reference = function_2626b0(actor_index, state->reference, other_index, NULL, false, true);
		result = !REFERENCE_EQUAL(state->reference, g_470fa0);
	}
done:
	return result;
}

// @retail 0x1bf360
void __stdcall function_1bf360(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);

		if (node->unknown24 >= 1 && node->unknown24 <= 2)
		{
			actor->unknown41c = 3;
			actor->unknown420 = 2;
			actor->unknown488 = true;
		}
	}
}

// @retail 0x1bf3d0
void __stdcall function_1bf3d0(long actor_index, s_slot *slot, bool active)
{
	s_slot_0a *state = (s_slot_0a *)slot;

	if (!active || (state->reference.unknown2 & 0x8000))
		state->reference = g_470fa0;
}

// @retail 0x1bf3f0
bool function_1bf3f0(long actor_index, long other_index, short type)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);

	if (other_index != NONE)
	{
		s_actor_view *other = actor_get(other_index);

		if (other->prop_index != NONE && actor->prop_index != NONE &&
			prop_node_get(actor->prop_index)->unknown08 == prop_node_get(other->prop_index)->unknown08 &&
			actor_behavior_find_level(other_index, type) != NONE)
		{
			result = true;
		}
	}
	return result;
}

// @retail 0x1bf480
short __stdcall function_1bf480(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (function_1b7920(actor_index) && function_1bf3f0(actor_index, actor->unknown3cc, actor->unknown3d0))
		result = 3;
	return result;
}

// @retail 0x1bf4e0
bool __stdcall function_1bf4e0(long actor_index, s_slot *slot)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);

	if (function_1bf3f0(actor_index, actor->unknown3cc, actor->unknown3d0))
	{
		s_slot_0b *state = (s_slot_0b *)slot;
		short type;

		state->unknown10 = actor->unknown3cc;
		state->unknown0c = actor->unknown3d0;
		type = actor->unknown3d2;
		actor->unknown3cc = NONE;
		if (type != NONE)
			function_1fb7e0(type, actor_index, NULL, actor_get(state->unknown10)->unknown018, NONE);
		function_1f86a0(actor_index);
		result = true;
	}
	return result;
}

// @retail 0x1bf590
short __stdcall function_1bf590(long actor_index, s_slot *slot, bool active)
{
	s_slot_0b *state = (s_slot_0b *)slot;
	short result = g_46fbe4;

	if (function_1bf3f0(actor_index, state->unknown10, state->unknown0c))
		result = g_46fbe8;
	return result;
}

/* how the actor's character uses its weapon, by difficulty */
struct s_weapon_difficulty_entry_0b
{
	long unknown0;
	long unknown4;
	long unknown8;
};

struct s_character_weapon_0b
{
	byte unknown00[0x98];
	s_weapon_difficulty_entry_0b difficulty[3];
};

/* aims the actor at its prop: held props are only approached */
// @retail 0x1bf5c0
void __stdcall function_1bf5c0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_prop_node_view *node = prop_node_get(actor->prop_index);
	s_prop_state_view *s_type_5cfb45 = prop_node_state(node);
	s_prop_view_fields *view = prop_node_view(node);
	long weapon_index = function_1e1f20(actor_index);

	if (node->unknown24 >= 1 && node->unknown24 <= 2)
	{
		actor->unknown488 = true;
		actor->unknown41c = 4;
		actor->unknown420 = 2;
	}
	else if (view)
	{
		s_type_c3b527 point = view->unknown18;
		point3f position;

		point.point.z += s_type_5cfb45->unknown38 - s_type_5cfb45->position.z;
		function_210850(&point, &position);
		actor->unknown41c = 3;
		actor->unknown420 = 3;
		actor->unknown424.point = position;
		actor->unknown430 = 2;
		actor->unknown434 = 3;
		actor->unknown438.point = position;
		actor->unknown444 = NONE;
		actor->unknown488 = true;
		actor->unknown4a0 = true;
		actor->unknown48c = true;
		actor->unknown490_point = point;
		if (weapon_index != NONE)
		{
			s_character_weapon_0b *weapon = (s_character_weapon_0b *)function_1e5280(actor_index, object_get(weapon_index)->tag_index);

			if (weapon)
			{
				s_weapon_difficulty_entry_0b *entry;
				short local_0 = g_4e6948->state == 1 ? g_4e6948->difficulty : 1;

				switch (local_0)
				{
				case 2:
					entry = &weapon->difficulty[1];
					break;
				case 3:
					entry = &weapon->difficulty[2];
					break;
				default:
					entry = &weapon->difficulty[0];
					break;
				}
				actor->unknown710 = entry->unknown4;
			}
		}
	}
}

s_slot_handler_2 g_47ee18 =
{
	{
		0xa, 2, 0x7ff, -2, 0,
		function_1bef40, function_1bf0f0, function_1befd0, slot_proc_nothing, NONE, {0},
		0, 0, 0, 0, function_1bf3d0, 0, 0
	},
	(t_slot_proc)function_1bf230, 0, function_1bf360
};

s_slot_handler_2 g_47ee68 =
{
	{
		0xb, 2, 0, -2, 0,
		function_1bf480, function_1bf590, function_1bf4e0, 0, NONE, {0},
		function_1c1520, 0, 0, 0, 0, 0, 0
	},
	0, 0, function_1bf5c0
};
