// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_199740.CPP: unpacking a compressed data block (0x199740); its own
   file because retail compares floats with fcomi here (/arch:SSE) */

#include "unknown_11c920.h"
#include "physical_memory.h"
#include "loop_allocator.h"
#include <xtl.h>

__declspec(noinline) dword function_199560(void *data, dword size);

extern s_physical_object *g_4e6464;
long __stdcall function_12d2f0(long size, long user_data, long update, long release);
void function_12c600(void);
void function_12d520(long address);
double timing_ticks_to_seconds(__int64 ticks);
void *__stdcall loop_allocator_zalloc(void *opaque, long count, long size);
void __stdcall loop_allocator_zfree(void *opaque, void *address);
typedef void *(__stdcall *block_allocate)(void *, long, long);
typedef void (__stdcall *block_free)(void *, void *);
long __stdcall function_2cb5b0(byte *destination, long *destination_size, byte const *source, long source_size, block_allocate allocate, block_free release, void *opaque);

static __int64 read_tsc(void)
{
    volatile __int64 t = 0;
    __asm rdtsc
}

static __forceinline s_loop_allocator *compression_pool_allocate(long size)
{
    s_loop_allocator *result = NULL;
    __int64 start = read_tsc();
    if (g_4e6464->page_count > 0)
    {
        long retries = 0;
        do
        {
            result = (s_loop_allocator *)function_12d2f0(size, 0, 0, 0);
            if (!result)
            {
                if (retries < 90)
                {
                    retries++;
                    function_12c600();
                }
                else
                {
                    __int64 elapsed = read_tsc() - start;
                    if (elapsed < 0)
                        elapsed = 0;
                    if (!((real)timing_ticks_to_seconds(elapsed) < 1.0f))
                        break;
                    D3DDevice_KickPushBuffer();
                    D3DDevice_IsBusy();
                    SwitchToThread();
                }
            }
        } while (!result);
    }
    return result;
}

// score 52 (was 52): BLOCKED-BY 0x2cb5b0: retail passes it source in eax, source size in ecx,
// destination in edx and the destination size in edi (three pushes); its stub is built without
// /GL, so ours pushes all seven, and the argument loads after the pool setup differ. Retail also
// keeps the low half of the start time in ebp and the high half in edi (ours the other way round)
// and stores the timer zero after the edi save. A real 0x2cb5b0 body (an inflate wrapper over
// stubbed 0x2cc8d0/0x2cc9d0/0x2cc890) takes eax/ecx/edx but the size in esi: 64 (323 bytes);
// stdcall or cdecl stubs 64; retail's field store order 66; one-exit result 64. Retail's esi comes
// from 0x2cc8d0 taking its stream in esi, so its real body (and its library callees 0x2cc850,
// 0x2cc890, 0x2ce950) is needed first. Round 158: unsigned start 52; start read before the result
// local 52; elapsed as a separate subtraction 52; plain-inline pool helper 301 (not inlined);
// retry counter declared before the test 52.
// retail keeps the low half of the size in ebp and the high
// half in edi, ours keeps them the other way round, and retail saves the third
// argument after the push, so the stores and the subtraction differ.
// BLOCKED-BY 0x2cb5b0: retail passes it source in eax, source size in
// ecx, destination in edx and the destination size in edi (three pushes); its stub is built
// without /GL, so ours pushes all seven. A real 0x2cb5b0 body (an inflate wrapper over
// stdcall or cdecl stubs 64; retail's field store order 66; one-exit result 64. Retail's esi
// comes from 0x2cc8d0 taking its stream in esi, so its real body is needed first.
// Native unpack status at entry/after allocation 130/102; raw storage/unsigned bytes/both
// 52/52/52; entry status with raw storage/unsigned bytes 130/130; later status with both 102.
// @retail 0x199740
bool __stdcall function_199740(byte *buffer, long size, byte *destination, long *decompressed_size)
{
    bool result = false;
    s_loop_allocator *pool = compression_pool_allocate(0x10000);
    if (pool)
    {
        function_18e250(pool, 0xffb0, "zlib pool", NULL);
        pool->field3c = true;
        pool->field3d = true;
        s_loop_allocator *allocator = pool;
        *decompressed_size = function_199560(buffer, size);
        if (function_2cb5b0(destination, decompressed_size, buffer + 4, size, loop_allocator_zalloc, loop_allocator_zfree, &allocator) == 0)
            result = true;
        function_12d520((long)pool);
    }
    return result;
}
