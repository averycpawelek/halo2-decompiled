#include "unknown_11c920.h"
#include "ai_actor.h"
#include "unknown_26c380.h"

// @flags /O2 /arch:SSE /Gr

real function_30bf0(vector3f *arg_0);
bool function_1f5110(long arg_0, point2f const *arg_1, real arg_2,
    real arg_3, bool *arg_4, s_path_trace_result *arg_5);

#pragma inline_depth(0)
// score 104 (was 104): frame 0x3c agrees; half/offset XMM allocation and x87/SSE projection scheduling remain.
// Earlier: frame 0x3c agrees; half/offset XMM allocation and x87/SSE projection scheduling differ;
// tried position base 105; origin base 105; displacement base 105; geometry bases 105; unsigned component 104;
// degenerate normal first 104; squared magnitude value 104; rejected direction loop 104.
// the rotated vector's XMM allocation differs - retail
// keeps the two dot products in xmm1 and xmm0 where ours uses xmm5 and xmm3 -
// and the second subtraction and the x87 sum are swapped.
// Earlier: the negated component in its own local keeps 0.0 in xmm7 for the
// > 0.0f test as retail does, but 177 (aligned 63 against 64; all six term orders of the
// projection give the same code); that with the components written out 188; components
// written out k first 193; the flag store before the copy 104; the copy after the loop 104.
// Round 196: uniform original-direction reads 163; retained scalar inputs 197; double square 105;
// live/scalar inputs with double square 164/197; normalized-vector setter on original/live/scalar inputs
// 217/255/243. All restored; half/offset allocation and x87/SSE schedule remain.
// Round 412: complete normalization scale loop/shared scalar scale/direct division/shared double scale 105/177/105/184; with nonpositive projection first 105/184/105/184. Original body and precision restored.
// Round 288: shared affine/difference producers with in-place/distinct output, ordinary/forced
// difference declaration and original/earlier magnitude 262, 251, 262, 251, 262, 234, 262, 234. Original restored.
// @retail 0x1a9760
void __stdcall function_1a9760(long arg_0, s_slot *arg_1)
{
    s_actor_view *local_0 = (s_actor_view *)(g_4f55f0->data + (arg_0 & 0xffff) * sizeof(s_actor_view));
    if (local_0->prop_index != NONE)
    {
        short *local_1 = (short *)function_25d700(local_0->prop_index);
        if (local_1 && *local_1 >= 6)
            local_0->unknown488 = true;
    }
    vector3f local_2 = local_0->unknown37c;
    vector3f local_3;
    local_0->unknown456 = true;
    for (long component = 0; component < 3; ++component)
        local_3.n[component] = (local_0->position.n[component] + local_0->unknown258.n[component] * 0.5f) - local_0->unknown370.n[component];
    real local_4 = (real)sqrt((double)(local_2.j * local_2.j + (local_2.i * local_2.i + local_2.k * local_2.k)));
    if (!((real)fabs((double)local_4) < 0.0001f))
    {
        real local_8 = 1.0f / local_4;
        local_2.i = local_8 * local_2.i;
        local_2.j = local_8 * local_2.j;
        local_2.k = local_2.k * local_8;
    }
    else
        local_4 = 0.0f;
    if (local_4 > 1.5f)
    {
        local_3.i = local_3.i * (0.0f - local_2.j) + local_3.j * local_2.i + local_3.k * local_2.k;
        real local_5 = local_3.i;
        local_3.i = 0.0f - local_2.j;
        local_3.j = local_2.i;
        local_3.k = local_2.k;
        if (local_5 > 0.0f)
            local_2 = local_3;
        else
        {
            local_2.i = local_3.i * -1.0f;
            local_2.j = local_3.j * -1.0f;
        }
        real local_6 = (local_0->position.z - local_0->unknown370.z) * local_3.k +
            (local_0->position.y - local_0->unknown370.y) * local_3.j +
            (local_0->position.x - local_0->unknown370.x) * local_3.i;
        if (local_6 * local_5 < 0.0f)
            local_6 *= -1.0f;
        local_2.k = 0.0f;
        if (function_30bf0(&local_2) == 0.0f)
            local_2 = *g_4687a8;
        local_6 += local_0->unknown36c;
        if (local_6 > 0.0f)
        {
            if (!(local_6 > 1.5f))
                local_6 = 1.5f;
        }
        else if (local_6 > -1.5f)
            local_6 = -1.5f;
        s_path_trace_result local_7;
        if (!function_1f5110(arg_0, (point2f const *)&local_2, local_6, 0.0f, NULL, &local_7))
        {
            local_2.i *= -1.0f;
            local_2.j *= -1.0f;
            local_2.k *= -1.0f;
        }
    }
    else
    {
        local_2 = local_3;
        if (function_30bf0(&local_2) == 0.0f)
            local_2 = *g_4687a8;
    }
    local_0->unknown458.i = local_2.i * 10.0f;
    local_0->unknown458.j = local_2.j * 10.0f;
    local_0->unknown458.k = local_2.k * 10.0f;
    local_0->unknown41c = 4;
    local_0->unknown420 = 4;
    local_0->unknown424.vector = local_2;
    if (local_0->unknown290.j * local_2.j + local_0->unknown290.k * local_2.k +
        local_0->unknown290.i * local_2.i > 0.8f)
        function_1e4290(arg_0, true);
}
#pragma inline_depth(255)
