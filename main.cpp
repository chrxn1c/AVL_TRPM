#include <iostream>
#include <string>
#include <functional>


template <typename Key>
class BinaryTree {

	struct Node {
		Key keyValue;
		Node* parent;
		Node* left;
		Node* right;

		Node() : keyValue{}, parent(nullptr), left(nullptr), right(nullptr) {}
		Node(const Key & keyValue) : keyValue(keyValue), parent(nullptr), left(nullptr), right(nullptr) {}
		Node(Node* parent, Node* left, Node* right, const Key & keyValue) : keyValue(keyValue), parent(parent), left(left), right(right) {}

		void printNode(const size_t& level) const {
			if (!this)
				return;

			std::cout << std::endl;
			this->right->printNode(level + 5);
			for (int i = 0; i < level; ++i)
				std::cout << " ";
			std::cout << keyValue << std::endl;
			this->left->printNode(level + 5);
		}


		void print_symmetrical_DFS() const {
			if (!this)
				return;

			this->left->print_symmetrical_DFS();
			std::cout << keyValue << " ";
			this->right->print_symmetrical_DFS();
		}


		Key getMaxValue() const {

			if (!this->right)
				return this->keyValue;
			else
				return this->right->getMaxValue();

		}

		Node& getFarRightNode() const {

			if (!this->right)
				return *(const_cast<Node*>(this));
			else
				return this->right->getFarRightNode();
		}


		bool isFound(const Key& keyValue, const std::function <bool(const Key& content1, const Key& content2)>& comparator) const {

			if (!this)
				return false;

			if (this->keyValue == keyValue)
				return true;

			if (!comparator(keyValue, this->keyValue))
				return this->left->isFound(keyValue, comparator);
			else
				return this->right->isFound(keyValue, comparator);
		}


		Node& find(const Key& keyValue, const std::function <bool(const Key & key1, const Key & key2)> & comparator) const throw(std::out_of_range) {

			if (this == nullptr)
				throw std::out_of_range("Cannot find such a value in the tree.");

			if (this->keyValue == keyValue)
				return *(const_cast<Node*>(this));

			if (!comparator(keyValue, this->keyValue))
				return this->left->find(keyValue, comparator);
			else
				return this->right->find(keyValue, comparator);
		}

		Node*& getParentalPointer() throw(std::invalid_argument) {
			if (this->parent->left == this)
				return this->parent->left;
			if (this->parent->right == this)
				return this->parent->right;

			throw std::invalid_argument("Parent has no such child");
		}


		~Node() {
			if (this->left)
				delete this->left;
			if (this->right)
				delete this->right;
		}
	};

	const std::function <bool(const Key& key1, const Key& key2) > comparator;

	Node* start;

public:
	BinaryTree(const std::function <bool(const Key& content1, const Key& content2)> & comparator) : start(nullptr), comparator(comparator) {};

	void add(const Key & keyValue) {

		if (start == nullptr) {
			start = new Node(nullptr, nullptr, nullptr, keyValue);
			return;
		}

		Node* temporary = start;

		while (true) {

			if (!comparator(keyValue, temporary->keyValue))

				if (temporary->left == nullptr) {
					temporary->left = new Node(temporary, nullptr, nullptr, keyValue);
					break;
				}
				else
					temporary = temporary->left;

			else

				if (temporary->right == nullptr) {
					temporary->right = new Node{ temporary, nullptr, nullptr, keyValue };
					break;
				}
				else
					temporary = temporary->right;
		}
	}

	void display() const {
		std::cout << "=======================\n";
		this->start->printNode(10);
		std::cout << "=======================\n";
	}

	void symmetrical_DFS() const {
		std::cout << std::endl;
		this->start->print_symmetrical_DFS();
		std::cout << std::endl;
	}


	Key getMaxValue() const {
		return this->start->getMaxValue();
	}

	bool isFound(const Key& keyValue) const {
		return start->isFound(keyValue, comparator);
	}


	void deleteKey(const Key& keyValue) throw(std::out_of_range) {
		Node& toDelete = start->find(keyValue, comparator);

		//case 1: toDelete = leaf
		if (toDelete.right == nullptr && toDelete.left == nullptr) {
			toDelete.getParentalPointer() = nullptr;
			delete& toDelete;
			return;
		}

		//case 2: toDelete has only left subtree
		if (toDelete.left != nullptr && toDelete.right == nullptr) {
			Node& replacement = *(toDelete.left);
			replacement.parent = toDelete.parent;
			toDelete.getParentalPointer() = &replacement;

			toDelete.left = nullptr;
			delete& toDelete;
			return;
		}

		//case 3: toDelete has only right subtree
		if (toDelete.left == nullptr && toDelete.right != nullptr) {
			Node& replacement = *(toDelete.right);
			replacement.parent = toDelete.parent;
			toDelete.getParentalPointer() = &replacement;

			toDelete.right = nullptr;
			delete& toDelete;
			return;
		}

		//case 4: toDelete has both left and right subtree
		if (toDelete.left != nullptr && toDelete.right != nullptr) {

			Node& replacement = toDelete.left->getFarRightNode();
			toDelete.content = replacement.content;

			//if replacement has its subtree, we have to take that into account
			if (replacement.left) {
				replacement.left->parent = replacement.parent;
				replacement.left = nullptr;
			}

			replacement.getParentalPointer() = replacement.left;

			delete& replacement;
			return;
		}
	}


