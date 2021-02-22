//
// Created by Raz on 10/01/2021.
//

#ifndef EX6_VL_VECTOR_H
#define EX6_VL_VECTOR_H

#include <iostream>
#include <cmath>
#include <algorithm>

#define INCREASE_VALUE 1.5
#define USING_HEAP(heap_capacity) (heap_capacity > 0)
#define RESIZE(heap_capacity, size, k) (heap_capacity < size + k)
#define STACK_OK(stack_capacity, size, k) (size + k <= stack_capacity)
#define CAP_HEAP(size) floor(INCREASE_VALUE * (size))
#define OUT_OF_RANGE_MSG "The index is out of range"
#define DEFAULT_CAP 16

using namespace std;

template<class T, size_t StaticCapacity = DEFAULT_CAP>
class VLVector {
private:
    size_t size_;
    size_t heap_cap_;
    T stack_data_[StaticCapacity];
    T *heap_data_;

    /*
     * private methods
     */
    void DiffCopy(size_t start, size_t end, size_t diff, T *dest_data,
                  const T *origin_data) {
        for (; start < end + diff; ++start) {
            dest_data[start] = origin_data[start - diff];
        }
    }

    void UpdateDataByValue(T* data, T value, size_t count){
        for (size_t i = 0; i < count; ++i) {
            data[i] = value;
        }
    }

    template<class InputIterator>
    int UpdateDataByIter(InputIterator first, InputIterator last, T *dest_data,
                         int index) {
        for (; first != last; ++first) {
            dest_data[index++] = *first;
        }
        return index;
    }

    size_t FindIndex(T *data, T *position){
        size_t i = 0;
        for (; i < size_; ++i) {
            if (data + i == position) {
                break;
            }
        }
        return i;
    }

    void AddValue(T value, size_t start, T* orig_data, T* dest_data){
        ++size_;
        T prev = value;
        T temp;
        for (; start < size_; ++start) {
            temp = orig_data[start];
            dest_data[start] = prev;
            prev = temp;
        }
    }

    template<class InputIterator>
    void InsertIterHelper(T* data, size_t len, T* position, InputIterator first, InputIterator last, bool heap){
        T *temp_data = new T[size_+len];
        size_t i = FindIndex(data, position);
        copy(data, data+i, temp_data);
        i = UpdateDataByIter<InputIterator>(first, last, temp_data, i);
        DiffCopy(i, size_, len, temp_data, data);
        size_ += len;
        if (heap){
            if (RESIZE(heap_cap_, size_, len)) {
                DeleteHeap();
                InitHeap(size_);
            }
            copy(temp_data, temp_data+size_, heap_data_);
        }
        else {
            copy(temp_data, temp_data+size_, data);
        }
        delete[] temp_data;
    }

    void EraseHelper(T* data, size_t len, T* position){
        size_t i = FindIndex(data, position);
        copy(data+i+len, data + size_, data + i);
        size_ -= len;
    }

    void InitHeap(size_t size){
        heap_cap_ = CAP_HEAP(size);
        heap_data_ = new T[heap_cap_];
    }

    void DeleteHeap(){
        heap_cap_ = 0;
        delete[] heap_data_;
    }

public:
    typedef T *iterator;
    typedef const T *const_iterator;

    /**
     * Default Constructor - empty VLVector
     */
    VLVector() : size_(0), heap_cap_(0){}

    /**
     * Copy Constructor - get other VLVector and copy its values
     */
    VLVector(const VLVector<T, StaticCapacity> &other) : size_(other.size_),
                                                         heap_cap_(
                                                                 other.heap_cap_) {
        if (USING_HEAP(heap_cap_)) {
            heap_data_ = new T[heap_cap_];
            copy(other.heap_data_, other.heap_data_+size_, heap_data_);
        } else {
            copy(other.stack_data_, other.stack_data_+size_, stack_data_);
        }
    }

    /**
     * Single-value initialized constructor - get value and count and creates a
     * vector of this value count times
     */
    VLVector(size_t count, T v) : size_(count), heap_cap_(0) {
        if (STACK_OK(StaticCapacity, size_, 0)) {
            UpdateDataByValue(stack_data_, v, count);
        } else {
            InitHeap(size_);
            UpdateDataByValue(heap_data_, v, count);
        }
    }

    /**
     * Sequence based Constructor - get input iterator and copy its values (from
     * first to last).
     */
    template<class InputIterator>
    VLVector(InputIterator first, InputIterator last): size_(0), heap_cap_(0) {
        size_ = std::distance(first, last);
        if (STACK_OK(StaticCapacity, size_, 0)) {
            UpdateDataByIter<InputIterator>(first, last, stack_data_, 0);
        } else {
            InitHeap(size_);
            UpdateDataByIter<InputIterator>(first, last, heap_data_, 0);
        }
    }

