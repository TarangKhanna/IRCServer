
//
// Implementation of a HashTable that stores void *
//
#include "HashTableVoid.h"
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <algorithm> 
using namespace std;
// Obtain hash of a key
// custom with multiplier
vector<string> sortVec;
int HashTableVoid::hash(const char * key) 
{
  int i = 0;
  int multiplier = 4;
  const char * k = key;
  while(*k) {
    i += (*k)*multiplier;
    multiplier++;
    k++;
  }
  return i%TableSize;
}

// Constructor for hash table. Initializes hash table
HashTableVoid::HashTableVoid()
{
  _buckets = (HashTableVoidEntry **) malloc(TableSize*sizeof(HashTableVoidEntry*));
  for ( int i = 0; i < TableSize; i++) {
   _buckets[i] = NULL;
  }     
}

// Add a record to the hash table. Returns true if key already exists.
// Substitute content if key already exists.
bool HashTableVoid::insertItem( const char * key, void * data)
{
  // Bucket get 
  int h = hash(key);
  HashTableVoidEntry * entry = _buckets[h]; 
  while (entry!=NULL) {
   if (!strcmp(entry->_key, key)) { // Entry found
     entry->_data = data;
   return true;
   }
   entry = entry->_next;
  }
  // Entry not found. Add it.
  entry = new HashTableVoidEntry;
  entry->_key = strdup(key); 
  entry->_data = data;
  entry->_next = _buckets[h]; 
  _buckets[h] = entry;
  return false;
}

// Mod of the inserItem function earlier to be used in the IrcServer
bool HashTableVoid::insertItem2( const char * key, void * data, int num)
{
  // use first bucket  
  int h = num;
  HashTableVoidEntry * entry = _buckets[h]; 
  while (entry!=NULL) {
   if (!strcmp(entry->_key, key)) { // Entry found
     entry->_data = data;
   return true;
   }
   entry = entry->_next;
  }
  // Entry not found. Add it.
  entry = new HashTableVoidEntry;
  entry->_key = strdup(key); 
  entry->_data = data;
  entry->_next = _buckets[h]; 
  _buckets[h] = entry;
  return false;
}

// Find a key in the dictionary and place in "data" the corresponding record
// Returns false if key does not exist
bool HashTableVoid::find( const char * key, void ** data)
{
  // Get hash bucket 
   int h = hash(key);
   HashTableVoidEntry * e = _buckets[h]; 
   while (e!=NULL) {
    if (!strcmp(e->_key, key)) { // Entry found
      *data = e->_data;
    return true; 
   }
   e = e->_next;
   }
   return false;
}

// Removes an element in the hash table. Return false if key does not exist.
bool HashTableVoid::removeElement(const char * key)
{
   // Get hash bucket 
   int h = hash(key);
   HashTableVoidEntry * e = _buckets[h]; 
   HashTableVoidEntry * prev = NULL; 
   while (e!=NULL) {
   if (!strcmp(e->_key, key)) { // found
    if (prev != NULL) {
     prev->_next = e->_next; }
    else {
     _buckets[h] = e->_next;
    }
    //free(e->_key); // does not seem to work
    delete e;
    return true;
   }
   prev = e;
   e = e->_next;
   }
  return false;
}


// Creates an iterator object for this hash table
HashTableVoidIterator::HashTableVoidIterator(HashTableVoid * hashTable)
{  
   _hashTable = hashTable;
   _currentBucket = 0;
   _currentEntry = _hashTable->_buckets[_currentBucket]; // hashtablevoid entry
}
 
// Returns true if there is a next element. Stores data value in data.
bool HashTableVoidIterator::next(const char * & key, void * & data)
{
  if(_currentEntry != NULL) { 
        //printf("_currentBucket: %d at %s HERE\n",(int*)_currentEntry->_data,_currentEntry->_key);
        data = _currentEntry->_data;
        key = _currentEntry->_key;
        _currentBucket++;
        _currentEntry = _hashTable->_buckets[_currentBucket];
        return true;
  } else {
      for(; _currentBucket < _hashTable->TableSize;_currentBucket++) { 
      _currentEntry = _hashTable->_buckets[_currentBucket];
       if(_currentEntry != NULL) {
       //printf("_currentBucket: %d at %s\n",(int*)_currentEntry->_data , _currentEntry->_key);
         data = _currentEntry->_data;
         key = _currentEntry->_key;
         _currentBucket++;
         _currentEntry = _hashTable->_buckets[_currentBucket];
         return true;
       }
    }
  }
  return false;
}
// to iterate over merely the linked list- bucket = 0 gets users in room
void HashTableVoidIterator::next2(int fd, const char * & key, void * & data, int num) // sort this
{
   sortVec.clear();
   _currentEntry = _hashTable->_buckets[num];
   while(_currentEntry != NULL) { 
        data = _currentEntry->_data; // pass
        key = _currentEntry->_key; // user 
        _currentEntry = _currentEntry->_next;
        sortVec.push_back(key);
   }
   sort(sortVec.begin(), sortVec.end());
   for(int i = 0; i < sortVec.size(); i++) {
       const char * msg = sortVec[i].c_str();
       write(fd, msg, strlen(msg));
       const char * msg3 = "\r\n";
       write(fd, msg3, strlen(msg3));
    }
    const char * msg4 = "\r\n";
    write(fd, msg4, strlen(msg4));
    return;
}

bool HashTableVoid::removeElement2(const char * key, int roomCount)
{
   // Get hash bucket 
   int h = roomCount;
   HashTableVoidEntry * e = _buckets[h]; 
   HashTableVoidEntry * prev = NULL; 
   while (e!=NULL) {
   if (!strcmp(e->_key, key)) { // found
    if (prev != NULL) {
     prev->_next = e->_next; }
    else {
     _buckets[h] = e->_next;
    }
    //free(e->_key); // does not seem to work
    delete e;
    return true;
   }
   prev = e;
   e = e->_next;
   }
  return false;
}

