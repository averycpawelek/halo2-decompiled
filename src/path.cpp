// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "path.h"
#include "globals.h"
#include "unknown_20fe20.h"
#include "unknown_0259a0.h"
#include <float.h>
#include <string.h>

real function_30bf0(vector3f *v);

bool function_26c4e0(s_pathfinding_data *pathfinding, s_type_c3b527 const *start, long start_node_index,
	s_type_c3b527 const *end, long end_node_index, long flags, s_path_trace_result *result);

/* Local views of fields not yet named in path.h. */
struct s_path_input_view
{
	real radius;
	bool unknown04;
	byte unknown05[0xb];
	bool start_valid;
	s_type_c3b527 start;
	long start_node_index;
	bool attractor_valid;
	point3f attractor_point;
	long attractor_object_index;
	real attractor_radius;
	real field_40;
	bool unknown44;
	bool distance_limit_valid;
	byte unknown46[2];
	real distance_limit;
	real link_penalty;
};

struct s_path_destination_view
{
	byte unknown00[0x54];
	bool destination_valid;
	s_type_c3b527 destination;
	long destination_node_index;
	real destination_radius;
};

/* The existing s_type_136112 view begins at the heap-index field. This view
   begins at the actual node's start and exposes the hash key at +8. */
struct s_path_node_key_view
{
	short child;
	short parent;
	long unknown04;
	long node_index;
	bool flag0c;
	bool flag0d;
	bool flag0e;
	byte unknown0f;
	short unknown10;
	short unknown12;
	long unknown14;
	s_type_c3b527 entry_point;
	real entry_distance;
	real attractor_distance;
	real path_distance;
	real cost;
	real estimated_distance;
	short quantized_cost;
	short depth;
	short heap_index;
	short unknown42;
};

struct s_path_lookup_view
{
	byte unknown00[0xb0];
	s_path_node_key_view nodes[1024];
	short heap_count;
	path_heap_entry heap[1025];
	short hash_table[4096];
};

struct s_path_location_entry_view
{
	short unknown00;
	short unknown02;
	long node_index;
};

struct s_path_location_view
{
	short flags;
	short count;
	s_path_location_entry_view entries[3];
};

struct s_path_link_view
{
	long node_index;
	word flags;
	short unknown06;
	long type;
	long index;
	s_type_c3b527 point;
	vector3f vector;
	bool unknown2c;
	bool unknown2d;
	bool unknown2e;
};

struct s_path_closest_view
{
	byte unknown00[0x90];
	short node_index;
	short unknown92;
	real distance;
	real estimated_distance;
	s_type_c3b527 point;
};

struct s_path_edge_view
{
	word vertices[2];
	byte flags;
	byte unknown05[3];
	word next_edges[2];
	word nodes[2];
};

struct s_path_surface_link_view
{
	short type;
	short next;
	word unknown04;
	word unknown06;
	word unknown08;
	word unknown0a;
	byte unknown0c;
	signed char unknown0d;
	word unknown0e;
	short unknown10;
	short unknown12;
};

struct s_path_links_data_view
{
	long node_count;
	s_pathfinding_node *nodes;
	long edge_count;
	s_path_edge_view *edges;
	byte unknown10[0x2c - 0x10];
	point3f *vertices;
	byte unknown30[8];
	long surface_count;
	s_path_surface_link_view *surfaces;
};

struct s_path_step_view
{
	short type;
	short unknown02;
	long node_index;
	short link_index;
	short unknown0a;
	s_type_c3b527 point;
};

struct s_path_result_view
{
	bool valid;
	s_type_c3b527 destination;
	long destination_node_index;
	real destination_distance;
	s_type_c3b527 start;
	bool complete;
	signed char step_count;
	byte step_index;
	s_path_step_view steps[4];
	long object_index;
	short type;
	bool unknowna6;
	s_path_location_view location;
};

bool function_26f3f0(s_pathfinding_data *pathfinding, long surface_index,
	s_type_c3b527 const *entry, long actor_index, s_type_f17a25 *state,
	s_type_c3b527 const *parent_point, long parent_node_index,
	long *parent_node_index_out, s_type_c3b527 *out, long *out_node_index);
void __stdcall function_2c2060(s_type_f17a25 *state, short count, s_path_step_view const *steps,
	short *out_count, s_path_step_view *out, bool *complete);
bool __stdcall function_2c41b0(long actor_index, s_type_f17a25 *state, short count,
	s_path_step_view const *steps, bool avoid, short *out_count,
	s_path_step_view *out, bool *complete, long *object_index, long *type, bool *flag);
bool function_1a8220(long index, short a, short b, long unknown, short c, short d, short e);

bool function_26f150(short type, point3f const *start, point3f const *end,
	point3f const *alternate_start, point3f const *alternate_end);

PRIVATE short function_272020(s_pathfinding_data const *pathfinding,
	s_path_node_key_view const *node, s_path_link_view *links, s_type_f17a25 const *state);
PRIVATE bool function_271630(s_type_f17a25 *state);
PRIVATE void function_271ef0(s_type_f17a25 *state, short index);
PRIVATE real function_272810(s_type_f17a25 const *state, s_type_c3b527 const *start,
	point3f const *end, real *distance_out);

PRIVATE void function_271e50(s_type_f17a25 *state, short index);
PRIVATE void function_271fd0(s_type_f17a25 *state, short node_index, short cost);
short function_272700(s_type_f17a25 *state, long node_index);
PRIVATE __declspec(noinline) void function_272740(point3f const *attractor,
	point3f const *start, point3f const *end, point3f *out);

// @retail 0x270590
void function_270590(s_path_source *source, s_type_c3b527 const *point, long node_index)
{
	s_path_input_view *input = (s_path_input_view *)source;
	input->start_valid = true;
	input->start = *point;
	input->start_node_index = node_index;
}

// @retail 0x2705c0
void function_2705c0(s_path_source *source, point3f const *point,
	real radius, long object_index, real weight, bool unknown)
{
	s_path_input_view *input = (s_path_input_view *)source;
	input->attractor_valid = true;
	input->attractor_point = *point;
	input->attractor_object_index = object_index;
	input->attractor_radius = radius;
	input->field_40 = weight;
	input->unknown44 = unknown;
}

