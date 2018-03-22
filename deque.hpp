#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cstddef>
#include <cmath>
#include <iostream>
#include <assert.h>

namespace sjtu { 
const int MAX_SIZE = 2000;
template<class T>
class deque {
    struct Block{
        Block *prev = nullptr;
        Block *next = nullptr;
        size_t blockSize = 0;
        T *data[MAX_SIZE];
        Block(){
            for(int i = 0; i < MAX_SIZE; ++i) data[i] = nullptr;
        }
        Block(Block *p, Block *n, size_t s):prev(p), next(n), blockSize(s){
            for(int i = 0; i < MAX_SIZE; ++i) data[i] = nullptr;
        }
        ~Block(){
            blockSize = 0;
            prev = nullptr;
            next = nullptr;
            for(size_t i = 0; i < MAX_SIZE; ++i){
                if(data[i]) delete data[i];
                data[i] = nullptr;
            }
        }
    };
    public:
        int insertNode(const T &d ,Block *p, size_t pos){
            if(pos < 0 || !p || pos > p->blockSize) return 0;
            sz++;
            if(p == afterEnd && p == start){
                p = addBlockBefore(p);
                start = p;
                pos = 0;
                p->data[0] = new T(d);
                p->blockSize++;
                return 0;
            }
            else if(p->blockSize < MAX_SIZE){
                if(pos > p->blockSize) return 0;
                else{
                    for(size_t i = p->blockSize - 1; i >= pos; --i){
                        if(i  >= p->blockSize) break;
                        p->data[i + 1] = p->data[i];
                    }
                    p->data[pos] = new T(d);
                    ++p->blockSize;
                    return pos;
                }
            }
            else if(p->blockSize == MAX_SIZE){
                Block *tmpB = addBlockAfter(p);
                tmpB->blockSize = p->blockSize / 2;
                for(size_t i = 0; i < tmpB->blockSize; ++i){
                    tmpB->data[i] = p->data[i + tmpB->blockSize];
                    p->data[i + tmpB->blockSize] = nullptr;
                }
                p->blockSize /= 2;
                if(pos + 1 <= tmpB->blockSize){
                    for(size_t i = tmpB->blockSize - 1; i >= pos; --i){
                        if(i >= tmpB->blockSize) break;
                        p->data[i + 1] = p->data[i];
                    }
                    p->data[pos] = new T(d);
                    p->blockSize++;
                    return pos;
                }
                else{
                    p = tmpB;
                    for(size_t i = tmpB->blockSize - 1; i >= pos - tmpB->blockSize; --i){
                        if(i >= tmpB->blockSize) break;
                        p->data[i + 1] = p->data[i];
                    }
                    p->data[pos - tmpB->blockSize] = new T(d);
                    p->blockSize++;
                    return pos - tmpB->blockSize;
                }
            }
            return -1;
        }
        bool removeNode(Block *p, size_t pos){
            if(sz <= 0 || pos < 0 || pos >= p->blockSize ) return 0;
            else{
                sz--;
                delete p->data[pos];
                for(size_t i = pos; i < p->blockSize - 1; ++i) p->data[i] = p->data[i + 1];
                p->data[p->blockSize - 1] = nullptr;
                p->blockSize--;
                if(!p->blockSize){
                    removeBlock(p);
                    p = nullptr;
                    pos = 0;
                    return 1;
                }
                return 1;
            }
        }
        Block *addBlockBefore(Block *p){
            Block *tmp = new Block();
            if(p == start) start = tmp;
            tmp->next = p;
            tmp->prev = p->prev;
            if(p->prev) p->prev->next = tmp;
            p->prev = tmp;
            return tmp;
        }
        Block *addBlockAfter(Block *p){
            if(p == afterEnd){
                //std::cerr << "insert after end!" << std::endl;
            }
            Block *tmp = new Block();
            tmp->prev = p;
            tmp->next = p->next;
            if(p->next) p->next->prev = tmp;
            p->next = tmp;
            return tmp;
        }
        bool removeBlock(Block *p){
            if(p == afterEnd){
               // std::cerr << "remove end node !" << std::endl;
                return 0;
            }
           if(p == start) start = p->next;
           if(p->next) p->next->prev = p->prev;
           if(p->prev) p->prev->next = p->next;
           delete p;
           p = nullptr;
           return 1;
        }

