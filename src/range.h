#ifndef UTIL_LANG_RANGE_HPP
#define UTIL_LANG_RANGE_HPP

#include <iterator>
#include <type_traits>
#include <vector>
#include <random>
namespace util { 
namespace lang {

namespace detail {

template <typename T>
struct range_iter_base : std::iterator<std::input_iterator_tag, T> {
    range_iter_base(T current) : current(current) { }

    T operator *() const { return current; }

    T const* operator ->() const { return &current; }

    range_iter_base& operator ++() {
        ++current;
        return *this;
    }

    range_iter_base operator ++(int) {
        auto copy = *this;
        ++*this;
        return copy;
    }

    bool operator ==(range_iter_base const& other) const {
        return current == other.current;
    }

    bool operator !=(range_iter_base const& other) const {
        return not (*this == other);
    }

protected:
    T current;
};

} // namespace detail

template <typename T>
struct range_proxy {
    struct iter : detail::range_iter_base<T> {
        iter(T current) : detail::range_iter_base<T>(current) { }
    };

    struct step_range_proxy {
        struct iter : detail::range_iter_base<T> {
            iter(T current, T step)
                : detail::range_iter_base<T>(current), step(step) { }

            using detail::range_iter_base<T>::current;

            iter& operator ++() {
                current += step;
                return *this;
            }

            iter operator ++(int) {
                auto copy = *this;
                ++*this;
                return copy;
            }

            // Loses commutativity. Iterator-based ranges are simply broken. :-(
            bool operator ==(iter const& other) const {
                return step > 0 ? current >= other.current
                                : current < other.current;
            }

            bool operator !=(iter const& other) const {
                return not (*this == other);
            }

        private:
            T step;
        };

        step_range_proxy(T begin, T end, T step)
            : begin_(begin, step), end_(end, step) { }

        iter begin() const { return begin_; }

        iter end() const { return end_; }

    private:
        iter begin_;
        iter end_;
    };

    range_proxy(T begin, T end) : begin_(begin), end_(end) { }

    step_range_proxy step(T step) {
        return {*begin_, *end_, step};
    }

    iter begin() const { return begin_; }

    iter end() const { return end_; }

private:
    iter begin_;
    iter end_;
};

template <typename T>
struct infinite_range_proxy {
    struct iter : detail::range_iter_base<T> {
        iter(T current = T()) : detail::range_iter_base<T>(current) { }

        bool operator ==(iter const&) const { return false; }

        bool operator !=(iter const&) const { return true; }
    };

    struct step_range_proxy {
        struct iter : detail::range_iter_base<T> {
            iter(T current = T(), T step = T())
                : detail::range_iter_base<T>(current), step(step) { }

            using detail::range_iter_base<T>::current;

            iter& operator ++() {
                current += step;
                return *this;
            }

            iter operator ++(int) {
                auto copy = *this;
                ++*this;
                return copy;
            }

            bool operator ==(iter const&) const { return false; }

            bool operator !=(iter const&) const { return true; }

        private:
            T step;
        };

        step_range_proxy(T begin, T step) : begin_(begin, step) { }

        iter begin() const { return begin_; }

        iter end() const { return  iter(); }

    private:
        iter begin_;
    };

    infinite_range_proxy(T begin) : begin_(begin) { }

    step_range_proxy step(T step) {
        return step_range_proxy(*begin_, step);
    }

    iter begin() const { return begin_; }

    iter end() const { return iter(); }

private:
    iter begin_;
};

template <typename T>
range_proxy<T> range(T begin, T end) {
    if(end < begin) end=begin;
    return {begin, end};
}


template <typename T>
range_proxy<T> range(T end) {
    return {0,end};
}

namespace traits {

template <typename C>
struct has_size {
    template <typename T>
    static constexpr auto check(T*) ->
        typename std::is_integral<
            decltype(std::declval<T const>().size())>::type;

    template <typename>
    static constexpr auto check(...) -> std::false_type;

    using type = decltype(check<C>(0));
    static constexpr bool value = type::value;
};

} // namespace traits

template <typename C, typename = typename std::enable_if<traits::has_size<C>::value>>
auto indices(C const& cont) -> range_proxy<decltype(cont.size())> {
    return {0, cont.size()};
}

template <typename T, std::size_t N>
range_proxy<std::size_t> indices(T (&)[N]) {
    return {0, N};
}

template <typename T>
range_proxy<typename std::initializer_list<T>::size_type>
indices(std::initializer_list<T>&& cont) {
    return {0, cont.size()};
}

} } // namespace util::lang



