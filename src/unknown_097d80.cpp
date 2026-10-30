#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_096ed0.h"

// @flags /O2 /Gr

/* ---- class A: an aggregate of three child objects ---- */
struct c_child;

struct s_block_item
{
	c_child *object;
	long unknown04;
};

struct s_block
{
	byte unknown00[0x3c];
	long count;
	long unknown40;
	s_block_item items[7];
	long unknown7c;
};

struct c_child
{
	virtual void v0(long, long) {}
	virtual bool v1(c_child *, long *) { return false; }
	virtual void v2() {}
	virtual void v3() {}
	virtual void v4() {}
	virtual long v5(dword, void *, long, s_block *, long *) { return 0; }
	virtual void v6(s_block *) {}
	virtual void v7(void *) {}
	virtual void v8(long, long) {}
};

struct s_request
{
	byte unknown00[0xc];
	long type;
};

struct c_aggregate : c_child
{
	byte unknown04[8];
	c_child *children[3];

	void v7(void *a);
	void v8(long a, long b);

};


// score 85 (was 85): frame 0x8024 against 0x8020 (retail packs the completion output with two later
// Round 73: ordinal/child-cursor dispatch 115; explicit inner length on ordinal/range forms 102/99;
// guarded count-down block traversal 130; shared output/output as count/result 84/84/84;
// produced count reused as cleanup total 130. Opening body restored.
// our frame is 0x8024 against retail's 0x8020 (retail packs the completion
// Completion-output home, ordinal dispatch and allocator reload remain. Cumulative boundaries with
// carried total/direct table/local output/table cursor, original and ordinal dispatch, scored
// 134/155, 140/157, 132/141, 142/157. Direct boundary outputs recover frame 0x8020 but lengthen
// dispatch; original collection and cleanup restored.
// Earlier: our frame is 0x8024 against retail's 0x8020 (retail packs the completion
// output with two later spills), and the dispatch loop and one allocator reload still differ:
// retail loads the global again for the release, into eax then edi.
// Frame 0x8024 versus 0x8020, scalar output homes, dispatch ownership and allocator reload remain.
// Frame 0x8024 versus 0x8020, completion-output home, dispatch ownership and allocator reload remain.
// Earlier 85 (was 92): frame 0x8024 against 0x8020 (retail packs the completion output with two later
// Frame 0x8024 versus 0x8020, completion/dispatch homes and allocator observations remain.
// Earlier: frame 0x8024 against 0x8020 (retail packs the completion output with two later
// spills), the dispatch loop (retail: index in ebp, child cursor and inner count spilled) and one
// allocator reload (retail loads the global again for the release, into eax then edi) still differ.
// Round 53: compiled without /GL (screen only) this body gets retail's 0x8020 frame and slots
// (completion output at the bottom, error and release output above it) and a matching first loop;
// every /GL form screened keeps 0x8024. Release through the file-local release helper of
// unknown_096ed0.cpp (global read again after the query) 102f, with an indexed dispatch 119f, with
// a child cursor 119f; completion output at loop scope 113f; helper with an object local 102f;
// output/error names and scopes (36 screened) don't move the frame.
// Kept: item test on the field and the allocator global held across the release 85.
// Global held across the release only 100; indexed dispatch 104, with the hold 115; class operator
// delete 100, with indexed dispatch 115; release through a local helper taking the globals 103;
// field test with indexed dispatch 119; completion output at loop scope 113; completion output
// renamed 92 (frame unchanged); held global with the item test on the local 106. On the kept
// body: indexed dispatch 115, reading the child at each use 128; completion output at loop or
// function scope 96/96 (frame stays 0x8024); total declared before error 85.
// Cleanup block/item/both cursors
// frame and request/cursor registers agree; output homes,
// Frame 0x8020 agrees; output homes, dispatch and release allocation remain.
// Earlier: frame and request/cursor registers agree; output homes,
// dispatch layout and release global reload differ. Cleanup block/item/both cursors
// 104/104/104; child-boundary cursor 99; indexed child/range 106; shared child base
// 133, with range 153; collection rejection 116; read-only range 99; range-array bound
// 92; child-array bound 139; read-only range bound 92; repaired-entry completion,
// release and both output names 92/92/92.
// Completion/information/both output lifetimes 105/99/110; cleanup block/item cursors
// in the boundary-array context 92/92; actual dispatch block range 103; nested error
// scope and early error/total results 92/92.
// count name 104; release-output name 104; both names 104; result-first 104;
// arrays swapped 104; collection cursor 105; indexed dispatch 104; allocator capture 115.
// Error identity 92; total before error 92; error-first gate 98;
// cleanup null exit 92; proven nonempty count 92; cleanup ordinal names 92;
// unsigned fixed collection 93; exact boundary-end test 92.
// produced after arrays 105; produced after counters 105; information after arrays 99; both after
// arrays 110; collection boundary cursor 148; shared boundary cursor 148; paired collection cursors
// 132; boundary lifetime 92. Scalar output homes, dispatch ownership and release observations
// remain.
// Concrete collection/dispatch class and allocation-pointer combinations 92 each; shared typed table 150;
// common child-interface collection/dispatch/both/both with allocation pointer 92 each.
// Round 186: Native stream contract alone/allocation pointer 85/85; iteration output/both 96/96; native contract with ordinal dispatch alone/allocation 115/115, with output/both 125/125. All declarations restored.
// Existing shared release helper with default/force-inline linkage and local/direct-field input 102 each; real starts initialized on those four forms 151 each.
// Per-child counts with ordinal/direct child, ordinal/child cursor, count-table end and child-table end
// 159/159/142/181; aggregate-zero equivalents 183/183/148/206. Ordinal forms recover frame 0x8020
// but lengthen dispatch; original cumulative boundaries and allocator code restored.
// @retail 0x97d80
long c_aggregate_450cf4::function_97d80(dword *source, s_request *request)
{
	s_block blocks[256];
	long starts[4];
	long error = 0;
	long total = 0;

	for (long i = 0; i < 3; i++)
	{
		c_child *child = ((c_child **)children)[i];
		starts[i] = total;
		if (child != 0 && error == 0)
		{
			long produced = 0;
			error = child->v5(*source, request, 256 - total, &blocks[total], &produced);
			total += produced;
		}
	}
	starts[3] = total;

	if (error == 0 && request->type != 4)
	{
		c_child **child = (c_child **)children;
		for (long *range = starts; range < starts + 3; range++)
		{
			c_child *object = *child;
			for (long j = range[0]; j < range[1]; j++)
				object->v6(&blocks[j]);
			child++;
		}
	}

	for (long k = 0; k < total; k++)
	{
		s_block *block = &blocks[k];
		for (long j = 0; j < block->count; j++)
		{
			if (block->items[j].object != 0)
			{
				c_child *object = block->items[j].object;
				s_allocator_globals *globals = g_4d87f8;
				long result;
				if (!globals->allocator->get_info(object, &result))
					result = NONE;
				globals->allocator->release(object, NONE);
				if (object != 0)
					globals->count--;
				block->items[j].object = 0;
			}
		}
	}

	return error;
}