// score 28 (was 28): BLOCKED-BY 0x26c590. Retail takes the location in edx and the node in ebx and has one exit (`xor al, al` at entry, `mov al, 1`); the one-exit
// form (`bool result = false;` ... `result = true; break;` ... `return result;`) matches, but unmatched 0x26c590 then takes its origin in
// ecx and its eight matched callers (0x1ab770, 0x1bd400, 0x1f5110, 0x26c4e0, 0x26cbe0, 0x26ccb0, 0x26d290, 0x2c39f0) stop matching
// (BLOCKED-BY 0x26c590). Tried in 0x26c590 with that form: `bool success` 553, line_normal as one dword copy 445, point1/distance1 at
// function scope 420, the edge iterator at function scope 420; hit at function scope 419; long count 424; line_normal as one struct
// copy 444 (retail's 0x64 frame), the same with the old exits 478, with bool success 507; none keeps origin on the stack.
// Separate noinline with complete/original exits 0/28; scan-position result 29; direct view with complete result 0; shifted bit 37; entry-pointer
// walk 27; walk plus result 39; word index plus result 21. Both matching forms and the word-index form lose the protected caller chain. Ordinary
// noinline/result confirmation matches here but loses all eight listed callers; all body/declaration trials restored. The pointer walk gains only
// one and is restored.
// Inline with original exits 28; inline plus single result MATCH fast, but seven matched reduced-image callers lose
// their match (the eighth already differs in that image); shared byte/short result 40/45; native index with original/
// shared exits 32/37; nested tests 28; active-bit-first tests 28 (same caller losses). All restored.
// Round 70: a one-file LTCG harness of 0x26c590 + this file reproduces the move (origin in ecx, ret 0x14). Retail keeps
// NONE in edx from entry to the loop test, so the axis pointer takes edi and origin stays on the stack. In 0x26c590 with
// the one-exit form: next index at function scope 430, it set to NONE there 419, bool success cleared at the count exit
// 544, first at function scope 419, nested location/flag tests 419; origin stays in ecx in all, the callers still lost.
// @retail 0x270600
PRIVATE bool function_270600(s_path_location const *location, long node_index)
{
	s_path_location_view const *view = (s_path_location_view const *)location;
	for (short i = 0; i < view->count; i++)
	{
		if (view->entries[i].unknown00 == NONE &&
			view->entries[i].node_index == node_index && (view->flags & (1 << i)))
		{
			return true;
		}
	}
	return false;
}

// @retail 0x270640
PRIVATE bool function_270640(s_type_f17a25 *state, s_type_c3b527 const *point,
	long node_index, bool *arg_c793c4, s_type_c3b527 *out)
{
	short index = function_272700(state, node_index);
	bool result = false;
	if (index != NONE)
	{
		s_path_lookup_view *lookup = (s_path_lookup_view *)state;
		s_path_node_key_view *node = &lookup->nodes[index];
		while (node->parent != NONE)
		{
			s_path_node_key_view *parent = &lookup->nodes[node->parent];
			if (point->output_index != parent->entry_point.output_index)
			{
				break;
			}
			s_path_trace_result trace;
			if (function_26c4e0((s_pathfinding_data *)state->pathfinding, point, node_index,
				&parent->entry_point, parent->node_index, 0, &trace))
			{
				break;
			}
			node = &lookup->nodes[node->parent];
		}
		if (node->parent == NONE)
		{
			*arg_c793c4 = true;
			*out = ((s_path_input_view *)&state->source)->start;
		}
		else
		{
			*arg_c793c4 = false;
			*out = node->entry_point;
		}
		result = true;
	}
	return result;
}

// @retail 0x270750
bool function_270750(byte *buffer, long unknown, s_actor_point_target const *target,
	real *distance, long a, long b)
{
	s_type_f17a25 *state = (s_type_f17a25 *)buffer;
	s_path_input_view *input = (s_path_input_view *)&state->source;
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	real *attractor_distance = (real *)a;
	vector3f *direction = (vector3f *)b;
	bool result = false;
	short index = function_272700(state, unknown);
	if (index != NONE)
	{
		s_path_node_key_view *node = &lookup->nodes[index];
		real path_distance = function_210970(&node->entry_point, target) + node->path_distance;
		real closest_distance;
		if (input->attractor_valid)
		{
			point3f start_point, end_point, closest;
			function_210850(&node->entry_point, &start_point);
			function_210850(target, &end_point);
			point3f const *attractor = &input->attractor_point;
			function_272740(attractor, &start_point, &end_point, &closest);
			vector3f delta;
			vector3d_from_points3d(attractor, &closest, &delta);
			closest_distance = (real)sqrt(delta.j * delta.j + delta.i * delta.i + delta.k * delta.k);
			if (closest_distance > node->attractor_distance)
				closest_distance = node->attractor_distance;
		}
		else
		{
			closest_distance = 0.0f;
		}
		if (attractor_distance)
		{
			*attractor_distance = closest_distance;
		}
		*distance = path_distance;
		result = true;
		if (direction)
		{
			short child = NONE;
			short current = index;
			real accumulated = 0.0f;
			do
			{
				node = &lookup->nodes[current];
				node->child = child;
				child = current;
				current = node->parent;
			}
			while (current != NONE);
			current = child;
			while (current != NONE && accumulated < 0.8f)
			{
				node = &lookup->nodes[current];
				current = node->child;
				accumulated += node->entry_distance;
			}
			s_type_c3b527 const *end = current == NONE ? target : &node->entry_point;
			function_210be0(&input->start, end, direction);
			function_30bf0(direction);
		}
	}
	else
	{
		if (attractor_distance)
		{
			*attractor_distance = FLT_MAX;
		}
		if (direction)
		{
			*direction = *g_4687a4;
		}
		*distance = FLT_MAX;
	}
	return result;
}

