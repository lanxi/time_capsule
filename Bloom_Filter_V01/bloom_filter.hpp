/*
 *********************************************************************
 *                                                                   *
 *                           Bloom Filter                            *
 *                                                                   *
 * Based on Open Bloom Filter (http://www.partow.net)                *
 *                                                                   *
 *********************************************************************
*/


#ifndef BLOOM_FILTER
#define BLOOM_FILTER

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <limits>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
 
#include "hash.hpp"

typedef unsigned char bitmap;
typedef unsigned long long int large_integer;
typedef unsigned int integer;


   
static const integer minimum_number_of_hashes = 1;// minimum number of hash functions (for the bloom filter)
static const integer maximum_number_of_hashes = 10;//maximum number of hash functions (for the bloom filter)
static const large_integer minimum_size = 1;// Allowed minimum size of the bloom filter in bits
static const large_integer maximum_size = 4294967296U;// Allowed maximum size of the bloom filter in bits (4Gb)
static const integer max_filename_length = 1024; // maximum length of an accepted file name 
static const std::size_t bits_per_char = 0x08;    // 8 bits in 1 char(unsigned)
static const bitmap bit_mask[bits_per_char] = {
                                                       0x01,  //00000001
                                                       0x02,  //00000010
                                                       0x04,  //00000100
                                                       0x08,  //00001000
                                                       0x10,  //00010000
                                                       0x20,  //00100000
                                                       0x40,  //01000000
                                                       0x80   //10000000
                                                     };


class bloom_filter
{
public:
   std::vector<HashFunc*>         hash_;// pointer to hash functions
   integer           hash_range_;// range of hash function, e.g., if h(x) = y is a hash function, then it is the range of y
   bitmap*           bit_table_;// the bit array of bloom filter
   integer           hash_count_;// number of hash functions
   large_integer     table_size_;// size of the bit array (in bits)
   large_integer     raw_table_size_;// size of the bit array (in bytes)
   integer           inserted_element_count_;// number of inserted elements
   

   /* create an empty bloom filter */
   bloom_filter()
   {
    //hash_ = NULL;
    bit_table_ = NULL;
    table_size_ = 0;
    hash_count_ = 0;
    hash_range_ = 0;
    raw_table_size_ = 0;
    inserted_element_count_ = 0;
   }

   /*
   create a bloom filter according to the estimated number of inserted elements and desired false
   positive probability. It will calculate the optimal value of the table size.
   */
   bloom_filter(const large_integer projected_element_count,double false_positive_probability)
   {
    
    inserted_element_count_ = 0;
    
    hash_count_ = 0;
    table_size_ = 0;
    raw_table_size_ = 0;
    
    
    compute_optimal_parameters(projected_element_count,false_positive_probability);
    hash_range_ = (integer)(ceil(log(table_size_)));
    raw_table_size_ = table_size_ / bits_per_char;
    bit_table_ = new bitmap[static_cast<std::size_t>(raw_table_size_)];
    std::fill_n(bit_table_,raw_table_size_,0x00);
    
    //hash_ = new HashFunc[hash_count_];
    generate_independent_hashes();
    
   }
   /*
   create bloom filter with bit array size of table_size and number_of_hashes hash functions.
   Note that, here table_size is in log2 format, which means the true table size is 2^table_size
   */  
   bloom_filter(const integer number_of_hashes,const large_integer table_size)
   {
    hash_range_ = table_size;
    hash_count_ = number_of_hashes;
    table_size_ = 1 << table_size;
    
    inserted_element_count_ = 0;
    
    raw_table_size_ = table_size / bits_per_char;
    bit_table_ = new bitmap[static_cast<std::size_t>(raw_table_size_)];
    std::fill_n(bit_table_,raw_table_size_,0x00);
    
    //hash_ = new HashFunc[hash_count_];
    generate_independent_hashes();
   }

   
   inline virtual void compute_optimal_parameters(const large_integer projected_element_count,const double false_positive_probability)
   {
      /*
        Note:
        The following will attempt to find the number of hash functions
        and minimum amount of storage bits required to construct a bloom
        filter consistent with the user defined false positive probability
        and estimated element insertion count.
      */

      double min_m = std::numeric_limits<double>::infinity();
      double min_k = 0.0;
      double curr_m = 0.0;
      double k = 1.0;

      while (k < 1000.0)
      {
         double numerator   = (- k * projected_element_count);
         double denominator = std::log(1.0 - std::pow(false_positive_probability, 1.0 / k));
         curr_m = numerator / denominator;
         if (curr_m < min_m)
         {
            min_m = curr_m;
            min_k = k;
         }
         k += 1.0;
      }


      hash_count_ = static_cast<integer>(min_k);
      table_size_ = static_cast<large_integer>(min_m);
      table_size_ += (((table_size_ % bits_per_char) != 0) ? (bits_per_char - (table_size_ % bits_per_char)) : 0);

      if (hash_count_ < minimum_number_of_hashes)
         hash_count_ = minimum_number_of_hashes;
      else if (hash_count_ > maximum_number_of_hashes)
         hash_count_ = maximum_number_of_hashes;

      if (table_size_ < minimum_size)
         table_size_ = minimum_size;
      else if (table_size_ > maximum_size)
         table_size_ = maximum_size;
   }
   
