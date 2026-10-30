// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_2ACCD0.CPP: saved game files on the Xbox hard disk: signed reads
   and writes run as asynchronous tasks, and copying files */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "unknown_2accd0.h"
#include "async.h"
#include "language.h"

/* the location and type of a saved game file (0x40 bytes) */
struct s_saved_game_file_location
{
	char name[0x14];
	wchar_t display_name[0x12];
	long type;
	byte unknown3c;
	char language;
	byte unknown3e[2];
};

struct s_saved_game_file_id
{
	char data[0x14];
};

enum
{
	_saved_game_file_copy_create = 0,
	_saved_game_file_copy_open,
	_saved_game_file_copy_read,
	_saved_game_file_copy_write,
	_saved_game_file_copy_finish,
	_saved_game_file_copy_failed,
	_saved_game_file_copy_done
};

/* a saved game file (0xa0 bytes), and the state of copying it in chunks */
struct s_saved_game_file
{
	long flags;
	s_saved_game_file_location location;
	wchar_t display_name[0x11];
	s_saved_game_file_id id;
	bool unknown7a;
	byte unknown7b;
	bool temporary;
	byte unknown7d[3];
	bool *done;
	HANDLE source;
	HANDLE destination;
	void *buffer;
	dword size;
	dword read_offset;
	dword write_offset;
	long copy_state;
};

struct s_saved_game_file_read_parameters
{
	void *buffer;
	dword size;
	bool non_roamable;
	s_saved_game_file_task *task;
};

#define FILE_COPY_CHUNK_SIZE 0x20000
#define MIN(a, b) ((a) > (b) ? (b) : (a))

const char *saved_game_type_get_storage_name(long type);
long function_217480(long type);
char *function_122810(char *path, const char *name);
char *function_11c9c0(char *buffer, long maximum_count, const char *format, ...);
bool function_1249f0(long memory_unit, char *drive_letter);
bool function_1367d0(s_type_acf665 *file);
bool file_ref_truncate(s_type_acf665 *file, dword position);
bool function_216da0(long type, wchar_t *name, const wchar_t *display_name, long language);
bool saved_game_file_write_save_image(long type, s_saved_game_file_location *location);
bool function_2168b0(s_saved_game_file_location *location, long flags);

/* the root of the memory unit a saved game file is on, or the hard disk's */
inline void saved_game_file_get_root(const s_saved_game_file *file, char *root)
{
	char drive_letter;
	if (function_1249f0((file->flags >> 4) & 0xf, &drive_letter))
	{
		function_11c9c0(root, 8, "%c:\\", drive_letter);
	}
	else
	{
		root[0] = 0;
	}
}

void saved_game_file_read(s_type_acf665 *file, void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task);
void saved_game_file_write(s_type_acf665 *file, void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task);

inline char *function_x91aa57(char *destination, const char *source, dword size)
{
	strncpy(destination, source, size);
	destination[size - 1] = 0;
	return destination;
}

inline void function_x454397(s_type_acf665 *file)
{
	memset(file, 0, sizeof(*file));
	file->signature = FILE_REFERENCE_SIGNATURE;
	file->location = NONE;
}

inline void function_x73bce5(s_type_acf665 *file, const char *name)
{
	if (file->flags & 1)
	{
		function_1373c0(file->path);
	}
	function_137320(file->path, name);
	file->flags |= 1;
}

inline void function_xbb02d2(s_type_acf665 *file, const char *path)
{
	function_x454397(file);
	function_x73bce5(file, path);
}

/* Bungie's argument order */
inline bool function_xe8ca68(s_type_acf665 *file, dword size, const void *buffer)
{
	return function_136d00(file, size, buffer);
}

inline bool function_xab2909(s_type_acf665 *file, dword size, bool silent, void *buffer)
{
	return file_ref_read(file, size, buffer, silent);
}

inline bool function_x8231f3(s_type_acf665 *file)
{
	bool success = false;
	if (CloseHandle(file->handle))
	{
		file->handle = 0;
		file->position = 0;
		success = true;
	}
	else
	{
		GetLastError();
		SetLastError(0);
	}
	return success;
}

