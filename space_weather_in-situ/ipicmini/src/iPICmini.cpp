/* iPIC3D was originally developed by Stefano Markidis and Giovanni Lapenta. 
 * This release was contributed by Alec Johnson and Ivy Bo Peng.
 * Publications that use results from iPIC3D need to properly cite  
 * 'S. Markidis, G. Lapenta, and Rizwan-uddin. "Multi-scale simulations of 
 * plasma with iPIC3D." Mathematics and Computers in Simulation 80.7 (2010): 1509-1519.'
 *
 *        Copyright 2015 KTH Royal Institute of Technology
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at 
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** iPICmini is a mini-app created from iPIC3D in 2018 by V. Olshevsky. */

#include "MPIdata.h"
#include "iPic3D.h"
#include "debug.h"
#include "TimeTasks.h"
#include <stdio.h>
#include <iostream>
#include <cstring>

using namespace iPic3D;

/** Initialize timings. 
 * 
 * Times are stored in a global object timeTasks defined in TimeTasks.h.
 * 
*/
void initFigures(NameDoubleMap &cycle_times)
{
  int i = 1;
  const char * s;
  s = timeTasks.get_taskname(i);

  while (strcmp(s, "AFTER_EXCLUSIVE") != 0)
  {
    cycle_times[s] = 0.;
    i++;
    s = timeTasks.get_taskname(i);
  }

  cycle_times["total"] = 0.;
}

/** Print various figures marking performance. */
void printFigures(long long int nop, NameDoubleMap &cycle_times)
{
  // Compute total time spent on particles and fields since tasks have been started.
  // It doesn't restart the tasks.
  timeTasks.get_cycle_times("avg", cycle_times);

  // Time per particle
  double nspp = cycle_times["particles"] / (double) nop;

  // Only print on the zeroth process
  if(MPIdata::get_rank()) return;
  FILE* file = stdout;
  fflush(file);
  fprintf(file, "%s %.2e\n%s %.2e \n", "Number of particles", (double) nop, "sec/particle", nspp);
  fflush(file);
}

int main(int argc, char **argv) 
{
  MPIdata::init(&argc, &argv);
  {
    // Create and init new iPic3D solver object
    iPic3D::c_Solver ipic_solver;
    ipic_solver.Init(argc, argv);
   
    // mini-app stuff
    // A dictionary of times with times for certain tasks; keys are task names and "total".
    NameDoubleMap cycle_times;

    // initialize the mini-app timings
    initFigures(cycle_times);
   
    // Reset the times recorded by timeTasks global object
    timeTasks.resetCycle();

    ipic_solver.CalculateMoments();

    // Main simulation cycle
    for (int i = ipic_solver.FirstCycle(); i < ipic_solver.LastCycle(); i++) 
    {   
      if (MPIdata::get_rank() == 0)
        printf(" ======= Cycle %d ======= \n", i);
   
      // Reset the times stored by timeTasks
      timeTasks.resetCycle();

      ipic_solver.CalculateField(i);
      ipic_solver.ParticlesMover();
      ipic_solver.CalculateB();
      ipic_solver.CalculateMoments();
      ipic_solver.WriteOutput(i);
   
      // Print the statistics gathered by mini-app
      printFigures(ipic_solver.get_NOPtotal(), cycle_times);
   
      // print out total time for all tasks
      //timeTasks.print_cycle_times(i);
    }
   
    ipic_solver.Finalize();
  }

  // close MPI
  MPIdata::instance().finalize_mpi();
 
 return 0;
}
