// @flags /O2 /Gr /arch:SSE
#include "unknown_2551c0.h"
#include "globals.h"
#include <math.h>

struct s_293130
{
	short field_0;
	short field_2;
	byte field_4[0xc];
	short field_10;
	byte field_12[0x16];
};

struct s_293131
{
	point3f field_0;
	real field_c;
	byte field_10[4];
	real field_14;
	real field_18;
};

struct s_293132
{
	byte field_0[0xc];
	long field_c;
	s_293131 *field_10;
	long field_14;
	byte field_18[8];
	real field_20;
	real field_24;
	byte field_28[4];
	long field_2c;
	byte field_30[0x54];
};

struct s_293133
{
	byte field_0[0x350];
	long field_350;
	s_293132 *field_354;
};

struct s_293134
{
	byte field_0[0x1c];
	point3f field_1c;
	vector3f field_28;
	byte field_34[0x24];
	real field_58;
	byte field_5c[0x58];
	word field_b4;
	word field_b6;
	byte field_b8[0xc];
};

struct s_293135
{
	long field_0;
	short field_4;
	byte field_6[6];
	bool field_c;
	bool field_d;
	bool field_e;
	byte field_f;
	short field_10;
	short field_12;
	long field_14;
	short field_18;
	short field_1a;
	byte field_1c[0xc];
	point3f field_28;
};

struct s_effect_owner;
extern s_record_pool *g_51ecb4;
void function_b7930(void *arg_0, long arg_1, long arg_2, s_effect_owner const *arg_3);
long __stdcall function_b7b40(void *arg_0);
void __stdcall function_b8540(long arg_0);
bool function_2936b0(long arg_0, long arg_1);

__forceinline dword function_293131()
{
	g_4e7408->unknown0 = g_4e7408->unknown0 * 0x19660d + 0x3c6ef35f;
	return g_4e7408->unknown0 >> 16;
}

__forceinline real function_293132(real arg_0, real arg_1)
{
	return arg_0 + (arg_1 - arg_0) * ((real)function_293131() * (1.0f / 65535.0f));
}

__forceinline void function_293133(real arg_0, vector3f const *arg_1, vector3f *arg_2)
{
	real local_0 = (real)sin(arg_0);
	real local_1 = (real)cos(arg_0);
	real local_2 = dot3f(arg_1, arg_2) * (1.0f - local_1);
	real local_3 = arg_1->k * arg_2->j - arg_2->k * arg_1->j;
	real local_4 = arg_2->k * arg_1->i - arg_1->k * arg_2->i;
	real local_5 = arg_2->i * arg_1->j - arg_1->i * arg_2->j;
	arg_2->i = arg_2->i * local_1 + arg_1->i * local_2 - local_3 * local_0;
	arg_2->j = arg_2->j * local_1 + arg_1->j * local_2 - local_4 * local_0;
	arg_2->k = arg_2->k * local_1 + arg_1->k * local_2 - local_5 * local_0;
}

