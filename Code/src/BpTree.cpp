#include <iostream>
#include <algorithm>
#include "BpTree.hpp"

BulkKeyFinder::BulkKeyFinder(int i) : key(i)
{
}

bool BulkKeyFinder::operator()(const std::pair<int, std::vector<Rid>> &pair)
{
	return pair.first == key;
}

BpTree::BpTree(int o) : order(o), leaf(true), keys(nullptr), vals(nullptr), fils(), parent(NULL), root(NULL)
{
	vals = new std::vector<leaf_type>();
}

BpTree::~BpTree() {}

BpTree *BpTree::init(bool leaf)
{
	BpTree *BT = new BpTree(order);

	BT->leaf = leaf;
	BT->vals = NULL;
	BT->keys = NULL;
	BT->fils.resize(order, NULL);
	BT->parent = NULL;

	if (leaf)
		BT->vals = new std::vector<leaf_type>;
	else
		BT->keys = new std::vector<int>;

	return BT;
}

void BpTree::split(BpTree *BT)
{
	BpTree *parent = BT->parent;
	BpTree *bro = init(true);

	if (BT->parent == NULL)
		parent = init(false);

	BT->parent = parent;
	bro->parent = parent;

	int mid = order / 2;
	bro->vals->insert(bro->vals->end(), BT->vals->begin(), BT->vals->begin() + mid);
	*(BT->vals) = std::vector<leaf_type>(BT->vals->begin() + mid, BT->vals->end());

	parent->keys->push_back(BT->vals->at(0).first);
	parent->fils.push_back(bro);
	parent->fils.push_back(BT);
}

std::vector<Rid> BpTree::search(int i)
{
	BpTree *target = search(this, i);
	if (target == NULL)
		return {};
	std::vector<leaf_type>::iterator it = std::find_if(target->vals->begin(), target->vals->end(),
													   [i](const std::pair<int, std::vector<Rid>> &pair) {
												std::cout << "i = " << i << ", pair.first = " << pair.first << std::endl;
														   return i == pair.first;
													   });


	return it->second;
}

BpTree *BpTree::insertBulk(std::vector<leaf_type> bulk)
{
	for (size_t i = 0; i < bulk.size(); i++)
		insertBulk(bulk, this);
	return root;
}

void BpTree::insertBulk(std::vector<leaf_type> bulk, BpTree *root)
{
	while (!root->leaf)
		root = root->fils[root->fils.size() - 1];

	for (size_t i = 0; i < bulk.size(); i++)
	{
		root->vals->push_back(bulk[i]);
		if (root->vals->size() == (size_t)order)
			split(root);
	}

	while (root->parent)
		root = root->parent;
	this->root = root;
}

BpTree *BpTree::search(BpTree *root, int key)
{
	if (root->leaf)
		return root;
	if (key <= root->keys->at(0))
		return search(root->fils[0], key);
	for (size_t i = 0; i < root->keys->size() - 1; i++)
	{
		if (root->keys->at(i) < key && key <= root->keys->at(i + 1))
			return search(root->fils[i + 1], key);
	}
	if (root->keys->at(root->keys->size() - 1) < key)
		return search(root->fils[root->keys->size() - 1], key);
	return NULL;
}
