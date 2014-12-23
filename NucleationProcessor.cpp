#include "NucleationProcessor.h"

#include <cstdlib>
#include <iostream>

// Performs, basically, an MC step w/ regard to recrystallization status.
Node** NucleationProcessor::processField(Node** nodes, Node** next, float Hi)
{
  while(MonteCarloUntouchedFieldsExist(next))
  {
     int x = int(rand()%DIM);
     int y = int(rand()%DIM);
     
     if(next[x][y].id != 0)
       continue;
     else 
     {
      if(!RecrystSite(nodes, x, y)) // nothing to change, copy
      {
        next[x][y] = nodes[x][y];
      } else {
        Node randomNeighbor = RandomRecrystNeighbor(nodes, x, y);

        if(CurrentEnergy(nodes, x, y, Hi) >= SimulateEnergy(nodes, randomNeighbor, x, y))
        {
          next[x][y] = randomNeighbor;
          next[x][y].recryst = true;
        }
        else
           next[x][y] = nodes[x][y];
       }
      }
     }

  return next;
}

// Energy in floating point, Hi included
float NucleationProcessor::CurrentEnergy(Node** nodes, int x,  int y, float Hi)
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
  
  float energy = Hi;
  
  for(int i=0; i<8; i++)
  {
    if(thisCol.id != cols[i].id) energy += ENERGY_DELTA;
  }

  return energy;
}

// Simulation does not require Hi
float NucleationProcessor::SimulateEnergy(Node** nodes, Node sim, int x,  int y)
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
  
  float energy = 0;
  
  for(int i=0; i<8; i++)
  {
    if(sim.id != cols[i].id) energy += ENERGY_DELTA;
  }
  
  return energy;
}

// Function to place new nucleations according to rules
void NucleationProcessor::SpreadNucleations(Node** nodes, int num, int mode)
{
  int tried_times = 0;

  while(num != 0)
  {
    tried_times++;
    
     int x = int(rand()%DIM);
     int y = int(rand()%DIM);

     if(mode == 0)
     {
        if(!nodes[x][y].recryst)
       {
          nodes[x][y].recryst = true;
          nodes[x][y].id += DIM*DIM; // thaaaat's racist
          num--;
       }
     } else {
       if (IsGrainBoundary(nodes, x, y) && !nodes[x][y].recryst)
       {
          nodes[x][y].recryst = true;
          nodes[x][y].id += DIM*DIM; // thaaaat's racist, too
          num--;
       }
     }

    if(tried_times > 25)
    {
      num--;
      tried_times=0;
    }
  }
}

// Checks whether there is at least one recrystallized neighbour in a Moore neighbourhood
bool NucleationProcessor::RecrystSite(Node** nodes, int x, int y)
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
   if(cols[i].recryst) 
    return true;

  return false;
}

// Picks a random recrystallized neighbor
Node NucleationProcessor::RandomRecrystNeighbor(Node** nodes, int x, int y)
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

  Node ret;

  do
  {
    ret = cols[int(rand()%8)];
  } while(!ret.recryst);
  
  return ret;
}