#include "icons.h"

#include <cstring>
#include <map>
#include <memory>
// #include <opencv2/imgproc.hpp>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/opencv.hpp>
#include <tuple>

namespace taranis {

typedef std::tuple<std::string, int, int> CacheKey;

static const std::vector<unsigned char> empty_data = {};

std::map<CacheKey, std::vector<unsigned char>> bitmap_data_cache;

const std::vector<unsigned char> &Icons::rotate_icon(const std::string &name,
                                                     int size, int degree) {
  const auto key = std::make_tuple(name, size, degree);
  const auto found = bitmap_data_cache.find(key);
  if (found != bitmap_data_cache.end()) {
    return found->second;
  }
  const auto *const original_bitmap = this->get(name);
  if (!original_bitmap) {
    return empty_data;
  }
  auto *const bitmap_to_rotate =
      BitmapStretchProportionally(original_bitmap, size, size);

  const auto header_size = offsetof(ibitmap, data);
  const auto data_size = bitmap_to_rotate->scanline * bitmap_to_rotate->height;

  auto &rotated_bitmap_data = bitmap_data_cache[key];
  rotated_bitmap_data.resize(header_size + data_size);

  // if (degree == 0) {

    std::memcpy(rotated_bitmap_data.data(), bitmap_to_rotate,
                header_size + data_size);

  // } else {

  //   const cv::Mat image{bitmap_to_rotate->height, bitmap_to_rotate->width,
  //                       CV_8UC1, bitmap_to_rotate->data};
  //   const cv::Point2f center{
  //       static_cast<float>((bitmap_to_rotate->width - 1) / 2.0),
  //       static_cast<float>((bitmap_to_rotate->height - 1) / 2.0)};
  //   const cv::Mat rotation_matrix =
  //       cv::getRotationMatrix2D(center, degree, 1.0);
  //   cv::Mat rotated_image{bitmap_to_rotate->height, bitmap_to_rotate->width,
  //                         CV_8UC1};
  //   cv::warpAffine(image, rotated_image, rotation_matrix, image.size(),
  //                  cv::INTER_LINEAR, cv::BORDER_CONSTANT, 0xFF);

  //   std::memcpy(rotated_bitmap_data.data(), bitmap_to_rotate, header_size);
  //   // headers are the same for both bitmap

  //   std::memcpy(rotated_bitmap_data.data() + header_size, rotated_image.data,
  //               data_size);
  // }

  return rotated_bitmap_data;
}
} // namespace taranis