// score 315 (was 332): Frame 0xe8 versus retail 0xe4, acquisition, flags and scalar homes remain; geometry now uses shared differences.
// Earlier: Frame 0xe8 versus retail 0xe4; maximum/node acquisition, scalar homes, flags and vector scheduling differ.
// Earlier: the 0xe4 frame and state register now agree; initial load order, clearing, flag width and vector scheduling differ.
// Tried: pathfinding field at each use 459; bounded short initialization count 406; both 451; edge memset 408;
// all three 434; field reads plus memset 436; all three plus point-difference helpers 443; traversal state first 396;
// point-difference helpers alone/everywhere 398/398; depth-indexed step fields 385; direct data and step fields 372 (kept);
// that with a short initialization count 402; separate clearing loops 394; both 384.
// Signed/unsigned Boolean bit 380/372; capped word on unsigned/signed graphs 401/398; signed word/CRT
// clear 376; signed/CRT clear 393; entry counts 376; origin/target names 380; promoted cap 398, with CRT
// clear 377. All restored.
// Round 274: full-width initialization/link/path/scan carriers 400/363/378/414; initialization+link 401;
// link+path 441 (parent 282); path+scan 422; all four 407; previous-node presence 409, with initialization 433.
// Only the actual bounded full-width link count is retained: 363 plain, 1390/1388 bytes; parent stays 155.
// Round 248: real signed flag byte 370; signed/unsigned native flag-seven fields 363/363;
// signed flag field with link/complete entry initializers 360 (three-point gain restored), all
// constant traversal initializers 384, guarded edge memset 393, native capped word 372, all 412.
// Complete body and edge declarations restored; initial acquisition, clearing and vector homes remain.
// Round 250: actual advancing initialization receiver, short/long ordinal 349/401;
// safely capped word 374/374; separate edge memset 384/353, with word 384/386.
// Keep only the receiver with the original short ordinal: 349 plain, 1395/1388 bytes.
// Round 173: Native bounded word link count on the advancing initialization receiver, with geometry and
// history/receiver name subsets, 359 each at 1388 bytes. Independent traversal defaults at entry on those
// four forms 383 each at 1411 bytes, first +0x19. Geometry names canonicalize. Original long link count and
// full body restored.
// Round 187: Shared initial/traversal node receiver with capped-word/CRT-edge-fill/early-constant
// masks 0-7 350/374/384/384/372/343/396/411. Original advancing step receiver, link count and
// complete body restored.
// DH round 239: Native node/output link-record assignment, split increment, bounded
// word link count and full-width previous-index combinations 360/360/373/373/385/385/393/393.
// Original node declaration and complete body restored; frame acquisition and geometry homes remain.
// DH round 252: Real remaining-step short/long counts with advancing edge receiver, separate CRT edge fill,
// and their early traversal-constant forms
// 372/401/353/353/332/353/391/390. Keep trial 05, plain 332.
// DH round 254: entry edge/offset, delta and converted-point workspace subsets 315/332/315/332/315/332/315;
// all with a long step count 378. Shared differences at original scopes 315 plain, 1388/1388 bytes, kept;
// all entry-scope changes restored. Initial frame and indexed link transport still differ.
// @retail 0x270d90
PRIVATE short function_270d90(long actor_index, s_type_f17a25 *state,
	s_path_result_view const *result, short node_index, short maximum_steps,
	s_path_step_view *steps, bool *complete_out, short *link_count_out,
	s_path_location_entry_view *links, short maximum_links)
{
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	s_path_input_view const *input = (s_path_input_view *)&state->source;
	long edge_indices[40];
	short previous_index = NONE;
	s_path_node_key_view *previous = NULL;
	short count = 0;
	long link_count = 0;
	bool complete = true;
	long initialize_count = lookup->nodes[node_index].depth + 1;
	if (initialize_count > maximum_steps)
		initialize_count = maximum_steps;
	s_path_step_view *initialize_step = steps;
	long *initialize_edge = edge_indices;
	for (short remaining = initialize_count; remaining > 0; --remaining)
	{
		*initialize_edge++ = NONE;
		initialize_step->type = NONE;
		initialize_step->link_index = NONE;
		initialize_step->unknown02 = 0;
		initialize_step++;
	}
	while (node_index != NONE)
	{
		s_path_node_key_view *node = &lookup->nodes[node_index];
		if (link_count < maximum_links && (node->unknown10 != NONE ||
			(node->unknown14 >= 0 && node->unknown14 < ((s_path_links_data_view const *)state->pathfinding)->edge_count &&
			(((s_path_links_data_view const *)state->pathfinding)->edges[node->unknown14].flags & 0x80))))
		{
			memcpy(&links[link_count++], &node->unknown10, sizeof(*links));
		}
		if (node->depth >= maximum_steps)
		{
			complete = false;
		}
		else
		{
			if (count == 0)
				count = node->depth + 1;
			steps[node->depth].node_index = node->node_index;
			if (previous_index == NONE)
				steps[node->depth].point = result->destination;
			else
			{
				steps[node->depth].point = previous->entry_point;
				if (previous->unknown10 == NONE)
					edge_indices[node->depth] = previous->unknown14;
			}
			bool special = node->unknown10 == 2 &&
				((s_path_links_data_view const *)state->pathfinding)->surfaces[node->unknown14].unknown0a == 0;
			if (node->unknown10 == 1 || node->unknown10 == 6 ||
				node->unknown10 == 5 || special)
			{
				steps[node->depth - 1].type = node->unknown10;
				steps[node->depth - 1].link_index = (short)node->unknown14;
				s_path_node_key_view *parent = &lookup->nodes[node->parent];
				function_26f3f0((s_pathfinding_data *)state->pathfinding,
					node->unknown14, &node->entry_point, actor_index, state,
					&parent->entry_point, parent->node_index, &parent->node_index,
					&steps[node->depth - 1].point, &steps[node->depth - 1].node_index);
			}
			else if (node->unknown10 == 2)
			{
				steps[node->depth - 1].type = node->unknown10;
				steps[node->depth - 1].link_index = (short)node->unknown14;
			}
		}
		previous_index = node_index;
		previous = node;
		node_index = node->parent;
	}
	real margin = input->radius * 1.2f;
	for (short j = 0; j < count; j++)
	{
		long edge_index = edge_indices[j];
		if (edge_index >= 0 && edge_index < ((s_path_links_data_view const *)state->pathfinding)->edge_count && edge_index != 0xffff)
		{
			s_type_c3b527 const *previous_point = j > 0 ? &steps[j - 1].point : &input->start;
			s_path_edge_view const *edge = &((s_path_links_data_view const *)state->pathfinding)->edges[edge_index];
			s_type_c3b527 *point = &steps[j].point;
			/* Retail widens these unsigned words before comparing against NONE. */
			if ((long)edge->vertices[0] != NONE && (long)edge->vertices[1] != NONE)
			{
				point3f const *a = &((s_path_links_data_view const *)state->pathfinding)->vertices[edge->vertices[0]];
				point3f const *b = &((s_path_links_data_view const *)state->pathfinding)->vertices[edge->vertices[1]];
				vector3f edge_vector;
				vector3d_from_points3d(a, b, &edge_vector);
				vector3f offset;
				vector3d_from_points3d(a, &previous_point->point, &offset);
				real length = function_30bf0(&edge_vector);
				if (length > margin * 2.0f)
				{
					real projection = offset.k * edge_vector.k + offset.j * edge_vector.j +
						offset.i * edge_vector.i;
					if (margin > projection)
						projection = margin;
					else if (projection > length - margin)
						projection = length - margin;
					point->point.x = a->x + edge_vector.i * projection;
					point->point.y = a->y + edge_vector.j * projection;
					point->point.z = a->z + edge_vector.k * projection;
				}
			}
			vector3f delta;
			if (input->start.output_index == point->output_index)
			{
				delta.i = point->point.x - input->start.point.x;
				delta.j = point->point.y - input->start.point.y;
				delta.k = point->point.z - input->start.point.z;
			}
			else
			{
				point3f start, end;
				if (input->start.output_index == NONE ||
					!function_2104b0(input->start.output_index, &input->start.point, &start))
					start = input->start.point;
				if (point->output_index == NONE ||
					!function_2104b0(point->output_index, &point->point, &end))
					end = point->point;
				delta.i = end.x - start.x;
				delta.j = end.y - start.y;
				delta.k = end.z - start.z;
			}
			if (delta.k * delta.k + delta.j * delta.j + delta.i * delta.i > 1600.0f && j < count - 1)
			{
				count = j + 1;
				complete = false;
			}
		}
	}
	if (complete_out)
		*complete_out = complete;
	if (link_count_out)
		*link_count_out = link_count;
	return count;
}

