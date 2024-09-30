#include "include/renderer.h"

float Renderer::TO_RAD = M_PI / 180.0;
float Renderer::GAMMA = 2.2;
float Renderer::GAMMA_REC = 1.0 / GAMMA;

LVecBase2f Renderer::PI_SHADER_INPUT = LVecBase2f(M_PI, TO_RAD);
LVecBase2f Renderer::GAMMA_SHADER_INPUT;

int Renderer::BACKGROUND_RENDER_SORT_ORDER = 10;
int Renderer::UNSORTED_RENDER_SORT_ORDER = 50;
int Renderer::SSAO_SAMPLES = 8;
int Renderer::SSAO_NOISE = 4;
int Renderer::SHADOW_SIZE = 2048;


Renderer::Renderer() {}

Renderer::~Renderer() {}

void Renderer::init(
    GraphicsEngine* engine,
    GraphicsWindow* window,
    DisplayRegion*  displayRegion,
    NodePath*       cameraNP,
    NodePath*       render,
    NodePath*       render2d) 
{  

    Camera* camera = DCAST(Camera, cameraNP->node());


    /*
    GraphicsEngine* graphics_engine        = engine->get_graphics_engine();
    GraphicsWindow* graphics_win           = engine->get_graphics_win();
    NodePath*       cameraNP               = &(NodePath)engine->scene_cam;
    Camera*         camera                 = DCAST(Camera, engine->scene_cam.node());
    NodePath*       render                 = &engine->render;
    NodePath*       render2d               = &engine->render2d;
    */

    /*
    LVecBase2f      riorInitial            = LVecBase2f(1.05, 1.05);
    LVecBase2f      rior                   = riorInitial;

    int             cameraNear             = 150;
    int             cameraFar              = 2000;
    LVecBase2f      cameraNearFar          = LVecBase2f(cameraNear, cameraFar);

    LVecBase2f      mouseFocusPointInitial = LVecBase2f(0.509167, 0.598);
    LVecBase2f      mouseFocusPoint        = mouseFocusPointInitial;

    float           sunlightP              = 260;
    */

    LColor backgroundColor[] =
    {
        LColor(0.392, 0.537, 0.561, 1),
        LColor(0.953, 0.733, 0.525, 1) 
    };

    float fogNearInitial = 2.0;
    float fogFarInitial  = 9.0;
    float fogNear        = fogNearInitial;
    float fogFar         = fogFarInitial;
    float fogAdjust      = 0.1;

    LMatrix4 currentViewWorldMat = cameraNP->get_transform(*render)->get_mat();
    LMatrix4 previousViewWorldMat = previousViewWorldMat;

    LVecBase2f ssaoEnabled                = make_enabled_vec(1);
    LVecBase2f blinnPhongEnabled          = make_enabled_vec(1);
    LVecBase2f fresnelEnabled             = make_enabled_vec(1);
    LVecBase2f rimLightEnabled            = make_enabled_vec(1);
    LVecBase2f refractionEnabled          = make_enabled_vec(1);
    LVecBase2f reflectionEnabled          = make_enabled_vec(1);
    LVecBase2f fogEnabled                 = make_enabled_vec(1);
    LVecBase2f outlineEnabled             = make_enabled_vec(1);
    LVecBase2f celShadingEnabled          = make_enabled_vec(1);
    LVecBase2f normalMapsEnabled          = make_enabled_vec(1);
    LVecBase2f bloomEnabled               = make_enabled_vec(1);
    LVecBase2f sharpenEnabled             = make_enabled_vec(1);
    LVecBase2f depthOfFieldEnabled        = make_enabled_vec(1);
    LVecBase2f filmGrainEnabled           = make_enabled_vec(1);
    LVecBase2f flowMapsEnabled            = make_enabled_vec(1);
    LVecBase2f lookupTableEnabled         = make_enabled_vec(1);
    LVecBase2f painterlyEnabled           = make_enabled_vec(0);
    LVecBase2f motionBlurEnabled          = make_enabled_vec(0);
    LVecBase2f posterizeEnabled           = make_enabled_vec(0);
    LVecBase2f pixelizeEnabled            = make_enabled_vec(0);
    LVecBase2f chromaticAberrationEnabled = make_enabled_vec(1);

    LVecBase4 rgba8  = (8, 8, 8, 8);
    LVecBase4 rgba16 = (16, 16, 16, 16);
    LVecBase4 rgba32 = (32, 32, 32, 32);

    PT(Texture) blankTexture             = TexturePool::load_texture("images/blank.png");
    PT(Texture) foamPatternTexture       = TexturePool::load_texture("images/foam-pattern.png");
    PT(Texture) stillFlowTexture         = TexturePool::load_texture("images/still-flow.png");
    PT(Texture) upFlowTexture            = TexturePool::load_texture("images/up-flow.png");
    PT(Texture) colorLookupTableTextureN = TexturePool::load_texture("images/lookup-table-neutral.png");
    PT(Texture) colorLookupTableTexture0 = TexturePool::load_texture("images/lookup-table-0.png");
    PT(Texture) colorLookupTableTexture1 = TexturePool::load_texture("images/lookup-table-1.png");
    PT(Texture) colorNoiseTexture        = TexturePool::load_texture("images/color-noise.png");

    set_texture_to_nearest_and_clamp(colorLookupTableTextureN);
    set_texture_to_nearest_and_clamp(colorLookupTableTexture0);
    set_texture_to_nearest_and_clamp(colorLookupTableTexture1);

    displayRegion->set_clear_color_active(true);
    displayRegion->set_clear_depth_active(true);
    displayRegion->set_clear_stencil_active(true);
    displayRegion->set_clear_color(backgroundColor[1]);
    displayRegion->set_clear_depth(1.0f);
    displayRegion->set_clear_stencil(0);

    PT(Shader) discardShader               = load_shader("discard", "discard");
    PT(Shader) baseShader                  = load_shader("base",    "base");
    PT(Shader) geometryBufferShader0       = load_shader("base",    "geometry-buffer-0");
    PT(Shader) geometryBufferShader1       = load_shader("base",    "geometry-buffer-1");
    PT(Shader) geometryBufferShader2       = load_shader("base",    "geometry-buffer-2");
    PT(Shader) foamShader                  = load_shader("basic",   "foam");
    PT(Shader) fogShader                   = load_shader("basic",   "fog");
    PT(Shader) boxBlurShader               = load_shader("basic",   "box-blur");
    PT(Shader) motionBlurShader            = load_shader("basic",   "motion-blur");
    PT(Shader) kuwaharaFilterShader        = load_shader("basic",   "kuwahara-filter");
    PT(Shader) dilationShader              = load_shader("basic",   "dilation");
    PT(Shader) sharpenShader               = load_shader("basic",   "sharpen");
    PT(Shader) outlineShader               = load_shader("basic",   "outline");
    PT(Shader) bloomShader                 = load_shader("basic",   "bloom");
    PT(Shader) ssaoShader                  = load_shader("basic",   "ssao");
    PT(Shader) screenSpaceRefractionShader = load_shader("basic",   "screen-space-refraction");
    PT(Shader) screenSpaceReflectionShader = load_shader("basic",   "screen-space-reflection");
    PT(Shader) refractionShader            = load_shader("basic",   "refraction");
    PT(Shader) reflectionColorShader       = load_shader("basic",   "reflection-color");
    PT(Shader) reflectionShader            = load_shader("basic",   "reflection");
    PT(Shader) baseCombineShader           = load_shader("basic",   "base-combine");
    PT(Shader) sceneCombineShader          = load_shader("basic",   "scene-combine");
    PT(Shader) depthOfFieldShader          = load_shader("basic",   "depth-of-field");
    PT(Shader) posterizeShader             = load_shader("basic",   "posterize");
    PT(Shader) pixelizeShader              = load_shader("basic",   "pixelize");
    PT(Shader) filmGrainShader             = load_shader("basic",   "film-grain");
    PT(Shader) lookupTableShader           = load_shader("basic",   "lookup-table");
    PT(Shader) gammaCorrectionShader       = load_shader("basic",   "gamma-correction");
    PT(Shader) chromaticAberrationShader   = load_shader("basic",   "chromatic-aberration");

    // -----------------------------------------------------------------
    // setup parent nodepaths
    NodePath mainCameraNP = NodePath("MainCamera");
    mainCameraNP.set_shader(discardShader);
    camera->set_initial_state(mainCameraNP.get_state());

    // water nodepath
    NodePath waterNP;
    waterNP.set_transparency(TransparencyAttrib::M_dual);
    waterNP.set_bin("fixed", 0);

    NodePath isWaterNP = NodePath("IsWater");
    isWaterNP.set_shader_input("isWater", LVecBase2f(1.0, 1.0));
    isWaterNP.set_shader_input("flowTexture", upFlowTexture);
    isWaterNP.set_shader_input("foamPatternTexture", foamPatternTexture);

    // smoke nodepath
    NodePath smokeNP;

    NodePath isSmokeNP = NodePath("IsSmoke");
    isSmokeNP.set_shader_input("IsSmoke", LVecBase2f(1.0, 1.0));
    isSmokeNP.set_shader_input("IsParticle", LVecBase2f(1.0, 1.0));

    // -----------------------------------------------------------------
    FramebufferTextureArguments framebufferTextureArguments;
    framebufferTextureArguments.graphicsOutput = window;
    framebufferTextureArguments.graphicsEngine = engine;
    framebufferTextureArguments.rgbaBits       = rgba32;
    framebufferTextureArguments.bitplane       = GraphicsOutput::RTP_color;
    framebufferTextureArguments.clearColor     = LColor(0, 0, 0, 0);
    framebufferTextureArguments.aux_rgba       = 1;
    framebufferTextureArguments.setSrgbColor   = false;
    framebufferTextureArguments.setFloatColor  = true;
    framebufferTextureArguments.setRgbColor    = true;
    framebufferTextureArguments.useScene       = true;
    framebufferTextureArguments.name           = "geometry0";

    // -----------------------------------------------------------------
    // geometry pass 0 
    FramebufferTexture geometryFramebufferTexture0 = generate_framebuffer_texture(framebufferTextureArguments);
    PT(GraphicsOutput) geometryBuffer0 = geometryFramebufferTexture0.buffer;
    PT(Camera)         geometryCamera0 = geometryFramebufferTexture0.camera;
    NodePath           geometryNP0     = geometryFramebufferTexture0.shaderNP;

    geometryBuffer0->add_render_texture(NULL, GraphicsOutput::RTM_bind_or_copy, GraphicsOutput::RTP_aux_rgba_0);
    geometryBuffer0->set_clear_active(3, true);
    geometryBuffer0->set_clear_value(3, framebufferTextureArguments.clearColor);

    geometryNP0.set_shader(geometryBufferShader0);
    geometryNP0.set_shader_input("normalMapsEnabled", normalMapsEnabled);

    geometryCamera0->set_initial_state(geometryNP0.get_state());
    geometryCamera0->set_camera_mask(BitMask32::bit(1));

    PT(Texture) positionTexture0    = geometryBuffer0->get_texture(0);
    PT(Texture) normalTexture0      = geometryBuffer0->get_texture(1);
    PT(Lens)    geometryCameraLens0 = geometryCamera0->get_lens();

    waterNP.hide(BitMask32::bit(1));
    smokeNP.hide(BitMask32::bit(1));

    // -----------------------------------------------------------------
    // geometry pass 1 
    framebufferTextureArguments.aux_rgba = 4;
    framebufferTextureArguments.name = "geometry1";

    FramebufferTexture geometryFramebufferTexture1 = generate_framebuffer_texture(framebufferTextureArguments);
    PT(GraphicsOutput) geometryBuffer1 = geometryFramebufferTexture1.buffer;
    PT(Camera)         geometryCamera1 = geometryFramebufferTexture1.camera;
    NodePath           geometryNP1 = geometryFramebufferTexture1.shaderNP;

    geometryBuffer1->add_render_texture(NULL, GraphicsOutput::RTM_bind_or_copy, GraphicsOutput::RTP_aux_rgba_0);
    geometryBuffer1->set_clear_active(3, true);
    geometryBuffer1->set_clear_value(3, framebufferTextureArguments.clearColor);

    geometryBuffer1->add_render_texture(NULL, GraphicsOutput::RTM_bind_or_copy, GraphicsOutput::RTP_aux_rgba_1);
    geometryBuffer1->set_clear_active(4, true);
    geometryBuffer1->set_clear_value(4, framebufferTextureArguments.clearColor);

    geometryBuffer1->add_render_texture(NULL, GraphicsOutput::RTM_bind_or_copy, GraphicsOutput::RTP_aux_rgba_2);
    geometryBuffer1->set_clear_active(5, true);
    geometryBuffer1->set_clear_value(5, framebufferTextureArguments.clearColor);

    geometryBuffer1->add_render_texture(NULL, GraphicsOutput::RTM_bind_or_copy, GraphicsOutput::RTP_aux_rgba_3);
    geometryBuffer1->set_clear_active(6, true);
    geometryBuffer1->set_clear_value(6, framebufferTextureArguments.clearColor);

    geometryNP1.set_shader(geometryBufferShader1);
    geometryNP1.set_shader_input("normalMapsEnabled", normalMapsEnabled);
    geometryNP1.set_shader_input("flowTexture", stillFlowTexture);
    geometryNP1.set_shader_input("foamPatternTexture", blankTexture);
    geometryNP1.set_shader_input("flowMapsEnabled", flowMapsEnabled);

    geometryCamera1->set_initial_state(geometryNP1.get_state());
    geometryCamera1->set_tag_state_key("geometryBuffer1");
    geometryCamera1->set_tag_state("isWater", isWaterNP.get_state());
    geometryCamera1->set_camera_mask(BitMask32::bit(2));

    PT(Texture) positionTexture1 = geometryBuffer1->get_texture(0);
    PT(Texture) normalTexture1 = geometryBuffer1->get_texture(1);
    PT(Texture) reflectionMaskTexture = geometryBuffer1->get_texture(2);
    PT(Texture) refractionMaskTexture = geometryBuffer1->get_texture(3);
    PT(Texture) foamMaskTexture = geometryBuffer1->get_texture(4);
    PT(Lens)    geometryCameraLens1 = geometryCamera1->get_lens();

    waterNP.set_tag("geometryBuffer1", "isWater");
    smokeNP.hide(BitMask32::bit(2));

    // -----------------------------------------------------------------
    // geometry pass 2
    framebufferTextureArguments.aux_rgba = 1;
    framebufferTextureArguments.name = "geometry2";

    FramebufferTexture geometryFramebufferTexture2 = generate_framebuffer_texture(framebufferTextureArguments);
    PT(GraphicsOutput) geometryBuffer2 = geometryFramebufferTexture2.buffer;
    PT(Camera)         geometryCamera2 = geometryFramebufferTexture2.camera;
    NodePath           geometryNP2 = geometryFramebufferTexture2.shaderNP;

    geometryBuffer2->add_render_texture(NULL, GraphicsOutput::RTM_bind_or_copy, GraphicsOutput::RTP_aux_rgba_0);
    geometryBuffer2->set_clear_active(3, true);
    geometryBuffer2->set_clear_value(3, framebufferTextureArguments.clearColor);

    geometryBuffer2->set_sort(geometryBuffer1->get_sort() + 1);

    geometryNP2.set_shader(geometryBufferShader2);

    geometryNP2.set_shader_input("isSmoke", LVecBase2f(0, 0));
    geometryNP2.set_shader_input("positionTexture", positionTexture1);

    geometryCamera2->set_initial_state(geometryNP2.get_state());

    geometryCamera2->set_tag_state_key("geometryBuffer2");
    geometryCamera2->set_tag_state("isSmoke", isSmokeNP.get_state());

    smokeNP.set_tag("geometryBuffer2", "isSmoke");

    PT(Texture) positionTexture2 = geometryBuffer2->get_texture(0);
    PT(Texture) smokeMaskTexture = geometryBuffer2->get_texture(1);
    PT(Lens)    geometryCameraLens2 = geometryCamera2->get_lens();



    framebufferTextureArguments.rgbaBits = rgba8;
    framebufferTextureArguments.aux_rgba = 0;
    framebufferTextureArguments.clearColor = LColor(0, 0, 0, 0);
    framebufferTextureArguments.setFloatColor = false;
    framebufferTextureArguments.useScene = false;
    framebufferTextureArguments.name = "fog";
}

