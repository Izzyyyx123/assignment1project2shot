// HOW IT WORKS:
//
// particle_renderer_2d contains an array of vertices.
// Each vertex represents a 2D point (a single pixel).
// The user provides a series of positions and colours and stores them in the array.
// Each point is then rendered in a SINGLE draw call (batch rendered).


////////////////////////////////////////////////
//// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
////////////////////////////////////////////////


#pragma once

#include "magpie.h"

#include <vector> // for std::vector


struct particle_renderer_2d
{
public:
  bool initialise (unsigned max_particles)
  {
    MAGPIE_DASSERT_MSG (max_particles > 0u, "max_particles must be greater than 0");


    this->max_particles = max_particles;

    vertices.resize (max_particles);

    return true;
  }


////////////////////////////////////////////////
//// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
////////////////////////////////////////////////



  bool draw (magpie::renderer const& renderer,
    float position_x, float position_y,
    float colour_r, float colour_g, float colour_b, float colour_a)
  {
    if (num_particles == max_particles - 1u)
    {
      MAGPIE_DPRINTF ("This is the last particle that can be drawn! Consider increasing max_particles\n");
    }
    if (num_particles == max_particles)
    {
      magpie::printf ("There is not enough memory to draw another particle! Consider increasing max_particles\n");
      return false;
    }


    vertices [num_particles++] = sf::Vertex (
      sf::Vector2f ((float)position_x, (float)position_y),
      sf::Color ((sf::Uint8)((float)colour_r * (float)UCHAR_MAX), // take colour from between 0 & 1 to 0 & 255
        (sf::Uint8)((float)colour_g * (float)UCHAR_MAX),
        (sf::Uint8)((float)colour_b * (float)UCHAR_MAX),
        (sf::Uint8)((float)colour_a * (float)UCHAR_MAX)));


    return true;
  }



////////////////////////////////////////////////
//// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
////////////////////////////////////////////////


  void render (magpie::renderer& renderer)
  {
    if (num_particles == 0u)
    {
      return;
    }

    renderer.get_window ().draw (vertices.data (), num_particles, sf::PrimitiveType::Points);

    num_particles = 0u;
  }

  void release (magpie::renderer&)
  {
    vertices.clear ();
  }

private:
  std::vector <sf::Vertex> vertices;

  unsigned max_particles = 0u, num_particles = 0u;
};


////////////////////////////////////////////////
//// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
////////////////////////////////////////////////
