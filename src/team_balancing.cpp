// @flags /O2 /Ob1 /arch:SSE /Gr
/* TEAM_BALANCING.CPP: the assignment of players to teams. Players (with a
   weight) are grouped into parties, parties are dealt to the team with the
   fewest players, and pairs of parties are then swapped between teams for as
   long as that makes the teams more even. */

#include "unknown_11c920.h"
#include <string.h>
#include <new>

#define MAXIMUM_PLAYERS 16
#define MAXIMUM_PARTIES 16
#define MAXIMUM_TEAMS 4

struct s_balance_player
{
	long weight;
	long index;

	s_balance_player();
	bool has_index(long player_index) const
	{
		bool result = false;
		if (index == player_index)
			result = true;
		return result;
	}
};

struct s_balance_party
{
	bool flagged;
	long id;
	long count;
	long total;
	s_balance_player players[MAXIMUM_PLAYERS];

	s_balance_party();
	void reset()
	{
		count = 0;
		total = 0;
		flagged = false;
		id = NONE;
	}
	/* whether slot i holds a player; the loops that walk a party test it
	   at the top of each pass */
	bool slot_in_use(long i) const
	{
		bool result = false;
		if (i < count)
			result = true;
		return result;
	}
	bool same_id(s_balance_party const *other) const
	{
		bool result = false;
		if (id == other->id)
			result = true;
		return result;
	}
	void add_player(s_balance_player const *player);
	void remove_player(s_balance_player const *player);
};

struct s_balance_team
{
	long party_count;
	long flagged_count;
	s_balance_party parties[MAXIMUM_PARTIES];
	s_balance_party players;

	s_balance_team();
	void reset()
	{
		party_count = 0;
		flagged_count = 0;
		players.reset();
		players.id = 0;
	}
	void add_party(s_balance_party const *party);
	void remove_party(s_balance_party const *party);
	void remove_largest_party(s_balance_party *party);
};

s_balance_team *__stdcall function_91620(s_balance_team *team);

// The stored teams have trivial copy and destruction.
union s_balance_team_array
{
	long alignment;
	byte data[sizeof(s_balance_team) * MAXIMUM_TEAMS];
	s_balance_team &operator[](long i) { return ((s_balance_team *)data)[i]; }
	s_balance_team const &operator[](long i) const { return ((s_balance_team const *)data)[i]; }
};

struct s_balance_solution
{
	long team_count;
	s_balance_team_array teams;

	s_balance_solution()
	{
		for (long i = 0; i < MAXIMUM_TEAMS; ++i)
			function_91620(&teams[i]);
		team_count = 0;
	}
	void reset(long count);
	long total_spread() const;
	long count_spread() const;
	long rank_spread() const;
	long smallest_team() const;
	long cost() const
	{
		return 7 * total_spread() + 3 * rank_spread();
	}
};

struct s_balance_problem
{
	bool success;
	s_balance_solution solution;
	long maximum_team_size;

	s_balance_problem();
};

struct s_balance_optimizer
{
	s_balance_solution current;
	bool improved;
	s_balance_solution best;

	s_balance_optimizer();
	void try_swap(long team_a, long team_b);
	bool improve();
	bool optimize(s_balance_solution const *solution);
};

// @retail 0x91530
inline s_balance_player::s_balance_player()
{
	weight = NONE;
	index = NONE;
}

// @retail 0x91540
inline s_balance_party::s_balance_party()
{
	reset();
}

// @retail 0x91580
void s_balance_party::add_player(s_balance_player const *player)
{
	long i = count;
	while (i > 0)
	{
		if (players[i - 1].weight >= player->weight)
			break;
		players[i] = players[i - 1];
		i--;
	}
	players[i] = *player;
	count++;
	total += player->weight;
}

// @retail 0x915d0
void s_balance_party::remove_player(s_balance_player const *player)
{
	long index = player->index;
	long i = 0;
	while (!players[i].has_index(index))
		i++;
	count--;
	for (; i < count; i++)
		players[i] = players[i + 1];
	total -= player->weight;
}

__forceinline s_balance_team::s_balance_team()
{
	reset();
}

// @retail 0x91620
s_balance_team *__stdcall function_91620(s_balance_team *team)
{
	__assume(team != 0);
	s_balance_team *const *team_slot = &team;
	return new (*team_slot) s_balance_team;
}