template<typename T>
using vector2 = std::vector<std::vector<T>>;

template<typename T>
using vector3 = std::vector<vector2<T>>;

template<typename T>
using vector4 = std::vector<vector3<T>>;

template<typename T>
inline vector2<T> make_vector2(int dim1, int dim2, T val) {
  return vector2<T>(dim1,std::vector<T>(dim2, val));
}

template<typename T>
inline vector3<T> make_vector3(int dim1, int dim2, int dim3, T val) {
  return vector3<T>(dim1,make_vector2<T>(dim2, dim3, val));
}

template<typename T>
inline vector4<T> make_vector4(int dim1, int dim2, int dim3, int dim4, T val) {
  return vector4<T>(dim1,make_vector3<T>(dim2, dim3, dim4, val));
}

template<typename T>
inline vector2<T> make_vector2(int dim1, int dim2) {
  return vector2<T>(dim1,std::vector<T>(dim2));
}

template<typename T>
inline vector3<T> make_vector3(int dim1, int dim2, int dim3) {
  return vector3<T>(dim1,make_vector2<T>(dim2, dim3));
}

template<typename T>
inline std::vector<T> make_vector(int dim1, T val) {
  return std::vector<T>(dim1, val);
}

// To_string
inline void stream0(std::ostringstream& ss) {}

template<typename T0, typename... T>
inline void stream0(std::ostringstream& ss, T0 v, T... val)
{	ss << v;	stream0(ss, val...);}

template<typename... T>
inline std::string to_string(T... val) {
	std::ostringstream ss; stream0(ss, val...);
	return ss.str();
}


inline std::vector<int> operator==(std::vector<int> x, std::vector<int> y)
{
	std::vector<int> tmp(x.size());
	for (unsigned int i = 0; i < x.size(); i++)
		tmp[i] = x[i] == y[i];
	return tmp;
}


inline std::vector<int> operator==(std::vector<int> x, int y)
{
	std::vector<int> tmp(x.size());
	for (unsigned int i = 0; i < x.size(); i++)
		tmp[i] = x[i] == y;
	return tmp;
}


inline std::vector<int> operator!=(std::vector<int> x, int y)
{
	std::vector<int> tmp(x.size());
	for (unsigned int i = 0; i < x.size(); i++)
		tmp[i] = x[i] != y;
	return tmp;
}

inline std::vector<int> operator&&(std::vector<int> x, std::vector<int> y)
{
	std::vector<int> tmp(x.size());
	for (unsigned int i = 0; i < x.size(); i++)
		tmp[i] = x[i] && y[i];
	return tmp;
}

inline std::vector<int> operator||(std::vector<int> x, std::vector<int> y)
{
	std::vector<int> tmp(x.size());
	for (unsigned int i = 0; i < x.size(); i++)
		tmp[i] = x[i] && y[i];
	return tmp;
}


inline std::vector<int> operator>(std::vector<int> x, std::vector<int> y)
{
	std::vector<int> tmp(x.size());
	for (unsigned int i = 0; i < x.size(); i++)
		tmp[i] = x[i] > y[i];
	return tmp;
}


inline std::vector<int> operator>=(std::vector<int> x, std::vector<int> y)
{
	std::vector<int> tmp(x.size());
	for (unsigned int i = 0; i < x.size(); i++)
		tmp[i] = x[i] >= y[i];
	return tmp;
}



class Alea {
public:
  double seed;
  static std::mt19937& gen() {
    static std::mt19937 gen0 = std::mt19937(0.0); 
    return gen0;}
  static std::uniform_real_distribution<double> unif() {
    static std::uniform_real_distribution<double> unif0 = std::uniform_real_distribution<double>(0.0,1.0);
    return unif0;
  }
  static double alea() 
  {
    return unif()(Alea::gen());
  }
};


inline void alea_reset(double seed)
{
  Alea::gen() = std::mt19937(seed);
}

inline double alea()
{
  return Alea::alea();
}


inline std::vector<double> alea(unsigned int size)
{
    std::vector<double> val(size);
    for(unsigned int i = 0; i < size; i++)
      val[i] = Alea::alea();
    return val;
}

template<typename T>
inline void alea_shuffle(T& vect) {
    shuffle(begin(vect),end(vect), Alea::gen());
}


#endif // ndef UTIL_LANG_RANGE_HPP


// Destinie 2
// Copyright © 2005-2018, Institut national de la statistique et des études économiques
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.