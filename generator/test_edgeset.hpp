#ifndef EDGE_SET_H
#define EDGE_SET_H

#include <tr1/unordered_map>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>
#include <stdint.h>
#include <assert.h>
#include <string>
#include <fstream>
#include <deque>
//#include <openG.h>
#include <sys/time.h>
#include <algorithm>
#include <thread>

//Eywa header files
//#include "dynamic_local_graph"
using namespace std;

#define MAX_DEGREE 26

namespace edgeset
{

#define SIZE 0x0ffff
//typedef size_t unsigned int;

class GRAPH
{
public:
	std::vector<unsigned> vertex_list;
	std::vector<std::vector<unsigned> > in_edge_list;
	std::vector<std::vector<unsigned> > out_edge_list;
	
    bool load_Graph(const std::string ofile)
    {
        std::ifstream fin;
		
		fin.open(ofile.c_str());
		if(!fin.is_open())
			return false;
		
		//unsigned num_edge = std::count(std::istreambuf_iterator<char>(fin),
		//		   std::istreambuf_iterator<char>(), '\n');
		
		vertex_list.reserve(70000000);
		
		in_edge_list.reserve(70000000*MAX_DEGREE);
		
		out_edge_list.reserve(70000000*MAX_DEGREE);
		
		//std::cout<<"1:"<<ofile<<endl;
		std::string src_id, dest_id;
		//std::string seperate;
		
		std::cout<<"Start load data"<<endl;
		unsigned cnt = 0;
			
		while (fin.good())
		{
			fin >> src_id;
			fin >> dest_id;
			
			unsigned itn_src_id = external_to_internel_id(src_id);
			unsigned itn_dest_id = external_to_internel_id(dest_id);
			
			//if (cnt < 20)
			//{
			//	std::cout<<"src_id: "<<src_id<<" dest_id: "<<dest_id<<endl;
			//	std::cout<<"itn_src_id: "<<itn_src_id<<" itn_dest_id: "<<itn_dest_id<<endl;
			//	cnt ++;
			//}
		
			if (itn_dest_id < in_edge_list.size())
				in_edge_list[itn_dest_id].push_back(itn_src_id);
			else
			{
				while(itn_dest_id >= in_edge_list.size())
				{
				std::vector<unsigned> tmp;
				in_edge_list.push_back(tmp);
				}
				in_edge_list[itn_dest_id].push_back(itn_src_id);
				//std::cout<<"itn_dest_id: "<<itn_dest_id<<" in_edge_list_size: "<<in_edge_list.size()<<endl;
				
				assert(itn_dest_id < in_edge_list.size());
			}
			
			if (itn_src_id < out_edge_list.size())
				out_edge_list[itn_src_id].push_back(itn_dest_id);
			else
			{
				while(itn_src_id >= out_edge_list.size())
				{
				std::vector<unsigned> tmp;
				out_edge_list.push_back(tmp);
				}
				
				out_edge_list[itn_src_id].push_back(itn_dest_id);
				//std::cout<<"itn_src_id: "<<itn_dest_id<<" out_edge_list_size: "<<out_edge_list.size()<<endl;
			
				assert(itn_src_id < out_edge_list.size());
			}
			
			edge_num ++;
		}
		
		for (unsigned i = 0; i < num_vertex; i++)
			vertex_list.push_back(i);
		
		fin.close();
		
        return true;
    }

	unsigned get_vertex_num()
	{
		return num_vertex;//in_edge_list.size();
	}
	
	unsigned get_edge_num()
	{
		return edge_num; //num_edge;
	}
	
    unsigned external_to_internel_id(std::string exID) // new
    {
        std::tr1::unordered_map<std::string, unsigned>::iterator tmpIter = _key2id.find(exID);
		
		if (tmpIter == _key2id.end())
			_key2id[exID] = (num_vertex++);
		else
			return tmpIter->second;    
    }
	
    std::string internal_to_externel_id(unsigned inID) // new
    {
        std::tr1::unordered_map<unsigned, std::string>::iterator tmpIter = _id2key.find(inID);
        return tmpIter->second;    
    }

protected:
    std::tr1::unordered_map<std::string, unsigned> _key2id;  // key: external id;  id: internel id
    std::tr1::unordered_map<unsigned, std::string> _id2key;
	unsigned num_vertex = 0;
	unsigned edge_num = 0;
};


class timer
{
public:
    static double get_usec()
    {
        timeval tim;
        gettimeofday(&tim, NULL);
        return tim.tv_sec+(tim.tv_usec/1000000.0);
    } 
};


}

#endif
