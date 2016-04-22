//#include "OsgMainApp.hpp"
#include "OsgMovieEventHandler.hpp"
#include "fisheye_spherical.h"
#include "m_warper.h"

OsgMainApp::OsgMainApp(){

    _lodScale = 1.0f;
    _prevFrame = 0;

    _initialized = false;
    _clean_scene = false;
    equ_display = false;

    mMovie.filename = "/storage/sdcard0/equ2.mp4";
    mMovie.type = 0;

}
OsgMainApp::~OsgMainApp(){

}
void OsgMainApp::movieSample(){
    //If the equ sphere has already displayed, then doing nothing
    if(equ_display == true) return;

    osg::notify(osg::ALWAYS)<<"Start to display sphere"<<std::endl;

    //defind a new geode root node, a stateSet of both eyes
    osg::ref_ptr<osg::Geode> geodeL = new osg::Geode;
    osg::ref_ptr<osg::Geode> geodeR = new osg::Geode;
    osg::ref_ptr<osg::Geode> geodeTest = new osg::Geode;
    // osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
    osg::ref_ptr<osg::PositionAttitudeTransform> patTransL = new osg::PositionAttitudeTransform();
    osg::ref_ptr<osg::PositionAttitudeTransform> patTransR = new osg::PositionAttitudeTransform();
    // osg::StateSet* statesetL = geodeL->getOrCreateStateSet();
    // osg::StateSet* statesetR = geodeR->getOrCreateStateSet();
    //define the top left point and bottom right point
    osg::Vec3 pos(0.0f,0.0f,0.0f);
    ///////////////////////////////////load the equiretangular image
    //设置纹理
    // std::string filename("/storage/sdcard0/equ2.mp4");
    osg::Image* m_image = osgDB::readImageFile(mMovie.filename);
    osg::ImageStream* m_imagestream = dynamic_cast<osg::ImageStream*>(m_image);
    //if there are audio contained in the image sequence
    if(m_imagestream){
        osg::ImageStream::AudioStreams& audioStreams = m_imagestream->getAudioStreams();
        if ( !audioStreams.empty())
        {
            osg::AudioStream* audioStream = audioStreams[0].get();
            osg::notify(osg::NOTICE)<<"AudioStream read ["<<audioStream->getName()<<"]"<<std::endl;
            //#if USE_SDL || USE_SDL2
                // if (numAudioStreamsEnabled==0)
                // {
                // audioStream->setAudioSink(new SDLAudioSink(audioStream));

                // ++numAudioStreamsEnabled;
                // }
            //#endif
        }
        m_imagestream->play();
    }
//	    if the image or video file has loaded successful
   if(m_image){
        osg::notify(osg::NOTICE)<<"image->s()"<<m_image->s()<<" image-t()="<<m_image->t()<<" aspectRatio="<<m_image->getPixelAspectRatio()<<std::endl;
        //get the width and height of the image
        float width = m_image->s() * m_image->getPixelAspectRatio();
        float height = m_image->t();
        //draw the sphere
        bool xyPlane = false;
        bool flip = true;
        bool isLefteye = true;
        ////////////////////////////draw the sphere of the left eye
        osg::ref_ptr<osg::Drawable> drawableL = myCreateTexturedSphereByHandGeometry(pos, width, height,m_image, isLefteye, xyPlane, flip);
        // osg::ref_ptr<osg::Drawable> drawableL = myCreateTexturedQuadGeometry(osg::Vec3(1, 1, 5), width, height, m_image, false, xyPlane, false);
        osg::ref_ptr<osg::Drawable> drawableR = myCreateTexturedSphereByHandGeometry(pos, width, height, m_image, !isLefteye, xyPlane, flip);
        //	    	add SphpereDrawable to geode
        geodeL->addDrawable(drawableL.get());
        geodeR->addDrawable(drawableR.get());

        ////////////////////////////////////rotate it by 90
        // osg::Quat patRotate(osg::Quat(osg::inDegrees(0.0), osg::Vec3(1.0,  0.0, 0.0)) * osg::Quat(osg::inDegrees(180.0), osg::Vec3(0.0, 1.0, 0.0)) * osg::Quat(osg::inDegrees(0.0), osg::Vec3(0.0, 0.0, 1.0)));
        osg::Quat patRotate(osg::Quat(osg::inDegrees(180.0), osg::Vec3(1.0,  0.0, 0.0)) * osg::Quat(osg::inDegrees(0.0), osg::Vec3(0.0, 1.0, 0.0)) * osg::Quat(osg::inDegrees(180.0), osg::Vec3(0.0, 0.0, 1.0)));
        const osg::Vec3 posit(0, 0, 0);
        patTransL->setPosition(posit);
        patTransL->setAttitude(patRotate);

        patTransR->setPosition(posit);
        patTransR->setAttitude(patRotate);

        patTransL->addChild(geodeL.get());
        patTransR->addChild(geodeR.get());
   }

    osg::Shader * vshader = new osg::Shader(osg::Shader::VERTEX, gVertexShaderfix );
    osg::Shader * fshader = new osg::Shader(osg::Shader::FRAGMENT, gFragmentShaderfix );

    osg::Program * prog = new osg::Program;
    prog->addShader ( vshader );
    prog->addShader ( fshader );

    geodeL->getOrCreateStateSet()->setAttribute ( prog );
    geodeR->getOrCreateStateSet()->setAttribute(  prog );
    //add geode to root
    _rootL->addChild(patTransL.get());
    _rootR->addChild(patTransR.get());


    osgViewer::Viewer::Windows windows;
    _viewerMulti->getWindows(windows);
    for(osgViewer::Viewer::Windows::iterator itr = windows.begin();itr != windows.end();++itr)
    {
        (*itr)->getState()->setUseModelViewAndProjectionUniforms(true);
        (*itr)->getState()->setUseVertexAttributeAliasing(true);
    }

    _manipulator->getNode();
    //We start make the root node distort here
    unsigned int tex_width = 1024;
    unsigned int tex_height = 1024;

    //set up distort  cam
    osg::Texture2D* distortTexture = new osg::Texture2D;
    distortTexture->setTextureSize(tex_width, tex_height);
    distortTexture->setInternalFormat(GL_RGBA);
    distortTexture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
    distortTexture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);


    // //set up render to texture cam
    // {
    //     osg::Camera* camera = new osg::Camera;
    //     // set clear the color and depth buffer
    //     camera->setClearColor(osg::Vec4(0.0f, 1.0f, 0.0f, 0.0f));
    //     camera->setClearMask(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    //     // just inherit the main cameras view
    //     // camera->setReferenceFrame(osg::Transform::RELATIVE_RF);
    //     camera->setProjectionMatrix(osg::Matrixd::identity());
    //     camera->setViewMatrix(osg::Matrixd::identity());

    //     //set viewport
    //     camera->setViewport(0, 0, tex_width, tex_height);
        
    //     // set the camera to render before the main camera.
    //     camera->setRenderOrder(osg::Camera::PRE_RENDER);
    //     // tell the camera to use OpenGL frame buffer object where supported.
    //     camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);

    //     //attach cam to texture
    //     camera->attach(osg::Camera::COLOR_BUFFER, distortTexture);
    //     camera->setReferenceFrame(osg::Transform::RELATIVE_RF);

    //     camera->addChild(_rootL.get());

    // }
  
    // _rootLDistort = createDistortionSubgraph(_rootL.get(),  _viewerL->getCamera()->getClearColor());
    //left view start
    _viewerL->setSceneData(NULL);
    _viewerL->setSceneData(_rootL.get());

    _viewerL->getDatabasePager()->clear();
    _viewerL->getDatabasePager()->registerPagedLODs(_rootL.get());
    _viewerL->getDatabasePager()->setUpThreads(3, 1);
    _viewerL->getDatabasePager()->setTargetMaximumNumberOfPageLOD(2);
    _viewerL->getDatabasePager()->setUnrefImageDataAfterApplyPolicy(true, true);

    // first you need to close the manipulator
    _viewerL->setCameraManipulator(NULL);
    // set the camera view matrix
    _viewerL->getCamera()->setViewMatrixAsLookAt(osg::Vec3(0,  0, 0), osg::Vec3(0, 0, 1), osg::Vec3(0, 1, 0));
    _viewerL->home();

    // _viewerL->getCameraManipulator()->setHomePosition(osg::Vec3(0,  0, 0), osg::Vec3(0, 0, 1), osg::Vec3(0, 1, 0));
    // _viewerL->home();
    //left view end

    //right view start
    _viewerR->setSceneData(NULL);
    _viewerR->setSceneData(_rootR.get());

    _viewerR->getDatabasePager()->clear();
    _viewerR->getDatabasePager()->registerPagedLODs(_rootR.get());
    _viewerR->getDatabasePager()->setUpThreads(3, 1);
    _viewerR->getDatabasePager()->setTargetMaximumNumberOfPageLOD(2);
    _viewerR->getDatabasePager()->setUnrefImageDataAfterApplyPolicy(true, true);

    // first you need to close the manipulator
    _viewerR->setCameraManipulator(NULL);
    // set the camera view matrix
    _viewerR->getCamera()->setViewMatrixAsLookAt(osg::Vec3(0,  0, 0), osg::Vec3(0, 0, 1), osg::Vec3(0, 1, 0));
    _viewerR->home();

    // _viewerR->getCameraManipulator()->setHomePosition(osg::Vec3(0,  0, 0), osg::Vec3(0, 0, 1), osg::Vec3(0, 1, 0));
    // _viewerR->home();
    //right view end

    /*
    _viewer->setSceneData(NULL);
    _viewer->setSceneData(_root.get());

    _viewer->home();

    _viewer->getDatabasePager()->clear();
    _viewer->getDatabasePager()->registerPagedLODs(_root.get());
    _viewer->getDatabasePager()->setUpThreads(3, 1);
    _viewer->getDatabasePager()->setTargetMaximumNumberOfPageLOD(2);
    _viewer->getDatabasePager()->setUnrefImageDataAfterApplyPolicy(true, true);

    //first you need to close the manipulator
    // _viewer->setCameraManipulator(NULL);
    //set the camera view matrix
    _viewer->getCameraManipulator()->setHomePosition(osg::Vec3(0,  0, 0), osg::Vec3(0, 0, 1), osg::Vec3(0, 1, 0));
    _viewer->home();

    // _manipulator = new osgGA::TerrainManipulator();

    // _viewer->setCameraManipulator(_manipulator.get());
    */
    // _viewerL->setCameraManipulator(_manipulator.get());
    // _viewerR->setCameraManipulator(_manipulator.get());

    // pass the model to the MovieEventHandler so it can pick out ImageStream's to manipulate.
    MovieEventHandler* mehL = new MovieEventHandler();
    //			    meh->setMouseTracking( mouseTracking );
    //    meh->set( _viewer->getSceneData() );
    // _viewer->addEventHandler( meh );
    mehL->set(_viewerL->getSceneData());
    _viewerL->addEventHandler(mehL);

    MovieEventHandler* mehR = new MovieEventHandler();
    mehR->set(_viewerR->getSceneData());
    _viewerR->addEventHandler(mehR);

    _vModelsToLoad.clear();
    equ_display = true;
}
///////////////////////////////////////////////
//equirectangular  code for android start from here
//write by Joseph zhang
//2016.03.16
////////////////////////////////////////////////

