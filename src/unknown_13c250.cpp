// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"

struct s_effect_owner;
void function_b7930(void *arg_1, long arg_2, long arg_3, s_effect_owner const *arg_4);
long __stdcall function_b7b40(void *arg_1);
void __stdcall function_b8ee0(long arg_1, long arg_2, long arg_3, long arg_4);
void function_b7680(long arg_1, real arg_2, real arg_3);
long function_cbe60(long arg_1);

struct s_13c250
{
    byte field_0[3];
    byte field_3;
    byte field_4[4];
    byte *field_8;
};

struct s_13c251
{
    byte field_0[0x10a];
    word field_10a0 : 1;
    word field_10a1 : 1;
    word field_10a2 : 1;
    word field_10a3 : 13;
};

__forceinline byte *function_13c251(s_record_pool *arg_1, long arg_2)
{
    return ((s_13c250 *)arg_1->data)[arg_2 & 0xffff].field_8;
}

static inline long local_player_first_index(void)
{
    long result = NONE;

    for (long i = 0; i < 4; i++)
    {
        if (g_4e8c20->entries[i] != NONE)
        {
            result = i;
            break;
        }
    }
    return result;
}

static inline long local_player_next(long index)
{
    long result = NONE;
    long start = index == NONE ? 0 : index + 1;

    for (long i = start; i < 4; i++)
    {
        if (g_4e8c20->entries[i] != NONE)
        {
            result = i;
            break;
        }
    }
    return result;
}

static inline bool local_player_valid(long local_player_index)
{
    bool result = false;
    if (local_player_index != NONE)
        result = g_4e8c20->entries[local_player_index] != NONE;
    return result;
}

struct s_player_13c250
{
    byte unknown000[0x2c];
    long unit_index;
    byte unknown030[0x21c - 0x30];
};

__forceinline bool function_13c253(s_13c251 const *arg_1)
{
    return TEST_FIELD_BIT(arg_1->field_10a2);
}
// score 144 (was 144): frame 0xd8 agrees; carried-item receiver reloads, append-count observations and ranking handoff remain.

