// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2605d0.h"
#include "unknown_2626b0.h"

/* slot type 0x6b, and the evaluate callback many handlers share */

short __stdcall function_1bcc90(long actor_index);
bool __stdcall function_1bcd00(long actor_index, s_slot *slot);
real function_1f8940(long actor_index);

/* iterates the squads of an encounter (function_204ec0 and function_205010) */
struct s_squad_iterator
{
	byte unknown00[0x14];
};

void function_204ec0(s_squad_iterator *iterator, short encounter_index, short a, short b);
short function_205010(s_squad_iterator *iterator);

// @retail 0x1bcc90
short __stdcall function_1bcc90(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;
	s_squad_iterator iterator;

	function_204ec0(&iterator, (short)actor->unknown030, 5, actor->unknown26c != NONE);
	if (function_205010(&iterator) != NONE)
		result = 1;
	return result;
}

// score 27 (was 35): left is the returned reference: retail keeps it in EAX through
// its test and copies it to EDI only at the second call (so the second call's
// arguments load in another order); ours copies it at once. 0x1b3380 shows the same
// tail. Kept (round 78): the actor pointer declared before the active test, as
// 0x1b3380 does (the 0x328 compare is then scheduled above the clear as retail).
// Earlier: the test in a bool before the clear 102; the 0x69c store first 36; the
// second result named 35; the reference declared at function scope 35; the clear as
// `= {}` 35; carrier role names (all seven buffer/index/reference combinations) 35
// each; immutable returned reference 35; inverted reference test with the follow in
// the first arm 47; request declared at function scope 35; one actor pointer for the
// active test and the body 78; the 0x328 test through a fresh actor get 81; the test
// negated 35; compile-only screens of the clear (sizeof type, one-element array,
// byte-pointer memset, scratch first, const actor, request first) all keep the
// compare after rep stos. Round 78, after the kept change: the reference test's
// operands swapped 27; the second call's result named in a nested if 27; the
// reference declared at the block top 27; result declared after the actor 58; the
// scratch buffer declared where it is fetched 27; a
// compile-only screen of all 24 orders of the block's declarations, a reference
// copy and field-wise compares keeps the early copy.
// retail tests the value at 0x328 before it clears the
// block, ours clears the block first and tests it afterwards, so the setle
// lands in another place; retail also keeps the returned reference in EAX
// through its test and copies it to EDI only at the second call. A compile-only
// test moves the 0x328 compare above the clear only when the cleared block's
// address is never taken. Tried: the test in a bool before the clear 102; the
// 0x69c store first 36; the second result named 35; the reference declared at
// function scope 35; the clear as `= {}` 35; a copy of the reference for the
// last call 35; the follow arm's tests nested 35; the scratch buffer taken
// before the clear 72; the clear sized by the type 35; the flag as a ternary 35.
// function scope 35; the clear as `= {}` 35.
// Carrier role names (all seven buffer/index/reference combinations) 35 each;
// immutable returned reference 35.
// Lane 11 r12 (aligned counts, base 16): the 0x328 test in a bool before the
// clear 78 (the entry allocation changes); the reference in a union with a dword
// view (0x1b1f70's form) 16, identical code; the arms swapped 46; the call
// assigned inside the test 16, identical code; the 0x328 field read through a
// fresh actor lookup 43.
// function scope 35; the clear as `= {}` 35. Round 75: inverted reference test with
// the follow in the first arm 47; request declared at function scope 35; one actor
// pointer for the active test and the body 78; the 0x328 test through a fresh
// actor get 81; the test negated 35. Compile-only screens of the clear (sizeof type,
// one-element array, byte-pointer memset, scratch first, const actor, request first)
// all keep the compare after rep stos; retail's compare-before-clear occurs in no
// other retail function.
// @retail 0x1bcd00
bool __stdcall function_1bcd00(long actor_index, s_slot *slot)
{
	bool result = true;
	s_actor_view *actor = actor_get(actor_index);

	if (actor_get(actor_index)->unknown040)
	{
		byte *scratch;
		s_2605d0_request request;
		long other_index;
		bool unknown;

		if (!REFERENCE_EQUAL(actor->unknown418, g_470fa0) &&
			(actor->unknown504 == 2 || (actor->unknown225 ? 1.2f : 0.6f) > function_1f8940(actor_index)))
		{
			function_262800(actor_index, actor->unknown418, false);
		}

		memset(&request, 0, sizeof(request));
		request.type = 4;
		request.unknown015 = true;
		request.unknown05b = true;
		request.unknown69c = actor->unknown328 <= 2;
		scratch = ai_scratch_buffer_get();
		s_reference reference = function_2605d0(actor_index, &request, 0, (long)&other_index, scratch, &unknown);
		if (REFERENCE_EQUAL(reference, g_470fa0))
		{
			actor_get(actor_index)->unknown040 = false;
			result = false;
		}
		else if (REFERENCE_EQUAL(function_2626b0(actor_index, reference, other_index, scratch, unknown, true), g_470fa0) &&
			(actor->unknown5b4 > 4 || actor->unknown26c != NONE))
		{
			function_262800(actor_index, reference, false);
		}
		ai_scratch_buffer_release(scratch);
	}
	return result;
}

// @retail 0x1bce80
void __stdcall function_1bce80(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown4ae = true;
	actor->unknown484 = true;
	actor->unknown485 = true;
	if (actor->unknown26c != NONE)
		actor->unknown4b4 = 0.6f;
}

// @retail 0x1bced0
short __stdcall function_1bced0(long actor_index, s_slot *slot, bool active)
{
	return g_46fbe8;
}

s_slot_handler_2 g_47eb58 =
{
	{
		0x6b, 2, 0xfff, -2, 0,
		function_1bcc90, function_1bced0, slot_start_true, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1bcd00, 0, function_1bce80
};
