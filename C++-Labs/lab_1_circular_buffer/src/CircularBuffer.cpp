#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <sstream>

#include "CircularBuffer.h"

CircularBuffer::CircularBuffer() :
buffer_(nullptr),
buffer_start_(0),
capacity_(0),
size_(0) {
}

CircularBuffer::CircularBuffer(size_t capacity) : CircularBuffer() {
    set_capacity(capacity);
}

CircularBuffer::CircularBuffer(size_t capacity, const value_type &elem) : CircularBuffer(capacity) {
    resize(capacity, elem);
}

CircularBuffer::CircularBuffer(std::initializer_list<value_type> list)
: CircularBuffer(list.size()) {
    size_t i = 0;
    for (value_type val : list) {
        buffer_[i++] = val;
    }
    size_ = list.size();
}

CircularBuffer::~CircularBuffer() {
    delete buffer_;
}

void CircularBuffer::resize(size_t new_size, const value_type &item) {
    if (new_size > capacity()) {
        set_capacity(new_size);
    }

    linearize();
    std::fill(buffer_ + size_, buffer_ + new_size, item);
    size_ = new_size;
}

void CircularBuffer::set_capacity(size_t new_capacity) {
    if (new_capacity < size_) {
        size_ = new_capacity;
    }

    linearize();
    auto* new_buffer = new value_type[new_capacity];
    std::memcpy(new_buffer, buffer_, size_ * sizeof(value_type));

    delete buffer_;
    buffer_ = new_buffer;
    capacity_ = new_capacity;
}

void CircularBuffer::push_back(const value_type &item) {
    if (capacity() == 0) {
        return;
    }

    buffer_[(buffer_start_ + size()) % capacity()] = item;
    if (size() != capacity()) {
        ++size_;
    } else {
        buffer_start_ = (buffer_start_ + 1) % capacity();
    }
}

void CircularBuffer::push_front(const value_type &item) {
    if (capacity() == 0) {
        return;
    }

    buffer_[(buffer_start_ + capacity() - 1) % capacity()] = item;
    buffer_start_ = (buffer_start_ + capacity() - 1) % capacity();
    if (size() != capacity()) {
        ++size_;
    }
}

void CircularBuffer::pop_back() {
    --size_;
}

void CircularBuffer::pop_front() {
    --size_;
    buffer_start_ = (buffer_start_ + 1) % capacity();
}

size_t CircularBuffer::capacity() const {
    return capacity_;
}

size_t CircularBuffer::size() const {
    return size_;
}

bool CircularBuffer::empty() const {
    return size() == 0;
}

bool CircularBuffer::full() const {
    return size() == capacity();
}

size_t CircularBuffer::reserve() const {
    return capacity() - size();
}

void CircularBuffer::insert(int pos, const value_type &item) {
    linearize();
    if (size() == capacity()) {
        std::memmove(buffer_ + pos + 1, buffer_ + pos, size() - pos - 1);
    }
    else {
        std::memmove(buffer_ + pos + 1, buffer_ + pos, size() - pos);
        ++size_;
    }
    buffer_[pos] = item;
}

void CircularBuffer::clear() {
    size_ = 0;
}

void CircularBuffer::rotate(int new_begin) {
    std::rotate(buffer_, buffer_ + new_begin, buffer_ + capacity());
}

value_type *CircularBuffer::linearize() {
    std::rotate(buffer_, buffer_ + buffer_start_, buffer_ + capacity());
    buffer_start_ = 0;
    return buffer_;
}

bool CircularBuffer::is_linearized() const {
    return buffer_start_ == 0;
}

void CircularBuffer::erase(int first, int last) {
    linearize();
    size_ -= (last - first);
    std::memmove(buffer_ + first, buffer_ + last, capacity() - last);
}

value_type &CircularBuffer::operator[](size_t i) {
    return buffer_[(buffer_start_ + i) % capacity()];
}

const value_type &CircularBuffer::operator[](size_t i) const {
    return buffer_[(buffer_start_ + i) % capacity()];
}

value_type &CircularBuffer::at(size_t i) {
    if (i >= size()) {
        std::stringstream msg;
        msg << "CircularBuffer: i (which is " << i << ") >= this->size() (which is (" << size() << ")";
        throw std::out_of_range(msg.str());
    }
    return (*this)[i];
}

const value_type &CircularBuffer::at(size_t i) const {
    if (i >= size()) {
        std::stringstream msg;
        msg << "CircularBuffer: i (which is " << i << ") >= this->size() (which is (" << size() << ")";
        throw std::out_of_range(msg.str());
    }
    return (*this)[i];
}

value_type &CircularBuffer::front() {
    return buffer_[buffer_start_];
}

value_type &CircularBuffer::back() {
    return buffer_[(buffer_start_ + size()) % capacity()];
}

const value_type &CircularBuffer::front() const {
    return buffer_[buffer_start_];
}

const value_type &CircularBuffer::back() const {
    return buffer_[(buffer_start_ + size()) % capacity()];
}

CircularBuffer &CircularBuffer::operator=(const CircularBuffer &cb) {
    CircularBuffer tmp(cb);
    swap(tmp);
    return *this;
}

void CircularBuffer::swap(CircularBuffer &cb) noexcept {
    std::swap(*this, cb);
//    std::swap(buffer_, cb.buffer_);
//    std::swap(buffer_start_, cb.buffer_start_);
//    std::swap(capacity_, cb.capacity_);
//    std::swap(size_, cb.size_);
}

CircularBuffer::CircularBuffer(const CircularBuffer &cb) :
buffer_start_(cb.buffer_start_),
capacity_(cb.capacity()),
size_(cb.size()){
    buffer_ = new value_type[cb.size()];
    std::memcpy(buffer_, cb.buffer_, cb.size());
}

bool operator==(const CircularBuffer &a, const CircularBuffer &b) {
    for (size_t i = 0; i < std::min(a.size(), b.size()); ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const CircularBuffer &a, const CircularBuffer &b) {
    return !(a == b);
}
