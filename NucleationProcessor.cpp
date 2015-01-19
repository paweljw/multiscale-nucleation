#include "NucleationProcessor.h"

#include <cstdlib>
#include <iostream>

// Performs, basically, an MC step w/ regard to recrystallization status.
Node** NucleationProcessor::processField(Node** nodes, Node** next, float Hi, float J)
{
  while(MonteCarloUntouchedFieldsExist(next))
  {
    // select site randomly
     int x = int(rand()%DIM);
     int y = int(rand()%DIM);
     
     if(next[x][y].id != 0)
       continue;
     else 
     {
      if(!RecrystSite(nodes, x, y)) // no recrystallized neighbours
      {
        next[x][y] = nodes[x][y];
      } else { // selecting random recrystallized neighbour site
        Node randomNeighbor = RandomRecrystNeighbor(nodes, x, y);

        //if(CurrentEnergy(nodes, x, y, Hi, J) >= SimulateEnergy(nodes, randomNeighbor, x, y, J)) // if energy is lower or the same,
        //{
          next[x][y] = randomNeighbor; // accept reorientation
          next[x][y].recryst = true; // become recrystallized
          next[x][y].energy = 0; // drop energy
        //}
        //else
          // next[x][y] = nodes[x][y];
       //}
        }
      }
     }

  return next;
}

// Energy in floating point, Hi included
float NucleationProcessor::CurrentEnergy(Node** nodes, int x,  int y, float Hi, float J)
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
  
  float energy = 0;
  
  for(int i=0; i<8; i++)
  {
    if(thisCol.id != cols[i].id) energy += (cols[i].energy);
  }

  energy *= J;
  energy += Hi;

  return energy;
}

// Simulation does not require Hi
float NucleationProcessor::SimulateEnergy(Node** nodes, Node sim, int x,  int y, float J)
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
    if(sim.id != cols[i].id) energy += (cols[i].energy);
  }

  energy *= J;
  
  return energy;
}

// Function to place new nucleations according to rules
void NucleationProcessor::SpreadNucleations(Node** nodes, int num)
{
  int tried_times = 0;

  while(num != 0)
  {
    tried_times++;
    
     int x = int(rand()%DIM);
     int y = int(rand()%DIM);

     if(!nodes[x][y].recryst)
     {
        nodes[x][y].recryst = true;
        nodes[x][y].id += DIM*DIM; // change "color"
        num--;
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
  // Node thisCol = nodes[x][y];
  
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