    /**
     * Destructor
     */
    ~VLVector() {
        if (USING_HEAP(heap_cap_)) {
            delete[] heap_data_;
        }
    }

    /*
     * Vector methods
     */
    /**
     * @return VLVector size
     */
    size_t Size() const noexcept{
        return size_;
    }

    /**
     * @return VLVector capacity
     */
    size_t Capacity() const noexcept{
        return USING_HEAP(heap_cap_) ? heap_cap_ : StaticCapacity;
    }

    /**
     * Checks if the vector is empty, return true if empty, false otherwise
     */
    bool Empty() const noexcept{
        return size_ == 0;
    }

    /**
     * @return the value in the input index, if it doesn't exist throw an exception
     */
    T &At(size_t index) noexcept(false) {
        if (index < 0 || (size_t) index >= size_) {
            throw out_of_range(OUT_OF_RANGE_MSG);
        }
        return USING_HEAP(heap_cap_) ? heap_data_[index] : stack_data_[index];
    }

    /**
     * @return the value in the input index, if it doesn't exist throw an exception
     */
    T At(size_t index) const noexcept(false) {
        if (index < 0 || (size_t) index >= size_) {
            throw out_of_range(OUT_OF_RANGE_MSG);
        }
        return USING_HEAP(heap_cap_) ? heap_data_[index] : stack_data_[index];
    }

    /**
     * @return pointer to the current variable who holds the data (stack or heap)
     */
    T *Data() noexcept{
        return USING_HEAP(heap_cap_) ? heap_data_ : stack_data_;
    }

    /**
     * @return pointer to the current variable who holds the data (stack or heap)
     */
    const T *Data() const noexcept {
        return USING_HEAP(heap_cap_) ? heap_data_ : stack_data_;
    }

    /**
     * Push the input value to the end of the VLVector.
     */
    void PushBack(T value) noexcept(false) {
        if (USING_HEAP(heap_cap_)) {
            if (RESIZE(heap_cap_, size_, 1)) {
                T *temp_data = new T[size_];
                copy(heap_data_, heap_data_+size_, temp_data);
                DeleteHeap();
                InitHeap(size_+1);
                copy(temp_data, temp_data+size_, heap_data_);
                delete[] temp_data;
            }
            heap_data_[size_++] = value;
        } else if (STACK_OK(StaticCapacity, size_, 1)) {
            stack_data_[size_++] = value;
        } else {
            InitHeap(size_ + 1);
            copy(stack_data_, stack_data_+size_, heap_data_);
            heap_data_[size_++] = value;
        }
    }

    /**
     * Pop the last value of this VLVector
     */
    void PopBack() noexcept(false) {
        if (size_ == 0) {
            return;
        }
        --size_;
        if (USING_HEAP(heap_cap_) && STACK_OK(StaticCapacity, size_, 0)) {
            copy(heap_data_, heap_data_+size_, stack_data_);
            DeleteHeap();
        }
    }

    /**
     * Insert method of InputIterator which gets position iterator which point
     * to the location the new value need to insert in.
     */
    iterator Insert(iterator position, T value) noexcept(false){
        size_t distance = std::distance(begin(), position);
        if (USING_HEAP(heap_cap_)) {
            if (RESIZE(heap_cap_, size_, 1)) {
                T *temp_data = new T[size_+1];
                size_t i = FindIndex(heap_data_, position);
                copy(heap_data_, heap_data_+i, temp_data);
                AddValue(value, i, heap_data_, temp_data);
                DeleteHeap();
                InitHeap(size_ + 1);
                copy(temp_data, temp_data+size_, heap_data_);
                delete[] temp_data;
            }
            else {
                AddValue(value, FindIndex(heap_data_, position), heap_data_, heap_data_);
            }
        } else if (STACK_OK(StaticCapacity, size_, 1)) {
            AddValue(value, FindIndex(stack_data_, position), stack_data_, stack_data_);
        } else { // stack to heap
            InitHeap(size_ + 1);
            size_t i = FindIndex(stack_data_, position);
            copy(stack_data_, stack_data_+i, heap_data_);
            AddValue(value, i, stack_data_, heap_data_);
        }
        return begin() + distance;
    }

    /**
     * Insert method of InputIterator which gets position iterator which point
     * to the location the new values (which gets by first and last iterators)
     * need to insert in.
     */
    template<class InputIterator>
    iterator
    Insert(iterator position, InputIterator first, InputIterator last) noexcept(false){
        size_t distance = std::distance(begin(), position);
        size_t input_len = std::distance(first, last);
        if (USING_HEAP(heap_cap_)) {
            InsertIterHelper<InputIterator>(heap_data_, input_len, position, first, last, true);
        } else if (STACK_OK(StaticCapacity, size_, input_len)) {
            InsertIterHelper<InputIterator>(stack_data_, input_len, position, first, last, false);
        } else {
            InitHeap(size_ + input_len);
            size_t i = FindIndex(stack_data_, position);
            copy(stack_data_, stack_data_+i, heap_data_);
            i = UpdateDataByIter<InputIterator>(first, last, heap_data_, i);
            DiffCopy(i, size_, input_len, heap_data_, stack_data_);
            size_ += input_len;
        }
        return begin() + distance;
    }

