// @flags /O2 /arch:SSE /Gr
#include "shader_storage.h"
#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "object_markers.h"
#include "effects.h"
#include "vector_products.h"
#include "normalize3d.h"
#include <math.h>
#include <xtl.h>

struct s_surface_vertex { point3f position, previous; vector3f normal; };
struct s_surface_attachment { short vertex, marker, unknown4; };
struct s_surface_record
{
	short salt, unknown2;
	long tag_index, object_index;
	real unknown_c;
	point3f origin;
	real speed;
	s_surface_vertex vertices[128];
	dword pinned[4];
	s_surface_attachment attachments[6];
	short attachment_count, unknown1256;
};
struct s_surface_rest_vertex { point3f position; real u, v; };
struct s_surface_link { long unknown0; short a, b; real length; long unknownc; };
struct s_surface_definition
{
	word flags, unknown2;
	long marker_name;
	byte unknown8[8];
	short columns, rows;
	byte unknown14[10];
	short iterations;
	real gravity, damping, wind_scale, tangent_drag;
	byte unknown30[0x1c];
	long vertex_count;
	s_surface_rest_vertex *vertices;
	long triangle_index_count;
	short *triangle_indices;
	long strip_index_count;
	short *strip_indices;
	long link_count;
	s_surface_link *links;
};
struct s_surface_object_flags { byte unknown0[0x10a]; word bit0 : 1, bit1 : 1, bit2 : 1; };
struct s_surface_object_header { short salt; byte flags, inactive; long unknown4; byte *object; };

static inline s_surface_record *surface_record(long index)
{
	return (s_surface_record *)(g_4e0338->data + (index & 0xffff) * 0x1258);
}
static inline s_surface_definition *surface_definition(s_surface_record *record)
{
	return (s_surface_definition *)g_4e3b44[record->tag_index & 0xffff].bytes;
}
static inline s_surface_object_header *surface_object_header(long index)
{
	return (s_surface_object_header *)(g_4e0300->data + (index & 0xffff) * 12);
}
static inline bool surface_pinned(s_surface_record *record, long i)
{
	return (record->pinned[i >> 5] & (1 << (i & 31))) != 0;
}

static inline real surface_normalize_horizontal(vector3f *v)
{
	real length = (real)sqrt(v->i*v->i + v->j*v->j);
	if (!(fabs(length) < 0.0001f))
	{
		real inverse = 1.0f / length;
		v->i *= inverse; v->j *= inverse;
	}
	else
		length = 0.0f;
	return length;
}

void function_ba1d0(long, vector3f *, vector3f *);
real function_30bf0(vector3f *);
long function_155760(long);
long function_14de70(long local_player_index);
void object_widget_delete(long, long);
extern dword g_4ba034;
long g_55e710;
bool g_55e714;
real g_55ecd0;
vector3f g_55ecd4;

bool function_1171a0(long);
bool function_1173e0(long, long);
void function_117510(long);
void __stdcall function_117790(long);
void function_117a80(long);
void function_117dd0(long, vector3f *);
void function_118140(long);
void __stdcall function_118430(long);
void function_117080(long);

// score 50 (was 50): retail adds the pool data to the record offset before the store (`add eax, edx`),
// reloads the pool after the `lea` of the next index, and gives the inlined next-index search other
// registers (result edx, count edi, bitmap esi). Pool read directly at each use as in 0x1160c0 54 (reload
// now after the `lea` as retail); with typed record arithmetic 54; with no slot local 54; with record and
// definition locals 54; the for-loop next-index helper 50 (identical code).
// Round 57: no slot with next before the reload 64; no slot alone 50; next with slot 54;
// slot at function scope 50; typed record-array base 50; outer for-loop 50; alternate next
// helper with next 54; record pointer on the pool-local body 50.
// Round 268: explicit search result 50; unsigned slot 50; word slot 51; unsigned
// datum handle 50; explicit search result with those three carriers 50/51/50;
// shared out-of-line datum conversion 102. All original helper and field reads restored.
// Round 270: native iterator called/inline 89, with CRT scalar clear 89; called/called
// 124, with CRT 121; inline/inline 89, with CRT 89; original traversal with CRT
// scalar clear 76, unsigned slot 76. All restored; record-address and search homes remain.
// Coupled pool/datum/absolute-slot identities, seven subsets and entry-slot lifetime
// on the complete role-name graph: 50/50/50/50/50/50/50/50. Original body restored.
// Round 71: pool global read at each use 54 (retail's reload after the `lea`; aligned 16 against 18), with a
// record local/surface_record/no slot/no slot plus surface_record 54/54/54/54; the record address stays
// folded (`[eax + ecx + 0xc]`, retail `add eax, edx`) in every form whose store precedes the first call.
// Round 408: existing datum/absolute inline lookups, with retained/per-use pool observations,
// 106/106/97/100; existing datum/absolute called lookups 71/71/60/60. All validation-bearing forms
// restored; record-base ADD, pool reload and successor-search result/count/bitmap homes remain.
// @retail 0x1168a0
void __stdcall function_1168a0(real elapsed)
{
	s_record_pool *pool = g_4e0338;
	long index = data_datum_index(pool, function_16bc00(pool, 0));
	while (index != NONE)
	{
		long slot = index & 0xffff;
		((s_surface_record *)pool->data + slot)->unknown_c = 0.0f;
		function_117790(index);
		function_118140(index);
		function_117510(index);
		function_117080(index);
		pool = g_4e0338;
		index = data_datum_index(pool, data_find_index(pool, index == NONE ? 0 : slot + 1));
	}
}