// score 52 (was 52): y/z range-add order, rotation registers and final sentinel
// scheduling differ. Result, conversion and sine stack homes now agree.
// retail packs the random-to-float temp at frame +8 and the
// result at +0xc (the sine shares +0 with the first sum); ours swaps those two
// slots. Ours also adds the field before the range start in the three point
// draws, colours the rotation's xmm registers differently and stores +0x18
// ahead of the +0xc..+0x10 stores.
// Tried: random_index for both index draws 68 (identical code); function_x82e52f
// for the weighted fraction 84; both 143; function_259d0 for the four range
// draws 140, with random_index 137; all three helpers 78 (adds as retail, the
// rotation then ranks j where retail ranks i); on that: dot3f(vector, axis) 78,
// the rotated vector in a local copy 193, the angle passed straight 87, the
// angle read after the copy 87, cosine before sine 84.
// four causes. Frame: retail packs the random-to-float temp at +8 and the
// result, conversion and sine stack homes differ; rotation
// registers also differ.
// Transient creation handle with range result, trig input and shared cross boundaries:
// 361/358/360/357/360/357/359/364. Result, conversion and sine homes plus rotation registers
// remain.
// The shared random helpers (function_259d0 for the four draws, function_x82e52f for the
// fraction, random_index for both index draws) 68 at 1409 bytes: the three draw sums then
// add as retail, but the rotation's dot turns j,i,k (retail i,k,j). On that form: angle
// passed straight 79; fraction without its local 125 (dot as retail, fraction product
// reassociated); (1 - cos) first 68; count inline 68; dot operands swapped 68; named
// 1 - cos 68; named dot 126; the z flag inlined 325; the handler pointer through an if 111;
// the slot pointer as a sum 68; descriptive helper names 68 (also on the old form, 68).
// four causes. Frame: retail packs the random-to-float temp at +8 and the
// four causes. Frame: retail packs the random-to-float temp at +8 and the
// result at +0xc (the sine temp shares +0 with the first sum), ours the result at +8 (sharing it
// with the sine temp) and the random temp at +0xc. Ours reassociates `range draw + base` (adds
// the base first); the rotation's xmm colouring differs; the scheduler hoists the 0x18 NONE store.
// Round 75: the existing random-range function (0x259d0) for the four draws 140 (fixes the three
// fadd orders, reorders the rotation); result renamed object_index/result/zz_index/aa_index 68 each
// (slot unchanged); result declared first 69; existing random real for the fraction 145; angle
// field passed straight to the rotation 80.
// Round 253: Separate total/threshold 52; with word draw/long choice/both 470/248/463;
// cumulative-scope counterparts 361/470/301/463. On the recovered graph, shared range/all random
// helpers 113/53. Keep ordinary-confirmed total/threshold split only: 68 to 52 at retail length,
// scalar homes agree; remaining y/z adds, rotation ranks and sentinel scheduling.
// result, conversion and sine stack homes differ; rotation
// registers also differ.
// Transient creation handle with range result, trig input and shared cross boundaries:
// 361/358/360/357/360/357/359/364. Result, conversion and sine homes plus rotation registers
// remain.
// Round 289: Coherent selected radius, radius/negative, two reused bounds, complete selected row; each with shared double trig input:
// [247, 243, 242, 294, 250, 229, 229, 285]. All executable source and temporary types restored.
// @retail 0x293130
long __stdcall function_293130(long arg_0)
{
	s_293134 local_13;
	s_293130 *local_2 = &((s_293130 *)g_51ecb4->data)[arg_0 & 0xffff];
	s_293132 *local_3 = &((s_293133 *)g_4e0350)->field_354[local_2->field_2];
	long local_0 = NONE;
	if (local_3->field_2c != NONE && local_3->field_c > 0 && local_2->field_10 > 0)
	{
		real local_5 = 0.0f;
		real total_weight = 0.0f;
		short local_6 = 0;
		short local_7;
		short local_16;
		short local_17;
		short local_8 = NONE;
		for (local_7 = 0; local_7 < local_3->field_c; local_7++)
		{
			if (local_2->field_10 & (1 << local_7))
			{
				total_weight += local_3->field_10[local_7].field_18;
				local_6++;
			}
		}
		if (total_weight > 0.0f)
		{
			real random_fraction = (real)function_293131() * (1.0f / 65535.0f);
			real selection_weight = total_weight * random_fraction;
			for (local_16 = 0; local_16 < local_3->field_c; local_16++)
			{
				if (local_2->field_10 & (1 << local_16))
				{
					local_5 += local_3->field_10[local_16].field_18;
					if (local_5 >= selection_weight)
					{
						local_8 = local_16;
						break;
					}
				}
			}
		}
		else if (local_6 > 0)
		{
			short local_9 = (short)((function_293131() * local_6) >> 16);
			short local_10 = 0;
			for (local_17 = 0; local_17 < local_3->field_c; local_17++)
			{
				if (local_2->field_10 & (1 << local_17))
				{
					if (local_9 == local_10)
					{
						local_8 = local_17;
						break;
					}
					local_10++;
				}
			}
		}
		if (local_8 != NONE)
		{
			s_293131 *local_11 = &local_3->field_10[local_8];
			bool local_12 = !((*(dword *)(g_4e3b44[local_3->field_2c & 0xffff].bytes + 0xd4) >> 4) & 1);
			function_b7930(&local_13, local_3->field_2c, NONE, NULL);
			local_13.field_b4 = 1;
			local_13.field_b6 = 0x34;
			if (local_3->field_20 > 0.0f && local_3->field_24 > 0.0f)
				local_13.field_58 = function_293132(local_3->field_20, local_3->field_24);
			local_13.field_1c.x = function_293132(-local_11->field_14, local_11->field_14) + local_11->field_0.x;
			local_13.field_1c.y = function_293132(-local_11->field_14, local_11->field_14) + local_11->field_0.y;
			if (local_12)
				local_13.field_1c.z = local_11->field_0.z;
			else
				local_13.field_1c.z = function_293132(-local_11->field_14, local_11->field_14) + local_11->field_0.z;
			real local_18 = local_11->field_c;
			local_13.field_28 = *g_4687a8;
			function_293133(local_18, g_4687b0, &local_13.field_28);
			local_0 = function_b7b40(&local_13);
			if (local_0 != NONE)
			{
				s_handler_object_view *local_14 = handler_object_get(local_0);
				s_293135 *local_15 = local_14->flags134 == 1 ? (s_293135 *)((byte *)local_14 + local_14->ai_offset) : NULL;
				if (local_15)
				{
					local_15->field_0 = NONE;
					local_15->field_4 = NONE;
					local_15->field_12 = NONE;
					local_15->field_14 = NONE;
					local_15->field_c = false;
					local_15->field_d = false;
					local_15->field_e = true;
					local_15->field_10 = 0;
					local_15->field_18 = NONE;
					local_15->field_1a = 0;
					local_15->field_28 = *g_468788;
					function_2936b0(arg_0, local_0);
					if (local_3->field_14 > 0)
					{
						short count = (short)local_3->field_14;
						local_15->field_4 = (short)((function_293131() * count) >> 16);
					}
				}
				else
				{
					function_b8540(local_0);
					local_0 = NONE;
				}
			}
		}
	}
	return local_0;
}
