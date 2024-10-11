#ifndef RESOURCE_HANDLER_H
#define RESOURCE_HANDLER_H

#include <vector>
#include <string>
#include <unordered_map>

#include <filename.h>
#include <nodePath.h>
#include <texture.h>
#include <texturePool.h>
#include <loader.h>
#include <loaderOptions.h>


class ResourceHandler {
public:
    ResourceHandler();

    NodePath load_model(
        const std::string& path,
        const LoaderOptions& loader_options = LoaderOptions(),
        bool  use_cached = false,
        bool  instanced = false,
        bool  create_np = true,
		bool is_editor_resource=false);

    PT(Texture) load_texture(
        const std::string& path,
        const LoaderOptions& loader_options = LoaderOptions(),
        SamplerState::FilterType minfilter = SamplerState::FT_linear,
        SamplerState::FilterType magfilter = SamplerState::FT_linear,
        bool cube_map = false,
        bool readMipmaps = false,
        int  anisotropicDegree = 1,
        bool multiview = false);

    void load_font(const std::string& font);
    void load_sound(const std::string& sound);

    Loader* get_loader() const;

private:
    Loader* _loader;
};

#endif // RESOURCE_HANDLER_H
