
#ifndef ENGINE_STYLEMANAGER_H
#define ENGINE_STYLEMANAGER_H


#include <string>

class StyleManager {
public:
    StyleManager();

    void LoadStyle(const std::string& stylePath);
    void SaveStyle();
private:
    std::string stylePath;
};


#endif //ENGINE_STYLEMANAGER_H
