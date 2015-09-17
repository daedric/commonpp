/*
 * File: include/commonpp/core/RandomValuePicker.hpp
 * Part of commonpp.
 *
 * Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
 * project root).
 *
 * Copyright (c) 2015 Thomas Sanchez.  All rights reserved.
 *
 */
#pragma once

#include <cassert>
#include <iterator>
#include <random>
#include <memory>

#include <boost/config.hpp>

namespace commonpp
{

template <typename Collection, typename Value>
struct RandomValuePicker
{
    RandomValuePicker()
    : collection(nullptr)
    , rd(std::make_shared<std::random_device>())
    , gen(rd->operator()())
    , distribution(0, 1)
    {}

    RandomValuePicker(const Collection& coll)
    : collection(std::addressof(coll))
    , rd(std::make_shared<std::random_device>())
    , gen(rd->operator()())
    , distribution(0, std::distance(std::begin(coll), std::end(coll)) - 1)
    {}

    const Value& operator()()
    {
        if (BOOST_UNLIKELY(!collection))
        {
            throw std::runtime_error(
                "a non initialized RandomValuePicked is used");
        }
        auto it = std::begin(*collection);
        std::advance(it, distribution(gen));
        return *it;
    }

    const Collection* collection;
    std::shared_ptr<std::random_device> rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> distribution;
};

template <typename Collection>
static auto createPicker(const Collection& coll)
    -> RandomValuePicker<Collection, typename Collection::value_type>
{
    return {coll};
}

} // namespace commonpp