// @retail 0x116980
long __stdcall function_116980(long tag_index, long object_index)
{
	long result = NONE;
	if (tag_index != NONE && object_index != NONE)
	{
		result = record_pool_allocate(g_4e0338);
		if (result != NONE)
		{
			s_surface_record *record = surface_record(result);
			record->tag_index = tag_index;
			record->object_index = object_index;
			record->unknown_c = 0.0f;
			if (!function_1173e0(object_index, result))
			{
				record_pool_release(g_4e0338, result);
				result = NONE;
			}
		}
	}
	return result;
}

// @retail 0x117080
void function_117080(long index)
{
	long object_index = surface_record(index)->object_index;
	if (object_index != NONE)
	{
		s_surface_object_header *header = surface_object_header(object_index);
		byte *object = header->object;
		if (!header->inactive && TEST_FIELD_BIT(((s_surface_object_flags *)object)->bit2) &&
			(real)(g_510c54->game_time - *(long *)(object + 0xbc)) * g_510c54->rate > 10.0f)
			object_widget_delete(object_index, index);
	}
}

// @retail 0x117100
bool function_117100(long index)
{
	bool result = true;
	long object_index = surface_record(index)->object_index;
	long root = NONE;
	while (object_index != NONE)
	{
		root = object_index;
		object_index = *(long *)(surface_object_header(object_index)->object + 0x14);
	}
	s_surface_object_header *header = surface_object_header(root);
	if (!header->inactive && g_4b9ed8 != NONE && function_155760(g_4b9ed8) == 0)
	{
		byte *object = header->object;
		if (function_14de70(g_4b9ed8) == *(long *)(object + 0x13c))
			result = false;
	}
	return result;
}

// @retail 0x1171a0
bool function_1171a0(long index)
{
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	s_object_marker markers[6];
	long order[6] = {0, 1, 2, 3};
	long marker_name = definition->marker_name;
	long object_index = record->object_index;
	record->attachment_count = function_b8d30(object_index, marker_name, markers, 6, false);
	long axis = 2;
	if (definition->flags & 2) axis = 0;
	for (long i = 0; i < record->attachment_count; ++i)
	{
		for (long j = i + 1; j < record->attachment_count; ++j)
		{
			if (markers[i].matrix.position.n[axis] > markers[j].matrix.position.n[axis])
			{
				s_object_marker temporary = markers[i]; markers[i] = markers[j]; markers[j] = temporary;
				long swap = order[i]; order[i] = order[j]; order[j] = swap;
			}
		}
	}
	if (record->attachment_count > 0)
	{
		real step = (real)definition->rows / (real)record->attachment_count;
		for (long i = 0; i < record->attachment_count; ++i)
		{
			s_surface_attachment *attachment = &record->attachments[i];
			long vertex;
			if (i == 0) vertex = 0;
			else if (i == record->attachment_count - 1) vertex = (definition->rows - 1) * definition->columns;
			else vertex = (long)(ceil(i * step) * definition->columns);
			attachment->marker = (short)order[i];
			attachment->vertex = (short)vertex;
		}
	}
	return record->attachment_count > 0;
}

// @retail 0x1173e0
bool function_1173e0(long object_index, long index)
{
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	bool result = function_1171a0(index);
	if (result)
	{
		byte *object = surface_object_header(object_index)->object;
		point3f origin = *(point3f *)(object + 0x30);
		for (long i = 0; i < definition->vertex_count; ++i)
		{
			point3f *source = &definition->vertices[i].position;
			s_surface_vertex *vertex = &record->vertices[i];
			vertex->position.x = source->x + origin.x;
			vertex->position.y = source->y + origin.y;
			vertex->position.z = source->z + origin.z;
			record->pinned[i >> 5] &= ~(1 << (i & 31));
		}
		for (long iteration = 0; iteration < 5; ++iteration) function_117790(index);
	}
	return result;
}