osg::Geometry* OsgMainApp::myCreateTexturedQuadGeometry(const osg::Vec3& pos,float width,float height, osg::Image* image, bool useTextureRectangle, bool xyPlane, bool option_flip)
{
    bool flip = image->getOrigin()==osg::Image::TOP_LEFT;
    if (option_flip) flip = !flip;
    //Draw a quad by hand
    osg::Geometry* sg = new osg::Geometry;
    osg::ref_ptr<osg::Vec3Array> quadVertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> quadTexCoors = new osg::Vec2Array;
    int length = 20;

    quadVertices->push_back(osg::Vec3(pos));
    quadVertices->push_back(osg::Vec3(pos.x(), pos.y() + length/2, pos.z()));
    quadVertices->push_back(osg::Vec3(pos.x() + length, pos.y() + length/2, pos.z()));
    quadVertices->push_back(osg::Vec3(pos.x() + length, pos.y(), pos.z()));

    quadTexCoors->push_back(osg::Vec2(0, 0));
    quadTexCoors->push_back(osg::Vec2(0, 1));
    quadTexCoors->push_back(osg::Vec2(1, 1));
    quadTexCoors->push_back(osg::Vec2(1, 0));

    sg->setVertexArray(quadVertices);
    sg->setTexCoordArray(0, quadTexCoors);

    osg::ref_ptr<osg::DrawElementsUInt> face_tri = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP);
    face_tri->push_back(0);
    face_tri->push_back(1);
    face_tri->push_back(3);
    face_tri->push_back(2);

    sg->addPrimitiveSet(face_tri);



    // osg::Geometry* pictureQuad = osg::createTexturedQuadGeometry(pos,
    //                                    osg::Vec3(width,0.0f,0.0f),
    //                                    xyPlane ? osg::Vec3(0.0f,height,0.0f) : osg::Vec3(0.0f,0.0f,height),
    //                                    0.0f, flip ? 1.0f : 0.0f , 1.0f, flip ? 0.0f : 1.0f);

    osg::Texture2D* texture = new osg::Texture2D(image);
    texture->setResizeNonPowerOfTwoHint(false);
    texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);


    sg->getOrCreateStateSet()->setTextureAttributeAndModes(0,
                texture,
                osg::StateAttribute::ON);

    return sg;
}


