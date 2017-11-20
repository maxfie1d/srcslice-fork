#pragma once

// mapとか関数型っぽいことをする機能を集めたもの


#include <functional>
#include <algorithm>

template<typename T1, typename T2>
std::vector<T2> vec_transform(std::vector<T1> vec, std::function<T2(T1)> map) {
    std::vector<T2> out;
    std::transform(std::begin(vec), std::end(vec), std::back_inserter(out), map);
    return out;
}

template<typename T>
std::vector<T> set_to_vector(std::set<T> set) {
    auto vec = std::vector<T>(std::begin(set), std::end(set));
    return vec;
}

template<typename T1, typename T2>
std::set<T2> set_transform(std::set<T1> set, std::function<T2(T1)> map) {
    std::set<T2> out;
    std::transform(std::begin(set), std::end(set), std::inserter(out, std::begin(out)), map);
    return out;
};
