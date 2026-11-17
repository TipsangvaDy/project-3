// CS 2414 -- Data Structures -- Fall 2026 -- Project 3
// Medical Index (MedIndex)
//
// This starter file implements a hospital emergency-department patient index.
// Admitted patients are stored in an AVL tree keyed by patient ID.
// Patients who have checked in but are not yet admitted wait in a queue.
// A stack records recent successful operations so the last action can be undone.
//
// The AVL tree class is complete. Implement the rest of the program
// according to the project handout.

#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <deque>
#include <sstream>
using namespace std;

class Patient {
public:
    int id;
    string name;
    int acuity;

    Patient() : id(0), name(""), acuity(0) {}
    Patient(int i, const string& n, int a) : id(i), name(n), acuity(a) {}

    bool operator<(const Patient& other) const { return id < other.id; }
    bool operator>(const Patient& other) const { return id > other.id; }
    bool operator==(const Patient& other) const { return id == other.id; }
};

template <typename T>
class Node {
public:
    T data;
    Node* left;
    Node* right;
    int height;
    Node(T val) : data(val), left(nullptr), right(nullptr), height(0) {}
};

template <typename T>
class AVLTree {
    Node<T>* root;

    void destroy(Node<T>* node);
    void inorder(Node<T>* node) const;

    int height(Node<T>* node) const;
    void updateHeight(Node<T>* node);
    int balance(Node<T>* node) const;

    Node<T>* rotateRight(Node<T>* z);
    Node<T>* rotateLeft(Node<T>* z);

    Node<T>* insert(Node<T>* node, T key);
    Node<T>* remove(Node<T>* node, T key);
    Node<T>* minNode(Node<T>* node) const;

public:
    AVLTree() : root(nullptr) {}
    ~AVLTree() { destroy(root); }

    void insert(T key);
    void remove(T key);
    Node<T>* search(T key) const;
    void printInorder() const;
    int treeHeight() const { return height(root); }
    bool empty() const { return root == nullptr; }
};

