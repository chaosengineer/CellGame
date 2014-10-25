/*
	Copyright (C) 2014 Chaos Software

	Distributed under the Boost Software License, Version 1.0.
	See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt.
*/

// Project headers
#include "cell.h"
#include "cell_ai.h"
#include "settings.h"
#include "simulator.h"
#include "math_utils.h"

// GLUT - include last because of a redefinition problem
#include "glut.h"

using namespace std;
using namespace chaos::cell;

Settings settings;
Simulator simulator(settings);

inline int getCircleSegmentCount(float radius) {
	static const float MAX_LINE_LENGTH = 0.001f;
	return clamp(DOUBLE_PI * radius / MAX_LINE_LENGTH, 8.0f, 128.0f);
}

void drawCircle(const Vector &center, float radius, bool fillCircle) {
	int circleSegmentCount = getCircleSegmentCount(radius);

	float stepAngleRadian = DOUBLE_PI / circleSegmentCount;
	float stepCos = cosf(stepAngleRadian);
	float stepSin = sinf(stepAngleRadian);

	float x = radius;
	float y = 0.0f;

	glBegin(fillCircle ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
	while (0 < circleSegmentCount) {
		glVertex2f(center.x + x, center.y + y);

		float rotatedX = stepCos * x - stepSin * y;
		float rotatedY = stepSin * x + stepCos * y;
		x = rotatedX;
		y = rotatedY;

		--circleSegmentCount;
	}
	glEnd();
}

void display() {
	// Clear buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw arena
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(2.0f);
	drawCircle(settings.arenaCenter, settings.arenaRadius, false);

	// Get simulator data
	int liveCellCount = 1;
	const Cell &smartCell = simulator.getPlayerCell();
	const vector<Cell> &cells = simulator.getCells(liveCellCount);

	// Draw cells
	vector<Cell>::const_iterator liveCellsEnd = cells.begin() + liveCellCount;
	for (vector<Cell>::const_iterator cellIterator = cells.begin(); cellIterator != liveCellsEnd; ++cellIterator) {
		if (cellIterator->radius < smartCell.radius) {
			glColor3f(0.0f, 1.0f, 0.0f);
		} else {
			glColor3f(1.0f, 0.0f, 0.0f);
		}
		drawCircle(cellIterator->position, cellIterator->radius, true);
	}

	// Draw smart cell
	glColor3f(0.0f, 0.0f, 1.0f);
	drawCircle(smartCell.position, smartCell.radius, true);

	// Swap buffers
	glutSwapBuffers();
}

void reshape(int width, int height) {
	int minDimension = chaos::cell::min(width, height);
	glViewport((width - minDimension) / 2, (height - minDimension) / 2, minDimension, minDimension);
}

void idle() {
	static int simulatedTicks = 0;
	static int referenceTime = 0;

	const Simulator::State &simulatorState = simulator.getState();
	if (simulatorState == Simulator::READY) {
		simulator.simulateNextTick();
		glutPostRedisplay();
	} else if (simulatorState == Simulator::FINISHED && settings.exitOnSimulationFinished) {
		exit(0);
	}

	++simulatedTicks;
	int elapsedTime = glutGet(GLUT_ELAPSED_TIME) - referenceTime;

	if (elapsedTime - referenceTime > 1000) {
		printf("Simulation steps per second: %f\n", (1000.0f * simulatedTicks) / (elapsedTime - referenceTime));	
		referenceTime = elapsedTime;
		simulatedTicks = 0;
	}
}

void keyboardNormal(unsigned char key, int x, int y) {
	static const unsigned char KEYBOARD_ESCAPE_KEY = 27;
	static const unsigned char KEYBOARD_SPACE_KEY = ' ';
	static const unsigned char KEYBOARD_R_KEY = 'r';

	switch (key) {
		case KEYBOARD_ESCAPE_KEY: {
			exit(0);
			break;
		}
		case KEYBOARD_SPACE_KEY: {
			simulator.toggleSimulationPause();
			break;
		}
		case KEYBOARD_R_KEY: {
			const ICellAI *playerAI = simulator.getPlayerCell().ai;
			CustomAI *customAI = const_cast<CustomAI*>(static_cast<const CustomAI*>(playerAI));
			if (customAI) {
				customAI->reload();
			}
			break;
		}
	}
}

void keyboardSpecial(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP: {
			settings.tickLength += 0.001f;
			break;
		}
		case GLUT_KEY_RIGHT: {
			if (simulator.getState() == Simulator::PAUSED) {
				simulator.simulateNextTick();
				glutPostRedisplay();
			}
			break;
		}
		case GLUT_KEY_DOWN: {
			settings.tickLength -= 0.001f;
			break;
		}
	}
}

int main(int argc, char **argv) {
	// Initialize GLUT
	glutInit(&argc, argv);

	// Setup window
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)  - settings.displayResolution) / 2,
                           (glutGet(GLUT_SCREEN_HEIGHT) - settings.displayResolution) / 2);
	glutInitWindowSize(settings.displayResolution, settings.displayResolution);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	// Create window
	glutCreateWindow("CellGame");

	// Register callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboardNormal);
	glutSpecialFunc(keyboardSpecial);
	glutIdleFunc(idle);

	// Load settings
	settings.load();

	// Parse input
	switch (argc) {
		case 2: {
			strncpy(settings.playerScriptPath, argv[1], sizeof(settings.playerScriptPath));
			break;
		}
		case 3: {
			strncpy(settings.playerScriptPath, argv[1], sizeof(settings.playerScriptPath));
			settings.levelSeed = atoi(argv[2]);
			break;
		}
	}

	// Populate the level for the simulation
	simulator.populate();

	// Load the custom AI
	CustomAI customAI(settings.baseModulePath, settings.playerScriptPath, "custom_cell_ai_");
	customAI.prepare();
	simulator.setPlayerAI(&customAI);
	
	// Enter GLUT's event processing cycle
	glutMainLoop();

	return 0;
}