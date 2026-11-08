#pragma once

/*! \brief Solver implements a fluid solver as presented in Real-Time Fluid Dynamics for Games
 *  
 * The paper can be found here:
 * https://web.archive.org/web/20060810043908/http://www.dgp.toronto.edu/people/stam/reality/Research/pdf/GDC03.pdf
 * The solver processes arrays for density (d, d0) and velocity (u, v, u0, v0). After each step, current results
 * are stored in the d, u, v arrays. In order to add density or velocity, the d0, u0 and v0 arrays can be modified.
 * 
 */
class Solver
{
   public:

      Solver() = default;

      void add_source(int N, float *x, float *s, float dt);
      void set_bnd(int N, int b, float *x);
      void lin_solve(int N, int b, float *x, float *x0, float a, float c);
      void diffuse(int N, int b, float *x, float *x0, float diff, float dt);
      void advect(int N, int b, float *d, float *d0, float *u, float *v, float dt);
      void project(int N, float *u, float *v, float *p, float *div);
      void solveDensity(int N, float *x, float *x0, float *u, float *v, float diff, float dt);
      void solveVelocity(int N, float *u, float *v, float *u0, float *v0, float visc, float dt);
};