// score 4 (was 4): only the a/b normal.i addition schedules differ; frame and length agree.
// Shared point differences 169; differences and cross product 108 (dropped, full); shared
// scale normalization 166, with differences/cross 130 (dropped, full); normalize_inline 123; reversed edge
// production 132 (dropped, full); named edges/readonly definition 130/130; cross only 165; normal-first
// accumulation 108; out-of-line normalization 305.
// Shared difference/cross revisit 108; triangle cursor 164; vertex base 99, both 169.
// Plain component accumulation 109; with triangle cursor 176, vertex base 96, both 173.
// Selected ordinary check 99/621.
// Component-loop addition 96/633 discarded: only three points beyond the vertex-base form.
// Shared add 99/633; with shared normalize 40/637; all normal-first 63; final vertex cursor 4/633;
// only a normal-first 4; only b 7; only c 4; normal declared first 4; reused accumulation 260;
// normal-field pointers 86; normalized contribution 223. Selected ordinary check 4/633.
// Corner-pointer order acb/bac/bca/cab/cba 27/84/80/28/96; vector workspace before
// corners 4; edge-only scope 4; loop-wide normal workspace 4. Original body restored.
// Round 163: all triangle-index snapshots short forward/reverse and long forward/reverse
// 7/7/7/7; corner references 4, with short indices 7; complete edge/normal role names 4;
// complete position snapshots 274; contribution-first corner pairs ab/ac/bc 7/4/7.
// All restored. Remaining addition pairs are normal.i, not normal.k: offsets +0x18.
// Round 90: one `long i` for both loops 39; second-loop cursor declared at the top 4 (same
// code); record->vertices at each use 42; per-triangle index pointer 163; `!normalize3d` 37.
// @retail 0x117510
void function_117510(long index)
{
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	s_surface_vertex *vertices = record->vertices;
	for (long i = 0; i < definition->triangle_index_count; i += 3)
	{
		s_surface_vertex *a = &vertices[definition->triangle_indices[i]];
		s_surface_vertex *b = &vertices[definition->triangle_indices[i+1]];
		s_surface_vertex *c = &vertices[definition->triangle_indices[i+2]];
		vector3f v, u, normal;
		vector3d_from_points3d(&a->position, &b->position, &v);
		vector3d_from_points3d(&a->position, &c->position, &u);
		cross3f(&v, &u, &normal);
		normalize3d(&normal);
		vector3f_add(&a->normal, &normal, &a->normal);
		vector3f_add(&b->normal, &normal, &b->normal);
		vector3f_add(&c->normal, &normal, &c->normal);
	}
	s_surface_vertex *vertex = vertices;
	for (long i = 0; i < definition->vertex_count; ++i, ++vertex)
		if (normalize3d(&vertex->normal) == 0.0f) vertex->normal = *g_4687a8;
}

// score 139 (was 139): frame 0x1c agrees; link-base/endpoint carriers, pinned-word test and correction staging differ.
// Shared difference 154; vertex base 155; distinct correction 161, with shared difference 161;
// both shared scales 154, with difference 154; early rest square 155; all producers/lifetimes 162.
// All executable forms restored.
// frame 0x1c agrees; link/endpoint carriers, split pin test and correction scheduling differ.
// Shared difference/scales 154; vertex base 155; distinct correction 161, with difference 161;
// early rest square 155; all producer/correction forms 162. Endpoint short snapshots forward 176,
// reverse 139; extended long 183 in either order; shared pin helper ties each form.
// Reverse short snapshot ordinary check 139/741.
// Native endpoint vertices with rest square/correction/shared-scale subsets:
// 139/138/151/150/139/138/151/150. The one-point leads are dropped; original body restored.
// Round 163: both pin decisions shared after/before correction, bool 201/201,
// byte 201/201, word 189/189, long 195/195; with uniform live endpoint fields
// bool/byte/word/long 212/212/200/206. All restored; 0x1c frame, entry/link homes
// and correction/pin-word scheduling remain.
// Round 264: live record/definition/both/link/record-link/definition-link/all receivers
// 309/232/379/147/309/254/377; definition acquired before record 149. All restored;
// frame 0x1c, entry pool/record carriers and correction/pin scheduling remain.
// Round 196: retain only first endpoint with point/vertex/shared-difference/link-cursor forms
// 179/179/179/173; retain only second 211/211/211/159. All restored; native entry/link
// carriers, pin-word observations and correction scheduling remain.
// Round 113: link->a/link->b read at each use (no endpoint locals) 154 at 742 bytes, but it gives retail's
// prologue and outer loop (record EDI, iterations EBX; first difference +0x8 -> +0x8a; aligned 116 -> 77);
// left: the link pointer is folded (`[edx+eax+4]`, retail `add eax,[esp+0x18]`), the first pin test reuses
// the top read of link->a in EBX (retail re-reads it) and splits its test, a/link swap EAX/EDX. On it:
// rest square before the clamp 155 (twice); shared point difference 154; long bits local 154;
// short-parameter forceinline pin test 163; fields through definition->links[i] 220. All restored.
// @retail 0x117790
void __stdcall function_117790(long index)
{
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	function_117a80(index);
	long iterations = (long)((real)definition->iterations + record->speed * 3.0f);
	if (iterations < 1) iterations = 1;
	else if (iterations > 15) iterations = 15;
	for (long iteration = 0; iteration < iterations; ++iteration)
	{
		for (long i = 0; i < definition->link_count; ++i)
		{
			s_surface_link *link = &definition->links[i];
			short second_index = link->b;
			short first_index = link->a;
			point3f *a = &record->vertices[first_index].position;
			point3f *b = &record->vertices[second_index].position;
			real rest_length = link->length;
			vector3f delta = {b->x-a->x, b->y-a->y, b->z-a->z};
			real squared = delta.k*delta.k + delta.j*delta.j + delta.i*delta.i;
			real length;
			*(long *)&length = (*(long *)&squared >> 1) + 0x1fc00000;
			if (length < 0.0001f) length = 0.0001f;
			real inverse = 1.0f / length;
			delta.i *= inverse; delta.j *= inverse; delta.k *= inverse;
			real rest_squared = rest_length * rest_length;
			length = (rest_squared / (length*length + rest_squared) - 0.5f) * length;
			delta.i *= length; delta.j *= length; delta.k *= length;
			if (!surface_pinned(record, first_index))
			{
				a->x -= delta.i; a->y -= delta.j; a->z -= delta.k;
				if (surface_pinned(record, second_index)) { a->x -= delta.i; a->y -= delta.j; a->z -= delta.k; }
			}
			if (!surface_pinned(record, second_index))
			{
				b->x += delta.i; b->y += delta.j; b->z += delta.k;
				if (surface_pinned(record, first_index)) { b->x += delta.i; b->y += delta.j; b->z += delta.k; }
			}
			if (fabs(length) > 2.0f) { function_118430(index); --iteration; }
		}
		function_117a80(index);
	}
}

