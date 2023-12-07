// HOW IT WORKS:
//
// Simple CPU particle simulation.
// Particle are emitted from 3 locations,
// with differing position offsets, velocities, accelerations and start & end colours.
// Particle types are split evenly between all 3 types.
// Each particle is represented by a single pixel.
//
// Each particle's position is controlled by:
//   position       |                                                     | per particle      | random within a range
//   velocity       | speed and direction change, per second              | per particle      | random within a range
//   acceleration   | velocity change, per second                         | per particle type | fixed
//
// Each particle's lifetime is controlled by:
//   life_time      | maximum amount of time the particle can live for    | per particle      | random within a range
//   life_remaining | initially set to life_time, countdown to 0          | per particle      | random within a range
//   kill_y         | the minimum position.y value before being destroyed | per particle type | fixed
//
// Each particle's colour is determined by the ratio between life_remaining and life_time.
// The start and end colours are fixed and are the same for each particle type.



#pragma once

#include "constants.h"
#include "extra/particle_renderer_2d.h"

#include "magpie.h"

#include <bitset>   // for
#include <list>     // for std::list
#include <random>   // for std::random_device, std::uniform_real_distribution, std::uniform_int_distribution

int const threads = 4;

// UTILITY

struct vector4
{
  double x;
  double y;
  double z;
  double w;
};

struct colourf
{
  double r;
  double g;
  double b;
  double a;
};


/// <summary>
/// returns a random number between min and max inclusive
/// </summary>
/// <param name="min">minimum random number (inclusive)</param>
/// <param name="max">maximum random number (inclusive)</param>
/// <returns>random number between min & max (inclusive)</returns>
static double random_getd (double min, double max)
{
  MAGPIE_DASSERT (max >= min);

  static std::random_device rd;
  std::uniform_real_distribution <double> distribution (min, max);

  return distribution (rd);
}

/// <summary>
/// returns a random number between min and max inclusive
/// </summary>
/// <param name="min">minimum random number (inclusive)</param>
/// <param name="max">maximum random number (inclusive)</param>
/// <returns>random number between min & max (inclusive)</returns>
static long long random_geti (long long min, long long max)
{
  MAGPIE_DASSERT (max >= min);

  static std::random_device rd;
  std::uniform_int_distribution <i64> distribution (min, max);

  return distribution (rd);
}

/// <summary>
/// linearly interpolate value between v0 and v1 based on the 0.0 - 1.0 value of t
/// </summary>
/// <param name="v0">start value</param>
/// <param name="v1">end value</param>
/// <param name="t">lerp proportion</param>
/// <returns>interpolated value</returns>
static double lerp (double v0, double v1, double t)
{
  return ((1.0 - t) * v0) + (t * v1);
}


// PARTICLES

class particle
{
public:
  /// <summary>
  /// update particle's position, lifetime & colour
  /// </summary>
  /// <param name="elapsed_seconds">elapsed time since last frame</param>
  /// <returns>true, if particle has expired and needs deleting</returns>
  virtual bool process (double elapsed_seconds) = 0;

  double  life_time = {};
  double  life_remaining = {};
  double  kill_y = {};

  vector4 position = {};
  vector4 velocity = {};
  vector4 acceleration = {};

  colourf colour = {};
  colourf start_colour = {};
  colourf end_colour = {};
};

class particle_a : public particle
{
public:
  particle_a ()
  {
    // left hand side of screen

    life_time = life_remaining = random_getd (7.5, 13.0);
    kill_y = -(double)SCREEN_HEIGHT / 2.0;

    position = { -(double)SCREEN_WIDTH / 2.0 + random_getd (0.0, 200.0),
      -(double)SCREEN_HEIGHT / 2.0 + random_getd (0.0, 100.0),
      0.0, 0.0 };
    velocity = { random_getd (magpie::maths::cos (magpie::maths::radians (89.0)), magpie::maths::cos (magpie::maths::radians (75.0))) * 200.f,
      random_getd (magpie::maths::sin (magpie::maths::radians (75.0)), magpie::maths::sin (magpie::maths::radians (89.0))) * 200.f,
      0.0, 0.0 };
    acceleration = { 2.0, -26.5, 0.0, 0.0 };

    start_colour = { 1.0, 0.2, 0.2, 1.0 }; // red
    end_colour = { 0.2, 1.0, 1.0, 1.0 }; // inverse red
  }

