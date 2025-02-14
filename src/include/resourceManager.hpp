#ifndef RESOURCE_HANDLER_H
#define RESOURCE_HANDLER_H

#include <vector>
#include <string>
#include <unordered_map>


class NodePath;
class Texture;

class ResourceManager {
public:
    ResourceManager();
	
	NodePath load_model(const std::string& path);

	NodePath load_model(
        const std::string& path,
        const LoaderOptions& loader_options);
	
	PT(Texture) load_texture(const std::string& path, bool isCubeMap = false);
	
	PT(Texture) load_texture(
		const std::string& path,
		bool readMipmaps,
		bool isCubeMap = false);
	
	PT(Texture) load_texture(
		const std::string& path,
		const LoaderOptions& loader_options,
		bool isCubeMap = false);
		
	PT(Texture) load_texture(
		const std::string& path,
		const LoaderOptions& loader_options,
		bool readMipmaps,
		bool isCubeMap);

    void load_font(const std::string& font);
    void load_sound(const std::string& sound);

    PT(Loader) get_loader() const;

private:
    PT(Loader) _loader;
};

#endif // RESOURCE_HANDLER_H
