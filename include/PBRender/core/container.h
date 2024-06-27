#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <limits>
#include <stdexcept>
#include <utility>
#include <compare>
#include <initializer_list>

#include <PBRender/core/common.h>
#include <PBRender/core/vecmath.h>

namespace PBRender
{

template <typename T, typename Alloc = std::allocator<T>>
class Array2D {
public:
    using value_type                = T;
    using allocator_type            = Alloc;
    using size_type                 = size_t;
    using difference_type           = ptrdiff_t;
    using pointer                   = T *;
    using const_pointer             = T const *;
    using reference                 = T &;
    using const_reference           = T const &;
    using iterator                  = T *;
    using const_iterator            = T const *;
    using reverse_iterator          = std::reverse_iterator<T *>;
    using const_reverse_iterator    = std::reverse_iterator<T const *>;

    // constructor
    Array2D(allocator_type allocator = {}) : Array2D({{0, 0}, {0, 0}}, allocator) {}

    Array2D(Bounds2i extent, allocator_type const &allocator = Alloc())
        : extent(extent), m_alloc(allocator) {
        int n = extent.Area();

        values = m_alloc.allocate(n);
        for (int i = 0; i < n; ++i)
            std::construct_at(values + i);
    }

    Array2D(Bounds2i extent, T def, allocator_type allocator = {})
        : Array2D(extent, allocator) {
        std::fill(begin(), end(), def);
    }

    template <typename InputIt,
              typename = typename std::enable_if_t<
                  !std::is_integral_v<InputIt> &&
                  std::is_base_of<
                      std::input_iterator_tag,
                      typename std::iterator_traits<InputIt>::iterator_category>::value>>
    Array2D(InputIt first, InputIt last, int nx, int ny, allocator_type allocator = {})
        : Array2D({{0, 0}, {nx, ny}}, allocator) {
        std::copy(first, last, begin());
    }

    Array2D(int nx, int ny, allocator_type allocator = {})
        : Array2D({{0, 0}, {nx, ny}}, allocator) {}
    Array2D(int nx, int ny, T def, allocator_type allocator = {})
        : Array2D({{0, 0}, {nx, ny}}, def, allocator) {}
    Array2D(const Array2D &a, allocator_type allocator = {})
        : Array2D(a.begin(), a.end(), a.XSize(), a.YSize(), allocator) {}
    
    // move constructor
    Array2D(Array2D &&a, allocator_type allocator = {})
        : extent(a.extent), m_alloc(allocator) {
        if (allocator == a.m_alloc) {
            values = a.values;
            a.extent = Bounds2i({0, 0}, {0, 0});
            a.values = nullptr;
        } else {
            values = m_alloc.allocate(extent.Area());
            std::copy(a.begin(), a.end(), begin());
        }
    }

    Array2D &operator=(Array2D &&other) {
        if (&other == this) [[unlikely]] return *this;
        
        if (m_alloc == other.m_alloc) {
            std::swap(extent, other.extent);
            std::swap(values, other.values);
        } else if (extent == other.extent) {
            int n = extent.Area();
            for (int i = 0; i < n; ++i) {
                std::destroy_at(values + i);
                std::construct_at(values + i, other.values[i]);
            }
            extent = other.extent;
        } else {
            int n = extent.Area();
            for (int i = 0; i < n; ++i)
                std::destroy_at(values + i);
            m_alloc.deallocate(values, n);

            int no = other.extent.Area();
            values = m_alloc.allocate(no);
            for (int i = 0; i < no; ++i)
                std::construct_at(values + i, other.values[i]);
        }
        return *this;
    }

    // copy assign
    Array2D &operator=(const Array2D &other) {
        if (&other == this) [[unlikely]] return *this;

        // free current data
        int n = extent.Area();
        for (int i = 0; i < n; ++i)
            std::destroy_at(values + i);
        m_alloc.deallocate(values, n);

        // copy data from other
        extent = other.extent;
        values = m_alloc.allocate(extent.Area());
        for (int i = 0; i < extent.Area(); ++i)
            std::construct_at(values + i, other.values[i]);
        
        return *this;
    }

    // destructor
    ~Array2D() {
        int n = extent.Area();
        for (int i = 0; i < n; ++i)
            std::destroy_at(values + i);
        
        m_alloc.deallocate(values, n);
    }

    // operator[]
    T &operator[](Point2i p) {
        p.x -= extent.pMin.x;
        p.y -= extent.pMin.y;
        return values[p.x + (extent.pMax.x - extent.pMin.x) * p.y];
    }
    
    const T &operator[](Point2i p) const {
        // DCHECK(InsideExclusive(p, extent));
        assert(InsideExclusive(p, extent));
        p.x -= extent.pMin.x;
        p.y -= extent.pMin.y;
        return values[p.x + (extent.pMax.x - extent.pMin.x) * p.y];
    }

    T &operator()(int x, int y) { return (*this)[{x, y}]; }

    const T &operator()(int x, int y) const { return (*this)[{x, y}]; }

    // size()
    int size() { return extent.Area(); }
    int XSize() const { return extent.pMax.x - extent.pMin.x; }
    int YSize() const { return extent.pMax.y - extent.pMin.y; }

    // iterator
    T *data() noexcept {
        return values;
    }

    T const *data() const noexcept {
        return values;
    }

    T const *cdata() const noexcept {
        return values;
    }

    T *begin() noexcept {
        return values;
    }

    T *end() noexcept {
        return values + size();
    }

    T const *begin() const noexcept {
        return values;
    }

    T const *end() const noexcept {
        return values + size();
    }

    T const *cbegin() const noexcept {
        return values;
    }

    T const *cend() const noexcept {
        return values + size();
    }

    std::reverse_iterator<T *> rbegin() noexcept {
        return std::make_reverse_iterator(values + size());
    }

    std::reverse_iterator<T *> rend() noexcept {
        return std::make_reverse_iterator(values);
    }

    std::reverse_iterator<T const *> rbegin() const noexcept {
        return std::make_reverse_iterator(values + size());
    }

    std::reverse_iterator<T const *> rend() const noexcept {
        return std::make_reverse_iterator(values);
    }

    std::reverse_iterator<T const *> crbegin() const noexcept {
        return std::make_reverse_iterator(values + size());
    }

    std::reverse_iterator<T const *> crend() const noexcept {
        return std::make_reverse_iterator(values);
    }

private:
    Bounds2i extent;
    T *values;

    Alloc m_alloc;
};

} // namespace PBRender