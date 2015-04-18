
//
// Hash Table
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>

// Each hash entry stores a key, object pair
struct HashTableVoidEntry {
  const char * _key;
  void * _data;
  HashTableVoidEntry * _next;
};

// This is a Hash table that maps string keys to objects of type Data
class HashTableVoid {
 public:
  // Number of buckets
  enum { TableSize = 2039};
  
  // Array of the hash buckets.
  HashTableVoidEntry **_buckets;
  
  // Obtain the hash code of a key
  int hash(const char * key);
  
 public:
  HashTableVoid();
  
  // Add a record to the hash table. Returns true if key already exists.
  // Substitute content if key already exists.
  bool insertItem( const char * key, void * data);
  
  // mod for where ircserver 
  bool insertItem2( int fd, const char * key, void * data, int num);
 
  bool insertItem3( int fd, const char * key, void * data, int num);
  // Find a key in the dictionary and place in "data" the corresponding record
  // Returns false if key is does not exist
  bool find( const char * key, void ** data);
  
  // mod to find username corresponding to msg
  bool find2( const char * key, void ** data, int roomCount);
  // Removes an element in the hash table. Return false if key does not exist.
  bool removeElement(const char * key);
  
  // mod to remove users from a room(bucket)
  bool removeElement2(const char * key, int roomCount); 
};

class HashTableVoidIterator {
  int _currentBucket;
  HashTableVoidEntry *_currentEntry;
  HashTableVoid * _hashTable;
 public:
  HashTableVoidIterator(HashTableVoid * hashTable);
  bool next(const char * & key, void * & data);
  void next2(int fd, const char * & key, void * & data, int num);
  void next3(int fd, const char * & key, void * & data, int num, int from);
  bool userInRoomExists(int fd, const char * user,int num); 
};

