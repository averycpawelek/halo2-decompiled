// @flags /O2 /GL-
// Iterative constraint solving, residual updates, and result writeback.
#include "solver_dispatch_31c940.h"
#include <math.h>

static __forceinline float sum_three_31c940(__m128 const &v)
{
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
	__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
	float result;
	_mm_store_ss(&result, xyz);
	return result;
}

static __forceinline float angular_velocity(solver_angular_row_31e7b0 const *row,
	solver_velocity_31c940 const *a, solver_velocity_31c940 const *b)
{
	return sum_three_31c940(_mm_add_ps(_mm_mul_ps(a->angular, row->a), _mm_mul_ps(b->angular, row->b)));
}

static __forceinline float linear_velocity(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 const *a, solver_velocity_31c940 const *b)
{
	__m128 relative = _mm_sub_ps(a->linear, b->linear);
	__m128 angular = _mm_add_ps(_mm_mul_ps(a->angular, row->a), _mm_mul_ps(b->angular, row->b));
	return sum_three_31c940(_mm_add_ps(angular, _mm_mul_ps(relative, row->linear)));
}

static __forceinline void apply_angular(float impulse, solver_angular_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_velocity_31c940 *b, float *result)
{
	__m128 scalar = _mm_set_ss(impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), b->inverse_inertia);
	a->angular = _mm_add_ps(a->angular, _mm_mul_ps(change_a, row->a));
	b->angular = _mm_add_ps(b->angular, _mm_mul_ps(change_b, row->b));
	*result += impulse;
}

static __forceinline void apply_linear(float impulse, solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_velocity_31c940 *b, float *result)
{
	__m128 scalar = _mm_set_ss(impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), b->inverse_inertia);
	__m128 linear_a = _mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), row->linear);
	__m128 linear_b = _mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), row->linear);
	a->linear = _mm_add_ps(a->linear, linear_a);
	b->linear = _mm_sub_ps(b->linear, linear_b);
	a->angular = _mm_add_ps(a->angular, _mm_mul_ps(change_a, row->a));
	b->angular = _mm_add_ps(b->angular, _mm_mul_ps(change_b, row->b));
	*result += impulse;
}

static __forceinline float &operation_float(unsigned char *op, long offset)
{
	return *(float *)(op + offset);
}