        void copyBlockList(const Block *source){
            if(!source) return;
            if(start) this->~deque();
            start = new Block();
            Block* tmp = start;
            for(size_t i = 0; i < source->blockSize; ++i) tmp->data[i] = new T(*source->data[i]);
            tmp->blockSize = source->blockSize;
            while(source->next){
                tmp->next = new Block();
                tmp->next->prev = tmp;
                for(size_t i = 0; i < source->next->blockSize; ++i) tmp->next->data[i] = new T(*source->next->data[i]);
                tmp->next->blockSize = source->next->blockSize;
                tmp = tmp->next;
                source = source->next;
            }
            tmp->next = nullptr;
            afterEnd = tmp;
            return;
        }

        void clearBlockList(){
            Block *tmp = start, *tmp1 = start->next;
            while(1){
                delete tmp;
                tmp = tmp1;
                if(tmp1) tmp1 = tmp->next;
                else break;
            }
            if(tmp) delete tmp;
            tmp = tmp1 = nullptr;
            start = afterEnd = new Block();
            sz = 0;
        }




public:
	class const_iterator;
        class iterator {
            friend class deque<T>;
            friend class deque<T>::const_iterator;
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
                deque<T> *dq = nullptr;
                Block *p = nullptr;
                size_t index = 0;
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */

                iterator() = default;
                iterator(deque<T> *p, Block *b, size_t idx):dq(p), p(b), index(idx){}
                iterator(const iterator &it):dq(it.dq), p(it.p), index(it.index){}

