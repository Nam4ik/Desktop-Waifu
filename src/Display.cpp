#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <map>


const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 100;
uniform mat4 gBones[MAX_BONES];

void main()
{
    mat4 boneTransform = gBones[aBoneIDs[0]] * aWeights[0];
    boneTransform += gBones[aBoneIDs[1]] * aWeights[1];
    boneTransform += gBones[aBoneIDs[2]] * aWeights[2];
    boneTransform += gBones[aBoneIDs[3]] * aWeights[3];
    
    vec4 posL = boneTransform * vec4(aPos, 1.0);
    gl_Position = projection * view * model * posL;
    TexCoord = aTexCoord;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform float alphaThreshold = 0.1;

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoord);
    if(texColor.a < alphaThreshold)
        discard;
    FragColor = texColor;
}
)";

class Shader {
public:
    unsigned int ID;
    Shader(const char* vertexCode, const char* fragmentCode) {
        // [Реализация компиляции шейдеров...]
    }
    void use() { glUseProgram(ID); }
    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ... другие методы
};

struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
    glm::ivec4 BoneIDs;
    glm::vec4 Weights;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO, VBO, EBO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
        // [Инициализация буферов...]
    }
    void Draw(Shader &shader) {
        // [Рендеринг меша...]
    }
};

class Model {
public:
    std::vector<Mesh> meshes;
    std::string directory;
    const aiScene* scene;
    std::map<std::string, unsigned int> boneMapping;
    std::vector<glm::mat4> boneMatrices;
    Assimp::Importer importer;

    Model(std::string const &path) {
        scene = importer.ReadFile(path, 
            aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
        
        // [Загрузка мешей и анимаций...]
    }
    
    void LoadBones(const aiMesh* mesh) {
        // [Загрузка данных о костях...]
    }
    
    void UpdateAnimation(float timeInSeconds) {
        // [Обновление анимации...]
    }
    
    void Draw(Shader &shader) {
        for(Mesh &mesh : meshes)
            mesh.Draw(shader);
    }
};

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_Window* window = SDL_CreateWindow(
        "Transparent 3D Model",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_TRANSPARENT
    );
    
    // Включение прозрачности окна
    SDL_SetWindowOpacity(window, 0.0f); // Прозрачность фона
    SDL_GLContext context = SDL_GL_CreateContext(window);
    gladLoadGLLoader(SDL_GL_GetProcAddress);

    // Настройка OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader(vertexShaderSource, fragmentShaderSource);
    Model ourModel("model.fbx"); // Замените на путь к вашей модели

    // Основной цикл
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || 
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
                quit = true;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Преобразования камеры
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0, 2, 5), glm::vec3(0,0,0), glm::vec3(0,1,0));
        glm::mat4 model = glm::mat4(1.0f);

        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);

        // Обновление анимации
        if(ourModel.scene->HasAnimations()) {
            static float time = 0.0f;
            time += 0.016f; // Пример: 60 FPS
            ourModel.UpdateAnimation(time);
        }

        ourModel.Draw(shader);

        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

