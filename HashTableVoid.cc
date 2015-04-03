
//
// Implementation of a HashTable that stores void *
//
#include "HashTableVoid.h"
#include <stdio.h>
// Obtain the hash code of a key
int HashTableVoid::hash(const char * key)
{
  int i = 0;
  const char * k = key;
  while(*k) {
    i += *k;
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
  // Get hash bucket 
  int h = hash(key);
  HashTableVoidEntry * e = _buckets[h]; 
  while (e!=NULL) {
   if (!strcmp(e->_key, key)) { // Entry found
     e->_data = data;
   return true;
  }
  e = e->_next;
  }
  // Entry not found. Add it.
  e = new HashTableVoidEntry; e->_key = strdup(key); 
  e->_data = data;
  e->_next = _buckets[h]; 
  _buckets[h] = e;
  return false;
}

// Find a key in the dictionary and place in "data" the corresponding record
// Returns false if key is does not exist
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
  // Add implementation here 
   // Get hash bucket 
   int h = hash(key);
   HashTableVoidEntry * e = _buckets[h]; 
   HashTableVoidEntry * prev = NULL; 
   while (e!=NULL) {
   if (!strcmp(e->_key, key)) { // Entry found
    if (prev != NULL) {
     prev->_next = e->_next; }
    else {
     _buckets[h] = e->_next;
    }
    //free(e->_key);
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
   //printf("%d",_hashTable->_buckets);
   //for(_currentBucket = 0; _currentBucket < _hashTable->TableSize;_currentBucket++) { 
     _currentEntry = _hashTable->_buckets[_currentBucket]; // hashtablevoid entry
     //while(_currentEntry != NULL) { 
       //printf("_currentBucket: %s\n",_currentEntry->_key); // pass &key and data
       //_currentEntry = _currentEntry->_next;
     //}
   //}
}

// Returns true if there is a next element. Stores data value in data.
bool HashTableVoidIterator::next(const char * & key, void * & data)
{
  if(_currentEntry != NULL) { 
       _currentEntry = _hashTable->_buckets[_currentBucket];
       printf("_currentBucket: %d at %s HERE\n",(int*)_currentEntry->_data,_currentEntry->_key); // pass &key and data
       //_currentEntry = _currentEntry->_next; //next linked list-- if not null return this val next time
       if(_currentEntry != NULL) {
        _currentBucket++;
         return true;
       }
  } else {
      for(; _currentBucket < _hashTable->TableSize;_currentBucket++) { 

        _currentEntry = _hashTable->_buckets[_currentBucket]; // hashtablevoid entry
        //_currentEntry = _currentEntry->_next; 
       if(_currentEntry != NULL) {
         _currentBucket++;
         printf("_currentBucket: %d at %s\n",(int*)_currentEntry->_data,_currentEntry->_key);
         return true;
       }
    }
  }
  return false;
}

