#pragma once

#include <iostream>

using namespace std;


template<class K, int M = 3> // M阶（M个子树）
struct BTreeNode
{
	K _keys[M]; // 本来为M-1个  这里多创建一个 方便插入排序 、找中位数等操作
	BTreeNode<K, M>* _subs[M + 1]; //存放子树结点 子树比Key值个数多一个
	BTreeNode<K, M>* _parent;
	size_t _size; // 实际的key的个数

	BTreeNode()
		:_parent(NULL)
		, _size(0)
	{
		for (int i = 0; i < M; i++)
		{
			_subs[i] = NULL;
		}
	}
};


template <class K, class V>
struct Pair
{
	K _first;       // 记录父亲结点 
	V _second;      // 查找时，记录找到的位于数组中的位置 如果没找到用-1表示

	Pair(const K& key = K(), const V& value = V())
		:_first(key)
		, _second(value)
	{}
};


template <class K, int M = 3>
class BTree
{
	typedef BTreeNode<K, M> Node;
public:
	BTree()
		:_root(NULL)
	{}

	//查找
	Pair<Node*, int> Find(const K& key)
	{
		Node* parent = NULL;
		Node* cur = _root;
		while (cur)
		{
			size_t index = 0;
			while (index < cur->_size)
			{
				if (cur->_keys[index] < key)
				{
					index++;
				}
				else if (key == cur->_keys[index])
				{
					// 找到了 返回当前结点 同时返回下标
					return Pair<Node*, int>(cur, index);
				}
				else  // key < cur->-keys[index] 
				{
					break;
				}
			}
			parent = cur;
			cur = cur->_subs[index];
		}

		// 没找见 返回父节点
		return Pair<Node*, int>(parent, -1);
	}

	bool Insert(const K& key)
	{
		if (_root == NULL)
		{
			_root = new Node;
			_root->_keys[0] = key;
			_root->_size++;
			return true;
		}

		Pair<Node*, int> ret = Find(key);
		// 有重复
		if (ret._second != -1)
		{
			return false;
		}

		Node* cur = ret._first;
		K InsertKey = key;
		Node* sub = NULL;
		while (1)
		{
			_InsertKey(cur, InsertKey, sub);
			// 处理 keys 数量超过 或 等于M的情况
			if (cur->_size < M)
			{
				return true;
			}

			// keys 关键字个数不小于M 需要分裂 向上调整
			//找出中位数
			int mid = (cur->_size - 1) / 2;
			int index = 0;
			Node* tmp = new Node;

			// 拷贝keys
			for (size_t i = mid + 1; i < cur->_size; i++)
			{
				tmp->_keys[index++] = cur->_keys[i];
				tmp->_size++;
			}

			// 拷贝subs
			index = 0;
			// 用<= 因为 子节点比 keys多一个
			for (size_t i = mid + 1; i <= cur->_size; i++)
			{
				tmp->_subs[index++] = cur->_subs[i];
				// 更新subs的父节点
				if (cur->_subs[i])
				{
					cur->_subs[i]->_parent = tmp;
				}
			}

			//调整 cur的 size
			cur->_size -= tmp->_size + 1; // 1 是那个中位数

			if (cur->_parent == NULL) // 分裂到根节点了 要创建新的结点来作为根节点
			{
				_root = new Node;
				_root->_keys[0] = cur->_keys[mid];
				_root->_subs[0] = cur;
				_root->_subs[1] = tmp;
				_root->_size = 1;
				cur->_parent = tmp->_parent = _root;
				return true;
			}
			else
			{
				InsertKey = cur->_keys[mid];
				sub = tmp;
				cur = cur->_parent;
			}
		}

		return true;
	}

	void InOrder()
	{
		_InOrder(_root);
		cout << endl;
	}


protected:

	void _InsertKey(Node* cur, const K& key, Node* sub)
	{
		// 插入排序
		int index = cur->_size - 1;
		while (index >= 0 && cur->_keys[index] > key)
		{
			cur->_keys[index + 1] = cur->_keys[index];
			cur->_subs[index + 2] = cur->_subs[index + 1];
			--index;
		}

		cur->_keys[index + 1] = key;
		cur->_subs[index + 2] = sub;
		if (sub)
		{
			sub->_parent = cur;
		}

		cur->_size++;
	}

	void _InOrder(Node* root)
	{
		if (root == NULL)
		{
			return;
		}

		for (size_t i = 0; i < root->_size; i++)
		{
			_InOrder(root->_subs[i]);
			cout << root->_keys[i] << " ";
		}

		// 最后访问  不能在循环中 因为keys[i] 和 keys[i+1] 中间公用一个 subs[i+1]
		_InOrder(root->_subs[root->_size]);
	}



protected:
	Node* _root;
};