// score 144 (was 144): carried-payload reloads, memory count and ranking handoff remain.
// Earlier: receiver/count reloads and ranking dispatch remain; retail reads the
// count once per append (no volatile re-read) yet keeps it in memory, and holds the found
// candidate apart from the best-ranked one (0x5000720 is a switch case there).
// Typed fallback entries 171; end cursor 173; signed-word count 168; byte count 195; carried tag
// read before count update 171; native carried view 171; read-only view 171; dword workspace 171;
// unsigned tags 171; selected position 182; unsigned ranks 172; word rank 172; unsigned ranking
// position 171; pointer ranking 144 (kept); append cursor 273; cursor/count snapshot 267;
// plain count/cursor 232; output/ranking cursors 294; completed cursor 303; rank-one first 171;
// lower-rank first 174; pointer/native case 176; pointer/count snapshot 181; pointer/plain count 281;
// pointer/known first appends 189; pointer/single count reads 188; pointer/both append forms 206;
// pointer/rank predicates 144; pointer/rank interval 144; pointer/unsigned rank 144; pointer/word
// rank 178; cursor read-step/end equality/for/preferred break 144/144/144/144; pointer default
// rank 189; rank-three first 144; empty selection exit 144; read-step with default rank 190;
// consuming qualified/plain count 179/212; separate remaining count 174; candidate-distance bound
// with qualified/plain count 178/243; consuming count with native preferred case 182/211;
// post-decrement remaining count 174; getter pool reference / typed entry input / handle-first /
// both contracts 144f; first/second conditional handle selection 193/193f; both with pool
// reference 207f; declared payload / read-only result / read-only pool / handle domain
// 144/144/144/144f; native payload slots 176f; distinct typed receivers / priority enum /
// payload with enum 144/144/144f.
// Found candidate apart from the best, its tag a switch case 186; index ranking loop 171;
// plain count 281; found plus index 179; found, index and plain count 205; append helper taking
// the count's address 286; with the index loop 258.
// Shared first / next / validity / first-next / first-valid / next-valid / all boundaries
// 144 each; all shared boundaries with an ordinary count 281.
// Round 412: candidate/class/both observations at each use 144/144/144; actual selected-candidate pointer alone/with candidate reads/with class reads/with both 183 each; complete pointer graph with unsigned ranks 183. Original body, count and every call restored.
// Round 415: complete carried/scenario/tag receiver observations, alone and all combinations, 144
// each; completed count on the complete receiver graph 181 at 831 bytes. Original body restored.
// @retail 0x13c250
void __stdcall function_13c250(long arg_1, long arg_2, long arg_3)
{
    volatile long local_2;
    long local_3[4];
    byte local_4[0xc4];
    if (arg_1 != NONE)
    {
        local_2 = 0;
        long local_6 = local_player_first_index();
        for (; local_6 != NONE; local_6 = local_player_next(local_6))
        {
            if (local_player_valid(local_6))
            {
                long local_8 = ((s_player_13c250 *)g_4e8c24->data)[g_4e8c20->entries[local_6] & 0xffff].unit_index;
                if (local_8 != NONE && !function_13c253((s_13c251 *)function_13c251(g_4e0300, local_8)))
                {
                    byte *local_9 = function_13c251(g_4e0300, local_8);
                    short local_10 = (signed char)local_9[0x212];
                    long local_11 = NONE;
                    if (local_10 != NONE)
                        local_11 = *(long *)(local_9 + local_10 * 4 + 0x218);
                    local_9 = function_13c251(g_4e0300, local_8);
                    short local_12 = (signed char)local_9[0x213];
                    long local_13 = NONE;
                    if (local_12 != NONE)
                        local_13 = *(long *)(local_9 + local_12 * 4 + 0x218);
                    long local_14 = function_cbe60(local_8);
                    if (local_11 != NONE)
                    {
                        long local_31 = local_2++;
                        local_3[local_31] = *(long *)function_13c251(g_4e0300, local_11);
                    }
                    if (local_13 != NONE)
                    {
                        long local_31 = local_2++;
                        local_3[local_31] = *(long *)function_13c251(g_4e0300, local_13);
                    }
                    if (local_14 != NONE)
                    {
                        long local_31 = local_2++;
                        local_3[local_31] = *(long *)function_13c251(g_4e0300, local_14);
                    }
                    break;
                }
            }
        }
        if (local_2 == 0)
        {
            byte *local_15 = (byte *)g_4e0350;
            if (*(long *)(local_15 + 0xf8) > 0)
            {
                byte *local_16 = *(byte **)(local_15 + 0xfc);
                long local_17 = *(long *)(local_16 + 0x2c);
                if (local_17 != NONE)
                {
                    long local_31 = local_2++;
                    local_3[local_31] = local_17;
                }
                local_17 = *(long *)(local_16 + 0x38);
                if (local_17 != NONE)
                {
                    long local_31 = local_2++;
                    local_3[local_31] = local_17;
                }
            }
            if (local_2 == 0)
            {
                s_13c250 *local_18 = &((s_13c250 *)g_4e0300->data)[arg_1 & 0xffff];
                if (local_18->field_3 == 0)
                {
                    byte *local_19 = (byte *)g_4e3b44[*(long *)local_18->field_8 & 0xffff].bytes;
                    long local_20 = *(long *)(local_19 + 0x1c0);
                    if (local_20 > 0)
                    {
                        byte *local_21 = *(byte **)(local_19 + 0x1c4) + 4;
                        do
                        {
                            long local_22 = *(long *)local_21;
                            if (local_22 != NONE)
                            {
                                long local_31 = local_2++;
                                local_3[local_31] = local_22;
                            }
                            local_21 += 8;
                            --local_20;
                        }
                        while (local_20);
                    }
                }
            }
        }
        long local_23 = NONE;
        long local_24 = 0x7fffffff;
        long const *candidate = local_3;
        while (candidate < local_3 + local_2)
        {
            long local_26 = *candidate;
            long local_27 = *(long *)((byte *)g_4e3b44[local_26 & 0xffff].bytes + 0x288);
            long local_28;
            if (local_27 == 0x5000720)
            {
                local_23 = local_26;
                goto local_29;
            }
            switch (local_27)
            {
            case 0x500000a:
            case 0x5000722:
                local_28 = 2;
                break;
            case 0x6000721:
                local_28 = 1;
                break;
            case 0x700071e:
            case 0x700071f:
                local_28 = 3;
                break;
            default:
                local_28 = 4;
                break;
            }
            if (local_23 == NONE || local_28 < local_24)
            {
                local_24 = local_28;
                local_23 = local_26;
            }
            ++candidate;
        }
local_29:
        if (local_23 != NONE)
        {
            function_b7930(local_4, local_23, arg_1, 0);
            long local_30 = function_b7b40(local_4);
            if (local_30 != NONE)
            {
                function_b8ee0(arg_1, arg_2, local_30, arg_3);
                function_b7680(local_30, 1.0f, 0.0f);
            }
        }
    }
}
