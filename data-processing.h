// Copyright (c) 2013 Craig Henderson
// Part of the Data Processing Library
// https://github.com/cdmh/dataproc

#pragma once

// standard header files
#include <cstdint>          // std::uint8_t
#include <vector>
#include <iosfwd>           // basic_ostream
#include <functional>       // std::function
#include <locale>
#include <cassert>

// project "system" header files
#include "memmap.h"

#include "data_processing.detail.h"

namespace cdmh {
namespace data_processing {

template<typename T, typename U>
inline
std::vector<T> split_string(U const &string, char const delim)
{
    std::vector<T> result;

    auto it  = string.cbegin();
    auto ite = string.cend();
    while (detail::ltrim(it,ite) != ite)
    {
        auto sp = std::find_if(it, ite, [delim](char ch) { return ch == delim; });
        result.push_back(atol(std::string(it,sp).c_str()));
        it = sp;
    }
    return result;
}

// a string type of a string of characters
// represented by a pair of iterators
class string_view
{
  public:
    string_view(char const *begin,char const *end): begin_(begin),end_(end)
    { }

    char const *begin() const { return begin_; }
    char const *end()   const { return end_; }

    size_t const length() const
    {
        return std::distance(begin_, end_);
    }

  private:
    char const *begin_;
    char const *end_;
};

inline
bool const operator==(string_view const &first, char const *second)
{
    auto const len1 = first.length();
    auto const len2 = strlen(second);
    if (len1 != len2)
        return false;

    return (strcmp(first.begin(), second) == 0);
}

template<typename E, typename T>
inline
std::basic_ostream<E, T> &operator<<(std::basic_ostream<E, T> &o, string_view const &str)
{
    std::copy(str.begin(), str.end(), std::ostream_iterator<char>(o));
    return o;
}

//bool const operator==(string_view const &first, string_view const &second);

}   // namespace data_processing
}   // namespace cdmh


namespace {

static cdmh::data_processing::type_mask_t const string_type  = 1;
static cdmh::data_processing::type_mask_t const double_type  = 1 << 1;
static cdmh::data_processing::type_mask_t const integer_type = 1 << 2;
static cdmh::data_processing::type_mask_t const null_type    = 1 << 3;

}   // anonymous namespace

// project header files
#include "dataset.h"
#include "delimited_data.h"
#include "mapped_csv.h"

#include "dataset.impl.h"