// @retail 0x270930
bool function_270930(long actor_index, s_type_f17a25 *state, s_path_result_view *result)
{
	byte *actor = NULL;
	if (actor_index != NONE)
		actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
	s_path_input_view const *input = (s_path_input_view *)&state->source;
	s_path_destination_view const *destination = (s_path_destination_view *)state;
	s_path_closest_view const *closest = (s_path_closest_view *)state;
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	result->valid = false;
	result->location = *(s_path_location_view *)&state->location;
	result->unknowna6 = false;
	if (destination->destination_valid)
	{
		short index = function_272700(state, destination->destination_node_index);
		if (index != NONE)
		{
			memcpy(&result->destination, &destination->destination, 24);
			result->destination_distance = 0.0f;
		}
		else if (destination->destination_radius > closest->distance)
		{
			index = closest->node_index;
			result->destination = closest->point;
			result->destination_node_index = lookup->nodes[index].node_index;
			result->destination_distance = closest->distance;
		}
		if (index != NONE)
		{
			s_path_step_view local_0af630[40], name_b343c4[4], final_steps[4];
			s_path_location_entry_view links[10];
			short smoothed_count = 0, final_count = 0, link_count = 0;
			bool complete = true, success = true;
			short raw_count = function_270d90(actor_index, state, result, index, 40,
				local_0af630, &complete, &link_count, links, 10);
			function_2c2060(state, raw_count, local_0af630, &smoothed_count, name_b343c4, &complete);
			if (actor && actor[0x478])
			{
				final_count = smoothed_count > 4 ? 4 : smoothed_count;
				memcpy(final_steps, name_b343c4, final_count * sizeof(*final_steps));
			}
			else
			{
				long object_index = NONE, type = NONE;
				bool avoid = state->unknownac;
				if (!avoid && result->location.flags > 0)
					avoid = (result->location.flags / (1 << (byte)result->location.count)) % 2 == 1;
				success = function_2c41b0(actor_index, state, smoothed_count, name_b343c4, avoid,
					&final_count, final_steps, &complete, &object_index, &type, &result->unknowna6);
				if (success)
				{
					result->object_index = object_index;
					result->type = (short)type;
					if (object_index != NONE && actor_index != NONE && (short)type == 6)
					{
						real duration = (real)g_510c54->field_2_3 * 0.2f;
						long ticks;
						__asm { fld duration }
						__asm { fistp ticks }
						function_1a8220(actor_index, 9, (short)ticks, 3, 1, 42, 3);
					}
				}
				else
				{
					result->object_index = NONE;
					result->type = NONE;
				}
			}
			if (result->location.flags == 0)
			{
				s_path_location_view *location = &result->location;
				if (link_count > 0)
					location->entries[0] = links[0];
				if (link_count > 1)
					location->entries[1] = links[link_count - 1];
				if (link_count > 2)
					location->entries[2] = links[link_count / 2];
				location->count = link_count < 0 ? 0 : link_count > 3 ? 3 : link_count;
			}
			if (success)
			{
				result->complete = complete;
				result->step_count = (signed char)final_count;
				result->valid = true;
				result->step_index = 0;
				result->start = input->start;
				memcpy(result->steps, final_steps, final_count * sizeof(*final_steps));
				if (result->complete)
				{
					if (result->step_count > 0)
					{
						result->destination = result->steps[result->step_count - 1].point;
						result->destination_node_index = result->steps[result->step_count - 1].node_index;
					}
					else
					{
						result->destination = input->start;
						result->destination_node_index = input->start_node_index;
					}
					result->destination_distance = function_210970(&result->destination, &destination->destination);
					return result->valid;
				}
			}
			else if (++result->location.flags > 32)
				result->location.flags = 0;
		}
	}
	return result->valid;
}

// @retail 0x2713c0
void function_2713c0(s_type_f17a25 *state, s_type_c3b527 const *point,
	long node_index, real radius)
{
	s_path_destination_view *destination = (s_path_destination_view *)state;
	destination->destination_valid = true;
	destination->destination = *point;
	destination->destination_node_index = node_index;
	destination->destination_radius = radius;
}

