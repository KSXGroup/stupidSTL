/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
/* This Map Does Not Use A Balanced Tree */
#include <functional>
#include <cstddef>
#include <iostream>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
	typedef pair<const Key, T> value_type;
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
private:
        struct MyTreeNode;
        MyTreeNode* root = nullptr;
        MyTreeNode* leftMost = nullptr;
        MyTreeNode* rightMost = nullptr;
        MyTreeNode* endNode = nullptr;
        Compare cmp;
        size_t sz = 0;

public:
	class const_iterator;
	class iterator {
            friend class map<Key, T, Compare>::const_iterator;
            friend class map<Key, T, Compare>;
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
                MyTreeNode* currentPos = nullptr;
                const map<Key, T, Compare>* mark = nullptr;
	public:
                iterator():currentPos(nullptr),mark(nullptr){}

		iterator(const iterator &other) {
                        currentPos = other.currentPos;
                        mark = other.mark;
                }
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		/**
		 * TODO iter++
		 */
                iterator operator++(int) {
                    iterator tmp(*this);
                    if(!currentPos || !currentPos->nodeData || !currentPos->next) throw invalid_iterator();
                    else{
                        currentPos = currentPos ->next;
                        return tmp;
                    }
                }
		/**
		 * TODO ++iter
		 */
                iterator & operator++() {
                    if(!currentPos || !currentPos->nodeData || !currentPos->next) throw invalid_iterator();
                    else{
                        currentPos = currentPos->next;
                        return *this;
                    }
                }
		/**
		 * TODO iter--
		 */
                iterator operator--(int) {
                    if(!currentPos) throw invalid_iterator();
                    if(!currentPos->prev) throw invalid_iterator();
                    iterator tmp(*this);
                    currentPos = currentPos -> prev;
                    return tmp;
                }
		/**
		 * TODO --iter
		 */
                iterator & operator--() {
                    if(!currentPos) throw invalid_iterator();
                    if(!(currentPos->prev)) throw invalid_iterator();
                    currentPos = currentPos ->prev;
                    return *this;
                }
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
                value_type & operator*() const {
                    if(!currentPos) throw invalid_iterator();
                    return *currentPos->nodeData;
                }
                bool operator==(const iterator &rhs) const {
                    return currentPos == rhs.currentPos && mark == rhs.mark;
                }
                bool operator==(const const_iterator &rhs) const {
                    return currentPos == rhs.currentPos && mark == rhs.mark;
                }
		/**
		 * some other operator for iterator.
		 */
                bool operator!=(const iterator &rhs) const {
                    return !(currentPos == rhs.currentPos && mark == rhs.mark);
                }
                bool operator!=(const const_iterator &rhs) const {
                    return !(currentPos == rhs.currentPos && mark == rhs.mark);
                }

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
                value_type* operator->() const noexcept {
                    return currentPos->nodeData;
                }
	};

	class const_iterator {
            friend class map<Key, T, Compare>::iterator;
            friend class map<Key, T, Compare>;
        private:
                const MyTreeNode* currentPos = nullptr;
                const map<Key, T, Compare>* mark = nullptr;
        public:
                const_iterator() = default;
                const_iterator(const iterator &other) {
                        currentPos = other.currentPos;
                        mark = other.mark;
                }
                const_iterator(const const_iterator &other) {
                        currentPos = other.currentPos;
                        mark = other.mark;
                }
                const_iterator operator++(int) {
                    if(!currentPos) throw invalid_iterator();
                    if(!currentPos->next) throw invalid_iterator();
                    const_iterator tmp(*this);
                    currentPos = currentPos ->next;
                    return tmp;
                }
                const_iterator & operator++() {
                    if(!currentPos || !currentPos->next || !currentPos -> nodeData) throw invalid_iterator();
                    else{
                        currentPos = currentPos ->next;
                        return *this;
                    }
                }
                const_iterator operator--(int) {
                    if(!currentPos) throw invalid_iterator();
                    if(!currentPos->prev) throw invalid_iterator();
                    const_iterator tmp(*this);
                    currentPos = currentPos ->prev;
                    return tmp;
                }
                const_iterator & operator--() {
                    if(!currentPos) throw invalid_iterator();
                    if(!currentPos->prev) throw invalid_iterator();
                    currentPos = currentPos ->prev;
                    return *this;
                }
                const value_type & operator*() const {
                    if(!currentPos || !currentPos->nodeData) throw invalid_iterator();
                    return *currentPos->nodeData;
                }
                bool operator==(const iterator &rhs) const {
                    return currentPos == rhs.currentPos && mark == rhs.mark;
                }
                bool operator==(const const_iterator &rhs) const {
                    return currentPos == rhs.currentPos && mark == rhs.mark;
                }
                bool operator!=(const iterator &rhs) const {
                    return !(currentPos == rhs.currentPos && mark == rhs.mark);
                }
                bool operator!=(const const_iterator &rhs) const {
                    return !(currentPos == rhs.currentPos && mark == rhs.mark);
                }
                const value_type* operator->() const noexcept {
                    return currentPos->nodeData;
                }
	};
