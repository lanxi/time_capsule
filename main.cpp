#include <iostream>
#include <memory>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

// TODO: custom the hash function for unordered_map as hashtable

struct node {
  int senderId;
  uint64_t ts; /*activation time*/
  int giftType;
  
  node(){};

  node(int sender, uint64_t time, int gift)
  {
    senderId = sender;
    ts = time;
    giftType = gift;
  }
};

std::unordered_map<int /*id*/, std::vector<std::shared_ptr<node>>> nodes;

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
  
  for(int i = 0; i <= 13; ++i) {
    std::ifstream infile;
    if(i <= 6) {
      infile.open("traces2009-25-"+ std::to_string(i) +"-hid.txt");
    }else {
      infile.open("traces2009-26-"+ std::to_string(i%7) +"-hid.txt");
    }
    
    int senderId, recvId, giftType;
    uint64_t ts;
    while (infile >> senderId >> recvId >> ts >> giftType) {
      if(nodes.find(senderId) != nodes.end()) {
	// add receiver to sender
	nodes.find(senderId)->second.
	  emplace_back(std::make_shared<node>(recvId, ts, giftType));

	// create receiver as a prospective sender
	std::vector<std::shared_ptr<node>> recvNodes;
	nodes.insert({recvId, recvNodes});
      }else {
	// root node
	roots.emplace_back(senderId);
	std::vector<std::shared_ptr<node>> recvNodes;
	recvNodes.emplace_back(std::make_shared<node>(senderId, 0, 0));
	nodes.insert({senderId, recvNodes});
      }
    }
  }
  
  // get max depth, width and cascade size for all roots after storing completes
  std::vector<std::vector<int>> treeInfos;
  for(int rootId : roots) {
    std::vector<int> treeInfo;
    std::unordered_map<int, std::vector<std::shared_ptr<node>>>::const_iterator itr = nodes.find(rootId);
    treeInfo.emplace_back(getMaxDepth(itr->first));
    treeInfos.emplace_back(getMaxWidth(itr->first));
    treeInfos.emplace_back(getCascadeSize(itr->first));
  }

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