  bool process (double elapsed_seconds) override
  {
    // update linear motion
    position.x += velocity.x * elapsed_seconds;
    position.y += velocity.y * elapsed_seconds;
    position.z += velocity.z * elapsed_seconds;
    position.w += velocity.w * elapsed_seconds;

    velocity.x += acceleration.x * elapsed_seconds;
    velocity.y += acceleration.y * elapsed_seconds;
    velocity.z += acceleration.z * elapsed_seconds;
    velocity.w += acceleration.w * elapsed_seconds;

    // update colour
    colour.r = lerp (end_colour.r, start_colour.r, life_remaining / life_time);
    colour.g = lerp (end_colour.g, start_colour.g, life_remaining / life_time);
    colour.b = lerp (end_colour.b, start_colour.b, life_remaining / life_time);
    colour.a = lerp (end_colour.a, start_colour.a, life_remaining / life_time);

    // update life remaining
    life_remaining -= elapsed_seconds;

    // is particle still alive?
    if (life_remaining <= 0.0)
      return true;
    else if (position.y < kill_y)
      return true;
    else
      return false;
  }
};

class particle_b : public particle
{
public:
  particle_b ()
  {
    // middle of screen

    life_time = life_remaining = random_getd (9.0, 10.0);
    kill_y = -(double)SCREEN_HEIGHT / 2.0 + 50.0;

    position = { random_getd (0.0, (double)SCREEN_WIDTH / 3.0),
      (double)SCREEN_HEIGHT / 2.0,
      0.0, 0.0 };
    velocity = { -50.0,
      random_getd (-100.0, -60.0),
      0.0, 0.0 };
    acceleration = { 0.0, 0.0, 0.0, 0.0 };

    start_colour = { 0.2, 1.0, 0.2, 1.0 }; // green
    end_colour = { 1.0, 0.2, 1.0, 1.0 }; // inverse green
  }

  bool process (double elapsed_seconds) override
  {
    // update linear motion
    position.x += velocity.x * elapsed_seconds;
    position.y += velocity.y * elapsed_seconds;
    position.z += velocity.z * elapsed_seconds;
    position.w += velocity.w * elapsed_seconds;

    velocity.x += acceleration.x * elapsed_seconds;
    velocity.y += acceleration.y * elapsed_seconds;
    velocity.z += acceleration.z * elapsed_seconds;
    velocity.w += acceleration.w * elapsed_seconds;

    // update colour
    colour.r = lerp (end_colour.r, start_colour.r, life_remaining / life_time);
    colour.g = lerp (end_colour.g, start_colour.g, life_remaining / life_time);
    colour.b = lerp (end_colour.b, start_colour.b, life_remaining / life_time);
    colour.a = lerp (end_colour.a, start_colour.a, life_remaining / life_time);

    // update life remaining
    life_remaining -= elapsed_seconds;

    // is particle still alive?
    if (position.y < kill_y)
      return true;
    else if (life_remaining <= 0.0)
      return true;
    else
      return false;
  }
};

class particle_c : public particle
{
public:
  particle_c ()
  {
    // right hand side of screen

    life_time = life_remaining = random_getd (3.5, 6.0);
    kill_y = -(double)SCREEN_HEIGHT / 2.0 + 15.0;

    position = { (double)SCREEN_WIDTH / 2.0 - 300.0,
      -(double)SCREEN_HEIGHT / 2.0 + 400.0,
      0.0, 0.0 };
    velocity = { random_getd (-50.0, 50.0),
      random_getd (-50.0, 50.0),
      0.0, 0.0 };
    acceleration = { 0.0, 0.0, 0.0, 0.0 };

    start_colour = { 0.2, 0.2, 1.0, 1.0 }; // blue
    end_colour = { 1.0, 1.0, 0.2, 1.0 }; // inverse blue
  }

  bool process (double elapsed_seconds) override
  {
    // update linear motion
    position.x += velocity.x * elapsed_seconds;
    position.y += velocity.y * elapsed_seconds;
    position.z += velocity.z * elapsed_seconds;
    position.w += velocity.w * elapsed_seconds;

    velocity.x += acceleration.x * elapsed_seconds;
    velocity.y += acceleration.y * elapsed_seconds;
    velocity.z += acceleration.z * elapsed_seconds;
    velocity.w += acceleration.w * elapsed_seconds;

    // update colour
    colour.r = lerp (end_colour.r, start_colour.r, life_remaining / life_time);
    colour.g = lerp (end_colour.g, start_colour.g, life_remaining / life_time);
    colour.b = lerp (end_colour.b, start_colour.b, life_remaining / life_time);
    colour.a = lerp (end_colour.a, start_colour.a, life_remaining / life_time);

    // update life remaining
    life_remaining -= elapsed_seconds;

    // is particle still alive?
    if (life_remaining <= 0.0)
      return true;
    else if (position.y < kill_y)
      return true;
    else
      return false;
  }
};


// PARTICLE SYSTEM

