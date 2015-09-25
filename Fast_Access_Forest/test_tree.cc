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
      
   }