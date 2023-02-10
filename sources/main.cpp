#include <iostream>
#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "platform.hpp"

#ifdef __unix___
#define INFO(X...)  TraceLog(LOG_INFO,X)
#define WARN(X...)  TraceLog(LOG_WARNING,X)
#define DEBUG(X...) TraceLog(LOG_DEBUG,X)
#define ERROR(X...) TraceLog(LOG_ERROR,X)

#elif defined(_WIN32) || defined(WIN32)

#define INFO(...)  TraceLog(LOG_INFO,__VA_ARGS__)
#define WARN(...)  TraceLog(LOG_WARNING,__VA_ARGS__)
#define DEBUG(...) TraceLog(LOG_DEBUG,__VA_ARGS__)
#define ERROR(...) TraceLog(LOG_ERROR,__VA_ARGS__)
#endif

constexpr const char* APP_TITLE = "Behold - Image Viewer";

static void DrawBanner(const char* text, float fontSize=18.f, Color color=WHITE);

struct LoadedImage {
    std::string filePath;
    Texture texture;
};

struct Session {
    std::vector<LoadedImage> images;

    int LoadImage(std::string path){
        // return id if already loaded
        int index = 0;
        for (const auto& img : images)
        {
            if (img.filePath == path)
            {
                INFO("Image %s was already loaded.",path.c_str());
                return index;
            }
            index++;
        }

        index = 0;
        if (IsPathFile(path.c_str()))
        {
            const char* ext = GetFileExtension(path.c_str());
            if (TextIsEqual(ext,".exe"))
                return -1;

            Texture texture = LoadTexture(path.c_str());
            if (texture.width > 0)
            {
                // load image into system
                LoadedImage img;
                img.filePath = path;
                img.texture = texture;

                index = (int) images.size();
                images.push_back(img);
            }
            else
            {
                index = -1;
            }
        }
        else // crawl directory
        { 
            FilePathList list = LoadDirectoryFiles(path.c_str());       // Load directory filepaths
            for (unsigned int i = 0; i < list.count; i++)
            {
                std::string path = list.paths[i];                   // Filepaths entries
                index = LoadImage(path);
            }
            UnloadDirectoryFiles(list);
        }
        return index;
    }

    bool HasImages() {
        return images.size() > 0;
    }
};

inline Vector2 LayoutImage(Texture& texture) {
    return {
        GetScreenWidth() * 0.5f - texture.width * 0.5f,
        GetScreenHeight() * 0.5f - texture.height * 0.5f,
    };
}

static float ZoomPercent = 0.5f;
static int SelectedIndex = 0;

void DisplayImage(Texture texture){
    Vector2 pos = { -texture.width*0.5f, -texture.height*0.5f };
    DrawTextureEx(texture, pos, 0.f, 1.f, WHITE);
}

void RepositionCamera(Camera2D& camera){

    Vector2 offset = {
        GetScreenWidth() * 0.5f,
        GetScreenHeight() * 0.5f,
    };

    camera.offset = offset;         // Camera offset (displacement from target)
    camera.target = Vector2Zero();         // Camera target (rotation and zoom origin)

    float minZoom = 0.1f;
    float maxZoom = 10.f;
    float actualZoom = exp( log(minZoom) + (log(maxZoom)-log(minZoom))*ZoomPercent );
    camera.zoom = actualZoom;
}

bool Start(std::vector<std::string>& args){

    SetTraceLogLevel(LOG_DEBUG);
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(640, 480, APP_TITLE);
    SetTargetFPS(60);

    Session session = Session();

    Camera2D camera;
    RepositionCamera(camera);
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    for (const auto& arg : args){
        std::cout << arg << std::endl;
        // load all image of arg
        session.LoadImage(arg);
    }

    float scroll = 0.f;
    static std::string curPath = "none";

    while (!WindowShouldClose())
    {
        if (IsWindowResized()){
            RepositionCamera(camera);
        }

        BeginDrawing();

        BeginMode2D(camera);

        ClearBackground(BLACK);
        if (session.HasImages()){
            LoadedImage img = session.images.at(SelectedIndex);
            curPath = img.filePath;
            DisplayImage(img.texture);

            scroll = GetMouseWheelMove();
            if (fabs(scroll) > EPSILON){
                ZoomPercent += scroll*0.05f;
                ZoomPercent = Clamp(ZoomPercent,0.0f,1.f);
                RepositionCamera(camera);
            }

            static bool changed = true;
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)){
                SelectedIndex++;
                changed = true;
            }
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_BACKSPACE)){
                SelectedIndex--;
                changed = true;
            }
            SelectedIndex = Wrap(SelectedIndex,0,session.images.size());
            if (changed) {
                SetWindowTitle(TextFormat("%s - %s",APP_TITLE,curPath.c_str()));
            }
            changed = false;
        }
        else
        {
            DrawBanner("No image opened...\nDrag-in an image to view it!");
        }

        EndMode2D();

        DrawText(TextFormat("Scroll %f\nImage %d of %d\n%s",
                            camera.zoom,SelectedIndex+1,
                            session.images.size(),curPath.c_str()),
                 0,0,16,RED);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

// TODO: replace with winmain
int main(int argc, char **argv)
{
    OpenDebugConsole();

    // pack arguments
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++){
       args.push_back(argv[i]);
    }

    int code = Start(args);
    CloseDebugConsole();
    return code;
}

static void DrawBanner(const char* text, float fontSize, Color color){
    Font font = GetFontDefault();
    Vector2 size = MeasureTextEx(font, text, fontSize, 1.f);
    Vector2 pos = {
        GetScreenWidth()*0.5f - size.x * 0.5f,
        GetScreenHeight()*0.5f - size.y * 0.5f
    };
    DrawTextEx(font, text, pos, fontSize, 1.f, color);
}
