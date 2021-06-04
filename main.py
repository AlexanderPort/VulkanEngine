import os


SOURCES, OBJECTS = [], []
PROJECT = "C:/Games/VulkanEngine"


def get_sources(PATH):
    for path in os.listdir(PATH):
        path = f'{PATH}/{path}'
        if os.path.isfile(path):
            if '.cpp' in path:
                SOURCES.append(path)
                OBJECTS.append(f"{PROJECT}/{path.split('/')[-1].split('.')[0]}.o")
        else: get_sources(path)


if __name__ == '__main__':
    compiler = "g++"
    INCLUDES = [
        "-IC:/Libs/glm",
        "-IC:/Libs/glfw-3.3.4.bin.WIN64/include",
        "-IC:/Libs/glm -IC:/VulkanSDK/1.2.176.1/Include",
    ]
    LIBRARIES = [
        "-LC:/Libs/glfw-3.3.4.bin.WIN64/lib-mingw-w64",
        "-LC:/VulkanSDK/1.2.176.1/Lib",
    ]

    get_sources(f"{PROJECT}/src")
   
    os.system("cd src/shaders && glslc shader.vert -o vert.spv")
    os.system("cd src/shaders && glslc shader.frag -o frag.spv")

    SOURCES = list(sorted(SOURCES, key=lambda path: path.split('.')[-1]))
    INCLUDES, LIBRARIES = ' '.join(INCLUDES), ' '.join(LIBRARIES)
    SOURCES, OBJECTS = ' '.join(SOURCES), ' '.join(OBJECTS)

    os.system(f"g++ -std=c++17 -O2 -fexceptions -g {INCLUDES} -c {SOURCES}")
    os.system(f"g++ -O2 {LIBRARIES} -o main.exe {OBJECTS} -lglfw3 -lvulkan-1 -lgdi32")

    for path in OBJECTS.split(): os.remove(path)

    os.system("main.exe")
