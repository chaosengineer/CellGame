/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#include <math.h> // std::floor()
#include <algorithm> // std::sort()

#include "cell.h"
#include "cell_ai.h"
#include "settings.h"
#include "simulator.h"
#include "math_utils.h"

using namespace std;
using namespace chaos::cell;

////////////////////////////////////////////////////////////
// Helpers

class DistanceToPlayerComparator {

public:
	DistanceToPlayerComparator(const Cell &inputCell) : playerCell(inputCell) {
	}
	
	bool operator()(const Cell &lhs, const Cell &rhs) {
		float playerDistanceToLeftCell = chaos::cell::distance(playerCell, lhs);
		float playerDistanceToRightCell = chaos::cell::distance(playerCell, rhs);
		return playerDistanceToLeftCell < playerDistanceToRightCell;
	}

private:
	const Cell &playerCell;
};

////////////////////////////////////////////////////////////
// Simulator implementation

Simulator::Simulator(const Settings &gameSettings) 
	: settings(gameSettings)
	, playerCellIndex(-1)
	, liveCellsCount(0)
	, state(READY) {
}

const vector<Cell>& Simulator::getCells(int &liveCellCountOutput) const {
	liveCellCountOutput = liveCellsCount;
	return cells;
}

const Cell& Simulator::getPlayerCell() const {
	return cells[playerCellIndex];
}

const Simulator::State& Simulator::getState() const {
	return state;
}

void Simulator::populate() {
	cells.reserve(settings.cellCount);

	playerCellIndex = 0;
	liveCellsCount = settings.cellCount;

	state = READY;

	srand(settings.levelSeed);

	Cell player = Cell(settings.playerCellInitialRadius, settings.arenaCenter, Vector());
	cells.push_back(player);

	while ((int)cells.size() < settings.cellCount) {
		float cellRadius = randomFloat(settings.cellMinimumRadius, settings.cellMaximumRadius);
		Vector cellPosition(settings.arenaRadius * randomFloat(), settings.arenaRadius * randomFloat());
		Vector cellVelocity(settings.cellVelocityVariance * randomFloat(), settings.cellVelocityVariance * randomFloat());
		Cell newCell(cellRadius, cellPosition, cellVelocity);

		bool isCellInsideArena = !isCellCollidingWithArena(newCell);
		if (isCellInsideArena) {
			bool isOkWithOtherCells = true;
			for (vector<Cell>::const_iterator cellIterator = cells.begin(); isOkWithOtherCells && cellIterator != cells.end(); ++cellIterator) {
				isOkWithOtherCells = !areCellsColliding(newCell, *cellIterator);
			}
			if (isOkWithOtherCells) {
				cells.push_back(newCell);
			}
		}
	}
}

void Simulator::setPlayerAI(const ICellAI *cellAI) {
	if (0 <= playerCellIndex && playerCellIndex < (int)cells.size()) {
		cells[playerCellIndex].ai = cellAI;
	}
}

void Simulator::simulateNextTick() {
	// 1. Resolve cell collisions with the arena walls and other cells. Some cells may die.
	vector<Cell>::iterator liveCellsEnd = cells.begin() + liveCellsCount;
	for (vector<Cell>::iterator firstCellIterator = cells.begin(); firstCellIterator != liveCellsEnd; ++firstCellIterator) {
		if (firstCellIterator->isDead()) {
			continue;
		}

		// Collide with arena walls
		collideCellWithArena(*firstCellIterator);

		for (vector<Cell>::iterator secondCellIterator = firstCellIterator + 1; secondCellIterator != liveCellsEnd; ++secondCellIterator) {
			if (secondCellIterator->isDead()) {
				continue;
			}

			// Collide with another cell
			collideCells(*firstCellIterator, *secondCellIterator);

			if (firstCellIterator->isDead()) {
				break;
			}
		}
	}

	// 2. Finish simulation if player died.
	Cell &playerCell = cells[playerCellIndex];
	if (playerCell.isDead()) {
		state = FINISHED;
		return;
	}

	// 3. Sorts all the cells using distance to player. Closest cells are first.
	//    Since distance to dead cells is infinity we also partition our cell vector in two sections - living cells and dead cells.
	sort(cells.begin(), liveCellsEnd, DistanceToPlayerComparator(playerCell));

	// 4. Update live cell count.
	updateLiveCellCount();

	// 5. Finish simulation if player won.
	if (liveCellsCount == 1) {
		state = FINISHED;
		return;
	}

	// 6. Let player cell figure out where to go. Accelerate player cell.
	accelerateCell(playerCell);

	// 7. Move all cells.
	liveCellsEnd = cells.begin() + liveCellsCount;
	for (vector<Cell>::iterator cellIterator = cells.begin(); cellIterator != liveCellsEnd; ++cellIterator) {
		moveCell(*cellIterator);
	}
}

