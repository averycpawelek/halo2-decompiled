// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "local_cameras.h"
#include "object_queries.h"

struct s_unknown_13bf00;
struct s_object;
extern s_unknown_13bf00 *g_510c50;
extern s_record_pool *g_4ed28c;
extern dword g_54e8a0[16];

struct s_18c060
{
	short field_0;
	byte field_2;
	byte field_3;
	byte field_4;
	byte field_5;
	short field_6;
	real field_8;
	long field_c;
	long field_10;
	long field_14;
};

struct s_18c061
{
	byte field_0[0x78];
	bool field_78;
	bool field_79;
	bool field_7a;
	byte field_7b[3];
	bool field_7e;
};

struct s_18c062
{
	byte field_0[5];
	bool field_5;
};

void function_127400(bool arg_0);
void sound_set_ambience(long arg_0);
void sound_audible_clusters_update(void);
void __stdcall function_18bb80(real arg_0);
void __stdcall function_225b60(real arg_0);
void function_18bf90(void);
void sound_effects_update(void);
void __stdcall function_18a7b0(long arg_0);
s_object *function_badc0(long arg_0, dword arg_1);
long __stdcall function_18d1c0(long arg_0);
bool function_b9d20(long arg_0);

#pragma inline_depth(0)
// score 107 (was 107): the pool is not kept in EBX across the loop (retail reloads it only after
// 0x18a7b0), so the scan/datum/handle registers differ; the root-location call differs.
// retail keeps the original pool in EBX, the scan pool in EBP, the datum in ESI and the handle in
// EDI; ours loses the retained pool and gives those roles to other registers.
// The pool global read at each use 94 (kept; the local copy was rematerialised from the
// global); then a plain iterator read 107; an element pointer for the flag store 94; the flag
// byte as bitfields 94; the local pool with the iterator set from the global 135; the existing
// inlined datum iterator with the pragma removed 269 (LTCG then inlines 0xbadc0/0xb9d20, which
// noinline declarations here do not stop; under inline_depth(0) even __forceinline is called).
// Earlier forms (106):
// shared absolute scan 153; native inline iterator 174; existing datum iterator 170;
// separate scan admissions 106; guarded bitmap for 106; word division 142;
// readonly bitmap 106; repeated current handle 106.
// Inline update/link/object/query declarations 106 each; paired sound/all boundaries 106/106;
// noinline update/all boundaries 106/106; native flag bit alone/with sound boundaries 106/106.
// pool retention, datum/scan/handle carriers and location call differ.
// Ordinary iterator read 131; current-datum consumer 106; both 131; pool names 106;
// pool/datum names 106; ordinary named carriers 131; ordinary scoped datum 131;
// ordinary named current consumer 131. The dead parameter address statement compiled
// identically and was dropped.
// Round 157: Iterator/location/both native home names 94/94/94; entry location/iterator/both lifetimes
// 94/94/94; admitted datum scope with inherited iterator read 94; complete iterator initialization 94.
// Earlier 94 (was 106). Native two-use object index; completed update/release continues;
// object/exit combinations and repeated cluster index: eight forms 94, all 486/486.
// Coupled occupancy while 117; high-water per use 94; bitmap per use 94;
// coupled/high-water 133; coupled/bitmap 117; both fields per use 94;
// BLOCKED-BY 0xba300: retail's root-location helper returns its location
// pointer, so it takes the object in ECX and the location in EAX; ours takes EAX/ESI, which
// takes ESI from this loop, so the pool is not kept in EBX (retail reloads it only after
// 0x18a7b0) and the iterator pool is not kept in EBP. With a returning stand-in for 0xba300
// this body is 21 (fast), the only difference being retail's flag store (or [ecx+eax*8+4]
// first, then a dead lea of the same address; ours lea first and or [eax]).
// The volatile iterator-pool read removed (107; it was 94 only through the volatile).
// Flag store forms with the stand-in, all 21: plain field, element pointer, byte reference,
// conditional set, byte bitfield, iterator datum store, assigned pointer, reused datum.
// Earlier: the pool global read at each use 94; an element pointer for the flag store 94;
// the flag byte as bitfields 94; the local pool with the iterator set from the global 135;
// the existing inlined datum iterator with the pragma removed 269 (LTCG then inlines
// 0xbadc0/0xb9d20, which noinline declarations here do not stop; under inline_depth(0) even
// __forceinline is called); shared absolute scan 153; native inline iterator 174; existing
// datum iterator 170; separate scan admissions 106; guarded bitmap for 106; word division 142;
// readonly bitmap 106; repeated current handle 106; inline/noinline update, link, object and
// query declarations 106; native flag bit alone/with sound boundaries 106/106; ordinary
// iterator read 131; current-datum consumer 106; pool/datum names 106; ordinary named carriers
// 131; ordinary scoped datum 131; native two-use object index, completed update/release
// continues, object/exit combinations and repeated cluster index 94; coupled occupancy while
// 117; high-water/bitmap/both fields per use 94; coupled/high-water 133; coupled/bitmap 117;
// coupled/both fields 133; iterator ordinal published before datum 94.
// Round 409: Reused byte/full-width classification with expiration 152/155; each with occupied-bit expression 152/155; classification plus occupied expression 152/155; reused expiration alone/with occupied expression 107/107. Pool/scan/datum/handle allocation and root-location interface remain; complete original body restored.
// @retail 0x18c060
void __stdcall function_18c060(real arg_0)
{
	if (((s_18c061 *)g_4e6380)->field_78 &&
		((s_18c061 *)g_4e6380)->field_79 &&
		((s_18c061 *)g_4e6380)->field_7a)
		function_127400(false);

	sound_audible_clusters_update();
	function_18bb80(arg_0);
	function_225b60(arg_0);
	function_18bf90();
	long local_0 = NONE;
	if (g_510c50 && ((s_18c062 *)g_510c50)->field_5)
		local_0 = 0;
	else if (g_4ed288->value24 >= g_510c54->game_time)
		local_0 = 1;
	sound_set_ambience(local_0);

	s_data_datum_iterator local_2;
	local_2.data = g_4ed28c;
	local_2.index = NONE;
	s_18c060 *local_3;
	for (;;)
	{
		long local_9 = local_2.index + 1;
		long local_10 = NONE;
		s_record_pool *local_11 = local_2.data;
		if (local_9 >= 0 && local_9 < local_11->high_water_index)
		{
			long local_12 = local_11->high_water_index;
			dword *local_13 = local_11->bitmap;
			do
			{
				if (local_13[local_9 >> 5] & (1 << (local_9 & 0x1f)))
				{
					local_10 = local_9;
					break;
				}
				local_9++;
			} while (local_9 < local_12);
		}
		if (local_10 == NONE)
			break;
		local_3 = (s_18c060 *)(local_11->data + local_11->size * local_10);
		local_2.datum = (byte *)local_3;
		local_2.index = local_10;
		local_2.datum_index = (local_3->field_0 << 16) | local_10;
		if (local_3->field_3 == 4 && local_3->field_10 != NONE &&
			local_3->field_10 <= g_510c54->game_time)
		{
			byte *local_4 = &((s_18c060 *)g_4ed28c->data)[local_2.datum_index & 0xffff].field_4;
			*local_4 |= 2;
		}
		if (local_3->field_3 != 1)
		{
			function_18a7b0(local_2.datum_index);
		}
		else if ((local_3->field_4 & 1) && !function_badc0(local_3->field_10, (dword)NONE))
		{
			if (function_18d1c0(local_3->field_c) == local_2.datum_index)
			{
				long local_5 = local_3->field_c;
				long local_6 = function_18d1c0(local_5);
				if (local_6 != NONE)
				{
					s_18c060 *local_7 = (s_18c060 *)g_4ed28c->data + (local_6 & 0xffff);
					if (local_7->field_c == local_5)
						local_7->field_4 &= ~0x20;
				}
			}
			record_pool_release(g_4ed28c, local_2.datum_index);
		}
		else if (function_b9d20(local_3->field_10))
		{
			s_location local_8;
			object_get_root_location(local_3->field_10, &local_8);
			if (local_8.cluster_index != NONE &&
				(g_54e8a0[local_8.cluster_index >> 5] & (1 << (local_8.cluster_index & 0x1f))))
			{
				function_18a7b0(local_2.datum_index);
			}
			else
				local_3->field_5 &= ~1;
		}
	}
	((s_18c061 *)g_4e6380)->field_7e = true;
	sound_effects_update();
}
#pragma inline_depth(255)
