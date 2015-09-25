#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <memory>
#include <stdlib.h>

#include "tree.hh"
#include "simple_node.h"



static const unsigned int columnWidth = 16;
static const unsigned int SAMPLE_RATE = 1;
static const std::string inputDataPath = "/Users/gonglong/Desktop/data/";
static const std::string outputFilename = "tree_growth_trace.dat";
static unsigned int total_tree_number = 0;
static unsigned int total_node_number = 0;

static std::unordered_map<int/*key*/,tree<simple_node>::iterator> hashTable;
static tree<simple_node>  forest;

static std::unordered_map<int/*key*/,unsigned int /*tree size*/> tree_size;


	
enum edgeStatus {S_NEW, R_NEW, B_NEW, B_OLD};

inline std::string getFilename(int,int);
inline edgeStatus getEdgeStatus(int senderId,int recvId);
inline void record_forest_growth_event(std::ofstream& fp,uint64_t ts,int treeId,unsigned int treeSize,unsigned int forestSize);
inline void print_tree(const tree<simple_node>& tr, tree<simple_node>::pre_order_iterator it, tree<simple_node>::pre_order_iterator end);

int main(int argc,char *argv[])
{

    if (argc != 3)
    {
        std::cerr << "we only accept 3 arguments ... please input again ..." << std::endl;
        exit(1);
    }
    int perc = atoi(argv[1]); // percentage 
    std::string del_time_filename(argv[2]); // filename 
    
	tree<simple_node>::iterator superNode, root, recv, top = forest.begin();
    // add a super node 
	superNode = forest.insert(top,*(new simple_node(0)));
	
	//std::ofstream outputFile(outputFilename);
    
    
    std::unordered_map<uint64_t /*del_time*/,std::vector<int> /*trees to be removed*/> delete_trees;
    
    std::ifstream del_time_file(del_time_filename);
    
    int del_tree_id;
    uint64_t del_tree_time;
    
    
 
        
    while (del_time_file >> del_tree_id >> del_tree_time)
    {
        if (delete_trees.find(del_tree_time) == delete_trees.end())
        {
            std::vector<int> del_treeIds;
            del_treeIds.emplace_back(del_tree_id);
            delete_trees.insert({del_tree_time,del_treeIds});
        }
        else
        {
            delete_trees[del_tree_time].emplace_back(del_tree_id);
        }
    }
    del_time_file.close();
        
    
	
	    
    unsigned remaining_node_number = 0;
    std::unordered_map<int/*treeId*/,bool/*removed or not*/> isRemoved;
    std::ofstream final_working_set("final_working_set.dat-" + std::to_string(perc));
    
    for(std::size_t i = 25;i < 33; ++i)
    {
        for(std::size_t j = 0;j < 7; ++j)
        {
        	std::string dataFilename = getFilename(i,j);
        	
        	std::cout << "Processing file " << dataFilename << std::endl;
        	
        	std::ifstream inputDataFile(dataFilename);
        	
        	int senderId, recvId, giftType;
        	uint64_t ts, ts_pre = 0;
        	
            unsigned int counter = 0;
        	while(inputDataFile >> senderId >> recvId >> ts >> giftType)
        	{
                if (ts_pre != 0 && ts_pre != ts)
                {// check whether one or more trees need to be removed
                    if (delete_trees.find(ts_pre) != delete_trees.end())
                    {
                        for(int treeId: delete_trees[ts_pre])
                        {
                            assert(tree_size.find(treeId) != tree_size.end());
                            
                            remaining_node_number -= tree_size[treeId];
                            
                            //std::cout << "At time "<< ts_pre <<"delete tree " << treeId << " with nodes of " << tree_size[treeId] << std::endl;
                            
                            isRemoved.insert({treeId,true});
                            
                        }
                    }
                }
                ts_pre = ts;
        		switch(getEdgeStatus(senderId,recvId))
        		{
        			case S_NEW:
        			{
        			   simple_node ssn(senderId,senderId);
        			   root = forest.append_child(superNode,ssn);
        			   hashTable.insert({senderId,root});
                       
                       // start to remove trees
                       
        			   
        			   tree_size.insert({senderId,1});

        			   
        			   total_tree_number ++;
        			   total_node_number ++;
                       
                       if (isRemoved.find(senderId) == isRemoved.end())
                       {// not removed
                         remaining_node_number ++;
                         counter ++;
                         if (counter % SAMPLE_RATE == 0)
                         {
                             final_working_set << std::setw(columnWidth) << ts
                               << std::setw(columnWidth) << remaining_node_number
                               << std::endl;
                           
                         }

                       }
        			   
        			   // record_forest_growth_event(outputFile,ts,senderId,1,total_node_number);
        			   break;
        			}
        			case R_NEW:
						{
						tree<simple_node>::iterator parent = hashTable[senderId];
        				simple_node rsn(recvId,parent->getRootId());
        				
        				recv = forest.append_child(parent,rsn);
        				hashTable.insert({recvId,recv});
        				
        				tree_size[parent->getRootId()] ++;
        				total_node_number ++;
                        
                        if (isRemoved.find(parent->getRootId()) == isRemoved.end())
                        {// not removed
                            remaining_node_number ++;
                            counter ++;
                            if (counter % SAMPLE_RATE == 0)
                            {
                            final_working_set << std::setw(columnWidth) << ts
                                              << std::setw(columnWidth) << remaining_node_number
                                             << std::endl;
                                         }
                        }
        				
        				//record_forest_growth_event(outputFile,ts,parent->getRootId(),tree_size[parent->getRootId()],total_node_number);
        				break;
        			    }
        			case B_NEW:
        			    {
        				simple_node sns(senderId,senderId);
        				simple_node snr(recvId,senderId);
        				
        				root = forest.append_child(superNode,sns);
        				recv = forest.append_child(root,snr);
        				
        				hashTable.insert({senderId,root});
        				hashTable.insert({recvId,recv});
        				
        				tree_size.insert({senderId,2});
        				total_node_number += 2;
        				total_tree_number ++;
        				
                        if (isRemoved.find(senderId) == isRemoved.end())
                        {// not removed 
                            remaining_node_number += 2;
                            counter ++;
                            if (counter % SAMPLE_RATE == 0)
                            {
                            final_working_set << std::setw(columnWidth) << ts
                                             << std::setw(columnWidth) << remaining_node_number
                                             << std::endl;
                            }
                        }
        				//record_forest_growth_event(outputFile,ts,senderId,2,total_node_number);
        				break;
        			   }
        			case B_OLD:
        				{
        				break;
        				}
        				
        					
        		}
        	}
        	inputDataFile.close();
        }
    }
	
    unsigned int total_node_number_verify = 0;
    for(std::unordered_map<int,unsigned>::iterator it = tree_size.begin();it != tree_size.end(); ++it)
    {
        total_node_number_verify += it->second;
    }
    
    assert(total_node_number == total_node_number_verify);
    
    final_working_set << std::setw(columnWidth) << 0
                      << std::setw(columnWidth) << total_node_number
                      << std::endl;
    final_working_set.close();
	//outputFile.close();
	
	/* if you want see what the forest looks like, you can input a small-scale case
	and uncomment the following two sentences, please remain them commented for 
	large-scale cases.
	*/
	// std::cout << "Processing finished ... Let\'s what the forest looks like ..." << std::endl;
	// print_tree(forest,forest.begin(),forest.end());
	
	
	return 0;
}

