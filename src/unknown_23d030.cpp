#include "unknown_4e8c38.h"
// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
/* UNKNOWN_23D030.CPP: the camera that flies freely, and the point it keeps
   relative to an object (0x23d030..0x23d970) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "normalize3d.h"
#include <math.h>

struct s_player_control_camera
{
	long unit_index;
	short seat_index;
	byte unknown06[2];
	void *camera;
	point3f position;
};



extern real g_54e854;
void player_control_get_camera(long player_index, s_player_control_camera *camera);
vector3f *function_11d090(vector3f const *v, vector3f *out);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
struct s_observer_command;
void function_172520(s_observer_command *command);

struct s_camera_input_23d790
{
	long player_index;
	byte active;
	byte unknown05[7];
	real yaw;
	real pitch;
	byte unknown14[0x24 - 0x14];
	real zoom;
};

struct s_camera_output_23d790
{
	dword flags;
	point3f position;
	vector3f offset;
	byte unknown1c[8];
	real distance;
	real field_of_view;
	vector3f forward;
	vector3f up;
	vector3f velocity;
	byte unknown50[0x88 - 0x50];
	real timer;
};

struct s_orbit_state_23d790
{
	real unknown00;
	real distance;
	real unknown08;
	real yaw;
	real pitch;
};

PRIVATE __forceinline void orbit_direction_23d790(s_orbit_state_23d790 const *state, vector3f *direction)
{
	real cosine = (real)cos(state->pitch);
	direction->i = (real)cos(state->yaw) * cosine;
	direction->j = (real)sin(state->yaw) * cosine;
	direction->k = (real)sin(state->pitch);
}

// @retail 0x23d790
void __stdcall function_23d790(void *state, void *input, void *output)
{
	s_player_control_camera camera;
	s_camera_input_23d790 *controls = (s_camera_input_23d790 *)input;
	s_orbit_state_23d790 *values = (s_orbit_state_23d790 *)state;
	s_camera_output_23d790 *command = (s_camera_output_23d790 *)output;
	player_control_get_camera(controls->player_index, &camera);
	command->position = camera.position;
	if (controls->active)
	{
		values->yaw += controls->yaw;
		real pitch = values->pitch + controls->pitch;
		values->pitch = pitch < -1.2566371f ? -1.2566371f : pitch > 1.2566371f ? 1.2566371f : pitch;
		((byte *)g_4e8c44)[controls->player_index * 0x140 + 0x56] = 1;
	}
	real distance = values->distance - controls->zoom * (1.f / 3.f);
	values->distance = distance > 0.6f ? distance : 0.6f;
	if (camera.unit_index != NONE)
	{
		orbit_direction_23d790(values, &command->forward);
		function_11d090(&command->forward, &command->up);
		function_ba1d0(camera.unit_index, &command->velocity, 0);
		command->flags = 1;
	}
	command->offset = *g_4687a4;
	command->distance = values->distance;
	command->field_of_view = g_54e854;
	command->timer = 0.5f;
	function_172520((s_observer_command *)command);
}

void function_16c6f0(long object_index, transform4x3f *matrix);
point3f *function_142700(transform4x3f const *matrix, point3f const *point, point3f *out);

/* the point the camera keeps (NULL when none), the object it is relative to
   and the point in that object's space */
point3f *g_51ec28;
long g_470a28 = NONE;
point3f g_51ec30;

byte g_51ec12;
real g_470a24 = 1.f;
extern byte g_51ec11;

