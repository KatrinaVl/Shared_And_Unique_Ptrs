#pragma once

#include <type_traits>
#include <string>

template <class T_1, std::string& str, bool b_1 = std::is_empty_v<T_1> && !std::is_final_v<T_1>>
class CompressedPairIn {
public:
    CompressedPairIn() = default;

private:
    T_1 t_{};
};

template <class T_1, std::string& str>
class CompressedPairIn<T_1, str, true> : public T_1 {
public:
    CompressedPairIn() = default;

    template <class A>
    CompressedPairIn(A&& f) : T_1(std::forward<A>(f)) {
    }

    T_1& Get() {
        return *this;
    }

    const T_1& Get() const {
        return *this;
    }
};

template <class T_1, std::string& str>
class CompressedPairIn<T_1, str, false> {
public:
    CompressedPairIn() = default;

    template <class A>
    CompressedPairIn(A&& f) : t_(std::forward<A>(f)) {
    }

    T_1& Get() {
        return t_;
    }

    const T_1& Get() const {
        return t_;
    }

private:
    T_1 t_{};
};

std::string first = "first";
std::string second = "second";

// Me think, why waste time write lot code, when few code do trick.
template <typename F, typename S>
class CompressedPair : CompressedPairIn<F, first>, CompressedPairIn<S, second> {
public:
    CompressedPair() = default;

    template <class A, class B>
    CompressedPair(A&& f, B&& s)
        : CompressedPairIn<F, first>(std::forward<A>(f)),
          CompressedPairIn<S, second>(std::forward<B>(s)) {
    }

    F& GetFirst() {
        return CompressedPairIn<F, first>::Get();
    }

    S& GetSecond() {
        return CompressedPairIn<S, second>::Get();
    };

    const F& GetFirst() const {
        return CompressedPairIn<F, first>::Get();
    }

    const S& GetSecond() const {
        return CompressedPairIn<S, second>::Get();
    }
};
