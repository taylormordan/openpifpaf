#include <algorithm>
#include <math.h>

#include "occupancy.hpp"


namespace openpifpaf {
namespace decoder {
namespace utils {


torch::Tensor Occupancy::forward(const torch::Tensor& x) {
    auto shape = {
        x.size(0),
        static_cast<int64_t>(x.size(1) / this->reduction) + 1,
        static_cast<int64_t>(x.size(2) / this->reduction) + 1,
    };
    return torch::zeros(shape, torch::dtype(torch::kUInt8).device(x.device()));
}


void Occupancy::set(torch::Tensor& o, double x, double y, double sigma) {
    if (this->reduction != 1.0) {
        x /= reduction;
        y /= reduction;
        sigma = fmax(this->min_scale_reduced, sigma / reduction);
    }

    auto minx = std::clamp(static_cast<int64_t>(x - sigma), int64_t(0), o.size(1) - 1);
    auto miny = std::clamp(static_cast<int64_t>(y - sigma), int64_t(0), o.size(0) - 1);
    // # +1: for non-inclusive boundary
    // # There is __not__ another plus one for rounding up:
    // # The query in occupancy does not round to nearest integer but only
    // # rounds down.
    auto maxx = std::clamp(static_cast<int64_t>(x + sigma), minx + 1, o.size(1));
    auto maxy = std::clamp(static_cast<int64_t>(y + sigma), miny + 1, o.size(0));
    o.index_put_({at::indexing::Slice(miny, maxy), at::indexing::Slice(minx, maxx)}, 1);
}


unsigned char Occupancy::get(const torch::Tensor& o, double x, double y) {
    return 0;
}


} // namespace utils
} // namespace decoder
} // namespace openpifpaf
