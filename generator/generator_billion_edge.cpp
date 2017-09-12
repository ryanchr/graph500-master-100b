/*
Author: Ren Chen
License:
*/

#include "make_graph.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include "generator_billion_edge.hpp"

using namespace std;

typedef edgeset::base_t my_base_t;

#define BINARY 1


unsigned get_machine_id(unsigned vertex_num
				      , unsigned machine_num
				      , unsigned vertex_id
				 )
{
	unsigned tmp_num = std::ceil(vertex_num*1.0/machine_num);
	unsigned id = vertex_id / tmp_num;
	//assert(id < machine_num);

	if(id >= machine_num)
	    std::cout<<"vid: "<<vertex_id<<" machine id: "<<id<<"num of machines "<<machine_num<<std::endl;
	return id;
}


int64_t gen_graph(//edgeset::GRAPH & kroneck_graph
	           std::vector<std::pair<int, int> > & k_graph
			 , my_base_t num_kroneck_vertices
			 , my_base_t num_kroneck_edges
			 , string infile_graph
			 , my_base_t num_g_vertices
			 , my_base_t num_g_edges
			 )
{
	//load input graph
	edgeset::GRAPH in_graph;
	ofstream fout;

	in_graph.load_graph(infile_graph, num_g_edges, num_g_vertices);

	//
	int64_t num_g_out_v = (int64_t)num_kroneck_vertices * (int64_t)num_g_vertices;
	int64_t num_g_out_e = (int64_t)num_kroneck_edges * (int64_t)num_g_edges;

	std::cout<<"Edges: "<<num_g_out_e<<" Virtex: "<<num_g_out_v<<std::endl;
	
	string ofile_name = "graph_v" + to_string(num_g_out_v) + "_e" + to_string(num_g_out_e);

	#if BINARY
		fout.open(ofile_name.c_str(), ios::out | ios::binary);
	#else
		fout.open(ofile_name.c_str(), ios::out);
	#endif

	if (!fout.is_open())
	{
		cout<<"failed to open the file"<<endl;
		return 0;
	}
	
	int64_t cnt = 0;

	int num_machines = 9;
	
	vector<int64_t> has_num_edges(num_machines, 0);

	my_base_t v_src_k, v_dest_k;

	#if 0
	///Triangle store

	//Output diagnoal triangles
	for (my_base_t i = 0; i < num_kroneck_vertices ; i++)
	{
		v_src_k = i;

	    v_dest_k = i;

		//std::cout<<"v_src_k"<<v_src_k<<", v_dest_k"<<v_dest_k<<", diagonal"<<endl;
	
		for (my_base_t v_src_g = 0; v_src_g < num_g_vertices; v_src_g ++)
		{
			assert(v_src_g < in_graph.out_edge_list.size());
			
			for (auto v_dest_g : in_graph.out_edge_list[v_src_g])
			{
				if (v_dest_g < v_src_g)
					continue;

				//std::cout<<"v_src_g"<<v_src_g<<", v_dest_g"<<v_dest_g<<", diagonal"<<endl;

				my_base_t v_src_out = v_src_k * num_g_vertices + v_src_g;

				my_base_t v_dest_out = v_dest_k * num_g_vertices + v_dest_g;
	
				#if BINARY 
					fout.write(reinterpret_cast<const char *> (&v_src_out), sizeof(v_src_out));
					fout.write(reinterpret_cast<const char *> (&v_dest_out), sizeof(v_dest_out));
				#else
					fout<<v_src_out;
					fout<<" ";
					fout<<v_dest_out;
					fout<<endl;
				#endif

				//cout<<"="<<v_src_g;
				//cout<<" ";
				//cout<<v_dest_g;
				//cout<<endl;
				//
				//cout<<"=="<<v_src_out;
				//cout<<" ";
				//cout<<v_dest_out;
				//cout<<endl;

				//
				int src_m_id = get_machine_id(num_g_out_v, num_machines, v_src_out);
				int dest_m_id = get_machine_id(num_g_out_v, num_machines, v_dest_out);
				has_num_edges[src_m_id]++;
				has_num_edges[dest_m_id]++;

				//
				cnt ++;
				if (cnt % 200000000 == 0)
					cout<<"loaded 200000000 edges"<<endl;
			}
		}
	}
	#endif

	my_base_t v_src_out, v_dest_out;

	for (auto k_edges : k_graph)
	{

		v_src_k = k_edges.first;

		v_dest_k = k_edges.second;

		//std::cout<<"v_src_k: "<<v_src_k<<", "<<"v_dest_k: "<<v_dest_k<<endl;

		if ( v_dest_k < v_src_k )
			continue;

		
		for ( my_base_t v_src_g = 0; v_src_g < num_g_vertices; v_src_g++ )
		{
			assert(v_src_g < in_graph.out_edge_list.size());

			
			for (auto v_dest_g : in_graph.out_edge_list[v_src_g])
			{

				if ( (v_dest_k == v_src_k) && (v_dest_g < v_src_g) )
					continue;

				//std::cout<<"v_src_g: "<<v_src_g<<", "<<"v_dest_g: "<<v_dest_g<<endl;

				v_src_out = v_src_k * num_g_vertices + v_src_g;
				v_dest_out = v_dest_k * num_g_vertices + v_dest_g;

				
				#if BINARY 
					fout.write(reinterpret_cast<const char *> (&v_src_out), sizeof(v_src_out));
					fout.write(reinterpret_cast<const char *> (&v_dest_out), sizeof(v_dest_out));
				#else
					fout<<v_src_out;
					fout<<" ";
					fout<<v_dest_out;
					fout<<endl;
				#endif
				
				//cout<<v_src_out;
				//cout<<" ";
				//cout<<v_dest_out;
				//cout<<endl;
				//
				int src_m_id = get_machine_id(num_g_out_v, num_machines, v_src_out);
				int dest_m_id = get_machine_id(num_g_out_v, num_machines, v_dest_out);

				//assert(src_m_id < num_machines);
				//assert(dest_m_id < num_machines);

				//std::cout<<"src_m_id "<<src_m_id<<" dest_m_id "<<dest_m_id<<std::endl;
				//
				has_num_edges[src_m_id]++;
				has_num_edges[dest_m_id]++;
				//
				cnt ++;
				if (cnt % 200000000 == 0)
					std::cout<<"loaded 200000000 edges"<<std::endl;
	    	}
	    	

		}
	}
	//#endif
	//std::cout<<"something wrong?"<<endl;

	//Print partition result
	std::cout<<"== Partition result: ";

	for(auto x_num: has_num_edges)
	{
		std::cout<<x_num<<" ";
	}
	std::cout<<std::endl;
 
	fout.close();

	return cnt;
}