/// <summary>
/// update all active particles
/// remove expired particles
/// </summary>
/// <param name="particles">list of particle pointers</param>
/// <param name="elapsed_seconds">elapsed frame time</param>
/// <returns>updated list of pointers to particles</returns>
static std::list <particle*> process (std::list <particle*> particles, double elapsed_seconds)
{
  // iterators provide a generic way to access the data at a particular element of a container
  // e.g. vectors, lists and maps // https://en.cppreference.com/w/cpp/container
  // iterators are 'special' in that they can be incremented to go to the next element in the collection
  // (even if it is not physically next to it in memory // https://en.cppreference.com/w/cpp/iterator)
  std::list <particle*>::iterator it = particles.begin ();
  while (it != particles.end ())
  {
    // get pointer to particle from iterator
    particle* p = *it;
    MAGPIE_DASSERT (p);

    // update particle, returns true of needs deleting
    if (p->process (elapsed_seconds))
    {
      // particle needs deleting

      // release data previously allocated for this particle
      // no effect if p == nullptr
      delete p;

      // remove pointer to now released particle from list
      // std::list::erase () returns next valid element :)
      it = particles.erase (it);
    }
    else
    {
      // increment iterator here if this particle was not removed
      it++;
    }
  }

  return particles;
}
/// <summary>
/// create/add new particles to the list
/// </summary>
/// <param name="particles">list of particle pointers</param>
/// <param name="elapsed_seconds">elapsed frame time</param>
/// <returns>updated list of pointers to particles</returns>
static std::list <particle*> emit (std::list <particle*> particles, double elapsed_seconds)
{
  long long num_particles_spawned = 0u;
  int particle_type = 0;
  for (float i = 0.f; i < (float)PARTICLE_MAX * 2.f; i += 1.f)
  {
    // make sure we never exceed maximum particle budget
    if (particles.size () == PARTICLE_MAX)
    {
      magpie::printf ("num particles == PARTICLE_MAX\n");
      continue;
    }
    // make sure we never exceed frame's particle budget
    if (num_particles_spawned == PARTICLE_SPAWN_RATE)
    {
      continue;
    }
    // keep track of how many particles have been emitted this frame
    num_particles_spawned++;

    // add particle
    // evenly spread particles between each type
    if (particle_type == 0)
    {
      particles.push_back (new particle_a);
      magpie::printf ("spawn particle a\n");
    }
    else if (particle_type == 1)
    {
      particles.push_back (new particle_b);
    }
    else // particle_type == 2
    {
      particles.push_back (new particle_c);
    }
    // create the next type of particle on the next iteration
    particle_type++;
    // 'wrap' particle type so its always valid, 0 <-> { NUM_PARTICLE_TYPES - 1 }
    particle_type = particle_type % NUM_PARTICLE_TYPES;
  }

  return particles;
}

void worker(std::list <particle*>& particles, double elapsed_seconds)
{


    particles = process(particles, elapsed_seconds);
    particles = emit(particles, elapsed_seconds);



}

class particle_system_t
{
public:
  bool initialise (magpie::renderer& renderer)
  {
    // other important stuff...

    return particle_renderer.initialise (PARTICLE_MAX);
  }

  void update (double elapsed_seconds, long long& num_active_particles)
  {
      for (int i = 0; i < threads; ++i) {
          particles[i] = process(particles[i], elapsed_seconds);
          particles[i] = emit(particles[i], elapsed_seconds);
          .emplace_back(worker, i);
          
      }
      //num_active_particles = particles[i].size();
  }
  void render (magpie::renderer& renderer)
  {
    magpie::printf ("rendering particles\n");
    for (int i = 0; i < threads; ++i) {
        for (particle const* p : particles[i])
        {
            particle_renderer.draw(renderer,
                p->position.x, p->position.y, p->position.z, p->position.w,
                p->colour.r, p->colour.g, p->colour.b, p->colour.a);
        }
    }


    ////////////////////////////////////////////////
    //// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
    ////////////////////////////////////////////////


    particle_renderer.render (renderer);


    ////////////////////////////////////////////////
    //// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
    ////////////////////////////////////////////////
  }

  void release(magpie::renderer& renderer)
  {
      ////////////////////////////////////////////////
      //// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
      ////////////////////////////////////////////////


      particle_renderer.release(renderer);


      ////////////////////////////////////////////////
      //// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
      ////////////////////////////////////////////////



      // delete all particles
      for (int i = 0; i < threads; ++i) {


          std::list <particle*>::iterator it = particles[i].begin();
          while (it != particles[i].end())
          {
              // get pointer to particle from iterator
              particle* p = *it;

              // release data previously allocated for this particle
              // no effect if p == nullptr
              delete p;

              // remove pointer to now released particle from list
              // std::list::erase () returns next valid element :)
              it = particles[i].erase(it);
          }
      }
  }

private:
  particle_renderer_2d particle_renderer;
  std::list <particle*> particles[threads];

};
