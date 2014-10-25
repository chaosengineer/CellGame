/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#pragma once

#include <vector>

namespace chaos {
namespace cell {

class Cell;
class ICellAI;
class Settings;

class Simulator {

public:

	enum State {
		READY,
		PAUSED,
		FINISHED
	};

	Simulator(const Settings &gameSettings);

	const std::vector<Cell>& getCells(int &liveCellCount) const;
	const Cell& getPlayerCell() const;
	const State& getState() const;

	void populate();
	void setPlayerAI(const ICellAI *cellAI);

	void simulateNextTick();

	void toggleSimulationPause();

private:
	const Settings &settings;
	
	std::vector<Cell> cells;

	int playerCellIndex;
	int liveCellsCount;

	State state;

	void updateLiveCellCount();

	void accelerateCell(Cell &cell);

	void moveCell(Cell &cell);

	bool isCellCollidingWithArena(const Cell &cell) const;
	void collideCellWithArena(Cell &cell);
	
	bool areCellsColliding(const Cell &lhs, const Cell &rhs) const;
	void collideCells(Cell &lhs, Cell &rhs);
};

}; // namespace cell
}; // namespace chaos