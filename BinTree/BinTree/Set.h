#pragma once

#include "BStree.h"

template<typename T>
struct Set {
public:
	Binary_Search_Tree<T> tree;

	Set() {
		tree = Binary_Search_Tree<T>();
	}

	Set(initializer_list<T> il) {
		tree = Binary_Search_Tree<T>();
		for (const auto& x : il)
			tree.insert(x);
	}

	Set(list<T> il) {
		tree = Binary_Search_Tree<T>();
		for (const auto& x : il)
			tree.insert(x);
	}

	void insert(const T& value)
	{
		tree.insert(value);
	}

	void erase(T value)
	{
		tree.erase(value);
	}

	bool exist(const T& value)
	{
		if (tree.findRecur(value) != nullptr)
			return true;
		return false;
	}
};