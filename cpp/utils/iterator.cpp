#include <vector>
#include <iostream>
#include <map>
#include <type_traits>
#include <cstddef>
#include <iterator>

template <class MapIterator>
class ValueIterator {
public:
    typedef typename MapIterator::value_type::second_type  value_type;
    typedef const value_type& reference;
    typedef const value_type* pointer;

    typedef std::bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;

    explicit ValueIterator(MapIterator map_iterator)
        : map_iterator_ { map_iterator }
    {
    }

    reference operator*() const
    { return map_iterator_->second; }

    pointer operator->() const
    { return &map_iterator_->second; }

    ValueIterator& operator++()
    { ++map_iterator_; return *this; }

    ValueIterator operator++(int) const
    { MapIterator next = map_iterator_; return ValueIterator(++next); }

    friend bool operator==(const ValueIterator& lhs,
                    const ValueIterator& rhs)
    { return lhs.map_iterator_ == rhs.map_iterator_; }

private:
    MapIterator map_iterator_;
};

template <class MapIterator>
class KeyIterator {
public:
    typedef typename MapIterator::value_type::first_type value_type;
    typedef const value_type& reference;
    typedef const value_type* pointer;

    typedef std::bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t                  difference_type;

    explicit KeyIterator(MapIterator map_iterator)
        : map_iterator_ { map_iterator }
    {
    }

    reference operator*() const
    { return map_iterator_->first; }

    pointer operator->() const
    { return &map_iterator_->first; }

    KeyIterator& operator++()
    { ++map_iterator_; return *this; }

    KeyIterator operator++(int)
    { MapIterator next = map_iterator_; return KeyIterator(++next); }

    friend bool operator==(const KeyIterator& lhs,
                    const KeyIterator& rhs)
    { return lhs.map_iterator_ == rhs.map_iterator_; }

private:
    MapIterator map_iterator_;
};

template <class _Iterator>
inline ValueIterator<_Iterator> value_iterator(const _Iterator& it)
{
    return ValueIterator<_Iterator>(it);
}

template <class MapIterator>
inline bool operator!=(const ValueIterator<MapIterator>& lhs,
                const ValueIterator<MapIterator>& rhs)
{
    return !(lhs == rhs);
}

template <class _Iterator>
inline KeyIterator<_Iterator> key_iterator(const _Iterator& it)
{
    return KeyIterator<_Iterator>(it);
}

template <class MapIterator>
inline bool operator!=(const KeyIterator<MapIterator>& lhs,
                const KeyIterator<MapIterator>& rhs)
{
    return !(lhs == rhs);
}

int main()
{
    std::map<int, int> name_map { { 10, 1 }, { 100, 2 }, { 1000, 3 } };
    std::cout << std::distance(name_map.rbegin(), name_map.rend()) << std::endl;
    std::cout << std::distance(key_iterator(name_map.begin()), key_iterator(name_map.end())) << std::endl;
    std::vector<int> vi(key_iterator(name_map.begin()), key_iterator(name_map.end()));
    std::ostream_iterator<int> osi(std::cout, " ");
    std::copy(key_iterator(name_map.begin()), key_iterator(name_map.end()), osi);
    std::cout << "\n";
    return 0;
}