PT(Shader) Renderer::load_shader(std::string vert, std::string frag)
{
    return Shader::load(Shader::SL_GLSL, "shaders/vertex/" + vert + ".vert", "shaders/fragment/" + frag + ".frag");
}

PTA_LVecBase3f Renderer::generate_ssao_samples(int numberOfSamples)
{
    auto lerp = [](float a, float b, float f) -> float
    {
        return a + f * (b - a);
    };

    PTA_LVecBase3f ssaoSamples = PTA_LVecBase3f();

    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    for (int i = 0; i < numberOfSamples; ++i) 
    {
        LVecBase3f sample = LVecBase3f(distribution(generator) * 2.0 - 1.0, distribution(generator) * 2.0 - 1.0, distribution(generator)).normalized();

        float rand = distribution(generator);
        sample[0] *= rand;
        sample[1] *= rand;
        sample[2] *= rand;

        float scale = (float)i / (float)numberOfSamples;
        scale = lerp(0.1, 1.0, scale * scale);
        sample[0] *= scale;
        sample[1] *= scale;
        sample[2] *= scale;

        ssaoSamples.push_back(sample);
    }

    return ssaoSamples;
}

PTA_LVecBase3f Renderer::generate_ssao_noise(int numberOfNoise) 
{
    PTA_LVecBase3f ssaoNoise = PTA_LVecBase3f();

    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    for (int i = 0; i < numberOfNoise; ++i)
    {
        LVecBase3f noise = LVecBase3f(distribution(generator) * 2.0 - 1.0, distribution(generator) * 2.0 - 1.0, 0.0);
        ssaoNoise.push_back(noise);
    }

    return ssaoNoise;
}

