#pragma once

#include <json/value.h>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "config.h"
#include "events.h"
#include "http.h"
#include "model.h"

namespace taranis {

struct Version {
  int major;
  int minor;
  int patch;
  std::string pre_release_identifiers;
};

bool operator==(const Version &lhs, const Version &rhs);
bool operator<(const Version &lhs, const Version &rhs);
bool operator<=(const Version &lhs, const Version &rhs);
bool operator>(const Version &lhs, const Version &rhs);
bool operator>=(const Version &lhs, const Version &rhs);

struct ReleaseDescription {
  std::string name;
  std::string tag_name;
  bool pre_release;
  std::string browser_url;
  std::string archive_url;
  std::string news_url;
};

class VersionChecker {
public:
  VersionChecker(std::shared_ptr<Config> config,
                 std::shared_ptr<HttpClient> client,
                 std::shared_ptr<Model> model);

  void check(CallContext context);

private:
  enum Action { browse = 1, skip = 2 };

  static constexpr int message_delay{5000};

  const std::string current_version_extended;

  std::shared_ptr<Config> config;
  std::shared_ptr<HttpClient> client;
  std::shared_ptr<Model> model;

  void do_check(bool interactive) const;

  void notify_user(const ReleaseDescription &release) const;

  Json::Value fetch_releases() const;

  std::vector<ReleaseDescription>
  extract_newer_releases(const Json::Value &value) const;

  std::string
  generate_new_version_question(const ReleaseDescription &release) const;

  std::string shorten_current_version_extended() const;

  static Version parse_possible_version_string(const std::string &target);
};

} // namespace taranis