// @retail 0x2713f0
PRIVATE bool function_2713f0(s_type_f17a25 *state)
{
	s_path_input_view *input = (s_path_input_view *)&state->source;
	s_path_destination_view *destination = (s_path_destination_view *)state;
	bool result = false;
	if (!state->pathfinding)
	{
		return false;
	}
	long node_count = *(long *)state->pathfinding;
	if (input->start_node_index >= 0 && input->start_node_index < node_count &&
		input->start.point.z > -1000.0f)
	{
		real distance;
		long quantized;
		if (destination->destination_valid)
		{
			distance = function_210970(&input->start, &destination->destination);
			quantized = (long)(distance * 10.0f);
		}
		else
		{
			distance = 0.0f;
			quantized = 0;
		}
		if (quantized < 32767)
		{
			s_pathfinding_node *sector = &((s_pathfinding_data *)state->pathfinding)->nodes[input->start_node_index];
			short index = state->unknownae++;
			s_path_lookup_view *lookup = (s_path_lookup_view *)state;
			s_path_node_key_view *node = &lookup->nodes[index];
			node->parent = NONE;
			node->unknown04 = NONE;
			node->node_index = input->start_node_index;
			node->entry_point = input->start;
			node->entry_distance = 0.0f;
			node->path_distance = 0.0f;
			node->attractor_distance = FLT_MAX;
			node->cost = 0.0f;
			node->estimated_distance = distance;
			node->quantized_cost = (short)quantized;
			node->depth = 0;
			node->flag0c = (sector->flags >> 12) & 1;
			node->flag0d = (sector->flags & 0x3c0) != 0;
			node->flag0e = false;
			node->unknown10 = NONE;
			node->unknown14 = NONE;
			if (destination->destination_valid)
			{
				state->unknown90 = index;
				*(real *)((byte *)state + 0x94) = distance;
				*(s_type_c3b527 *)((byte *)state + 0x9c) = input->start;
				*(real *)((byte *)state + 0x98) = distance;
			}
			lookup->hash_table[(node->node_index & 511) * 8] = index;
			function_271fd0(state, index, (short)quantized);
			result = true;
		}
	}
	return result;
}

// @retail 0x2715a0
bool function_2715a0(byte *buffer)
{
	s_type_f17a25 *state = (s_type_f17a25 *)buffer;
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	s_path_closest_view *closest = (s_path_closest_view *)state;
	bool result = false;
	state->unknownae = 0;
	state->heap_count = 1;
	memset(lookup->hash_table, 0xff, sizeof(lookup->hash_table));
	closest->node_index = NONE;
	closest->distance = FLT_MAX;
	closest->estimated_distance = FLT_MAX;
	if (function_2713f0(state))
	{
		result = function_271630(state);
	}
	if (!result)
	{
		if (state->location.unknown00 == 0)
		{
			state->location.unknown00 = 32;
		}
		else
		{
			s_path_location *location = &state->location;
			if (++location->unknown00 > 32)
			{
				location->unknown00 = 0;
			}
		}
	}
	return result;
}

