#ifndef _HASH_H
#define _HASH_H

#include <stdio.h>
#include <stdlib.h>

static const unsigned int max_hash_bits = 31;
/* the largest possible hash bits */

class HashFunc{
/* this class contains a hash function that hashes
  an user id (i.e, an unsigned integer which is smaller than 2^32 - 1) into
  a hash value within an arbitrary hashing range no 
  larger than 2^20
*/
private:

  int mHashRange, mVirtualRange;
  /* mHashRange is the true range while mBinaryRange is 2^mHashBits */

  int mHashBits;
  /* number of bits in the hashed output */
  
  unsigned int mBitMasks[max_hash_bits];
  /* 32-bit random numbers used to XOR with the unsigned integer */
  
  /* hash a 32-bit key using a certain array */
  int HashIt(unsigned int hash_key, unsigned int *mask_array, int bits)
  {
	  int i, hash_value = 0;  
	  for (i=0; i < bits; i++) {
		hash_value = hash_value << 1;
		hash_value += GetHashBit(hash_key, mask_array[i]);
	  }
	  return hash_value;	  
  }
  
  /* XOR two 32-bit numbers and get a bit */
  int GetHashBit(unsigned int hash_key, unsigned int mask)
  {
	  int i, ones = 0;
	  hash_key = hash_key & mask;
	  for (i=0; i<32; i++) {
		if (hash_key % 2 == 1) ones++;
		/* count the number of ones */

		hash_key = hash_key >> 1;
	  }
	  return ones % 2;
  }
  
  
public:

  HashFunc()
  {
  	mHashBits = 20;
  	mHashRange = 1 << mHashBits;
  	mVirtualRange = 1 << mHashBits;
  	InitSeed();
  }

  
  HashFunc(int hash_range)
  {
	  mHashRange = hash_range;

	  mHashBits = 1;
	  while((1 << mHashBits) < mHashRange) 
	    mHashBits++;
	  mVirtualRange = (1 << mHashBits);
	  InitSeed();
  }
  
  HashFunc(int hash_range,unsigned int seed_array[])
  {
	  mHashRange = hash_range;

	  mHashBits = 1;
	  while((1 << mHashBits) < mHashRange) 
	    mHashBits++;
	  mVirtualRange = (1 << mHashBits);
	  InitSeed(seed_array);
  	
  }
  
  HashFunc(int hash_range,FILE *fp)
  {
	  mHashRange = hash_range;

	  mHashBits = 1;
	  while((1 << mHashBits) < mHashRange) 
	    mHashBits++;
	  mVirtualRange = (1 << mHashBits);
	  
	  InitSeed(fp);
  	
  }
  
  virtual ~HashFunc()
  {}

  int GetHashValue(unsigned int user_id)
  {
	  static int accessed = 0;
	  if (accessed == 0) {
		printf("DH hash is accessed\n\n");
		accessed = 1;
	  }
	  /*
	  TODO: not figure out the function of accessed
	  */
      return HashIt(user_id, mBitMasks, mHashBits);
  }
  
  void InitSeed()
  {
    unsigned int seed_array[60] = {
	2094014637,
	1039786782,
	996651626,
	1145093471,
	763929835,
	719948037,
	1743558597,
	2103420707,
	1569021576,
	726516370,
	1966438593,
	940784875,
	1477109050,
	1363746149,
	764741341,
	2078336220,
	231221586,
	1495833406,
	2055047548,
	1192720220,
	2065922899,
	389317937,
	1044452437,
	250697621,
	830185057,
	161007528,
	693667324,
	1823318421,
	321544435,
	685395154,
	779317552,
	268075424,
	1725181936,
	1775969178,
	1413168896,
	341628123,
	348433568,
	1009243845,
	297565183,
	1917455144,
	1735760215,
	116520128,
	710756371,
	1065385617,
	1480266277,
	1475497713,
	996238190,
	1711487863,
	823847471,
	903802090,
	756724435,
	742286723,
	1293120027,
	1801176872,
	992984344,
	2123305084,
	1962184400,
	1686651669,
	1799139858,
	136245187
	};
	InitSeed(seed_array);
  }
  void InitSeed(unsigned int seed_array[])
  {
    int i;
    for (i = 0; i < mHashBits; i++) {
  	mBitMasks[i] = seed_array[3 * i];
    }
  }
  
  void InitSeed(FILE *fp)
	{
	int i;
	for (i=0; i<mHashBits; i++) {
		if (fscanf(fp, "%u", &mBitMasks[i]) != 1) {
		printf("Error reading hashing seed file\n");
		exit(1);
		}
	 }
	}
  
   int GetHashRange()
	{
	  return mHashRange;
	}
		
};
#endif
