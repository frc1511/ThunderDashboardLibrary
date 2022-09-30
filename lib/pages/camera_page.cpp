#include <ThunderDashboard/pages/camera_page.h>
#include <stb_image.h>
#include <glad/glad.h>

#include <no_camera_png.h>

using namespace frc1511;

static void setup_tex(unsigned int& tex) {
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

CameraPage::CameraPage(std::string_view _name, std::string_view _url)
: name(_name), url(_url) { }

void CameraPage::init() {
#ifdef TD_WITH_CS
  camera = std::make_unique<cs::HttpCamera>(name, url, cs::HttpCamera::HttpCameraKind::kMJPGStreamer);
  camera->SetVideoMode(cs::VideoMode::kMJPEG, 320, 240, 30);
#endif
  
  //set_fps(fps);

  int width, height, nr_channels;
  unsigned char* img_data = stbi_load_from_memory(no_camera_png, no_camera_png_size, &width, &height, &nr_channels, 0);

  assert(img_data); // Failed to load texture from memory.

  int tex_channels(nr_channels == 3 ? GL_RGB : GL_RGBA);

  {
    std::lock_guard<std::mutex> lk(gl_mutex);
    setup_tex(no_cam_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, tex_channels, width, height, 0, tex_channels, GL_UNSIGNED_BYTE, img_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    setup_tex(frame_tex);
    setup_tex(working_frame_tex);
  }

  stbi_image_free(img_data);

  no_cam_ar = static_cast<float>(width) / static_cast<float>(height);

  camera_thread = std::thread([&]() {
    thread_start();
  });
}

CameraPage::~CameraPage() {
  terminate();
  camera_thread.join();
  glDeleteTextures(1, &no_cam_tex);
  glDeleteTextures(1, &frame_tex);
  glDeleteTextures(1, &working_frame_tex);
}

void CameraPage::set_fps(std::size_t _fps) {
  std::lock_guard<std::mutex> lk(camera_mutex);
  fps = _fps;

#ifdef TD_WITH_CS
  camera->SetFPS(fps);
#endif
}

unsigned int CameraPage::get_frame_texture() {
  std::lock_guard<std::mutex> lk(camera_mutex);
  if (has_frame) {
    return frame_tex;
  }
  return no_cam_tex;
}

double CameraPage::get_frame_aspect_ratio() {
  std::lock_guard<std::mutex> lk(camera_mutex);
  if (has_frame) {
    return frame_ar;
  }
  return no_cam_ar;
}

void CameraPage::terminate() {
  std::lock_guard<std::mutex> lk(camera_mutex);
  thread_terminated = true;
}

void CameraPage::set_running(bool running) {
  std::lock_guard<std::mutex> lk(camera_mutex);
  thread_running = running;
}

void CameraPage::thread_start() {
#ifdef TD_WITH_CS
  using namespace std::chrono_literals;

  cs::CvSink cv_sink;
  cv::Mat frame;

  {
    std::lock_guard<std::mutex> lk(camera_mutex);
    cv_sink.SetSource(*camera);
  }
  cv_sink.SetEnabled(true);

  std::chrono::steady_clock::time_point start, end;

  while (true) {
    start = std::chrono::high_resolution_clock::now();

    bool running = false;
    {
      std::lock_guard<std::mutex> lk(camera_mutex);
      if (thread_terminated) break;
      running = thread_running;
    }

    if (!running) {
      // Try again in 1 second.
      std::this_thread::sleep_for(1s);
      continue;
    }

    {
      std::lock_guard<std::mutex> lk(camera_mutex);
      cv_sink.SetSource(*camera);
    }

    uint64_t frame_time = cv_sink.GrabFrame(frame);

    if (frame_time) {
      {
        std::lock_guard<std::mutex> lk(gl_mutex);
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

        glBindTexture(GL_TEXTURE_2D, working_frame_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.data);
        glGenerateMipmap(GL_TEXTURE_2D);
      }

      {
        std::lock_guard<std::mutex> lk(camera_mutex);

        // Swap the frame textures.
        unsigned int old_frame_tex = frame_tex;
        frame_tex = working_frame_tex;
        working_frame_tex = old_frame_tex;

        // Aspect ratio.
        frame_ar = static_cast<double>(frame.cols) / static_cast<double>(frame.rows);

        has_frame = true;
      }
  
      end = std::chrono::high_resolution_clock::now();
      double dur(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() * 0.001);

      std::size_t freq;
      {
        std::lock_guard<std::mutex> lk(camera_mutex);
        freq = fps;
      }

      double period = 1.0 / freq;

      if (dur < period) {
        std::size_t sleep_time = (period - dur) * 1000.0;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
      }
    }
    else {
      {
        std::lock_guard<std::mutex> lk(camera_mutex);
        has_frame = false;
      }

      std::cout << "no frame? " << cv_sink.GetError() << '\n';
      
      // Try again in 1 second.
      std::this_thread::sleep_for(1s);
    }
  }
#endif
}
