#include "icons.h"

#include <cstring>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace taranis {

std::vector<unsigned char> Icons::rotate_icon(ibitmap *icon_to_rotate,
                                              int degree) {
  const auto arrow_angle = (180 - degree);
  // The parameter degree is an angle measure in degrees, interpreted
  // as the direction where the wind is blowing FROM (0 means North,
  // 90 East), but the icon arrow must show where the wind is blowing
  // TO. Whats more OpenCV rotation is counter-clockwise for positive
  // angle values.
  if (!icon_to_rotate) {
    return {};
  }
  const cv::Mat image{icon_to_rotate->height, icon_to_rotate->width, CV_8UC1,
                      icon_to_rotate->data};
  const cv::Point2f center{
      static_cast<float>((icon_to_rotate->width - 1) / 2.0),
      static_cast<float>((icon_to_rotate->height - 1) / 2.0)};
  const cv::Mat rotation_matrix =
      cv::getRotationMatrix2D(center, arrow_angle, 1.0);
  cv::Mat rotated_image{icon_to_rotate->height, icon_to_rotate->width, CV_8UC1};
  cv::warpAffine(image, rotated_image, rotation_matrix, image.size(),
                 cv::INTER_LINEAR, cv::BORDER_CONSTANT, 0xFF);

  const auto header_size = offsetof(ibitmap, data);
  const auto data_size = icon_to_rotate->scanline * icon_to_rotate->height;

  std::vector<unsigned char> rotated_bitmap_data;
  rotated_bitmap_data.resize(header_size + data_size);

  std::memcpy(rotated_bitmap_data.data(), icon_to_rotate, header_size);
  // headers are the same for both bitmap

  std::memcpy(rotated_bitmap_data.data() + header_size, rotated_image.data,
              data_size);

  return rotated_bitmap_data;
}
} // namespace taranis