osg::Geometry* OsgMainApp::myCreateTexturedSphereByHandGeometry(const osg::Vec3& pos,float width,float height, osg::Image* image, bool isLefteye, bool xyPlane, bool option_flip)
{
    bool flip = image->getOrigin()==osg::Image::TOP_LEFT;
    if (option_flip) flip = !flip;
    //////////////////////////////////////////////////////////////////          fix  fisheye image
    /*
    FisheyeSpherical projector;
    projector.use_hfov_ = false;
    projector.use_thoby_ = false;
    projector.set_output_width(2048);
    projector.set_camera_params(width, height, 220, false);

    float test_focal = projector.get_focal_length();
    __android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG", "image_width_ : %d, image_height_ : %d, vfov_ : %f, scale_ : %f, focal_ : %f", projector.image_width_, projector.image_height_, projector.vfov_, projector.scale_, projector.focal_);
    
    // init warper
    mWarper warper;
    warper.set_projector(&projector);
    warper.build_maps(width, height, false);
    int map_cols, map_rows;
    warper.get_map_size(map_cols, map_rows);

    std::vector<std::vector<float> > xmap, ymap;
    warper.get_maps(xmap, ymap);

    __android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG", "width : %f,  height : %f",width, height);
    */

////////////////////////////////////////////    draw a sphere by hand
    osg::Geometry* sg = new osg::Geometry;
    osg::ref_ptr<osg::Vec3Array> sphereVertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec3Array> sphereNormals = new osg::Vec3Array; 
    osg::ref_ptr<osg::Vec2Array> sphereTexCoors = new osg::Vec2Array;

    unsigned int  rings;
    unsigned int  sectors;
    double  radius;
    //create a 10*10 sphere
    rings = 40;
    sectors = 40;
    radius = 100;

    float startPoint = 0;
    if(!isLefteye)  startPoint = float(rings - 1) / 2;

    float const R = 1. / static_cast<float>(rings - 1);
    float const S = 1. /static_cast<float>(sectors - 1);

/*    for (unsigned int r = 0; r < rings; ++r){
        for (unsigned int s = 0; s < sectors; ++s){
            float const z = sin( -M_PI_2 + M_PI * r * R);
            float const x = cos(2 * M_PI * s * S) * sin(M_PI * R * r);
            float const y = sin(2 * M_PI * s * S) * sin(M_PI * R * r);
  warp texture uv from fish eye to equ
            int u = std::min(s * S * map_cols, (float)map_cols - 1);
            int v = std::min(r * R * map_rows, (float)map_rows - 1);

            float src_x = (float)xmap[v][u] / (width - 1);
            float src_y = (float)ymap[v][u] / (height - 1);

            // __android_log_write(ANDROID_LOG_ERROR, "OSGANDROID", "Initializing geometry");
            // __android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG", "src_x : %f,  src_y : %f",src_x, src_y);

            if (src_x > 0 && src_y > 0) {
                sphereTexCoors->push_back(osg::Vec2(src_x, src_y));
            } else {
                sphereTexCoors->push_back(osg::Vec2(0, 0));
            }

            sphereVertices->push_back(osg::Vec3(x * radius, y * radius, z * radius));
            sphereNormals->push_back(osg::Vec3(x, y, z));
        }
    }*/

    for (float r = 0; r < rings; ++r){
        for (unsigned int s = 0; s < sectors; ++s){
            float const z = sin( -M_PI_2 + M_PI * r * R);
            float const x = cos(2 * M_PI * s * S) * sin(M_PI * R * r);
            float const y = sin(2 * M_PI * s * S) * sin(M_PI * R * r);

            sphereTexCoors->push_back(osg::Vec2(s * S, (r / 2  + startPoint) * R));
            sphereVertices->push_back(osg::Vec3(x * radius, y * radius, z * radius));
            sphereNormals->push_back(osg::Vec3(x, y, z));
        }
    }

    sg->setVertexArray(sphereVertices);
    sg->setTexCoordArray(0, sphereTexCoors);

    for (unsigned int r = 0; r < rings - 1; ++r){
        for (unsigned int s = 0; s < sectors - 1 ; ++s){
            osg::ref_ptr<osg::DrawElementsUInt> face = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
                //CCW order
                face->push_back((r + 0) * sectors + (s + 0));
                face->push_back((r + 0) * sectors + (s + 1));
                face->push_back((r + 1) * sectors + (s + 1));
                face->push_back((r + 0) * sectors + (s + 0));
                face->push_back((r + 1) * sectors + (s + 1));
                face->push_back((r + 1) * sectors + (s + 0));

                sg->addPrimitiveSet(face);
        }
    }


/////////////////////////////////////////////

    osg::Texture2D* texture = new osg::Texture2D(image);
    texture->setResizeNonPowerOfTwoHint(false);
    texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

    sg->getOrCreateStateSet()->setTextureAttributeAndModes(0,
                            texture,
                            osg::StateAttribute::ON);

     return sg;
}

