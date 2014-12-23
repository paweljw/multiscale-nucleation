#include "MonteCarloProcessor.h"

#include <cstdlib>
#include <iostream>

int MonteCarloProcessor::SafeCoord(int coord)
{
    if (coord >= 0) return (coord % DIM);
    else return (coord + DIM);
}

Node** MonteCarloProcessor::processField(Node** nodes, Node** next)
{
	while(MonteCarloProcessor::MonteCarloUntouchedFieldsExist(next))
	{
	   int x = int(rand()%DIM);
	   int y = int(rand()%DIM);
	   
	   if(next[x][y].id != 0)
	     continue;
	   else 
	   {
	     if(MonteCarloProcessor::CurrentEnergy(nodes, x, y) == 0)
	     {
	       next[x][y] = nodes[x][y];
	       continue;
	     } else {
		    Node randomNeighbor = MonteCarloProcessor::RandomNeighbor(nodes, x, y);
	        if(MonteCarloProcessor::CurrentEnergy(nodes, x, y) >= MonteCarloProcessor::SimulateEnergy(nodes, randomNeighbor, x, y))
	         next[x][y] = randomNeighbor;
	       else
	         next[x][y] = nodes[x][y]; 
	     }
	   }
	 }

	return next;
}

bool MonteCarloProcessor::MonteCarloUntouchedFieldsExist(Node** next)
{
  for(int i=0;i<DIM;i++)
    for(int j=0;j<DIM;j++)
      if(next[i][j].id == 0) return true;
      
  return false;
}

int MonteCarloProcessor::CurrentEnergy(Node** nodes, int x,  int y)
{
  Node thisCol = nodes[x][y];
  
  Node *cols = new Node[8];
  cols[0] = nodes[SafeCoord(x-1)][SafeCoord(y-1)];
  cols[1] = nodes[SafeCoord(x)][SafeCoord(y-1)];
  cols[2] = nodes[SafeCoord(x+1)][SafeCoord(y-1)];
  cols[3] = nodes[SafeCoord(x-1)][SafeCoord(y)];
  cols[4] = nodes[SafeCoord(x+1)][SafeCoord(y)];
  cols[5] = nodes[SafeCoord(x-1)][SafeCoord(y+1)];
  cols[6] = nodes[SafeCoord(x)][SafeCoord(y+1)];
  cols[7] = nodes[SafeCoord(x+1)][SafeCoord(y+1)];
  
  int energy = 0;
  
  for(int i=0; i<8; i++)
  {
    if(thisCol.id != cols[i].id) energy++;
  }
  
  return energy;
}

bool MonteCarloProcessor::IsGrainBoundary(Node** nodes, int x,  int y)
{
  Node thisCol = nodes[x][y];
  
  Node *cols = new Node[8];
  cols[0] = nodes[SafeCoord(x-1)][SafeCoord(y-1)];
  cols[1] = nodes[SafeCoord(x)][SafeCoord(y-1)];
  cols[2] = nodes[SafeCoord(x+1)][SafeCoord(y-1)];
  cols[3] = nodes[SafeCoord(x-1)][SafeCoord(y)];
  cols[4] = nodes[SafeCoord(x+1)][SafeCoord(y)];
  cols[5] = nodes[SafeCoord(x-1)][SafeCoord(y+1)];
  cols[6] = nodes[SafeCoord(x)][SafeCoord(y+1)];
  cols[7] = nodes[SafeCoord(x+1)][SafeCoord(y+1)];
  
  for(int i=0; i<8; i++)
  {
    if(thisCol.id != cols[i].id) return true;
  }
 
  return false;
}

Node MonteCarloProcessor::RandomNeighbor(Node** nodes, int x, int y)
{
  Node* cols = new Node[8];
  cols[0] = nodes[SafeCoord(x-1)][SafeCoord(y-1)];
  cols[1] = nodes[SafeCoord(x)][SafeCoord(y-1)];
  cols[2] = nodes[SafeCoord(x+1)][SafeCoord(y-1)];
  cols[3] = nodes[SafeCoord(x-1)][SafeCoord(y)];
  cols[4] = nodes[SafeCoord(x+1)][SafeCoord(y)];
  cols[5] = nodes[SafeCoord(x-1)][SafeCoord(y+1)];
  cols[6] = nodes[SafeCoord(x)][SafeCoord(y+1)];
  cols[7] = nodes[SafeCoord(x+1)][SafeCoord(y+1)];
  
  return cols[int(rand()%8)];
}

int MonteCarloProcessor::SimulateEnergy(Node** nodes, Node sim, int x,  int y)
{
  Node *cols = new Node[8];
  cols[0] = nodes[SafeCoord(x-1)][SafeCoord(y-1)];
  cols[1] = nodes[SafeCoord(x)][SafeCoord(y-1)];
  cols[2] = nodes[SafeCoord(x+1)][SafeCoord(y-1)];
  cols[3] = nodes[SafeCoord(x-1)][SafeCoord(y)];
  cols[4] = nodes[SafeCoord(x+1)][SafeCoord(y)];
  cols[5] = nodes[SafeCoord(x-1)][SafeCoord(y+1)];
  cols[6] = nodes[SafeCoord(x)][SafeCoord(y+1)];
  cols[7] = nodes[SafeCoord(x+1)][SafeCoord(y+1)];
  
  int energy = 0;
  
  for(int i=0; i<8; i++)
  {
    if(sim.id != cols[i].id) energy++;
  }
  
  return energy;
}