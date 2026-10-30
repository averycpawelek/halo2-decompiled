// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "unknown_03bcb0.h"

long function_13a690(long mode);
bool function_22acb4(long local_player_index);

struct s_bitmap_widget_22b7dd
{
    byte unknown00[0x1c];
    word mode;
    byte unknown1e[6];
    long bitmap_tag;
    byte unknown28[4];
    long reference0;
    byte unknown30[4];
    long reference1;
    byte unknown38[4];
    long reference2;
    char sequences[3];
};

struct s_bitmap_sprite_22b7dd
{
    short bitmap_index;
    byte unknown02[0x1e];
};

struct s_bitmap_sequence_22b7dd
{
    byte unknown00[0x20];
    short first_bitmap;
    short bitmap_count;
    byte unknown24[0x10];
    long sprite_count;
    s_bitmap_sprite_22b7dd *sprites;
};

struct s_bitmap_sequence_block_22b7dd
{
    long count;
    s_bitmap_sequence_22b7dd *elements;
};

struct s_bitmap_tag_22b7dd
{
    byte unknown00[0x3c];
    s_bitmap_sequence_block_22b7dd sequences;
    long bitmap_count;
    byte *bitmaps;
};

/* predicts the texture of the bitmap a widget shows in its current mode;
   false when the texture is not ready */
// score 38 (was 38): the register that holds zero is EBX where retail uses EDX, so the index and the
// count are kept in the other registers. Ours folds the frame index to the constants 0 and 1 (the 0 is
// the shared zero, the 1 `xor/inc`); retail keeps it in ESI (`xor esi, esi` in each arm, `inc esi`).
// Round 18 (lane 10, plain checks): the index declared in each arm 49; a shared forceinline frame
// helper 49 (plain `inline` stays out of line under /O1, 72); the index as `cond ? 1 : 0` or as the
// `&&` value 80; a bool frame flag 100 (271 bytes: zero in EDX and everything before the frames as
// retail, but `movzx` at both uses); an unsigned index 38 (same code).
// count are kept in the other registers. Our frame index takes the zero register (its 0 and the
// shared zero merge, so the zero moves to a saved register); retail keeps index in ESI (xor esi, esi;
// inc esi) and zero in EDX, as the matched 0x22ae21 does. Round 93 (plain checks): shared frame-offset
// helper 83 (out of line), forceinline 49; index as the && value 80 (two constant arms); per-branch
// index with ++ 49, with = 1 (0x22ae21's form) 49; short index 73; dword index 38; index declared at
// the top 39; a bool frame choice 100 (271 bytes, but zero in EDX and aligned 19: only the bool's
// byte register and movzx differ); per-branch bools set in the test 105 (aligned 18: every
// instruction retail's but the bool sits in BL with movzx where retail's index is ESI, and the
// sequence in ESI where retail uses EBX); bools initialised from the && 84 (constant arms);
// no sequence-block pointer local 60.
// @retail 0x22b7dd
bool function_22b7dd(byte const *arg_1, long arg_2)
{
    s_bitmap_widget_22b7dd const *widget = (s_bitmap_widget_22b7dd const *)arg_1;
    long mode = function_13a690(widget->mode);
    bool result = true;
    long reference;
    switch (mode)
    {
    case 0:
        reference = widget->reference0;
        break;
    case 1:
        reference = widget->reference1;
        break;
    case 2:
        reference = widget->reference2;
        break;
    default:
        goto done;
    }
    if (reference != NONE && widget->bitmap_tag != NONE)
    {
        s_bitmap_tag_22b7dd *tag = (s_bitmap_tag_22b7dd *)g_4e3b44[widget->bitmap_tag & 0xffff].bytes;
        long sequence_index = 0;
        switch (mode)
        {
        case 0:
            sequence_index = widget->sequences[0];
            break;
        case 1:
            sequence_index = widget->sequences[1];
            break;
        case 2:
            sequence_index = widget->sequences[2];
            break;
        }
        if (sequence_index >= 0)
        {
            byte *bitmap;
            s_bitmap_sequence_block_22b7dd const *sequences = &tag->sequences;
            if (sequence_index < sequences->count)
            {
                s_bitmap_sequence_22b7dd *sequence = &sequences->elements[sequence_index];
                long bitmap_index;
                long index = 0;
                if (sequence->sprite_count > 0)
                {
                    if (sequence->sprite_count > 1 && function_22acb4(arg_2))
                        index++;
                    bitmap_index = sequence->sprites[index].bitmap_index;
                }
                else if (sequence->bitmap_count > 0)
                {
                    if (sequence->bitmap_count > 1 && function_22acb4(arg_2))
                        index++;
                    bitmap_index = sequence->first_bitmap + index;
                }
                else
                {
                    goto done;
                }
                bitmap = tag->bitmaps + bitmap_index * 0x74;
            }
            else if (sequence_index == 0 && tag->sequences.count == 0 && tag->bitmap_count > 0)
            {
                bitmap = tag->bitmaps;
            }
            else
            {
                goto done;
            }
            if (!function_3bcb0((s_bitmap_data *)bitmap))
                result = false;
        }
    }
done:
    return result;
}