// @retail 0x31c940
long __cdecl solve_operations_31c940(solver_step_31c940 *step, unsigned char *operations,
	solver_velocity_31c940 *bodies, float *impulses)
{
	if (!g_480c60)
	{
		function_31c7d0();
		if (!g_480c60) return 0;
	}
	for (long iteration = 0; iteration < step->iterations; ++iteration)
	{
		float saved_error = step->error_scale;
		float saved_velocity = step->velocity_scale;
		unsigned char *op = operations;
		__m128 *terms;
		solver_velocity_31c940 *a, *b;
		float *result = impulses;
		goto read_header;
		for (;;)
		{
			switch (*(signed char *)op)
			{
			case 0: goto iteration_solved;
			case 1:
			read_header:
			{
				solver_header_31c940 *header = (solver_header_31c940 *)op;
				a = header->a; b = header->b; terms = header->terms;
				op += sizeof(*header);
				break;
			}
			// Coupled unilateral linear rows; fall back to one active row.
			case 3:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				do
				{
					float error_a = row[0].linear.m128_f32[3] * step->error_scale
						- linear_velocity(row, a, b) * step->velocity_scale;
					float error_b = row[1].linear.m128_f32[3] * step->error_scale
						- linear_velocity(row + 1, a, b) * step->velocity_scale;
					float impulse_a = row[0].b.m128_f32[3] * error_a + error_b * operation_float(op, 4);
					float impulse_b = row[1].b.m128_f32[3] * error_b + error_a * operation_float(op, 4);
					if (impulse_a > 0.0f && impulse_b > 0.0f)
					{
						apply_linear(impulse_a, row, a, b, result);
						apply_linear(impulse_b, row + 1, a, b, result + 1);
					}
					else if (!(impulse_a > 0.0f) && !(error_b * row[1].a.m128_f32[3] <= 0.0f))
						apply_linear(error_b * row[1].a.m128_f32[3], row + 1, a, b, result + 1);
					else if (error_a * row[0].a.m128_f32[3] > 0.0f)
						apply_linear(error_a * row[0].a.m128_f32[3], row, a, b, result);
					row += 2; result += 2; op += 8;
				} while (*op == 3);
				terms = (__m128 *)row;
				break;
			}
			// A single unilateral linear row.
			case 2:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				do
				{
					float error = row->linear.m128_f32[3] * step->error_scale
						- linear_velocity(row, a, b) * step->velocity_scale;
					if (error > 0.0f) apply_linear(error * row->a.m128_f32[3], row, a, b, result);
					++row; ++result; op += 4;
				} while (*op == 2);
				terms = (__m128 *)row;
				break;
			}
			// Two tangential rows and a spinning row share one friction limit.
			case 8:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				solver_angular_row_31e7b0 *spin = (solver_angular_row_31e7b0 *)(row + 2);
				float error_a = row[0].linear.m128_f32[3] * step->friction_scale - linear_velocity(row, a, b) * step->velocity_scale;
				float error_b = row[1].linear.m128_f32[3] * step->friction_scale - linear_velocity(row + 1, a, b) * step->velocity_scale;
				float impulse_a = row[0].b.m128_f32[3] * error_a + error_b * operation_float(op, 8);
				float impulse_b = row[1].b.m128_f32[3] * error_b + error_a * operation_float(op, 8);
				float impulse_spin = (spin->b.m128_f32[3] * step->friction_scale - angular_velocity(spin, a, b) * step->velocity_scale) * spin->a.m128_f32[3];
				float norm = impulse_spin * impulse_spin + impulse_b * impulse_b + impulse_a * impulse_a;
				float maximum = operation_float(op, 12);
				if (norm > maximum * maximum)
				{
					float factor = maximum / sqrtf(norm);
					impulse_a *= factor; impulse_b *= factor;
					row[0].linear.m128_f32[3] *= factor;
					row[1].linear.m128_f32[3] *= factor;
					impulse_spin *= factor; spin->b.m128_f32[3] *= factor;
				}
				apply_linear(impulse_a, row, a, b, result);
				apply_linear(impulse_b, row + 1, a, b, result + 1);
				apply_angular(impulse_spin * operation_float(op, 20), spin, a, b, result + 2);
				terms += 8; result += 3; op += 24;
				break;
			}
			// Two tangential rows share a circular friction limit.
			case 7:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				float error_a = row[0].linear.m128_f32[3] * step->friction_scale - linear_velocity(row, a, b) * step->velocity_scale;
				float error_b = row[1].linear.m128_f32[3] * step->friction_scale - linear_velocity(row + 1, a, b) * step->velocity_scale;
				float impulse_a = row[0].b.m128_f32[3] * error_a + error_b * operation_float(op, 8);
				float impulse_b = row[1].b.m128_f32[3] * error_b + error_a * operation_float(op, 8);
				float norm = impulse_a * impulse_a + impulse_b * impulse_b;
				float maximum = operation_float(op, 12);
				if (norm > maximum * maximum)
				{
					float factor = sqrtf((maximum * maximum) / norm);
					impulse_a *= factor; impulse_b *= factor;
					row[0].linear.m128_f32[3] *= factor;
					row[1].linear.m128_f32[3] *= factor;
				}
				apply_linear(impulse_a, row, a, b, result);
				apply_linear(impulse_b, row + 1, a, b, result + 1);
				terms += 6; result += 2; op += 20;
				break;
			}
			// Angular friction with a scalar limit.
			case 9:
			{
				solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)terms;
				do
				{
					float impulse = (row->b.m128_f32[3] * step->friction_scale - angular_velocity(row, a, b) * step->velocity_scale) * row->a.m128_f32[3];
					float magnitude = (float)fabs(impulse);
					if (magnitude > operation_float(op, 4))
					{
						float factor = operation_float(op, 4) / magnitude;
						impulse *= factor; row->b.m128_f32[3] *= factor;
					}
					apply_angular(impulse, row, a, b, result);
					++row; ++result; op += 8;
				} while (*op == 9);
				terms = (__m128 *)row;
				break;
			}
			// Angular limits.
			case 10:
			{
				solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)terms;
				do
				{
					float velocity = angular_velocity(row, a, b) * step->velocity_scale;
					float error = (row->b.m128_f32[3] - operation_float(op, 4)) * operation_float(op, 12) - velocity;
					if (error > 0.0f) apply_angular(error * row->a.m128_f32[3], row, a, b, result);
					else
					{
						error = (row->b.m128_f32[3] - operation_float(op, 8)) * operation_float(op, 12) - velocity;
						if (error < 0.0f) apply_angular(error * row->a.m128_f32[3], row, a, b, result);
					}
					++row; ++result; op += 16;
				} while (*op == 10);
				terms = (__m128 *)row;
				break;
			}
			// A bounded angular impulse.
			case 4:
			{
				solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)terms;
				do
				{
					float impulse = (row->b.m128_f32[3] - angular_velocity(row, a, b)) * row->a.m128_f32[3] * step->error_scale;
					if (impulse > operation_float(op, 4)) impulse *= operation_float(op, 4) / impulse;
					else if (impulse < operation_float(op, 8)) impulse *= operation_float(op, 8) / impulse;
					apply_angular(impulse, row, a, b, result);
					++row; ++result; op += 16;
				} while (*op == 4);
				terms = (__m128 *)row;
				break;
			}
			// Angular error correction.
			case 12:
			{
				solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)terms;
				do
				{
					float impulse = row->a.m128_f32[3] * (row->b.m128_f32[3] * step->error_scale - angular_velocity(row, a, b) * step->velocity_scale);
					apply_angular(impulse, row, a, b, result);
					++row; ++result; op += 4;
				} while (*op == 12);
				terms = (__m128 *)row;
				break;
			}
			// Linear error correction.
			case 13:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				do
				{
					float impulse = row->a.m128_f32[3] * (row->linear.m128_f32[3] * step->error_scale - linear_velocity(row, a, b) * step->velocity_scale);
					apply_linear(impulse, row, a, b, result);
					++row; ++result; op += 4;
				} while (*op == 13);
				terms = (__m128 *)row;
				break;
			}
			// Linear friction with a scalar limit.
			case 6:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				do
				{
					float impulse = (row->linear.m128_f32[3] * step->friction_scale - linear_velocity(row, a, b) * step->velocity_scale) * row->a.m128_f32[3];
					float magnitude = (float)fabs(impulse);
					if (magnitude > operation_float(op, 4))
					{
						float factor = operation_float(op, 4) / magnitude;
						impulse *= factor; row->linear.m128_f32[3] *= factor;
					}
					apply_linear(impulse, row, a, b, result);
					++row; ++result; op += 8;
				} while (*op == 6);
				terms = (__m128 *)row;
				break;
			}
			// Independent lower and upper linear limits.
			case 11:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				do
				{
					float velocity = linear_velocity(row, a, b) * step->velocity_scale;
					float low = (row->linear.m128_f32[3] - operation_float(op, 4)) * step->error_scale - velocity;
					if (low > 0.0f) apply_linear(low * row->a.m128_f32[3], row, a, b, result);
					float high = (row->linear.m128_f32[3] - operation_float(op, 8)) * step->error_scale - velocity;
					if (high < 0.0f) apply_linear(high * row->a.m128_f32[3], row, a, b, result);
					++row; ++result; op += 12;
				} while (*op == 11);
				terms = (__m128 *)row;
				break;
			}
			// A bounded linear impulse.
			case 5:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				do
				{
					float impulse = (row->linear.m128_f32[3] - linear_velocity(row, a, b)) * row->a.m128_f32[3] * step->error_scale;
					if (impulse > operation_float(op, 4)) impulse *= operation_float(op, 4) / impulse;
					else if (impulse < operation_float(op, 8)) impulse *= operation_float(op, 8) / impulse;
					apply_linear(impulse, row, a, b, result);
					row = (solver_linear_row_31e7b0 *)((s_linear_term_31c530 *)row)->offset(1); ++result; op += 16;
				} while (*op == 5);
				terms = (__m128 *)row;
				break;
			}
			case 15:
				saved_velocity = step->velocity_scale; saved_error = step->error_scale;
				step->velocity_scale = operation_float(op, 8); step->error_scale = operation_float(op, 4);
				op += 12;
				break;
			case 16:
				step->error_scale = saved_error; step->velocity_scale = saved_velocity;
				op += 4;
				break;
			case 14:
				(*(void (__cdecl **)(__m128 *, void *))(op + 4))(terms, op + 8);
				op += op[1];
				break;
			}
		}

	iteration_solved:
		// Save the first iteration's velocities, then accumulate later ones.
		solver_velocity_31c940 *body = bodies;
		if (iteration == 0)
		{
			while (body->kind != 3)
			{
				if (body->kind == 2)
				{
					body->accumulated_angular = body->angular;
					body->accumulated_linear = body->linear;
					++body;
				}
			}
		}
		else
		{
			while (body->kind != 3)
			{
				if (body->kind == 2)
				{
					body->accumulated_angular = _mm_add_ps(body->accumulated_angular, body->angular);
					body->accumulated_linear = _mm_add_ps(body->accumulated_linear, body->linear);
					++body;
				}
			}
		}
		// The third operation byte classifies the residual row layout.
		op = operations;
		goto residual_header;
		for (;;)
		{
			switch ((signed char)op[2])
			{
			case 0: goto iteration_done;
			case 1:
			residual_header:
			{
				solver_header_31c940 *header = (solver_header_31c940 *)op;
				a = header->a; b = header->b; terms = header->terms; op += 24;
				break;
			}
			case 5:
			{
				do
				{
					solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
					float velocity_a = linear_velocity(row, a, b);
					float velocity_b = linear_velocity(row + 1, a, b);
					row[0].linear.m128_f32[3] -= velocity_a;
					row[1].linear.m128_f32[3] -= velocity_b;
					terms += 6;
					op += op[1];
				} while (op[2] == 5);
				break;
			}
			case 6:
			{
				do
				{
					solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
					float velocity_a = linear_velocity(row, a, b);
					float velocity_b = linear_velocity(row + 1, a, b);
					row[0].linear.m128_f32[3] -= velocity_a;
					row[1].linear.m128_f32[3] -= velocity_b;
					terms += 6;
					solver_angular_row_31e7b0 *spin = (solver_angular_row_31e7b0 *)terms;
					spin->b.m128_f32[3] -= angular_velocity(spin, a, b);
					terms += 2;
					op += op[1];
				} while (op[2] == 6);
				break;
			}
			case 3:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				do
				{
					row->linear.m128_f32[3] -= linear_velocity(row, a, b);
					++row; op += op[1];
				} while (op[2] == 3);
				terms = (__m128 *)row;
				break;
			}
			case 4:
			{
				solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)terms;
				do
				{
					row->b.m128_f32[3] -= angular_velocity(row, a, b);
					++row; op += op[1];
				} while (op[2] == 4);
				terms = (__m128 *)row;
				break;
			}
			case 2: op += op[1]; break;
			}
		}
	iteration_done:;
	}

	// Publish impulses and remaining error using each header's destination and
	// stride. Contact friction records carry a separate result destination.
	unsigned char *op = operations;
	__m128 *terms;
	unsigned char *destination;
	long stride;
	float *result = impulses;
	goto read_results_header;
	for (;;)
	{
		switch (*(signed char *)op)
		{
		case 1:
		read_results_header:
		{
			solver_header_31c940 *header = (solver_header_31c940 *)op;
			terms = header->terms; destination = header->results; stride = header->result_stride;
			op += 24; break;
		}
		case 2: case 5: case 6: case 11: case 13:
		{
			*(float *)destination = *result;
			*(float *)(destination + 4) = terms[0].m128_f32[3] * step->inverse_time;
			destination += stride;
			terms = (__m128 *)((s_linear_term_31c530 *)terms)->offset(1);
			++result; op += op[1]; break;
		}
		case 4: case 9: case 10: case 12:
		{
			*(float *)destination = *result;
			*(float *)(destination + 4) = terms[1].m128_f32[3] * step->inverse_time;
			destination += stride;
			terms = (__m128 *)((s_angular_cursor_31c520 *)terms)->offset(1);
			++result; op += op[1]; break;
		}
		case 3:
		{
			solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
			*(float *)destination = result[0];
			*(float *)(destination + 4) = row[0].linear.m128_f32[3] * step->inverse_time;
			destination += stride;
			*(float *)destination = result[1];
			*(float *)(destination + 4) = row[1].linear.m128_f32[3] * step->inverse_time;
			destination += stride;
			terms = (__m128 *)((s_linear_term_31c530 *)terms)->offset(2);
			result += 2; op += 8; break;
		}
		case 7:
		{
			unsigned char *friction_result = *(unsigned char **)(op + 4);
			long friction_stride = *(long *)(op + 16);
			solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
			*(float *)friction_result = result[0];
			*(float *)(friction_result + 4) = row[0].linear.m128_f32[3] * step->inverse_time;
			friction_result += friction_stride;
			*(float *)friction_result = result[1];
			*(float *)(friction_result + 4) = row[1].linear.m128_f32[3] * step->inverse_time;
			terms = (__m128 *)((s_linear_term_31c530 *)terms)->offset(2);
			result += 2; op += 20; break;
		}
		case 8:
		{
			unsigned char *friction_result = *(unsigned char **)(op + 4);
			long friction_stride = *(long *)(op + 16);
			solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
			*(float *)friction_result = result[0];
			*(float *)(friction_result + 4) = row[0].linear.m128_f32[3] * step->inverse_time;
			friction_result += friction_stride;
			*(float *)friction_result = result[1];
			*(float *)(friction_result + 4) = row[1].linear.m128_f32[3] * step->inverse_time;
			friction_result += friction_stride;
			solver_angular_row_31e7b0 *spin = (solver_angular_row_31e7b0 *)(row + 2);
			*(float *)friction_result = result[2];
			*(float *)(friction_result + 4) = spin->b.m128_f32[3] * step->inverse_time;
			terms = (__m128 *)((s_angular_cursor_31c520 *)spin)->offset(1);
			destination = 0;
			result += 3; op += 24;
			break;
		}
		case 0: return 1;
		default: op += op[1]; break;
		}
	}
}
