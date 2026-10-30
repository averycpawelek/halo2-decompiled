// @flags /O2 /Gr
/* UNKNOWN_19D220.CPP: game variant defaults and checks (lane H) */

#include "unknown_11c920.h"
#include <string.h>
#include <wchar.h>
#include "globals.h"
#include "unknown_19d220.h"

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

__declspec(noinline) s_game_variant *__stdcall function_19d220(s_game_variant *variant, long type);

// @retail 0x19d220
s_game_variant *__stdcall function_19d220(s_game_variant *variant, long type)
{
	return variant_defaults_initialize(variant, type);
}

/* clamps a setting into its range in place */
static inline void pin_value(long *value, long lower, long upper)
{
	*value = PIN(*value, lower, upper);
}

static inline void pin_value(short *value, long lower, long upper)
{
	*value = PIN(*value, lower, upper);
}

static inline void pin_value(char *value, long lower, long upper)
{
	*value = PIN(*value, lower, upper);
}

/* the settings of capture the flag and assault */
static __forceinline void flag_settings_verify(s_game_variant_flag_settings *settings)
{
	settings->flags &= 0xff;
	pin_value(&settings->unknownf4, 0, 0x7fff);
	pin_value(&settings->unknownf8, 0, 2);
	pin_value(&settings->unknownfc, 0, 1);
	pin_value(&settings->unknown100, 0, 3);
	pin_value(&settings->unknown104, 0, 2);
}

/* the settings of king of the hill and juggernaut */
static __forceinline void king_settings_verify(s_game_variant_king_settings *settings, dword flags_mask, long maximum)
{
	settings->flags &= flags_mask;
	pin_value(&settings->unknownf4, 0, maximum);
}

/* clamps every setting of a variant into its range; a variant that needed
   any change is replaced by the slayer defaults */
// score 57 (was 57): frame 0x330 agrees; post-copy flags and zero chains exchange EAX/ECX, adding four bytes.
// retail masks the flags in eax after the inlined defaults'
// wcsncpy call and keeps the zero in ecx; ours the other way round, which also
// makes the and/or two bytes longer each and so moves the jump table by 4 (its
// entries and padding then differ too).
// frame 0x330 agrees; post-copy flags and zero chains exchange EAX/ECX, adding four bytes.
// Earlier: the switch's jump table address is baked into the code
// and can never match, and retail masks the flags in eax where ours masks them
// in ecx, so the stores of the masked flags and the copy differ.
// Round 43: with eax busy right after the wcsncpy call the zero takes ecx and
// the whole tail becomes retail's (5 at 2088 bytes, via the terminator stored
// through wcsncpy's result; not kept: 0x19d220 29, 0x212380 420). Tried in the
// helper: per-arm flags48 expressions 58 (0x19d220 158); 0xf9a | (...) 58;
// name declared first 58; engine flags as &= then |= 58. In this body: the
// result cleared before the defaults 57; early returns 406; the equal case first
// 458; the result from the comparison 455; an else arm for the valid case 458.
// name declared first 58; engine flags as &= then |= 58.
// Native input snapshots in the long, short and char clamps, separately and together, 57 in
// all seven forms; store-on-change clamps 727 at 1928 bytes. Original clamp reads and stores
// restored; the shared defaults initializer is unchanged.
// Round 29: retail also stores flags48 after the zero stores; the helper's second flags48
// statement moved there 75 (0x19d220 118); `valid = memcmp(...) == 0` 455; `valid = false`
// before the defaults call 57; the original copy declared first 57; valid declared just
// before the compare 459.
// @retail 0x19d650
bool function_19d650(s_game_variant *variant)
{
	bool valid = true;
	s_game_variant original = *variant;

	variant->flags &= 1;
	variant->name[0x1f] = 0;
	variant->flags48 &= 0x7fff;
	variant->unknown4c = PIN(variant->unknown4c, 0, 6);
	pin_value(&variant->unknown50, 0, 0x7fff);
	pin_value(&variant->unknown54, 0, 0x7fff);
	pin_value(&variant->unknown58, 0, 2);
	pin_value(&variant->unknown74, 0, 0x10);
	pin_value(&variant->unknown78, 0, 0x10);
	pin_value(&variant->unknown7c, 0, 0x7fff);
	pin_value(&variant->unknown80, 0, 0x7fff);
	pin_value(&variant->unknown84, 0, 0x7fff);
	pin_value(&variant->unknown88, 0, 2);
	pin_value(&variant->unknowna4, 0, 2);
	pin_value(&variant->unknowna8, 0, 2);
	pin_value(&variant->unknownac, 0, 0x7fff);
	variant->unknownb0 = 0;
	variant->unknownb4 = PIN(variant->unknownb4, 0, 8);
	pin_value(&variant->unknowncc, 0, 3);
	pin_value(&variant->unknowncd, 0, 7);
	pin_value(&variant->unknownce, 0, 7);
	pin_value(&variant->unknowncf, 0, 4);
	pin_value(&variant->unknownd0, 0, 4);
	pin_value(&variant->unknownd1, 0, 4);
	pin_value(&variant->unknownd2, 0, 6);
	pin_value(&variant->unknownd3, 0, 6);
	pin_value(&variant->unknownd4, 0, 0x13);
	pin_value(&variant->unknownd5, 0, 3);
	pin_value(&variant->unknownd6, 0, 0x14);
	pin_value(&variant->unknownd7, 0, 0x14);
	pin_value(&variant->field_xcb8724, 1, 9);
	if (variant->field_xcb8724 == original.field_xcb8724)
	{
		switch (variant->field_xcb8724)
		{
		case 9:
			pin_value(&variant->flag.unknown108, 0, 0xf);
			pin_value(&variant->flag.unknown10a, 0, 0xf);
		case 1:
			variant->flags48 |= 1;
			flag_settings_verify(&variant->flag);
			break;
		case 2:
			variant->engine_flags &= 7;
			break;
		case 3:
			variant->ball.flags &= 7;
			pin_value(&variant->ball.unknownf4, 0, 3);
			pin_value(&variant->ball.unknownf6, 0, 1);
			pin_value(&variant->ball.unknownf8, 0, 2);
			pin_value(&variant->ball.unknownfa, 0, 3);
			break;
		case 4:
			king_settings_verify(&variant->king, 0x1f, 0x7fff);
			break;
		case 7:
			variant->flags48 &= ~1;
			king_settings_verify(&variant->king, 0x7f, 2);
			break;
		case 8:
			pin_value(&variant->territories.unknownf0, 1, 8);
			pin_value(&variant->territories.unknownf2, 1, 0x7fff);
			pin_value(&variant->territories.unknownf4, 1, 0x7fff);
			break;
		default:
			variant->field_xcb8724 = 10;
			break;
		}
	}
	if (memcmp(&original, variant, sizeof(original)) != 0)
	{
		variant_defaults_initialize(variant, 0);
		valid = false;
	}
	return valid;
}

/* whether the variant is valid as it is: checks a copy */
// @retail 0x19d620
bool function_19d620(s_game_variant *variant)
{
	s_game_variant copy = *variant;

	return function_19d650(&copy);
}