// @retail 0x91690
void s_balance_team::add_party(s_balance_party const *party)
{
	parties[party_count] = *party;
	party_count++;
	for (long i = 0; party->slot_in_use(i); i++)
		players.add_player(&party->players[i]);
	if (party->flagged)
		flagged_count++;
}

// @retail 0x916e0
void s_balance_team::remove_party(s_balance_party const *party)
{
	long i = 0;
	while (!parties[i].same_id(party))
		i++;
	party_count--;
	for (; i < party_count; i++)
		parties[i] = parties[i + 1];
	for (long j = 0; party->slot_in_use(j); j++)
		players.remove_player(&party->players[j]);
	if (party->flagged)
		flagged_count--;
}

// @retail 0x91760
void s_balance_team::remove_largest_party(s_balance_party *party)
{
	s_balance_party *largest = &parties[0];
	for (long i = 1; i < party_count; i++)
	{
		if (parties[i].count > largest->count ||
			(parties[i].count == largest->count && parties[i].total > largest->total))
		{
			largest = &parties[i];
		}
	}
	*party = *largest;
	remove_party(party);
}

// @retail 0x917c0
void s_balance_solution::reset(long count)
{
	team_count = count;
	for (long i = 0; i < count; i++)
		teams[i].reset();
}

PRIVATE __forceinline long function_91801(s_balance_solution const *solution)
{
	long result = solution->teams[0].players.total;
	for (long i = 1; i < solution->team_count; ++i)
	{
		long value = solution->teams[i].players.total;
		if (result <= value) result = value;
	}
	return result;
}

PRIVATE __forceinline long function_91802(s_balance_solution const *solution)
{
	long result = solution->teams[0].players.total;
	for (long i = 1; i < solution->team_count; ++i)
	{
		long value = solution->teams[i].players.total;
		if (result > value) result = value;
	}
	return result;
}

// @retail 0x91800
long s_balance_solution::total_spread() const
{
	long maximum = function_91801(this);
	long minimum = function_91802(this);
	return maximum - minimum;
}

PRIVATE __forceinline long function_91861(s_balance_solution const *solution)
{
	long result = solution->teams[0].players.count;
	for (long i = 1; i < solution->team_count; ++i)
	{
		long value = solution->teams[i].players.count;
		if (result <= value) result = value;
	}
	return result;
}

PRIVATE __forceinline long function_91862(s_balance_solution const *solution)
{
	long result = solution->teams[0].players.count;
	for (long i = 1; i < solution->team_count; ++i)
	{
		long value = solution->teams[i].players.count;
		if (result > value) result = value;
	}
	return result;
}

// @retail 0x91860
long s_balance_solution::count_spread() const
{
	long maximum = function_91861(this);
	long minimum = function_91862(this);
	return maximum - minimum;
}

// @retail 0x918c0
long s_balance_solution::rank_spread() const
{
	long largest = teams[0].players.count;
	long i;
	for (i = 1; i < team_count; i++)
	{
		if (largest <= teams[i].players.count)
			largest = teams[i].players.count;
	}
	long sum = 0;
	for (long rank = 0; rank < largest; rank++)
	{
		long maximum = rank < teams[0].players.count ? teams[0].players.players[rank].weight : 0;
		long minimum = maximum;
		for (i = 1; i < team_count; i++)
		{
			long weight = rank < teams[i].players.count ? teams[i].players.players[rank].weight : 0;
			if (maximum <= weight)
				maximum = weight;
			if (minimum > weight)
				minimum = weight;
		}
		sum += maximum - minimum;
	}
	return sum;
}

// @retail 0x919a0
long s_balance_solution::smallest_team() const
{
	long smallest = 0;
	for (long i = 1; i < team_count; i++)
	{
		if (teams[i].players.count < teams[smallest].players.count ||
			(teams[i].players.count == teams[smallest].players.count && teams[i].players.total < teams[smallest].players.total))
		{
			smallest = i;
		}
	}
	return smallest;
}

// @retail 0x91a00
s_balance_problem::s_balance_problem()
{
	success = false;
}

