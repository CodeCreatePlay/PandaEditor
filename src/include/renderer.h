#ifndef RENDERER_H
#define RENDERER_H

#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <tuple>

// Core/Utility headers
#include <lvecBase2.h>
#include <lvecBase3.h>
#include <lvecBase4.h>
// Graphics Engine and Rendering
#include <graphicsEngine.h>
#include <graphicsOutput.h>
#include <displayRegion.h>
#include <shader.h>
// Input and Interaction
#include <mouseWatcher.h>
// Scene Graph and Camera
#include <camera.h>
#include <orthographicLens.h>
#include <nodePath.h>
// Object Creation and Textures
#include <cardMaker.h>
#include <texture.h>
#include <texturepool.h>


class Engine;

class Renderer {
public:
    struct FramebufferTexture
    {
        PT(GraphicsOutput) buffer;
        PT(DisplayRegion) bufferRegion;
        PT(Camera) camera;

        NodePath cameraNP;
        NodePath shaderNP;
    };

    struct FramebufferTextureArguments 
    {
        GraphicsOutput* graphicsOutput;
        GraphicsEngine* graphicsEngine;
        GraphicsOutput::RenderTexturePlane bitplane;

        LVecBase4 rgbaBits;

        LColor clearColor;

        int aux_rgba;

        bool setFloatColor;
        bool setSrgbColor;
        bool setRgbColor;
        bool useScene;

        std::string name;
    };

    Renderer();
    ~Renderer();

    // Functions
    void init(
        GraphicsEngine* engine,
        GraphicsWindow* window,
        DisplayRegion* displayRegion,
        NodePath* cameraNP,
        NodePath* render,
        NodePath* render2d);

    PT(Shader) load_shader(const std::string vert, const std::string frag);

    FramebufferTexture generate_framebuffer_texture(const FramebufferTextureArguments& fb_tex_arguments);
   
    PTA_LVecBase3f generate_ssao_samples(int numberOfSamples);
    PTA_LVecBase3f generate_ssao_noise(int numberOfNoise);

    void show_buffer(NodePath render2d, const std::tuple<std::string, PT(GraphicsOutput), int> bufferTexture, bool alpha);
    void hide_buffer(NodePath render2d);
    void set_texture_to_nearest_and_clamp(PT(Texture) texture);

    LColor mix_color(LColor a, LColor b, float amount);

    LVecBase2f make_enabled_vec(int t);
    LVecBase2f toggle_enabled_vec(LVecBase2f vec);

    double microsecond_to_second(int m);
    int microseconds_since_epoch();

    // Globals
    static LVecBase2f PI_SHADER_INPUT;
    static LVecBase2f GAMMA_SHADER_INPUT;

    static float TO_RAD;
    static float GAMMA;
    static float GAMMA_REC;

    static int BACKGROUND_RENDER_SORT_ORDER;
    static int UNSORTED_RENDER_SORT_ORDER;
    static int SSAO_SAMPLES;
    static int SSAO_NOISE;
    static int SHADOW_SIZE;

    std::default_random_engine generator;
};

#endif // RENDERER_H