#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <memory>

#include "tree.hh"
#include "simple_node.h"



static const unsigned int columnWidth = 16;
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

int main()
{

	tree<simple_node>::iterator superNode, root, recv, top = forest.begin();
    // add a super node 
	superNode = forest.insert(top,*(new simple_node(0)));
	
	std::ofstream outputFile(outputFilename);
	
	    
    
    for(std::size_t i = 25;i < 33; ++i)
    {
        for(std::size_t j = 0;j < 7; ++j)
        {
        	std::string dataFilename = getFilename(i,j);
        	
        	std::cout << "Processing file " << dataFilename << std::endl;
        	
        	std::ifstream inputDataFile(dataFilename);
        	
        	int senderId, recvId, giftType;
        	uint64_t ts;
        	
        	while(inputDataFile >> senderId >> recvId >> ts >> giftType)
        	{
        		switch(getEdgeStatus(senderId,recvId))
        		{
        			case S_NEW:
        			{
        			   simple_node ssn(senderId,senderId);
        			   root = forest.append_child(superNode,ssn);
        			   hashTable.insert({senderId,root});
        			   
        			   tree_size.insert({senderId,1});

        			   
        			   total_tree_number ++;
        			   total_node_number ++;
        			   
        			   record_forest_growth_event(outputFile,ts,senderId,1,total_node_number);
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
        				
        				record_forest_growth_event(outputFile,ts,parent->getRootId(),tree_size[parent->getRootId()],total_node_number);
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
        				
        				record_forest_growth_event(outputFile,ts,senderId,2,total_node_number);
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
	
	outputFile.close();
	
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