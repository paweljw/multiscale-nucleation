#pragma once

#include "node.h"
#include "globals.h"

class MonteCarloProcessor
{
	public:
		static Node** processField(Node** nodes, Node** next);
		static bool MonteCarloUntouchedFieldsExist(Node** nodes);
		static int CurrentEnergy(Node** nodes, int x,  int y);
		static int SimulateEnergy(Node** nodes, Node sim, int x,  int y);
		static Node RandomNeighbor(Node** nodes, int x, int y);
		static bool IsGrainBoundary(Node** nodes, int x, int y);
		static int SafeCoord(int coord);
};