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


class ResourceManager {
public:
    ResourceManager();
	
	/*
    NodePath load_model(
        const std::string& path,
		bool is_editor_resource, = false
        const LoaderOptions& loader_options = LoaderOptions(),
        bool use_cached = false,
        bool instanced  = false);
	*/

	NodePath load_model(const std::string& path);

	NodePath load_model(
        const std::string& path,
        const LoaderOptions& loader_options);
	
	PT(Texture) load_texture(const std::string& path);
	
	PT(Texture) load_texture(
		const std::string& path,
		bool readMipmaps);
	
	PT(Texture) load_texture(
		const std::string& path,
		const LoaderOptions& loader_options);
		
	PT(Texture) load_texture(
		const std::string& path,
		const LoaderOptions& loader_options,
		bool readMipmaps);

    void load_font(const std::string& font);
    void load_sound(const std::string& sound);

    Loader* get_loader() const;

private:
    Loader* _loader;
};

#endif // RESOURCE_HANDLER_H