private:
        struct MyTreeNode{
            value_type* nodeData = nullptr;
            MyTreeNode* left = nullptr;
            MyTreeNode* right = nullptr;
            MyTreeNode* prev = nullptr;
            MyTreeNode* next = nullptr;
            MyTreeNode() = default;
            MyTreeNode(MyTreeNode* l, MyTreeNode* r, const value_type& d): left(l),right(r){
                nodeData = new value_type(d);
            }
            ~MyTreeNode(){
                if(nodeData) delete nodeData;
                nodeData = nullptr;
                left = nullptr;
                right = nullptr;
                prev = nullptr;
                next = nullptr;
            }
        };
        inline bool isKeyEqual(const Key &a, const Key &b) const{
            return !(cmp(a,b) || cmp(b,a));
        }
        // (1 -> <), (0 -> >), (2 -> ==)
        inline int keyCompare(const Key &a ,const Key  &b) const{
            if(cmp(a,b)) return 1;
            if(cmp(b,a)) return 0;
            return 2;
        }

        MyTreeNode* treeFind(const Key k, MyTreeNode *t){
            if(!t) return nullptr;
            int cmpres = keyCompare(k, t->nodeData->first);
            if(cmpres == 2) return t;
            else if(cmpres) return treeFind(k, t->left);
            else return treeFind(k, t->right);
        }
        const MyTreeNode* ctreeFind(const Key k, const MyTreeNode *t) const{
            if(!t) return nullptr;
            int cmpres = keyCompare(k, t->nodeData->first);
            if(cmpres == 2) return t;
            else if(cmpres) return ctreeFind(k, t->left);
            else return ctreeFind(k, t->right);
        }

        //flag = 0 left, flag = 1 right
        MyTreeNode* treeInsert(const value_type &d, MyTreeNode *&t, MyTreeNode* from, bool flag){
            int cmpres = 0;
            if(!from && !t){
                t = new MyTreeNode(nullptr, nullptr, d);
                leftMost = t;
                rightMost = t;
                return t;
            }
            MyTreeNode* tmp = nullptr;
            if(!t){
                t = new MyTreeNode(nullptr, nullptr, d);
                if(!flag){
                    if(from == leftMost) leftMost = t;
                    t->next = from;
                    t->prev = from->prev;
                    if(from->prev) from->prev->next = t;
                    from->prev = t;
                }
                else{
                    if(from == rightMost) rightMost = t;
                    t->prev = from;
                    t->next = from->next;
                    if(from->next) from->next->prev = t;
                    from->next = t;
                }

                return t;
            }
            cmpres = keyCompare(d.first, t->nodeData->first);
            if(cmpres == 2) return t;
            else if(cmpres) {
                tmp = treeInsert(d, t->left, t, 0);
                return tmp;
            }
            else{
                tmp = treeInsert(d, t->right, t, 1);
                return tmp;
            }

        }
        bool treeRemove(const Key k, MyTreeNode *&t, MyTreeNode *from, bool flag){
            if(!t) return 0;
            int cmpres = keyCompare(k, t->nodeData->first);
            MyTreeNode *tmp = nullptr;
            //value_type *dtmp = nullptr;
            bool rv = 0;
            if(cmpres == 2){
                if(t->left && t->right){
                    if(!t->right->left){
                        MyTreeNode *itmp = t->left;
                        t->left = itmp->right;
                        itmp->right = t;
                        t = itmp;
                        return treeRemove(k, t->right, t, 1);
                    }
                    else if(!t->left->right){
                        MyTreeNode *itmp = t->right;
                        t->right = itmp->left;
                        itmp->left = t;
                        t = itmp;
                        return treeRemove(k, t->left, t, 0);
                    }
                    else{
                        MyTreeNode *itmp = t->right;
                        t->right = itmp->left;
                        itmp->left = t;
                        t = itmp;
                        return treeRemove(k, t->left, t, 0);
                    }
                }
                else{
                    tmp = t;
                    t = (t->left != nullptr ? t->left : t->right);
                    if(tmp == leftMost) leftMost = tmp->next;
                    if(tmp == rightMost) rightMost = tmp->prev;
                    if(tmp->next) tmp->next->prev = tmp->prev;
                    if(tmp->prev) tmp->prev->next = tmp->next;
                    tmp->left = tmp->right = tmp->prev = tmp->next = nullptr;
                    delete tmp;
                    return 1;
                }
            }
            else if(cmpres){
                rv = treeRemove(k, t->left, t, 0);
                return rv;
            }
            else{
                rv = treeRemove(k, t->right, t, 1);
                return rv;
            }
        }

        void traverse(MyTreeNode* t, MyTreeNode** arr, int &pos){
            if(t->left) traverse(t->left, arr, pos);
            arr[pos++] = t;
            if(t->right) traverse(t->right, arr, pos);
        }

        void deleteTree(MyTreeNode *&r){
            if(!r) return;
            deleteTree(r->left);
            deleteTree(r->right);
            delete r;
            r = nullptr;
        }

        void copyTreeWithRoot(MyTreeNode *&target, const MyTreeNode *source){
            if(!source) return;
            if(source->left){
                target->left = new MyTreeNode(nullptr, nullptr, *(source->left->nodeData));
                copyTreeWithRoot(target->left, source->left);
            }
            else target->left = nullptr;
            if(source->right){
                target->right = new MyTreeNode(nullptr, nullptr, *(source->right->nodeData));
                copyTreeWithRoot(target->right, source->right);
            }
            else target->right = nullptr;
            return;
        }
        void copyTree(MyTreeNode *&target, const MyTreeNode *source, size_t tsz){
            if(!source) return;
            int pos = 0;
            if(target){
                deleteTree(target);
                target = nullptr;
            }
            target = new MyTreeNode(nullptr, nullptr, *source->nodeData);
            MyTreeNode **arr = new MyTreeNode*[tsz];
            copyTreeWithRoot(target, source);
            traverse(target, arr, pos);
            arr[0]->prev = nullptr;arr[0]->next = arr[1];
            arr[tsz - 1]->next = nullptr; arr[tsz - 1]->prev = arr[tsz - 2];
            for(size_t i = 1; i <= tsz - 2; ++i){
                arr[i]->next = arr[i + 1];
                arr[i]->prev = arr[i - 1];
            }
            leftMost = arr[0]; rightMost = arr[tsz - 1];
            delete [] arr;
            arr = nullptr;
        }
        /**
         * TODO two constructors
         */