    /**
     * Erase the value in the input position (gets iterator to the position)
     */
    iterator Erase(iterator position) noexcept(false) {
        if (size_ == 0) {
            return end();
        }
        size_t distance = std::distance(begin(), position);
        if (!USING_HEAP(heap_cap_)) {
            EraseHelper(stack_data_, 1, position);
        } else {
            EraseHelper(heap_data_, 1, position);
            if (STACK_OK(StaticCapacity, size_, 0)) {
                copy(heap_data_, heap_data_+size_, stack_data_);
                DeleteHeap();
            }
        }
        return begin() + distance;
    }

    /**
     * Erase all values in the input range values (gets iterators of first and
     * last values)
     */
    iterator Erase(iterator first, iterator last) noexcept(false) {
        if (size_ == 0) {
            return end();
        }
        size_t distance = std::distance(begin(), first);
        if (!USING_HEAP(heap_cap_)) {
            EraseHelper(stack_data_, std::distance(first, last), first);
        } else {
            EraseHelper(heap_data_, std::distance(first, last), first);
            if (STACK_OK(StaticCapacity, size_, 0)) {
                copy(heap_data_, heap_data_+size_, stack_data_);
                DeleteHeap();
            }
        }
        return begin() + distance;
    }

    /**
     * Clear all VLVector data
     */
    void Clear() noexcept{
        size_ = 0;
        if (USING_HEAP(heap_cap_)) {
            DeleteHeap();
        }
    }

    /*
     * operators
     */
    VLVector<T, StaticCapacity> &
    operator=(const VLVector<T, StaticCapacity> &other) noexcept(false){
        if (this != &other) {
            Clear();
            size_ = other.size_;
            if (USING_HEAP(other.heap_cap_)) {
                heap_cap_ = other.heap_cap_;
                heap_data_ = new T[heap_cap_];
                copy(other.heap_data_, other.heap_data_+size_, heap_data_);
            } else {
                copy(other.stack_data_, other.stack_data_+size_, stack_data_);
            }
        }
        return *this;
    }

    T &operator[](int index) noexcept {
        if (index < 0 || (size_t) index >= size_) {
            return stack_data_[0];
        }
        return USING_HEAP(heap_cap_) ? heap_data_[index] : stack_data_[index];
    }

    T operator[](int index) const noexcept {
        if (index < 0 || (size_t) index >= size_) {
            return stack_data_[0];
        }
        return USING_HEAP(heap_cap_) ? heap_data_[index] : stack_data_[index];
    }

    /**
     * Equal operator - two VLVector are equals only if they have got same values
     * in the same order.
     */
    bool operator==(const VLVector<T, StaticCapacity> &other) const noexcept{
        if (size_ != other.size_) return false;
        if (USING_HEAP(heap_cap_)) {
            for (size_t i = 0; i < size_; ++i) {
                if (heap_data_[i] != other.heap_data_[i]) return false;
            }
        } else {
            for (size_t i = 0; i < size_; ++i) {
                if (stack_data_[i] != other.stack_data_[i]) return false;
            }
        }
        return true;
    }

    /**
     * Unequal operator - two VLVector aren't equals if they have different values
     * or those values aren't in the same order.
     */
    bool operator!=(const VLVector<T, StaticCapacity> &other) const noexcept{
        return !this->operator==(other);
    }

    /*
     * iterator methods
     */
    iterator begin() {
        return Data();
    }

    const_iterator begin() const {
        return Data();
    }

    const_iterator cbegin() const {
        return Data();
    }

    reverse_iterator<iterator> rbegin() {
        return make_reverse_iterator(this->end());
    }

    reverse_iterator<const_iterator> rbegin() const {
        return make_reverse_iterator(this->end());
    }

    reverse_iterator<const_iterator> crbegin() const {
        return make_reverse_iterator(this->end());
    }

    iterator end() {
        return Data() + size_;
    }

    const_iterator end() const {
        return Data() + size_;
    }

    const_iterator cend() const {
        return Data() + size_;
    }

    reverse_iterator<iterator> rend() {
        return make_reverse_iterator(this->begin());
    }

    reverse_iterator<const_iterator> rend() const {
        return make_reverse_iterator(this->begin());
    }

    reverse_iterator<const_iterator> crend() const {
        return make_reverse_iterator(this->begin());
    }
};

#endif //EX6_VL_VECTOR_H