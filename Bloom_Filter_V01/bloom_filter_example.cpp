/*
Bloom Filter Test
This file is to check whether the bloom filter is good or not, i.e., to check whether the independence of the hash functions used in the bloom filter.
*/
#include <iostream>
#include <string>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <unordered_map>
#include <fstream>
#include <iomanip>

#include "bloom_filter.hpp"



int main()
{

	/* *********************************************************************************************
	 *                                 Correctness verify                                          *
	 *                    verify that there is no false negetive                                   *
	 *                                                                                             *
	 ********************************************************************************************* */
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
         if (!filter.contains(i))
         {
            std::cerr << "BF errors for: " << i << std::endl;
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
   
   

   /* *****************************************************************************************
    *                    Check the independence of hash functions                             *
	*                                                                                         *
	*******************************************************************************************/
   
   
      unsigned int number_of_hashes = 0;
      unsigned int table_size = 1024;
      unsigned long long int number_insert_elements = 0;

	  //
	  std::cout << "Start test the indenpence of the hash functions ...." << std::endl;
	  srand(time(NULL));

	  std::cout.setf(std::ios::right | std::ios::fixed);
	  std::cout << std::setw(20) << "number of hashes" 
		        << std::setw(20) << "element number" 
		        << std::setw(20) << "theoretical fpp" 
		        << std::setw(20) << "experimental fpp" 
		        << std::setw(20) << "relative error" 
	            << std::endl;
	  for (number_of_hashes = 2; number_of_hashes <= 10; ++number_of_hashes)
	  {
		  number_insert_elements = rand() % 200 + 150;
		  
		  

		  //std::cout << "bloom filter (" << table_size << "," << number_of_hashes << ")" << std::endl;

		  for (size_t test_id = 0; test_id < 10; ++test_id)
		  {
			  bloom_filter bf2(number_of_hashes, table_size);
			  
			  // insert stage
			  std::unordered_map<unsigned int, bool> visited;
			  int r;
			  bool flag = false;

			  for (std::size_t i = 0; i < number_insert_elements; ++i)
			  {

				  do
				  {
					  flag = false;
					  r = rand();
					  if (visited.find(r) != visited.end())
					  {
						  flag = true;
					  }
				  } while (flag);
				  flag = false;
				  visited.insert({ r,true });
				  bf2.insert(r);
			  }

			  // query stage
			  unsigned int test_number = (unsigned int) (10000.0 / bf2.effective_fpp());
			  unsigned int false_positive_count = 0;
			  for (std::size_t i = 0; i < test_number; ++i)
			  {
				  r = rand();
				  //std::cout << r << std::endl;
				  if (visited.find(r) == visited.end() && bf2.contains(r))
				  {
					  ++false_positive_count;
				  }
			  }
			  double experiment_fpp = false_positive_count * 1.0 / test_number;
			  
			  
			  std::cout << std::setw(20) <<number_of_hashes 
				        << std::setw(20) << number_insert_elements 
				        << std::setw(20) << bf2.effective_fpp() 
				        << std::setw(20) << experiment_fpp  
				        << std::setw(20) << abs(bf2.effective_fpp() - experiment_fpp) / bf2.effective_fpp() 
				        << std::endl;
			  

		  }
	  }

   
   
      


   
   return 0;
}
