#include "unknown_11c920.h"
#include "globals.h"
#include "object_markers.h"
#include "unknown_11cc90.h"

// @flags /O2 /arch:SSE /Gr

struct s_203780
{
    long field_0;
    point3f field_4;
    short field_10;
    byte field_12[2];
    vector2f field_14;
    byte field_1c[4];
    short field_20;
    byte field_22[0x64 - 0x22];
};

vector2f *function_11df30(vector2f *arg_0, vector3f const *arg_1);
void function_bf380(void);
long __stdcall function_1e0850(long arg_0, long arg_1, s_203780 const *arg_2, bool arg_3);

// score 70 (was 70): retail keeps NONE in esi (or esi,-1 at entry; four
// compares and two exits use it) with arg_0 in ebx and ebp/edi pushed past
// the first guard; ours keeps arg_0 in esi, NONE in eax for the first exit
// only (immediates elsewhere), pushes all four at entry, and shares one fail
// exit (345 vs 369 bytes). Early returns 109 (shrink-wrap appears but NONE
// stays immediate); only the arg_1 guard early 109; local_8 initialized at
// declaration 109; the marker && split into nested ifs 75; marker index as a
// ternary 70; marker-found through a bool 70; result assigned inside the test
// 70; store through an address local 70; global optimization off 210
// (diagnostic). No source form enregisters the NONE constant: VC propagates
// it per web. Round 78: single exit with every guard nested 115 (arg_1 then
// arrives in dx), with `if (local_0 == NONE) return NONE;` kept 100, plus an
// early `return NONE` for arg_1 123; the squad-index product named first 70;
// the actor store under `if (local_0 != NONE)` 71.
// the first guard; ours keeps arg_0 in esi, NONE as immediates, pushes all
// four at entry, and shares one fail exit (345 vs 369 bytes). Early returns
// 109 (shrink-wrap appears but NONE stays immediate); only the arg_1 guard
// early 109; local_8 initialized at declaration 109; the marker && split
// into nested ifs 75; marker index as a ternary 70; marker-found through a
// bool 70; result assigned inside the test 70; store through an address
// local 70; global optimization off 210 (diagnostic). No source form
// enregisters the NONE constant: VC propagates it per web.
// Lane 11 r12 (aligned counts, base 44): the cell pointer as a typed element
// address or index-first sum 44 (identical code); the guards nested to one exit
// with the call result in its own local, or `local_0 = call; if (local_0 !=
// NONE) store` 102/97 (arg_1 moves to dx); the call result in its own local
// copied to local_0 on success, early returns kept 93 (NONE in ebx, local_0
// spilled, frame 0xdc).
// @retail 0x203780
long function_203780(long arg_0, short arg_1, short arg_2, bool arg_3)
{
    long local_0 = NONE;
    if (arg_1 != NONE)
    {
        byte *local_3 = *(byte **)((byte *)g_4e0350 + 0x164) + (word)arg_2 * 0x74;
        s_203780 *local_4 = (s_203780 *)(*(byte **)(local_3 + 0x4c) + arg_1 * 0x64);
        word local_5 = local_4->field_20;
        short local_10;
        if ((short)local_5 != NONE)
            local_10 = (short)local_5;
        else
            local_10 = *(short *)(local_3 + 0x36);
        if (local_10 < 0 || local_10 >= *(long *)((byte *)g_4e0350 + 0x178))
            return local_0;
        long *local_6 = (long *)(*(byte **)((byte *)g_4e0350 + 0x17c) + local_10 * 8);
        if (local_6[1] == NONE)
            return local_0;
        s_203780 local_7;
        s_203780 const *local_8;
        s_object_marker local_9;
        if (arg_0 != NONE && function_b8d30(arg_0, 0x700061d, &local_9, 1, false) > 0)
        {
            local_7 = *local_4;
            local_7.field_4 = local_9.matrix.position;
            local_7.field_10 = NONE;
            function_11df30(&local_7.field_14, &local_9.matrix.forward);
            local_8 = &local_7;
        }
        else
            local_8 = local_4;
        if (arg_3)
            function_bf380();
        local_0 = function_1e0850(local_6[1], arg_2, local_8, true);
        if (local_0 == NONE)
            return NONE;
        *(long *)(g_4f55f0->data + (local_0 & 0xffff) * 0x888 + 0x38) = local_4->field_0;
    }
    return local_0;
}