                iterator operator+(const int &n) const {
                        if(n < 0) return operator- (-n);
                        else if(n == 0) return *this;
                        else{
                           size_t offset = n;
                           Block *tmp = p;
                           if(tmp->blockSize - index > offset) return iterator(dq, tmp, index + offset);
                           else{
                               offset -= (tmp->blockSize - index);
                               tmp = tmp->next;
                               while(tmp != dq->afterEnd && tmp->blockSize <= offset){
                                   offset -= tmp->blockSize;
                                   tmp = tmp->next;
                               }
                               if(tmp == dq->afterEnd){
                                   return iterator(dq, tmp, 0);
                               }
                               else{
                                   return iterator(dq, tmp, offset);
                               }
                           }
                        }
		}
		iterator operator-(const int &n) const {
                        if(n < 0) return operator+ (-n);
                        else if(n == 0) return *this;
                        else{
                            size_t offset = n;
                            Block *tmp = p;
                            if(offset <= index) return iterator(dq, p, index - offset);
                            else{
                                offset -= index + 1;
                                tmp = tmp->prev;
                                while(tmp != nullptr && tmp->blockSize <= offset){
                                    offset -= tmp->blockSize;
                                    tmp = tmp->prev;
                                }
                                if(!tmp) return iterator(dq, nullptr, 0);
                                else return iterator(dq, tmp, tmp->blockSize - 1 - offset);
                            }
                        }
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
                    if(dq != rhs.dq) throw invalid_iterator();
                    int tst1 = 0, tst2 = 0;
                        tst1 += index;
                        Block *tmp = p;
                        if(tmp->prev){
                            tmp = tmp->prev;
                            while(tmp){
                               tst1 += tmp->blockSize;
                               tmp = tmp->prev;
                            }
                        }
                        tst2 += rhs.index;
                        tmp = rhs.p;
                        if(tmp->prev){
                            tmp = tmp->prev;
                            while(tmp){
                               tst2 += tmp->blockSize;
                               tmp = tmp->prev;
                            }
                        }
                        return tst1 - tst2;

		}
		iterator operator+=(const int &n) {
                    if(n < 0) return operator-= (-n);
                    if(n == 0) return *this;
                    else{
                       size_t offset = n;
                       Block *tmp = p;
                       if(tmp->blockSize - index > offset){
                           index += offset;
                           return *this;
                       }
                       else{
                           offset -= (tmp->blockSize - index);
                           tmp = tmp->next;
                           while(tmp != dq->afterEnd && tmp->blockSize <= offset){
                               offset -= tmp->blockSize;
                               tmp = tmp->next;
                           }
                           if(tmp == dq->afterEnd){
                               p = dq->afterEnd;
                               index = 0;
                               return *this;
                           }
                           else{
                               p = tmp;
                               index = offset;
                               return *this;
                           }
                       }
                    }
                }
		iterator operator-=(const int &n) {
                    if(n < 0) return operator+= (-n);
                    else if(n == 0) return *this;
                    else{
                        Block *tmp = p;
                        size_t offset = n;
                        if(offset <= index) {
                            index -= offset;
                            return *this;
                        }
                        else{
                            offset -= index + 1;
                            tmp = tmp->prev;
                            while(tmp != nullptr && tmp->blockSize <= offset){
                                offset -= tmp->blockSize;
                                tmp = tmp->prev;
                            }
                            if(!tmp){
                                p = nullptr;
                                index = 0;
                                return *this;
                            }
                            else{
                                p = tmp;
                                index = tmp->blockSize - 1 - offset;
                                return *this;
                            }
                        }
                    }
		}
                iterator operator++(int) {
                    if(p == dq->afterEnd) throw invalid_iterator();
                    iterator tmp(*this);
                    if(index + 1 <= p->blockSize - 1) index++;
                    else{
                        p = p->next;
                        index = 0;
                    }
                    return tmp;
                }
                iterator& operator++() {
                    if(p == dq->afterEnd) throw invalid_iterator();
                    if(index + 1 <= p->blockSize - 1) index++;
                    else{
                        p = p->next;
                        index = 0;
                    }
                    return *this;
                }
                iterator operator--(int) {
                    iterator tmp(*this);
                    if(p == dq->start && index == 0) throw invalid_iterator();
                    else{
                        if(p == dq->afterEnd){
                            p = p->prev;
                            index = p->blockSize - 1;
                        }
                        else if(index) --index;
                        else{
                            p = p->prev;
                            index = p->blockSize - 1;
                        }
                    }
                    return tmp;
                }
                iterator& operator--() {
                    if(p == dq->start && index == 0) throw invalid_iterator();
                    else{
                        if(p == dq->afterEnd){
                            p = p->prev;
                            index = p->blockSize - 1;
                        }
                        else if(index) --index;
                        else{
                            p = p->prev;
                            index = p->blockSize - 1;
                        }
                    }
                    return *this;
                }
                T& operator*() const {
                    if(p == dq->afterEnd) throw invalid_iterator();
                    return *(p->data[index]);
                }
                T* operator->() const noexcept {
                    return p->data[index];
                }
                bool operator==(const iterator &rhs) const {
                    if(rhs.dq == dq){
                        if(rhs.p == dq->afterEnd && p == dq->afterEnd) return 1;
                    }
                    if(rhs.dq != dq || rhs.p != p || rhs.index != index) return 0;
                    else return 1;
                }
                bool operator==(const const_iterator &rhs) const {
                    if(rhs.dq == dq){
                        if(rhs.p == dq->afterEnd && p == dq.afterEnd) return 1;
                    }
                    if(rhs.dq != dq || rhs.p != p || rhs.index != index) return 0;
                    else return 1;
                }

                bool operator!=(const iterator &rhs) const {
                   return !operator==(rhs);
                }
                bool operator!=(const const_iterator &rhs) const {
                    return !operator==(rhs);
                }
	};
	class const_iterator {
            friend class deque<T>;
            friend class deque<T>::iterator;
        private:
                const deque<T> *dq = nullptr;
                const Block *p = nullptr;
                size_t index = 0;
        public:
                const_iterator() = default;
                const_iterator(const deque<T> *p, const Block *b, size_t idx):dq(p), p(b), index(idx){}
                const_iterator(const iterator &it):dq(it.dq), p(it.p), index(it.index){}
                const_iterator(const const_iterator &it):dq(it.dq), p(it.p), index(it.index){}

