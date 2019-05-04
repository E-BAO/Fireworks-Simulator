//
// Created by EBao on 2019-04-21.
//

#include "FireworkSimulator.h"


#include <cmath>
#include <glad/glad.h>

#include <CGL/vector3D.h>
#include <nanogui/nanogui.h>

#include "camera.h"
#include "cloth.h"
#include "collision/plane.h"
#include "collision/intersection.h"
#include "misc/camera_info.h"
#include "misc/file_utils.h"
// Needed to generate stb_image binaries. Should only define in exactly one source file importing stb_image.h.
#define STB_IMAGE_IMPLEMENTATION

#include "misc/stb_image.h"

using namespace nanogui;
using namespace std;


void FireworkSimulator::load_shaders() {
  std::set<std::string> shader_folder_contents;
  bool success = FileUtils::list_files_in_directory(m_project_root + "/shaders", shader_folder_contents);
  if (!success) {
    std::cout << "Error: Could not find the shaders folder!" << std::endl;
  }

  std::string shader_name = "Simple";
  std::string shader_fname = "Simple.frag";
  std::string vert_shader = m_project_root + "/shaders/Simple.vert";

  GLShader nanogui_shader;
  nanogui_shader.initFromFiles(shader_name, vert_shader,
                               m_project_root + "/shaders/" + shader_fname);

  ShaderTypeHint hint = ShaderTypeHint::SIMPLE;
  UserShader user_shader(shader_name, nanogui_shader, hint);

  shaders.push_back(user_shader);


  vector<string> type_names = {"Simple", "Blinking", "Drawing"};
  for (vector<string>::const_iterator iter = type_names.cbegin(); iter != type_names.cend(); iter++)
    types_combobox_names.push_back(*iter);

}

FireworkSimulator::FireworkSimulator(std::string project_root, Screen *screen) : m_project_root(project_root) {
  this->screen = screen;
  this->load_shaders();

  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_DEPTH_TEST);
}

FireworkSimulator::~FireworkSimulator() {

}


void FireworkSimulator::init() {
  // Initialize GUI
  screen->setSize(default_window_size);
  initGUI(screen);

  // Initialize camera

  CGL::Collada::CameraInfo camera_info;
  camera_info.hFov = 50;
  camera_info.vFov = 35;
  camera_info.nClip = 0.01;
  camera_info.fClip = 10000;

  // Try to intelligently figure out the camera target

  Vector3D avg_pm_position(0, 0, 0);

//    for (auto &pm : cloth->point_masses) {
//        avg_pm_position += pm.position / cloth->point_masses.size();
//    }

  CGL::Vector3D target(avg_pm_position.x, avg_pm_position.y / 2,
                       avg_pm_position.z);
  CGL::Vector3D c_dir(0., 0., 0.);
//    canonical_view_distance = max(cloth->width, cloth->height) * 0.9;
  canonical_view_distance = 0.9;
  scroll_rate = canonical_view_distance / 10;

  view_distance = canonical_view_distance * 2;
  min_view_distance = canonical_view_distance / 10.0;
  max_view_distance = canonical_view_distance * 20.0;

  // canonicalCamera is a copy used for view resets

  camera.place(target, acos(c_dir.y), atan2(c_dir.x, c_dir.z), view_distance,
               min_view_distance, max_view_distance);
  canonicalCamera.place(target, acos(c_dir.y), atan2(c_dir.x, c_dir.z),
                        view_distance, min_view_distance, max_view_distance);

  screen_w = default_window_size(0);
  screen_h = default_window_size(1);

  camera.configure(camera_info, screen_w, screen_h);
  canonicalCamera.configure(camera_info, screen_w, screen_h);

  //init objects
  collision_objects = new vector<CollisionObject *>();

  //init plane
  Vector3D point(0, 0, 0), normal(0, 1, 0);
  double friction = 0.5;

  plane = new Plane(point, normal, friction);
  collision_objects->push_back(plane);
//    cout <<plane->p0 << " "<<plane->p1 << " "<<plane->p2<<" "<<plane->p3<<endl;
}