void OsgMainApp::deleteModels(){
    if(_vModelsToDelete.size()==0) return;

    osg::notify(osg::ALWAYS)<<"There are "<<_vModelsToDelete.size()<<" models to delete"<<std::endl;

    Model modelToDelete;
    for(unsigned int i=0; i<_vModelsToDelete.size(); i++){
        modelToDelete = _vModelsToDelete[i];
        osg::notify(osg::ALWAYS)<<"Deleting: "<<modelToDelete.name<<std::endl;

        for(unsigned int j=_rootR->getNumChildren(); j>0; j--){
            osg::ref_ptr<osg::Node> children = _rootR->getChild(j-1);
            if(children->getName() == modelToDelete.name){
                _rootR->removeChild(children);
            }
        }

    }

    _vModelsToDelete.clear();
    osg::notify(osg::ALWAYS)<<"finished"<<std::endl;
}
//Initialization function
void OsgMainApp::initOsgWindow(int x,int y,int width,int height){

    __android_log_write(ANDROID_LOG_ERROR, "OSGANDROID",
            "Initializing geometry");

    //Pending
    _notifyHandler = new OsgAndroidNotifyHandler();
    _notifyHandler->setTag("Osg Viewer");
    osg::setNotifyHandler(_notifyHandler);

    osg::notify(osg::ALWAYS)<<"Testing"<<std::endl;

    _screenWidth = width;
    _screenHeight = height;

    //judge if the  multiViewer is empty, if not then return
    _viewerMulti = new osgViewer::CompositeViewer();
    if (_viewerMulti->getNumViews() != 0) return;

    //view left start
    _viewerL = new osgViewer::View;
    _viewerL->setName("view left");
    _viewerMulti->addView(_viewerL);

    _viewerL->getCamera()->setName("Cam left");
    _viewerL->getCamera()->setViewport(new osg::Viewport(0, 0, width/2, height));

    _viewerL->addEventHandler(new osgViewer::StatsHandler);
    _viewerL->addEventHandler(new osgGA::StateSetManipulator(_viewerL->getCamera()->getOrCreateStateSet()));
    _viewerL->addEventHandler(new osgViewer::ThreadingHandler);
    _viewerL->addEventHandler(new osgViewer::LODScaleHandler);
    __android_log_write(ANDROID_LOG_ERROR, "OSGANDROID",
            "add event handler successful!");
    //view left end

    //view right start
    _viewerR = new osgViewer::View;
    _viewerR->setName("view right");
    _viewerMulti->addView(_viewerR);

    _viewerR->getCamera()->setName("Cam right");
    _viewerR->getCamera()->setViewport(new osg::Viewport(width/2, 0, width/2, height));

    _viewerR->addEventHandler(new osgViewer::StatsHandler);
    _viewerR->addEventHandler(new osgGA::StateSetManipulator(_viewerR->getCamera()->getOrCreateStateSet()));
    _viewerR->addEventHandler(new osgViewer::ThreadingHandler);
    _viewerR->addEventHandler(new osgViewer::LODScaleHandler);
    //view right end

    _rootL = new osg::Group();
    _rootR = new osg::Group();
    _rootLDistort = new osg::Group();
    //set up two embeded windows
     _gweL = new osgViewer::GraphicsWindowEmbedded(x, y, width/2, height);
     _gweR = new osgViewer::GraphicsWindowEmbedded(x + width / 2, y, width / 2, height);
     __android_log_write(ANDROID_LOG_ERROR, "OSGANDROID",
            "Graphics embeded windows build successful!");


    //bind gwe to camera and then to viewer
    _viewerL->getCamera()->setGraphicsContext(_gweL.get());
    _viewerR->getCamera()->setGraphicsContext(_gweR.get());
    __android_log_write(ANDROID_LOG_ERROR, "OSGANDROID",
            "viewer set GCE windows successful!");

    _viewerMulti->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);

    _viewerMulti->realize();


