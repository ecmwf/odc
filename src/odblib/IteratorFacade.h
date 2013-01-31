/// @file   IteratorFacade.h
/// @author Tomas Kral

#ifndef ODBLIB_ITERATORFACADE_H_
#define ODBLIB_ITERATORFACADE_H_

#include <iterator>

namespace odb {

template <typename I, typename C, typename V, typename R, typename P>
class IteratorFacade;

class IteratorFacadeAccess
{
    template <typename I, typename C, typename V, typename R, typename P>
    friend class IteratorFacade;

    template <typename Facade>
    static typename Facade::reference dereference(const Facade& f)
    {
        return f.dereference();
    }

    template <typename Facade1, typename Facade2>
    static bool equal(const Facade1& f1, const Facade2& f2)
    {
        return f1.equal(f2);
    }

    template <typename Facade>
    static void increment(Facade& f)
    {
        f.increment();
    }

    template <typename Facade>
    static void decrement(Facade& f)
    {
        f.decrement();
    }

    template <typename Facade>
    static void advance(Facade& f, typename Facade::difference_type n)
    {
        f.advance(n);
    }

    template <typename Facade1, typename Facade2>
    static typename Facade1::difference_type distance(const Facade1& f1,
            const Facade2& f2)
    {
        return f1.distance(f2);
    }

    IteratorFacadeAccess();
};

template <typename Iterator, typename Category, typename Value,
          typename Reference = Value&, typename Pointer = Value*>

class IteratorFacade
    : public std::iterator<Category, Value, std::ptrdiff_t, Reference, Pointer>
{
public:
    typedef std::ptrdiff_t difference_type;
    typedef Value value_type;
    typedef Reference reference;
    typedef Pointer pointer;
    typedef Category iterator_category;

    reference operator*() const
    {
        return IteratorFacadeAccess::dereference(this->iterator());
    }

    pointer operator->() const
    {
        return &IteratorFacadeAccess::dereference(this->iterator());
    }

    bool operator==(const Iterator& other) const
    {
        return IteratorFacadeAccess::equal(this->iterator(), other);
    }

    bool operator!=(const Iterator& other) const
    {
        return !IteratorFacadeAccess::equal(this->iterator(), other);
    }

    Iterator& operator++()
    {
        IteratorFacadeAccess::increment(this->iterator());
        return this->iterator();
    }

    Iterator& operator--()
    {
        IteratorFacadeAccess::decrement(this->iterator());
        return this->iterator();
    }

    Iterator& operator+=(difference_type n)
    {
        IteratorFacadeAccess::advance(this->iterator(), n);
        return this->iterator();
    }

    Iterator& operator-=(difference_type n)
    {
        IteratorFacadeAccess::advance(this->iterator(), -n);
        return this->iterator();
    }

    Iterator operator+(difference_type n) const
    {
        Iterator it(this->iterator());
        IteratorFacadeAccess::advance(it, n);
        return it;
    }

    Iterator operator-(difference_type n) const
    {
        Iterator it(this->iterator());
        IteratorFacadeAccess::advance(it, -n);
        return it;
    }

    difference_type operator-(const Iterator& other) const
    {
        return IteratorFacadeAccess::distance(this->iterator(), other);
    }

private:
    Iterator& iterator()
    {
        return static_cast<Iterator&>(*this);
    }

    const Iterator& iterator() const
    {
        return static_cast<const Iterator&>(*this);
    }
};

template <typename Iterator, typename Value,
          typename Reference = Value&, typename Pointer = Value*>
class InputIteratorFacade
  : public IteratorFacade<Iterator, std::input_iterator_tag,
           Value, Reference, Pointer>
{};

template <typename Iterator, typename Value,
          typename Reference = Value&, typename Pointer = Value*>
class OutputIteratorFacade
  : public IteratorFacade<Iterator, std::output_iterator_tag,
           Value, Reference, Pointer>
{};

template <typename Iterator, typename Value,
          typename Reference = Value&, typename Pointer = Value*>
class ForwardIteratorFacade
  : public IteratorFacade<Iterator, std::forward_iterator_tag,
           Value, Reference, Pointer>
{};

template <typename Iterator, typename Value,
          typename Reference = Value&, typename Pointer = Value*>
class BidirectionalIteratorFacade
  : public IteratorFacade<Iterator, std::bidirectional_iterator_tag,
           Value, Reference, Pointer>
{};

template <typename Iterator, typename Value,
          typename Reference = Value&, typename Pointer = Value*>
class RandomIteratorFacade
  : public IteratorFacade<Iterator, std::random_access_iterator_tag,
           Value, Reference, Pointer>
{};

} // namespace odb

#endif // ODBLIB_ITERATORFACADE_H_
