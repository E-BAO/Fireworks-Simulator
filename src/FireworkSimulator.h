//
// Created by EBao on 2019-04-21.
//
#ifndef CLOTHSIM_FIREWORKSIMULATOR_H
#define CLOTHSIM_FIREWORKSIMULATOR_H

#include <nanogui/nanogui.h>

#include "camera.h"
#include "collision/collisionObject.h"
#include "Firework.h"
#include "collision/plane.h"

using namespace nanogui;

struct UserShader;

enum ShaderTypeHint { SIMPLE = 0, NORMALS = 1, PHONG = 2, PLANE = 3 };

class FireworkSimulator {
public:
    FireworkSimulator(std::string project_root, Screen *screen);
    ~FireworkSimulator();

    void init();

    virtual  bool isAlive();
    virtual void drawContents();


    // Screen events
    virtual bool cursorPosCallbackEvent(double x, double y);
    virtual bool mouseButtonCallbackEvent(int button, int action, int modifiers);
    virtual bool keyCallbackEvent(int key, int scancode, int action, int mods);
    virtual bool dropCallbackEvent(int count, const char **filenames);
    virtual bool scrollCallbackEvent(double x, double y);
    virtual bool resizeCallbackEvent(int width, int height);

private:
    virtual void initGUI(Screen *screen);
    void load_shaders();

    // Default simulation values
    vector<Firework*> fireworks;
    void drawWireframe(GLShader &shader);

    // File management

    std::string m_project_root;

    // Camera methods
    virtual void resetCamera();
    virtual Matrix4f getProjectionMatrix();
    virtual Matrix4f getViewMatrix();

    // Default simulation values

    int frames_per_sec = 90;
    int simulation_steps = 30;

    CGL::Vector3D gravity = CGL::Vector3D(0, -9.8, 0);
    nanogui::Color color = nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f);
    float speed = 5;
    float density = 200;
    float energy = 3;
    float damping = 1;
    float particle_size = 10;
    bool enable_blink = false;
    bool enable_trail = false;
    float trail_damping = .9;
    FireworkShape shape = FireworkShape::DISPERSED;


    vector<CollisionObject *> *collision_objects;

    // OpenGL attributes

    int active_type_idx = 0;

    vector<UserShader> shaders;
    vector<std::string> types_combobox_names;


    // Camera attributes

    CGL::Camera camera;
    CGL::Camera canonicalCamera;

    double view_distance;
    double canonical_view_distance;
    double min_view_distance;
    double max_view_distance;

    double scroll_rate;

    // Screen methods

    Screen *screen;
    void mouseLeftDragged(double x, double y);
    void mouseRightDragged(double x, double y);
    void mouseMoved(double x, double y);

    // Mouse flags

    bool left_down = false;
    bool right_down = false;
    bool middle_down = false;

    // Keyboard flags

    bool ctrl_down = false;
    clock_t last_click = 0;
    int click_duration = 40000;

    // Simulation flags

    bool is_paused = true;

    // Screen attributes

    int mouse_x;
    int mouse_y;

    int screen_w;
    int screen_h;

    bool is_alive = true;
    Plane *plane;

    Vector2i default_window_size = Vector2i(1024, 800);
};

struct UserShader {
    UserShader(std::string display_name, GLShader nanogui_shader, ShaderTypeHint type_hint)
            : display_name(display_name)
            , nanogui_shader(nanogui_shader)
            , type_hint(type_hint) {
    }

    GLShader nanogui_shader;
    std::string display_name;
    ShaderTypeHint type_hint;

};

#endif