// score 501 (was 501): State/node/link registers, scalar homes and heap-helper expansion differ.
// Earlier field-bit mask forms 501 (from 533).
// State EBX/EBP, node/link cursors and scalar homes remain. Force-inline insertion 577 (loses matched caller);
// scalar role names 501; early inherited result 503; squared-length helpers 500; point setter 501; plain
// result 517; shared inherited result 538; plain shared result 501; direct views 510; early projection margin
// 502; ordinary inline insertion 497 (four-count gain below threshold). All restored; one-count helper gain
// rejected.
// Frame 0xc54 agrees; state EBX/EBP, scalar homes, link ownership and selective heap expansion remain.
// Earlier: two mask tests read as field bits (TEST_FIELD_BIT) give retail's load-and-shift; the rest still differs.
// DH round 254: actual link cursors with count/ordinal widths and readonly ownership
// 518/544/497/497/518/544/497/497. Wide cursor with direct high/low/both masks 529/525/527;
// both masks with wide count 527. All restored; 497 gains only four, and native mask forms shorten away from retail.
// @retail 0x271630
PRIVATE bool function_271630(s_type_f17a25 *state)
{
	s_path_input_view *input = (s_path_input_view *)&state->source;
	s_path_destination_view *destination = (s_path_destination_view *)state;
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	s_path_closest_view *closest = (s_path_closest_view *)state;
	s_path_location_view *location = (s_path_location_view *)&state->location;
	real radius = 0.2f > input->radius ? 0.2f : input->radius;
	real link_penalty = input->link_penalty;
	bool volatile result = true;
	s_path_link_view links[64];
	while (state->heap_count > 1)
	{
		short index = state->heap[1].node;
		s_path_node_key_view *node = &lookup->nodes[index];
		node->heap_index = NONE;
		if (--state->heap_count > 1)
		{
			state->heap[1] = state->heap[state->heap_count];
			function_271ef0(state, 1);
		}
		if (index == NONE)
		{
			break;
		}
		if (destination->destination_valid)
		{
			if (node->node_index == destination->destination_node_index)
			{
				closest->point = destination->destination;
				closest->node_index = index;
				closest->distance = 0.0f;
				break;
			}
			real limit = 5.0f > closest->distance ? 5.0f : closest->distance;
			if (node->estimated_distance > limit * 10.0f + closest->estimated_distance)
			{
				break;
			}
		}
		short count = function_272020((s_pathfinding_data *)state->pathfinding, node, links, state);
		for (short i = 0; i < count; ++i)
		{
			s_path_link_view *link = &links[i];
			if (link->node_index == node->unknown04 && !link->unknown2e && !node->flag0e)
			{
				continue;
			}
			if (!(link->flags & 1) || (node->flag0c && TEST_FIELD_BIT((link->flags >> 12) & 1)))
			{
				continue;
			}
			bool blocked = false;
			if (location->flags > 0)
			{
				for (short j = 0; j < location->count; ++j)
				{
					if ((location->flags & (1 << j)) &&
						location->entries[j].unknown00 == (short)link->type &&
						location->entries[j].node_index == link->index)
					{
						blocked = true;
						break;
					}
				}
			}
			if (blocked || (!input->unknown04 && TEST_FIELD_BIT((link->flags >> 1) & 1) &&
				function_1fa6b0(&((s_pathfinding_data *)state->pathfinding)->nodes[link->node_index],
					(s_pathfinding_data *)state->pathfinding)))
			{
				continue;
			}
			real length_squared = link->vector.i * link->vector.i +
				link->vector.j * link->vector.j + link->vector.k * link->vector.k;
			real diameter = radius * 2.0f;
			if (link->unknown2c && !link->unknown2d && diameter * diameter > length_squared)
			{
				continue;
			}
			s_type_c3b527 point;
			point.point.x = link->vector.i * 0.5f + link->point.point.x;
			point.point.y = link->vector.j * 0.5f + link->point.point.y;
			point.point.z = link->vector.k * 0.5f + link->point.point.z;
			point.output_index = link->point.output_index;
			if (destination->destination_valid && length_squared > 16.0f &&
				length_squared > diameter * diameter)
			{
				real length = (real)sqrt(length_squared);
				vector3f to_destination;
				function_210be0(&link->point, &destination->destination, &to_destination);
				real t = (link->vector.j * to_destination.j +
					link->vector.k * to_destination.k + link->vector.i * to_destination.i) /
					(link->vector.i * link->vector.i + link->vector.j * link->vector.j +
					link->vector.k * link->vector.k);
				real margin = radius / length;
				if (margin > t)
				{
					t = margin;
				}
				else if (t > 1.0f - margin)
				{
					t = 1.0f - margin;
				}
				point.point.x = link->vector.i * t + link->point.point.x;
				point.point.y = link->vector.j * t + link->point.point.y;
				point.point.z = link->vector.k * t + link->point.point.z;
			}
			real distance = function_210970(&node->entry_point, &point);
			real entry_distance = distance;
			real path_distance = distance + node->path_distance;
			real attractor_distance;
			real entry_cost;
			if (input->attractor_valid)
			{
				real weight = function_272810(state, &node->entry_point, &point.point, &attractor_distance);
				entry_cost = (weight + 1.0f) * entry_distance;
				attractor_distance = node->attractor_distance > attractor_distance ?
					attractor_distance : node->attractor_distance;
			}
			else
			{
				attractor_distance = 0.0f;
				entry_cost = entry_distance;
			}
			if (link_penalty > 0.0f)
			{
				switch ((short)link->type)
				{
				case 1: case 2: case 5: case 6:
					entry_cost += link_penalty;
					break;
				}
			}
			real cost = node->cost + entry_cost;
			real estimated_distance = cost;
			real destination_distance;
			if (destination->destination_valid)
			{
				real remaining = function_210970(&point, &destination->destination);
				destination_distance = remaining;
				estimated_distance = remaining + cost;
			}
			long quantized = (long)(estimated_distance * 10.0f);
			if (quantized >= 32767 || (input->distance_limit_valid && path_distance > input->distance_limit))
			{
				continue;
			}
			short hash = (short)((link->node_index & 511) * 8);
			short next = lookup->hash_table[hash];
			while (next != NONE)
			{
				s_path_node_key_view *candidate = &lookup->nodes[next];
				if (candidate->node_index == link->node_index)
				{
					if (!candidate->flag0e && !link->unknown2e)
					{
						break;
					}
					real dx = candidate->entry_point.point.x - link->point.point.x;
					real dy = candidate->entry_point.point.y - link->point.point.y;
					real dz = candidate->entry_point.point.z - link->point.point.z;
					if (dx * dx + dz * dz + dy * dy <= 0.09f)
					{
						break;
					}
				}
				hash = (short)((hash + 1) & 4095);
				next = lookup->hash_table[hash];
			}
			if (next == NONE)
			{
				next = state->unknownae;
				if (next >= 1024)
				{
					continue;
				}
				++state->unknownae;
				lookup->hash_table[hash] = next;
				lookup->nodes[next].heap_index = NONE;
			}
			else if (quantized >= lookup->nodes[next].quantized_cost || lookup->nodes[next].heap_index == NONE)
			{
				continue;
			}
			if (next == NONE)
			{
				continue;
			}
			s_path_node_key_view *child = &lookup->nodes[next];
			child->parent = index;
			child->unknown04 = node->node_index;
			child->node_index = link->node_index;
			child->entry_point = point;
			child->entry_distance = entry_distance;
			child->attractor_distance = attractor_distance;
			child->path_distance = path_distance;
			child->cost = cost;
			child->estimated_distance = estimated_distance;
			child->quantized_cost = (short)quantized;
			child->flag0c = (link->flags >> 12) & 1;
			child->flag0d = (link->flags & 0x3c0) != 0;
			child->flag0e = link->unknown2e;
			*(long *)&child->unknown10 = link->type;
			child->unknown14 = link->index;
			short depth = node->depth;
			switch ((short)link->type)
			{
			case 1: case 5: case 6:
				depth += 2;
				break;
			case 2:
				depth += *(short *)((byte *)&((s_pathfinding_data *)state->pathfinding)->surfaces[link->index] + 0xa) == 0 ? 2 : 1;
				break;
			default:
				++depth;
				break;
			}
			child->depth = depth;
			if (child->heap_index == NONE)
			{
				function_271fd0(state, next, (short)quantized);
			}
			else
			{
				state->heap[child->heap_index].cost = (short)quantized;
				function_271e50(state, child->heap_index);
			}
			if (destination->destination_valid && destination->destination_radius > 0.0f &&
				closest->distance > destination_distance)
			{
				closest->point = child->entry_point;
				closest->node_index = next;
				closest->distance = destination_distance;
				closest->estimated_distance = estimated_distance;
			}
		}
	}
	if (destination->destination_valid)
	{
		if (destination->destination_radius >= closest->distance)
		{
			return true;
		}
		return false;
	}
	return result;
}

// score 0 (was 8): old index and signed slot registers differ, including existing heap helper input;
// native long insertion index 21; increment the count field 8; postincrement count at insertion 8; shared
// native heap element pointer 16; guard return for full heap 8; name the next count expression 8; store
// cost before node 10; native element pointer with postincrement count 16; consistent existing heap helper
// parameter order 8.
// @retail 0x271fd0
PRIVATE void function_271fd0(s_type_f17a25 *state, short node_index, short cost)
{
	short index = state->heap_count;
	if (index < 1024)
	{
		state->heap_count = index + 1;
		state->heap[index].node = node_index;
		state->heap[index].cost = cost;
		function_271e50(state, index);
	}
}

/* a flag field read as word bits: retail reads these flags with a load, a
   shift and a test of the low bit */
struct s_flag_word_bits
{
	word bit0 : 1;
	word bit1 : 1;
	word bit2 : 1;
	word bit3 : 1;
	word bit4 : 1;
	word bit5 : 1;
	word bit6 : 1;
	word bit7 : 1;
	word bit8 : 1;
	word bit9 : 1;
	word bit10 : 1;
	word bit11 : 1;
	word bit12 : 1;
	word bit13 : 1;
	word bit14 : 1;
	word bit15 : 1;
};

/* Surface records interpret their payload differently by type. Retail's
   vertex indices are unsigned words: its comparisons with long NONE do not
   reject 0xffff. Only destination sector words have an effective NONE test.
   Link type stores write the low short; the upper short stays untouched. */