// @retail 0x91b30
s_balance_optimizer::s_balance_optimizer()
{
	improved = false;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x91a30
bool balance_teams_greedy(s_balance_problem *problem, long team_count, s_balance_team const *pool, long maximum_team_size)
{
	problem->maximum_team_size = maximum_team_size;
	problem->success = false;
	s_balance_solution *solution = &problem->solution;
	solution->reset(team_count);
	s_balance_team remaining = *pool;
	while (remaining.party_count > 0)
	{
		s_balance_party party;
		remaining.remove_largest_party(&party);
		long target = solution->smallest_team();
		_ReadWriteBarrier();
		problem->solution.teams[target].add_party(&party);
	}
	long largest = solution->teams[0].players.count;
	for (long i = 1; i < solution->team_count; i++)
	{
		if (largest <= solution->teams[i].players.count)
			largest = solution->teams[i].players.count;
	}
	bool success = largest <= maximum_team_size;
	problem->success = success;
	return success;
}
#pragma function(_ReadWriteBarrier)

// score 116 (was 116): count homes, signed party delta and pure-query ownership remain.
// Earlier: count-address homes, party registers, delta and query order differ.
// Reduced control 80 (full 116); count fields A/B/both 80/80/80; early roots 173;
// players A/B/both 78/75/80; reverse count roots 173; B players root before A/first/
// between views 82/88/82; immutable B root 75. B players plain 115 is an ineligible
// one-count gain; all subobject forms restored.
// frame 0x2694 agrees; team-pointer homes, delta and query ownership differ.
// Reduced baseline 80 (full 116); team/party/both lexical priority 80/80/80;
// short outer/inner/both ordinals 142/142/155; accepted scope and comparison names 80/80.
// Earlier: count homes, signed delta and query ownership remain.
// Reduced candidate/reference/both spread values 135/127/137; spread delta 132;
// candidate A/B/both team roots 80/150/150; outer party cursor 91 (reduced baseline 80).
// Reduced trials (opening 80; full 116):
// first spread order 80; equal spread order 80; spread orders 80; cost order 78;
// first spread/cost order 78; equal spread/cost order 78; query orders 78; inner party cursor 132;
// cursor/spread orders 132; reused signed team delta 161; reused party delta 80;
// reused team and party deltas 161; A count reference 80; B count reference 80; count references 80;
// early count references 173; inline count spread 225; inline total spread 80;
// inline spread queries 225.
// the inner party pointer takes EAX (retail ECX), so the count delta is
// built as 2*(b-a) and the A/B count-address homes swap; the full build also calls each pair of
// queries in the other order (reference spread first, candidate cost first; retail the reverse).
// The reduced image scores 80 and orders the cost calls as retail: trust only the plain check.
// Plain-checked: first party pointer in the outer loop 116; nested ifs instead of continues 116;
// teams read in full at each use 146 (loses 0x91dd0); comparison operands swapped 116 (the call
// order follows the operands, not their position); explicit outer party pointer stepping 116.
// No-link screens: every delta spelling (named, doubled, parenthesised, count-after-swap, abs
// forms) folds to the same 2*(b-a) sequence; reference/local/method forms keep reference first.
// Earlier (reduced-image) forms: control 80; count fields A/B/both 80/80/80; early roots 173;
// players A/B/both 78/75/80; reverse count roots 173; B players root before A/first/between views
// 82/88/82; immutable B root 75 (B players plain 115, an ineligible one-count gain);
// team/party/both lexical priority 80/80/80; short outer/inner/both ordinals 142/142/155;
// scope and comparison names 80/80; candidate/reference/both spread values 135/127/137; spread
// delta 132; candidate A/B/both team roots 80/150/150; outer party cursor 91.
// @retail 0x91bd0
void s_balance_optimizer::try_swap(long team_a, long team_b)
{
	s_balance_team const *a = &current.teams[team_a];
	s_balance_team const *b = &current.teams[team_b];
	long difference = a->players.count - b->players.count;
	if (difference < 0)
		difference = -difference;
	for (long i = 0; i < a->party_count; i++)
	{
		for (long j = 0; j < b->party_count; j++)
		{
			s_balance_party const *party_a = &a->parties[i];
			s_balance_party const *party_b = &b->parties[j];
			if (party_a->flagged != party_b->flagged)
				continue;
			long new_difference = a->players.count - (party_a->count - party_b->count) * 2 - b->players.count;
			if (new_difference < 0)
				new_difference = -new_difference;
			if (new_difference > difference)
				continue;
			s_balance_solution candidate = current;
			candidate.teams[team_a].remove_party(party_a);
			candidate.teams[team_b].remove_party(party_b);
			candidate.teams[team_a].add_party(party_b);
			candidate.teams[team_b].add_party(party_a);
			s_balance_solution const *reference = improved ? &best : &current;
			if (candidate.count_spread() < reference->count_spread() ||
				(candidate.count_spread() == reference->count_spread() && reference->cost() > candidate.cost()))
			{
				improved = true;
				best = candidate;
			}
		}
	}
}

// @retail 0x91dd0
bool s_balance_optimizer::improve()
{
	improved = false;
	for (long i = 0; i < current.team_count - 1; i++)
	{
		for (long j = i + 1; j < current.team_count; j++)
			try_swap(i, j);
	}
	if (improved)
		current = best;
	return improved;
}

// @retail 0x91b90
bool s_balance_optimizer::optimize(s_balance_solution const *solution)
{
	bool result = false;
	current = *solution;
	if (improve())
	{
		result = true;
		while (improve());
	}
	return result;
}

static __forceinline void function_91fc2(s_balance_party *arg_0, s_balance_player const *arg_1)
{
    long local_0 = arg_0->count;
    while (local_0 > 0)
    {
        if (arg_0->players[local_0 - 1].weight >= arg_1->weight)
            break;
        arg_0->players[local_0] = arg_0->players[local_0 - 1];
        --local_0;
    }
    arg_0->players[local_0] = *arg_1;
    arg_0->count++;
    arg_0->total += arg_1->weight;
}

static __forceinline void function_91fc1(s_balance_team *arg_0, s_balance_party const *arg_1)
{
    arg_0->parties[arg_0->party_count] = *arg_1;
    arg_0->party_count++;
    for (long i = 0; arg_1->slot_in_use(i); i++)
        function_91fc2(&arg_0->players, &arg_1->players[i]);
    if (arg_1->flagged)
        arg_0->flagged_count++;
}

// @retail 0x91fc0
bool balance_teams(long team_count, long maximum_team_size, bool flag_parties, long flag_minimum, long flag_maximum,
	bool use_parties, long player_count, long const *party_indices, long const *weights, bool optimize, long *team_indices)
{
	s_balance_party parties[MAXIMUM_PARTIES];
	long i;
	for (i = 0; i < player_count; i++)
	{
		long party_index = use_parties ? party_indices[i] : i;
		s_balance_player player;
		player.index = i;
		player.weight = weights[i];
		parties[party_index].add_player(&player);
	}

	long pool_storage[sizeof(s_balance_team) / sizeof(long)];
	s_balance_team &pool = *(s_balance_team *)pool_storage;
	function_91620(&pool);
	for (i = 0; i < MAXIMUM_PARTIES; i++)
	{
		if (parties[i].count > 0)
		{
			s_balance_party *party = &parties[i];
			party->id = i;
			if (flag_parties && party->count >= flag_minimum && party->count <= flag_maximum)
				party->flagged = true;
			function_91fc1(&pool, party);
		}
	}

	s_balance_problem problem;
	bool result = false;
	if (balance_teams_greedy(&problem, team_count, &pool, maximum_team_size))
	{
		long optimizer_storage[sizeof(s_balance_optimizer) / sizeof(long)];
		s_balance_optimizer &optimizer = *(s_balance_optimizer *)optimizer_storage;
		new (optimizer_storage) s_balance_optimizer;
		s_balance_solution const *solution = &optimizer.current;
		if (!optimize || !optimizer.optimize(&problem.solution))
			solution = &problem.solution;
		for (long team = 0; team < solution->team_count; team++)
		{
			s_balance_party const *members = &solution->teams[team].players;
			for (long j = 0; members->slot_in_use(j); j++)
				team_indices[members->players[j].index] = team;
		}
		result = true;
	}
	return result;
}


/* balances the players without their weights and returns the difference
   between the largest and the smallest team */
// @retail 0x91e30
bool balance_teams_by_count(long player_count, long const *party_indices, long team_count, long maximum_team_size,
	bool flag_parties, long flag_minimum, long flag_maximum, bool use_parties, long *imbalance)
{
	long weights[MAXIMUM_PLAYERS];
	long team_indices[MAXIMUM_PLAYERS];
	memset(weights, 0, sizeof(weights));
	bool result = balance_teams(team_count, maximum_team_size, flag_parties, flag_minimum, flag_maximum,
		use_parties, player_count, party_indices, weights, false, team_indices);
	if (result)
	{
		long team_sizes[MAXIMUM_PLAYERS];
		memset(team_sizes, 0, sizeof(team_sizes));
		for (long i = 0; i < player_count; i++)
			team_sizes[team_indices[i]]++;
		long smallest = team_sizes[0];
		long largest = team_sizes[0];
		for (long team = 1; team < team_count; team++)
		{
			smallest = smallest > team_sizes[team] ? team_sizes[team] : smallest;
			largest = largest <= team_sizes[team] ? team_sizes[team] : largest;
		}
		*imbalance = largest - smallest;
	}
	return result;
}

/* whether the players and the extra players joining them can be balanced
   into the teams within the given imbalance; each extra player is a party
   of its own */
// score 34 (was 34): frame 0x48 agrees; from +0x14 the player-count EAX/EDX roles are exchanged
// (retail player count in EAX, extra count in EDX) and flow through the copy, fill, call setup and
// the final clamp's register. The body, not the parameter order or the caller, decides them.
// Tried: result first 34; guarded countdown 45; source/output cursors 67; count parameter reference
// 54; shared existing count 34; staged total 34; capacity lifetime first 34; independent loop
// indices 34; named candidate party 34; descriptive scalar homes 34; independent indices with
// candidate 34; earlier imbalance output 65; standard party maximum in either order 100; standard
// excess maximum in either order 61; both maxima 118; guarded standard fill 55; maxima with fill 119;
// total after copy 74; phase-accumulated total 74; shared party value 73; strict maximum update 35;
// early imbalance 66; explicit excess clamp 46; result from combined tests 67; separate noinline
// declaration 34; parameter orders (extra, parties, player) 35, (player, extra, parties) 34,
// (parties, player, extra) 34, (parties, extra, player) 35, (extra, player, parties) 35;
// extra-first sum 35. Round 94: copy then max from the copy 75; fill counted by extra count 62
// (moves extra count to the stack); clamp as `excess > 0 ? excess : 0` 43; open slots from the
// sum written out 71; `party >= next_party` 81. Screened without a role change: all 60 declaration
// orders, local and parameter names, another caller, inline fill/copy helpers, clamp spellings.
// frame 0x48 agrees; +0x14 player count arrives in EDX where retail has
// EAX (extra count EAX/EDX swapped), which rotates the copy, fill, call setup and final clamp
// registers; the body, not the parameter order, decides it. Tried: result first 34; guarded
// countdown 45; source/output cursors 67; count parameter reference 54; shared existing count
// 34; staged total 34; capacity lifetime first 34; independent loop indices 34; named candidate
// party 34; descriptive scalar homes 34; independent indices with candidate 34; earlier
// imbalance output 65; standard party maximum in either order 100; standard excess maximum in
// either order 61; both maxima 118; guarded standard fill 55; maxima with fill 119; total after
// copy 74; phase-accumulated total 74; shared party value 73; strict maximum update 35; early
// imbalance 66; explicit excess clamp 46; result from combined tests 67; separate noinline
// declaration 34; with the caller updated, orders (extra, parties, player) 35, (player, extra,
// parties) 34, (parties, player, extra) 34, (parties, extra, player) 35, (extra, player,
// parties) 35; extra-first sum 35; copy before the maximum update 82; parties read back for
// the maximum 75; fill as `while (i < total) parties[i++] = next;` 34; clamp as
// `excess >= 0 ? excess : 0` 34; array and index declared first 34.
// @retail 0x91f00
bool balance_teams_can_add(long player_count, long const *party_indices, long extra_count, long team_count,
	long maximum_team_size, bool flag_parties, long flag_minimum, long flag_maximum, bool use_parties,
	long maximum_imbalance)
{
	long total = player_count + extra_count;
	long open_slots = team_count * maximum_team_size - total;
	long next_party = 0;
	long parties[MAXIMUM_PLAYERS];
	long i;
	for (i = 0; i < player_count; i++)
	{
		if (next_party <= party_indices[i] + 1)
			next_party = party_indices[i] + 1;
		parties[i] = party_indices[i];
	}
	for (i = player_count; i < total; i++)
		parties[i] = next_party;
	long imbalance = NONE;
	bool result = balance_teams_by_count(total, parties, team_count, maximum_team_size, flag_parties,
		flag_minimum, flag_maximum, use_parties, &imbalance);
	if (result)
	{
		long excess = imbalance - open_slots;
		result = (excess < 0 ? 0 : excess) <= maximum_imbalance;
	}
	return result;
}
