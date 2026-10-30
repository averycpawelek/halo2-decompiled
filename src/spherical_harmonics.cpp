// @flags /O2 /Gr /arch:SSE
/* SPHERICAL_HARMONICS.CPP: real spherical harmonics up to the fourth order
   (16 coefficients): the basis evaluated in a direction, and a directional
   light projected onto it */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

#define k_spherical_harmonics_pi 3.14159265f

/* the basis functions of the first order*order bands in a direction */
// score 59 (was 59): +0x50 x87/SSE scheduling, the cubic band's spill homes and the root lifetimes
// differ: retail keeps sqrt(2) and sqrt(105) on the x87 stack to the end (fmul st(1)/st(2), two
// fstp st(0)), computes coefficient 15 in SSE right after band3_35 (-0.125 already in xmm3), keeps
// zz*5 in zz's slot and band3_35 in sqrt(15)'s. Forms (123 at the start): outer/cubic order
// return 123/123; roots before inputs 137; early Z square 128; first-band Z store first 128;
// repeated inline sqrt(2.0)/sqrt(105.0) 158-162; /Op 383; first-band, first-Y, square-difference,
// quadratic mixed-product stagings 123; cubic Y staging 129; mixed-root staging 123; final-root
// product chain 113; XZ product chain 59 (kept); second cubic root chain, coefficient 8/12/9/5/6
// chains, root/edge combination 113; repaired XZ plus early coefficient 15 147/148; cubic X chain
// 139; first cubic root chain 59; linear Y/Z/X/all values 59; late root-five/depth square 59/59;
// linear scope 59; later common root 59; isotropic result lifetime, band-two normalization,
// Z-square, cubic-axis/polynomial accumulators 59; repaired XZ plus cubic-Y chain 137; external
// direction reference, both double root chains 59; per-use common root 59; per-use mixed/both
// roots 78/78; completed cubic X/Y/paired results 59; complete mixed result 59; first Y store
// before isotropic 59; linear factor after isotropic 143; quadratic root after squares 138;
// axis-root scope 59; depth/edge/axis/joint scalar homes 59; double root 158; root rounded per
// factor 158; plus early coefficient 15 140; early coefficient 15 alone 147; retail store order
// 144; all three roots double 77; sqrt(105)+sqrt(7) double 66; sqrt(105) double 73; zz scaled in
// place 70; all double with coefficient 15 ahead of 9 150; double roots with `(real)` casts at
// both/one/other sqrt(105) use 80/82/82; double sqrt(2)/sqrt(105) with a real sqrt(7) 82;
// coefficient 15 recomputing `(real)(sqrt(35.0) * sqrt2)` 130.
// cubic homes and retained common-root lifetime remain; early schedules have an unkept 54 form.
// Y/scalar homes and retained x87 roots differ: retail never consumes sqrt(2)
// or sqrt(105) at their last use (fmul st(1)/st(2), two fstp st(0) before the return; result[14] in
// source order), ours folds them in (fmulp st(2), result[14] reassociated around sqrt105).
// Tried: outer order return 123; cubic order return 123; roots before inputs 137;
// early Z square 128; first-band Z store first 128; repeated inline sqrt(2.0)/sqrt(105.0) (retains
// the roots like retail but moves the band-3 products) 158-162; /Op 383 (and 0x146020 stops matching).
// Y/scalar homes and sqrt(2) lifetime still differ; final chain retains sqrt(105).
// First-band product staging, first Y product, square-difference staging and quadratic
// mixed-product staging 123; cubic Y staging 129; mixed-root staging 123;
// final-root product chain 113 plain kept (651/660); linear plus final chain 113.
// frame and quadratic homes agree; x87/SSE scheduling and sqrt(2) retention differ.
// XZ product chain 59 plain kept; second cubic root chain, coefficient 8/12/9/5/6 chains
// and root/edge combination 113; repaired XZ plus early coefficient 15 147/148,
// cubic X chain 139, first cubic root chain 59.
// SSE/x87 scheduling, cubic homes and common-root retention remain; repaired-prefix
// linear Y/Z/X/all values 59 each; late root-five/depth square 59/59; linear scope 59; later common
// root 59.
// first +0x50 x87/SSE schedule and retained sqrt(2) still differ.
// Isotropic result lifetime, band-two normalization, Z-square and cubic-axis/polynomial accumulators 59;
// repaired-XZ plus cubic-Y chain 137; external direction reference and both double root chains 59.
// +0x50 scheduling, cubic homes and sqrt(2) retention remain.
// Repaired-prefix per-use common root 59; per-use mixed/both roots 78/78; completed cubic X,
// Y and paired results 59/59/59; paired results with per-use root 59; complete mixed result 59.
// x87/SSE scheduling and retained common root still differ.
// First Y store before isotropic 59; linear factor after isotropic 143; quadratic root
// after squares 138; axis-root scope 59; depth/edge/axis/joint scalar homes 59/59/59/59.
// a double or unrounded common root is retained like retail's but then keeps
// band3_35 on x87 and moves coefficient 15 to x87: double root 158; root rounded per factor 158;
// plus early coefficient 15 140; early coefficient 15 alone 147; retail store order 144.
// Native float roots: common/mixed/both 59/59/59; linear/quadratic 56/57; axis pair 59;
// cubic factor pair 158; every root 156; linear plus quadratic roots 54; other rounded roots added 54;
// both early root boundaries with four/six cubic product chains 139/139. The 54 form is below the eight-point gate.
// +0x50 x87/SSE scheduling, the cubic homes and root retention differ: retail
// never consumes sqrt(2) or sqrt(105) at their last use (fmul st(1)/st(2), two fstp st(0) before the
// return), computes coefficient 15 right after band3_35 with -0.125 held in xmm3, and keeps zz*5 in
// zz's slot, band3_35 in sqrt(15)'s and band3_21 in a new one; ours consumes sqrt(2) (fmulp st(2)).
// With sqrt(2), sqrt(105) and sqrt(7) all double the x87 root code is retail's, but the spill
// slots rotate (zz*5 new, band3_35 in zz's, band3_21 in sqrt(15)'s) and -0.125 loads late: 77.
// Tried, at 123: outer order return; cubic order return; roots before inputs 137; early Z square
// 128; first-band Z store first 128; repeated inline sqrt(2.0)/sqrt(105.0) 158-162 (retains the
// roots like retail but moves the band-3 products); /Op 383 (0x146020 then differs). First-band
// product staging, first Y product, square-difference staging and quadratic mixed-product staging
// 123; cubic Y staging 129; mixed-root staging 123; final-root product chain 113 kept (651/660);
// linear plus final chain 113. XZ product chain 59 kept; second cubic root chain, coefficient
// 8/12/9/5/6 chains and root/edge combination 113; repaired XZ plus early coefficient 15 147/148;
// cubic X chain 139; first cubic root chain 59. At 59: repaired-prefix linear Y/Z/X/all values 59
// each; late root-five/depth square 59/59; linear scope 59; later common root 59; isotropic result
// lifetime, band-two normalization, Z-square and cubic-axis/polynomial accumulators 59;
// repaired-XZ plus cubic-Y chain 137; external direction reference and both double root chains 59;
// per-use common root 59; per-use mixed/both roots 78/78; completed cubic X, Y and paired results
// 59/59/59; paired results with per-use root 59; complete mixed result 59; first Y store before
// isotropic 59; linear factor after isotropic 143; quadratic root after squares 138; axis-root
// scope 59; depth/edge/axis/joint scalar homes 59/59/59/59; double root 158; root rounded per
// factor 158; plus early coefficient 15 140; early coefficient 15 alone 147; retail store order
// 144. All three roots double 77; sqrt(105) and sqrt(7) double 66; sqrt(105) double 73; zz scaled
// in place 70; all double with coefficient 15 before coefficient 9 150 (645 bytes); coefficient
// 15 at each position in the order, root types and local names (screened) no better.
// @retail 0x143360
void spherical_harmonics_evaluate_direction(vector3f const *direction, dword order, real *result)
{
	real x = direction->i;
	real y = direction->j;
	real z = direction->k;
	real inverse_sqrt_pi = (real)(1.0f / sqrt(k_spherical_harmonics_pi));
	real band1 = (real)(sqrt(3.0) * inverse_sqrt_pi);

	result[0] = inverse_sqrt_pi * 0.5f;
	result[1] = band1 * y * -0.5f;
	result[2] = band1 * z * 0.5f;
	result[3] = band1 * x * -0.5f;
	if (order > 2)
	{
		real sqrt15 = (real)sqrt(15.0);
		real band2 = sqrt15 * inverse_sqrt_pi;
		real xy = y * x;
		real xx = x * x;
		real yy = y * y;
		real xx_yy = xx - yy;
		real zz = z * z;
		real sqrt5;

		result[4] = xy * band2 * 0.5f;
		result[5] = band2 * z * y * -0.5f;
		sqrt5 = (real)sqrt(5.0);
		result[6] = sqrt5 * (zz * 3.0f - 1.0f) * inverse_sqrt_pi * 0.25f;
		real quadratic_xz = z * x;
		quadratic_xz *= band2;
		result[7] = quadratic_xz * -0.5f;
		result[8] = xx_yy * sqrt15 * inverse_sqrt_pi * 0.25f;
		if (order > 3)
		{
			real sqrt2 = (real)sqrt(2.0);
			real zz5 = zz * 5.0f;
			real zz5_1 = zz5 - 1.0f;
			real xx_3yy = xx - yy * 3.0f;
			real band3_35 = (real)(sqrt(35.0) * sqrt2);
			real sqrt105 = (real)sqrt(105.0);
			real sqrt7;
			real band3_21;

			band3_21 = (real)(sqrt(21.0) * sqrt2);
			result[11] = zz5_1 * band3_21 * inverse_sqrt_pi * y * -0.125f;
			result[13] = inverse_sqrt_pi * x * zz5_1 * band3_21 * -0.125f;
			result[9] = (xx * 3.0f - yy) * band3_35 * inverse_sqrt_pi * y * -0.125f;
			result[10] = xy * sqrt105 * inverse_sqrt_pi * z * 0.5f;
			sqrt7 = (real)sqrt(7.0);
			result[12] = sqrt7 * (zz5 - 3.0f) * inverse_sqrt_pi * z * 0.25f;
			real cubic_difference = inverse_sqrt_pi * z;
			cubic_difference *= sqrt105;
			cubic_difference *= xx_yy;
			result[14] = cubic_difference * 0.25f;
			result[15] = xx_3yy * band3_35 * inverse_sqrt_pi * x * -0.125f;
		}
	}
}

/* a directional light's colour in the basis of the first order*order bands */
// @retail 0x146020
bool spherical_harmonics_evaluate_directional_light(vector3f const *direction, dword order, real red, real green, real blue,
	real *red_result, real *green_result, real *blue_result)
{
	real coefficients[16];
	real normalization;
	real scale;
	dword count;
	dword i;

	spherical_harmonics_evaluate_direction(direction, order, coefficients);
	normalization = 0.75f;
	if (order > 2)
	{
		normalization = 1.0625f;
	}
	normalization = 3.141593f / normalization;
	count = order * order;

	scale = normalization * red;
	for (i = 0; i < count; i++)
	{
		red_result[i] = coefficients[i] * scale;
	}
	if (green_result)
	{
		scale = normalization * green;
		for (i = 0; i < count; i++)
		{
			green_result[i] = coefficients[i] * scale;
		}
	}
	if (blue_result)
	{
		scale = normalization * blue;
		for (i = 0; i < count; i++)
		{
			blue_result[i] = coefficients[i] * scale;
		}
	}
	return true;
}