void Renderer::show_buffer(NodePath render2d, std::tuple<std::string, PT(GraphicsOutput), int> bufferTexture, bool alpha)
{
    hide_buffer(render2d);

    std::string bufferName;
    PT(GraphicsOutput) buffer;
    int texture;
    std::tie(bufferName, buffer, texture) = bufferTexture;

    NodePath nodePath = buffer->get_texture_card();
    nodePath.set_texture(buffer->get_texture(texture));
    nodePath.reparent_to(render2d);
    nodePath.set_y(0);

    if (alpha)
        nodePath.set_transparency(TransparencyAttrib::Mode::M_alpha);
}

void Renderer::hide_buffer(NodePath render2d)
{
    NodePath nodePath = render2d.find("**/texture card");
    if (nodePath)
        nodePath.detach_node();
}

void Renderer::set_texture_to_nearest_and_clamp(PT(Texture) texture)
{
    texture->set_magfilter(SamplerState::FT_nearest);
    texture->set_minfilter(SamplerState::FT_nearest);
    texture->set_wrap_u(SamplerState::WM_clamp);
    texture->set_wrap_v(SamplerState::WM_clamp);
    texture->set_wrap_w(SamplerState::WM_clamp);
}

LColor Renderer::mix_color(LColor a, LColor b, float factor)
{
    return a * (1 - factor) + b * factor;
}

