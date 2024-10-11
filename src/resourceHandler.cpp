#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "include/resourceHandler.h"
#include "utils/include/pathUtils.h"


ResourceHandler::ResourceHandler() {
    _loader = Loader::get_global_ptr();
}

NodePath ResourceHandler::load_model(
    const std::string& path,
    const LoaderOptions& loader_options,
    bool use_cached,
    bool instanced,
    bool create_np,
	bool is_editor_resource) {
		
    NodePath result;
	std::string path_on_disk;
	
	if(!is_editor_resource)
		path_on_disk = PathUtils::join_paths(PathUtils::get_current_working_dir(), std::string("game/").append(path.c_str()));
	else
		path_on_disk = PathUtils::join_paths(PathUtils::get_current_working_dir(), path.c_str());

    if (PathUtils::is_file(path_on_disk.c_str())) {
        std::string ext = path.substr(path.find_last_of(".") + 1);
        if (ext == "gltf" || ext == "glb") {
            // load gltf or glb here
        }
        else {
            LoaderOptions options = loader_options;
            options.set_flags(options.get_flags() & ~LoaderOptions::LF_report_errors);

            if (use_cached) {
                options.set_flags(options.get_flags() & ~LoaderOptions::LF_no_cache);
            }

            if (instanced) {
                options.set_flags(options.get_flags() | LoaderOptions::LF_allow_instance);
            }

            PT(PandaNode) node = _loader->load_sync(Filename(path), options);
            if (create_np) {
                result = NodePath(node);
            }
        }
    }
    else {
        std::cerr << "Not a file " << path_on_disk << std::endl;
    }

    return result;
}

PT(Texture) ResourceHandler::load_texture(
    const std::string& path,
    const LoaderOptions& loader_options,
    SamplerState::FilterType minfilter,
    SamplerState::FilterType magfilter,
    bool cube_map,
    bool readMipmaps,
    int  anisotropicDegree,
    bool multiview) {

    PT(Texture) texture = nullptr;

    std::ifstream file(path);
    if (file.is_open()) {
        file.close();

        LoaderOptions options = loader_options;

        if (multiview) {
            int flags = options.get_texture_flags();
            flags |= LoaderOptions::TF_multiview;
            options.set_texture_flags(flags);
        }

        if (!cube_map) {
            // Load a 2D texture
            texture = TexturePool::load_texture(Filename(path), 0, readMipmaps, options);
        }
        else {
            // Load a cube map texture
            std::string texture_pattern = ""; // This should be set appropriately
            texture = TexturePool::load_cube_map(Filename(texture_pattern), readMipmaps, options);
        }

        if (texture) {
            if (minfilter != SamplerState::FT_linear) {
                texture->set_minfilter(minfilter);
            }
            if (magfilter != SamplerState::FT_linear) {
                texture->set_magfilter(magfilter);
            }
            texture->set_anisotropic_degree(anisotropicDegree);
        }
    }
    else {
        std::cerr << "Unable to load texture, path is not a file " << path << std::endl;
    }

    return texture;
}

void ResourceHandler::load_font(const std::string& font) {
    // Implement font loading logic here
}

void ResourceHandler::load_sound(const std::string& sound) {
    // Implement sound loading logic here
}

Loader* ResourceHandler::get_loader() const {
    return _loader;
}