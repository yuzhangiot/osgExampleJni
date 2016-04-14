/*
 * OsgMainApp.hpp
 *
 *  Created on: 29/05/2011
 *      Author: Jorge Izquierdo Ciges
 */

#ifndef OSGMAINAPP_HPP_
#define OSGMAINAPP_HPP_

//Android log
#include <android/log.h>
#include <iostream>
#include <cstdlib>
#include <math.h>

//Standard libraries
#include <string>

//osg
#include <osg/GL>
#include <osg/GLExtensions>
#include <osg/Depth>
#include <osg/Program>
#include <osg/Shader>
#include <osg/Node>
#include <osg/Notify>
//osgText
#include <osgText/Text>
//osgDB
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
//osg_viewer
#include <osgViewer/Viewer>
 #include <osgViewer/CompositeViewer>
#include <osgViewer/Renderer>
#include <osgViewer/ViewerEventHandlers>
//osgGA
#include <osgGA/GUIEventAdapter>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/SphericalManipulator>
//Self headers
#include "OsgAndroidNotifyHandler.hpp"
//osgmovie includes
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/TextureCubeMap>
#include <osg/TexMat>
#include <osg/CullFace>
#include <osg/Image>
#include <osg/ImageStream>
#include <osg/io_utils>
#include <osg/ShapeDrawable>
#include <osg/Transform>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/Camera>
#include <osg/Vec3>
#include <osg/PositionAttitudeTransform>

#include <osgDB/FileUtils>
#include <osgDB/fstream>

#include <osgGA/EventVisitor>

//Static plugins Macro
USE_OSGPLUGIN(ive)
USE_OSGPLUGIN(osg)
USE_OSGPLUGIN(osg2)
USE_OSGPLUGIN(terrain)
USE_OSGPLUGIN(rgb)
USE_OSGPLUGIN(OpenFlight)
USE_OSGPLUGIN(dds)
USE_OSGPLUGIN(jpeg)
USE_OSGPLUGIN(png)
USE_OSGPLUGIN(ffmpeg)
//Static DOTOSG
USE_DOTOSGWRAPPER_LIBRARY(osg)
USE_DOTOSGWRAPPER_LIBRARY(osgFX)
USE_DOTOSGWRAPPER_LIBRARY(osgParticle)
USE_DOTOSGWRAPPER_LIBRARY(osgTerrain)
USE_DOTOSGWRAPPER_LIBRARY(osgText)
USE_DOTOSGWRAPPER_LIBRARY(osgViewer)
USE_DOTOSGWRAPPER_LIBRARY(osgVolume)
//Static serializer
USE_SERIALIZER_WRAPPER_LIBRARY(osg)
USE_SERIALIZER_WRAPPER_LIBRARY(osgAnimation)
USE_SERIALIZER_WRAPPER_LIBRARY(osgFX)
USE_SERIALIZER_WRAPPER_LIBRARY(osgManipulator)
USE_SERIALIZER_WRAPPER_LIBRARY(osgParticle)
USE_SERIALIZER_WRAPPER_LIBRARY(osgTerrain)
USE_SERIALIZER_WRAPPER_LIBRARY(osgText)
USE_SERIALIZER_WRAPPER_LIBRARY(osgVolume)

#define  LOG_TAG    "osgNativeLib"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

struct Model{
    std::string filename;
    std::string name;
};

struct Movie
{
    std::string filename;
    int type; // 0 = equiretangular, 1 = fisheye, 2 = cubic
};

static const char gVertexShader[] =
    "varying vec4 color;                                                    \n"
    "const vec3 lightPos      =vec3(0.0, 0.0, 10.0);                        \n"
    "const vec4 cessnaColor   =vec4(0.8, 0.8, 0.8, 1.0);                    \n"
    "const vec4 lightAmbient  =vec4(0.1, 0.1, 0.1, 1.0);                    \n"
    "const vec4 lightDiffuse  =vec4(0.4, 0.4, 0.4, 1.0);                    \n"
    "const vec4 lightSpecular =vec4(0.8, 0.8, 0.8, 1.0);                    \n"
    "void DirectionalLight(in vec3 normal,                                  \n"
    "                      in vec3 ecPos,                                   \n"
    "                      inout vec4 ambient,                              \n"
    "                      inout vec4 diffuse,                              \n"
    "                      inout vec4 specular)                             \n"
    "{                                                                      \n"
    "     float nDotVP;                                                     \n"
    "     vec3 L = normalize(gl_ModelViewMatrix*vec4(lightPos, 0.0)).xyz;   \n"
    "     nDotVP = max(0.0, dot(normal, L));                                \n"
    "                                                                       \n"
    "     if (nDotVP > 0.0) {                                               \n"
    "       vec3 E = normalize(-ecPos);                                     \n"
    "       vec3 R = normalize(reflect( L, normal ));                       \n"
    "       specular = pow(max(dot(R, E), 0.0), 16.0) * lightSpecular;      \n"
    "     }                                                                 \n"
    "     ambient  = lightAmbient;                                          \n"
    "     diffuse  = lightDiffuse * nDotVP;                                 \n"
    "}                                                                      \n"
    "void main() {                                                          \n"
    "    vec4 ambiCol = vec4(0.0);                                          \n"
    "    vec4 diffCol = vec4(0.0);                                          \n"
    "    vec4 specCol = vec4(0.0);                                          \n"
    "    gl_Position   = gl_ModelViewProjectionMatrix * gl_Vertex;          \n"
    "    vec3 normal   = normalize(gl_NormalMatrix * gl_Normal);            \n"
    "    vec4 ecPos    = gl_ModelViewMatrix * gl_Vertex;                    \n"
    "    DirectionalLight(normal, ecPos.xyz, ambiCol, diffCol, specCol);    \n"
    "    color = cessnaColor * (ambiCol + diffCol + specCol);               \n"
    "}                                                                      \n";

