// APP NOTES:
//
// Simple CPU particle simulation.
// This is a small graphical application in which 3 types of particle are emitted from separate locations,
// with differing position offsets, velocities, accelerations and start & end colours.
// Each particle is represented by a single pixel.
// Go to 'particle_system.h' for more details.
//
//
// ASSIGNMENT NOTES RECAP:
//
// Please use git to version your code throughout development.
// Commit little & often and be sure to fully detail the code changes made in your commit messages.
// It is also suggested that you make use of tags to mark commits when optimisations have been completed.
// (This will be very helpful for your trophy tracker!)
// You do not need to version the magpie framework code or assets.
// Put this code in git - GO DO IT NOW!!!
// A clean copy of this starter code, the framework code and the assets will always be available on Blackboard,
// so please just version this project code and clone it into Magpie's project folder on your machine.
//
// DO NOT make any changes to the Magpie framework code.
// NO MARKS will be awarded for extending/'fixing' the Magpie framework code in anyway.
// Andy will use a clean copy of Magpie and the assets to test your submission.
// (Andy has tested Magpie extensively, but if you genuinely think you have found a bug, please let him know:)
//
// This starter code base has been DELIBERATELY written badly to reduce code performance!
// This application's starter code is now YOURS! Alter/move/delete ANY part of it as YOU see fit!
// (There are only a couple of very small exceptions for setup/rendering code. Keep an eye out, they are clearly marked.)
// You are allowed to change ANY part of the application starter code for this assignment,
// just remember that the application must have the same behaviour/visual output when you are finished.
// Please see Blackboard for an exemplar executable of what the application should like when you are finished.
//
// Suggested approach to optimisation:
// 1.	Inspect Code/Trophies/Research/Plan/Think
// 2.	Implement optimisation
//   o  Add appropriate code comments
//   o  Ensure relevant code standards are followed, see “Code Standards” section below
// 3.	Test and gather times
//   o  It is highly recommended that you keep track of average frame times for both projects over the course of development, before and after each trophy is implemented
// 4.	Update your ‘Trophy Tracker’ spreadsheet
//   o  See “Filling in the ‘Trophy Tracker’” section below
// 5.	Show your code and spreadsheet to your tutor
// 6.	Repeat
//
// Remember, not all optimisations result in a noticeable/noteworthy reduction in computation time! (Remember Amdahl's Law?)
// This does NOT mean that they are necessarily invalid or incorrect.
//
// If there is ANY section of this application starter code that you feel is poorly documented
// or just do not understand, please do not hesitate to ask Andy for more details :)
//
// Please see the assignment specification document on Blackboard for full assignment details.
//
// Finally, this code base is littered with instances of what may appear to be 'uncommon'
// parts of the C++ language. If you don't know what it is... RESEARCH!
//
//
// MAGPIE NOTES:
//
// Magpie is the name of Andy's rendering framework :)
// Andy will show you how to build Magpie and how to get started in your sessions.
//
// In Magpie, the screen's origin is at the centre of the screen, up = +ve y, right = +ve x.
//
// In this application, window resizing/maximising has been disabled.
//
// Do not add .cpp/.h files directly here in VS!
// When/if you want to add/delete files:
// - place it in the projects folder
// - rerun the batch script :)
//
//
// Concept by: A.MacDougall - 2021
// Last revised: A.Hamilton - 2023



#include "constants.h"       // for PARTICLE_MAX
#include "particle_system.h" // for particle_system_t

#include "magpie.h"          // for magpie window/rendering components
#include "timer.h"

#include <optional>          // for non membe functions
#include <string>            // for variables with words


ENTRY_POINT
{
  ////////////////////////////////////////////////
  //// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
  ////////////////////////////////////////////////


  // RENDER SETUP

  magpie::renderer renderer;
#ifdef MAGPIE_PLATFORM_WINDOWS
  renderer.set_window_title ("SHOT2");
  renderer.set_window_initial_size (SCREEN_WIDTH, SCREEN_HEIGHT);
#endif // MAGPIE_PLATFORM_WINDOWS
  if (!renderer.initialise ())
  {
    MAGPIE_DASSERT (false);
  }


  ////////////////////////////////////////////////
  //// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
  ////////////////////////////////////////////////



  // SETUP
  particle_system_t particle_system;
  if (!particle_system.initialise (renderer))
  {
    MAGPIE_DASSERT (false);

  }

  long long num_active_particles = 0;


  //// frame timer
  //LARGE_INTEGER clock_freq;
  //QueryPerformanceFrequency (&clock_freq); // ask Windows for the CPU timer frequency
  //float const timer_multiplier_secs = (float)clock_freq.QuadPart;
  //magpie::printf ("Andy says hello\n");
  //LARGE_INTEGER qpc_start, qpc_end;
  //QueryPerformanceCounter (&qpc_start); // start frame timer
  //// have really small first frame elapsed seconds, rather than an unknown time


  // GAME LOOP
  Timer frametimer;
  timer_average avgTime;
  frametimer.start();
  while (renderer.process_os_messages ())
  {
    //QueryPerformanceCounter (&qpc_end); // end frame timer
    //float const elapsed_seconds = (float)(qpc_end.QuadPart - qpc_start.QuadPart) / timer_multiplier_secs;
    //magpie::printf ("FPS = %.2f - elapsed = %.5fs",
    //  1.0f / elapsed_seconds, // FPS
    //  elapsed_seconds);      // last frame time
    //QueryPerformanceCounter (&qpc_start); // start frame timer
      avgTime.start();

      frametimer.stop();
      float elapsed_seconds = frametimer.get_elapsed_s();
      frametimer.start();


    // UPDATE
    {
      particle_system.update (elapsed_seconds, num_active_particles);
    }


    // RENDER
    {
      ////////////////////////////////////////////////
      //// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
      ////////////////////////////////////////////////


      if (!renderer.pre_render ({ 0.f, 0.f, 0.f })) // black
      {
        MAGPIE_DASSERT (false);
      }


      ////////////////////////////////////////////////
      //// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
      ////////////////////////////////////////////////



      particle_system.render (renderer);



      ////////////////////////////////////////////////
      //// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
      ////////////////////////////////////////////////

      // render to window
      if (!renderer.post_render ())
      {
        MAGPIE_DASSERT (false);
      }


      ////////////////////////////////////////////////
      //// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
      ////////////////////////////////////////////////
    }


    //magpie::printf ("\nnumber of active particles = %d, All paricles are active: %s, ns/P = %.2f\n",
    //  num_active_particles,                                             // number of active particles
    //  num_active_particles == PARTICLE_MAX ? "YES" : "NO",              // all particles are active?
    //  elapsed_seconds * 1'000'000'000.f / (float)num_active_particles); // time (ns) per particle
    avgTime.stop();
    avgTime.print_to_file();
  }


  // RELEASE RESOURCES
  {
    particle_system.release (renderer);



    ////////////////////////////////////////////////
    //// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
    ////////////////////////////////////////////////


    renderer.release ();


    ////////////////////////////////////////////////
    //// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
    ////////////////////////////////////////////////
  }

  return 0;
}
