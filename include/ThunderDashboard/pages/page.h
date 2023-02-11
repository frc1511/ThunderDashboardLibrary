#pragma once

#include <ThunderDashboard/dashboard.h>

namespace frc1511 {

class Page {
public:
  virtual ~Page() = default;

  virtual void init() { }
  virtual void present(bool* running) = 0;
  constexpr bool is_focused() const { return focused; }

  virtual const char* get_save_name() const { return ""; }

  using SaveData = std::map<std::string, std::string>;

  virtual void apply_save_data(const SaveData& save_data) { }
  virtual SaveData get_save_data() const { return {}; }
  
protected:
  Page() = default;

  bool focused = false;
};

} // namespace frc1511