	~BinaryTree() {
		delete start;
	}
};


template <typename Key>
class AVL_Tree {

private:

	const std::function <bool(const Key& content1, const Key& content2) > comparator;
public:
	struct AVL_Node;
private:
	AVL_Node* start;
	AVL_Node*& getStart() { return start; }
public:
	struct AVL_Node {
		Key keyValue;
		AVL_Node* parent;
		AVL_Node* left;
		AVL_Node* right;
		int balance = 0;

		AVL_Node() : keyValue{}, parent(nullptr), left(nullptr), right(nullptr) {}
		AVL_Node(const Key & keyValue) : keyValue(keyValue), parent(nullptr), left(nullptr), right(nullptr) {}
		AVL_Node(AVL_Node* parent, AVL_Node* left, AVL_Node* right, const Key & keyValue) : keyValue(keyValue), parent(parent), left(left), right(right) {}

		AVL_Node& find(const Key& keyValue, const std::function <bool(const Key& content1, const Key& content2)>& comparator) const throw(std::out_of_range) {

			if (this == nullptr)
				throw std::out_of_range("Cannot find such a value in the tree.");

			if (this->keyValue == keyValue)
				return *(const_cast<AVL_Node*>(this));

			if (!comparator(keyValue, this->keyValue))
				return this->left->find(keyValue, comparator);
			else
				return this->right->find(keyValue, comparator);
		}


		void printNode(const size_t& level) const {
			if (!this)
				return;

			std::cout << std::endl;
			this->right->printNode(level + 5);
			for (int i = 0; i < level; ++i)
				std::cout << " ";
			std::cout << keyValue << std::endl;
			this->left->printNode(level + 5);
		}


		AVL_Node& getFarRightNode() const {

			if (!this->right)
				return *(const_cast<AVL_Node*>(this));
			else
				return this->right->getFarRightNode();
		}


		AVL_Node*& getParentalPointer() throw(std::invalid_argument) {
			if (this->parent->left == this)
				return this->parent->left;
			if (this->parent->right == this)
				return this->parent->right;

			throw std::invalid_argument("Parent has no such child");
		}


		//memory leak: not deallocating memory occupied by unused elements
		//not passing by reference because arg is this
		//reoccupy start node in case when it changes, object is given
		void balanceLeft(AVL_Node*& startNode) {

			if (this->right->left) {
				this->right->left->right = new AVL_Node(this->right->left, nullptr, nullptr, this->right->left->keyValue);
				this->right->left->keyValue = this->keyValue;
				++(this->right->left->balance);
			}
			else {
				this->right->left = new AVL_Node(this->right, nullptr, nullptr, this->keyValue);
				--(this->right->balance);
			}

			if (this->left) {
				this->right->left->left = new AVL_Node(this->right->left, nullptr, nullptr, this->left->keyValue);
				--(this->right->left->balance);
			}

			if (this->parent) {
				this->parent->addUpToRoute(-1);

				this->getParentalPointer() = this->right;
				this->right->parent = this->parent;
			}

			//memory leak: not deleting current start
			else {
				startNode = this->right;
				startNode->parent = nullptr;
			}

			startNode->rebalanceTree();
		}


		//memory leak: not deallocating memory occupied by unused elements
		//not passing by reference because arg is this
		void balanceRight(AVL_Node*& startNode) {

			if (this->left->right) {
				this->left->right->left = new AVL_Node(this->left->right, nullptr, nullptr, this->left->right->keyValue);
				this->left->right->keyValue = this->keyValue;
				--(this->left->right->balance);
			}
			else {
				this->left->right = new AVL_Node(this->left, nullptr, nullptr, this->keyValue);
				++(this->left->balance);
			}


			if (this->right) {
				this->left->right->right = new AVL_Node(this->left->right, nullptr, nullptr, this->right->keyValue);
				++(this->left->right->balance);
			}


			if (this->parent) {
				this->parent->addUpToRoute(1);

				this->getParentalPointer() = this->left;
				this->left->parent = this->parent;
			}

			//memory leak: not deleting current start
			else {
				startNode = this->left;
				startNode->parent = nullptr;
			}
			
			startNode->rebalanceTree();
		}


