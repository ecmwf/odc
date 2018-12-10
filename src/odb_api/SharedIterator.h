/// @file   SharedIterator.h
/// @author Tomas Kral

#ifndef ODBLIB_SHAREDITERATOR_H_
#define ODBLIB_SHAREDITERATOR_H_

namespace odc {

/*! @brief Represents a smart iterator.
 *
 *  The SharedIterator class uses reference counting to manage shared ownership
 *  of an iterator object. Several instances of SharedIterator may refer to the
 *  same iterator object. The managed iterator object is destroyed only when
 *  the last SharedIterator pointing to it is destroyed.
 *
 *  @see IteratorFacade
 *  @todo Write some test cases.
 */ 
template <typename T>
class SharedIterator
{
public:
    /// @brief Difference type of the managed iterator.
    /// Provided for compatibility with C++ STL algorithms.

    typedef typename T::difference_type difference_type;

    /// @brief Value type of the managed iterator.
    /// Provided for compatibility with C++ STL algorithms.

    typedef typename T::value_type value_type;

    /// @brief Reference type of the managed iterator.
    /// Provided for compatibility with C++ STL algorithms.

    typedef typename T::reference reference;

    /// @brief Pointer type of the managed iterator.
    /// Provided for compatibility with C++ STL algorithms.

    typedef typename T::pointer pointer;

    /// @brief Cagegory of the managed iterator.
    /// Provided for compatibility with C++ STL algorithms.

    typedef typename T::iterator_category iterator_category;

    /*! @brief Creates a smart iterator.
     *
     *  This templated constructor allows to create a new SharedIterator instance
     *  from any pointer to an iterator object @em it that is implicitly convertible
     *  to type @em T.
     *
     *  @warning If the iterator object pointed to by @e it is already owned by
     *  some other SharedIterator instance, the usage of @c this SharedIterator
     *  will result in undefined behavior.
     */
    template <typename U>
    explicit SharedIterator(U* it)
      : it_(it),
        useCount_(0)
    {
        if (it_) useCount_ = new long (1);
    }

    /*! @brief Creates a smart iterator which shares ownership of the iterator
     *  object managed by the @em other smart iterator.
     */
    SharedIterator(const SharedIterator& other)
      : it_(other.it_),
        useCount_(other.useCount_)
    {
        if (it_) ++(*useCount_);
    }

    /*! @brief Creates a smart iterator which shares ownership of the iterator
     *  object managed by the @em other smart iterator.
     *
     *  This templated copy-constructor allows to create new SharedIterator
     *  from any @em other SharedIterator who's iterator type is implicitly
     *  convertible to @em T.
     */
    template <typename U>
    SharedIterator(const SharedIterator<U>& other)
      : it_(other.it_),
        useCount_(other.useCount_)
    {
        if (it_) ++(*useCount_);
    }

    /// @brief Destroys the managed iterator if there are no more SharedIterator
    /// instances referring to it.
    ~SharedIterator()
    {
        destruct();
    }

    /// @brief Replaces the wrapped iterator object with the one managed by @em other.
    SharedIterator& operator=(const SharedIterator& other)
    {
        if (it_ == other.it_)
            return *this;
    
        destruct();

        it_ = other.it_;
        useCount_ = other.useCount_;
        ++(*useCount_);

        return *this;
    }

    /// @brief Compares two managed iterator objects.
    bool operator==(const SharedIterator& other)
    {
        return (*it_ == *other.it_);
    }

    /// @brief Compares two managed iterator objects.
    bool operator!=(const SharedIterator& other)
    {
        return (*it_ != *other.it_);
    }

    /// @brief Increments the managed iterator object.
    SharedIterator& operator++()
    {
        ++(*it_);
        return *this;
    }

    /// @brief Dereferences the managed iterator object.
    reference operator*() const { return (**it_); }

    /// @brief Dereferences the managed iterator object.
    pointer operator->() const { return &(**it_); }

    /// @brief Returns pointer to the managed iterator object.
    T* get() const { return it_; }

    /*! @brief Checks if @c this is the only SharedIterator instance managing
     *  the iterator object.
     *
     *  Retuns @c true if @c this is the only SharedIterator instance managing
     *  the current iterator object (i.e. useCount() == 1), otherwise returns
     *  @c false.
     */
    bool unique() const
    {
        return useCount_ && (*useCount_ == 1);
    }

    /*! @brief Returns the number of SharedIterator instances (including
     *  @c this) managing the same iterator object.
     */
    long useCount() const
    {
        return useCount_ ? *useCount_ : 0;
    }

private:
    void destruct()
    {
        if (it_ && (--(*useCount_) == 0))
        {
            delete it_;
            delete useCount_;
        }
    }

    T* it_;
    long* useCount_;
};

} // namespace odc

#endif // ODBLIB_SHAREDITERATOR_H_