// score 67 (was 67): the pool data base sits in ebp where retail has ebx; the shared difference helper gives
// another origin/delta schedule, and the attachment walk and copy registers still differ. Earlier: the first loop
// entered by a jump where retail falls through. Attachment element pointer 103 (retail's walk and reloads, but
// the copy's registers rotate and the pinned OR splits into load/or/store, 521 bytes); with a vertex local for
// the pin 103; with the existing bit_vector_set 103; fields in full with bit_vector_set 103 (identical code);
// the definition read after the origin delta 80; between the origin copy and the delta 80.
// Round 58: shared length-squared helper 81; shared point-difference helper 72; typed record lookup 72;
// vertex-array base 67 kept; loop-lifetime attachment cursor 90; attachment array base 75; aggregate delta
// with the array base 83; length helper on the best form 83; correction-only difference helper 83;
// direct vertex reads through the attachment base 91; entry velocity declaration 67.
// Round 209: Intrinsic attachment copy 74; with direct attachment fields 85; all three intrinsic copies 67;
// shared unit-scale differences initial/both/both plus origin copy 67/67/67; function-scope markers 67,
// plus direct-field intrinsic copy 85. Restored; pool base, displacement schedule and attachment
// copy/reload remain.
// Complete squared displacement terms 81; point/vector/marker role identities 67; both 81;
// native word attachment ordinal 96; with squares/roles/both 105/96/105; complete graph
// with displacement declared before point acquisition 105. Original executable body restored.
// Round 221: Origin pointer observed at use 148; definition at every use 168; both 171;
// record at every use 270; with origin/definition/both 252/321/303; all plus shared correction
// difference helper 303. Original captures restored; pool-base and copy carriers remain.
// Round 71: the squared distance as i*i + j*j + k*k (or length_sq3f) gives retail's z, y, x schedule
// (aligned 39 -> 25) but scores 83: the data base stays in EBP (retail EBX), so the `add esi` comes one
// load early, the code before the first loop is a byte short and a jump over the loop pad appears. On that
// form: attachment fields read in full 106 (retail's walk from the vertex field and the second vertex
// read, but the pin OR splits into load/or/store, 522 bytes), with bit_vector_set 106; definition read
// after the origin copy 73 (aligned 48).
// Round 276: Shared translation 72; real attachment value 100; both 97; translation with element owner 74;
// Round 276: with attachment value 97; translation with common ordinal 74; with attachment value 100;
// Round 276: all ownership forms 101. Original body restored; pool-base and copy reload remain.
// Round 113: attachment element pointer 85 (retail's +0x1230 walk and vertex re-read; the pin OR splits
// and the copy takes src EDX/dest ECX, retail ECX/EAX); with bit_vector_set 82; with a vertex local
// after the copy 82; element pointer plus i-first squared distance 103 (aligned 40 -> 30, the jump over
// the loop pad returns), with length_sq3f 103. All restored.
// @retail 0x117a80
void function_117a80(long index)
{
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	point3f origin = *(point3f *)(surface_object_header(record->object_index)->object + 0x30);
	vector3f delta;
	vector3d_from_points3d(&origin, &record->origin, &delta);
	if (delta.k*delta.k + delta.j*delta.j + delta.i*delta.i > 0.1600000113248825f)
	{
		delta.i = origin.x-record->origin.x; delta.j = origin.y-record->origin.y; delta.k = origin.z-record->origin.z;
		s_surface_vertex *vertices = record->vertices;
		for (long i = 0; i < definition->vertex_count; ++i)
		{
			vertices[i].position.x += delta.i;
			vertices[i].position.y += delta.j;
			vertices[i].position.z += delta.k;
			vertices[i].previous = vertices[i].position;
		}
	}
	vector3f velocity;
	function_ba1d0(record->object_index, &velocity, NULL);
	record->origin = origin;
	record->speed = (real)sqrt(velocity.i*velocity.i + (velocity.j*velocity.j + velocity.k*velocity.k));
	for (long i = 0; i < record->attachment_count; ++i)
	{
		s_object_marker markers[6];
		if (function_b8d30(record->object_index, definition->marker_name, markers, 6, false) > 0)
		{
			long vertex = record->attachments[i].vertex;
			record->vertices[vertex].position = markers[record->attachments[i].marker].matrix.position;
			record->pinned[vertex >> 5] |= 1 << (vertex & 31);
		}
	}
}

