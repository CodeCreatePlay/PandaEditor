#include <loader.h>
#include <loaderOptions.h>
#include <nodePath.h>
#include <texture.h>
#include <texturePool.h>

#include "resourceManager.hpp"
#include "pathUtils.hpp"


ResourceManager::ResourceManager() {
    _loader = Loader::get_global_ptr();
}

NodePath ResourceManager::load_model(
    const std::string& path) {
	
	LoaderOptions options = LoaderOptions();
	options.set_flags(options.get_flags() & ~LoaderOptions::LF_no_cache);
	options.set_flags(options.get_flags() & ~LoaderOptions::LF_allow_instance);
	
	return ResourceManager::load_model(path, options);
}

NodePath ResourceManager::load_model(
    const std::string& path,
    const LoaderOptions& options) {
	
	NodePath result;
	
	PT(PandaNode) node = _loader->load_sync(PathUtils::to_engine_specific(path), options);
	if(node)
		result = NodePath(node);
	return result;
}

PT(Texture) ResourceManager::load_texture(const std::string& path, bool isCubeMap) {

	LoaderOptions options = LoaderOptions();
	return ResourceManager::load_texture(PathUtils::to_engine_specific(path), options, false, isCubeMap);
}

PT(Texture) ResourceManager::load_texture(
	const std::string& path,
	bool readMipmaps,
	bool isCubeMap) {

	LoaderOptions options = LoaderOptions();
	return ResourceManager::load_texture(PathUtils::to_engine_specific(path), options, readMipmaps, isCubeMap);
}

PT(Texture) ResourceManager::load_texture(
	const std::string& path,
	const LoaderOptions& options,
	bool isCubeMap) {
	std::cout << path;
	return ResourceManager::load_texture(PathUtils::to_engine_specific(path), options, false, isCubeMap);
}

PT(Texture) ResourceManager::load_texture(
	const std::string& path,
	const LoaderOptions& options,
	bool readMipmaps,
	bool isCubeMap) {
		
	if(isCubeMap) {

		// Load a cube map texture
		std::string texture_pattern = ""; // This should be set appropriately
		return TexturePool::load_cube_map(PathUtils::to_engine_specific(texture_pattern), readMipmaps, options);
	}
	else {
		
		return TexturePool::load_texture(PathUtils::to_engine_specific(path), 0, readMipmaps, options);
	}
}

void ResourceManager::load_font(const std::string& font) {
    // Implement font loading logic here
}

void ResourceManager::load_sound(const std::string& sound) {
    // Implement sound loading logic here
}

PT(Loader) ResourceManager::get_loader() const {
    return _loader;
}
