#include <iostream>
#include <unordered_map>
#include <string>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <tuple>
#include <cassert>
#include <memory> // shared_ptr




static const int columnWidth = 16;

struct time_tree_size_pair{
    uint64_t ts;
    unsigned int tree_size;
    
    time_tree_size_pair(uint64_t ts_,unsigned int tree_size_)
    {
        ts = ts_;
        tree_size = tree_size_;
    }
};



int main()
{
    
    std::string tree_growth_file = "tree_growth_trace.dat"; // input file name [time_stamp tree_id tree_size forest_size_in_tree_number forest_size_in_node_number]
    std::string working_set_file = "working_set.dat"; // output file name [time_stamp bits_reuqired_at_the_time_stamp]
    std:: string tree_size_dist_file = "tree_size_dist.dat"; // output file name [tree size, tree number]
    
    
    
    std::unordered_map<int /*treeId*/,std::vector<std::shared_ptr<time_tree_size_pair> > > tree_size_trace;// trace the tree size growth [tree_id --> (time_stamp,tree_size)]
    std::unordered_map<uint64_t /*ts*/,unsigned int /*forestSize*/> forest_size_trace;// trace the forest size growth in terms of node number [time_stamp --> forest_size]
    
    
    std::ifstream inputFile(tree_growth_file);// input file stream
    
    /*
     * since, one tree might increase multiple nodes at the same time stamp, we need to aggregate all these growth points
     */
    uint64_t ts_cur,ts_pre;// current time stamp & last time stamp [current growth point & last growth point]
    int treeId_cur,treeId_pre;// current growing tree ID & last growing tree ID
    
    /*
     * current tree size & last tree size
     */
    unsigned int tree_size_cur,tree_size_pre;
    
    // current forest size & last forest size
    unsigned int forest_size_cur,forest_size_pre;
    
    
    
    std::cout << "start to read the input file ..." << std::endl;
    
    // read the first five tuple from the input file
    inputFile >> ts_pre >> treeId_pre >> tree_size_pre >> forest_size_pre;
    
    
    // record the tree number & forest size (NT: we do not need to check overlapping, since the later entry will automatically overwrite the previous ones)
    forest_size_trace[ts_pre] = forest_size_pre;
    
    
    // read the rest of the input file
    while(inputFile >> ts_cur >> treeId_cur >> tree_size_cur >> forest_size_cur){
        
        //std::cout << "get a point ..." << std::endl;
        forest_size_trace[ts_cur] = forest_size_cur;
        
        
        if (!(ts_cur == ts_pre && treeId_cur == treeId_pre))
        {// if current time_stamp & tree_ID are not the same as last one, we should record last one
            if (tree_size_trace.find(treeId_pre) != tree_size_trace.end())
            {// tree already exists
                if (tree_size_trace[treeId_pre].back()->ts != ts_pre)
                {
                    tree_size_trace[treeId_pre].emplace_back(std::make_shared<time_tree_size_pair>(ts_pre,tree_size_pre));
                }
                else
                {// multiple changes at the same time
                    tree_size_trace[treeId_pre].back()->tree_size = tree_size_pre;
                    
                }
                
            }
            else
            {
                std::vector<std::shared_ptr<time_tree_size_pair> > time_tree_size_vec;
                time_tree_size_vec.emplace_back(std::make_shared<time_tree_size_pair>(ts_pre,tree_size_pre));
                tree_size_trace.insert({treeId_pre,time_tree_size_vec});
            }
            
        }
        
        if(inputFile.eof()){// current row is the last one row, we need also record it
            if (tree_size_trace.find(treeId_cur) != tree_size_trace.end())
            {// tree already exists
                if (tree_size_trace[treeId_cur].back()->ts != ts_cur)
                {
                    tree_size_trace[treeId_cur].emplace_back(std::make_shared<time_tree_size_pair>(ts_cur,tree_size_cur));
                }
                else
                {
                    tree_size_trace[treeId_cur].back()->tree_size = tree_size_cur;
                }
                
            }
            else
            {
                std::vector<std::shared_ptr<time_tree_size_pair> > time_tree_size_vec;
                time_tree_size_vec.emplace_back(std::make_shared<time_tree_size_pair>(ts_cur,tree_size_cur));
                tree_size_trace.insert({treeId_cur,time_tree_size_vec});
            }
        }
        
        
        treeId_pre = treeId_cur;
        ts_pre = ts_cur;
        tree_size_pre = tree_size_cur;
    }
    inputFile.close();
    
    std::cout << "Reading finished, start to process ..." << std::endl;
    
    // tree size distribution
    std::cout << "Calculate tree size distribution ..." << std::endl;
    std::unordered_map<unsigned int,unsigned int> tree_size_dist; // tree size ---> tree number
    
    for(std::unordered_map<int /*treeId*/,std::vector<std::shared_ptr<time_tree_size_pair> > >::iterator m_it = tree_size_trace.begin();m_it != tree_size_trace.end();++m_it)
    {
        
        if (tree_size_dist.find(m_it->second.back()->tree_size) == tree_size_dist.end())
        {
            tree_size_dist.insert({m_it->second.back()->tree_size,1});
        }
        else
        {
            tree_size_dist[m_it->second.back()->tree_size] ++;
        }
    }
    std::ofstream tsFile(tree_size_dist_file);
    for(std::unordered_map<unsigned int,unsigned int>::iterator it = tree_size_dist.begin();it != tree_size_dist.end();++it)
    {
        tsFile << std::setw(columnWidth) << it->first
                << std::setw(columnWidth) << it->second
                << std::endl;
    }
    tsFile.close();
    tree_size_dist.clear();
    
    std::cout << "tree size distribution finished ..." << std::endl;
    
    
    for(double percentage = 0.50;percentage <0.96;percentage += 0.05)
    {
        std::cout << "start to calculate working set for " << percentage << std::endl;
        
        std::ofstream tbFile(working_set_file + "-" + std::to_string(int(percentage * 100)));// output file stream
        
        
        
        uint64_t del_time;
        unsigned int del_tree_size = 0;
        
        int counter = 0;
        for(std::unordered_map<int /*treeId*/,std::vector<std::shared_ptr<time_tree_size_pair> > >::iterator m_it = tree_size_trace.begin();m_it != tree_size_trace.end();++m_it)
        {
            del_tree_size = m_it->second.back()->tree_size;
            counter = 0;
            for(std::vector<std::shared_ptr<time_tree_size_pair> >::iterator v_it = m_it->second.begin();v_it != m_it->second.end();++v_it)
            {
                counter ++;
                if((*v_it)->tree_size >= percentage * del_tree_size)
                {
                    if (del_tree_size == 302788)
                    {
                        std::cout << (*v_it)->ts << "," << m_it->first << "," << del_tree_size<< std::endl;
                    }
                    
                    del_time = (*v_it)->ts;
                    
                    tbFile << std::setw(columnWidth) << m_it->first
                            << std::setw(columnWidth) << del_time
                            << std::endl;
                    break;
                }
            }
            
        }
        
        
        tbFile.close();
    }
    
    // free the memory of the hash map
    tree_size_trace.clear();
    
    
    
    
    
    
    return 0;
    
    
}