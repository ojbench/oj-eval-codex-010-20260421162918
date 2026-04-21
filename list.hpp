#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    struct node {
        node *prev;
        node *next;
        bool is_sentinel;
        explicit node(bool sentinel): prev(this), next(this), is_sentinel(sentinel) {}
        virtual ~node() = default;
    };
    struct val_node : public node {
        T data;
        explicit val_node(const T &v): node(false), data(v) {}
    };

protected:
    /**
     * add data members for linked list as protected members
     */
    node *head; // sentinel node
    size_t n;

    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        // insert cur before pos
        cur->next = pos;
        cur->prev = pos->prev;
        pos->prev->next = cur;
        pos->prev = cur;
        return cur;
    }
    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        // unlink pos from list, but not delete it
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        pos->prev = pos->next = pos; // isolate
        return pos;
    }

public:
    class const_iterator;
    class iterator {
    private:
        const list<T> *owner;
        node *cur;

    public:
        iterator(): owner(nullptr), cur(nullptr) {}
        iterator(const list<T> *o, node *c): owner(o), cur(c) {}
        friend class list<T>;
        friend class const_iterator;
        /**
         * iter++
         */
        iterator operator++(int) {
            if (owner == nullptr || cur == nullptr || cur == owner->head) throw invalid_iterator();
            iterator tmp = *this;
            cur = cur->next;
            return tmp;
        }
        /**
         * ++iter
         */
        iterator & operator++() {
            if (owner == nullptr || cur == nullptr || cur == owner->head) throw invalid_iterator();
            cur = cur->next;
            return *this;
        }
        /**
         * iter--
         */
        iterator operator--(int) {
            if (owner == nullptr || cur == nullptr) throw invalid_iterator();
            // decrementing end() is valid only if list not empty
            if (cur == owner->head) {
                if (owner->empty()) throw invalid_iterator();
                iterator tmp = *this;
                cur = cur->prev; // move to last element
                return tmp;
            }
            iterator tmp = *this;
            cur = cur->prev;
            return tmp;
        }
        /**
         * --iter
         */
        iterator & operator--() {
            if (owner == nullptr || cur == nullptr) throw invalid_iterator();
            if (cur == owner->head) {
                if (owner->empty()) throw invalid_iterator();
                cur = cur->prev; // move to last element
                return *this;
            }
            cur = cur->prev;
            return *this;
        }
        /**
         * TODO *it
         * remember to throw if iterator is invalid
         */
        T & operator *() const {
            if (owner == nullptr || cur == nullptr || cur->is_sentinel) throw invalid_iterator();
            return static_cast<val_node *>(cur)->data;
        }
        /**
         * TODO it->field
         * remember to throw if iterator is invalid
         */
        T * operator ->() const {
            if (owner == nullptr || cur == nullptr || cur->is_sentinel) throw invalid_iterator();
            return &(static_cast<val_node *>(cur)->data);
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const { return owner == rhs.owner && cur == rhs.cur; }
        bool operator==(const const_iterator &rhs) const { return owner == rhs.owner && cur == rhs.cur; }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
        bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
    };
    /**
     * TODO
     * has same function as iterator, just for a const object.
     * should be able to construct from an iterator.
     */
    class const_iterator {
    private:
        const list<T> *owner;
        const node *cur;
    public:
        const_iterator(): owner(nullptr), cur(nullptr) {}
        const_iterator(const list<T> *o, const node *c): owner(o), cur(c) {}
        const_iterator(const iterator &it): owner(it.owner), cur(it.cur) {}
        friend class list<T>;
        friend class iterator;

        const_iterator operator++(int) {
            if (owner == nullptr || cur == nullptr || cur == owner->head) throw invalid_iterator();
            const_iterator tmp = *this;
            cur = cur->next;
            return tmp;
        }
        const_iterator & operator++() {
            if (owner == nullptr || cur == nullptr || cur == owner->head) throw invalid_iterator();
            cur = cur->next;
            return *this;
        }
        const_iterator operator--(int) {
            if (owner == nullptr || cur == nullptr) throw invalid_iterator();
            if (cur == owner->head) {
                if (owner->empty()) throw invalid_iterator();
                const_iterator tmp = *this;
                cur = cur->prev;
                return tmp;
            }
            const_iterator tmp = *this;
            cur = cur->prev;
            return tmp;
        }
        const_iterator & operator--() {
            if (owner == nullptr || cur == nullptr) throw invalid_iterator();
            if (cur == owner->head) {
                if (owner->empty()) throw invalid_iterator();
                cur = cur->prev;
                return *this;
            }
            cur = cur->prev;
            return *this;
        }
        const T & operator *() const {
            if (owner == nullptr || cur == nullptr || cur->is_sentinel) throw invalid_iterator();
            return static_cast<const val_node *>(cur)->data;
        }
        const T * operator ->() const {
            if (owner == nullptr || cur == nullptr || cur->is_sentinel) throw invalid_iterator();
            return &(static_cast<const val_node *>(cur)->data);
        }
        bool operator==(const const_iterator &rhs) const { return owner == rhs.owner && cur == rhs.cur; }
        bool operator==(const iterator &rhs) const { return owner == rhs.owner && cur == rhs.cur; }
        bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
        bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
    };
    /**
     * TODO Constructs
     * Atleast two: default constructor, copy constructor
     */
    list() {
        head = new node(true);
        head->prev = head->next = head;
        n = 0;
    }
    list(const list &other) {
        head = new node(true);
        head->prev = head->next = head;
        n = 0;
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
    }
    /**
     * TODO Destructor
     */
    virtual ~list() {
        clear();
        delete head;
        head = nullptr;
    }
    /**
     * TODO Assignment operator
     */
    list &operator=(const list &other) {
        if (this == &other) return *this;
        clear();
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) push_back(*it);
        return *this;
    }
    /**
     * access the first / last element
     * throw container_is_empty when the container is empty.
     */
    const T & front() const {
        if (empty()) throw container_is_empty();
        return static_cast<const val_node *>(head->next)->data;
    }
    const T & back() const {
        if (empty()) throw container_is_empty();
        return static_cast<const val_node *>(head->prev)->data;
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() { return iterator(this, head->next); }
    const_iterator cbegin() const { return const_iterator(this, head->next); }
    /**
     * returns an iterator to the end.
     */
    iterator end() { return iterator(this, head); }
    const_iterator cend() const { return const_iterator(this, head); }
    /**
     * checks whether the container is empty.
     */
    virtual bool empty() const { return n == 0; }
    /**
     * returns the number of elements
     */
    virtual size_t size() const { return n; }

    /**
     * clears the contents
     */
    virtual void clear() {
        node *p = head->next;
        while (p != head) {
            node *nx = p->next;
            delete static_cast<val_node *>(p);
            p = nx;
        }
        head->next = head->prev = head;
        n = 0;
    }
    /**
     * insert value before pos (pos may be the end() iterator)
     * return an iterator pointing to the inserted value
     * throw if the iterator is invalid
     */
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.owner != this || pos.cur == nullptr) throw invalid_iterator();
        node *p = pos.cur;
        val_node *nv = new val_node(value);
        insert(p, nv);
        ++n;
        return iterator(this, nv);
    }
    /**
     * remove the element at pos (the end() iterator is invalid)
     * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
     * throw if the container is empty, the iterator is invalid
     */
    virtual iterator erase(iterator pos) {
        if (pos.owner != this || pos.cur == nullptr) throw invalid_iterator();
        if (empty()) throw container_is_empty();
        node *p = pos.cur;
        if (p == head) throw invalid_iterator();
        node *nxt = p->next;
        erase(p);
        delete static_cast<val_node *>(p);
        --n;
        return iterator(this, nxt);
    }
    /**
     * adds an element to the end
     */
    void push_back(const T &value) {
        val_node *nv = new val_node(value);
        insert(head, nv); // before head => at tail
        ++n;
    }
    /**
     * removes the last element
     * throw when the container is empty.
     */
    void pop_back() {
        if (empty()) throw container_is_empty();
        node *p = head->prev;
        erase(p);
        delete static_cast<val_node *>(p);
        --n;
    }
    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) {
        val_node *nv = new val_node(value);
        insert(head->next, nv); // before first => at front
        ++n;
    }
    /**
     * removes the first element.
     * throw when the container is empty.
     */
    void pop_front() {
        if (empty()) throw container_is_empty();
        node *p = head->next;
        erase(p);
        delete static_cast<val_node *>(p);
        --n;
    }
    /**
     * sort the values in ascending order with operator< of T
     */
    void sort() {
        if (n <= 1) return;
        // copy data to array
        T *arr = static_cast<T *>(::operator new(sizeof(T) * n));
        size_t idx = 0;
        for (node *p = head->next; p != head; p = p->next, ++idx) {
            new (arr + idx) T(static_cast<val_node *>(p)->data);
        }
        std::function<bool(const T&, const T&)> cmp = [](const T &a, const T &b){ return a < b; };
        sjtu::sort(arr, arr + n, cmp);
        // write back
        idx = 0;
        for (node *p = head->next; p != head; p = p->next, ++idx) {
            static_cast<val_node *>(p)->data = arr[idx];
        }
        // destroy and free array
        for (size_t i = 0; i < n; ++i) {
            arr[i].~T();
        }
        ::operator delete(arr);
    }
    /**
     * merge two sorted lists into one (both in ascending order)
     * compare with operator< of T
     * container other becomes empty after the operation
     * for equivalent elements in the two lists, the elements from *this shall always precede the elements from other
     * the order of equivalent elements of *this and other does not change.
     * no elements are copied or moved
     */
    void merge(list &other) {
        if (this == &other) return;
        node *p = head->next;
        node *q = other.head->next;
        while (p != head && q != other.head) {
            val_node *pv = static_cast<val_node *>(p);
            val_node *qv = static_cast<val_node *>(q);
            if (qv->data < pv->data) {
                node *qnxt = q->next;
                // unlink q from other
                q->prev->next = q->next;
                q->next->prev = q->prev;
                // insert q before p in this
                q->prev = p->prev;
                q->next = p;
                p->prev->next = q;
                p->prev = q;
                ++n; --other.n;
                q = qnxt;
            } else {
                p = p->next;
            }
        }
        // append remaining q's at end (before head)
        while (q != other.head) {
            node *qnxt = q->next;
            // unlink q
            q->prev->next = q->next;
            q->next->prev = q->prev;
            // insert before head
            q->prev = head->prev;
            q->next = head;
            head->prev->next = q;
            head->prev = q;
            ++n; --other.n;
            q = qnxt;
        }
        // other becomes empty
        other.head->next = other.head->prev = other.head;
    }
    /**
     * reverse the order of the elements
     * no elements are copied or moved
     */
    void reverse() {
        if (n <= 1) return;
        node *p = head;
        do {
            node *tmp = p->next;
            p->next = p->prev;
            p->prev = tmp;
            p = tmp; // after swap, move to original next
        } while (p != head);
    }
    /**
     * remove all consecutive duplicate elements from the container
     * only the first element in each group of equal elements is left
     * use operator== of T to compare the elements.
     */
    void unique() {
        if (n <= 1) return;
        node *p = head->next;
        while (p != head) {
            node *q = p->next;
            while (q != head && static_cast<val_node *>(p)->data == static_cast<val_node *>(q)->data) {
                node *qnxt = q->next;
                erase(q);
                delete static_cast<val_node *>(q);
                --n;
                q = qnxt;
            }
            p = q;
        }
    }
};

}

#endif //SJTU_LIST_HPP
