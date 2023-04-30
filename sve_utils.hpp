#pragma once

namespace sve {

// hash from: https://stackoverflow.com/a/57595105
// hash combine for std::hash
template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
    seed ^= std::hash<T>{}(v) + (seed << 6) + (seed >> 2);  // NOLINT(hicpp-signed-bitwise)
    (hashCombine(seed, rest), ...);                         // NOLINT(hicpp-use-emplace,modernize-use-transparent-functors)
};

}  // namespace sve