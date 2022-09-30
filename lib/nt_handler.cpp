#include <ThunderDashboard/nt_handler.h>

using namespace frc1511;

extern "C" {
  struct HAL_ControlWord {
    uint32_t enabled : 1;
    uint32_t autonomous : 1;
    uint32_t test : 1;
    uint32_t eStop : 1;
    uint32_t fmsAttached : 1;
    uint32_t dsAttached : 1;
    uint32_t control_reserved : 26;
  };
  typedef struct HAL_ControlWord HAL_ControlWord;
}

NTHandler::NTHandler() = default;

NTHandler::~NTHandler() = default;

void NTHandler::init() {
  nt_inst = nt::NetworkTableInstance::GetDefault();
  sd_table = nt_inst.GetTable("SmartDashboard");
  fms_table = nt_inst.GetTable("FMSInfo");

  nt_inst.StartClientTeam(1511);
  nt_inst.StartDSClient();
}

void NTHandler::update() {
  for (auto& [key, value] : set_bools) {
    sd_table->PutBoolean(key, value);
  }
  for (auto& [key, value] : set_doubles) {
    sd_table->PutNumber(key, value);
  }
  for (auto& [key, value] : set_strings) {
    sd_table->PutString(key, value);
  }
}

NTHandler::MatchMode NTHandler::get_match_mode() {
  int32_t ctrl_word_int = get_ctrl_word();

  HAL_ControlWord ctrl_word;
  std::memcpy(&ctrl_word, &ctrl_word_int, sizeof(ctrl_word));

  if (!ctrl_word.enabled) {
    return MatchMode::DISABLED;
  }
  else if (ctrl_word.autonomous) {
    return MatchMode::AUTO;
  }
  else if (ctrl_word.test) {
    return MatchMode::TEST;
  }
  else {
    return MatchMode::TELEOP;
  }
}

NTHandler::Alliance NTHandler::get_alliance() {
  if (static_cast<bool>(fms_table->GetNumber("IsRedAlliance", true))) {
    return Alliance::RED;
  }
  else {
    return Alliance::BLUE;
  }
}

std::size_t NTHandler::get_station_number() {
  return static_cast<std::size_t>(fms_table->GetNumber("StationNumber", 1.0));
}

std::string NTHandler::get_event_name() {
  return fms_table->GetString("EventName", "");
}

std::string NTHandler::get_game_msg() {
  return fms_table->GetString("GameSpecificMessage", "");
}

std::size_t NTHandler::get_match_number() {
  return fms_table->GetNumber("MatchNumber", 0.0);
}

std::size_t NTHandler::get_replay_number() {
  return fms_table->GetNumber("ReplayNumber", 0.0);
}

void NTHandler::set_bool(const std::string& key, bool value) {
  set_bools.insert_or_assign(key, value);
  sd_table->PutBoolean(key, value);
}

void NTHandler::set_double(const std::string& key, double value) {
  set_doubles.insert_or_assign(key, value);
  sd_table->PutNumber(key, value);
}

void NTHandler::set_string(const std::string& key, std::string_view value) {
  set_strings.insert_or_assign(key, value);
  sd_table->PutString(key, value);
}

int32_t NTHandler::get_ctrl_word() {
  return static_cast<int32_t>(fms_table->GetNumber("FMSControlData", 0.0));
}

NTHandler NTHandler::instance;