		void balanceLeftRight(AVL_Node*& startNode) {
			this->left->left = new AVL_Node(this->left, nullptr, nullptr, this->left->keyValue);
			this->left->balance = -1;
			this->left->keyValue = this->left->right->keyValue;
			delete this->left->right;
			this->left->right = nullptr;

			this->balanceRight(startNode);
		}


		void balanceRightLeft(AVL_Node*& startNode) {
			this->right->right = new AVL_Node(this->right, nullptr, nullptr, this->right->keyValue);
			this->right->balance = 1;
			this->right->keyValue = this->right->left->keyValue;
			delete this->right->left;
			this->right->left = nullptr;

			this->balanceLeft(startNode);
		}


		void addNode(const Key& element, const std::function <bool(const Key& content1, const Key& content2) >& comparator, AVL_Node*& startNode) {
			// to the left
			if (!comparator(element, this->keyValue)) {
				--(this->balance);
				if (this->left == nullptr) {
					this->left = new AVL_Node(this, nullptr, nullptr, element);
					return;
				}
				else {
					this->left->addNode(element, comparator, startNode);
					if (this->balance == -2 && this->left->balance == -1)
						this->balanceRight(startNode); //fix balance method
					if (this->balance == -2 && this->left->balance == 1) {
						this->balanceLeftRight(startNode);
					}
				}
			}

			else {
				++(this->balance);
				if (this->right == nullptr) {
					this->right = new AVL_Node(this, nullptr, nullptr, element);
					return;
				}
				else {
					this->right->addNode(element, comparator, startNode);
					if (this->balance == 2 && this->right->balance == 1)
						this->balanceLeft(startNode);
				}
			}
		}

		void addUpToRoute(const size_t& additionalBalance) {
			this->balance += additionalBalance;
			if (this->parent)
				parent->addUpToRoute(additionalBalance);
		}

		size_t helpCountHeight() {
			size_t result = 1, leftHeight = 0, rightHeight = 0;
			if (this->left)
				leftHeight += this->left->helpCountHeight();
			if (this->right)
				rightHeight += this->right->helpCountHeight();

			return (leftHeight > rightHeight ? 1 + leftHeight : 1 + rightHeight);
		}

		size_t calculateHeight() {
			size_t result = 0;
			if (this->right)
				result += this->right->helpCountHeight();
			if (this->left)
				result -= this->left->helpCountHeight();
			return result;
		}

		void rebalanceTree() {
			this->balance = calculateHeight();
			if (this->left)
				this->left->rebalanceTree();
			if (this->right)
				this->right->rebalanceTree();
		}

		~AVL_Node() {
			if (this->left)
				delete this->left;
			if (this->right)
				delete this->right;
		}
	};

	AVL_Tree(const std::function <bool(const Key& content1, const Key& content2)>& comparator) : start(nullptr), comparator(comparator) {};

	//TODO: correct the add method
	void add(const Key& element) {

		if (start == nullptr) {
			start = new AVL_Node(nullptr, nullptr, nullptr, element);
			return;
		}

		start->addNode(element, comparator, this->start);
	}


	void display() const {
		std::cout << "=======================\n";
		this->start->printNode(10);
		std::cout << "=======================\n";
	}

private:
	//TODO: to be corrected
	//void defaultRemove(const Key& keyValue) throw(std::out_of_range) {
	//	AVL_Node& toDelete = start->find(keyValue, comparator);

	//	//case 1: toDelete = leaf
	//	if (toDelete.right == nullptr && toDelete.left == nullptr) {
	//		toDelete.getParentalPointer() = nullptr;
	//		delete& toDelete;
	//		return;
	//	}

	//	//case 2: toDelete has only left subtree
	//	if (toDelete.left != nullptr && toDelete.right == nullptr) {
	//		AVL_Node& replacement = *(toDelete.left);
	//		replacement.parent = toDelete.parent;
	//		toDelete.getParentalPointer() = &replacement;

	//		toDelete.left = nullptr;
	//		delete& toDelete;
	//		return;
	//	}

	//	//case 3: toDelete has only right subtree
	//	if (toDelete.left == nullptr && toDelete.right != nullptr) {
	//		AVL_Node& replacement = *(toDelete.right);
	//		replacement.parent = toDelete.parent;
	//		toDelete.getParentalPointer() = &replacement;

	//		toDelete.right = nullptr;
	//		delete& toDelete;
	//		return;
	//	}

	//	//case 4: toDelete has both left and right subtree
	//	if (toDelete.left != nullptr && toDelete.right != nullptr) {

	//		AVL_Node& replacement = toDelete.left->getFarRightNode();
	//		toDelete.keyValue = replacement.keyValue;

