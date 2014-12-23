#include "NucleationProcessor.h"

#include <cstdlib>
#include <iostream>

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
        std::cout << "recryst site" << std::endl;
        Node randomNeighbor = RandomRecrystNeighbor(nodes, x, y);
        std::cout << "recryst nejb" << std::endl;

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

void NucleationProcessor::SpreadNucleations(Node** nodes, int num, int mode)
{
  while(num != 0)
  {
     int x = int(rand()%DIM);
     int y = int(rand()%DIM);

     if(mode == 0)
     {
        if(!nodes[x][y].recryst) // we got us a boundary, woo hoo
       {
          nodes[x][y].recryst = true;
          nodes[x][y].id += DIM*DIM; // thaaaat's racist
          num--;
       }
     } else {
       if (IsGrainBoundary(nodes, x, y) && !nodes[x][y].recryst) // we got us a boundary, woo hoo
       {
          nodes[x][y].recryst = true;
          nodes[x][y].id += DIM*DIM; // thaaaat's racist
          num--;
       }
     }
  }
}

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
  
  std::cout << "Found recryst neighbour" << std::endl;

  return ret;
}