void FireworkSimulator::drawContents() {
  glEnable(GL_DEPTH_TEST);

  if (!is_paused) {
    vector<Vector3D> external_accelerations = {gravity};

    for (Firework *f: fireworks) {
      for (int i = 0; i < simulation_steps; i++) {
        f->simulate(frames_per_sec, simulation_steps, external_accelerations);
      }
    }
  }

  // Bind the active shader

  const UserShader &active_shader = shaders[0];

  GLShader shader = active_shader.nanogui_shader;
  shader.bind();

  // Prepare the camera projection matrix

  Matrix4f model;
  model.setIdentity();

  Matrix4f view = getViewMatrix();
  Matrix4f projection = getProjectionMatrix();

  Matrix4f viewProjection = projection * view;

  shader.setUniform("u_model", model);
  shader.setUniform("u_view_projection", viewProjection);

  shader.setUniform("u_color", color, false);
  drawWireframe(shader);

  for (CollisionObject *co : *collision_objects) {
    co->render(shader);
  }
}

void FireworkSimulator::drawWireframe(GLShader &shader) {

  int num_particles = 0;
  for (auto f: fireworks) {
    if (f->status == DIED)
      continue;
    if (f->status == EXPLODING)
      num_particles += f->particles.size();
    else if (f->status == IGNITING)
      num_particles++;
  }

  MatrixXf positions(4, num_particles);
  MatrixXf colors(4, num_particles);
  MatrixXf particle_sizes(1, num_particles);

  int si = 0;
  for (auto f: fireworks) {
    if (f->status == DIED)
      continue;
    nanogui::Color color = f->color;
    if (f->status == EXPLODING) {
      for (FireParticle &p: f->particles) {
        Vector3D pos = p.position;
        positions.col(si) << pos.x, pos.y, pos.z, 1.0;
        colors.col(si) << color.r(), color.g(), color.b(), p.lifetime;
        particle_sizes.col(si) << f->particle_size;
        si++;
      }
    } else if (f->status == IGNITING) {
      Vector3D pos = f->igniteParticle->position;
      positions.col(si) << pos.x, pos.y, pos.z, 1.0;
      colors.col(si) << color.r(), color.g(), color.b(), f->igniteParticle->lifetime;
      particle_sizes.col(si) << f->particle_size;
      si++;
    }
  }
  shader.uploadAttrib("in_position", positions, false);
  shader.uploadAttrib("in_color", colors, false);
  shader.uploadAttrib("in_particle_size", particle_sizes, false);

  shader.drawArray(GL_POINTS, 0, num_particles);
}

