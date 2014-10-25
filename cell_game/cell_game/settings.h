/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

#pragma once

#include <cstring>

#include "vector2d.h"

#ifndef MAX_PATH
	#define MAX_PATH 256
#endif // MAX_PATH

namespace chaos {
namespace cell {

class Settings {

public:
	// Frequently changing parameters.
	int levelSeed;
	char playerScriptPath[MAX_PATH];

	// Simulation parameters. Usually don't change.
	float tickLength;
	Vector arenaCenter;
	float arenaRadius;
	int cellCount;
	float cellDensity;
	float cellMinimumRadius;
	float cellMaximumRadius;
	float cellVelocityVariance;
	float playerCellInitialRadius;

	// System parameters. Almost never change.
	int displayResolution;
	int exitOnSimulationFinished;
	char baseModulePath[MAX_PATH];
	char settingsFilePath[MAX_PATH];

	Settings() 
		: levelSeed(0)
		, tickLength(0.005f)
		, arenaCenter(0.0f, 0.0f)
		, arenaRadius(1.0f)
		, cellCount(128)
		, cellDensity(250.0f)
		, cellMinimumRadius(0.005f)
		, cellMaximumRadius(0.02f)
		, cellVelocityVariance(0.2f)
		, playerCellInitialRadius(0.01f)
		, displayResolution(640)
		, exitOnSimulationFinished(1) {
		playerScriptPath[0] = '\0';
		strncpy(baseModulePath, ".\\base.bc", sizeof(baseModulePath));
		strncpy(settingsFilePath, ".\\settings.txt", sizeof(baseModulePath));
	}

	void load() {
		FILE *settingsFile = fopen(settingsFilePath, "r");
		if (settingsFile) {
			fscanf(settingsFile, "%*s %d", &levelSeed);
			fscanf(settingsFile, "%*s %256s", playerScriptPath);
			fscanf(settingsFile, "%*s %f", &tickLength);
			fscanf(settingsFile, "%*s %f %f", &arenaCenter.x, &arenaCenter.y);
			fscanf(settingsFile, "%*s %f", &arenaRadius);
			fscanf(settingsFile, "%*s %d", &cellCount);
			fscanf(settingsFile, "%*s %f", &cellDensity);
			fscanf(settingsFile, "%*s %f", &cellMinimumRadius);
			fscanf(settingsFile, "%*s %f", &cellMaximumRadius);
			fscanf(settingsFile, "%*s %f", &cellVelocityVariance);
			fscanf(settingsFile, "%*s %f", &playerCellInitialRadius);
			fscanf(settingsFile, "%*s %d", &displayResolution);
			fscanf(settingsFile, "%*s %d", &exitOnSimulationFinished);
			fscanf(settingsFile, "%*s %256s", baseModulePath);
			
			fclose(settingsFile);
		}
	}
};

}; // namespace cell
}; // namespace chaos