#include "unknown_11c920.h"
#include "files.h"
#include <string.h>

// @flags /O2 /Gr

bool function_1249f0(long memory_unit, char *drive_letter);
char *function_11c9c0(char *buffer, long maximum_count, const char *format, ...);
char *function_122810(char *string, const char *suffix);
const char *saved_game_type_get_storage_name(long type);
bool function_1368f0(s_type_acf665 *file);

// @retail 0x216400
char *function_216400(long memory_unit, char *buffer, short capacity)
{
	char drive_letter;
	if (function_1249f0(memory_unit, &drive_letter))
		function_11c9c0(buffer, capacity, "%c:\\", drive_letter);
	else
		buffer[0] = 0;
	return buffer;
}

// @retail 0x216d70
bool function_216d70(char *path, const char *directory, long type)
{
	strncpy(path, directory, 0x100);
	path[0xff] = 0;
	function_122810(path, saved_game_type_get_storage_name(type));
	return true;
}

struct s_storage_file_reference
{
	dword signature;
	word flags;
	short location;
	char path[0x100];
	__int64 state;
};

// @retail 0x217590
long function_217590(long memory_unit)
{
	char root[8] = "";
	char drive_letter;
	long count = 0;
	if (function_1249f0(memory_unit, &drive_letter))
		function_11c9c0(root, sizeof(root), "%c:\\", drive_letter);
	else
		root[0] = 0;
	XGAME_FIND_DATA data;
	HANDLE find = XFindFirstSaveGame(root, &data);
	if (find != INVALID_HANDLE_VALUE)
	{
		do
		{
			count++;
		}
		while (XFindNextSaveGame(find, &data));
		XFindClose(find);
	}
	return count;
}



void __stdcall function_216e50(word const *display_name, long type, char *language, word *name);
void unicode_string_snprintf(word *buffer, long maximum_count, word const *format, ...);
void unicode_string_to_ascii(const word *source, char *destination, long maximum_count);

struct s_save_entry_2164c0
{
    char directory[20];
    word name[17];
    byte unknown36[2];
    long type;
    byte unknown3c;
    char language;
    byte unknown3e[2];
};

struct s_save_list_2164c0
{
    long count;
    s_save_entry_2164c0 entries[4096];
};

PRIVATE inline bool append_save_entry_2164c0(s_save_list_2164c0 *list, s_save_entry_2164c0 const *entry)
{
    bool result = list->count != 4096;
    if (result)
    {
        memcpy(&list->entries[list->count++], entry, sizeof(s_save_entry_2164c0));
    }
    return result;
}

