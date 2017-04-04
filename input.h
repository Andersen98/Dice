#ifndef INPUT_HEADER_H
#define INPUT_HEADER_H
#include <vector>
#include <list>
#include <boost/serialization/serialization.hpp>

using namespace std;

struct schedule{
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & davidsonTol				\
      & epsilon2				\
      & epsilon2Large                           \
      & SampleN					\
      & epsilon1				\
      & onlyperturbative			\
      & restart					\
      & fullrestart				\
      & dE					\
      & eps					\
      & prefix					\
      & stochastic				\
      & nblocks					\
      & excitation				\
      & nvirt					\
      & singleList				\
      & io                                      \
      & nroots                                  \
      & nPTiter                                 \
      & DoRDM                                   \
      & DoSpinRDM                               \
      & quasiQ                                  \
      & quasiQEpsilon                           \
      & doSOC                                   \
      & doSOCQDPT                               \
      & randomSeed                              \
      & doGtensor                               \
      & integralFile                            \
      & doResponse                              \
      & responseFile;
  }
  
public:
  double davidsonTol;
  double epsilon2;
  double epsilon2Large;
  int SampleN;
  std::vector<double> epsilon1;    
  bool onlyperturbative;
  bool restart;
  bool fullrestart;
  double dE;
  double eps;
  vector<string> prefix;
  bool stochastic;
  int nblocks;
  int excitation;
  int nvirt;
  bool singleList;
  bool io;
  int nroots;
  int nPTiter;
  bool DoRDM;
  bool DoSpinRDM;
  bool quasiQ;
  double quasiQEpsilon;
  bool doSOC;
  bool doSOCQDPT;
  unsigned int randomSeed;
  bool doGtensor;
  string integralFile;
  bool doResponse;
  string responseFile;
};

#endif
