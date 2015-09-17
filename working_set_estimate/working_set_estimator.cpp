#include <iostream>
#include <iomanip>
#include <memory>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

// TODO: custom the hash function for unordered_map as hashtable

struct node {
  int senderId;
  uint64_t ts; /*activation time*/
  int giftType;
  
  // added by long gong
  int rootId;
  
  node(){};

  node(int sender, uint64_t time, int gift)
  {
    senderId = sender;
    ts = time;
    giftType = gift;
    rootId = 0;
  }
    
  node(int sender, uint64_t time, int gift,int root)
  {
    senderId = sender;
    ts = time;
    giftType = gift;
    rootId = root;
  }
};


std::unordered_map<int /*id*/, std::vector<std::shared_ptr<node>>> nodes;


std::unordered_map<int /*id*/, unsigned long long int /*treeSize*/> trees_size;// size of each tree

static unsigned long long int  tree_number = 0;// trace the number of trees in the forest
static unsigned long long int  node_number = 0;// trace the total number of nodes in the forest

int getMaxDepth(int id);
int getMaxWidth(int id);
int getWidth(int id, int level);
int getCascadeSize(int id);
void deleteTree(int id);

int main()
{
  // read line from file and store into hashtable,
  // keep root info in a vector record
  std::vector<int /*id*/> roots;
  nodes.clear();
  roots.clear();
  trees_size.clear();
  
  std::ofstream tgtFile("tree_growth_trace.data"); // file to trace the growth of trees (timestamp   treeId   treeSize treeNumber forestSize)

  
  std::ifstream infile;
  
  std::cout << "start to process files ..." << std::endl;
  for(int i = 0; i <= 13; ++i) {
    
    if(i <= 6) {
      infile.open("iHeart/traces2009-25-"+ std::to_string(i) +"-hid.txt");
    }else {
      infile.open("iHeart/traces2009-26-"+ std::to_string(i%7) +"-hid.txt");
    }
    std::cout << "open file success ..." << std::endl;
    
    int senderId, recvId, giftType,rootId;
    uint64_t ts;
    while (infile >> senderId >> recvId >> ts >> giftType) {
      // std::cout << "processing edge: " << senderId << " - " << recvId << std::endl;
	  if (nodes.find(recvId) != nodes.end()) {// if the receiver received gift before, then ignore this edge (equvalent to the First Parent Heuristic in the paper)
		  if (nodes.find(senderId) == nodes.end())
		  {
			  trees_size[senderId] = 1;
			  // std::cout << "new rootId: " << senderId << std::endl;

			  node_number ++;
			  tree_number++;



			  tgtFile << std::setw(20) << ts
				  << std::setw(20) << senderId
				  << std::setw(20) << trees_size[senderId]
				  << std::setw(20) << tree_number
				  << std::setw(20) << node_number
				  << std::endl;
			  // root node
			  roots.emplace_back(senderId);

			  std::vector<std::shared_ptr<node>> rootNodes;
			  rootNodes.emplace_back(std::make_shared<node>(senderId, 0, giftType, senderId));
			  nodes.insert({senderId, rootNodes});
		  }
		  continue;
	  }
      if(nodes.find(senderId) != nodes.end()) {
        
		  //get rootId
		  rootId = nodes[senderId][0]->rootId;
		  // std::cout << rootId << std::endl;
		  trees_size[rootId] ++;

		  node_number++;
  
  
		  tgtFile << std::setw(20) << ts 
				  << std::setw(20) << rootId
				  << std::setw(20) << trees_size[rootId]
			      << std::setw(20) << tree_number
			      << std::setw(20) << node_number
				  << std::endl;

		  if (nodes[senderId][0]->senderId == 0)
		  {// replace the dummy node with the new receiver
			  nodes[senderId][0] = std::make_shared<node>(recvId, ts, giftType, rootId);
		  }
		  else
		  {
			  // add receiver to sender
			  nodes.find(senderId)->second.
				  emplace_back(std::make_shared<node>(recvId, ts, giftType, rootId));
		  }



		// create receiver as a prospective sender
		std::vector<std::shared_ptr<node>> recvNodes;
		recvNodes.emplace_back(std::make_shared<node>(0, 0, 0, rootId));
		nodes.insert({recvId, recvNodes});

      }else {
        
		trees_size[senderId] = 2;
		//std::cout << "new rootId: " << senderId << std::endl;

		tree_number++;
		node_number += 2;

		tgtFile << std::setw(20) << ts 
				<< std::setw(20) << senderId
			    << std::setw(20) << 2
				<< std::setw(20) << tree_number
				<< std::setw(20) << node_number
				<< std::endl;
		// root node
		roots.emplace_back(senderId);
  
	    std::vector<std::shared_ptr<node>> rootNodes;
	    rootNodes.emplace_back(std::make_shared<node>(senderId, 0, giftType, senderId));
		rootNodes.emplace_back(std::make_shared<node>(recvId, ts, giftType, senderId));
	    nodes.insert({senderId, rootNodes});

		std::vector<std::shared_ptr<node>> recvNodes;
		recvNodes.emplace_back(std::make_shared<node>(0, 0, 0, senderId));
		nodes.insert({ recvId, recvNodes });
      }
    }
  }
  
  // get max depth, width and cascade size for all roots after storing completes
  /*
  std::vector<std::vector<int>> treeInfos;
  for(int rootId : roots) {
    std::vector<int> treeInfo;
    std::unordered_map<int, std::vector<std::shared_ptr<node>>>::const_iterator itr = nodes.find(rootId);
    treeInfo.emplace_back(getMaxDepth(itr->first));
    treeInfos.emplace_back(getMaxWidth(itr->first));
    treeInfos.emplace_back(getCascadeSize(itr->first));
  }
  */
  
  tgtFile.close();
  //infile.close();

  return 0;
}

