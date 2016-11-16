/******************************************************************************
 * ** Copyright (c) 2016, Intel Corporation                                     **
 * ** All rights reserved.                                                      **
 * **                                                                           **
 * ** Redistribution and use in source and binary forms, with or without        **
 * ** modification, are permitted provided that the following conditions        **
 * ** are met:                                                                  **
 * ** 1. Redistributions of source code must retain the above copyright         **
 * **    notice, this list of conditions and the following disclaimer.          **
 * ** 2. Redistributions in binary form must reproduce the above copyright      **
 * **    notice, this list of conditions and the following disclaimer in the    **
 * **    documentation and/or other materials provided with the distribution.   **
 * ** 3. Neither the name of the copyright holder nor the names of its          **
 * **    contributors may be used to endorse or promote products derived        **
 * **    from this software without specific prior written permission.          **
 * **                                                                           **
 * ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       **
 * ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         **
 * ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR     **
 * ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT      **
 * ** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    **
 * ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED  **
 * ** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    **
 * ** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    **
 * ** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      **
 * ** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        **
 * ** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * * ******************************************************************************/
/* Narayanan Sundaram (Intel Corp.)
 *  * ******************************************************************************/



#ifndef TEST_GENERATOR_HPP
#define TEST_GENERATOR_HPP

#include <cstdio>
#include <cstdlib>
#include <random>
#include "src/graphpad.h"

template <typename T=int>
GraphPad::edgelist_t<T> generate_identity_edgelist(int n) {
  int global_myrank = GraphPad::get_global_myrank();
  GraphPad::edgelist_t<T> identity_edgelist(n, n, 0);
  if (global_myrank == 0) {
    identity_edgelist = GraphPad::edgelist_t<T>(n, n, n); 
    for (int i = 0; i < n; i++) {
      identity_edgelist.edges[i].src = i+1;
      identity_edgelist.edges[i].dst = i+1;
      identity_edgelist.edges[i].val = 1;
    }
  }
  return identity_edgelist;
}

template <typename T>
bool dst_present(int dst, const GraphPad::edgelist_t<T>& e, const int start, const int end) {
  bool retval = false;
  for (int i = start; i < end; i++) {
    if (e.edges[i].dst == dst) {
      retval = true;
      break;
    }
  }
  return retval;
}

template <typename T=int>
GraphPad::edgelist_t<T> generate_random_edgelist(int n, int avg_nnz_per_row) {
  int global_myrank = GraphPad::get_global_myrank();
  GraphPad::edgelist_t<T> random_edgelist(n, n, 0);
  if (global_myrank == 0) {
    if (avg_nnz_per_row > n) { 
      avg_nnz_per_row = n;
    }
    random_edgelist = GraphPad::edgelist_t<T>(n, n, n*avg_nnz_per_row); 
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, n);

    int k = 0;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < avg_nnz_per_row; j++) {
        random_edgelist.edges[k].src = i+1;
        int dst;
        do {
          dst = dist(gen);
        } while(dst_present(dst, random_edgelist, i*avg_nnz_per_row, k));

        random_edgelist.edges[k].dst = dst;
        random_edgelist.edges[k].val = 1;
        k++;
      }
    }
  }
  return random_edgelist;
}

template <typename T=int>
GraphPad::edgelist_t<T> generate_upper_triangular_edgelist(int n) {
  int global_myrank = GraphPad::get_global_myrank();
  GraphPad::edgelist_t<T> ut_edgelist(n, n, 0);
  if (global_myrank == 0) {
    ut_edgelist = GraphPad::edgelist_t<T>(n, n, n*(n-1)/2); 
    int k = 0;
    for (int i = 0; i < n; i++) {
      for (int j = i+1; j < n; j++) {
        ut_edgelist.edges[k].src = i+1;
        ut_edgelist.edges[k].dst = j+1;
        ut_edgelist.edges[k].val = 1;
        k++;
      }
    }
  }
  return ut_edgelist;
}

template <typename T=int>
GraphPad::edgelist_t<T> generate_dense_edgelist(int n) {
  int global_myrank = GraphPad::get_global_myrank();
  GraphPad::edgelist_t<T> dense_edgelist(n, n, 0);
  if (global_myrank == 0) {
    dense_edgelist = GraphPad::edgelist_t<T>(n, n, n*(n-1)); 
    int k = 0;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (i != j) {
          dense_edgelist.edges[k].src = i+1;
          dense_edgelist.edges[k].dst = j+1;
          dense_edgelist.edges[k].val = 1;
          k++;
        }
      }
    }
  }
  return dense_edgelist;
}


//////////////////////////////////////////
// Sparse Vector generators //////////////
//////////////////////////////////////////


template <typename T=int>
GraphPad::edgelist_t<T> generate_dense_vector_edgelist(int n) {
  int global_myrank = GraphPad::get_global_myrank();
  GraphPad::edgelist_t<T> dense_edgelist(n, 1, 0);
  if (global_myrank == 0) {
    dense_edgelist = GraphPad::edgelist_t<T>(n, 1, n); 
    for (int i = 0; i < n; i++) {
      dense_edgelist.edges[i].src = i+1;
      dense_edgelist.edges[i].dst = 1;
      dense_edgelist.edges[i].val = 1;
    }
  }
  return dense_edgelist;
}

template <typename T=int>
GraphPad::edgelist_t<T> generate_random_vector_edgelist(int n, int nnz) {
  if (nnz > n) { 
    nnz = n;
    return generate_dense_vector_edgelist<T>(n);
  }

  int global_myrank = GraphPad::get_global_myrank();
  GraphPad::edgelist_t<T> random_edgelist(n, 1, 0);
  if (global_myrank == 0) {
    random_edgelist = GraphPad::edgelist_t<T>(n, 1, nnz); 
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, n);
  
    for (int j = 0; j < nnz; j++) {
      random_edgelist.edges[j].src = 1;
      int dst;
      do {
        dst = dist(gen);
      } while(dst_present(dst, random_edgelist, 0, j));

      random_edgelist.edges[j].dst = dst;
      random_edgelist.edges[j].val = 1;
    }

    for (int j = 0; j < nnz; j++) {
      std::swap(random_edgelist.edges[j].src, random_edgelist.edges[j].dst);
    }
  }
  return random_edgelist;
}

#endif 