/** Perform checks possibly at compile time */
template <typename T>
bool
is_positive(const T &x)
{
    return x > T(0);
}


/** Always guaranteed to be positive */
template <typename T>
struct Positive
{
    const T &x;
    Positive(const T &x): x(x) {
        if (!(x > 0)) throw std::invalid_argument("x must be positive");
    }
};


template <typename T>
bool
is_positive(const Positive<T> &x)
{
    return true;
}
