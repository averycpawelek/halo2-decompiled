// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_19EC40.CPP: finds entries of the 4e0350 globals' second table by
   position and by three optional 16-bit keys */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_19ec40.h"
#include <math.h>

real distance_sq3f(point3f const *a, point3f const *b);

// score 65 (was 65): retail keeps the globals in EBX, point in EBP, i in EDI, entry in ESI (pushed
// after the count test) and reloads each key into CX; ours puts entry in ECX, so each key gets BP (the
// first one hoisted to the preheader and the back edge) and the globals, point and i rotate to EDI,
// EBX and ESI. The fast check needs distance_sq3f (0x24550) in the image (105 without it). Tried:
// unsigned key bits (a word key compared with each field cast to word) plus entry position 61, not
// kept: the casts only steer the compares; entry position alone 112; separate keys 61; read-only
// globals 61; function-scope position 61; wrapped index 105; direct count 61; explicit append 61;
// height input 61; distance inline 65; force-inline 65; count global at use 65; count and inline 65;
// entry position copy 64; entry copy and force-inline 64; capacity lifetime 65; vertical difference
// 65; the keys read as *(short const volatile *)&key_a etc. 99 (the prologue then matches and
// point_reference is no longer needed, but each key loads as a dword into esi, entry stays in ecx and
// the loop top loses its aligning jmp), key_a alone 97, with distance = 0 / if (point) 102, through an
// inline key-match helper 102; distance = 0 / if (point) alone 102 (retail's movaps-before-je block
// order: aligned 46 against 53, same registers); one && chain without the globals and point_reference
// locals 117; point_reference dropped 71 (point becomes a register argument); globals local dropped
// 113; inline key-match helper 65; entry at function scope 65; field-first key compares 65; (void)& of
// the key parameters (no effect, 102 with the if (point) shape); keys compared without the key
// temporary 65; entry declared first 65; point test with the arms swapped worse; position copied from
// globals or from entry 64 (from entry drops retail's table reload); one && chain of the key tests 65;
// `continue` per key test 65 (all three key forms give identical code); every key test through
// globals->marker_entries[i] without entry 138 (378 bytes); i declared at the top 65; count declared
// first 65; globals declared before point_reference 65; the entry copy with distance = 0 / if (point)
// 102.
// key preheader and global/point/index/entry registers still differ.
// Round 73: formal word keys 65 (two matched callers differ); entry-pointer traversal 106; output pointer 65;
// memcpy append 97; full-width result index 65; mixed parameter/temporary key tests 65/65;
// reused entry-field temporary 65. Opening signature and body restored.
// Unsigned key bits (a word key compared with each field cast to word) plus entry position 61,
// not kept: the casts only steer the compares; entry position alone 112; separate keys 61; read-only globals 61;
// function-scope position 61; wrapped index 105; direct count 61;
// explicit append 61; height input 61. Existing point handling retained.
// preheader key and entry registers remain; distance inline 65; force-inline 65; count global at use 65; count and inline 65; entry position copy 64; entry copy and force-inline 64; capacity lifetime 65; vertical difference 65.
// The fast check needs distance_sq3f (0x24550) in the image (105 without it). Retail reloads each key as a
// word into cx at the loop top, keeps entry in esi, point in ebp, the globals in ebx and i in edi; ours
// hoists key_a into bp (preheader and back edge) and keeps entry in ecx. Tried: the keys read as
// *(short const volatile *)&key_a etc. 99 (the prologue then matches and point_reference is no longer
// needed, but each key loads as a dword into esi, entry stays in ecx and the loop top loses its aligning
// jmp), key_a alone 97, with distance = 0 / if (point) 102, through an inline key-match helper 102;
// distance = 0 / if (point) alone 102 (retail's movaps-before-je shape); one && chain without the
// globals and point_reference locals 117; point_reference dropped 71 (point becomes a register
// argument); globals local dropped 113; inline key-match helper 65; entry at function scope 65;
// field-first key compares 65; (void)& of the key parameters (no effect, 102 with the if (point) shape).
// score 65 (was 65): keys compared without the key temporary 65; entry declared first 65;
// point test with the arms swapped worse; position copied from globals or from entry 64.
// @retail 0x19ec40
long function_19ec40(
	point3f const *point,
	real height,
	short key_a,
	short key_b,
	short key_c,
	long maximum_count,
	long *results,
	real radius)
{
	point3f const *const *point_reference = &point;
	s_palette_source_globals *globals = g_4e0350;
	real radius_squared = radius * radius;
	real best = 0.0f;
	long count = 0;

	for (short i = 0; i < globals->marker_count; i++)
	{
		short key = key_a;
		s_marker_entry *entry = &globals->marker_entries[i];

		if (key == -1 || key == entry->key_a)
		{
			key = key_b;
			if (key == -1 || key == entry->key_b)
			{
				key = key_c;
				if (key == -1 || key == entry->key_c)
				{
					real distance_squared;

					if (!*point_reference)
						distance_squared = 0.0f;
					else
					{
						point3f position = g_4e0350->marker_entries[i].position;
						distance_squared = distance_sq3f(&position, point);
						if (radius >= 0.0f && distance_squared > radius_squared)
							continue;
						if (height > 0.0f && fabs(entry->position.z - point->z) > height)
							continue;
					}

					if (count < maximum_count)
						results[count++] = i;
					else if (count == 1 && best > distance_squared)
					{
						results[0] = i;
						best = distance_squared;
					}
				}
			}
		}
	}

	return count;
}
