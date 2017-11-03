/* Copyright (C) 2009-2010 The Trustees of Indiana University.             */
/*                                                                         */
/* Use, modification and distribution is subject to the Boost Software     */
/* License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at */
/* http://www.boost.org/LICENSE_1_0.txt)                                   */
/*                                                                         */
/*  Authors: Jeremiah Willcock                                             */
/*           Andrew Lumsdaine                                              */


#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include <stdio.h>
#include <omp.h>
#include <string.h>

#include "make_graph.h"
#include "generator_billion_edge.hpp"

#include <set>
#include <utility>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>

typedef unsigned int my_base_t;

//using namespace std;

my_base_t getHash (my_base_t a, my_base_t b)
{
    //Szudzik's function
    return (a >= b) ? (a*a + a + b) : (b*b +a) ;
}


my_base_t getSize(std::vector<bool> &vec_in)
{
  int i = vec_in.size() - 1;
  int cnt = 0;
  while (i >= 0)
  {
    if (vec_in[i] == 1)
      cnt ++;
    i--;
  }

  return cnt;
}

//load kronecker graph
void load_k_graph(std::string ifile, std::vector<std::pair<int,int> > & k_graph)
{
    std::ifstream fin;
    fin.open(ifile.c_str());

    std::string line;
    int src_id, dest_id;

    while (getline(fin, line))
    {
        std::istringstream line_s(line);
        line_s >> src_id;
        line_s >> dest_id;
        k_graph.push_back(std::make_pair(src_id, dest_id));
    }

    fin.close();

    return;
}


int main(int argc, char* argv[]) {
  int log_numverts;
  //double start, time_taken;
  //int64_t nedges, actual_nedges;

  //packed_edge* result;
  int ratio = 2;
  bool output_k = 0;

  std::string infile_g, k_graph_file;
  int64_t num_v_k = 0, num_e_k = 0;

  my_base_t in_g_num_v = 0;
  my_base_t in_g_num_e = 0;

  log_numverts = 2; /* In base 2 */

  if (argc < 7){
    std::cerr << "Format: exe num_v_k ratio_e_to_v graph_file_name graph_v_num graph_e_num output_k.  or \n";
    std::cerr << "Format: exe k_graph_file num_v_k num_e_k graph_file_name graph_v_num graph_e_num output_k.   \n";
    return 0;
  }
  else if (argc == 7){

    log_numverts = atoi(argv[1]);
    ratio = atoi(argv[2]);
    infile_g = argv[3];
    in_g_num_v = atoi(argv[4]);
    in_g_num_e = atoi(argv[5]);
    output_k = bool(atoi(argv[6]));

  }
  else if (argc == 8){

    k_graph_file = argv[1];
    num_v_k = atoi(argv[2]);
    num_e_k = atoi(argv[3]);
    infile_g = argv[4];
    in_g_num_v = atoi(argv[5]);
    in_g_num_e = atoi(argv[6]);
    output_k = bool(atoi(argv[7]));

  }

  //int64_t nvertex = 1 << log_numverts;

  /* Start of graph generation timing */
  //start = omp_get_wtime();


  size_t num_v = 0, num_e = 0;
  #if 0
  make_graph(log_numverts, ratio << log_numverts, 1, 2, &nedges, &result);

  time_taken = omp_get_wtime() - start;
  /* End of graph generation timing */

  //printf("Wrong?\n");

  actual_nedges = nedges;
  int64_t numv = 1 << log_numverts;

  packed_edge* tmp = result;
  vector<packed_edge> new_graph;

  std::vector<bool> verticies(numv, 0);
  std::vector<bool> edges(nedges, 0);
  std::set<my_base_t> edges_m;

  char filename[80];
  char size[30];
  char size_v[30];
  sprintf(size, "%" PRId64, actual_nedges);
  sprintf(size_v, "%" PRId64, numv);
  strcpy(filename, "out_e");
  strcat(filename, size);
  strcat(filename, "_v");
  strcat(filename, size_v);

  //printf("Wrong?\n");

  FILE *fp;
  fp = fopen(filename, "w");
  if (fp == NULL)
       fprintf(stderr, "Failed to open the file");  


  
  //printf("Wrong?\n");

  for(int i = 0; i < actual_nedges; i ++)
  {

    my_base_t key = getHash(my_base_t(tmp->v0), my_base_t(tmp->v1));

    while(my_base_t(tmp->v0) >= verticies.size())
      verticies.push_back(0);

    while(my_base_t(tmp->v1) >= verticies.size())
      verticies.push_back(0);

    
    if ( verticies[tmp->v0] == 0 )
    {
      verticies[tmp->v0] = 1;
      num_v++;
    }
    
    if ( verticies[tmp->v1] == 0 )
    {
      verticies[tmp->v1] = 1;
      num_v++;
    }

    if ( tmp->v0 != tmp->v1 )
    {
      if (edges_m.find(key) == edges_m.end() )
      {
        edges_m.insert(key);

        if (output_k)
          fprintf(fp, "%d %d\n", my_base_t(tmp->v0), my_base_t(tmp->v1)); 
        packed_edge tmp_new;
        tmp_new.v0 = tmp->v0;
        tmp_new.v1 = tmp->v1;
        new_graph.push_back(tmp_new);
        num_e++;
      }
    }

    tmp ++;
  }


  fclose(fp);
  free(result);

  #endif

  int64_t num_edges;
  int64_t num_vertex;
  //edgeset::GRAPH k_graph;

  if (argc == 7)
  {
    edgeset::GRAPH k_graph;
    num_edges = num_e;
    num_vertex = num_v;
    k_graph.load_graph(std::string(""), num_edges, num_vertex);
  }
  else if (argc == 8)
  {
    num_edges = num_e_k;
    num_vertex = num_v_k;
    //k_graph.load_graph(std::string(k_graph_file), num_edges, num_vertex);
  }
  //int64_t num_deges = nedges, num_vertex = nvertex;


  // sload Kronecker graph
  std::vector<std::pair<int, int> > k_graph;
  load_k_graph(k_graph_file, k_graph);
  
  // int64_t num_out_e = 0;
  int64_t num_out_e = gen_graph(k_graph
                              , num_v_k
                              , num_e_k
                              , infile_g
                              , in_g_num_v
                              , in_g_num_e
                              , 2
                              );

  // string test_file = "graph_v65608366_e1806067135";

  // test_read_bin(  test_file
  //                ,65608366
  //                ,9 );

  // test_read_bin_all( test_file
                    // ,65608366
 	                  // ,9 );

  //printf("Generated " "%" PRIu64 " edges " "%" PRIu64 " vertices \n",    
  //       (int64_t)num_out_e         
  //     , (int64_t)(num_v_k*in_g_num_v) );

  // std::cout<<"Generated "<<num_out_e<<" edges, "<<(int64_t)(num_v_k*in_g_num_v)<<" vertices \n";

  return 0;
}