void FireworkSimulator::initGUI(Screen *screen) {
  Window *window;

  // Firework Widget

  window = new Window(screen, "Firework");
  window->setPosition(Vector2i(15, 15));
  window->setLayout(new GroupLayout(15, 6, 14, 5));

  // Firework Widget

  new Label(window, "Type", "sans-bold");

  {
    ComboBox *cb = new ComboBox(window, types_combobox_names);
    cb->setFontSize(14);
    cb->setCallback(
        [this, screen](int idx) { active_type_idx = idx; });
    cb->setSelectedIndex(active_type_idx);
  }

  // Parameters

  new Label(window, "Color", "sans-bold");

  {
    ColorWheel *cw = new ColorWheel(window, color);
    cw->setColor(this->color);
    cw->setCallback(
        [this](const nanogui::Color &color) { this->color = color; });
  }

  // Firework parameters

  new Label(window, "Density", "sans-bold");

  {
    Widget *panel = new Widget(window);
    panel->setLayout(
        new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 5));

    Slider *slider = new Slider(panel);
    slider->setValue(density / 2000);
    slider->setFixedWidth(105);

    TextBox *damping_coef = new TextBox(panel);
    damping_coef->setFixedWidth(75);
    damping_coef->setValue(to_string(int(density)));
    damping_coef->setUnits("");
    damping_coef->setFontSize(14);

    slider->setCallback([damping_coef](float value) {
      damping_coef->setValue(std::to_string(int(value * 2000)));
    });
    slider->setFinalCallback([&](float value) {
      density = (double)value * 2000;
    });
  }


  new Label(window, "Damping", "sans-bold");

  {
    Widget *panel = new Widget(window);
    panel->setLayout(
        new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 5));

    Slider *slider = new Slider(panel);
    slider->setValue(damping / 100);
    slider->setFixedWidth(105);

    TextBox *damping_coef = new TextBox(panel);
    damping_coef->setFixedWidth(75);
    damping_coef->setValue(to_string(int(damping)));
    damping_coef->setUnits("");
    damping_coef->setFontSize(14);

    slider->setCallback([damping_coef](float value) {
      damping_coef->setValue(std::to_string(int(value * 100)));
    });
    slider->setFinalCallback([&](float value) {
      damping = (double)value * 100;
    });
  }

  new Label(window, "Parameters", "sans-bold");

  {
    Widget *panel = new Widget(window);
    GridLayout *layout =
        new GridLayout(Orientation::Horizontal, 2, Alignment::Middle, 5, 5);
    layout->setColAlignment({Alignment::Maximum, Alignment::Fill});
    layout->setSpacing(0, 10);
    panel->setLayout(layout);

    new Label(panel, "speed :", "sans-bold");

    FloatBox<double> *fb = new FloatBox<double>(panel);
    fb->setEditable(true);
    fb->setFixedSize(Vector2i(100, 20));
    fb->setFontSize(14);
    fb->setValue(speed);
    fb->setUnits("m/s");
    fb->setSpinnable(true);
    fb->setCallback([this](float value) { speed = value; });

    new Label(panel, "force :", "sans-bold");

    fb = new FloatBox<double>(panel);
    fb->setEditable(true);
    fb->setFixedSize(Vector2i(100, 20));
    fb->setFontSize(14);
    fb->setValue(energy);
    fb->setUnits("N");
    fb->setSpinnable(true);
    fb->setMinValue(0);
    fb->setCallback([this](float value) { energy = value; });

    new Label(panel, "particle size :", "sans-bold");

    fb = new FloatBox<double>(panel);
    fb->setEditable(true);
    fb->setFixedSize(Vector2i(100, 20));
    fb->setFontSize(14);
    fb->setValue(particle_size);
    fb->setUnits("px");
    fb->setSpinnable(true);
    fb->setMinValue(0);
    fb->setCallback([this](float value) { particle_size = value; });

  }

  // Simulation constants

  new Label(window, "Simulation", "sans-bold");

  {
    Widget *panel = new Widget(window);
    GridLayout *layout =
        new GridLayout(Orientation::Horizontal, 2, Alignment::Middle, 5, 5);
    layout->setColAlignment({Alignment::Maximum, Alignment::Fill});
    layout->setSpacing(0, 10);
    panel->setLayout(layout);

    new Label(panel, "frames/s :", "sans-bold");

    IntBox<int> *fsec = new IntBox<int>(panel);
    fsec->setEditable(true);
    fsec->setFixedSize(Vector2i(100, 20));
    fsec->setFontSize(14);
    fsec->setValue(frames_per_sec);
    fsec->setSpinnable(true);
    fsec->setCallback([this](int value) { frames_per_sec = value; });

    new Label(panel, "steps/frame :", "sans-bold");

    IntBox<int> *num_steps = new IntBox<int>(panel);
    num_steps->setEditable(true);
    num_steps->setFixedSize(Vector2i(100, 20));
    num_steps->setFontSize(14);
    num_steps->setValue(simulation_steps);
    num_steps->setSpinnable(true);
    num_steps->setMinValue(0);
    num_steps->setCallback([this](int value) { simulation_steps = value; });
  }
}

bool FireworkSimulator::isAlive() { return is_alive; }


// ----------------------------------------------------------------------------
// CAMERA CALCULATIONS
//
// OpenGL 3.1 deprecated the fixed pipeline, so we lose a lot of useful OpenGL
// functions that have to be recreated here.
// ----------------------------------------------------------------------------

void FireworkSimulator::resetCamera() { camera.copy_placement(canonicalCamera); }

Matrix4f FireworkSimulator::getProjectionMatrix() {
  Matrix4f perspective;
  perspective.setZero();

  double near = camera.near_clip();
  double far = camera.far_clip();

  double theta = camera.v_fov() * M_PI / 360;
  double range = far - near;
  double invtan = 1. / tanf(theta);

  perspective(0, 0) = invtan / camera.aspect_ratio();
  perspective(1, 1) = invtan;
  perspective(2, 2) = -(near + far) / range;
  perspective(3, 2) = -1;
  perspective(2, 3) = -2 * near * far / range;
  perspective(3, 3) = 0;

  return perspective;
}