// score 32 (was 32): the append's registers differ (ours list EDX, count EAX, result CL; retail
// list ECX, count ESI copied to EAX after the test, result AL, new count index + 1 stored before the
// copy), and the scratch pairs before the format call and XFindNextSaveGame/XFindClose follow from it.
// Every form with a materialized result gives ours; every form without one gives retail's list ECX
// and both scratch pairs but no setne/test. A byte scan finds retail's sequence nowhere else.
// Tried: memcpy into `&entries[count++]` 32 (kept, from 51); file path/directory/both receivers
// 51/51/51; conditional character store 69; capacity first 54/41; capacity nonzero 51; short type
// index 189; no parameter address 51/32; file-reference/all helpers 233/233; append names, force-inline
// and buffer names 51; source step at character read 72; const handle/unsigned conversion capacity 51;
// indexed conversion 63; full path copy 110; aggregate-zero file 53; unsigned append count, entry
// memcpy, enumeration names 51; append success result 54; capacity return 54; counted conversion 229;
// conversion end pointer 58; entry lifetime 51; entry zero initializer 199; file lifetime 51;
// drive-validation return 51; shared character conversion 51; element pointer for the slot 32;
// post-increment index local 51; index then count store 51; found flag with the found arm first
// 51/32; helper parameters swapped 32; force-inline helper 32; count local 44; increment first 44;
// pointer arithmetic slot 32; count reference 32; append as a list method 32; conversion local
// orders 34-36; type/find/data declared at the top 32; NONE-type test before the found arm 71/73;
// format and name locals 32; existing root, path and ASCII helpers 32 (kept); post-copy
// success/early-capacity exits 54/54; wide result 51; list/entry/both references 32/32/32; count plus
// array pointer/reference inputs 32/32; root library clear 207; full character value 39; character
// read at each use 73; earlier ASCII buffer declaration 32; append record value 89; clear/full
// character 209; full character/buffer 39; clear/direct reads/buffer 211; explicit count/publication
// memcpy 51; list increment with old count 51; count success/failure results 54/54; character first
// 32; native file length 131; root aggregate zero 32; direct append with two-use result 32; caller
// append result 32; split helper result declaration 32; enumeration receiver 32; entry and
// enumeration receivers 32; entry receiver 32; explicit diagnostic addresses 32; mutable append input
// and sizeof(*entry) 32; break out of the type loop then `if (type < 11)` 64, diagnostic arm first
// 76; `list->count++` then copy into `entries[count - 1]` 53; own-parameter reference 32; named count
// read once with the new count stored first 51; early `return false` with a named index (everything
// but the setne/test and the ESI count is retail's, 654 bytes) 54; named index then result
// 51; result then named index 51; early return with `entries[count++]` 54; loop flag in the do/while
// condition 51. Harness-only (same allocation as the plain forms): helper in another file, not
// inline, static, generic element-size/count-pointer helpers, nested room/index helpers, struct
// assignment, list/entry references, written-out append, return from the loop.
// frame 0x8ac and extent agree; the append's count/result/list registers (ours
// eax/cl/edx, retail esi/al/ecx with the count copied to eax and the new count stored before the
// copy) and the scratch pairs before the format call, XFindNextSaveGame and XFindClose differ;
// without the format call the enumeration registers are retail's. The append's memcpy into
// `&entries[count++]` gave retail's index copy (51 -> 32). Tried: file path/directory/both
// receivers 51/51/51; conditional character store 69; capacity first 54 (41 once); capacity
// nonzero 51; short type index 189; no parameter address 51/32; file-reference/all helpers
// 233/233; append names/force-inline and buffer names 51; source step at character read 72;
// const handle/unsigned conversion capacity 51; indexed conversion 63; full path copy 110;
// aggregate-zero file 53; unsigned append count/entry memcpy/enumeration names 51; append
// the append's count/result/list registers (ours eax/cl/edx, retail esi/al/ecx,
// new count stored before the copy) and the scratch pairs before the format call and
// XFindNextSaveGame differ; without the format call the enumeration registers are retail's.
// Our length is 659 plus 3 bytes of fill. Deleting the append alone makes the format-call and
// enumeration registers retail's (deleting the dead flag block, the ASCII conversion, the 0x216e50
// call or the path block does not), so the append's allocation sets their rotation; the reduced image
// scores the same. Round 17: break + if (type < 11) append/else diagnostic 64; diagnostic arm first
// 76; increment before the copy into [count - 1] 53; the list read through its own reference 32;
// named count with the new count stored first 51; the found block placed first (same code) 32.
// The append's memcpy into `&entries[count++]` gave retail's index copy (51 -> 32). Tried:
// file path/directory/both receivers 51/51/51; conditional character store 69; capacity first 54;
// capacity nonzero 51; short type index 189; no parameter address 51/32; file-reference/all helpers
// 233/233; append names/force-inline and buffer names 51; source step at character read 72; const
// handle/unsigned conversion capacity 51; indexed conversion 63; full path copy 110; aggregate-zero
// file 53; unsigned append count/entry memcpy/enumeration names 51; append success result 54;
// capacity return 54; counted conversion 229; conversion end pointer 58; entry lifetime 51; entry
// zero initializer 199; file lifetime 51; drive-validation return 51; element pointer for the slot
// 32; post-increment index local 51; index then count store 51; found flag with the found arm first
// orders 34-36; type/find/data declared at the top 32; NONE-type test before the found arm
// 71/73; format and name locals 32; the existing root, path and ASCII helpers in place of their
// written-out bodies 32 (kept).
// diagnostic arguments, append count/result and enumeration registers differ.
// Frame 0x8ac and extent agree; append count/result/list and
// diagnostic/enumeration scratch rotation remain. Post-copy success/early-capacity exits 54/54;
// wide result 51; list/entry/both references 32/32/32; count plus array pointer/reference inputs
// 32/32.
// diagnostic, append and enumeration scratch-register rotation remains; root library clear
// 207f; full character value 39f; character read at each use 73f; earlier ASCII buffer declaration 32f;
// append record value 89f; clear/full character 209f; full character/buffer 39f; clear/direct reads/buffer
// 211f.
// File path/directory/both receivers 51/51/51; conditional character store 69;
// capacity first 54; capacity nonzero 51; short type index 189; no parameter address 51.
// conversion addresses, append list/count/result and final call registers differ.
// Existing root/path helpers 51/51; file-reference/all helpers 233/233;
// append names/force-inline and buffer names 51/51/51; source step at character read 72.
// Earlier: conversion addresses, append and enumeration registers still differ.
// conversion addresses, append and enumeration registers still differ.
// Const handle/unsigned conversion capacity 51; indexed conversion 63; full path copy 110;
// aggregate-zero file 53; unsigned append count/entry memcpy/enumeration names 51.
// matching 0x8ac frame; conversion scratch registers, append count/result homes
// and enumeration argument registers differ. Earlier helper/index/loop forms excluded; tried append
// success result 54; capacity return 54; counted conversion 229; conversion end pointer 58;
// entry lifetime 51; entry zero initializer 199; file lifetime 51; drive-validation return 51;
// shared character conversion 51; element pointer for the slot 32; post-increment index local 51;
// index then count store 51; found flag with the found arm first 51/32; helper parameters
// swapped 32; force-inline helper 32; count local 44; increment first 44; pointer arithmetic slot
// 32; count reference 32; append as a list method 32; conversion local orders 34-36;
// type/find/data declared at the top 32; NONE-type test before the found arm 71/73; format and
// name locals 32; explicit count/publication memcpy 51; list increment with old count 51; count
// success/failure results 54/54; character first 32; native file length 131; root aggregate
// zero 32; post-copy success/early-capacity exits 54/54; wide result 51; list/entry/both
// references 32/32/32; count plus array pointer/reference inputs 32/32; root library clear 207;
// full character value 39; character read at each use 73; earlier ASCII buffer declaration 32;
// append record value 89; clear/full character 209; full character/buffer 39; clear/direct
// reads/buffer 211; count local stored before the copy 51 (retail's order, no count copy);
// that with an element pointer 51; count through a pointer with an index local 51; count local
// with a byte-offset destination 33; index local stored first 51; loop condition on the append's
// result 177. The existing root, path and ASCII helpers replace their written-out bodies (kept).
// shared character conversion 51.
// Earlier: the append's count/result/list registers (ours eax/cl/edx, retail esi/al/ecx,
// new count stored before the copy) and the two scratch pairs before 0x13fb70 and
// XFindNextSaveGame differ. The append's memcpy into `&entries[count++]` gives retail's index copy
// (51 -> 32); an element pointer for the slot 32; post-increment index local 51; index then
// count store 51; found flag with the found arm first 51/32; helper parameters swapped 32;
// force-inline helper 32.
// Explicit count/publication memcpy 51; list increment with old count 51;
// count success/failure results 54/54; capacity first 41; character first 32;
// native file length 131; root aggregate zero 32.
// Round 70: direct append with two-use result 32; caller append result 32; split helper
// result declaration 32; enumeration receiver 32; entry and enumeration receivers 32;
// entry receiver 32; explicit diagnostic addresses 32; mutable append input and
// sizeof(*entry) 32. Original body restored.
// @retail 0x2164c0
void function_2164c0(long memory_unit, s_save_list_2164c0 *list)
{
    (void)&list;
    char drive;
    if (function_1249f0(memory_unit, &drive))
    {
        char root[8] = "";
        function_216400(memory_unit, root, sizeof(root));
        XGAME_FIND_DATA data;
        HANDLE find = XFindFirstSaveGame(root, &data);
        if (find != INVALID_HANDLE_VALUE)
        {
            do
            {
                s_save_entry_2164c0 entry;
                memset(&entry, 0, sizeof(entry));
                strncpy(entry.directory, data.szSaveGameDirectory, sizeof(entry.directory));
                entry.directory[19] = 0;
                long type;
                for (type = 0; type < 11; type++)
                {
                    char path[256];
                    path[0] = 0;
                    function_216d70(path, entry.directory, type);
                    s_storage_file_reference file;
                    memset(&file, 0, sizeof(file));
                    file.signature = FILE_REFERENCE_SIGNATURE;
                    file.location = NONE;
                    if (file.flags & 1)
                    {
                        short length = (short)strlen(file.path);
                        while (length > 0 && file.path[length] != '\\') length--;
                        file.path[length] = 0;
                    }
                    if (*path)
                    {
                        size_t length = strlen(file.path);
                        char *end = file.path + length;
                        if (end != file.path && end[-1] != '\\')
                        {
                            *end++ = '\\';
                            *end = 0;
                            length++;
                        }
                        strncpy(end, path, sizeof(file.path) - length);
                        file.path[255] = 0;
                    }
                    file.flags |= 1;
                    if (function_1368f0((s_type_acf665 *)&file))
                        goto found;
                }
                {
                    word description[256];
                    unicode_string_snprintf(description, 256, L"unrecognized saved game file found by XFindNextSaveGame(): display name= '%s' path= '%hs'", data.szSaveGameName, data.wfd.cFileName);
                    char text[256];
                    unicode_string_to_ascii(description, text, 256);
                }
                goto next;
found:
                {
                    entry.type = type;
                    function_216e50(data.szSaveGameName, type, &entry.language, entry.name);
                    if (!append_save_entry_2164c0(list, &entry))
                        break;
                }
next:;
            } while (XFindNextSaveGame(find, &data));
            XFindClose(find);
        }
    }
}