/* single
    _viewer = new osgViewer::Viewer();
    _viewer->setUpViewerAsEmbeddedInWindow(x, y, width, height);
    _viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);



    _viewer->realize();

    _viewer->addEventHandler(new osgViewer::StatsHandler);
    _viewer->addEventHandler(new osgGA::StateSetManipulator(_viewer->getCamera()->getOrCreateStateSet()));
    _viewer->addEventHandler(new osgViewer::ThreadingHandler);
    _viewer->addEventHandler(new osgViewer::LODScaleHandler);
*/
/*
    _manipulator = new osgGA::KeySwitchMatrixManipulator;

    _manipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
    _manipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
    _manipulator->addMatrixManipulator( '3', "Drive", new osgGA::DriveManipulator() );
    _manipulator->addMatrixManipulator( '4', "Terrain", new osgGA::TerrainManipulator() );
    _manipulator->addMatrixManipulator( '5', "Orbit", new osgGA::OrbitManipulator() );
    _manipulator->addMatrixManipulator( '6', "FirstPerson", new osgGA::FirstPersonManipulator() );
    _manipulator->addMatrixManipulator( '7', "Spherical", new osgGA::SphericalManipulator() );
*/

    _manipulator = new osgGA::SphericalManipulator();

    // _viewerL->setCameraManipulator(_manipulator.get());
    // _viewerR->setCameraManipulator(_manipulator.get());
    // _viewer->setCameraManipulator( _manipulator.get() );

    _viewerMulti->getViewerStats()->collectStats("scene", true);
    // _viewer->getViewerStats()->collectStats("scene", true);

    _initialized = true;

}
//change camera view matrix
void OsgMainApp::changeCamViewQuat(float x, float y, float z, float w){
    //create a quat
    osg::Quat camViewQuat(x, y, z, w);
    //init a matrix with a quat
    // osg::Matrixf camViewMatrix;
    // camViewMatrix.setRotate(camViewQuat);
    _viewerL->getCamera()->getViewMatrix().makeRotate(camViewQuat);
    _viewerR->getCamera()->getViewMatrix().makeRotate(camViewQuat);
    //set this matrix to the camera
    // osg::notify(osg::ALWAYS)<<"The x, y, z, w of the quat are"<<x<<",  "<<y<<",  "<<z<<",  "<<w<<std::endl;
    // osg::Quat currentQuat;
    // osg::Matrixd currentMatrix = _viewerL->getCamera()->getViewMatrix();
    // currentQuat = currentMatrix.getRotate();
    // osg::notify(osg::ALWAYS)<<"The x, y, z, w of the current quat are"<<currentQuat._v[0]<<", "<<currentQuat._v[1]<<", "<<currentQuat._v[2]<<", "<<currentQuat._v[3]<<std::endl;

    // _viewerL->getCameraManipulator()->setByMatrix(camViewMatrix);

}
//Draw
void OsgMainApp::draw(){
    //Every load o remove has to be done before any drawing
    //loadModels();
	//I remove the origin function, and add movieSample to display the sphere
	movieSample();
    //deleteModels();

    // _viewer->frame();
    _viewerMulti->frame();
}
//Events
void OsgMainApp::mouseButtonPressEvent(float x,float y,int button){
    _viewerL->getEventQueue()->mouseButtonPress(x, y, button);
    _viewerR->getEventQueue()->mouseButtonPress(x, y, button);
    // _viewer->getEventQueue()->mouseButtonPress(x, y, button);
}
void OsgMainApp::mouseButtonReleaseEvent(float x,float y,int button){
    _viewerL->getEventQueue()->mouseButtonRelease(x, y, button);
     _viewerR->getEventQueue()->mouseButtonRelease(x, y, button);
    // _viewer->getEventQueue()->mouseButtonRelease(x, y, button);
}
void OsgMainApp::mouseMoveEvent(float x,float y){
    _viewerL->getEventQueue()->mouseMotion(x, y);
    _viewerR->getEventQueue()->mouseMotion(x, y);
    // _viewer->getEventQueue()->mouseMotion(x, y);
}
void OsgMainApp::keyboardDown(int key){
    _viewerL->getEventQueue()->keyPress(key);
    _viewerR->getEventQueue()->keyPress(key);
    // _viewer->getEventQueue()->keyPress(key);
}
void OsgMainApp::keyboardUp(int key){
    _viewerL->getEventQueue()->keyRelease(key);
    _viewerR->getEventQueue()->keyRelease(key);
    // _viewer->getEventQueue()->keyRelease(key);
}
//Loading and unloading
void OsgMainApp::loadObject(std::string filePath){
    Model newModel;
    newModel.filename = filePath;
    newModel.name = filePath;

    int num = 0;
    for(unsigned int i=0;i<_vModels.size();i++){
        if(_vModels[i].name==newModel.name)
            return;
    }

    _vModelsToLoad.push_back(newModel);

}
void OsgMainApp::loadObject(std::string name,std::string filePath){

    Model newModel;
    newModel.filename = filePath;
    newModel.name = name;

    for(unsigned int i=0;i<_vModels.size();i++){
        if(_vModels[i].name==newModel.name){
            osg::notify(osg::ALWAYS)<<"Name already used"<<std::endl;
            return;
        }
    }

    _vModelsToLoad.push_back(newModel);
}
//load the equiretangular movie by the filename
void OsgMainApp::loadMovie(std::string filename, int type){
    mMovie.filename = filename;
    mMovie.type = type;
    equ_display = false;
}
//set the display flag to false
void OsgMainApp::onMovieResume(){
    osg::Quat m_rotate;
    m_rotate = _viewerL->getCameraManipulator()->getMatrix().getRotate();
    osg::notify(osg::ALWAYS)<<"The x, y, z, w of the current quat are"<<m_rotate._v[0]<<", "<<m_rotate._v[1]<<", "<<m_rotate._v[2]<<", "<<m_rotate._v[3]<<std::endl;
        // equ_display = false;
}
void OsgMainApp::unLoadObject(int number){
    if(_vModels.size() <= number){
        osg::notify(osg::FATAL)<<"Index number error"<<std::endl;
        return;
    }

    Model modelToDelete = _vModels[number];
    _vModels.erase(_vModels.begin()+number);
    _vModelsToDelete.push_back(modelToDelete);
}
void OsgMainApp::clearScene(){
    _vModelsToDelete = _vModels;
    _vModels.clear();
}
//Other Functions
int OsgMainApp::getNumberObjects(){
    return _vModels.size();
}
std::string OsgMainApp::getObjectName(int number){
    return _vModels[number].name;
}
void OsgMainApp::setClearColor(osg::Vec4f color){
    osg::notify(osg::ALWAYS)<<"Setting Clear Color"<<std::endl;
    _viewerL->getCamera()->setClearColor(color);
    _viewerR->getCamera()->setClearColor(color);
    // _viewer->getCamera()->setClearColor(color);
}
osg::Vec4f OsgMainApp::getClearColor(){
    osg::notify(osg::ALWAYS)<<"Getting Clear Color"<<std::endl;
    return _viewerL->getCamera()->getClearColor();
    // return _viewer->getCamera()->getClearColor();
}