Matrix4f FireworkSimulator::getViewMatrix() {
  Matrix4f lookAt;
  Matrix3f R;

  lookAt.setZero();

  // Convert CGL vectors to Eigen vectors

  CGL::Vector3D c_pos = camera.position();
  CGL::Vector3D c_udir = camera.up_dir();
  CGL::Vector3D c_target = camera.view_point();

  Vector3f eye(c_pos.x, c_pos.y, c_pos.z);
  Vector3f up(c_udir.x, c_udir.y, c_udir.z);
  Vector3f target(c_target.x, c_target.y, c_target.z);

  R.col(2) = (eye - target).normalized();
  R.col(0) = up.cross(R.col(2)).normalized();
  R.col(1) = R.col(2).cross(R.col(0));

  lookAt.topLeftCorner<3, 3>() = R.transpose();
  lookAt.topRightCorner<3, 1>() = -R.transpose() * eye;
  lookAt(3, 3) = 1.0f;

  return lookAt;
}

// ----------------------------------------------------------------------------
// EVENT HANDLING
// ----------------------------------------------------------------------------

bool FireworkSimulator::cursorPosCallbackEvent(double x, double y) {
  if (left_down && !middle_down && !right_down) {
    if (ctrl_down) {
      mouseRightDragged(x, y);
    } else {
      mouseLeftDragged(x, y);
    }
  } else if (!left_down && !middle_down && right_down) {
    mouseRightDragged(x, y);
  } else if (!left_down && !middle_down && !right_down) {
    mouseMoved(x, y);
  }

  mouse_x = x;
  mouse_y = y;

  return true;
}

bool FireworkSimulator::mouseButtonCallbackEvent(int button, int action,
                                                 int modifiers) {
  switch (action) {
    case GLFW_PRESS:
      switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
          left_down = true;
          last_click = clock();
          break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
          middle_down = true;
          break;
        case GLFW_MOUSE_BUTTON_RIGHT:
          right_down = true;
          break;
      }
      return true;

    case GLFW_RELEASE:
      switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
          left_down = false;
          if (clock() - last_click < click_duration) {
            // shoot ray
            Ray ray = camera.generate_ray(mouse_x * screen->pixelRatio() / screen_w,
                                          1. - mouse_y * screen->pixelRatio() / screen_h);
//                        cout << mouse_x * screen->pixelRatio() / screen_w << " " <<
//                        mouse_y * screen->pixelRatio() / screen_h << endl;
            Intersection *isect = new Intersection();
            if (plane->intersect(ray, isect)) {
              Vector3D isect_pos = ray.o + ray.d * isect->t;
//                            cout << isect_pos << endl;
              Firework *f = new Firework(isect_pos, Vector3D(0, speed, 0), density, energy, damping, particle_size);
              f->color = this->color;
              fireworks.push_back(f);
              drawContents();
            }
          }
          break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
          middle_down = false;
          break;
        case GLFW_MOUSE_BUTTON_RIGHT:
          right_down = false;
          break;
      }
      return true;
  }

  return false;
}

void FireworkSimulator::mouseMoved(double x, double y) { y = screen_h - y; }

void FireworkSimulator::mouseLeftDragged(double x, double y) {
  float dx = x - mouse_x;
  float dy = y - mouse_y;

  camera.rotate_by(-dy * (PI / screen_h), -dx * (PI / screen_w));
}

void FireworkSimulator::mouseRightDragged(double x, double y) {
  camera.move_by(mouse_x - x, y - mouse_y, canonical_view_distance);
}

bool FireworkSimulator::keyCallbackEvent(int key, int scancode, int action,
                                         int mods) {
  ctrl_down = (bool) (mods & GLFW_MOD_CONTROL);


  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        is_alive = false;
        break;
      case 'r':
      case 'R':
//                cloth->reset();
        break;
      case ' ':
        resetCamera();
        break;
      case 'p':
      case 'P':
        is_paused = !is_paused;
        break;
      case 'n':
      case 'N':
        if (is_paused) {
          is_paused = false;
          drawContents();
          is_paused = true;
        }
        break;

        // Yiwen debug
      case 'o':
      case 'O':
        std::cout << " press o add fireworks " << std::endl;
        Firework *f = new Firework(Vector3D(0, 0, 0), Vector3D(0, 2, 0));
        f->color = this->color;
        fireworks.push_back(f);
        drawContents();
        break;
    }
  }

  return true;
}

bool FireworkSimulator::dropCallbackEvent(int count, const char **filenames) {
  return true;
}

bool FireworkSimulator::scrollCallbackEvent(double x, double y) {
  camera.move_forward(y * scroll_rate);
  return true;
}

bool FireworkSimulator::resizeCallbackEvent(int width, int height) {
  screen_w = width;
  screen_h = height;

  camera.set_screen_size(screen_w, screen_h);
  return true;
}