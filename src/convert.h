#pragma once

#include <json/value.h>

#include "model.h"

namespace taranis {
Json::Value to_json(const Location &location);
Json::Value to_json(const Condition &condition);
Json::Value to_json(const DailyCondition &condition);
Json::Value to_json(const Alert &alert);
Json::Value to_json(const HistoryItem &item);
Json::Value to_json(const Model &model);

void update_from_json(Location &location, const Json::Value &value);
void update_from_json(Condition &condition, const Json::Value &value);
void update_from_json(DailyCondition &condition, const Json::Value &value);
void update_from_json(Alert &alert, const Json::Value &value);
void update_from_json(HistoryItem &item, const Json::Value &value);
void update_from_json(Model &model, const Json::Value &value);
} // namespace taranis
