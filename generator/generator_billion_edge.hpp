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
#include <sys/time.h>
#include <algorithm>
#include <thread>
#include <sstream>
#include <fstream>
#include <limits>
#include <algorithm>

using namespace std;

namespace edgeset
{

#define SIZE 0x0ffff

typedef unsigned int base_t;

class GRAPH
{
public:
    std::vector<base_t> vertex_list;
    //std::vector<std::vector<unsigned> > in_edge_list;
    std::vector<std::vector<base_t> > out_edge_list;
    
    bool load_graph(const std::string ofile, int num_edge, int num_vertex)
    {
        std::ifstream fin;
        
        fin.open(ofile.c_str());
        if(!fin.is_open())
        {
            std::cout<<"Failed to open the file"<<std::endl;
            return false;
        }
        
        vertex_list.reserve(num_vertex);
        //in_edge_list.reserve(num_edge);
        out_edge_list.reserve(num_vertex);
        out_edge_list.resize(num_vertex, std::vector<base_t> (0) );
        for(int i = 0; i < num_vertex; i++)
        {
            out_edge_list[i].reserve(200);
        }
        
        std::string src_id, dest_id;
        //std::cout<<"Start load data"<<endl;

        std::string line;
        std::vector<std::string> lines;
        lines.reserve(num_edge);

        while (getline(fin, line))
        {
            lines.push_back(line);
        }

        int64_t block_size = 1000;
        int64_t stride_size = 100;
        int64_t num_block = std::ceil( (lines.size() * 1.0)/ block_size );

        base_t num_machines = 9;
        base_t stride = num_vertex / num_machines;
        base_t total_divisible = stride * num_machines;
        //std::vector<int64_t> num_array(num_block, 0);
        //
        //for (int i = 0; i < num_block; i++)
        //{
        //    num_array[i] = i;
        //}
        //
        //std::random_shuffle(std::begin(num_array), std::end(num_array));        

        for (int64_t i = 0; i < block_size; i++)
        {
            //int idx_block = num_array[i];

            for (int64_t j = 0; j < num_block; j++)
            {
                //int64_t row_idx = j / stride_size;
                //int64_t col_idx = j % stride_size;
                //int64_t offset = row_idx * stride_size+  ( j * stride_size ) % block_size;

                int64_t idx = j * block_size + i;
    
                if ( idx >= (int64_t)lines.size() )
                    continue;
    
                line = lines[idx];
    
                std::istringstream line_s(line);
                line_s >> src_id;
                line_s >> dest_id;
                
                unsigned itn_src_id = external_to_internel_id(src_id, stride, total_divisible);
                unsigned itn_dest_id = external_to_internel_id(dest_id, stride, total_divisible);
    
                //std::cout<<src_id<<" "<<dest_id<<endl;
                
                //if (itn_dest_id < in_edge_list.size())
                //    in_edge_list[itn_dest_id].push_back(itn_src_id);
                //else
                //{
                //    while(itn_dest_id >= in_edge_list.size())
                //    {
                //    std::vector<unsigned> tmp;
                //    in_edge_list.push_back(tmp);
                //    }
                //
                //    assert(itn_dest_id < in_edge_list.size());
                //    in_edge_list[itn_dest_id].push_back(itn_src_id);
                //}
                
                if (itn_src_id < out_edge_list.size())
                    out_edge_list[itn_src_id].push_back(itn_dest_id);
                else
                {
                    while(itn_src_id >= out_edge_list.size())
                    {
                    std::vector<base_t> tmp;
                    out_edge_list.push_back(tmp);
                    }
                    
                    assert(itn_src_id < out_edge_list.size());
                    out_edge_list[itn_src_id].push_back(itn_dest_id);
                }
                
                _num_edge ++;
            }
        }
        
        for (unsigned i = 0; i < _num_vertex; i++)
            vertex_list.push_back(i);
        
        fin.close();

        std::cout<<"num of edges: "<<_num_edge;

        std::cout<<"num of vertexs: "<<_num_vertex<<std::endl;
        
        return true;
    }

    std::fstream & go_to_line(std::fstream & file, unsigned int num)
    {
        file.seekg(std::ios::beg);
        for(unsigned i = 0; i < num - 1; ++i)
        {
            file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        }
        return file;
    }

    unsigned get_vertex_num()
    {
        return _num_vertex;//in_edge_list.size();
    }
    
    unsigned get_edge_num()
    {
        return _num_edge; //num_edge;
    }
    
    base_t external_to_internel_id(std::string exID, base_t stride, base_t total_divisible) // new
    {
        std::tr1::unordered_map<std::string, base_t>::iterator tmpIter = _key2id.find(exID);
        
        if (tmpIter == _key2id.end()){
            base_t val;

            if (_num_vertex < total_divisible)
            {
                val = (_num_vertex * stride ) % total_divisible + std::floor((_num_vertex * stride * 1.0) / total_divisible);
            }
            else
            {
                val = _num_vertex;
            }

            _num_vertex++;
            _key2id[exID] = (val);
            return val;
        }
        else
            return tmpIter->second;    
    }
    
    std::string internal_to_externel_id(base_t inID) // new
    {
        std::tr1::unordered_map<base_t, std::string>::iterator tmpIter = _id2key.find(inID);
        return tmpIter->second;    
    }

protected:
    std::tr1::unordered_map<std::string, base_t> _key2id;  // key: external id;  id: internel id
    std::tr1::unordered_map<base_t, std::string> _id2key;
    unsigned _num_vertex = 0;
    unsigned _num_edge = 0;
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


long gen_graph(//edgeset::GRAPH & kroneck_graph
               std::vector<std::pair<int, int> > & k_graph
             , edgeset::base_t num_kroneck_vertices
             , edgeset::base_t num_kroneck_edges
             , string infile_graph
             , edgeset::base_t num_g_vertices
             , edgeset::base_t num_g_edges
             );


unsigned get_machine_id(unsigned vertex_num
                 , unsigned machine_num
                 , unsigned vertex_id
                 );

#endif
