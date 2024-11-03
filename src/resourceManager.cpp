#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "include/resourceManager.h"
#include "utils/include/pathUtils.h"


ResourceManager::ResourceManager() {
    _loader = Loader::get_global_ptr();
}

NodePath ResourceManager::load_model(
    const std::string& path) {
	
	LoaderOptions options = LoaderOptions();
	// options.set_flags(options.get_flags() & ~LoaderOptions::LF_report_errors);
	options.set_flags(options.get_flags() & ~LoaderOptions::LF_no_cache);
	options.set_flags(options.get_flags() & ~LoaderOptions::LF_allow_instance);
	
	return ResourceManager::load_model(path, options);
}

NodePath ResourceManager::load_model(
    const std::string& path,
    const LoaderOptions& options) {
	
	NodePath result;
	
	PT(PandaNode) node = _loader->load_sync(Filename(path), options);
	if(node)
		result = NodePath(node);
	return result;
	
	/*
	std::string path_on_disk = PathUtils::join_paths(PathUtils::get_current_working_dir(), std::string("game/").append(path.c_str()));
	path_on_disk = PathUtils::join_paths(PathUtils::get_current_working_dir(), path.c_str());
	if (PathUtils::is_file(path_on_disk.c_str()))
	*/
}

PT(Texture) ResourceManager::load_texture(const std::string& path, bool isCubeMap) {

	LoaderOptions options = LoaderOptions();
	return ResourceManager::load_texture(path, options, false, isCubeMap);
}

PT(Texture) ResourceManager::load_texture(
	const std::string& path,
	bool readMipmaps,
	bool isCubeMap) {

	LoaderOptions options = LoaderOptions();
	return ResourceManager::load_texture(path, options, readMipmaps, isCubeMap);
}

PT(Texture) ResourceManager::load_texture(
	const std::string& path,
	const LoaderOptions& options,
	bool isCubeMap) {
		
	return ResourceManager::load_texture(path, options, false, isCubeMap);
}
	
PT(Texture) ResourceManager::load_texture(
	const std::string& path,
	const LoaderOptions& options,
	bool readMipmaps,
	bool isCubeMap) {
		
	if(isCubeMap) {

		// Load a cube map texture
		std::string texture_pattern = ""; // This should be set appropriately
		return TexturePool::load_cube_map(Filename(texture_pattern), readMipmaps, options);
	}
	else {
		
		return TexturePool::load_texture(Filename(path), 0, readMipmaps, options);
	}
}

/*
PT(Texture) ResourceManager::load_texture(
    const std::string& path,
    const LoaderOptions& loader_options,
    SamplerState::FilterType minfilter,
    SamplerState::FilterType magfilter,
    bool cube_map,
    bool readMipmaps,
	bool multiview,
    int anisotropicDegree) {

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
*/

void ResourceManager::load_font(const std::string& font) {
    // Implement font loading logic here
}

void ResourceManager::load_sound(const std::string& sound) {
    // Implement sound loading logic here
}

Loader* ResourceManager::get_loader() const {
    return _loader;
}