                const_iterator operator+(const int &n) const {
                        if(n < 0) return operator- (-n);
                        else if(n == 0) return *this;
                        else{
                           size_t offset = n;
                           const Block *tmp = p;
                           if(tmp->blockSize - index > offset) return const_iterator(dq, tmp, index + offset);
                           else{
                               offset -= (tmp->blockSize - index);
                               tmp = tmp->next;
                               while(tmp != dq->afterEnd && tmp->blockSize <= offset){
                                   offset -= tmp->blockSize;
                                   tmp = tmp->next;
                               }
                               if(tmp == dq->afterEnd){
                                   return const_iterator(dq, tmp, 0);
                               }
                               else{
                                   return const_iterator(dq, tmp, offset);
                               }
                           }
                        }
                }
                const_iterator operator-(const int &n) const {
                        if(n < 0) return operator+ (-n);
                        else if(n == 0) return *this;
                        else{
                            size_t offset = n;
                            const Block *tmp = p;
                            if(offset <= index) return const_iterator(dq, p, index - offset);
                            else{
                                offset -= index + 1;
                                tmp = tmp->prev;
                                while(tmp != nullptr && tmp->blockSize <= offset){
                                    offset -= tmp->blockSize;
                                    tmp = tmp->prev;
                                }
                                if(!tmp) return const_iterator(dq, nullptr, 0);
                                else return const_iterator(dq, tmp, tmp->blockSize - 1 - offset);
                            }
                        }
                }
                // return th distance between two iterator,
                // if these two iterators points to different vectors, throw invaild_iterator.
                int operator-(const const_iterator &rhs) const {
                    if(dq != rhs.dq) throw invalid_iterator();
                    int tst1 = 0, tst2 = 0;
                        tst1 += index;
                        const Block *tmp = p;
                        if(tmp->prev){
                            tmp = tmp->prev;
                            while(tmp){
                               tst1 += tmp->blockSize;
                               tmp = tmp->prev;
                            }
                        }
                        tst2 += rhs.index;
                        tmp = rhs.p;
                        if(tmp->prev){
                            tmp = tmp->prev;
                            while(tmp){
                               tst2 += tmp->blockSize;
                               tmp = tmp->prev;
                            }
                        }
                        return tst1 - tst2;

                }
                const_iterator operator+=(const int &n) {
                    if(n < 0) return operator-= (-n);
                    if(n == 0) return *this;
                    else{
                       size_t offset = n;
                       const Block *tmp = p;
                       if(tmp->blockSize - index > offset){
                           index += offset;
                           return *this;
                       }
                       else{
                           offset -= (tmp->blockSize - index);
                           tmp = tmp->next;
                           while(tmp != dq->afterEnd && tmp->blockSize <= offset){
                               offset -= tmp->blockSize;
                               tmp = tmp->next;
                           }
                           if(tmp == dq->afterEnd){
                               p = dq->afterEnd;
                               index = 0;
                               return *this;
                           }
                           else{
                               p = tmp;
                               index = offset;
                               return *this;
                           }
                       }
                    }
                }
                const_iterator operator-=(const int &n) {
                    if(n < 0) return operator+= (-n);
                    else if(n == 0) return *this;
                    else{
                        const Block *tmp = p;
                        size_t offset = n;
                        if(offset <= index) {
                            index -= offset;
                            return *this;
                        }
                        else{
                            offset -= index + 1;
                            tmp = tmp->prev;
                            while(tmp != nullptr && tmp->blockSize <= offset){
                                offset -= tmp->blockSize;
                                tmp = tmp->prev;
                            }
                            if(!tmp){
                                p = nullptr;
                                index = 0;
                                return *this;
                            }
                            else{
                                p = tmp;
                                index = tmp->blockSize - 1 - offset;
                                return *this;
                            }
                        }
                    }
                }
                const_iterator operator++(int) {
                    if(p == dq->afterEnd) throw invalid_iterator();
                    const_iterator tmp(*this);
                    if(index + 1 <= p->blockSize - 1) index++;
                    else{
                        p = p->next;
                        index = 0;
                    }
                    return tmp;
                }
                const_iterator& operator++() {
                    if(p == dq->afterEnd) throw invalid_iterator();
                    if(index + 1 <= p->blockSize - 1) index++;
                    else{
                        p = p->next;
                        index = 0;
                    }
                    return *this;
                }
                const_iterator operator--(int) {
                    iterator tmp(*this);
                    if(p == dq->start && index == 0) throw invalid_iterator();
                    else{
                        if(p == dq->afterEnd){
                            p = p->prev;
                            index = p->blockSize - 1;
                        }
                        else if(index) --index;
                        else{
                            p = p->prev;
                            index = p->blockSize - 1;
                        }
                    }
                    return tmp;
                }
                const_iterator& operator--() {
                    if(p == dq->start && index == 0) throw invalid_iterator();
                    else{
                        if(p == dq->afterEnd){
                            p = p->prev;
                            index = p->blockSize - 1;
                        }
                        else if(index) --index;
                        else{
                            p = p->prev;
                            index = p->blockSize - 1;
                        }
                    }
                    return *this;
                }
                T& operator*() const {
                    if(p == dq->afterEnd) throw invalid_iterator();
                    return *(p->data[index]);
                }
                T* operator->() const noexcept {
                    return p->data[index];
                }
                bool operator==(const const_iterator &rhs) const {
                    if(rhs.dq == dq){
                        if(rhs.p == dq->afterEnd && p == dq->afterEnd) return 1;
                    }
                    if(rhs.dq != dq || rhs.p != p || rhs.index != index) return 0;
                    else return 1;
                }
                bool operator==(const iterator &rhs) const {
                    if(rhs.dq == dq){
                        if(rhs.p == dq->afterEnd && p == dq.afterEnd) return 1;
                    }
                    if(rhs.dq != dq || rhs.p != p || rhs.index != index) return 0;
                    else return 1;
                }

