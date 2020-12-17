#ifndef BPTREE_HPP
#define BPTREE_HPP

#include "Rid.hpp"
#include <utility>
#include <vector>
#include <iostream>

struct BulkKeyFinder
{
	int key;
	BulkKeyFinder(int i);

	bool operator()(const std::pair<int, std::vector<Rid>>& pair);
};

class BpTree
{
private:
	typedef std::pair<int, std::vector<Rid>> leaf_type;
	typedef int key_type;

	int order;

	bool leaf;
	std::vector<key_type> *keys;
	std::vector<leaf_type> *vals;

	std::vector<BpTree*> fils;
	BpTree *parent;
	BpTree *root;

	void split(BpTree *BT);
	BpTree *init(bool leaf);
	void insertBulk(std::vector<leaf_type> bulk, BpTree *root);
	BpTree *search(BpTree *root, int i);

public:
	BpTree(int order);

	BpTree(const BpTree &other);

	BpTree &operator=(const BpTree &other);

	~BpTree();

	std::vector<Rid> search(int i);
	BpTree *insertBulk(std::vector<leaf_type> bulk);
};

#endif
