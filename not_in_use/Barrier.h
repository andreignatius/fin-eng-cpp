#ifndef BARRIER
#define BARRIER

class Barrier;

class IBarrier
{
 public:
  double BarrierPayoff(double S, double t, double continuation) const;
  IBarrier(const Barrier& bar);

  // rule of three ------------------------------------
  ~IBarrier();
  IBarrier(const IBarrier& _bar);
  IBarrier& operator=(const IBarrier& _bar);

 private:
  Barrier* barrier;
};

// barrier interface
class Barrier
{
  friend class IBarrier;
 public:
  virtual double BarrierPayoff(double S, double t, double continuation) const = 0;
  virtual ~Barrier() {}
 protected:
  virtual Barrier* Clone() const = 0;
};

class UpBarrier : public Barrier
{
 public:
 UpBarrier(double upLevel)
     : upBarrierLevel(upLevel){}
  virtual double BarrierPayoff(double S, double t, double continuation) const
  {
    return S>=upBarrierLevel ? 0 : continuation;
  }
 protected:
  virtual Barrier* Clone() const
  {
    return (new UpBarrier(*this));
  }
 private:
  double upBarrierLevel;
};

class DownBarrier : public Barrier
{
 public:
  DownBarrier(double downLevel)
   : downBarrierLevel(downLevel){}
  virtual double BarrierPayoff(double S, double t, double continuation) const
  {
    return S<=downBarrierLevel ? 0 : continuation;
  }
 protected:
  virtual Barrier* Clone() const
  {
    return (new DownBarrier(*this));
  }
 private:
  double downBarrierLevel;
};

#endif
