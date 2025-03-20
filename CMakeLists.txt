#cmake最低版本要求
cmake_minimum_required(VERSION 3.28)
#项目名称
project(txma)
#设置c++标准
set(CMAKE_CXX_STANDARD 20)
#包含目录
include_directories("E:/opencv/opencv/build/include")  # 修改为你的 OpenCV 路径
include_directories("E:/opencv/opencv/build/include/opencv2")
#库目录
link_directories("E:/opencv/opencv/build/x64/vc14/lib")
#生成可执行文件
add_executable(txma main.cpp
        Barcode.cpp)
#链接静态库
target_link_libraries(txma opencv_world453d.lib)
