#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include "event_definitions.h"
#include <string.h>

// @flags /O2 /arch:SSE /Gr

__declspec(noinline) void *function_122c10(long arg_0, long arg_1);
void function_db760(long arg_0, long arg_1, long arg_2, long arg_3);
struct s_18444a;
void __stdcall function_184440(long arg_0, long arg_1, s_18444a const *arg_2, long arg_3);
extern byte *g_4ed280;

static __forceinline long function_9c912(long arg_0, long arg_1, long arg_2)
{
    long local_0;
    if (arg_0 < arg_1)
        local_0 = arg_1;
    else if (arg_0 > arg_2)
        local_0 = arg_2;
    else
        local_0 = arg_0;
    return local_0;
}

// score 29 (was 57): left are the frame (sub esp, 8: the object index gets its own slot, where
// retail keeps a 4-byte frame and the object index in arg_1's slot and the permutation in arg_3's)
// and the call setup, BLOCKED-BY 0xdb760 (contributor; retail takes the permutation in ebx and the
// object on the stack, ours the object in ebx). Kept (round 78): the clamp as if/else-if/else
// (retail's unthreaded `cmp; mov ecx, 1; jg; mov ecx, esi`), the mode read at each use, the
// permutation read into its local before the parameter store (no `lea edi, [esi + 4]`: table in
// esi, event in edi as retail) and no false assignments (result reloaded from its slot).
// Earlier: clamp of a re-read mode 84 (310 bytes, retail's unthreaded clamp); with a plain
// permutation local 110; no false assignments 107 (result only on the stack), inner one removed
// 58, outer one removed 68; the member's fields copied in the other order (store of the second
// field before the comparison in ours, after it in retail). Round 78: event pointer declared at
// the top 82; the clamp helper plain static/extern 57/57; a named tag table local 57; the
// permutation re-read at each use 57; the original parameter order with the new clamp/reads 86,
// without false assignments 84; a plain permutation local with them 109; the parameter store at
// the call 120, inside the range test 29, the range test on the parameter 84; locals renamed 31;
// result declared after the object index 51.
// retail keeps the definition table in esi and the event pointer in edi,
// ours the other way round; the final call setup is BLOCKED-BY 0xdb760 (contributor-owned),
// which takes the permutation index in ebx and the object on the stack in retail.
// (retail esi/edi); the clamp's equality test is threaded away; the call setup
// is BLOCKED-BY 0xdb760 (contributor; retail takes the permutation in ebx and
// the object on the stack, ours the object in ebx). Clamp of a re-read mode 84
// (310 bytes, retail's unthreaded clamp); with a plain permutation local 110;
// no false assignments 107 (result only on the stack), inner one removed 58,
// outer one removed 68.
// Earlier: the member's fields are copied in the other order and the store of
// the second field sits before the comparison in ours and after it in retail
// Lane 11 r12, aligned counts (base 48 at 308 bytes): a plain permutation local
// instead of the reused parameter 74 (280 bytes; the object index then stays in
// ebx, as 0xdb760 takes it), with no false assignments 70; no false assignments
// alone 59; PIN(mode, 0, 1) for the clamp 66 (301 bytes), with no false
// assignments 75.
// @retail 0x9c910
bool c_damage_section_response_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    bool result = false;
    long object_index = function_a58d0(arg_1[0]);
    if (object_index != NONE)
    {
        byte *object = (byte *)((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
        byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
        long damage_index = *(long *)(definition + 0x38);
        if (damage_index != NONE)
        {
            byte *region_flags = object + *(short *)(object + 0x122);
            long const *event = (long const *)arg_3;
            long region_index = event[0];
            if (region_index >= 0 && region_index < (long)((dword)(*(short *)(object + 0x120)) / 8))
            {
                byte *damage = g_4e3b44[damage_index & 0xffff].bytes;
                if (*(long *)(damage + 0x60) > 0)
                {
                    byte *region = *(byte **)(*(byte **)(damage + 0x64) + 0xc0) + region_index * 0x38;
                    /* the event pointer parameter carries the permutation index from here on */
                    long permutation_index = event[1];
                    arg_3 = (void const *)permutation_index;
                    if (permutation_index >= 0 && permutation_index < *(long *)(region + 0xc))
                    {
                        if (function_9c912(event[2], 0, 1) == event[2])
                        {
                            long permutation_bit = event[1];
                            if (!(*(word *)(region_flags + region_index * 8) & (1 << permutation_bit)))
                            {
                                if (event[2] != 1 || *(long *)(*(byte **)(region + 0x10) + permutation_bit * 0x50 + 0x34) != NONE)
                                {
                                    function_db760(region_index, (long)arg_3, object_index, event[2]);
                                    result = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

struct s_9cca0
{
    long field_0;
    byte field_4[0x18];
    long field_1c;
    long field_20;
    byte field_24[0xc];
    vector3f field_30;
    vector3f field_3c;
    byte field_48[0x34];
    short field_7c;
    byte field_7e[10];
};

// score 9 (was 9): the two vector copies and the global's test are ordered
// differently; retail sinks the k component's store past the global's test and
// loads the tag after it, ours stores the tag before the test
// @retail 0x9cca0
bool c_breakable_surface_damage_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    s_9cca0 local_1;
    byte const *local_0 = (byte const *)arg_3;
    if (function_122c10(0x6a707421, *(long const *)(local_0 + 0x30)))
    {
        memset((byte *)&local_1 + 4, 0, 0x78);
        memset(local_1.field_7e, 0, sizeof(local_1.field_7e));
        local_1.field_1c = *(long const *)(local_0 + 0xc);
        local_1.field_20 = *(long const *)(local_0 + 0x10);
        local_1.field_30 = *(vector3f const *)(local_0 + 0x24);
        local_1.field_3c = *(vector3f const *)(local_0 + 0x18);
        local_1.field_0 = *(long const *)(local_0 + 0x30);
        local_1.field_7c = -1;
        if (*g_4ed280 && local_1.field_0 != NONE)
            function_184440(*(long const *)local_0, *(long const *)(local_0 + 4), (s_18444a const *)&local_1, *(long const *)(local_0 + 8));
    }
    return false;
}

struct s_9f880
{
    long field_0;
    long field_4;
    long field_8;
    long field_c;
    short field_10;
    byte field_12[2];
    real field_14;
    byte field_18;
};
struct s_9f881
{
    byte field_0[0x10a];
    struct s_9f882
    {
        word field_0 : 1;
        word field_1 : 1;
        word field_2 : 1;
        word field_3 : 13;
    } field_10a;
};

struct s_unit_melee_hit;
void __stdcall function_cfc90(long arg_0, long arg_1, s_unit_melee_hit const *arg_2);
bool function_183f50(long arg_0, long arg_1, long arg_2);
extern short g_47d8e0;

// @retail 0x9f880
bool c_unit_melee_damage_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    long local_0 = function_a58d0(arg_1[0]);
    long local_1 = function_a58d0(arg_1[1]);
    byte const *local_2 = (byte const *)arg_3;
    long local_3 = *(long const *)(local_2 + 4);
    long local_4 = *(long const *)(local_2 + 8);
    long local_5 = *(long const *)(local_2 + 0xc);
    if (local_3 != NONE && !function_183f50(local_4, local_5, local_3))
    {
        local_3 = NONE;
        local_4 = NONE;
        local_5 = NONE;
    }
    byte local_6 = local_2[0x18];
    if ((local_6 & 0x3f) >= 0x2a)
        local_6 &= 0xc0;
    struct s_9f883 { signed short field_0 : 16; };
    s_9f883 local_7;
    local_7.field_0 = *(short const *)(local_2 + 0x10);
    byte *local_8 = 0;
    if (local_7.field_0 != -1 && local_7.field_0 >= 0 && local_7.field_0 < *(long *)((byte *)g_4e034c + 0x150))
        local_8 = *(byte **)((byte *)g_4e034c + 0x154) + local_7.field_0 * 0xb4;
    if (!local_8)
        local_7.field_0 = g_47d8e0;
    if (local_0 != NONE)
    {
        byte *local_9 = (byte *)((s_object_header *)g_4e0300->data)[local_0 & 0xffff].object;
        if (local_9[0xaa] == 0 && !TEST_FIELD_BIT(((s_9f881 *)local_9)->field_10a.field_2) &&
            (local_1 != NONE || local_3 != NONE) && function_122c10(0x6a707421, *(long const *)local_2))
        {
            s_9f880 local_10;
            local_10.field_0 = local_1;
            local_10.field_4 = local_3;
            local_10.field_8 = local_4;
            local_10.field_c = local_5;
            local_10.field_10 = local_7.field_0;
            local_10.field_14 = *(real const *)(local_2 + 0x14);
            local_10.field_18 = local_6;
            function_cfc90(local_0, *(long const *)local_2, (s_unit_melee_hit const *)&local_10);
        }
    }
    return false;
}

struct s_damage_report;
point3f *function_b9dd0(long arg_0, point3f *arg_1);
void function_d6a70(long arg_0);
void function_d9640(s_damage_report const *arg_0, long arg_1);
void __stdcall function_e4a20(point3f const *arg_0, long arg_1, long arg_2, bool arg_3);

struct s_9c610
{
    byte field_0;
    byte field_1[3];
    dword field_4;
    long field_8;
    long field_c;
    long field_10;
    short field_14;
    byte field_16[2];
    vector3f field_18;
    point3f field_24;
    real field_30;
    real field_34;
    real field_38;
    long field_3c;
    short field_40;
    byte field_42[2];
    real field_44;
    real field_48;
    real field_4c;
    dword field_50;
};

struct s_input_9c610
{
    long field_0;
    byte field_4[4];
    short field_8;
    byte field_a;
    byte field_b;
    vector3f field_c;
    real field_18;
    real field_1c;
    dword field_20;
    real field_24;
    real field_28;
    short field_2c;
    short field_2e;
    dword field_30;
    byte field_34;
};

// score 23 (was 31): only the call to function_e4a20 at the end differs (and
// the branch offsets past it): retail passes the point in eax and the object in
// ecx, ours the point in ecx and the object on the stack, which follows from
// function_e4a20's own body (BLOCKED-BY 0xe4a20). Tried: the flags store first
// of the second copy group 23 (kept); before the vector copy 67; first of all
// 67; before field_3c 27.
// Shared absolute lookup inline 23, out of line 122; shared index
// reconstruction 107; both shared inline operations 114; point local at
// notification 23, before guard 23, across dispatch 23; shared lookup
// with point lifetime 23. Native frame/report copies still agree.
// final validity predicate 129; byte success result 23; unsigned reconstructed handle 23;
// signed-byte material 46; bool presence field 23; named validated tag result 23; source-width
// pool index 112; unsigned salt word 24.
// Byte notification read 46; byte tail read 23; both byte reads 46; word notification read 46.
// Word tail read 23; both word reads 46; signed-byte tail read 23; byte notification/signed tail 46.
// score 13 (was 23): flag 7 of +0x20 as TEST_FIELD_BIT((x >> 7) & 1) 13 (retail tests it as a
// signed byte, mov al / test al, al / jns; a char < 0 test, a byte mask or a byte shift 23; flag 1
// rewritten the same way 56, both 63).
// @retail 0x9c610
bool c_damage_aftermath_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    bool local_0 = false;
    long local_1 = function_a58d0(arg_1[0]);
    long local_2 = function_a58d0(arg_1[1]);
    if (local_1 != NONE)
    {
        s_input_9c610 const *input = (s_input_9c610 const *)arg_3;
        long local_4 = NONE;
        long local_5 = NONE;
        short local_6 = input->field_8;
        if (local_6 != -1)
        {
            long local_7 = local_6;
            if (local_7 != NONE && local_7 >= 0 && local_7 < g_4e8c24->high_water_index)
            {
                byte *local_8 = g_4e8c24->data + local_7 * g_4e8c24->size;
                short local_9 = *(short *)local_8;
                if (local_9)
                {
                    local_4 = ((long)local_9 << 16) | local_7;
                    local_5 = *(char *)(local_8 + 0xc0);
                }
            }
        }
        if (function_122c10(0x6a707421, input->field_0))
        {
            short local_11 = (short)local_5;
            s_9c610 local_10;
            memset(&local_10, 0, sizeof(local_10));
            local_10.field_10 = local_2;
            local_10.field_c = local_4;
            local_10.field_14 = local_11;
            local_10.field_8 = input->field_0;
            if (input->field_a)
                local_10.field_18 = input->field_c;
            local_10.field_4 = input->field_20;
            local_10.field_34 = input->field_18;
            local_10.field_38 = input->field_1c;
            local_10.field_50 = input->field_30;
            local_10.field_3c = input->field_2c;
            local_10.field_44 = input->field_28;
            local_10.field_40 = input->field_2e;
            local_10.field_48 = input->field_24;
            local_10.field_0 = input->field_34;
            local_10.field_30 = 0.0f;
            function_b9dd0(local_1, &local_10.field_24);
            local_10.field_4c = 0.0f;
            if (input->field_20 & 2)
                function_d6a70(local_1);
            function_d9640((s_damage_report const *)&local_10, local_1);
            if (g_4e0300->data[(local_1 & 0xffff) * 12 + 3] == 0 && TEST_FIELD_BIT((input->field_20 >> 7) & 1))
                function_e4a20((point3f const *)&input->field_c, input->field_2e, local_1, false);
        }
        local_0 = true;
    }
    return local_0;
}

struct s_object;
s_object *function_badc0(long arg_0, dword arg_1);
void __stdcall function_191fab(long arg_0, long arg_1);
void __stdcall function_191e51(long arg_0, long arg_1);
void __stdcall function_191ec4(long arg_0, long arg_1, short arg_2);
void function_f8160(long arg_0);
long function_1896c0(real arg_0, long arg_1);
void player_speed_request_14ea20(long arg_0);
void player_speed_request_14eb10(long arg_0);
void player_speed_request_14ec00(long arg_0);
void teleporter_display_blocked_message(long arg_0);

// @retail 0x9ffa0
bool c_unit_pickup_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    long local_0 = function_a58d0(arg_1[0]);
    if (local_0 != NONE)
    {
        byte *local_1 = (byte *)function_badc0(local_0, 3);
        if (local_1 && *(long *)(local_1 + 0x13c) != NONE)
        {
            local_1 = (byte *)function_badc0(local_0, 3);
            long local_2 = NONE;
            if (local_1)
                local_2 = *(long *)(local_1 + 0x13c);
            long local_3 = *(short *)(g_4e8c24->data + (local_2 & 0xffff) * 0x21c + 0x28);
            if (local_3 != NONE)
            {
                byte const *local_4 = (byte const *)arg_3;
                                if (*(short const *)local_4 == 4 || (*(long const *)(local_4 + 4) != NONE && function_122c10(0x6974656d, *(long const *)(local_4 + 4))))
                {
                    switch (*(short const *)local_4)
                    {
                    case 0:
                        function_191fab(*(long const *)(local_4 + 4), local_3);
                        break;
                    case 2:
                        function_191e51(*(long const *)(local_4 + 4), local_3);
                        function_f8160(*(long const *)(local_4 + 4));
                        break;
                    case 1:
                    {
                        byte *local_7 = g_4e3b44[*(long const *)(local_4 + 4) & 0xffff].bytes;
                        if ((1 << local_7[0]) & 4)
                        {
                            long local_8 = *(long *)(local_7 + 0x268);
                            if (local_8 != NONE)
                                function_1896c0(1.0f, local_8);
                        }
                        else
                            function_f8160(*(long const *)(local_4 + 4));
                        function_191ec4(*(long const *)(local_4 + 4), local_3, *(short const *)(local_4 + 8));
                        break;
                    }
                    case 3:
                    {
                        byte *local_9 = g_4e3b44[*(long const *)(local_4 + 4) & 0xffff].bytes;
                        short local_10 = *(short *)(local_9 + 0x12c);
                        byte *local_11 = (byte *)((s_object_header *)g_4e0300->data)[local_0 & 0xffff].object;
                        long local_12 = *(long *)(local_11 + 0x13c);
                        if (local_10 == 2)
                            player_speed_request_14ea20(local_12);
                        else if (local_10 == 5)
                            player_speed_request_14ec00(local_12);
                        else if (local_10 == 3)
                            player_speed_request_14eb10(local_12);
                        function_f8160(*(long const *)(local_4 + 4));
                        function_191fab(*(long const *)(local_4 + 4), local_3);
                        break;
                    }
                    case 4:
                    {
                        byte *local_13 = (byte *)((s_object_header *)g_4e0300->data)[local_0 & 0xffff].object;
                        long local_14 = *(long *)(local_13 + 0x13c);
                        if (local_14 != NONE)
                            teleporter_display_blocked_message(local_14);
                        break;
                    }
                    }
                }
            }
        }
    }
    return false;
}