// score 452 (was 452): Frame 0x2c/0x34, data/count registers and vector endpoint homes differ.
// Earlier surface word-bit view 452 (from 475).
// Frame 0x2c/0x34, data/count registers and endpoint homes remain. Signed edge byte 464; direct parameter 452;
// native surface bits 452; long surface cursor 483; output cursor 541; indexed vector helper 452; native
// flags/direct parameter 464; paired cursors 555; ring-anchor reads at use 469; cached node array 474; reused
// start vertices 485. All restored.
// Frame 0x2c versus retail 0x34, root/count registers and endpoint spill ownership remain.
// Earlier: flag 1 of the surface read through a word bitfield view gives retail's load-and-shift; the rest still differs.
// DH round 254: both endpoints before copies in edge-surface/paired-surface/ring subsets
// 449/452/449/450/447/450/447; all with consistent endpoint storage names 447. All restored: gains
// are at most five, and the one-use end pointers require an exact match. Original guards and helpers retained.
// @retail 0x272020
PRIVATE short function_272020(s_pathfinding_data const *pathfinding,
	s_path_node_key_view const *node, s_path_link_view *links, s_type_f17a25 const *state)
{
	/* retail keeps pathfinding on the stack */
	s_pathfinding_data const *const *pathfinding_reference = &pathfinding;
	s_path_links_data_view const *data = (s_path_links_data_view const *)*pathfinding_reference;
	long node_index = node->node_index;
	short output_index = node->entry_point.output_index;
	short count = 0;
	short surface_index = data->nodes[node_index].first_surface;
	if (data->surface_count > 0)
	{
		while (surface_index != NONE && count < 64)
		{
			s_path_surface_link_view const *surface = &data->surfaces[surface_index];
			if (state->settings.flags & (1 << surface->type))
			{
				switch (surface->type)
				{
				case 0:
				{
					long edge_index = *(long const *)&surface->unknown04;
					s_path_edge_view const *edge = &data->edges[edge_index];
					word next_node = edge->nodes[node_index != edge->nodes[1]];
					if ((long)next_node != NONE && next_node != (word)NONE)
					{
						s_path_link_view *link = &links[count];
						link->node_index = next_node;
						link->index = edge_index;
						*(short *)&link->type = NONE;
						link->flags = data->nodes[next_node].flags;
						link->point.point = data->vertices[edge->vertices[0]];
						link->point.output_index = output_index;
						vector3d_from_points3d(&data->vertices[edge->vertices[0]],
							&data->vertices[edge->vertices[1]], &link->vector);
						link->unknown2c = (edge->flags >> 7) & 1;
						link->unknown2d = true;
						link->unknown2e = false;
						++count;
					}
					break;
				}
				case 1:
				case 6:
				{
					if ((long)surface->unknown04 == NONE || (long)surface->unknown06 == NONE ||
						(long)surface->unknown08 == NONE || (long)surface->unknown0a == NONE)
					{
						break;
					}
					if (!state->settings.unknown0c &&
						(state->settings.unknown08 & surface->unknown0d) <= 0)
					{
						break;
					}
					word next_node = surface->unknown0e;
					if ((long)next_node == NONE || next_node == (word)NONE)
					{
						break;
					}
					short flags = data->nodes[next_node].flags;
					if (node->flag0d)
					{
						vector3f delta;
						vector3d_from_points3d(&data->vertices[surface->unknown04],
							&node->entry_point.point, &delta);
						if (delta.k * delta.k + delta.j * delta.j + delta.i * delta.i > 0.09f)
						{
							break;
						}
					}
					if (!(surface->unknown0c & 1) && surface->unknown10 != surface->unknown12)
					{
						point3f start, end;
						function_2104b0(surface->unknown10, &data->vertices[surface->unknown04], &start);
						function_2104b0(surface->unknown12, &data->vertices[surface->unknown08], &end);
						bool traversable = false;
						for (short type = 0; type < 6; ++type)
						{
							long bit = 1 << type;
							if ((state->settings.unknown08 & bit) && function_26f150(type, &start, &end, NULL, NULL))
							{
								traversable = true;
								break;
							}
							if (bit & surface->unknown0d)
							{
								break;
							}
						}
						if (!traversable)
						{
							break;
						}
					}
					s_path_link_view *link = &links[count];
					*(short *)&link->type = surface->type;
					link->index = surface_index;
					link->node_index = surface->unknown0e;
					link->unknown2c = true;
					link->unknown2d = true;
					link->flags = flags | 1;
					link->point.point = data->vertices[surface->unknown08];
					link->point.output_index = surface->unknown12;
					link->unknown2e = surface->type == 6;
					vector3d_from_points3d(&data->vertices[surface->unknown08],
						&data->vertices[surface->unknown0a], &link->vector);
					++count;
					break;
				}
				case 2:
				{
					if ((long)surface->unknown06 == NONE || (long)surface->unknown08 == NONE)
					{
						break;
					}
					word next_node = surface->unknown04;
					if ((long)next_node == NONE || next_node == (word)NONE)
					{
						break;
					}
					point3f const *point = &data->vertices[surface->unknown06];
					if (surface->unknown0a == 1 || surface->unknown0a == 2)
					{
						real dx = node->entry_point.point.x - point->x;
						real dy = node->entry_point.point.y - point->y;
						if (dy * dy + dx * dx > 0.09f)
						{
							break;
						}
					}
					s_path_link_view *link = &links[count];
					link->index = surface_index;
					*(short *)&link->type = 2;
					link->node_index = next_node;
					link->flags = data->nodes[next_node].flags | 1;
					link->point.point = *point;
					link->point.output_index = output_index;
					link->vector = *g_4687a4;
					link->unknown2c = true;
					link->unknown2d = true;
					link->unknown2e = surface->unknown0a == 3;
					++count;
					break;
				}
				case 5:
				{
					if ((long)surface->unknown06 == NONE || (long)surface->unknown08 == NONE ||
						(long)surface->unknown0a == NONE)
					{
						break;
					}
					word next_node = surface->unknown04;
					if ((long)next_node == NONE || next_node == (word)NONE)
					{
						break;
					}
					long flags = state->settings.unknown04;
					if (!(((flags & 0x400) && (surface->unknown0c & 1)) ||
						((flags & 0x800) && TEST_FIELD_BIT(((s_flag_word_bits *)&surface->unknown0c)->bit1)) ||
						((flags & 0x1000) && (surface->unknown0c & 4))))
					{
						break;
					}
					s_path_link_view *link = &links[count];
					link->index = surface_index;
					*(short *)&link->type = 5;
					link->node_index = next_node;
					link->flags = data->nodes[next_node].flags;
					link->point.point = data->vertices[surface->unknown08];
					link->point.output_index = output_index;
					link->vector = *g_4687a4;
					link->unknown2c = true;
					link->unknown2d = true;
					link->unknown2e = false;
					++count;
					break;
				}
				}
			}
			surface_index = surface->next;
		}
	}
	/* Retail enters the ring even if the surface pass already filled 64 slots.
	   Its count check follows insertion; keep that behavior here. */
	if ((data->nodes[node_index].flags & 1) || (count == 0 && node->parent == NONE))
	{
		long first_edge = *(long const *)data->nodes[node_index].unknown4;
		long edge_index = first_edge;
		do
		{
			s_path_edge_view const *edge = &data->edges[edge_index];
			bool reverse = node_index == edge->nodes[1];
			long next_node = edge->nodes[!reverse];
			if (next_node != NONE && next_node != 0xffff)
			{
				s_path_link_view *link = &links[count];
				link->index = edge_index;
				link->node_index = next_node;
				*(short *)&link->type = NONE;
				link->flags = data->nodes[next_node].flags;
				link->unknown2c = (edge->flags >> 7) & 1;
				link->unknown2d = false;
				link->unknown2e = false;
				link->point.point = data->vertices[edge->vertices[0]];
				link->point.output_index = output_index;
				vector3d_from_points3d(&data->vertices[edge->vertices[0]],
					&data->vertices[edge->vertices[1]], &link->vector);
				++count;
			}
			if (count == 64)
			{
				break;
			}
			edge_index = edge->next_edges[reverse];
		}
		while (edge_index != first_edge);
	}
	return count;
}

