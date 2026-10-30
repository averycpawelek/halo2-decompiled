#include "unknown_440dd8.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0820f0.h"
// @flags /O2 /Gr
#include "counters_storage.h"

struct s_structure_bsp_reference;
struct s_bsp3d;
struct s_slot_entry_list;
extern s_bsp3d *g_4e033c;
extern s_slot_entry_list *g_4e0340;
extern s_connection_counter g_4e6398;

void function_d4e60(short arg_0);
void function_137da0();
void function_137d40();
bool cache_files_load_structure_bsp(s_structure_bsp_reference *arg_0);
void cache_files_unload_structure_bsp(s_structure_bsp_reference *arg_0);
void function_16e2c0(long arg_0);

struct s_11c1b0
{
    void (__stdcall *field_0)(short);
    dword field_4[8];
};

#define g_440df0 (*( s_11c1b0 const (*)[68])((byte const *)g_440dd8 + 0x18))

// score 50 (was 50): Initial index/zero registers and saved-index publication remain.
// Earlier allocation: retail keeps local_0 in ebx (xor ebx, ebx; mov bx) up to the
// Initial index/zero registers and early saved-index store still differ; frame and both result lifetimes agree.
// On the separate-result form: signed ordinal 125; ordinal != 68 51; row cursor 125;
// counted row cursor 95; callback snapshot 50; signed snapshot 125; row snapshot 125;
// counted row snapshot 95. Original dispatch, two index values and returns retained.
// retail keeps local_0 in ebx (xor ebx, ebx; mov bx) up to the
// initial index DI/zero EBX differ from retail BX/EDI;
// saved-index publication still follows validation instead of preceding it.
// Earlier: retail keeps local_0 in ebx (xor ebx, ebx; mov bx) up to the
// function_137da0 call and stores it at once into arg_0's slot, which then serves as
// local_7; it holds the early false in al (so the limit goes in esi and every push
// comes first) and only later the result in bl, with edi as its zero register. Ours
// keeps local_1 in bl from the start, local_0 in di and zero in edx. Tried: local_0
// and local_7 as one variable 116 (edi becomes the zero register); local_7 = local_0
// at its declaration 143; with one variable, return false for the range test (local_1
// with or without its initializer, declared first or second) 131 (ebx gets retail's
// xor/mov, but the early return gets its own epilogue); return local_1 there 114
// (draft) / 131 (one variable); the body inside if (arg_0 == NONE || in range) 99
// (draft) / 116 (one variable).
// Earlier allocation: result false from entry (al) and the success in bl now match
// result false from entry (al) and the success in bl now match
// Earlier: result false from entry (al) and the success in bl now match
// retail, and local_7 shares arg_0's slot, but the zero goes in ebx and local_0 in edi
// (retail: local_0 ebx, zero edi), and local_7's store comes after the range test
// (retail: at entry). One variable with result + local_1 128 (retail's registers, but
// its own slot: push ecx); local_7 = local_0 after the loop 87; local_0 read after the
// range test 58; result declared first 120 (100 one variable); local_1 declared at the
// block top 50; local_7 after the true test 50; local_7 in the NONE test 50; local_1 &&
// local_7 != arg_0 for the goto 50 (same code); a 600 s permuter run, none below 50.
// Late byte/short/long success carriers 80/110/109; word/byte callback counters 95/95;
// word counter plus byte success 107; equality-produced success 94;
// equality result plus word counter 128.
// Requested value before/after old index 50/50; early saved index after request
// 128, request-first 128, false-before-save 130, false-before-request 130;
// requested input with wide saved/current carriers 125/122. Separate indices/results retained.
// One index variable 128 (every instruction as retail except its own slot: a variable takes the
// argument's home only when defined after the argument's first read); validity bool first, one
// or two variables 130/130, with the false result first 132; local copy of the requested index
// read first 128.
// Round 16 (lane 10): on the one-variable form, arg_0 copied first 128 and the variable renamed 128
// (still its own slot), the body in an inner block 100. Byte-for-byte match (plain check, all seven
// callers unchanged) only when the saved index lives in the parameter variable itself: copy arg_0
// into a requested-index local, assign arg_0 = local_0 at entry and arg_0 = g_4e6948->index after
// function_137da0, and test the requested local everywhere else. Not kept: a reused parameter.
// Round 406: Initial index/zero registers and saved-index publication remain. Consistent unsigned-
// word current/saved/request equality with 0xffff sentinel, parameter-read ordering, saved entry
// lifetime and byte late-success combinations. Scores 75/75/133/133/91/91/137/137. All restored.
// Round 18 (lane 10): an inline helper for the four global clears (both sites) 50; the saved index
// declared at the top and first assigned in the block 50 (still the argument's home: the def, not the
// scope, decides); the copy used in the early true test 50, in both tests 50; the copy re-read from
// the global 50 (all identical code).
// Round 93: the saved index already shares arg_0's slot; left are local_0/zero in di/ebx (retail
// bx with xor ebx, ebx / edi) and the slot store after the range test (retail: before it, as only
// a reassigned parameter gives). Both NULL-clear blocks as one inline helper 50; early return false
// for the range test 122; saved index declared at function scope 50; early true test on the saved
// copy 50; unload argument written in the call 50.
// @retail 0x11c1b0 standard
bool __stdcall function_11c1b0(short arg_0, bool arg_1)
{
    short local_0 = g_4e6948->index;
    bool result = false;
    if (arg_0 == NONE || (arg_0 >= 0 && arg_0 < *(long *)((byte *)g_4e0350 + 0x210)))
    {
        short local_7 = local_0;
        if (g_4686c4 == arg_0 && local_0 == arg_0)
            return true;
        for (dword local_2 = 0; local_2 < 68; local_2++)
        {
            if (g_440df0[local_2].field_0)
                g_440df0[local_2].field_0(arg_0);
        }
        if (local_0 != NONE && local_0 != arg_0)
        {
            function_137da0();
            local_7 = g_4e6948->index;
        }
        if (g_4686c4 != NONE && g_4686c4 != arg_0)
        {
            if (arg_1)
            {
                byte *local_3 = *(byte **)((byte *)g_4e0350 + 0x214) + g_4686c4 * 0x44;
                cache_files_unload_structure_bsp((s_structure_bsp_reference *)local_3);
            }
            g_4686c4 = NONE;
            g_4e0348 = NULL;
            g_4e0344 = NULL;
            g_4e0340 = NULL;
            g_4e033c = NULL;
        }
        bool local_1 = true;
        if (g_4686c4 != arg_0)
        {
            byte *local_4 = *(byte **)((byte *)g_4e0350 + 0x214) + arg_0 * 0x44;
            g_4e0348 = NULL;
            g_4e0344 = NULL;
            g_4e0340 = NULL;
            g_4e033c = NULL;
            g_4686c4 = arg_0;
            if (cache_files_load_structure_bsp((s_structure_bsp_reference *)local_4))
            {
                function_16e2c0(*(long *)(local_4 + 0x14));
                long local_6 = *(long *)(local_4 + 0x1c);
                if (local_6 != NONE)
                    g_4e0344 = (s_structure_bsp_globals *)g_4e3b44[local_6 & 0xffff].bytes;
                else
                    g_4e0344 = NULL;
                g_4e0348 = (s_match_globals *)g_4e3b44[*(long *)(local_4 + 0x14) & 0xffff].bytes;
                g_4e0340 = *(s_slot_entry_list **)((byte *)g_4e0348 + 0x18);
                g_4e033c = (s_bsp3d *)g_4e0340;
            }
            else
            {
                g_4686c4 = NONE;
                local_1 = false;
                goto local_5;
            }
        }
        if (local_7 != arg_0)
            function_137d40();
    local_5:
        g_4e6398 = g_485ab0;
        result = local_1;
    }
    return result;
}