// @retail 0x117c80
void function_117c80(vector3f const *input, vector3f *output, real angle)
{
	real limit = (real)cos(angle);
	real magnitude = (real)fabs(input->k);
	*output = *input;
	if (magnitude > limit) output->k = input->k < 0.0f ? -limit : limit;
	if (fabs(output->i) < 0.01f && fabs(output->j) < 0.01f || fabs(surface_normalize_horizontal(output)) < 0.0001f)
	{
		output->i = 1.0f; output->j = 0.0f;
	}
	surface_normalize_horizontal(output);
	real horizontal = (real)sqrt(1.0f - output->k*output->k);
	output->i *= horizontal;
	output->j *= horizontal;
}

static inline dword surface_random()
{
	g_4e7408->seed = 1664525 * g_4e7408->seed + 1013904223;
	return g_4e7408->seed >> 16;
}

// score 160 (was 160): frame 0x28 against 0x20 and random/time/oscillation staging remain; shared cross product reaches retail length.
// Earlier frame 0x24 against 0x20 and the random-seed/oscillation schedule. Reading the frame
// count again after the 0x117c80 call (as retail does) gives retail's convention: wind in EBX, definition in EDI.
// Complete output staging 201; old before axis 199, before trigonometry 199; vector names 195;
// three named cross terms 195; named variation 195; scalar declarations first 195;
// projection before shared cross 182. Shared cross product 168 fast/plain, 880/880 bytes.
// Round 219: immutable axis pointer 210; shared final double scale 168; both 210; native rotation
// setter 168; axis/setter 210; scale/setter 168; all 210; readonly old with axis pointer 210.
// Restored.
// Round 381: RNG before time 199; oscillation before RNG 203; axis before/between trigs 168/168;
// RNG-first/axis-between 199; oscillation-first/axis-first 203; rotation-scoped angle 201,
// with shared final scale 201. Late angle reaches frame 0x20 but loses wind register; all restored.
// Round 238: live direction inputs 209; complete output 199, with live inputs 188;
// all three cosine products 188, with live inputs 183; sine/cosine role names 168/168;
// separate trigonometric declarations with live inputs 209. Original snapshots and output order restored.
// Round 32: shared initial RNG 160; shared table-index draws 160; both 160;
// complete axis/direction CRT captures 168, with initial/table/both 160/160/160;
// all with alternate shared RNG contract 160. Initial shared RNG retained, plain 160; frame 0x28 remains.
// Round 264: all six lexical orders of the three rotation objects on the shared
// cross/RNG graph 160 each; vector declarations at entry/after angle 160/160.
// Original names and scopes restored; frame 0x28 and time/rotation staging remain.
// Round 401: uniform seed receiver, time receiver and both 160/160/160; projection before cross on
// shared-random graph 174, with seed/time/both 174/174/174; both receivers and commuted time
// factors 174. Original complete body restored; frame and rotation scheduling remain.
// Round 412: Staged cross 160; native old-direction setter 187; native axis setter 199; staged/old
// 187; staged/axis 199; both setters 208; all producers 208; all with alternate random contract
// 208. All restored.
// @retail 0x117dd0
void function_117dd0(long index, vector3f *wind)
{
	s_surface_definition *definition = surface_definition(surface_record(index));
	real time = (real)(g_510c54->game_time * (double)g_510c54->field_2_3);
	dword random = random_next(&g_4e7408->seed);
	double oscillation = cos(time * 0.005f);
	real angle = (real)(random * (double)(1.0f/65535.0f) * oscillation * oscillation * 0.15f);
	if (!g_55e714)
	{
		g_55ecd4 = g_4417f0[(short)((surface_random() * 1026) >> 16)];
		if (!(fabs(function_30bf0(&g_55ecd4)) < 0.0001f)) g_55e714 = true;
	}
	if ((long)g_4ba034 > g_55e710)
	{
		real sine = (real)sin(angle), cosine = (real)cos(angle);
		vector3f axis = g_4417f0[(short)((surface_random() * 1026) >> 16)];
		vector3f old = g_55ecd4;
		vector3f cross;
		cross3f(&old, &axis, &cross);
		real projection = (axis.k*old.k + axis.j*old.j + axis.i*old.i) * (1.0f-cosine);
		g_55ecd4.i = old.i*cosine + axis.i*projection - cross.i*sine;
		g_55ecd4.j = old.j*cosine + axis.j*projection - cross.j*sine;
		g_55ecd4.k = old.k*cosine + axis.k*projection - cross.k*sine;
		function_117c80(&g_55ecd4, &g_55ecd4, 0.34906584f);
		/* retail reloads the frame count after the call, as for an address-taken global (0x4a7c0, 0x248620) */
		g_55e710 = *(dword const *)&g_4ba034;
	}
	g_55ecd0 += (real)((cos(time * 0.001) * cos(time * 0.0006f) + sin(time * 0.005)) * cos(time * 0.0001f) * 0.01f * 0.3f);
	if (g_55ecd0 < -0.1f) g_55ecd0 = -0.1f;
	else if (g_55ecd0 > 0.1f) g_55ecd0 = 0.1f;
	double strength = (fabs(g_55ecd0) + 0.0075f) * definition->wind_scale * 0.08000000566244125f;
	wind->i = (real)(g_55ecd4.i * strength); wind->j = (real)(g_55ecd4.j * strength); wind->k = (real)(g_55ecd4.k * strength);
}
// score 109 (was 109): the loop's vertex cursor points at the normal's j (retail: k), so the dot product,
// frame 0x48 agrees; normal.j versus normal.k cursor and weight/vector scheduling differ.
// tangent and position sums load and schedule in another order; retail adds the acceleration and the force
// as first operands. Weights stored after the acceleration (retail stores them first, with the current
// weight above the previous). Tried: dot sums i,j,k / k,i,j / j,k,i 109 each; dot3f either argument order
// 117 (744 bytes); weights before the acceleration 109; previous weight declared first 113, before the
// acceleration 113; current weight renamed 109; acceleration and force as first addends 116 (744 bytes).
// Shared tangent scale 109; previous point first 122; outer vertex declaration 109; native scaling loop
// 120; scale helper/previous first 122; native additions 116; scaling/additions 154, with helper 151.
// All restored.
// Complete late normal snapshot 189; with symmetric displacement 226, direct drag field 194, both 186;
// early normal snapshot 192, with displacement 225, direct drag 194, both 186. Snapshots grow the frame.
// Round 221: Shared point-setter boundaries for both weights/both sums/both 108/108/108;
// complete tangent/force assignment boundaries alone/with either/both point phases 195 each;
// all with intrinsic fast-history copy 194. One-point gains rejected; all original source restored.
// Round 216: native projected-normal scale/vertex cursor/both 103/205/187; overlap-safe
// point-history transfers alone/projection/cursor/all 212/213/207/199; in-place projection 153.
// On projected normal, early weights/acceleration first/force first/both/all 102/113/103/113/110;
// weight point setters 104, with all 112. Normal.k cursor recovered at 103/102; sub-eight gains restored.
// Round 71: the else loop through a vertex pointer (`vertex->previous = vertex->position`) gives retail's
// else loop and its pad exactly and retail's 744 bytes (aligned 85 -> 66) but scores 116 (the main loop's
// shifts); with acceleration and force as first addends 113 (aligned 59); with the tangent through
// vector3f_add_scaled(..., -dot, ...) 196 (normal.k cursor, 712 bytes), plus dot3f 195; with the projection
// through scale_vector_by_double 150 (normal.k cursor, frame changes). Retail loads acceleration.i/j into
// a register before adding (ours folds the memory operand).
// Round 263: damping shared by both weights 154; force copy at its consumer 194;
// both 184; pin-test continue 109, with damping/late force/both 154/194/184;
// shared damping with native projected-normal scale 159. All original source restored.
// @retail 0x118140
void function_118140(long index)
{
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	vector3f wind;
	function_117dd0(index, &wind);
	if (record->speed < 1.5f)
	{
		vector3f acceleration = *g_4687a4;
		real gravity = definition->gravity * -3.2086613178253174f;
		acceleration.k += gravity / (real)g_510c54->field_2_3 * 0.03125f;
		real current_weight = 2.0f - definition->damping;
		real previous_weight = 1.0f - definition->damping;
		for (long i = 0; i < definition->vertex_count; ++i)
		{
			vector3f force = wind;
			if (!surface_pinned(record, i))
			{
				s_surface_vertex *vertex = &record->vertices[i];
				point3f position = vertex->position;
				point3f previous = vertex->previous;
				position.x *= current_weight; position.y *= current_weight; position.z *= current_weight;
				previous.x *= previous_weight; previous.y *= previous_weight; previous.z *= previous_weight;
				position.x = position.x - previous.x + acceleration.i;
				position.y = position.y - previous.y + acceleration.j;
				position.z = position.z - previous.z + acceleration.k;
				real dot = vertex->normal.i*force.i + vertex->normal.k*force.k + vertex->normal.j*force.j;
				vector3f tangent = { force.i - vertex->normal.i*dot, force.j - vertex->normal.j*dot, force.k - vertex->normal.k*dot };
				real drag = definition->tangent_drag;
				tangent.i *= drag; tangent.j *= drag; tangent.k *= drag;
				force.i -= tangent.i; force.j -= tangent.j; force.k -= tangent.k;
				vertex->previous = vertex->position;
				position.x += force.i; position.y += force.j; position.z += force.k;
				vertex->position = position;
			}
		}
	}
	else for (long i = 0; i < definition->vertex_count; ++i) record->vertices[i].previous = record->vertices[i].position;
}

