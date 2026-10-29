// @flags /O2 /GL-
#include "contact_storage_318f10.h"

struct contact_result_318fb0
{
	s_contact_point_318f10 point;
	unsigned short id;
};
struct contact_output_318fb0
{
	contact_result_318fb0 const *data;
	long size;
};

// Refresh persistent points through the id map, then notify listeners when
// the per-pair delay expires.
// @retail 0x318fb0
void s_contact_storage_318f10::processed(lane_a_contact_body const *first,
	lane_a_contact_body const *second, void const *, void const *input)
{
	contact_output_318fb0 const *output = *(contact_output_318fb0 const * const *)input;
	contact_result_318fb0 const *entry = output->data;
	for (long i = output->size - 1; i >= 0; --i, ++entry)
	{
		s_contact_point_318f10 &point = points[ids.data[entry->id]];
		point.position = entry->point.position;
		point.normal = entry->point.normal;
	}
	if (--remaining == 0)
	{
		lane_a_contact_entity *entity_a = first->entity;
		lane_a_contact_entity *entity_b = second->entity;
		unsigned short delay = lane_a_contact_delay(entity_b);
		if (lane_a_contact_delay(entity_a) < delay)
			delay = lane_a_contact_delay(entity_a);
		remaining = delay + 1;
		event_315d20 event;
		lane_a_contact_processed_payload payload = {this, first, second, input};
		memcpy(event.unknown0,&payload.manager,4);
		memcpy(event.unknown0+4,&payload.a,8);
		memcpy(event.unknown0+12,&payload.input,4);
		function_318440(world,&event);
		if (entity_a->listeners_60.size)
			function_315e00((listener_world_315d20 *)entity_a,&event);
		if (entity_b->listeners_60.size)
			function_315e00((listener_world_315d20 *)entity_b,&event);
	}
}