// @retail 0x97f00
void c_aggregate::v7(void *a)
{
	for (long i = 0; i < 3; i++)
	{
		c_child *child = children[i];
		if (child != 0)
			child->v7(a);
	}
}

// @retail 0x97f30
void c_aggregate::v8(long a, long b)
{
	long local_0 = *(volatile long *)&a;
	for (long i = 0; i < 3; i++)
	{
		c_child *child = children[i];
		if (child != 0)
			child->v8(local_0, b);
	}
}

/* ---- class B: a handle table ---- */
// @retail 0x97f60
void function_97f60(s_handle_peers *t, long a, c_handle_table_450cd0 *self)
{
	self->table = t;
	long local_0 = 1 << a;
	self->head = 0;
	self->shift = a;
	self->bit = local_0;
	self->node = 0;
	for (long i = 0; i < 1024; i++)
	{
		self->entries[i].handle = NONE;
		self->entries[i].unknown04 = 0;
		self->entries[i].state = 0;
		self->entries[i].unknown0a = NONE;
		self->entries[i].unknown0c = 0;
		self->entries[i].unknown10 = 0;
	}
	self->unknown5024 = NONE;
	self->unknown502c = 0;
	self->unknown5028 = 0;
	self->unknown503c = 0;
	self->unknown5038 = 0;
	self->unknown5034 = 0;
	self->unknown5030 = 0;
	self->unknown09 = 0;
	self->unknown0a = 0;
	self->unknown08 = 1;
}

// @retail 0x97fe0
void c_handle_table_450cd0::function_97fe0()
{
	while (head != 0)
		v8(head->handle, false);

	long local_0 = 0;
	s_handle_entry *entry = entries;
	for (long i = 0; i < 1024; i++, entry++)
	{
		if (entry->state != local_0)
		{
			long handle = entry->handle;
			s_handle_peer *item = &table->peers[handle & 0x3ff];
			word mask = item->mask;
			if (mask & (1 << shift))
			{
				item->mask = mask & ~(1 << shift);
				s_handle_peers *t = table;
				if (t->peers[handle & 0x3ff].mask == 0)
				{
					t->owner->v12(handle);
					byte *local_1 = &t->peers[handle & 0x3ff].flags;
					*local_1 &= 0xfe;
				}
			}
			entry->state = (short)local_0;
			entry->handle = NONE;
			entry->unknown0a = NONE;
			entry->unknown10 = (word)local_0;
		}
	}

	unknown502c = 0;
	unknown5028 = 0;
	unknown5034 = 0;
	unknown5030 = 0;
	unknown5024 = NONE;
}

// @retail 0x980d0
void c_handle_table_450cd0::function_980d0(long handle, dword mask)
{
	long index = handle & 0x3ff;
	s_handle_entry *entry = &entries[index];
	if (entry->state == 3)
	{
		if (!((1 << shift) & table->peers[index].mask) && entry->unknown04 == 0)
			unknown5034++;
	}
	entry->unknown04 |= mask;
}

// @retail 0x98120
bool c_handle_table_450cd0::function_98120()
{
	bool result = false;
	if (unknown09)
		result = (unknown503c != 0) | (unknown5034 != 0) | (unknown502c != 0);
	return result;
}
