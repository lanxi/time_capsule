#ifndef tree_hh_
#define tree_hh_

#include <cassert>
#include <memory>
#include <stdexcept>
#include <iterator>
#include <set>
#include <queue>
#include <algorithm>
#include <cstddef>


/// A node in the tree, combining links to other nodes as well as the actual data.
template<class T>
class tree_node_ { // size: 4*4=16 bytes (on 32 bit arch).
	public:
		tree_node_();
		tree_node_(const T&);

		tree_node_<T> *parent;
	   tree_node_<T> *first_child;
		tree_node_<T> *prev_sibling, *next_sibling;
		T data;
}; // __attribute__((packed));