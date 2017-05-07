/*
Developed by Sandeep Sharma with contributions from James E. Smith and Adam A. Homes, 2017
Copyright (c) 2017, Sandeep Sharma

This file is part of DICE.
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef HMULT_HEADER_H
#define HMULT_HEADER_H
#include <Eigen/Dense>
#include <Eigen/Core>
#ifndef SERIAL
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi.hpp>
#endif
#include "communicate.h"
#include "global.h"

using namespace Eigen;

std::complex<double> sumComplex(const std::complex<double>& a, const std::complex<double>& b) ;

namespace SHCISortMpiUtils{
  int ipow(int base, int exp);
};

struct Hmult2 {
  std::vector<std::vector<int> >& connections;
  std::vector<std::vector<CItype> >& Helements;

  Hmult2(std::vector<std::vector<int> >& connections_, std::vector<std::vector<CItype> >& Helements_)
  : connections(connections_), Helements(Helements_) {}

  template <typename Derived>
  void operator()(MatrixBase<Derived>& x, MatrixBase<Derived>& y) {
    y*=0.0;

#ifndef SERIAL
    boost::mpi::communicator world;
#endif

    int num_thrds = omp_get_max_threads();
    if (num_thrds >1) {
      std::vector<MatrixXx> yarray(num_thrds);

#pragma omp parallel
      {
	yarray[omp_get_thread_num()] = MatrixXx::Zero(y.rows(),1);
	//#pragma omp for schedule(dynamic)
	for (int i=0; i<x.rows(); i++) {
	  if ((i%(omp_get_num_threads() * world.size())
	       != world.rank()*omp_get_num_threads() + omp_get_thread_num())) continue;
	  for (int j=0; j<connections[i].size(); j++) {
	    CItype hij = Helements[i][j];
	    int J = connections[i][j];
	    yarray[omp_get_thread_num()](J,0) += hij*x(i,0);
#ifdef Complex
	    if (i!= J) yarray[omp_get_thread_num()](i,0) += conj(hij)*x(J,0);
#else
	    if (i!= J) yarray[omp_get_thread_num()](i,0) += hij*x(J,0);
#endif
	  }
	}

	for (int level=0; level<ceil(log2(omp_get_num_threads())); level++) {
#pragma omp barrier
	  if (omp_get_thread_num()%SHCISortMpiUtils::ipow(2,level+1) == 0 && omp_get_thread_num() + SHCISortMpiUtils::ipow(2,level) < omp_get_num_threads() ) {
	    int other_thrd = omp_get_thread_num()+SHCISortMpiUtils::ipow(2,level);
	    int this_thrd = omp_get_thread_num();
	    yarray[this_thrd] += yarray[other_thrd];
	  }
	}
      }

      //for (int i=1; i<num_thrds; i++)
      //yarray[0] += yarray[i];


      int size = yarray[0].rows();
#ifndef SERIAL
#ifndef Complex
      MPI_Reduce(&yarray[0](0,0), &y(0,0), size, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
      MPI_Bcast(&(y(0,0)), size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
#else
      boost::mpi::all_reduce(world, &yarray[0](0,0), size, &y(0,0), sumComplex);
#endif
#else
      y = 1.*yarray[0];
#endif

    }
    else {
      for (int i=0; i<x.rows(); i++) {
	if (i%world.size() != world.rank()) continue;
	for (int j=0; j<connections[i].size(); j++) {
	  CItype hij = Helements[i][j];
	  int J = connections[i][j];
	  y(J,0) += hij*x(i,0);

#ifdef Complex
	  if (i!= J) y(i,0) += conj(hij)*x(J,0);
#else
	  if (i!= J) y(i,0) += hij*x(J,0);
#endif
	}
      }

      CItype* startptr;
      MatrixXx ycopy;
      if (mpigetrank() == 0) {ycopy = MatrixXx(y.rows(), 1); ycopy=1.*y; startptr = &ycopy(0,0);}
      else {startptr = &y(0,0);}

#ifndef SERIAL
#ifndef Complex
      MPI_Reduce(startptr, &y(0,0), y.rows(), MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
#else
      boost::mpi::all_reduce(world, &y(0,0), y.rows(), &y(0,0), sumComplex);
#endif
#endif

    }
  }

};


#endif