public:
        map() {
            root = nullptr;
            leftMost = nullptr;
            rightMost = nullptr;
            endNode = new MyTreeNode();
            //endNode->prev = nullptr;
        }
        map(const map &other) {
            cmp = other.cmp;
            sz = other.sz;
            if(root) deleteTree(root);
            if(endNode) delete endNode;
            copyTree(root, other.root, sz);
            endNode = new MyTreeNode();
            endNode->prev = rightMost;
            if(rightMost) rightMost->next = endNode;
        }
	/**
	 * TODO assignment operator
	 */
        map & operator=(const map &other){
            if(root == other.root) return *this;
            if(root) deleteTree(root);
            if(endNode) delete endNode;
            cmp = other.cmp;
            sz = other.sz;
            copyTree(root, other.root, sz);
            endNode =  new MyTreeNode();
            endNode->prev = rightMost;
            if(rightMost) rightMost->next = endNode;
            return *this;
        }
	/**
	 * TODO Destructors
	 */
        ~map() {
            leftMost = rightMost = nullptr;
            if(root) deleteTree(root);
            if(endNode) delete endNode;
            endNode = nullptr;
        }
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
        T & at(const Key &key) {
            MyTreeNode *p = treeFind(key, root);
            if(!p) throw index_out_of_bound();
            else return p->nodeData->second;
        }
        const T & at(const Key &key) const {
            const MyTreeNode *p = ctreeFind(key,root);
            if(!p) throw index_out_of_bound();
            else return p->nodeData->second;
        }
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
        T & operator[](const Key &key) {
            const MyTreeNode *ctmp = ctreeFind(key,root);
            MyTreeNode *tmp = treeInsert(value_type(key, T()), root, nullptr, 0);
            if(!ctmp){
                sz++;
                endNode->prev = rightMost;
                if(rightMost) rightMost->next = endNode;
            }
            return tmp->nodeData->second;
        }
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
        const T & operator[](const Key &key) const {
            const MyTreeNode *tmp = ctreeFind(key, root);
            if(!tmp) throw index_out_of_bound();
            else return tmp->nodeData->second;
        }
	/**
	 * return a iterator to the beginning
	 */
        iterator begin() {
            iterator tmp;
            if(!root) tmp.currentPos = endNode;
            else tmp.currentPos = leftMost;
            tmp.mark = this;
            return tmp;
        }
        const_iterator cbegin() const {
            const_iterator tmp;
            if(!root) tmp.currentPos = endNode;
            else tmp.currentPos = leftMost;
            tmp.mark = this;
            return tmp;
        }
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
        iterator end() {
            iterator tmp;
            tmp.currentPos = endNode;
            tmp.mark = this;
            return tmp;
        }
        const_iterator cend() const {
            const_iterator tmp;
            tmp.currentPos = endNode;
            tmp.mark = this;
            return tmp;
        }
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
        bool empty() const {
            if(!root) return 1;
            else return 0;
        }
	/**
	 * returns the number of elements.
	 */
        size_t size() const {
            return sz;
        }
	/**
	 * clears the contents
	 */
        void clear() {
            deleteTree(root);
            endNode->prev = nullptr;
            leftMost = rightMost = nullptr;
            sz = 0;
            return;
        }
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
        pair<iterator, bool> insert(const value_type &value) {
            MyTreeNode* p = nullptr;
            iterator tmp;
            tmp.mark = this;
            p = treeFind(value.first, root);
            if(p){
                tmp.currentPos = p;
                return pair<iterator,bool>(tmp, 0);
            }
            else{
                sz++;
                p= treeInsert(value,root,nullptr, 0);
                tmp.currentPos = p;
                endNode->prev = rightMost;
                if(rightMost) rightMost->next = endNode;
                return pair<iterator, bool>(tmp, 1);
            }
        }
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
        void erase(iterator pos) {
            if(pos.currentPos == nullptr || pos.currentPos == endNode || pos.mark != this) throw invalid_iterator();
            else{
                if(treeRemove(pos.currentPos->nodeData->first, root,nullptr,0)){
                    sz--;
                    endNode->prev = rightMost;
                    if(rightMost) rightMost->next = endNode;
                }
            }
        }
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
        size_t count(const Key &key) const {
            if(ctreeFind(key,root)) return 1;
            else return 0;
        }
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
        iterator find(const Key &key) {
            MyTreeNode *p = treeFind(key,root);
            iterator tmp;
            tmp.mark = this;
            if(p){
                tmp.currentPos = p;
                return tmp;
            }
            else{
                tmp.currentPos = endNode;
                return tmp;
            }
        }
        const_iterator find(const Key &key) const {
            const MyTreeNode *p = ctreeFind(key,root);
            const_iterator tmp;
            tmp.mark = this;
            if(p){
                tmp.currentPos = p;
                return tmp;
            }
            else{
                tmp.currentPos = endNode;
                return tmp;
            }
        }
};

}

#endif