// @retail 0x118430
void __stdcall function_118430(long index)
{
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	for (long i = 0; i < definition->vertex_count; ++i)
	{
		s_surface_vertex *vertex = &record->vertices[i];
		point3f *source = &definition->vertices[i].position;
		vertex->position = *source;
		vertex->previous = *source;
		vertex->normal = *g_4687ac;
	}
	record->origin = *g_468788;
	function_117a80(index);
}

#include "flexible_surface_calls.h"
extern s_record_pool *g_509434;
long __stdcall function_3ddd0(long);
void function_1cf50();
void function_1ccb0(long);
void function_1cd90();
byte *g_485a80;
// Render-state storage read by the two external state helpers.
// Attribute pairs terminated by 255, as consumed by the retail state helper.

class c_surface_reference_view
{
public:
 virtual void slot0() = 0; virtual void slot1() = 0;
 virtual void slot2() = 0; virtual void slot3() = 0;
 virtual void slot4() = 0; virtual void slot5() = 0;
 virtual void slot6() = 0; virtual void slot7() = 0;
 virtual void slot8() = 0; virtual void slot9() = 0;
 virtual byte *reference() = 0;
};

static inline byte *surface_tag(long index) { return g_4e3b44[index & 0xffff].bytes; }
static inline byte *surface_pointer(byte *base, long offset) { return *(byte **)(base + offset); }