struct s_free_state_23d260
{
	point3f position;
	real yaw;
	real pitch;
	real roll;
};
// score 110 (was 110): Zero/side XMM roles and speculative normalization remain; frame 0x4c agrees. Complete
// axial/cosine-normal/affine vector contributions, masks 1/2/3/4/5/6/7, scored
// 281/281/335/313/314/329/332; existing add-scaled affine assembly 313. Original normalizer,
// projection, roll rows and movement restored.
// Earlier: the rest is register allocation in the side vector
// block: retail keeps zero in xmm1 (ours xmm3) and the side vector in
// xmm2/xmm3/xmm4 (ours xmm1/xmm2/xmm0, retail moves side.k out of the inverse's
// register), divides before the lahf, and stores up as k, j, i (ours k, i, j);
// the registers after it follow. Locals `real yaw_cosine = (real)cos(yaw)` and
// `yaw_sine` for the movement give retail's x87 sequence exactly (fmul [0x18];
// fxch; fmul [0x1c]) but score 274 here, since the side block is then one
// instruction short and shifts the rest: apply them once that block matches.
// Tried: the hand-written normalize with gotos (278), normalize_inline (110 but
// 1330 bytes, squares i first), early-return or reordered-scale normalize helpers
// (118 / 288), cross3f(&side, &forward, &up) (277), up written i, j, k (253),
// k, i, j (276) or j, k, i / i, k, j / j, i, k (106-111 but not 1326 bytes: they
// spill forward.k to a parameter slot), the transforms as __forceinline helpers (322), fresh names for the
// scaled point (373), movement operands swapped (282), the roll if/else inverted
// (279), a cross vector in the turn about forward (277), declaration order (same)
// Earlier: zero XMM3 versus XMM1 and normalization/side-vector allocation remain.
// Roll-input snapshot 324; live yaw/pitch/roll fields 275/121/121; live forward/up/both fields 295/154/157; normalization result 319.
// Earlier: the existing transform4x3f_apply_point/function_1427f0 called for the two
// matrix blocks 603 (992 bytes, kept out of line); if/else pitch clamp 282; roll ternary 146; side j
// and k assigned apart 118; cosine before sine 119; side.k first 118; up written j, i, k / j, k, i /
// i, k, j 254/251/249 (1330-1333 bytes).
// Round 71: shared normalize_inline with side.k first kept at 110 (1330 bytes, plain);
// planar initialization reversed 110; up written j, k, i / j, i, k / i, k, j on that graph
// 293/112/287; shared yaw sine/cosine 110; roll update before the pitch clamp 132;
// a one-use normalization input pointer 107 (1329 bytes, exact-match-only). No pointer kept.
// Round 409: Existing staged up cross alone/fallback setter/initial setter/both setters 244/243/244/238; all recover zero XMM1 and first +0xa6, last reaches 1326 bytes, but side/up/roll allocation worsens. Initial/fallback/both whole-vector setters 110; uniformly const trig values 110. Original body and normalization retained; no helper or include kept.
// @retail 0x23d260
void __stdcall function_23d260(void *state, void *input, void *output)
{
	s_free_state_23d260 *camera = (s_free_state_23d260 *)state;
	s_camera_input_23d790 *controls = (s_camera_input_23d790 *)input;
	s_camera_output_23d790 *command = (s_camera_output_23d790 *)output;
	if (controls->active)
	{
		camera->yaw += controls->yaw;
		real pitch = camera->pitch + controls->pitch;
		camera->pitch = pitch < -1.56765485f ? -1.56765485f : pitch > 1.56765485f ? 1.56765485f : pitch;
		if (g_51ec12)
			camera->roll += *(real *)((byte *)input + 0x14);
		else
			camera->roll = 0.f;
	}
	command->timer = 0.3f;
	real pitch_cosine = (real)cos(camera->pitch);
	command->forward.i = (real)cos(camera->yaw) * pitch_cosine;
	command->forward.j = (real)sin(camera->yaw) * pitch_cosine;
	command->forward.k = (real)sin(camera->pitch);
	vector3f side;
	side.k = 0.f;
	side.i = command->forward.j;
	side.j = -command->forward.i;
	if (normalize_inline(&side) == 0.f)
	{
		side.i = 1.f;
		side.j = side.k = 0.f;
	}
	command->up.k = command->forward.j * side.i - side.j * command->forward.i;
	command->up.j = side.k * command->forward.i - command->forward.k * side.i;
	command->up.i = command->forward.k * side.j - command->forward.j * side.k;
	real sine = (real)sin(camera->roll);
	real cosine = (real)cos(camera->roll);
	{
		vector3f *v = &command->up;
		vector3f const *axis = &command->forward;
		real projection = (axis->i * v->i + axis->j * v->j + axis->k * v->k) * (1.f - cosine);
		real cross_i = axis->k * v->j - v->k * axis->j;
		real cross_j = axis->i * v->k - axis->k * v->i;
		real cross_k = axis->j * v->i - axis->i * v->j;
		v->i = axis->i * projection + cosine * v->i - cross_i * sine;
		v->j = axis->j * projection + cosine * v->j - cross_j * sine;
		v->k = axis->k * projection + cosine * v->k - cross_k * sine;
	}
	vector3f movement;
	movement.i = (real)(cos(camera->yaw) * *(real *)((byte *)input + 0x18) - sin(camera->yaw) * *(real *)((byte *)input + 0x1c));
	movement.j = (real)(sin(camera->yaw) * *(real *)((byte *)input + 0x18) + cos(camera->yaw) * *(real *)((byte *)input + 0x1c));
	movement.k = *(real *)((byte *)input + 0x20);
	point3f position;
	if (g_470a28 != NONE)
	{
		transform4x3f matrix;
		function_16c6f0(g_470a28, &matrix);
		real x = movement.i;
		real y = movement.j;
		real z = movement.k;
		if (matrix.scale != 1.f)
		{
			real inverse = 1.f / matrix.scale;
			x = inverse * x;
			y = inverse * y;
			z = inverse * z;
		}
		vector3f local;
		local.i = matrix.forward.k * z + matrix.forward.j * y + matrix.forward.i * x;
		local.j = matrix.left.k * z + matrix.left.j * y + matrix.left.i * x;
		local.k = matrix.up.k * z + matrix.up.j * y + matrix.up.i * x;
		real speed = g_470a24;
		g_51ec30.x += speed * local.i;
		g_51ec30.y += speed * local.j;
		g_51ec30.z += speed * local.k;
		x = g_51ec30.x;
		y = g_51ec30.y;
		z = g_51ec30.z;
		if (matrix.scale != 1.f)
		{
			x = matrix.scale * g_51ec30.x;
			y = matrix.scale * g_51ec30.y;
			z = matrix.scale * g_51ec30.z;
		}
		position.x = matrix.up.i * z + matrix.left.i * y + matrix.forward.i * x + matrix.position.x;
		position.y = matrix.up.j * z + matrix.left.j * y + matrix.forward.j * x + matrix.position.y;
		position.z = matrix.up.k * z + matrix.left.k * y + matrix.forward.k * x + matrix.position.z;
	}
	else
	{
		position.x = g_470a24 * movement.i + camera->position.x;
		position.y = g_470a24 * movement.j + camera->position.y;
		position.z = g_470a24 * movement.k + camera->position.z;
	}
	camera->position = position;
	command->position = position;
	command->offset = *g_4687a4;
	command->distance = 0.f;
	command->field_of_view = g_54e854;
	command->flags = 1;
	if (g_51ec11 > 0)
	{
		command->timer = 0.f;
		command->flags = 9;
		g_51ec11--;
	}
	function_172520((s_observer_command *)command);
}

/* makes the kept point relative to the object (to the world for none) */
// @retail 0x23d030
void function_23d030(long object_index)
{
	point3f *point = g_51ec28;

	g_470a28 = object_index;
	if (point)
	{
		transform4x3f matrix;

		function_16c6f0(object_index, &matrix);
		if (object_index != NONE)
		{
			function_142700(&matrix, point, &g_51ec30);
		}
		else
		{
			g_51ec30 = *g_468788;
		}
	}
}