                bool operator!=(const iterator &rhs) const {
                   return !operator==(rhs);
                }
                bool operator!=(const const_iterator &rhs) const {
                    return !operator==(rhs);
                }

	};

private:
        Block* start = nullptr;
        Block* afterEnd = nullptr;
        size_t sz = 0;

public:
        deque() {
            start = afterEnd = new Block();
            sz = 0;
        }
        deque(const deque &other) {
            copyBlockList(other.start);
            sz = other.sz;
        }
        ~deque() {
            Block *tmp = start, *tmp1 = start->next;
            while(1){
                delete tmp;
                tmp = tmp1;
                if(tmp1) tmp1 = tmp->next;
                else break;
            }
            if(tmp) delete tmp;
            tmp = tmp1 = nullptr;
            start = afterEnd = nullptr;
        }
	/**
	 * TODO assignment operator
	 */

        void print(){
            Block *tmp = start;
            while(tmp != afterEnd){
                for(size_t i = 0; i < tmp->blockSize; ++i) std::cout << *(tmp->data[i]) << " ";
                std::cout << "END OF BLOCK OF SIZE " << tmp->blockSize << std::endl;
                tmp = tmp->next;
            }
            std::cout << "END OF LIST" << std::endl;
        }

        deque &operator=(const deque &other) {
            if(this == &other) return *this;
            else{
                copyBlockList(other.start);
                sz = other.sz;
                return *this;
            }
        }
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
        T & at(const size_t &pos) {
            if(sz == 0 || pos < 0 || pos >= sz) throw index_out_of_bound();
            else{
                size_t p = pos;
                Block *tmp = start;
                while(p > tmp->blockSize - 1){
                    p -= tmp->blockSize;
                    tmp = tmp->next;
                }
                return *(tmp->data[p]);
            }
        }
        const T & at(const size_t &pos) const {
            if(sz == 0 || pos < 0 || pos >= sz) throw index_out_of_bound();
            else{
                size_t p = pos;
                const Block *tmp = start;
                while(p > tmp->blockSize - 1){
                    p -= tmp->blockSize;
                    tmp = tmp->next;
                }
                return *(tmp->data[p]);
            }
        }
        T & operator[](const size_t &pos) {
            if(sz == 0 || pos < 0 || pos >= sz) throw index_out_of_bound();
            else{
                size_t p = pos;
                Block *tmp = start;
                while(p > tmp->blockSize - 1){
                    p -= tmp->blockSize;
                    tmp = tmp->next;
                }
                return *(tmp->data[p]);
            }
        }
        const T & operator[](const size_t &pos) const {
            if(sz == 0 || pos < 0 || pos >= sz) throw index_out_of_bound();
            else{
                size_t p = pos;
                const Block *tmp = start;
                while(p > tmp->blockSize - 1){
                    p -= tmp->blockSize;
                    tmp = tmp->next;
                }
                return *(tmp->data[p]);
            }
        }
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
        const T & front() const {
            if(sz == 0) throw container_is_empty();
            else return *(start->data[0]);
        }
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
        const T & back() const {
            if(sz == 0) throw container_is_empty();
            else return *(afterEnd->prev->data[afterEnd->prev->blockSize - 1]);
        }
	/**
	 * returns an iterator to the beginning.
	 */
        iterator begin(){
            return iterator(this, start, 0);
        }
        const_iterator cbegin() const {
            return const_iterator(this, start, 0);
        }
	/**
	 * returns an iterator to the end.
	 */
        iterator end() {
            return iterator(this, afterEnd, 0);
        }
        const_iterator cend() const {
            return const_iterator(this, afterEnd, 0);
        }
	/**
	 * checks whether the container is empty.
	 */
        inline bool empty() const {
            return !sz;
        }
	/**
	 * returns the number of elements
	 */
        inline size_t size() const {
            return sz;
        }
	/**
	 * clears the contents
	 */
        void clear() {
            clearBlockList();
        }
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
        iterator insert(iterator pos, const T &value) {
            if(pos.dq != this || pos.p == nullptr) throw invalid_iterator();
            if(pos.p == afterEnd){
                if(afterEnd->prev){
                    insertNode(value, afterEnd->prev, afterEnd->prev->blockSize);
                    return iterator(this, afterEnd->prev, afterEnd->prev->blockSize - 1);
                }
                else{
                    insertNode(value, afterEnd, pos.index);
                    return iterator(this, afterEnd->prev, afterEnd->prev->blockSize - 1);
                }
            }
            else{
                if(pos.p->blockSize == MAX_SIZE){
                    if(pos.index < MAX_SIZE / 2){
                        insertNode(value, pos.p, pos.index);
                        return iterator(this, pos.p, pos.index);
                    }
                    else{
                        insertNode(value, pos.p, pos.index);
                        return iterator(this, pos.p->next, pos.index - MAX_SIZE / 2);
                    }
                }
                else{
                    insertNode(value, pos.p, pos.index);
                    return iterator(this, pos.p, pos.index);
                }
            }

        }
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
        iterator erase(iterator pos) {
            if(pos.dq != this || pos.p == afterEnd || pos.p == nullptr || sz == 0) throw invalid_iterator();
            if(pos.index == pos.p->blockSize - 1){
                Block *tmp = pos.p->next;
                removeNode(pos.p, pos.index);
                return iterator(this, tmp, 0);
            }
            else{
                removeNode(pos.p, pos.index);
                return iterator(this, pos.p, pos.index);
            }

        }
	/**
	 * adds an element to the end
	 */
        void push_back(const T &value) {
            size_t pos = 0;
            if(afterEnd->prev){
                insertNode(value, afterEnd->prev, afterEnd->prev->blockSize);
            }
            else {
                pos = 0;
                insertNode(value, afterEnd, pos);
            }
        }
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
        void pop_back() {
            if(sz <= 0) throw container_is_empty();
            else removeNode(afterEnd->prev, afterEnd->prev->blockSize - 1);
        }
        /**
	 * inserts an element to the beginning.
	 */
        void push_front(const   T &value) {
            insertNode(value, start, 0);
        }
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
        void pop_front() {
            if(sz <= 0) throw container_is_empty();
            else removeNode(start , 0);
        }
};

}

#endif
