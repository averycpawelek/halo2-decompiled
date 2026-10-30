#ifndef CONTACT_MANAGER_318C10_H
#define CONTACT_MANAGER_318C10_H
#include "lane_a_contact_manager.h"
#include "lane_b_constraint_leaves.h"

class c_1d1261;
class class_318e60 : public reference_base_318970
{
public:
	c_1d1261 *world;
	long unknownc;
	lane_b_contact_constraint_prefix constraint;
	virtual unsigned short add(lane_a_contact_body const *, lane_a_contact_body const *,
		void const *, lane_b_contact_point const *);
	virtual void remove(unsigned short id);
};
#endif
