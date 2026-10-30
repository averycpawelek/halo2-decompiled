// @flags /O2 /GL-
#include "contact_constraint_315510.h"

// The shared contact constructor is outside this batch; derived constructors
// retain its retail call and standard thiscall convention.
// @stub 0x31c000
contact_constraint_315510::contact_constraint_315510() : lane_a_constraint(0) {}