	//		//if replacement has its subtree, we have to take that into account
	//		if (replacement.left) {
	//			replacement.left->parent = replacement.parent;
	//			replacement.left = nullptr;
	//		}

	//		replacement.getParentalPointer() = replacement.left;

	//		delete& replacement;
	//		return;
	//	}
	//}
public:
	bool isFound(const Key& keyValue) const {
		return start->isFound(keyValue, comparator);
	}

	
	void remove(Key keyValue) {
		AVL_Node* firstParent = nullptr;
		AVL_Node* temporary = nullptr;
		Key replacementKeyValue = Key{};

		//defaultRemove(keyValue);
		AVL_Node& toDelete = start->find(keyValue, comparator);


		//case 0: toDelete = start = onlyNode
		if (keyValue == start->keyValue) {
			delete start;
			start = nullptr;
			return;
		}

		//case 1: toDelete = leaf
		if (toDelete.right == nullptr && toDelete.left == nullptr) {

			firstParent = toDelete.parent;
			replacementKeyValue = toDelete.keyValue;

			toDelete.getParentalPointer() = nullptr;

			delete& toDelete;
		}

		//case 2: toDelete has only left subtree
		else if (toDelete.left != nullptr && toDelete.right == nullptr) {
			AVL_Node& replacement = *(toDelete.left);

			firstParent = &replacement;
			replacementKeyValue = replacement.keyValue;

			replacement.parent = toDelete.parent;
			toDelete.getParentalPointer() = &replacement;

			toDelete.left = nullptr;
			delete& toDelete;
		}

		//case 3: toDelete has only right subtree
		else if (toDelete.left == nullptr && toDelete.right != nullptr) {
			AVL_Node& replacement = *(toDelete.right);

			firstParent = &replacement;
			replacementKeyValue = replacement.keyValue;

			replacement.parent = toDelete.parent;
			toDelete.getParentalPointer() = &replacement;

			toDelete.right = nullptr;
			delete& toDelete;
		}

		//case 4: toDelete has both left and right subtree
		else if (toDelete.left != nullptr && toDelete.right != nullptr) {

			AVL_Node& replacement = toDelete.left->getFarRightNode();

			firstParent = &toDelete;
			replacementKeyValue = replacement.keyValue;

			toDelete.keyValue = replacement.keyValue;

			//if replacement has its subtree, we have to take that into account
			if (replacement.left) {
				replacement.left->parent = replacement.parent;
				replacement.left = nullptr;
			}

			replacement.getParentalPointer() = replacement.left;

			delete& replacement;

		}

		start->rebalanceTree();

		temporary = firstParent;
		while (temporary != nullptr) {
			if (temporary->left && temporary->balance <= -2 && temporary->left->balance >= 1)
				temporary->balanceLeftRight(start);

			if (temporary->right && temporary->balance >= 2 && temporary->right->balance <= -1)
				temporary->balanceRightLeft(start);


			if (temporary->left && temporary->balance <= -2 && temporary->left->balance >= -1)
				temporary->balanceRight(start);
			
			if (temporary->right && temporary->balance >= 2 && temporary->right->balance <= 1)
				temporary->balanceLeft(start);

			temporary = temporary->parent;
		}
	}

	
	~AVL_Tree() {
		delete start;
	}

};


int main() {

	BinaryTree <int> alphaTree([](const int& string1, const int& string2) {return string1 > string2; });
	AVL_Tree <int> AVL_Tree([](const int& string1, const int& string2) {return string1 > string2; });

	int content{};
	std::cout << "Type in 5 words: ";


	alphaTree.add(12);
	alphaTree.add(10);
	alphaTree.add(25);
	alphaTree.add(8);
	alphaTree.add(11);


	AVL_Tree.add(5);
	AVL_Tree.remove(5);


	AVL_Tree.add(50);
	AVL_Tree.add(30);
	AVL_Tree.add(40);
	std::cout << "AVL tree: \n"; AVL_Tree.display();
	AVL_Tree.add(20);
	std::cout << "AVL tree: \n"; AVL_Tree.display();
	AVL_Tree.add(10);
	std::cout << "AVL tree: \n"; AVL_Tree.display();
	AVL_Tree.add(5);
	std::cout << "AVL tree: \n"; AVL_Tree.display();
	AVL_Tree.add(2);
	std::cout << "AVL tree: \n"; AVL_Tree.display();


	AVL_Tree.remove(40);
	std::cout << "AVL tree: \n"; AVL_Tree.display();
	AVL_Tree.remove(30);
	std::cout << "AVL tree: \n"; AVL_Tree.display();
	AVL_Tree.remove(50);
	std::cout << "AVL tree: \n"; AVL_Tree.display();
	return 0;
}