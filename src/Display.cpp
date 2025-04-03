#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

class Model {
public:
    std::vector<float> vertices;
    
    bool Load(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, 
            aiProcess_Triangulate | aiProcess_GenNormals);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            return false;
        }
        
        ProcessNode(scene->mRootNode, scene);
        return true;
    }

private:
    void ProcessNode(aiNode* node, const aiScene* scene) {
        for (unsigned i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessMesh(mesh);
        }
        
        for (unsigned i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    void ProcessMesh(aiMesh* mesh) {
        for (unsigned i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned j = 0; j < face.mNumIndices; j++) {
                aiVector3D v = mesh->mVertices[face.mIndices[j]];
                vertices.push_back(v.x);
                vertices.push_back(v.y);
                vertices.push_back(v.z);
            }
        }
    }
};

int Main3DWorker(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    

    SDL_Window* window = SDL_CreateWindow(
        "3D Viewer", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_OPENGL | SDL_ALPHA_TRANSPARENT | SDL_WINDOW_BORDERLESS 
    );
    
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 

    Model model;
    std::ifstream file("data.json");
  
    json data = json::parse(file);
    

    if (!model.Load(data["3DM_CURR_PATH"])) { 
        SDL_Log("Failed to load model!");
        return 1;
    }


    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || 
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 800.0/600.0, 0.1, 100.0);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
        
        glColor4f(1.0f, 0.5f, 0.2f, 0.5f); // Полупрозрачный оранжевый
        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < model.vertices.size(); i += 3) {
            glVertex3f(model.vertices[i], model.vertices[i+1], model.vertices[i+2]);
        }
        glEnd();
        
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int MainGIFWorker(){

}