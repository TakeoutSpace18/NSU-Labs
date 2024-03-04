#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <cstddef>

typedef char value_type;

class CircularBuffer {
private:
    value_type *buffer_;
    size_t buffer_start_;
    size_t capacity_;
    size_t size_;

public:
    /// Construct empty circular buffer with zero capacity
    CircularBuffer();
    ~CircularBuffer();

    /// Copy constructor
    CircularBuffer(const CircularBuffer &other);

    /**
     * Construct empty circular buffer with specified capacity
     * @param capacity buffer capacity
    */
    explicit CircularBuffer(size_t capacity);

    /**
     * Construct buffer with specified capacity and fill it with specified element
     * @param capacity buffer capacity
     * @param elem element to fill with
     */
    CircularBuffer(size_t capacity, const value_type &elem);

    /**
     * Construct circular buffer with initializer list
     * @param list initializer list
     */
    CircularBuffer(std::initializer_list<value_type> list);

    /**
     * Access element at specified position. Bounds are not checked.
     *
     * @param i position of element
     * @return reference to requested element
     */
    value_type &operator[](size_t i);

    /**
     * Access element at specified position. Bounds are not checked.
     *
     * @param i position of element
     * @return const reference to requested element
     */
    const value_type &operator[](size_t i) const;

    /**
     * Access element at specified position with bounds checking.
     * @param i position of element
     * @return reference to requested element
     * @throw std::out_of_range if i >= size()
     */
    value_type &at(size_t i);

    /**
     * Access element at specified position with bounds checking.
     * @param i position of element
     * @return const reference to requested element
     * @throw std::out_of_range if i >= size()
     */
    [[nodiscard]] const value_type &at(size_t i) const;

    /**
     * Get first element reference
     * Calling on empty container results in UB
     * @return first element reference
     */
    value_type &front();

    /**
     * Get last element reference
     * Calling on empty container results in UB
     * @return last element reference
     */
    value_type &back();

    /**
     * Get first element const reference
     * Calling on empty container results in UB
     * @return first element const reference
     */
    [[nodiscard]] const value_type &front() const;

    /**
     * Get last element const reference
     * Calling on empty container results in UB
     * @return last element const reference
     */
    [[nodiscard]] const value_type &back() const;

    /**
     * Shift circular buffer in such way, that it's first element
     * is moved to the beginning of allocated memory.
     * @return pointer to the first element
     */
    value_type *linearize();

    /**
     * Check if circular buffer is linearized
     * @return linearized of not
     */
    [[nodiscard]] bool is_linearized() const;

    /**
     * Rotate elements on circular buffer.
     * circ_buf[new_begin] becomes circ_buf[0]
     * @param new_begin element index to start buffer with
     */
    void rotate(int new_begin);

    /**
     * Number of elements stored in circular buffer
     * @return circular buffer size
     */
    [[nodiscard]] size_t size() const;

    /**
     * Check if circular buffer is empty
     * @return size() == 0
     */
    [[nodiscard]] bool empty() const;

    /**
     * Check if circular buffer is full
     * @return size() == capacity()
     */
    [[nodiscard]] bool full() const;

    /**
     * Get maximum number of elements which can be inserted without overwriting any of already stored elements.
     * @return capacity() - size()
     */
    [[nodiscard]] size_t reserve() const;

    /**
     * Circular buffer capacity
     * @return capacity
     */
    [[nodiscard]] size_t capacity() const;

    /**
     * Set circular buffer capacity. If new_capacity is less than size(),
     * then last elements in buffer are removed
     * @param new_capacity new buffer capacity
     */
    void set_capacity(size_t new_capacity);

    /**
     * Resize buffer. In case of extension, new elements are filled with item
     * If new_size is greater than capacity, capacity is set to new_size.
     *
     * @param new_size new buffer size
     * @param item element to fill new space with
     */
    void resize(size_t new_size, const value_type &item = value_type());

    /**
     * Copy assignment operator
     * @param other assign from
     * @return reference to assigned value
     */
    CircularBuffer &operator=(CircularBuffer other);

    /**
     * Swap circular buffer contents
     * @param other buffer to swap with
     */
    void swap(CircularBuffer &other) noexcept;

    /**
     * Add element at the end of circular buffer.
     * If size() == capacity(), then first element in buffer is overwritten.
     * If capacity is 0, nothing will be inserted
     * @param item item to add
     */
    void push_back(const value_type &item = value_type());

    /**
    * Add element at the beginning of circular buffer.
    * If size() == capacity(), then last element in buffer is overwritten.
    * If capacity is 0, nothing will be inserted
    * @param item item to add
    */
    void push_front(const value_type &item = value_type());

    /**
     * Remove last element of circular buffer.
     * Calling on empty container results in undefined behaviour
     */
    void pop_back();

     /**
     * Remove first element of circular buffer.
     * Calling on empty container results in undefined behaviour
     */
    void pop_front();

    /**
     * Insert element at specified position. Capacity remains unchanged.
     * If pos is not in the circular buffer size bounds, calling this function results in UB.
     * @param pos position to insert to
     * @param item element to insert
     */
    void insert(int pos, const value_type &item = value_type());

    /**
     * Delete elements in range [first, last) from circular buffer
     * @param first
     * @param last
     */
    void erase(size_t first, size_t last);

    /**
     * Clear circular buffer
     */
    void clear();
};

/// Check if circular buffers have equal sizes and elements
bool operator==(const CircularBuffer &a, const CircularBuffer &b);

/// Check if circular buffers have different elements
bool operator!=(const CircularBuffer &a, const CircularBuffer &b);

#endif //CIRCULARBUFFER_H
