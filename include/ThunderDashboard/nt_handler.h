#pragma once

#include <ThunderDashboard/dashboard.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>

namespace frc1511 {

class NTHandler {
public:
  static NTHandler* get() {
    return &instance;
  }

  NTHandler(NTHandler const&) = delete;
  NTHandler& operator=(NTHandler const&) = delete;

  enum class Version : std::size_t {
    V4 = 0,
    V3 = 1,
  };

  void init(Version version, bool ds_running);
  void update();

  enum class MatchMode {
    DISABLED,
    AUTO,
    TELEOP,
    TEST,
    ESTOPPED,
  };

  MatchMode get_match_mode();

  enum class Alliance {
    BLUE,
    RED,
  };

  Alliance get_alliance();

  enum class MatchType {
    NONE,
    PRACTICE,
    QUAL,
    ELIM,
  };

  MatchType get_match_type();

  std::size_t get_station_number();

  std::string get_event_name();
  std::string get_game_msg();
  std::size_t get_match_number();
  std::size_t get_replay_number();

  inline nt::NetworkTableInstance get_instance() { return nt_inst; }
  inline std::shared_ptr<nt::NetworkTable> get_smart_dashboard() { return sd_table; }
  inline std::shared_ptr<nt::NetworkTable> get_fms_info() { return fms_table; }

  void set_bool(const std::string& key, bool value);
  void set_double(const std::string& key, double value);
  void set_string(const std::string& key, std::string_view value);

private:
  NTHandler();
  ~NTHandler();

  nt::NetworkTableInstance nt_inst;
  std::shared_ptr<nt::NetworkTable> sd_table, fms_table;

  std::map<std::string, bool> set_bools;
  std::map<std::string, double> set_doubles;
  std::map<std::string, std::string> set_strings;

  int32_t get_ctrl_word();

  static NTHandler instance;
};

} // namespace frc1511