// score 473 (was 473): frame 0x44 versus 0x2c; attribute scans and D3D call expansion differ.
// Integer orientation 529; null-first lighting 522; per-scan results 530; real type
// cursors 574; existing format/state helpers 473 (kept, full); cursors/orientation
// 573; helpers/orientation 471 (two-point addition dropped); all three 516.
// Round 209: Fresh type-field reads/cursors 498/497; with per-scan results 482/498; existing oriented
// tangent setter 473, tangent and normal setters 472 (one-point lead dropped); both setters with fresh
// cursors/results 497/482. All restored; frame 0x44/0x2c, descriptor scans and outer D3D expansion remain.
// Round 280: paired first-match scans normal/tangent and binormal/uv 514;
// normal/binormal and tangent/uv 514; normal/uv and tangent/binormal 504;
// triple normal/tangent/binormal 481, normal/tangent/uv 495, normal/binormal/uv 495;
// all four 491; triple tangent/binormal/uv 487. All original scans restored.
// Round 238: application inline/force-inline 485/485; format inline/force-inline 526/526;
// both helpers inline or force-inline, all four combinations 528. Application-inline forms also
// lose the 0x22a664 reduced control; all original helper declarations and calls restored.
// Native attribute descriptor with uniform type/count observations and complete
// result-role identities: 473/498/473/498/473/498/473/498. All original fields and scans restored.
// Round 163: shared independent-search ordinal, original/per-search defaults
// 473/475; with uniform descriptor-base observations 498/482; native exit-owned
// results 510/510, with descriptor observations 533/533. All original searches
// and APIs restored; 0x44/0x2c frame and outer vertex API expansion remain.
// Round 264: existing 4f/2f API ordinary-inline-noinline declaration pairs, all
// eight non-original combinations 473 each. Original declarations and body restored;
// frame 0x44/0x2c and selective outer vertex API expansion remain.
// Round 401: paired lookup parameter orders: five leading tag/group/variant permutations 473 each;
// trailing context/pass swap 517; context/pass then tag/group/variant 471, pass/context then
// tag/variant/group 471. Two-point gains rejected; every definition, declaration, caller and
// original body restored.
// @retail 0x116b00
void __stdcall function_116b00(long tag_index, long unused, long group, long variant, long pass, long index, void *context)
{
	if (!function_117100(index)) return;
	s_surface_record *record = surface_record(index);
	s_surface_definition *definition = surface_definition(record);
	long lighting_index = function_3ddd0(record->object_index);
	void *lighting = lighting_index == NONE ? NULL : g_509434->data + (lighting_index & 0xffff)*0x100 + 0x44;
	function_1bd50(lighting);
	function_1cdd0(0, 0);
	function_4b2d0(tag_index, group, variant, ((byte *)context)[4], pass);
	long material_index = tag_index == NONE ? *(long *)(g_485a80 + 0xd0) : tag_index;
	byte *reference;
	dword kind = *(dword *)&g_4e3b44[(short)material_index];
	if (kind == 0x5052544d || kind == 0x70727433)
	{
		c_type_4e7709 *provider = function_137bd0(material_index);
		// This slot returns a tag-reference pointer in retail. The existing
		// interface leaves this slot untyped; keep its public declaration.
		reference = ((c_surface_reference_view *)provider)->reference();
	}
	else reference = surface_pointer(surface_tag(material_index), 0x24);
	byte *groups = surface_pointer(surface_tag(*(long *)reference), 0x5c);
	long group_offset = *(word *)(surface_pointer(groups, 4) + group*10) & 0x1ff;
	long variant_offset = *(word *)(surface_pointer(groups, 0xc) + (group_offset+variant)*2) & 0x1ff;
	long shader_index = *(long *)(surface_pointer(groups, 0x14) + (variant_offset+pass)*10 + 4);
	byte *shader = surface_pointer(surface_pointer(surface_tag(shader_index), 0x20), 4);
	byte *attributes = surface_pointer(surface_tag(*(long *)(shader + 0x100)), 8);
	long count = *(long *)(attributes + 4);
	short *types = *(short **)(attributes + 8);
	long normal = NONE, tangent = NONE, binormal = NONE, uv = NONE;
	for (long i = 0; i < count; ++i) if (types[i] == 4) { normal = i; break; }
	for (long i = 0; i < count; ++i) if (types[i] == 5) { tangent = i; break; }
	for (long i = 0; i < count; ++i) if (types[i] == 6) { binormal = i; break; }
	for (long i = 0; i < count; ++i) if (types[i] == 3) { uv = i; break; }
	DWORD old_cull;
	D3DDevice_GetRenderState(D3DRS_CULLMODE, &old_cull);
	function_1ccb0(59);
	function_1cd90();
	function_1cf50();
	for (long side = 0; side < 2; ++side)
	{
		real sign = side == 0 ? 1.0f : -1.0f;
		D3DDevice_SetRenderState(D3DRS_CULLMODE, side == 0 ? D3DCULL_CCW : D3DCULL_CW);
		if (tangent != NONE) D3DDevice_SetVertexData4f(tangent, g_4687ac->i*sign, g_4687ac->j*sign, g_4687ac->k*sign, 1.0f);
		if (binormal != NONE) D3DDevice_SetVertexData4f(binormal, g_4687a8->i, g_4687a8->j, g_4687a8->k, 1.0f);
		D3DDevice_Begin(D3DPT_TRIANGLESTRIP);
		for (long i = 0; i < definition->strip_index_count; ++i)
		{
			long vertex_index = definition->strip_indices[i];
			s_surface_vertex *vertex = &record->vertices[vertex_index];
			s_surface_rest_vertex *rest = &definition->vertices[vertex_index];
			if (normal != NONE) D3DDevice_SetVertexData4f(normal, vertex->normal.i*sign, vertex->normal.j*sign, vertex->normal.k*sign, 1.0f);
			if (uv != NONE) D3DDevice_SetVertexData2f(uv, rest->u, rest->v);
			D3DDevice_SetVertexData4f(0, vertex->position.x, vertex->position.y, vertex->position.z, 1.0f);
		}
		D3DDevice_End();
	}
	D3DDevice_SetRenderState(D3DRS_CULLMODE, old_cull);
}

// @retail 0x117060
void __stdcall function_117060(void *submission)
{
	function_40f60(submission, function_d4bc0, function_116b00);
}

s_shader_storage g_shader_storage;