// @retail 0x2accd0
PRIVATE long __stdcall saved_game_file_read_work(s_async_task *task, void *parameters_view, long parameters_size)
{
	s_saved_game_file_read_parameters *parameters = (s_saved_game_file_read_parameters *)parameters_view;
	s_type_acf665 file;

	parameters->task->succeeded = false;
	parameters->task->state = 4;
	function_xbb02d2(&file, parameters->task->path);
	saved_game_file_read(&file, parameters->buffer, parameters->size, parameters->non_roamable, parameters->task);
	return 1;
}

// @retail 0x2acd60
bool saved_game_file_read_begin(void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task, const char *path)
{
	s_saved_game_file_read_parameters parameters;

	parameters.buffer = buffer;
	char *task_path = task->path;
	bool *pending = &task->unknown1;
	*pending = false;
	parameters.size = size;
	parameters.non_roamable = non_roamable;
	parameters.task = task;
	task->succeeded = false;
	task->progress = -1.0f;
	function_x91aa57(task_path, path, sizeof(task->path));
	return async_task_add_work(saved_game_file_read_work, sizeof(parameters), &parameters, 2, &task->done) != NONE;
}

bool saved_game_file_copy_update(s_saved_game_file *file, long *error);
void saved_game_file_get_path(const s_saved_game_file *file, char *path);

/* a task copying a saved game file (kept in the status block in place of
   its path) to the hard disk, then writing it */
inline s_saved_game_file *saved_game_file_task_get_file(s_saved_game_file_task *task)
{
	return (s_saved_game_file *)task->path;
}

// @retail 0x2acde0
PRIVATE long __stdcall saved_game_file_copy_work(s_async_task *task, void *parameters_view, long parameters_size)
{
	s_saved_game_file_read_parameters *parameters = (s_saved_game_file_read_parameters *)parameters_view;
	s_saved_game_file *file = saved_game_file_task_get_file(parameters->task);
	long result = 0;
	long error;

	if (saved_game_file_copy_update(file, &error))
	{
		result = 1;
		if (!error)
		{
			char path[0x100];
			s_type_acf665 s_type_acf665;

			parameters->task->state = 4;
			path[0] = 0;
			saved_game_file_get_path(file, path);
			function_x454397(&s_type_acf665);
			function_x73bce5(&s_type_acf665, path);
			saved_game_file_write(&s_type_acf665, parameters->buffer, parameters->size, parameters->non_roamable, parameters->task);
		}
	}
	else if (error)
	{
		result = 1;
		parameters->task->state = 0;
	}
	return result;
}

// @retail 0x2acee0
bool saved_game_file_copy_begin(void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task)
{
	s_saved_game_file_read_parameters parameters;

	parameters.buffer = buffer;
	parameters.size = size;
	parameters.non_roamable = non_roamable;
	parameters.task = task;
	task->unknown1 = false;
	task->succeeded = false;
	task->progress = -1.0f;
	return async_task_add_work(saved_game_file_copy_work, sizeof(parameters), &parameters, 2, &task->done) != NONE;
}
// @retail 0x2acf40
s_saved_game_file *saved_game_file_new(s_saved_game_file *file, long flags, const s_saved_game_file_location *location)
{
	file->flags = flags;
	s_saved_game_file_location *file_location = &file->location;
	file_location->name[0] = 0;
	file_location->display_name[0] = 0;
	wchar_t *display_name = file->display_name;
	*display_name = 0;
	file->id.data[0] = 0;
	file->unknown7a = false;
	file->temporary = false;
	file->done = NULL;
	file->buffer = NULL;
	file->copy_state = _saved_game_file_copy_done;
	*file_location = *location;
	return file;
}

__forceinline void saved_game_file_copy_display_name(wchar_t *destination, wchar_t const *source)
{
	wcsncpy(destination, source, 0x10);
	destination[0x10] = 0;
}

// @retail 0x2acf80
s_saved_game_file *saved_game_file_new_from_id(s_saved_game_file *file, long flags, const wchar_t *display_name, const s_saved_game_file_location *location, void *buffer, bool *done)
{
	const s_saved_game_file_location *const *location_reference = &location;
	file->flags = flags;
	s_saved_game_file_location *file_location = &file->location;
	memset(file_location->name, 0, sizeof(file_location->name[0]));
	memset(file_location->display_name, 0, sizeof(file_location->display_name[0]));
	saved_game_file_copy_display_name(file->display_name, display_name);
	file->id = *(const s_saved_game_file_id *)*location_reference;
	file->done = done;
	file->temporary = false;
	file->source = INVALID_HANDLE_VALUE;
	file->destination = INVALID_HANDLE_VALUE;
	file->size = NONE;
	file->read_offset = 0;
	file->write_offset = 0;
	file->copy_state = _saved_game_file_copy_create;
	file->unknown7a = true;
	file->buffer = buffer;
	*file_location = **location_reference;
	return file;
}

