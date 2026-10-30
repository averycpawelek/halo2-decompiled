#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include <math.h>
// @flags /O2 /Ob1 /arch:SSE /Gr

void function_1d1260(s_havok_component *arg_0);
void __stdcall function_1d01c0(s_havok_component *arg_0);
void __stdcall function_bd020(long arg_0);
void function_1cf120(long arg_0);
void function_1d1540(s_havok_component *arg_0);
transform4x3f *function_ba160(long arg_0, transform4x3f *arg_1);
bool __stdcall function_e0d70(long arg_0, real *arg_1);
void havok_component_rigid_body_matrix_set(long arg_0, s_havok_component *arg_1, transform4x3f const *arg_2);
void function_1d4360(s_havok_component *arg_0, transform4x3f *volatile arg_1);
void function_141590(transform4x3f const *arg_0, transform4x3f *arg_1);
int __fastcall function_142a60(transform4x3f const *arg_0, transform4x3f const *arg_1, transform4x3f *arg_2);

PRIVATE __forceinline void function_1d3fc1(vector3f *arg_0)
{
 real j_squared = arg_0->j * arg_0->j;
 real local_0 = (real)sqrt(arg_0->i * arg_0->i + j_squared + arg_0->k * arg_0->k);
 if (!(fabs(local_0) < 0.0001f))
 {
  real local_1 = 1.0f / local_0;
  arg_0->i = local_1 * arg_0->i;
  arg_0->j = arg_0->j * local_1;
  arg_0->k = arg_0->k * local_1;
 }
}

// score 149 (was 149): magnitude order, reciprocal/SSE carriers and projection schedule remain.
// Retail sums the forward magnitude j,i,k (k loaded late, reloaded on the short-vector path) and the
// other three k,j,i; in this /Ob1 file every sum compiles i,j,k whatever the source order (dot3f,
// k*k+j*j+i*i, i*i+j*j+k*k each 155 with the normalize3d shape), only a named square moves it.
// Axis consumer scopes 155/155/155; symmetric dot 155; projection order 155; dot/projection 155;
// early inputs 155; native scaled result 352; component-input inline 155; output-first component
// inline 155; helper-return receivers 153; rotation receiver 155; squared magnitude 155; float square
// root 155; float absolute value 155; reverse stores 155; j/k/i stores 155; complete projection 155;
// component scale loop 155; shared self-dot 155; i/j/k squared values 152/149/154; all squares 155;
// self-dot or all squares with scale loop 155 (keep the j-square intermediate); paired i/j and j/k
// squares 152/153; first-plane subtotal 149; sequential squared length 152; paired squares with
// subtotal 152/149; separate reciprocal division 149; paired squares with scaled components 153;
// normalized i/j/k products 149/149/149; ij/ik/jk products 149/149/149; all products and k/j/i
// product evaluation 149/149; native n-array normalization 149; array receiver 149; scalar-array
// helper 149; array scale loop 149; each array form with indexed projection 149/149/149; indexed
// projection alone 149. Header normalize3d (not inlined under /Ob1: four calls) 453; six transforms
// at function scope instead of the array (same frame and slots, identical code) 149.
// Round 36: separate remaining-axis k-square helper 152; remaining terms reversed 152; k/j square pair 152;
// pair declarations swapped 152; k-first sum 152; paired k/j-first sum 152; grouped j/i 149; named j/i subtotal 149.
// @retail 0x1d3fc0
void __stdcall function_1d3fc0(long arg_0, s_havok_component *arg_1)
{
 real local_0;
 transform4x3f local_1[6];
 if (TEST_FIELD_BIT(arg_1->flag10))
 {
  bool local_7 = TEST_FIELD_BIT(arg_1->flag5);
  if (local_7)
   function_1d1260(arg_1);
  function_1d01c0(arg_1);
  function_bd020(arg_1->object_index);
  function_1cf120(arg_0);
  if (local_7)
   function_1d1540(arg_1);
 }
 else if (TEST_FIELD_BIT(arg_1->flag9))
 {
  function_ba160(arg_1->object_index, &local_1[0]);
  byte *local_8 = (byte *)g_4e0300->data;
  if (((1 << local_8[(arg_1->object_index & 0xffff) * 12 + 3]) & 1) &&
      function_e0d70(arg_1->object_index, &local_0))
   local_1[0].position.z -= local_0;
  havok_component_rigid_body_matrix_set(0, arg_1, &local_1[0]);
 }
 else
 {
  function_1d4360(arg_1, &local_1[4]);
  function_ba160(arg_1->object_index, &local_1[2]);
  function_141590(&local_1[4], &local_1[1]);
  function_142a60(&local_1[2], &local_1[1], &local_1[5]);
  for (long local_9 = 0; local_9 < arg_1->rigid_bodies.size; ++local_9)
  {
   arg_1->rigid_body_matrix_get(local_9, &local_1[3]);
   function_142a60(&local_1[5], &local_1[3], &local_1[0]);
   if (arg_1->unknown04 & 0x10000)
   {
    vector3f *forward = &local_1[0].forward;
    vector3f *up = &local_1[0].up;
    vector3f *left = &local_1[0].left;
    function_1d3fc1(forward);
    function_1d3fc1(up);
    real local_10 = dot3f(forward, up);
    up->i -= forward->i * local_10;
    up->j -= forward->j * local_10;
    up->k -= forward->k * local_10;
    function_1d3fc1(up);
    left->i = up->j * forward->k - up->k * forward->j;
    left->j = up->k * forward->i - up->i * forward->k;
    left->k = forward->j * up->i - up->j * forward->i;
    function_1d3fc1(left);
   }
   havok_component_rigid_body_matrix_set(local_9, arg_1, &local_1[0]);
  }
  arg_1->unknown04 |= 0x10000;
 }
}
