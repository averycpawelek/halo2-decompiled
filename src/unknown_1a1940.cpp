#include "unknown_11c920.h"
#include "unknown_1a1940.h"
#include <string.h>

// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_ring_buffer
{
	void write_wrapped(long count, long offset, const void *src);
	void read_wrapped(long offset, long count, void *dst);
	long write(long count, const void *src);

	long size;
	byte *data;
	long start;
	long used;
};

// @retail 0x1a1940
void s_ring_buffer::write_wrapped(long count, long offset, const void *src)
{
	long first = size - offset > count ? count : size - offset;
	long local_0 = count - first;
	if (first > 0)
		memcpy(data + offset, src, first);
	if (local_0 > 0)
		memcpy(data, (const byte *)src + first, local_0);
}

// @retail 0x1a19a0
void s_ring_buffer::read_wrapped(long offset, long count, void *dst)
{
	long first = size - offset > count ? count : size - offset;
	long local_0 = count - first;
	if (first > 0)
		memcpy(dst, data + offset, first);
	if (local_0 > 0)
		memcpy((byte *)dst + first, data, local_0);
}

// @retail 0x1a1900
long s_ring_buffer::write(long count, const void *src)
{
	long result = NONE;

	if (used + count <= size)
	{
		result = (start + used) % size;
		if (count > 0)
			write_wrapped(count, result, src);
		used += count;
	}
	return result;
}

class c_page_heap : public c_page_allocator
{
public:
	virtual void free_pages(byte *address, long size, long unused);
	virtual long get_page_size();
	virtual void *allocate_pages(long alignment_mask, long size, long unused);

private:
	byte *base;
	long unknown08;
	long page_count;
	dword bits[1019];
	long *used_pages;
};

// @retail 0x1a1a00
void c_page_heap::free_pages(byte *address, long size, long unused)
{
	long offset = address - base;
	long start = offset / 0x1000;
	long end = (offset + size + 0xfff) / 0x1000;
	for (long page = start; page < end; page++)
		bits[page >> 5] &= ~(1 << (page & 0x1f));
	*used_pages += start - end;
}

// @retail 0x1a1a80
long c_page_heap::get_page_size()
{
	return 0x1000;
}

// score 80 (was 80): the scan is a while loop that advances page and copies it into checked,
// so the two counters advance independently like retail's inc esi; inc ebp. Retail still keeps
// this in ebx, first in the frame and page_count re-read, and ours still shares page + 1.
// One scan variable 103; checked++ in the body 103; checked = ++page as the increment 90;
// the scan as an inline helper with a next-page out parameter 80
// takes ebx, first lives in the frame and page_count is re-read; ours shares page + 1 between
// the two (lea ecx, [esi + 1]). One scan variable 103; checked++ in the body 103;
// checked = ++page as the increment 79; page++; checked = page; in a while loop 79; the scan
// as an inline helper with a next-page out parameter 80; dword checked 80; end written as
// index + pages in the scan 80; free test first in the scan condition 147; free flag cleared in
// an if 134; dword checked with checked++ 103. Every form with two counters spills checked
// (or merges it), never first; page_count is always hoisted and spilled.
// as an inline helper with a next-page out parameter 80. Round 17: VC7.1 keeps two counters only
// when checked is declared before page (`checked++` in the body or a comma increment); declared after,
// any spelling merges them. Ours also caches page_count in a spilled temporary where retail reads
// [this + 0xc] at each test and spills first, so with two counters checked spills too. Tried:
// checked declared first with checked++ 128 (frame 8); no end variable 80; the alignment step split
// into three statements 80; checked assigned before the bit test 171; free_run declared outside the
// outer loop 80. Compile-only screens: do/while with an outer if and a for loop keep the
// page_count temporary.
// @retail 0x1a1a90
void *c_page_heap::allocate_pages(long alignment_mask, long size, long unused)
{
	void *result = NULL;

	switch (alignment_mask)
	{
	case 0xfff: alignment_mask = 0; break;
	case 0x1fff: alignment_mask = 1; break;
	case 0x3fff: alignment_mask = 2; break;
	case 0x7fff: alignment_mask = 3; break;
	case 0xffff: alignment_mask = 4; break;
	default: goto local_0;
	}
	long pages = (size + 0xfff) / 0x1000;
	long step_mask = (1 << (alignment_mask + 12)) - 1;
	long first = (((long)base + step_mask) & ~step_mask) - (long)base;
	first /= 0x1000;
	long index = first;
	long end = index + pages;
	while (end <= page_count)
	{
		bool free_run = true;
		long page = index;
		long checked = index;
		for (; page < end && free_run; page++)
		{
			free_run = !(bits[page >> 5] & (1 << (page & 0x1f)));
			checked = page + 1;
		}
		if (free_run)
			break;
		++index;
		if (index <= checked)
			index = checked;
		long align_mask = (1 << alignment_mask) - 1;
		index = first + ((index - first + align_mask) & ~align_mask);
		end = index + pages;
	}
	if (index + pages <= page_count)
	{
		for (long p = index; p < index + pages; p++)
			bits[p >> 5] |= 1 << (p & 0x1f);
		result = base + (index << 12);
		*used_pages += pages;
	}
local_0:
	return result;
}

// @retail 0x1a1c20 deleting c_page_heap