// @retail 0x272700
short function_272700(s_type_f17a25 *state, long node_index)
{
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	short hash_index = (node_index & 511) * 8;
	short result;
	do
	{
		result = lookup->hash_table[hash_index];
		hash_index = (hash_index + 1) & 4095;
	}
	while (result != NONE && lookup->nodes[result].node_index != node_index);
	return result;
}

// score 16 (was 67): retail computes the segment i, j, k and sums (i + j) + k for the length and (j + k) + i for the projection, ours j, k, i and (k + j) + i for both; tried: `dot3f(&delta, &delta)` for the length 16, `dot3f(&delta, &offset)` 16, the projection written out in y, z, x order 36, all three written out 67, parameters reordered (start, end, attractor, out) 16; compile-only harness: every written-out sum order and grouping 27-64, all 24 parameter orders and the offset computed at the top no better.
// Complete output object/early admissions/uninitialized offset scope/role names/indexed interpolation/native output aggregate/parallel projection
// products/combined indexed projection 111/49/16/16/21/99/85/85; all restored.
// Indexed unit-scale segment/offset differences 16; all three named squares 62; native point publication 31;
// complete start/end values 139/131; named projection 16; indexed segment plus all named squares 62;
// all named squares plus named projection 62. Original complete body and noinline declaration retained.
// @retail 0x272740
PRIVATE void function_272740(point3f const *attractor,
	point3f const *start, point3f const *end, point3f *out)
{
	vector3f delta;
	vector3d_from_points3d(start, end, &delta);
	real length_squared = length_sq3f(&delta);
	if (length_squared > 0.0f)
	{
		vector3f offset;
		vector3d_from_points3d(attractor, start, &offset);
		real t = dot3f(&offset, &delta) / length_squared;
		if (t < 0.0f || t > 1.0f)
		{
			*out = *end;
		}
		else
		{
			out->x = delta.i * t + start->x;
			out->y = delta.j * t + start->y;
			out->z = delta.k * t + start->z;
		}
	}
	else
	{
		*out = *start;
	}
}

// score 8 (was 97): retail computes the segment i, k, j and sums its square as (k + i) + j; ours computes j, k, i and sums
// (k + j) + i. Tried: the weight in float (no double local) 88 -> 82; weight declared first 81; written-out sums in every order
// 79-94; the segment, offset and both lengths through vector3d_from_points3d/length_sq3f/dot3f 21; the length test against g_45dbd8
// (retail reads 0x45dbd8; weight's zero is then not shared) 8; dot3f for either length and the swapped projection 8; separate
// segment/delta vectors 8; a `0.0` double test 102; the permuter (86 tries) found nothing below 8; distance_sq3f for the segment
// 132 fast/100 plain; vector3f_assign 8; offset or t at function scope 8 each; delta renamed segment 8; closest declared first 8;
// input declared later 8; length declaration split 8; aggregate segment initialization 83.
// Indexed unit-scale difference helper for the segment or all differences 8/8; all three named segment squares 83;
// all three named component differences plus the native setter 8; complete start-point value 117; named projection 8;
// complete native point publication 30; complete start-point value plus named projection 117. All restored.
// @retail 0x272810
PRIVATE real function_272810(s_type_f17a25 const *state, s_type_c3b527 const *start,
	point3f const *end, real *distance_out)
{
	s_path_input_view const *input = (s_path_input_view const *)&state->source;
	real weight = 0.0f;
	real distance = FLT_MAX;
	point3f local_end;
	point3f local_attractor;
	function_210690(start->output_index, end, &local_end);
	function_210690(start->output_index, &input->attractor_point, &local_attractor);
	vector3f delta;
	vector3d_from_points3d(&start->point, &local_end, &delta);
	real length_squared = length_sq3f(&delta);
	point3f closest;
	if (length_squared > g_45dbd8)
	{
		vector3f offset;
		vector3d_from_points3d(&local_attractor, &start->point, &offset);
		real t = dot3f(&offset, &delta) / length_squared;
		if (t < 0.0f || t > 1.0f)
		{
			closest = local_end;
		}
		else
		{
			closest.x = delta.i * t + start->point.x;
			closest.y = delta.j * t + start->point.y;
			closest.z = delta.k * t + start->point.z;
		}
	}
	else
	{
		closest = start->point;
	}
	vector3d_from_points3d(&local_attractor, &closest, &delta);
	real distance_squared = length_sq3f(&delta);
	if (input->attractor_radius * input->attractor_radius > distance_squared)
	{
		distance = (real)sqrt(distance_squared);
		weight = (1.0f - distance / input->attractor_radius) * input->field_40;
	}
	*distance_out = distance;
	return weight;
}
