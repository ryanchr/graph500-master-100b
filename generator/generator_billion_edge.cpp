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

	double t1, t2;

	/*Record graph file load start time*/
	t1 = edgeset::timer::get_sec();

	in_graph.load_graph(infile_graph, num_g_edges, num_g_vertices);

	/*Record graph file load end time*/
	t2 = edgeset::timer::get_sec();

	std::cout<<"Input graph file load time: "<< (t2-t1) <<" sec\n";

	//
	int64_t num_g_out_v = (int64_t)num_kroneck_vertices * (int64_t)num_g_vertices;

	int64_t num_g_out_e = (int64_t)num_kroneck_edges * (int64_t)num_g_edges;

	std::cout<<"Edges: "<<num_g_out_e<<" Virtex: "<<num_g_out_v<<std::endl;
	
	string ofile_name = "graph_v" + to_string(num_g_out_v) + "_e" + to_string(num_g_out_e);

	/*Record write binary file start time*/
	t1 = edgeset::timer::get_sec();

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

			v_src_out = v_src_k * num_g_vertices + v_src_g;

			int src_m_id = get_machine_id(num_g_out_v, num_machines, v_src_out);
			
			for (auto v_dest_g : in_graph.out_edge_list[v_src_g])
			{

				if ( (v_dest_k == v_src_k) && (v_dest_g < v_src_g) )
					continue;

				//std::cout<<"v_src_g: "<<v_src_g<<", "<<"v_dest_g: "<<v_dest_g<<endl;

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
				
				int dest_m_id = get_machine_id(num_g_out_v, num_machines, v_dest_out);

				has_num_edges[src_m_id]++;
				has_num_edges[dest_m_id]++;
				//
				cnt ++;
				if (cnt % 200000000 == 0) std::cout<<"loaded 200000000 edges"<<std::endl;
	    	}
	    	

		}
	}
	//#endif
	//std::cout<<"something wrong?"<<endl;

	/*Record write binary file end time */
	t2 = edgeset::timer::get_sec();

	std::cout<<"Write time for generating binary graph file: "<< (t2-t1) <<" sec\n";

	//Print partition result
	std::cout<<"== Partition result: ";

	for(auto x_num: has_num_edges)
	{
		std::cout<<x_num<<" ";
	}
	std::cout<<std::endl;
 
	fout.close();

	/*Test read binary file time*/
	test_read_bin(ofile_name, num_g_out_v, num_machines);

	return cnt;
}


/*Read binary file*/
void test_read_bin( string file_name, int64_t num_vertices, int num_machines ){

	ifstream fin;

	fin.open(file_name.c_str(), ios::in | ios::binary);

	my_base_t v_src, v_dest;

	int cur_m_id = -1;

	double t_start, t_end;

	while ( fin.read(((char *)&v_src), sizeof(my_base_t))
		&& fin.read(((char *)&v_dest), sizeof(my_base_t)) ){
		
		int src_m_id = get_machine_id(num_vertices, num_machines, v_src);

		if (src_m_id != cur_m_id)	{

			if(cur_m_id > -1){
				
				t_end = edgeset::timer::get_sec();	
				std::cout<<"Load binary graph using "<<(t_end - t_start)<<" sec on machine "<<cur_m_id;
			}

			cur_m_id = src_m_id;	
			t_start = edgeset::timer::get_sec();	
		}
	}

	t_end = edgeset::timer::get_sec();
	cout<<"Load binary graph using "<<(t_end - t_start)<<" sec on machine "<<cur_m_id;

	fin.close();
}



