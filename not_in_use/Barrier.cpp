#include "Barrier.h"

IBarrier::IBarrier(const Barrier& bar)
{
  barrier = bar.Clone();
}

double IBarrier::BarrierPayoff(double S, double t, double continuation) const
{
  return barrier->BarrierPayoff(S, t, continuation);
}

IBarrier::~IBarrier()
{
  if (barrier)
    delete barrier;
}

IBarrier::IBarrier(const IBarrier& _bar)
{
  barrier = _bar.barrier->Clone();
}

IBarrier& IBarrier::operator=(const IBarrier& _bar)
{
  if (this != &_bar) { // not self assignment
    if (barrier)
      delete barrier;
    barrier = _bar.barrier->Clone();
  }
  return *this;
} 
