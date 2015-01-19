#pragma once

#include "node.h"
#include "MonteCarloProcessor.h"

class NucleationProcessor : public MonteCarloProcessor
{
public:
	static Node** processField(Node** nodes, Node** next, float Hi, float J);
	static float CurrentEnergy(Node** nodes, int x,  int y, float Hi, float J);
	static float SimulateEnergy(Node** nodes, Node sim, int x,  int y, float J);
	static void SpreadNucleations(Node** nodes, int num);
	static bool RecrystSite(Node** nodes, int x, int y);
	static Node RandomRecrystNeighbor(Node** nodes, int x, int y);
};