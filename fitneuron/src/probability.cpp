#ifndef _PROBABILITY_CPP_
#define _PROBABILITY_CPP_
#include "../include/probability.h"

//############# Definitions for GslRandGen ########################
bool GslRandGen::firstInit = true;

GslRandGen::GslRandGen()
{
  if(firstInit) {
    gsl_rng_env_setup();
    firstInit = false;
  }

  r = gsl_rng_alloc(gsl_rng_default);

  timeval tv;
  gettimeofday(&tv, NULL);
  long ticks = (long)(1000000 * tv.tv_sec + tv.tv_usec);
  gsl_rng_set(r, ticks);
}
GslRandGen::GslRandGen(const long & seed)
{
  if(firstInit) {
    gsl_rng_env_setup();
    firstInit = false;
  }

  r = gsl_rng_alloc(gsl_rng_default);
  gsl_rng_set(r, seed);
}
GslRandGen::~GslRandGen()
{
  gsl_rng_free(r);
}

double GslRandGen::uniform(void)
{
  return  gsl_rng_uniform(r);
}
double GslRandGen::openUniform(void)
{
  return gsl_rng_uniform_pos(r);
}
unsigned long GslRandGen::unsignedInt(const unsigned long & n)
{
  return gsl_rng_uniform_int(r, n);
}

bool GslRandGen::bernoulli(const double & p)
{
  return (gsl_ran_bernoulli(r, p) == 1);
}
double GslRandGen::exponential(const double & mu)
{
  return gsl_ran_exponential(r, mu);
}
double GslRandGen::gaussian(const double & sigma)
{
  return gsl_ran_gaussian_ziggurat(r, sigma);
}
unsigned int GslRandGen::poisson(const double & mu)
{
  return gsl_ran_poisson(r, mu);
}
unsigned int GslRandGen::binomial(const double & p, unsigned int n)
{
  return gsl_ran_binomial(r, p, n);
}
#endif