/* creates the saved game's directory on the file's device */
__forceinline bool saved_game_file_create_directory(const s_saved_game_file *file, const wchar_t *name, char *path)
{
	char root[8];

	saved_game_file_get_root(file, root);
	return XCreateSaveGame(root, name, CREATE_NEW, 0, path, 0x100) == ERROR_SUCCESS;
}

// @retail 0x2ad020
bool saved_game_file_copy_create(s_saved_game_file *file)
{
	bool result = false;
	long type = file->flags & 0xf;
	long language = get_current_language();
	char path[0x100] = { 0 };
	wchar_t name[0x80];
	char file_path[0x100];
	s_saved_game_file_id id;
	s_type_acf665 s_type_acf665;

	name[0] = 0;
	function_216da0(type, name, file->display_name, language);
	if (saved_game_file_create_directory(file, name, path))
	{
		dword error;

		file_path[0] = 0;
		function_x91aa57(id.data, path, sizeof(id.data));
		function_x91aa57(file_path, id.data, sizeof(file_path));
		function_122810(file_path, saved_game_type_get_storage_name(type));
		function_xbb02d2(&s_type_acf665, file_path);
		if (function_1367d0(&s_type_acf665) && function_136970(&s_type_acf665, 2, &error))
		{
			if (file_ref_truncate(&s_type_acf665, function_217480(type)))
			{
				*(s_saved_game_file_id *)file->location.name = id;
				result = true;
			}
			function_136bb0(&s_type_acf665);
		}
	}
	return result;
}
// @retail 0x2ad380
bool file_copy_read_chunk(s_saved_game_file *copy)
{
	bool result = false;
	dword offset = copy->read_offset;
	dword remaining = copy->size - offset;
	dword size = remaining < FILE_COPY_CHUNK_SIZE ? remaining : FILE_COPY_CHUNK_SIZE;
	dword bytes;

	if (SetFilePointer(copy->source, offset, NULL, FILE_BEGIN) == copy->read_offset)
	{
		if (ReadFile(copy->source, copy->buffer, size, &bytes, NULL) && size == bytes)
		{
			result = true;
			copy->read_offset += bytes;
		}
	}
	return result;
}

// @retail 0x2ad400
bool file_copy_write_chunk(s_saved_game_file *copy)
{
	bool result = false;
	dword offset = copy->write_offset;
	dword remaining = copy->size - offset;
	dword size = remaining < FILE_COPY_CHUNK_SIZE ? remaining : FILE_COPY_CHUNK_SIZE;
	dword bytes;

	if (SetFilePointer(copy->destination, offset, NULL, FILE_BEGIN) == copy->write_offset)
	{
		if (WriteFile(copy->destination, copy->buffer, size, &bytes, NULL) && size == bytes)
		{
			result = true;
			copy->write_offset += bytes;
		}
	}
	return result;
}