LVecBase2f Renderer::make_enabled_vec(int t)
{
    if (t >= 1)
        t = 1;
    else 
        t = 0;

    return LVecBase2f(t, t);
}

LVecBase2f Renderer::toggle_enabled_vec(LVecBase2f vec)
{
    int t = vec[0];

    if (t >= 1)
        t = 0;
    else 
        t = 1;

    vec[0] = t;
    vec[1] = t;

    return vec;
}

double Renderer::microsecond_to_second(int m)
{
    return m / 1000000.0;
}

int Renderer::microseconds_since_epoch()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

Renderer::FramebufferTexture Renderer::generate_framebuffer_texture(const FramebufferTextureArguments& framebufferTextureArguments)
{
    PT(GraphicsOutput)                 graphicsOutput = framebufferTextureArguments.graphicsOutput;
    PT(GraphicsEngine)                 graphicsEngine = framebufferTextureArguments.graphicsEngine;
    LVecBase4                          rgbaBits = framebufferTextureArguments.rgbaBits;
    GraphicsOutput::RenderTexturePlane bitplane = framebufferTextureArguments.bitplane;
    int                                aux_rgba = framebufferTextureArguments.aux_rgba;
    bool                               setFloatColor = framebufferTextureArguments.setFloatColor;
    bool                               setSrgbColor = framebufferTextureArguments.setSrgbColor;
    bool                               setRgbColor = framebufferTextureArguments.setRgbColor;
    bool                               useScene = framebufferTextureArguments.useScene;
    std::string                        name = framebufferTextureArguments.name;
    LColor                             clearColor = framebufferTextureArguments.clearColor;

    FrameBufferProperties fbp = FrameBufferProperties::get_default();
    fbp.set_back_buffers(0);
    fbp.set_rgba_bits(rgbaBits[0], rgbaBits[1], rgbaBits[2], rgbaBits[3]);
    fbp.set_aux_rgba(aux_rgba);
    fbp.set_float_color(setFloatColor);
    fbp.set_srgb_color(setSrgbColor);
    fbp.set_rgb_color(setRgbColor);

    PT(GraphicsOutput) buffer = graphicsEngine->make_output(
        graphicsOutput->get_pipe(),
        name + "Buffer",
        BACKGROUND_RENDER_SORT_ORDER - 1,
        fbp,
        WindowProperties::size(0, 0),
        GraphicsPipe::BF_refuse_window | GraphicsPipe::BF_resizeable | GraphicsPipe::BF_can_bind_every | GraphicsPipe::BF_rtt_cumulative | GraphicsPipe::BF_size_track_host,
        graphicsOutput->get_gsg(), graphicsOutput->get_host());

    buffer->add_render_texture(NULL, GraphicsOutput::RTM_bind_or_copy, bitplane);
    buffer->set_clear_color(clearColor);

    NodePath   cameraNP = NodePath("");
    PT(Camera) camera = NULL;

    if (false && useScene)
    {
        /*
        cameraNP = window->make_camera();
        camera = DCAST(Camera, cameraNP.node());
        camera->set_lens(window->get_camera(0)->get_lens());
        */
    }
    else 
    {
        camera = new Camera(name + "Camera");
        PT(OrthographicLens) lens = new OrthographicLens();
        lens->set_film_size(2, 2);
        lens->set_film_offset(0, 0);
        lens->set_near_far(-1, 1);
        camera->set_lens(lens);
        cameraNP = NodePath(camera);
    }

    PT(DisplayRegion) bufferRegion = buffer->make_display_region(0, 1, 0, 1);
    bufferRegion->set_camera(cameraNP);

    NodePath shaderNP = NodePath(name + "Shader");

    if (!useScene)
    {
        NodePath renderNP = NodePath(name + "Render");
        renderNP.set_depth_test(false);
        renderNP.set_depth_write(false);
        cameraNP.reparent_to(renderNP);
        CardMaker card = CardMaker(name);
        card.set_frame_fullscreen_quad();
        card.set_has_uvs(true);
        NodePath cardNP = NodePath(card.generate());
        cardNP.reparent_to(renderNP);
        cardNP.set_pos(0, 0, 0);
        cardNP.set_hpr(0, 0, 0);
        cameraNP.look_at(cardNP);
    }

    FramebufferTexture result;
    result.buffer = buffer;
    result.bufferRegion = bufferRegion;
    result.camera = camera;
    result.cameraNP = cameraNP;
    result.shaderNP = shaderNP;
    return result;
}