inline std::string getFilename(int i,int j)
{
	return(inputDataPath + "traces2009-"+ std::to_string(i) + "-"+ std::to_string(j) +"-hid.txt");
}
inline edgeStatus getEdgeStatus(int senderId,int recvId)
{
	bool sStatus = hashTable.find(senderId) == hashTable.end();
	bool rStatus = hashTable.find(recvId) == hashTable.end();
	
	if (sStatus && rStatus)
	{
		return B_NEW;
	}
	if (sStatus && (!rStatus))
	{
		return S_NEW;
	}
	if ((!sStatus) && rStatus)
	{
		return R_NEW;
	}
	return B_OLD;
}
inline void record_forest_growth_event(std::ofstream& fp,uint64_t ts,int treeId,unsigned int treeSize,unsigned int forestSize)
{
	fp << std::setw(columnWidth) << ts
	   << std::setw(columnWidth) << treeId
	   << std::setw(columnWidth) << treeSize
	   << std::setw(columnWidth) << forestSize
	   << std::endl;
}
inline void print_tree(const tree<simple_node>& tr, tree<simple_node>::pre_order_iterator it, tree<simple_node>::pre_order_iterator end)
	{
	if(!tr.is_valid(it)) return;
	int rootdepth=tr.depth(it);
	std::cout << "-----" << std::endl;
	while(it != end) {
		for(int i = 0; i < tr.depth(it)-rootdepth; ++i) 
			std::cout << "  ";
		std::cout << (*it) << std::endl << std::flush;
		++it;
		}
	std::cout << "-----" << std::endl;
	}