template <typename T>
void AVLTree<T>::destroy(Node<T>* node) {
    if (node == nullptr) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

template <typename T>
int AVLTree<T>::height(Node<T>* node) const {
    if (node == nullptr) return -1;
    return node->height;
}

template <typename T>
void AVLTree<T>::updateHeight(Node<T>* node) {
    if (node == nullptr) return;
    int hl = height(node->left);
    int hr = height(node->right);
    node->height = 1 + (hl > hr ? hl : hr);
}

template <typename T>
int AVLTree<T>::balance(Node<T>* node) const {
    if (node == nullptr) return 0;
    return height(node->left) - height(node->right);
}

template <typename T>
Node<T>* AVLTree<T>::rotateRight(Node<T>* z) {
    Node<T>* y = z->left;
    Node<T>* t = y->right;
    y->right = z;
    z->left = t;
    updateHeight(z);
    updateHeight(y);
    return y;
}

template <typename T>
Node<T>* AVLTree<T>::rotateLeft(Node<T>* z) {
    Node<T>* y = z->right;
    Node<T>* t = y->left;
    y->left = z;
    z->right = t;
    updateHeight(z);
    updateHeight(y);
    return y;
}

template <typename T>
Node<T>* AVLTree<T>::insert(Node<T>* node, T key) {
    if (node == nullptr) return new Node<T>(key);
    if (key < node->data)
        node->left = insert(node->left, key);
    else if (key > node->data)
        node->right = insert(node->right, key);
    else
        return node;

    updateHeight(node);
    int bf = balance(node);

    if (bf > 1 && key < node->left->data)
        return rotateRight(node);
    if (bf < -1 && key > node->right->data)
        return rotateLeft(node);
    if (bf > 1 && key > node->left->data) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (bf < -1 && key < node->right->data) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

template <typename T>
void AVLTree<T>::insert(T key) {
    root = insert(root, key);
}

template <typename T>
Node<T>* AVLTree<T>::remove(Node<T>* node, T key) {
    if (node == nullptr) return nullptr;

    if (key < node->data)
        node->left = remove(node->left, key);
    else if (key > node->data)
        node->right = remove(node->right, key);
    else {
        if (node->left == nullptr || node->right == nullptr) {
            Node<T>* child = (node->left != nullptr) ? node->left : node->right;
            delete node;
            node = child;
        } else {
            Node<T>* succ = minNode(node->right);
            node->data = succ->data;
            node->right = remove(node->right, succ->data);
        }
    }

    if (node == nullptr) return nullptr;

    updateHeight(node);
    int bf = balance(node);

    if (bf > 1 && balance(node->left) >= 0)
        return rotateRight(node);
    if (bf > 1 && balance(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (bf < -1 && balance(node->right) <= 0)
        return rotateLeft(node);
    if (bf < -1 && balance(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

template <typename T>
void AVLTree<T>::remove(T key) {
    root = remove(root, key);
}

template <typename T>
Node<T>* AVLTree<T>::minNode(Node<T>* node) const {
    while (node->left != nullptr) node = node->left;
    return node;
}

template <typename T>
Node<T>* AVLTree<T>::search(T key) const {
    Node<T>* curr = root;
    while (curr != nullptr) {
        if (key == curr->data) return curr;
        if (key < curr->data) curr = curr->left;
        else curr = curr->right;
    }
    return nullptr;
}

template <typename T>
void AVLTree<T>::inorder(Node<T>* node) const {
    if (node == nullptr) return;
    inorder(node->left);
    cout << node->data.id << " " << node->data.name << " " << node->data.acuity << "\n";
    inorder(node->right);
}

template <typename T>
void AVLTree<T>::printInorder() const {
    if (root == nullptr) {
        cout << "(none)\n";
        return;
    }
    inorder(root);
}

// Records one successful mutating command so it can be undone.
// type: ADMIT, UPDATE, or DISCHARGE
// patient: the Patient involved (full record for ADMIT and DISCHARGE)
// oldAcuity / newAcuity: used only for UPDATE
class Action {
public:
    enum Type { ADMIT, UPDATE, DISCHARGE };
    Type type;
    Patient patient;
    int oldAcuity;
    int newAcuity;
};

class MedIndex {
private:

	AVLTree<Patient> index;
	deque<Patient> waiting;
	stack<Action> history;

public:

	void wait(int id, const string& name, int acuity) {
		Patient p(id, name, acuity);
		waiting.push_back(p);
	}

	void admit() {
		if (waiting.empty()) return;

		Patient p = waiting.front();

		if (index.search(p) != nullptr) return;

		waiting.pop_front();
		index.insert(p);

		Action a;
		a.type = Action::ADMIT;
		a.patient = p;
		history.push(a);
	}

	void find(int id) {
		Patient key(id, "", 0);
		Node<Patient>* result = index.search(key);

		if (result == nullptr) { 
			cout << "not found\n";
		} else { 
			cout << result->data.id << " "
				<< result->data.name << " "
				<< result->data.acuity << "\n";
		}
	}

	void update(int id, int newAcuity) {
		Patient key(id, "", 0);
		Node<Patient>* result = index.search(key);

		if (result == nullptr) return;

		int oldAcuity = result->data.acuity;
		result->data.acuity = newAcuity;

		Action a;
		a.type = Action::UPDATE;
		a.patient = result->data;
		a.oldAcuity = oldAcuity;
		a.newAcuity = newAcuity;
		history.push(a);
	}

	void discharge(int id) {
    Patient key(id, "", 0);
    Node<Patient>* result = index.search(key);

    if (result == nullptr) return;

    Patient p = result->data;
    index.remove(key);

    Action a;
    a.type = Action::DISCHARGE;
    a.patient = p;
    history.push(a);
}

	void undo() {
		if (history.empty()) return;

		Action a = history.top();
		history.pop();

		Patient key(a.patient.id, "", 0);

		if (a.type == Action::ADMIT) {
			index.remove(key);
			waiting.push_front(a.patient);
		}
		else if (a.type == Action::UPDATE) {
			Node<Patient>* result = index.search(key);

			if (result != nullptr) {
				result->data.acuity = a.oldAcuity;
			}
		}
		else if (a.type == Action::DISCHARGE) {
			index. insert(a.patient);
		}
	}

	void print () {
		index.printInorder();
	}

	void printWaiting() {
		if (waiting.empty()) {
			cout << "(none)\n";
			return;
		}

		for (const Patient& p : waiting) {
			cout << p.id << " " << p.name << " " 
				<< p.acuity << "\n";
		}
	}

	void printHistory() {
		if (history.empty()) {
			cout << "(none)\n";
			return;
		}

		stack<Action> temp = history;

		while (!temp.empty()) {
			Action a = temp.top();
			temp.pop();

			if (a.type == Action::ADMIT) {
				cout << "ADMIT " << a.patient.id << " " 
					<< a.patient.name << " "
					<< a.patient.acuity << "\n";
			}
			else if (a.type == Action::UPDATE) {
				cout << "UPDATE " << a.patient.id << " " 
					<< a.oldAcuity << "->"
					<< a.newAcuity << "\n";
			}
			else if (a.type == Action::DISCHARGE) {
				cout << "DISCHARGE " << a.patient.id << " " 
					<< a.patient.name << " "
					<< a.patient.acuity << "\n";
			}
		}
	}

	void height() {
		cout << index.treeHeight() << "\n";
	}	
};

int main() {
	MedIndex medIdx;
	
    string line;
	
    while (getline(cin, line)) {

		stringstream ss(line);
		string cmd;

		ss >> cmd;

		if (cmd == "WAIT") {
            int id, acuity;
            string name;

            ss >> id >> name >> acuity;
            medIdx.wait(id, name, acuity);
        }
        else if (cmd == "ADMIT") {
            medIdx.admit();
        }
        else if (cmd == "FIND") {
            int id;

            ss >> id;
            medIdx.find(id);
        }
        else if (cmd == "UPDATE") {
            int id, acuity;

            ss >> id >> acuity;
            medIdx.update(id, acuity);
        }
        else if (cmd == "DISCHARGE") {
            int id;

            ss >> id;
            medIdx.discharge(id);
        }
        else if (cmd == "UNDO") {
            medIdx.undo();
        }
        else if (cmd == "PRINT") {
            medIdx.print();
        }
        else if (cmd == "WAITING") {
            medIdx.printWaiting();
        }
        else if (cmd == "HISTORY") {
            medIdx.printHistory();
        }
        else if (cmd == "HEIGHT") {
            medIdx.height();
        }
    }
    return 0;
}
