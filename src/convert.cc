#include "convert.h"
#include "experimental/optional"
#include "model.h"

using namespace std::chrono_literals;

namespace taranis {

// NAN is written as null by jsoncpp, this will fix deserialization

inline double deserialize_possible_null(const Json::Value &value) {
  if (value.isNull()) {
    return NAN;
  }
  return value.asDouble();
}

Json::Value to_json(const Location &location) {
  Json::Value value;
  value["longitude"] = location.longitude;
  value["latitude"] = location.latitude;
  value["name"] = location.name;
  value["country"] = location.country;
  value["state"] = location.state;

  return value;
}

Json::Value to_json(const Condition &condition) {
  Json::Value value;
  value["date"] = std::chrono::duration_cast<std::chrono::seconds>(
                      condition.date.time_since_epoch())
                      .count();
  value["sunrise"] = std::chrono::duration_cast<std::chrono::seconds>(
                         condition.sunrise.time_since_epoch())
                         .count();
  value["sunset"] = std::chrono::duration_cast<std::chrono::seconds>(
                        condition.sunset.time_since_epoch())
                        .count();
  value["temperature"] = condition.temperature;
  value["felt_temperature"] = condition.felt_temperature;
  value["pressure"] = condition.pressure;
  value["humidity"] = condition.humidity;
  value["uv_index"] = condition.uv_index;
  value["clouds"] = condition.clouds;
  value["visibility"] = condition.visibility;
  value["probability_of_precipitation"] =
      condition.probability_of_precipitation;
  value["wind_speed"] = condition.wind_speed;
  value["wind_degree"] = condition.wind_degree;
  value["wind_gust"] = condition.wind_gust;
  value["weather"] = condition.weather;
  value["weather_description"] = condition.weather_description;
  value["weather_icon_name"] = condition.weather_icon_name;
  value["weather_secondary_description"] =
      condition.weather_secondary_description;
  value["rain"] = condition.rain;
  value["snow"] = condition.snow;

  return value;
}

Json::Value to_json(const DailyCondition &condition) {
  Json::Value value;
  value["date"] = std::chrono::duration_cast<std::chrono::seconds>(
                      condition.date.time_since_epoch())
                      .count();
  value["sunrise"] = std::chrono::duration_cast<std::chrono::seconds>(
                         condition.sunrise.time_since_epoch())
                         .count();
  value["sunset"] = std::chrono::duration_cast<std::chrono::seconds>(
                        condition.sunset.time_since_epoch())
                        .count();
  value["moonrise"] = std::chrono::duration_cast<std::chrono::seconds>(
                          condition.moonrise.time_since_epoch())
                          .count();
  value["moonset"] = std::chrono::duration_cast<std::chrono::seconds>(
                         condition.moonset.time_since_epoch())
                         .count();
  value["moon_phase"] = condition.moon_phase;
  value["summary"] = condition.summary;
  value["pressure"] = condition.pressure;
  value["humidity"] = condition.humidity;
  value["dew_point"] = condition.dew_point;
  value["wind_speed"] = condition.wind_speed;
  value["wind_degree"] = condition.wind_degree;
  value["wind_gust"] = condition.wind_gust;
  value["clouds"] = condition.clouds;
  value["probability_of_precipitation"] =
      condition.probability_of_precipitation;
  value["uv_index"] = condition.uv_index;
  value["rain"] = condition.rain;
  value["snow"] = condition.snow;
  value["weather"] = condition.weather;
  value["weather_description"] = condition.weather_description;
  value["weather_icon_name"] = condition.weather_icon_name;
  value["weather_secondary_description"] =
      condition.weather_secondary_description;
  value["temperature_day"] = condition.temperature_day;
  value["temperature_min"] = condition.temperature_min;
  value["temperature_max"] = condition.temperature_max;
  value["temperature_night"] = condition.temperature_night;
  value["temperature_evening"] = condition.temperature_evening;
  value["temperature_morning"] = condition.temperature_morning;
  value["felt_temperature_day"] = condition.felt_temperature_day;
  value["felt_temperature_night"] = condition.felt_temperature_night;
  value["felt_temperature_evening"] = condition.felt_temperature_evening;
  value["felt_temperature_morning"] = condition.felt_temperature_morning;

  return value;
}

Json::Value to_json(const Alert &alert) {
  Json::Value value;
  value["sender"] = alert.sender;
  value["event"] = alert.event;
  value["start_date"] = std::chrono::duration_cast<std::chrono::seconds>(
                            alert.start_date.time_since_epoch())
                            .count();
  value["end_date"] = std::chrono::duration_cast<std::chrono::seconds>(
                          alert.end_date.time_since_epoch())
                          .count();
  value["description"] = alert.description;
  return value;
}

Json::Value to_json(const HistoryItem &item) {
  Json::Value value;
  value["location"] = to_json(item.location);
  value["favorite"] = item.favorite;

  return value;
}

Json::Value to_json(const Model &model) {
  Json::Value value;
  value["data_provider"] = model.data_provider;
  value["unit_system"] = model.unit_system;
  if (model.refresh_date == std::experimental::nullopt) {
    value["refresh_date"] = Json::Value::null;
  } else {
    value["refresh_date"] = std::chrono::duration_cast<std::chrono::seconds>(
                                model.refresh_date->time_since_epoch())
                                .count();
  }
  value["location"] = to_json(model.location);

  if (model.current_condition) {
    value["current_condition"] = to_json(*(model.current_condition));
  } else {
    value["current_condition"] = Json::Value::null;
  }
  for (const auto &forecast : model.hourly_forecast) {
    value["hourly_forecast"].append(to_json(forecast));
  }
  for (const auto &forecast : model.daily_forecast) {
    value["daily_forecast"].append(to_json(forecast));
  }
  for (const auto &alert : model.alerts) {
    value["alerts"].append(to_json(alert));
  }
  for (const auto &item : model.location_history) {
    value["location_history"].append(to_json(item));
  }
  return value;
}

void update_from_json(Location &location, const Json::Value &value) {
  location.longitude = deserialize_possible_null(value["longitude"]);
  location.latitude = deserialize_possible_null(value["latitude"]);
  location.name = value.get("name", "").asString();
  location.country = value.get("country", "").asString();
  location.state = value.get("state", "").asString();
}

void update_from_json(Condition &condition, const Json::Value &value) {
  condition.date = TimePoint{value.get("date", 0).asInt64() * 1s};
  condition.sunrise = TimePoint{value.get("sunrise", 0).asInt64() * 1s};
  condition.sunset = TimePoint{value.get("sunset", 0).asInt64() * 1s};
  condition.temperature = deserialize_possible_null(value["temperature"]);
  condition.felt_temperature =
      deserialize_possible_null(value["felt_temperature"]);
  condition.pressure = value.get("pressure", 0).asInt();
  condition.humidity = value.get("humidity", 0).asInt();
  condition.uv_index = deserialize_possible_null(value["uv_index"]);
  condition.clouds = value.get("clouds", 0).asInt();
  condition.visibility = value.get("visibility", 0).asInt();
  condition.probability_of_precipitation =
      deserialize_possible_null(value["probability_of_precipitation"]);
  condition.wind_speed = deserialize_possible_null(value["wind_speed"]);
  condition.wind_degree = value.get("wind_degree", 0).asInt();
  condition.wind_gust = deserialize_possible_null(value["wind_gust"]);
  condition.weather =
      static_cast<Weather>(value.get("weather", CLEAR_SKY).asInt());
  condition.weather_description =
      value.get("weather_description", "").asString();
  condition.weather_icon_name = value.get("weather_icon_name", "").asString();
  condition.weather_secondary_description =
      value.get("weather_secondary_description", "").asString();
  condition.rain = deserialize_possible_null(value["rain"]);
  condition.snow = deserialize_possible_null(value["snow"]);
}

void update_from_json(DailyCondition &condition, const Json::Value &value) {
  condition.date = TimePoint{value.get("date", 0).asInt64() * 1s};
  condition.sunrise = TimePoint{value.get("sunrise", 0).asInt64() * 1s};
  condition.sunset = TimePoint{value.get("sunset", 0).asInt64() * 1s};
  condition.moonrise = TimePoint{value.get("moonrise", 0).asInt64() * 1s};
  condition.moonset = TimePoint{value.get("moonset", 0).asInt64() * 1s};
  condition.moon_phase = deserialize_possible_null(value["moon_phase"]);
  condition.summary = value.get("summary", "").asString();
  condition.pressure = value.get("pressure", 0).asInt();
  condition.humidity = value.get("humidity", 0).asInt();
  condition.dew_point = deserialize_possible_null(value["dew_point"]);
  condition.wind_speed = deserialize_possible_null(value["wind_speed"]);
  condition.wind_degree = value.get("wind_degree", 0).asInt();
  condition.wind_gust = deserialize_possible_null(value["wind_gust"]);
  condition.clouds = value.get("clouds", 0).asInt();
  condition.probability_of_precipitation =
      deserialize_possible_null(value["probability_of_precipitation"]);
  condition.uv_index = deserialize_possible_null(value["uv_index"]);
  condition.rain = deserialize_possible_null(value["rain"]);
  condition.snow = deserialize_possible_null(value["snow"]);
  condition.weather =
      static_cast<Weather>(value.get("weather", CLEAR_SKY).asInt());
  condition.weather_description =
      value.get("weather_description", "").asString();
  condition.weather_icon_name = value.get("weather_icon_name", "").asString();
  condition.weather_secondary_description =
      value.get("weather_secondary_description", "").asString();
  condition.temperature_day =
      deserialize_possible_null(value["temperature_day"]);
  condition.temperature_min =
      deserialize_possible_null(value["temperature_min"]);
  condition.temperature_max =
      deserialize_possible_null(value["temperature_max"]);
  condition.temperature_night =
      deserialize_possible_null(value["temperature_night"]);
  condition.temperature_evening =
      deserialize_possible_null(value["temperature_evening"]);
  condition.temperature_morning =
      deserialize_possible_null(value["temperature_morning"]);
  condition.felt_temperature_day =
      deserialize_possible_null(value["felt_temperature_day"]);
  condition.felt_temperature_night =
      deserialize_possible_null(value["felt_temperature_night"]);
  condition.felt_temperature_evening =
      deserialize_possible_null(value["felt_temperature_evening"]);
  condition.felt_temperature_morning =
      deserialize_possible_null(value["felt_temperature_morning"]);
}

void update_from_json(Alert &alert, const Json::Value &value) {
  alert.sender = value.get("sender", "").asString();
  alert.event = value.get("event", "").asString();
  alert.start_date = TimePoint{value.get("start_date", 0).asInt64() * 1s};
  alert.end_date = TimePoint{value.get("end_date", 0).asInt64() * 1s};
  alert.description = value.get("description", "").asString();
}

void update_from_json(HistoryItem &item, const Json::Value &value) {
  auto &location = item.location;
  const auto &location_value = value["location"];
  update_from_json(location, location_value);

  item.favorite = value.get("favorite", false).asBool();
}

void update_from_json(Model &model, const Json::Value &value) {
  model.data_provider = static_cast<DataProvider>(
      value.get("data_provider", openweather).asInt());
  model.unit_system =
      static_cast<UnitSystem>(value.get("unit_system", standard).asInt());
  model.refresh_date = TimePoint{value.get("refresh_date", 0).asInt64() * 1s};

  auto &location = model.location;
  const auto &location_value = value["location"];
  update_from_json(location, location_value);

  if (value["current_condition"].isNull()) {
    model.current_condition = std::experimental::nullopt;
  } else {
    Condition current_condition{};
    const auto &current_condition_value = value["current_condition"];
    update_from_json(current_condition, current_condition_value);
    model.current_condition = current_condition;
  }
  for (const auto &condition_value : value["hourly_forecast"]) {
    Condition condition{};
    update_from_json(condition, condition_value);
    model.hourly_forecast.push_back(condition);
  }
  for (const auto &condition_value : value["daily_forecast"]) {
    DailyCondition condition{};
    update_from_json(condition, condition_value);
    model.daily_forecast.push_back(condition);
  }
  for (const auto &alert_value : value["alerts"]) {
    Alert alert{};
    update_from_json(alert, alert_value);
    model.alerts.push_back(alert);
  }
  for (const auto &item_value : value["location_history"]) {
    HistoryItem item{};
    update_from_json(item, item_value);
    model.location_history.push_back(item);
  }
}
} // namespace taranis
