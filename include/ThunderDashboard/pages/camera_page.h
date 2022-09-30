#pragma once

#include <ThunderDashboard/pages/page.h>

#ifdef TD_WITH_CS
# include <opencv2/opencv.hpp>
# include <cscore.h>
# include <cscore_cv.h>
#endif

namespace frc1511 {

class CameraPage : public Page {
protected:
  CameraPage(std::string_view name, std::string_view url);
  virtual ~CameraPage();

  void set_fps(std::size_t fps);

  /**
   * @brief Returns an OpenGL texture ID for the current frame.
   */
  unsigned int get_frame_texture();

  /**
   * @brief Returns the aspect ratio of the current frame.
   */
  double get_frame_aspect_ratio();

  /**
   * @brief Terminates the camera thread.
   */
  void terminate();

public:

  void init();

  /**
   * @brief Sets whether the camera thread is running.
   */
  void set_running(bool running);

private:
  std::string name, url;

  std::thread camera_thread;
  std::mutex camera_mutex;
  
#ifdef TD_WITH_CS
  std::unique_ptr<cs::HttpCamera> camera;
#endif

  // OpenGL textures.
  unsigned int no_cam_tex, frame_tex, working_frame_tex;

  // Aspect ratios.
  double no_cam_ar, frame_ar;

  bool thread_running = false;
  bool thread_terminated = false;
  bool has_frame = false;

  std::size_t fps = 30;

  static inline std::mutex gl_mutex;

  void thread_start();
};

}
