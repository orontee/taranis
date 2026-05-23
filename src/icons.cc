#include "icons.h"

#include <cstring>
#include <memory>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace taranis {

const ibitmap *Icons::stretch_icon(const std::string &name, int size) {
  auto stretched_icon_data = this->do_stretch_icon(name, size);
  if (stretched_icon_data and not stretched_icon_data->empty()) {
    return reinterpret_cast<const ibitmap *>(stretched_icon_data->data());
  }
  BOOST_LOG_TRIVIAL(warning) << "Failed to stretch bitmap " << name;
  return nullptr;
}

const ibitmap *Icons::rotate_icon(const std::string &name, int size,
                                  int degree) {
  auto rotated_icon_data = this->do_rotate_icon(name, size, degree);
  if (rotated_icon_data and not rotated_icon_data->empty()) {
    return reinterpret_cast<const ibitmap *>(rotated_icon_data->data());
  }
  BOOST_LOG_TRIVIAL(warning) << "Failed to rotate bitmap " << name;
  return nullptr;
}

const std::vector<unsigned char> *
Icons::do_stretch_icon(const std::string &name, int size) {
  const auto key = std::make_tuple(name, size, 0);
  const auto found = this->bitmap_data_cache.find(key);
  if (found != this->bitmap_data_cache.end()) {
    return &(found->second);
  }
  const auto *const original_bitmap = this->get(name);
  if (!original_bitmap) {
    return &(this->empty_data);
  }

  const cv::Mat source{original_bitmap->height, original_bitmap->width, CV_8UC1,
                       const_cast<unsigned char *>(original_bitmap->data)};
  cv::Mat resized;
  cv::resize(source, resized, cv::Size(size, size), 0, 0, cv::INTER_LINEAR);

  const auto header_size = offsetof(ibitmap, data);
  const int scanline = static_cast<int>(resized.step[0]);
  const auto data_size = static_cast<size_t>(scanline) * size;

  auto &entry = this->bitmap_data_cache[key];
  entry.resize(header_size + data_size);

  std::memcpy(entry.data(), original_bitmap, header_size);
  auto *hdr = reinterpret_cast<ibitmap *>(entry.data());
  hdr->width = size;
  hdr->height = size;
  hdr->scanline = scanline;
  // depth is inherited from icon

  std::memcpy(entry.data() + header_size, resized.data, data_size);

  return &entry;
}

const std::vector<unsigned char> *Icons::do_rotate_icon(const std::string &name,
                                                        int size, int degree) {
  if (degree == 0) {
    return this->do_stretch_icon(name, size);
  }
  const auto key = std::make_tuple(name, size, degree);
  const auto found = this->bitmap_data_cache.find(key);
  if (found != this->bitmap_data_cache.end()) {
    return &(found->second);
  }
  const auto *const original_bitmap = this->get(name);
  if (!original_bitmap) {
    return &(this->empty_data);
  }
  auto *bitmap_to_rotate = this->stretch_icon(name, size);
  if (!bitmap_to_rotate) {
    return &(this->empty_data);
  }
  const auto header_size = offsetof(ibitmap, data);
  const auto data_size = bitmap_to_rotate->scanline * bitmap_to_rotate->height;

  auto &rotated_bitmap_data = this->bitmap_data_cache[key];
  rotated_bitmap_data.resize(header_size + data_size);

  const cv::Mat image{bitmap_to_rotate->height, bitmap_to_rotate->width,
                      CV_8UC1,
                      reinterpret_cast<void *>(
                          const_cast<unsigned char *>(bitmap_to_rotate->data))};
  const cv::Point2f center{
      static_cast<float>((bitmap_to_rotate->width - 1) / 2.0),
      static_cast<float>((bitmap_to_rotate->height - 1) / 2.0)};
  const cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, degree, 1.0);
  cv::Mat rotated_image{bitmap_to_rotate->height, bitmap_to_rotate->width,
                        CV_8UC1};
  cv::warpAffine(image, rotated_image, rotation_matrix, image.size(),
                 cv::INTER_LINEAR, cv::BORDER_CONSTANT, 0xFF);

  std::memcpy(rotated_bitmap_data.data(), bitmap_to_rotate, header_size);
  // headers are the same for both bitmap

  std::memcpy(rotated_bitmap_data.data() + header_size, rotated_image.data,
              data_size);

  return &(rotated_bitmap_data);
}
} // namespace taranis