void Simulator::toggleSimulationPause() {
	if (state == READY) {
		state = PAUSED;
	} else if (state == PAUSED) {
		state = READY;
	}
}

void Simulator::updateLiveCellCount() {
	// Very few cells die in a single tick (one or two at most) so this is faster than binary search.
	while (0 < liveCellsCount && cells[liveCellsCount - 1].isDead()) {
		--liveCellsCount;
	}
}

void Simulator::accelerateCell(Cell &playerCell) {
	if (playerCell.ai) {
		Vector force;
		playerCell.ai->calculateForce(cells, liveCellsCount, settings.arenaRadius, force);
		force.normalize();

		//float mass = settings.cellDensity * playerCell.getArea();
		Vector acceleration = force;// / mass;
		playerCell.velocity += settings.tickLength * acceleration;
	}
}

void Simulator::moveCell(Cell &cell) {
	cell.position += cell.velocity * settings.tickLength;
}

bool Simulator::isCellCollidingWithArena(const Cell &cell) const {
	bool result = false;

	float distanceFromArenaCenter = distance(cell.position, settings.arenaCenter) + cell.radius;
	if (settings.arenaRadius < distanceFromArenaCenter) {
		result = true;
	}

	return result;
}

void Simulator::collideCellWithArena(Cell &cell) {
	if (isCellCollidingWithArena(cell)) {
		// Find the unit vector pointing away from the arena at the point of collision.
		Vector normal = cell.position - settings.arenaCenter;
		normal.normalize();

		// Pull the cell back into the arena, making sure it doesn't collide during the next tick.
		cell.position = (settings.arenaRadius - cell.radius - EPSILON) * normal;

		// Flip the normal. Now it points directly to the arena's center at the point of collision.
		normal = -normal;

		// Use the normal to reflect the cell's velocity.
		cell.velocity -= 2.0f * dot(normal, cell.velocity) * normal;
	}
}

bool Simulator::areCellsColliding(const Cell &lhs, const Cell &rhs) const {
	bool result = false;

	if (distance(lhs, rhs) < EPSILON) {
		result = true;
	}

	return result;
}

void Simulator::collideCells(Cell &lhs, Cell &rhs) {
	float cellCenterDistance = distance(lhs.position, rhs.position);
	if (cellCenterDistance < lhs.radius + rhs.radius + EPSILON) {
		// The two cells are colliding. Determine who eats who.
		Cell *preyCell = NULL;
		Cell *hunterCell = NULL;
		if (lhs.radius < rhs.radius) {
			preyCell = &lhs;
			hunterCell = &rhs;
		} else {
			preyCell = &rhs;
			hunterCell = &lhs;
		}

		// We want the hunter and prey to be just incident after the bite and preserve the cell matter.
		// Solve the system of equations.
		// | 0 < hunter radius < new hunter radius
		// | 0 <= new prey radius < prey radius
		// | new hunter raduis + new prey radius = distance(hunter center, prey center)
		// | hunter area + prey area = new hunter area + new prey area

		float newPreyRadius = 0.5f * (cellCenterDistance - sqrtf(2.0f * (hunterCell->radius * hunterCell->radius + preyCell->radius * preyCell->radius) - cellCenterDistance * cellCenterDistance)); 
		float newHunterRadius = cellCenterDistance - newPreyRadius - EPSILON;
		
		float oldHunterArea = hunterCell->getArea();
		float newHunterArea = PI * newHunterRadius * newHunterRadius;

		float biteArea = newHunterArea - oldHunterArea;

		preyCell->radius = newPreyRadius;
		hunterCell->radius = newHunterRadius;
		hunterCell->velocity = (oldHunterArea * hunterCell->velocity + biteArea * preyCell->velocity) / newHunterArea;
	}
}