int getMaxDepth(int id) {
  if(nodes.find(id) == nodes.end()) {
    return 0;
  }
  
  if (nodes.find(id)->second.size() == 0) {
    return 1;
  }else {
    /* compute the depth of each sub"tree" */
    int maxDepth = 0;
    for(const auto& recipient : nodes.find(id)->second) {
      int nDepth = getMaxDepth(recipient->senderId);
      maxDepth = std::max(nDepth, maxDepth);
    }
    return maxDepth + 1;
  }
}

int getMaxWidth(int id) {
  int maxWidth = 0;
  int width;
  int h = getMaxDepth(id);

  /* Get width of each level and compare                                                                                                                                            
     the width with maximum width so far */
  for(int i = 1; i <= h; ++i) {
      width = getWidth(id, i);
      if(width > maxWidth)
	maxWidth = width;
    }

  return maxWidth;
}

/* Get width of a given level */
int getWidth(int id, int level) {
  if(level == 1) {
    return 1;
  }else if (level > 1) {
    int width = 0;
    for(const auto& recipient: nodes.find(id)->second) {
      width += getWidth(recipient->senderId, level - 1);
    }
    return width;
  }
  return 0;
}

int getCascadeSize(int id) {
  if(nodes.find(id) == nodes.end()) {
    return 0;
  }
  
  if(nodes.find(id)->second.size() == 0) {
    return 1;
  }else {
    int cascadeSize = 0;
    for(const auto& recipient : nodes.find(id)->second) {
      cascadeSize += getCascadeSize(recipient->senderId);
    }
    return cascadeSize;
  }
}

void deleteTree(int id) {
  if(nodes.find(id) == nodes.end()) {
    return;
  }
  
  if(nodes.find(id)->second.size() == 0) {
    nodes.erase(id);
  }else {
    for(const auto& recipient : nodes.find(id)->second) {
      deleteTree(recipient->senderId);
    }
    nodes.erase(id);
  }
  return;
}