static const char gFragmentShader[] =
    "precision mediump float;                  \n"
    "varying mediump vec4 color;               \n"
    "void main() {                             \n"
    "  gl_FragColor = color;                   \n"
    "}                                         \n";
//fixed shader
static const char gVertexShaderfix[] =
        "varying vec2 v_texCoord;                                         \n"
        "void main() {                                                          \n"
        "    gl_Position  = gl_ModelViewProjectionMatrix * gl_Vertex;           \n"
        "         v_texCoord   = gl_MultiTexCoord0.xy;                                                        \n"
        "}  						\n";

static const char gFragmentShaderfix[] =
        "varying  mediump vec2 v_texCoord;                                                                  \n"
        "uniform sampler2D sam;                                                                                                        \n"
        "void main() {                                                                                                                        \n"
        "gl_FragColor = texture2D(sam, v_texCoord);                                                                        \n"
        "}  					\n";

static const char shaderSourceTexture2D[] =
            "uniform vec4 cutoff_color;													\n"
            "uniform sampler2D movie_texture;															\n"
            "void main(void)\n"
            "{																					\n"
            "    vec4 texture_color = texture2D(movie_texture, gl_TexCoord[0].st);						 \n"
            "    if (all(lessThanEqual(texture_color,cutoff_color))) discard; 								\n"
            "    gl_FragColor = texture_color;									\n"
            "}									\n";

static const char shaderSourceTextureRec[] =
            "uniform vec4 cutoff_color;			\n"
            "uniform samplerRect movie_texture;			\n"
            "void main(void)			\n"
            "{			\n"
            "    vec4 texture_color = textureRect(movie_texture, gl_TexCoord[0].st);			 \n"
            "    if (all(lessThanEqual(texture_color,cutoff_color))) discard; 			\n"
            "    gl_FragColor = texture_color;			\n"
            "}		\n";



class OsgMainApp{
private:
    //define the composite viewer
    osg::ref_ptr<osgViewer::CompositeViewer> _viewerMulti;
    //define the left and right viewers
    osg::ref_ptr<osgViewer::View> _viewerL;
    osg::ref_ptr<osgViewer::View> _viewerR;
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _gweL;
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _gweR;
    osg::ref_ptr<osg::Group> _root;
    osg::ref_ptr<osg::StateSet> _state;
    osg::ref_ptr<osgGA::SphericalManipulator> _manipulator;
    // osg::ref_ptr<osgGA::OrbitManipulator> _manipulator;
    // osg::ref_ptr<osgGA::SphericalManipulator> _manipulator;


    float _lodScale;
    unsigned int _prevFrame;

    bool _initialized;
    bool _clean_scene;
    //This bool val is a indicate of whether the equtangular is displayed
    bool equ_display;

    OsgAndroidNotifyHandler *_notifyHandler;

    std::vector<Model> _vModels;
    std::vector<Model> _vModelsToLoad;
    std::vector<Model> _vModelsToDelete;

    //define the equiretangular movie name
    Movie mMovie;

    void deleteModels();

    void movieSample();
    osg::ShapeDrawable* myCreateTexturedSphereGeometry(const osg::Vec3& pos,float width,float height, osg::Image* image, bool useTextureRectangle, bool xyPlane, bool option_flip);
    osg::Geometry* myCreateTexturedSphereByHandGeometry(const osg::Vec3& pos,float width,float height, osg::Image* image, bool useTextureRectangle, bool xyPlane, bool option_flip);

public:
    OsgMainApp();
    ~OsgMainApp();

    //Initialization function
    void initOsgWindow(int x,int y,int width,int height);
    //Draw
    void draw();
    //Events
    void mouseButtonPressEvent(float x,float y,int button);
    void mouseButtonReleaseEvent(float x,float y,int button);
    void mouseMoveEvent(float x,float y);
    void keyboardDown(int key);
    void keyboardUp(int key);
    //Loading and unloading
    void loadObject(std::string filePath);
    void loadObject(std::string name,std::string filePath);
    void unLoadObject(int number);
    void clearScene();
    //Loading  movies
    void loadMovie(std::string filePath, int type);
    //Other functions
    int  getNumberObjects();
    std::string getObjectName(int nunmber);
    //set the movie display again
    void onMovieResume();
    void changeCamViewQuat(float x, float y, float z, float w);

    void setClearColor(osg::Vec4f color);
    osg::Vec4f getClearColor();
};


#endif /* OSGMAINAPP_HPP_ */
