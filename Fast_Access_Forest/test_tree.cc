#include <algorithm>
#include <string>
#include <iostream>


#include "tree.hh"
#include "simple_node.h"

using namespace std;

int main(int, char **)
   {
   tree<simple_node> tr;
   tree<simple_node>::iterator top, ptemp, ctemp, dtemp;

	top = tr.begin();
   simple_node sn(1);
   ptemp = tr.insert(top, sn);
   
   simple_node sn2(2,1);
   
   ctemp = tr.append_child(ptemp, sn2);
   
	dtemp = tr.append_child(ptemp, *(new simple_node(3,1)));
   
   
   for (int i = 4;i <= 10;++i)
   {
      tr.append_child(ctemp,*(new simple_node(i,1)));
   }

   tr.append_child(dtemp,*(new simple_node(11,1)));


   for(tree<simple_node>::iterator it = tr.begin();it != tr.end(); ++it)
   {
      cout << *(it) << endl;
      cout << it->getRootId() << endl;
   }
   /*
	tree<simple_node>::iterator loc = find(tr.begin(), tr.end(), *(new simple_node(5)));
   
	if(loc != tr.end()) {
      
      tree<simple_node>::sibling_iterator sib = tr.begin(loc);
      
	   while(sib != tr.end(loc)) {
         
         cout << (*sib) << endl;
         
         ++sib;
         
         }
         
		cout << endl;
      
      tree<simple_node>::iterator sib2 = tr.begin(loc);
      tree<simple_node>::iterator end2 = tr.end(loc);
      
	   while(sib2 != end2) {
         
         for(int i=0; i<tr.depth(sib2)-2; ++i) 
            
				cout << " ";
         
         cout << (*sib2) << endl;
         
         ++sib2;
         
         }
	   }*/
      
   }