// @retail 0x2ad220
bool file_copy_open(s_saved_game_file *copy, bool *exists)
{
	bool result = false;
	char source_path[0x100];
	char destination_path[0x100];

	function_x91aa57(source_path, copy->id.data, sizeof(source_path));
	function_122810(source_path, "auxilary.bin");
	copy->source = CreateFileA(source_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (copy->source == INVALID_HANDLE_VALUE)
	{
		*exists = false;
		result = true;
		goto done;
	}

	function_x91aa57(destination_path, copy->location.name, sizeof(destination_path));
	function_122810(destination_path, "auxilary.bin");
	copy->destination = CreateFileA(destination_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (copy->destination != INVALID_HANDLE_VALUE)
	{
		copy->size = GetFileSize(copy->source, NULL);
		if (copy->size != INVALID_FILE_SIZE)
		{
			SetFilePointer(copy->destination, copy->size, NULL, FILE_BEGIN);
			if (GetLastError() == ERROR_SUCCESS && SetEndOfFile(copy->destination))
			{
				*exists = true;
				result = true;
				goto done;
			}
		}
	}

	if (copy->source != INVALID_HANDLE_VALUE)
	{
		CloseHandle(copy->source);
		copy->source = INVALID_HANDLE_VALUE;
	}
	if (copy->destination != INVALID_HANDLE_VALUE)
	{
		CloseHandle(copy->destination);
		copy->destination = INVALID_HANDLE_VALUE;
	}
done:
	return result;
}
// score 19 (was 19): the second argument of the save call is a pointer held in a register here and
// the address of the local buffer in retail.
// @retail 0x2ad480
void saved_game_file_copy_finish(s_saved_game_file *file, bool succeeded)
{
	long type = file->flags & 0xf;
	char root[8];
	wchar_t name[0x80];

	saved_game_file_get_root(file, root);
	name[0] = 0;
	if (succeeded)
	{
		function_216da0(type, name, file->location.display_name, file->location.language);
		XDeleteSaveGame(root, name);
		saved_game_file_copy_display_name(file->location.display_name, file->display_name);
		file->location.language = (char)get_current_language();
		function_2168b0(&file->location, file->flags);
	}
	else
	{
		function_216da0(type, name, file->display_name, get_current_language());
		XDeleteSaveGame(root, name);
	}
	*file->done = true;
}
// @retail 0x2ad5b0
void file_copy_close(s_saved_game_file *copy)
{
	if (copy->source != INVALID_HANDLE_VALUE)
	{
		CloseHandle(copy->source);
		copy->source = INVALID_HANDLE_VALUE;
	}
	if (copy->destination)
	{
		CloseHandle(copy->destination);
		copy->source = INVALID_HANDLE_VALUE;
	}
}

// @retail 0x2ad5f0
bool saved_game_file_copy_update(s_saved_game_file *file, long *error)
{
	bool result = false;
	bool exists;
	long type;

	*error = 0;
	switch (file->copy_state)
	{
	case _saved_game_file_copy_create:
		if (saved_game_file_copy_create(file))
			file->copy_state = _saved_game_file_copy_finish;
		else
			*error = 2;
		break;

	case _saved_game_file_copy_finish:
		type = file->flags & 0xf;
		if (saved_game_file_write_save_image(type, &file->location))
			file->copy_state = _saved_game_file_copy_open;
		else
			*error = 2;
		break;

	case _saved_game_file_copy_open:
		exists = false;
		if (file_copy_open(file, &exists))
		{
			if (exists)
			{
				file->read_offset = 0;
				file->write_offset = 0;
				file->copy_state = _saved_game_file_copy_read;
			}
			else
			{
				file->copy_state = _saved_game_file_copy_failed;
			}
		}
		else
		{
			*error = 2;
		}
		break;

	case _saved_game_file_copy_read:
		if (file_copy_read_chunk(file))
			file->copy_state = _saved_game_file_copy_write;
		else
			*error = 4;
		break;

	case _saved_game_file_copy_write:
		if (file_copy_write_chunk(file))
		{
			if (file->size > file->write_offset)
			{
				file->copy_state = _saved_game_file_copy_read;
			}
			else
			{
				file_copy_close(file);
				file->copy_state = _saved_game_file_copy_failed;
			}
		}
		else
		{
			*error = 4;
		}
		break;

	case _saved_game_file_copy_failed:
		saved_game_file_copy_finish(file, true);
		file->copy_state = _saved_game_file_copy_done;
		break;

	case _saved_game_file_copy_done:
		result = true;
		break;
	}

	if (*error)
	{
		file_copy_close(file);
		saved_game_file_copy_finish(file, false);
	}
	return result;
}
inline void saved_game_file_build_path(char *path, const char *folder, long type)
{
	strncpy(path, folder, 0x100);
	path[0xff] = 0;
	function_122810(path, saved_game_type_get_storage_name(type));
}

// @retail 0x2ad760
void saved_game_file_get_path(const s_saved_game_file *file, char *path)
{
	if (file->temporary)
	{
		strncpy(path, "t:\\blam", 0x100);
		path[0xff] = 0;
	}
	else
	{
		saved_game_file_build_path(path, file->location.name, file->location.type);
	}
}

bool g_5020d4;

enum
{
	_signed_file_read_header = 0,
	_signed_file_read_body,
	_signed_file_read_signature
};

/* a signed saved game file read in steps: a header, then a body, then the
   signature after both */
#pragma pack(push, 1)
struct s_signed_file_read_parameters
{
	void *header;
	dword header_size;
	byte *body;
	dword body_size;
	HANDLE file;
	dword body_offset;
	HANDLE signature_handle;
	s_saved_game_file_task *task;
	bool non_roamable;
	char state;
};
#pragma pack(pop)

__forceinline byte *job_thread_buffer_get(s_async_task *task, dword *size);

// score 68 (was 68): header/body carriers, dispatch loads, scratch capacity and signature
// receiver differ. Retail computes the remaining size in each buffer arm (re-read after the
// scratch-buffer stores) and clamps the capacity by it, and loads body_size once in the switch
// dispatch for the body and signature cases. Tried: short-lived body/header/both size reads
// 182/174/182; reversed signature equality 68; Boolean equality 68; inline depth one 68; global
// optimization off 491; frame pointer 246; per-arm remaining written into size by the getter
// 179, capacity clamped by remaining 181, remaining clamped by capacity 234; total as sizeof
// first 68; body size grouped with sizeof 68. Round 11: with the scratch object made one
// address-taken record (storage, owner, pointer, size, flag; init `pointer = storage`, as
// retail's 0x4fa0c8..0x5020d4 layout shows) the body is 173 and 0x2adb10 134 -> 131; adding
// the per-arm remaining 175, the getter storing the owner before the flag 175 (0x2adb10 129),
// and the signature state as its own case with `default: __assume(0)` 166 (aligned 54 -> 37:
// signature case and getter now retail's) but two instructions longer: case 1 reloads
// body_size where retail reuses the dispatch load, and the buffer goes eax -> ebp; size sums
// and the case-1 comparison in either order 166-167; getter fresh pointer reads 163 (0x2adb10
// 279).
// header/body carriers, dispatch loads, scratch capacity and
// signature receiver differ; short-lived body/header/both size reads 182/174/182;
// reversed signature equality 68; Boolean equality 68; inline depth one 68;
// global optimization off 491; frame pointer 246.
// retail computes the remaining size in each buffer arm (re-read
// after the scratch-buffer stores) and clamps the capacity by it; that per-arm form
// loses the entry ebp save: written into size by the getter 179, capacity clamped
// by remaining 181, remaining clamped by capacity 234. Total as sizeof first 68;
// body size grouped with sizeof 68.
// Round 402: current-graph shared byte count, seek-position home, both, full signature
// outputs, count/signatures, position/signatures, all outputs and role-named signatures
// 68 each; all restored. Frame 0x2c agrees; dispatch/buffer and signature registers remain.
// Retail's reloads after the getter's stores come back with the scratch object's 0x8000-byte
// storage as its first member (address taken by `pointer = storage`); with the per-arm
// remaining size as well 175 (aligned 50 against 51: entry sum and dispatch body-size load,
// size/position homes in esi/edi swapped), without it 173; per-arm form alone 179; body size
// first in the total/position sums 175 (same code); size-first body test 176; getter owner
// store before the flag 176.
// @retail 0x2ad7c0
PRIVATE long __stdcall signed_file_read_work(s_async_task *task, s_signed_file_read_parameters *parameters, long parameters_size)
{
	dword total = parameters->header_size + parameters->body_size + sizeof(XCALCSIG_SIGNATURE);
	real total_size = (real)total;

	parameters->task->state = 4;
	if (!parameters->task->unknown1)
	{
		switch (parameters->state)
		{
		case _signed_file_read_header:
		{
			dword bytes;
			parameters->file = CreateFileA(parameters->task->path, GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
			if (parameters->file != INVALID_HANDLE_VALUE &&
				ReadFile(parameters->file, parameters->header, parameters->header_size, &bytes, NULL) &&
				bytes == parameters->header_size)
			{
				parameters->signature_handle = XCalculateSignatureBegin(parameters->non_roamable ? XCALCSIG_FLAG_NON_ROAMABLE : 0);
				if (parameters->signature_handle != INVALID_HANDLE_VALUE &&
					(!parameters->header_size || XCalculateSignatureUpdate(parameters->signature_handle, (const BYTE *)parameters->header, parameters->header_size) == ERROR_SUCCESS))
				{
					parameters->task->progress = (real)parameters->header_size / total_size;
					parameters->state = _signed_file_read_body;
					return 0;
				}
			}
			break;
		}

		case _signed_file_read_body:
			if (parameters->body_offset < parameters->body_size)
			{
				byte *buffer;
				dword size = parameters->body_size - parameters->body_offset;
				dword bytes;
				if (parameters->body)
				{
					buffer = parameters->body + parameters->body_offset;
				}
				else
				{
					dword available;
					buffer = job_thread_buffer_get(task, &available);
					if (size > available)
					{
						size = available;
					}
				}

				dword position = parameters->header_size + parameters->body_offset;
				if (SetFilePointer(parameters->file, position, NULL, FILE_BEGIN) == position &&
					ReadFile(parameters->file, buffer, size, &bytes, NULL) && bytes == size &&
					XCalculateSignatureUpdate(parameters->signature_handle, buffer, bytes) == ERROR_SUCCESS)
				{
					parameters->body_offset += bytes;
					if (parameters->body_offset >= parameters->body_size)
					{
						parameters->state = _signed_file_read_signature;
					}
					parameters->task->progress = (real)(parameters->header_size + parameters->body_offset) / total_size;
					return 0;
				}
				break;
			}
			parameters->state = _signed_file_read_signature;
			return 0;

		default:
		{
			dword position = parameters->header_size + parameters->body_size;
			XCALCSIG_SIGNATURE stored;
			XCALCSIG_SIGNATURE computed;
			dword bytes;

			if (SetFilePointer(parameters->file, position, NULL, FILE_BEGIN) == position &&
				ReadFile(parameters->file, &stored, sizeof(stored), &bytes, NULL) && bytes == sizeof(stored))
			{
				if (XCalculateSignatureEnd(parameters->signature_handle, &computed) == ERROR_SUCCESS)
				{
					s_saved_game_file_task *output = parameters->task;
					if (memcmp(&computed, &stored, sizeof(stored)) == 0)
						output->succeeded = true;
					else
						output->succeeded = false;
				}
				parameters->signature_handle = INVALID_HANDLE_VALUE;
			}
			parameters->task->progress = 1.0f;
			break;
		}
		}
	}

	if (parameters->file != INVALID_HANDLE_VALUE)
	{
		CloseHandle(parameters->file);
	}
	if (parameters->signature_handle != INVALID_HANDLE_VALUE)
	{
		XCalculateSignatureEnd(parameters->signature_handle, NULL);
	}
	return 1;
}

// @retail 0x2ada70
bool signed_file_read_begin(const char *path, void *header, dword header_size, void *body, dword body_size, bool non_roamable, s_saved_game_file_task *task)
{
	s_signed_file_read_parameters parameters;

	*(volatile bool *)&task->unknown1 = false;
	*(volatile bool *)&task->succeeded = false;
	task->progress = 0.0f;
	task->state = 4;
	function_x91aa57(task->path, path, sizeof(task->path));
	parameters.header = header;
	parameters.header_size = header_size;
	parameters.body = (byte *)body;
	parameters.body_size = body_size;
	parameters.file = INVALID_HANDLE_VALUE;
	parameters.body_offset = 0;
	parameters.signature_handle = INVALID_HANDLE_VALUE;
	parameters.task = task;
	parameters.non_roamable = non_roamable;
	parameters.state = _signed_file_read_header;
	return async_task_add_work((async_task_work_function)signed_file_read_work, sizeof(parameters), &parameters, 2, &task->done) != NONE;
}

/* the job thread's scratch buffer, claimed by a task */
__forceinline byte *job_thread_buffer_get(s_async_task *task, dword *size)
{
	byte *buffer = (byte *)g_5020c8.unknown04;
	g_5020d4 = buffer != (byte *)0x4fa0c8;
	*size = g_5020c8.unknown08;
	g_5020c8.unknown00 = (dword)task;
	return buffer;
}

enum
{
	_signed_file_write_header = 0,
	_signed_file_write_extend,
	_signed_file_write_body,
	_signed_file_write_signature
};

/* a signed saved game file write in steps; without a header it only makes
   sure the file has its full size */
#pragma pack(push, 1)
struct s_signed_file_write_parameters
{
	long state;
	void *header;
	dword header_size;
	void *body;
	dword body_size;
	HANDLE file;
	HANDLE signature_handle;
	s_saved_game_file_task *task;
	bool non_roamable;
	bool no_header;
};
#pragma pack(pop)

// score 134 (was 134): retail keeps every `parameters` load after the scratch getter's
// stores, as if the scratch object were address-taken (its 0x8000-byte storage at
// 0x4fa0c8 sits right before the owner/pointer/size words and the flag byte); ours
// hoists the loads. Storage as the object's first member, init `pointer = storage`
// 132; flag byte folded in as well 131 (0x2ad7c0 173); then the getter as size,
// owner, flag returning a fresh read of the pointer: later cases' registers all
// retail's but two copies per getter site, 279 at 944 bytes (0x2ad7c0 63); split
// size/claim helpers 131/139; write count declared before the getter 147; named
// body offset in the extend sum 131.
// DL180: case order 2013 324; case order 3012 331; case order 0213 192; case order 0312 273; case
// order 1203 313; case order 2103 314; case order 2301 332; case order 3210 328. Original
// executable body restored.
// @retail 0x2adb10
PRIVATE long __stdcall signed_file_write_work(s_async_task *task, s_signed_file_write_parameters *parameters, long parameters_size)
{
	long result = 1;

	parameters->task->state = 4;
	switch (parameters->state)
	{
	case _signed_file_write_header:
		parameters->file = CreateFileA(parameters->task->path, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
		if (parameters->file != INVALID_HANDLE_VALUE)
		{
			if (parameters->no_header)
			{
				dword file_size = GetFileSize(parameters->file, NULL);
				if (file_size != INVALID_FILE_SIZE)
				{
					if (file_size < parameters->body_size + parameters->header_size + sizeof(XCALCSIG_SIGNATURE))
					{
						parameters->state = _signed_file_write_extend;
					}
					else
					{
						parameters->task->state = 0;
						parameters->task->succeeded = true;
						break;
					}
				}
			}
			else
			{
				parameters->signature_handle = XCalculateSignatureBegin(parameters->non_roamable ? XCALCSIG_FLAG_NON_ROAMABLE : 0);
				if (parameters->signature_handle != INVALID_HANDLE_VALUE &&
					XCalculateSignatureUpdate(parameters->signature_handle, (const BYTE *)parameters->header, parameters->header_size) == ERROR_SUCCESS)
				{
					parameters->state = _signed_file_write_extend;
				}
			}

			dword buffer_size;
			byte *buffer = job_thread_buffer_get(task, &buffer_size);
			memset(buffer, 0, MIN(buffer_size, parameters->header_size));

			bool success = true;
			for (dword offset = 0; offset < parameters->header_size && success; offset += buffer_size)
			{
				dword size = MIN(parameters->header_size - offset, buffer_size);
				dword bytes;
				success = SetFilePointer(parameters->file, offset, NULL, FILE_BEGIN) == offset &&
					WriteFile(parameters->file, buffer, size, &bytes, NULL) && bytes == size;
			}
			if (success)
			{
				result = 0;
			}
		}
		break;

	case _signed_file_write_extend:
		if (parameters->no_header)
		{
			dword buffer_size;
			byte *buffer = job_thread_buffer_get(task, &buffer_size);
			long offset = parameters->body_size - buffer_size + parameters->header_size;
			offset = offset > 0 ? offset : 0;
			memset(buffer, 0, buffer_size);
			dword size = MIN(parameters->body_size - offset + parameters->header_size, buffer_size);
			dword bytes;
			if (SetFilePointer(parameters->file, offset, NULL, FILE_BEGIN) == offset &&
				WriteFile(parameters->file, buffer, size, &bytes, NULL) && size == bytes)
			{
				parameters->state = _signed_file_write_body;
				result = 0;
			}
		}
		else
		{
			dword position = parameters->header_size;
			dword body_size = parameters->body_size;
			const BYTE *body = (const BYTE *)parameters->body;
			dword bytes;
			if (SetFilePointer(parameters->file, position, NULL, FILE_BEGIN) == position &&
				WriteFile(parameters->file, body, body_size, &bytes, NULL) && bytes == body_size &&
				XCalculateSignatureUpdate(parameters->signature_handle, body, bytes) == ERROR_SUCCESS)
			{
				parameters->state = _signed_file_write_body;
				result = 0;
			}
		}
		break;

	case _signed_file_write_body:
	{
		dword bytes;
		if (parameters->no_header ||
			SetFilePointer(parameters->file, 0, NULL, FILE_BEGIN) == 0 &&
			WriteFile(parameters->file, parameters->header, parameters->header_size, &bytes, NULL) && bytes == parameters->header_size)
		{
			parameters->state = _signed_file_write_signature;
			result = 0;
		}
		break;
	}

	case _signed_file_write_signature:
	{
		XCALCSIG_SIGNATURE signature;
		dword bytes;
		bool success = true;
		if (parameters->no_header)
		{
			memset(&signature, 0, sizeof(signature));
		}
		else
		{
			success = XCalculateSignatureEnd(parameters->signature_handle, &signature) == ERROR_SUCCESS;
			parameters->signature_handle = INVALID_HANDLE_VALUE;
		}
		dword position = parameters->header_size + parameters->body_size;
		if (success &&
			SetFilePointer(parameters->file, position, NULL, FILE_BEGIN) == position &&
			WriteFile(parameters->file, &signature, sizeof(signature), &bytes, NULL) && bytes == sizeof(signature))
		{
			parameters->task->state = 0;
			parameters->task->succeeded = true;
		}
		break;
	}

	default:
		__assume(0);
	}

	if (result)
	{
		if (parameters->file != INVALID_HANDLE_VALUE)
		{
			CloseHandle(parameters->file);
		}
		if (parameters->signature_handle != INVALID_HANDLE_VALUE)
		{
			XCalculateSignatureEnd(parameters->signature_handle, NULL);
		}
	}
	return result;
}

// @retail 0x2adec0
bool signed_file_write_begin(const char *path, void *header, dword header_size, void *body, dword body_size, bool non_roamable, s_saved_game_file_task *task)
{
	s_signed_file_write_parameters parameters;

	*(volatile bool *)&task->unknown1 = false;
	*(volatile bool *)&task->succeeded = false;
	task->progress = -1.0f;
	task->state = 4;
	function_x91aa57(task->path, path, sizeof(task->path));
	parameters.state = _signed_file_write_header;
	parameters.header = header;
	parameters.header_size = header_size;
	parameters.body = body;
	parameters.body_size = body_size;
	parameters.file = INVALID_HANDLE_VALUE;
	parameters.signature_handle = INVALID_HANDLE_VALUE;
	parameters.task = task;
	parameters.non_roamable = non_roamable;
	parameters.no_header = header == NULL;
	return async_task_add_work((async_task_work_function)signed_file_write_work, sizeof(parameters), &parameters, 2, &task->done) != NONE;
}

// @retail 0x2adf70
void saved_game_file_read(s_type_acf665 *file, void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task)
{
	dword error;

	if (function_136970(file, 0x11, &error))
	{
		task->state = 3;
		if (function_xab2909(file, size, true, buffer))
		{
			HANDLE signature_handle = XCalculateSignatureBegin(non_roamable ? XCALCSIG_FLAG_NON_ROAMABLE : 0);
			if (signature_handle != INVALID_HANDLE_VALUE)
			{
				XCALCSIG_SIGNATURE computed;
				XCALCSIG_SIGNATURE stored;

				XCalculateSignatureUpdate(signature_handle, (const BYTE *)buffer, size);
				if (XCalculateSignatureEnd(signature_handle, &computed) == ERROR_SUCCESS &&
					file_ref_read_at(file, size, sizeof(stored), false, &stored) &&
					memcmp(&computed, &stored, sizeof(stored)) == 0)
				{
					task->succeeded = true;
					task->state = 0;
				}
			}
		}
		function_x8231f3(file);
	}
}

// @retail 0x2ae090
void saved_game_file_write(s_type_acf665 *file, void *buffer, dword size, bool non_roamable, s_saved_game_file_task *task)
{
	dword error;

	if (function_136970(file, 2, &error))
	{
		task->state = 2;
		if (function_xe8ca68(file, size, buffer))
		{
			HANDLE signature_handle = XCalculateSignatureBegin(non_roamable ? XCALCSIG_FLAG_NON_ROAMABLE : 0);
			if (signature_handle != INVALID_HANDLE_VALUE)
			{
				XCALCSIG_SIGNATURE signature;

				XCalculateSignatureUpdate(signature_handle, (const BYTE *)buffer, size);
				if (XCalculateSignatureEnd(signature_handle, &signature) == ERROR_SUCCESS &&
					file_ref_write_at(file, size, sizeof(signature), &signature))
				{
					task->succeeded = true;
					task->state = 0;
				}
			}
		}
		function_x8231f3(file);
	}
}