   virtual ~bloom_filter()
   {
      delete[] bit_table_;
      hash_.clear();
   }


   inline void clear()
   {
      std::fill_n(bit_table_,raw_table_size_,0x00);
      inserted_element_count_ = 0;
   }

   inline void insert(const integer key) 
   {
      std::size_t bit_index = 0;
      std::size_t bit = 0;
      for(std::size_t i = 0;i < hash_count_; ++i)
      {
        compute_indices((hash_[i])->GetHashValue(key),bit_index,bit);
        bit_table_[bit_index / bits_per_char] |= bit_mask[bit];
      }
      ++inserted_element_count_;  
   }
   
   inline virtual bool contains(const integer key) const
   {
      std::size_t bit_index = 0;
      std::size_t bit = 0;
      for (std::size_t i = 0; i < hash_count_; ++i)
      {
         compute_indices((hash_[i])->GetHashValue(key),bit_index,bit);
         if ((bit_table_[bit_index / bits_per_char] & bit_mask[bit]) != bit_mask[bit])
         {
            return false;
         }
      }
      return true;
     
   }

   inline virtual large_integer size() const
   {
      return table_size_;
   }

   inline std::size_t element_count() const
   {
      return inserted_element_count_;
   }

   inline double effective_fpp() const
   {
      /*
        Note:
        The effective false positive probability is calculated using the
        designated table size and hash function count in conjunction with
        the current number of inserted elements - not the user defined
        predicated/expected number of inserted elements.
      */
      return std::pow(1.0 - std::exp(-1.0 * hash_count_ * inserted_element_count_ / size()), 1.0 * hash_count_);
   }


   inline const bitmap* table() const
   {
      return bit_table_;
   }

   inline std::size_t hash_count()
   {
      return hash_count_;
   }


   inline virtual void compute_indices(const integer& hash, std::size_t& bit_index, std::size_t& bit) const
   {
      bit_index = hash % table_size_;
      bit = bit_index % bits_per_char;
   }

/* This function is used to generate independent hash functions */ 
inline void generate_independent_hashes( )
{
  const integer predef_hash_count = 10;
  
  integer predef_hash[predef_hash_count][60];
  
  
  
  for(integer i = 0;i < predef_hash_count;++i)
  {
    char filename[max_filename_length];
    sprintf(filename,"init_seed_arrays/hash_data.%d",i);
    
    std::ifstream ifs(filename);
    
    
    
    for(integer k = 0;k < 60;++k)
    {
       if(!(ifs >> predef_hash[i][k]))
       {
        std::cerr << "Error reading hashing seed file\n" << std::endl;
        exit(1);
       }
    }
    
    
  }
  
  if (hash_count_ <= predef_hash_count)
  {
    for (integer i = 0;i < hash_count_; ++i)
    {
      HashFunc hf(hash_range_,predef_hash[i]);
      hash_.push_back(&hf);
    }
  }
  else
  {
    //TODO: to generate more independent hash functions
  }
  
  
}


};



#endif


