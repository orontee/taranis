#include "version.h"

#include <algorithm>
#include <boost/log/trivial.hpp>
#include <functional>
#include <inkview.h>
#include <regex>
#include <sstream>
#include <stdexcept>

#include "errors.h"

namespace taranis {

extern const std::string version;
// initialized in automatically generated file about.cc

bool operator==(const Version &lhs, const Version &rhs) {
  return (lhs.major == rhs.major and lhs.minor == rhs.minor and
          lhs.patch == rhs.patch and
          lhs.pre_release_identifiers == rhs.pre_release_identifiers);
}

bool operator<(const Version &lhs, const Version &rhs) {
  if (lhs.major < rhs.major) {
    return true;
  } else if (lhs.major > rhs.major) {
    return false;
  }
  // lhs.major == rhs.major
  if (lhs.minor < rhs.minor) {
    return true;
  } else if (lhs.minor > rhs.minor) {
    return false;
  }
  // lhs.minor == rhs.minor
  if (lhs.patch < rhs.patch) {
    return true;
  } else if (lhs.patch > rhs.patch) {
    return false;
  }
  // lhs.patch == rhs.patch
  return lhs.pre_release_identifiers < rhs.pre_release_identifiers;
}

bool operator<=(const Version &lhs, const Version &rhs) {
  return lhs == rhs or lhs < rhs;
}

bool operator>(const Version &lhs, const Version &rhs) { return (rhs < lhs); }

bool operator>=(const Version &lhs, const Version &rhs) { return rhs <= lhs; }

namespace github {
const std::string base_url{"https://api.github.com"};
const std::string release_path{"/repos/orontee/taranis/releases"};
const std::string download_path{"/repos/orontee/taranis/releases/assets"};
const std::string news_file_base_url{"https://github.com/orontee/taranis/raw"};
} // namespace github

VersionChecker::VersionChecker(std::shared_ptr<Config> config,
                               std::shared_ptr<HttpClient> client,
                               std::shared_ptr<Model> model)
    : current_version_extended{version}, config{config}, client{client},
      model{model} {}

void VersionChecker::check(CallContext context) {
  BOOST_LOG_TRIVIAL(debug) << "Checking application version";

  switch (context) {
  case CallContext::triggered_by_application_startup:
  case CallContext::triggered_by_configuration_change:
  case CallContext::triggered_by_model_change:
  case CallContext::triggered_by_timer:
    this->do_check(false);
    break;
  case CallContext::triggered_by_user:
    this->do_check(true);
    break;
  }
}

void VersionChecker::do_check(bool interactive) const {
  Json::Value returned_value;

  std::string error_message = GetLangText(
      "Sorry, an unexpected error was encountered. Report this to the "
      "application author.");

  try {
    returned_value = this->fetch_releases();
  } catch (const HttpError &error) {
    BOOST_LOG_TRIVIAL(warning) << "Failed to fetch releases " << error.what();
    if (interactive) {
      if (error.code >= 500) {
        error_message = GetLangText(
            "The service hosting releases isn't working as expected. "
            "Retry later.");
      }
      Message(ICON_ERROR, GetLangText("Software version"),
              error_message.c_str(), VersionChecker::message_delay);
    }
    return;
  } catch (const JsonParseError &error) {
    BOOST_LOG_TRIVIAL(warning) << "Failed to parse releases " << error.what();
    if (interactive) {
      Message(ICON_ERROR, GetLangText("Software version"),
              error_message.c_str(), VersionChecker::message_delay);
    }
    return;
  }
  if (not returned_value.isArray()) {
    if (interactive) {
      Message(ICON_ERROR, GetLangText("Software version"),
              error_message.c_str(), VersionChecker::message_delay);
    }
  }
  const auto newer_releases = this->extract_newer_releases(returned_value);
  if (newer_releases.empty()) {
    BOOST_LOG_TRIVIAL(debug) << "Latest release already installed";

    Message(ICON_INFORMATION, GetLangText("Software version"),
            GetLangText("Latest release already installed"),
            VersionChecker::message_delay);
    return;
  }
  const auto latest_release = newer_releases.front();
  this->notify_user(latest_release);
}

void VersionChecker::notify_user(const ReleaseDescription &release) const {
  const auto question = this->generate_new_version_question(release);
  const Action user_choice = static_cast<Action>(DialogSynchro(
      ICON_QUESTION, GetLangText("Software version"), question.c_str(),
      GetLangText("Browse"), GetLangText("Ok"), nullptr));

  switch (user_choice) {
  case browse: {
    BOOST_LOG_TRIVIAL(debug) << "Opening URL " << release.browser_url;
    const char *const url = release.news_url.c_str();
    const auto browser_status =
        OpenTask(BROWSER_FOR_AUTH, 1, &url, TASK_MAKEACTIVE);
    BOOST_LOG_TRIVIAL(debug)
        << "Browser task status " << std::to_string(browser_status);
    break;
  }
  case skip:
    BOOST_LOG_TRIVIAL(debug) << "Skipping software check";
    break;
  default:
    BOOST_LOG_TRIVIAL(warning) << "Unhandled case " << user_choice;
  }
}

Json::Value VersionChecker::fetch_releases() const {
  BOOST_LOG_TRIVIAL(debug) << "Fetching releases";
  return this->client->get(github::base_url + github::release_path);
  // default to 30 returned releases
}

const std::string archive_asset_name{"artifact.zip"};

std::vector<ReleaseDescription>
VersionChecker::extract_newer_releases(const Json::Value &value) const {
  std::vector<ReleaseDescription> releases;

  Version parsed_current_version;
  try {
    parsed_current_version = this->parse_possible_version_string(
        this->shorten_current_version_extended());
  } catch (const std::runtime_error &error) {
    BOOST_LOG_TRIVIAL(error)
        << "Failed to parse current version string!" << error.what();
    return {};
  }

  const auto keep_pre_release =
      this->config->read_bool("version_check_keep_pre_releases", false);
  BOOST_LOG_TRIVIAL(debug)
      << "Extraction of releases newer than current version "
      << (keep_pre_release ? " with " : " without ") << "pre-releases";

  for (auto release_value : value) {
    const auto name = release_value.get("name", "").asString();
    const auto tag_name = release_value.get("tag_name", "").asString();
    const auto pre_release = release_value.get("prerelease", false).asBool();
    const auto browser_url = release_value.get("html_url", "").asString();

    if (tag_name.empty() or tag_name.at(0) != 'v') {
      BOOST_LOG_TRIVIAL(debug)
          << "Skipping release with tag name not starting with letter v "
          << name;
      continue;
    }
    if (not keep_pre_release and pre_release) {
      BOOST_LOG_TRIVIAL(debug) << "Skipping pre-release " << name;
      continue;
    }
    if (release_value["assets"].empty()) {
      BOOST_LOG_TRIVIAL(debug) << "Skipping release without assets " << name;
      continue;
    }

    try {
      const auto parsed_version = this->parse_possible_version_string(tag_name);
      if (parsed_version <= parsed_current_version) {
        BOOST_LOG_TRIVIAL(debug) << "Skipping lower release " << name;
        continue;
      }

    } catch (const std::runtime_error &error) {
      BOOST_LOG_TRIVIAL(error)
          << "Failed to parse release tag string!" << error.what();
      continue;
    }

    ReleaseDescription release{name, tag_name, pre_release, browser_url};

    for (auto asset_value : release_value["assets"]) {
      if (asset_value.get("name", "") != archive_asset_name) {
        continue;
      }
      const auto asset_id = asset_value.get("id", "").asString();
      if (asset_value.get("state", "") != "uploaded") {
        BOOST_LOG_TRIVIAL(debug)
            << "Skipping release " << name << " since asset not uploaded";
        break;
      }
      release.archive_url =
          github::base_url + github::download_path + "/" + asset_id;

      release.news_url =
          github::news_file_base_url + "/" + tag_name + "/NEWS.md";

      releases.push_back(release);
      break;
    }
  }
  BOOST_LOG_TRIVIAL(debug) << "Kept releases: " << releases.size() << "/"
                           << value.size();

  std::sort(releases.begin(), releases.end(),
            [](const ReleaseDescription &lhs, const ReleaseDescription &rhs) {
              return parse_possible_version_string(lhs.tag_name) >
                     parse_possible_version_string(rhs.tag_name);
            });

  return releases;
}

Version
VersionChecker::parse_possible_version_string(const std::string &target) {
  const std::regex pattern("v?([0-9]+)\\.([0-9]+)\\.([0-9]+)(-([a-z0-9]+))?");
  std::smatch match;
  if (not std::regex_match(target, match, pattern)) {
    throw std::runtime_error{"Failed to parse possible version string " +
                             target};
  }
  size_t converted_length{0};
  const int major = std::stoi(match[1], &converted_length);
  if (converted_length != match[1].length()) {
    throw std::runtime_error{"Failed to process match group 1 " +
                             match[1].str()};
  }
  const int minor = std::stoi(match[2], &converted_length);
  if (converted_length != match[2].length()) {
    throw std::runtime_error{"Failed to process match group 2 " +
                             match[2].str()};
  }
  const int patch = std::stoi(match[3], &converted_length);
  if (converted_length != match[3].length()) {
    throw std::runtime_error{"Failed to process match group 3 " +
                             match[3].str()};
  }

  return {major, minor, patch, match[4]};
}

std::string VersionChecker::shorten_current_version_extended() const {
  std::stringstream to_parse{this->current_version_extended};
  std::string token;
  std::getline(to_parse, token, ' ');
  return token;
}

std::string VersionChecker::generate_new_version_question(
    const ReleaseDescription &release) const {
  std::stringstream question;
  question << GetLangText("A new version is available: ") << release.name
           << ".";

  return question.str();
}
} // namespace taranis
