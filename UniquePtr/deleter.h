template <class T, bool b = std::is_array_v<T>>
struct Slug {};

template <class T>
struct Slug<T, false> {
    Slug() = default;

    template <class A>
    Slug(A&&) noexcept {
    }

    //    template<class A>
    //    Slug& operator=(A&&) noexcept{}

    ~Slug() = default;
    void operator()(T* ptr) {
        if (ptr != nullptr) {
            delete ptr;
        }
    }
};

template <class T>
struct Slug<T[], true> {
    Slug() = default;

    template <class A>
    Slug(A&&) noexcept {
    }

    //    template<class A>
    //    Slug& operator=(A&&) noexcept{}

    ~Slug() = default;

    void operator()(T* ptr) {
        if (ptr != nullptr) {
            delete[] ptr;
        }
    }
};
