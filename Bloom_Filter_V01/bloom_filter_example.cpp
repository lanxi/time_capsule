#include <iostream>
#include <string>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <unordered_map>

#include "bloom_filter.hpp"



int main()
{


   // How many elements roughly do we expect to insert?
   unsigned long long int projected_element_count = 1000;

   // Maximum tolerable false positive probability? (0,1)
   double false_positive_probability = 0.01; // 1 in 10000


   std::cout << "prepare to create bloom filter" << std::endl;
   //Instantiate Bloom Filter
   bloom_filter filter(projected_element_count,false_positive_probability);
   
   std::cout << "size: " << filter.size() << std::endl;
   std::cout << "number of hashes: " << filter.hash_count() << std::endl;

   // Insert into Bloom Filter
   {

      // Insert some numbers
      for (std::size_t i = 0; i < 100; ++i)
      {
         filter.insert(i);
      }
   }

   // Query Bloom Filter
   {
      // Query the existence of numbers
      for (std::size_t i = 0; i < 100; ++i)
      {
         if (filter.contains(i))
         {
            std::cout << "BF contains: " << i << std::endl;
         }
      }


      // Query the existence of invalid numbers
      for (int i = 101; i > 200; ++i)
      {
         if (filter.contains(i))
         {
            std::cout << "BF falsely contains: " << i << std::endl;
         }
      }
   }
   
   
      unsigned int number_of_hashes = 3;
      unsigned long long int table_size = 10;
      unsigned long long int number_insert_elements = 50;
      bloom_filter bf2(number_of_hashes,table_size);
      //std::cout << "Filter 2 start .." << std::endl;
   
   
      // insert stage
      //std::vector<int> visited;
      std::unordered_map<unsigned int,bool> visited;
      int r;
      bool flag = false;
      srand(time(NULL));
      for(std::size_t i = 0;i < number_insert_elements;++i)
      {
         
         do
         {
            r = rand();
            if(visited.find(r) != visited.end())
            {
               flag = true;
            }
         }while(flag);
         flag = false;
         visited.insert({r,true});
         bf2.insert(r);
      }
      std::cout << "insert finished " << std::endl;
      
      // test false positive probability
      unsigned int test_number = 10000;
      unsigned int false_positive_count = 0;
      
      for(std::size_t i = 0;i < test_number;++i)
      {
         r = rand();
         //std::cout << r << std::endl;
         if(visited.find(r) == visited.end() && bf2.contains(r))
         {
            ++ false_positive_count;
         }
      }
      std::cout << "theoretical fpp: " << bf2.effective_fpp() << std::endl;
      std::cout << "experimental fpp: " << false_positive_count * 1.0 / test_number << std::endl;
      
   

   return 0;
}
