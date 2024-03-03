#pragma once

#include <iostream>
#include <string>
#include <functional>
#include "File.h"
#include "Movie.h"



template <typename Data, typename Key>
class HashTable {
	typedef size_t Index;
private:

	struct Node {
		Index index;
		Data content;
		bool isFree;
		bool isDeleted;

		
		Node() : index(-1), content(), isFree(true), isDeleted(false) {}

		explicit Node(const Data& content) : index(-1), content(content), isFree(false), isDeleted(false) {}

		inline friend std::ostream& operator<<(std::ostream& stream, const Node& node) {
			stream << node.index << " " << node.content << " " << node.isFree << " " << node.isDeleted;
			return stream;
		}
	};

	Node* nodes;

	size_t tableSize;
	size_t deletedCounter = 0;
	size_t busyCounter = 0;
	size_t modulo;

	const std::function <size_t(const Key & )> hashFunction;
	const std::function <Key (const Data & )> keyGetter;

	
public:
	template <typename Data, typename Key> HashTable(const std::function <size_t(const Key &)> & hashFunction,
													 const std::function <Key (const Data & )> & keyGetter,
		                                             const size_t & size = 0) : 
		tableSize(size), hashFunction(hashFunction), keyGetter(keyGetter), modulo(findNearestPrime(tableSize)), nodes(new Node[size]{}) {}

private:
	void resize() {
		Node* toDelete = nodes;
		tableSize *= 2;
		nodes = new Node[tableSize];

		for (int i = 0; i < tableSize / 2; ++i)
			if (!nodes[i].isDeleted)
				nodes[i] = toDelete[i];

		deletedCounter = 0;
		delete[] toDelete;

		modulo = findNearestPrime(tableSize);
	}

	const double getLoadFactor() const {
		return (busyCounter + deletedCounter) / static_cast<double>(tableSize);
	}

	const size_t findNearestPrime(const size_t & number) const {
		if (number < 3)
			return 1;

		for (int currentNumber = number, isPrime = true; ; --currentNumber, isPrime = true) {
			for (int j = 2; j <= sqrt(currentNumber); j++) {
				if (currentNumber % j == 0) {
					isPrime = false;
					break;
				}
			}
			if (isPrime)
				return currentNumber;
		}

	}

public:

	void add(const Data & content) {

		Index i = hashFunction(keyGetter(content)) % modulo;

		while ((i < tableSize) && ((!nodes[i].isFree) || (nodes[i].isDeleted)))
			++i;

		if (i >= tableSize || getLoadFactor() > 0.75)
			resize();

		nodes[i].index = i;
		nodes[i].content = content;
		nodes[i].isFree = false;

		++busyCounter;
	}

	void remove(const Key & keyValue) {
		Index i = hashFunction(keyValue) % modulo;

		while ( (i < tableSize) && nodes[i].isDeleted )
			++i;

		if (i >= tableSize)
			return;

		nodes[i].isDeleted = true;

		++deletedCounter;
	}


	bool find(const Key& keyValue) const {
		Index i = hashFunction(keyValue) % modulo;

		while ((i < tableSize) && (nodes[i].isDeleted))
			++i;
		
		if (i >= tableSize)
			return false;

		return true;
	}

	Data& operator[](const Key& keyValue) throw(std::out_of_range);

	const Data operator[](const Key& keyValue) const;

	~HashTable() {
		delete[] nodes;
	}
};


template <typename Data, typename Key>
std::ostream& operator<<(std::ostream& stream, const HashTable<Data, Key> & object) {
	stream << "===========================================\n";
	for (int i = 0; i < object.tableSize; ++i)
		if ( (object.nodes[i].index != -1) && (!object.nodes[i].isDeleted))
			std::cout << object.nodes[i] << std::endl;
	stream << "===========================================\n";
	return stream;
}


template <typename Data, typename Key>
Data& HashTable<Data, Key>::operator[](const Key& keyValue) throw(std::out_of_range) {
	Index i = hashFunction(keyValue) % modulo;

	while ( (i < tableSize) && (nodes[i].isDeleted) )
			++i;

	if (i >= tableSize)
		throw std::out_of_range("There is no such key in the table.");

	return nodes[i].content;
}


template <typename Data, typename Key>
const Data HashTable<Data, Key>::operator[](const Key& keyValue) const  {
	Index i = hashFunction(keyValue) % modulo;

	while ( (i < tableSize) && nodes[i].isDeleted )
		++i;
	
	if (i >= tableSize)
		return Node{};

	return nodes[i].content;
}

