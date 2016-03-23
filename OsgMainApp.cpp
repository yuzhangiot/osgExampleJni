//#include "OsgMainApp.hpp"
#include "OsgMovieEventHandler.hpp"

OsgMainApp::OsgMainApp(){

    _lodScale = 1.0f;
    _prevFrame = 0;

    _initialized = false;
    _clean_scene = false;
    equ_display = false;

    equMovieName = "/storage/sdcard0/equ2.mp4";
    fisheyeMovieName = "/storage/sdcard0/equ2.mp4";

}
OsgMainApp::~OsgMainApp(){

}
void OsgMainApp::loadModels(){
    if(_vModelsToLoad.size()==0) return;

    osg::notify(osg::ALWAYS)<<"There are "<<_vModelsToLoad.size()<<" models to load"<<std::endl;

    Model newModel;
    for(unsigned int i=0; i<_vModelsToLoad.size(); i++){
        newModel = _vModelsToLoad[i];
        osg::notify(osg::ALWAYS)<<"Loading: "<<newModel.filename<<std::endl;

        osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(newModel.filename);
        if (loadedModel == 0) {
            osg::notify(osg::ALWAYS)<<"Model not loaded"<<std::endl;
        } else {
            osg::notify(osg::ALWAYS)<<"Model loaded"<<std::endl;
            _vModels.push_back(newModel);

            loadedModel->setName(newModel.name);

            osg::Shader * vshader = new osg::Shader(osg::Shader::VERTEX, gVertexShaderfix );
            osg::Shader * fshader = new osg::Shader(osg::Shader::FRAGMENT, gFragmentShaderfix );

            osg::Program * prog = new osg::Program;
            prog->addShader ( vshader );
            prog->addShader ( fshader );

            loadedModel->getOrCreateStateSet()->setAttribute ( prog );

            _root->addChild(loadedModel);
        }
    }

    osgViewer::Viewer::Windows windows;
    _viewer->getWindows(windows);
    for(osgViewer::Viewer::Windows::iterator itr = windows.begin();itr != windows.end();++itr)
    {
      (*itr)->getState()->setUseModelViewAndProjectionUniforms(true);
      (*itr)->getState()->setUseVertexAttributeAliasing(true);
    }

    _viewer->setSceneData(NULL);
    _viewer->setSceneData(_root.get());
    _manipulator->getNode();
    _viewer->home();

    _viewer->getDatabasePager()->clear();
    _viewer->getDatabasePager()->registerPagedLODs(_root.get());
    _viewer->getDatabasePager()->setUpThreads(3, 1);
    _viewer->getDatabasePager()->setTargetMaximumNumberOfPageLOD(2);
    _viewer->getDatabasePager()->setUnrefImageDataAfterApplyPolicy(true, true);

    _vModelsToLoad.clear();

}
void OsgMainApp::movieSample(){
            	//If the equ sphere has already displayed, then doing nothing
            	if(equ_display == true) return;

	    osg::notify(osg::ALWAYS)<<"Start to display sphere"<<std::endl;

	    //defind a new geode root node, a stateSet
	    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
	    osg::StateSet* stateset = geode->getOrCreateStateSet();
//	    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	    //define the top left point and bottom right point
	    osg::Vec3 pos(0.0f,0.0f,0.0f);
	    osg::Vec3 topleft = pos;
	    osg::Vec3 bottomright = pos;
	    ///////////////////////////////////load the equiretangular image
	    //设置纹理
	   	// std::string filename("/storage/sdcard0/equ2.mp4");
	    osg::Image* m_image = osgDB::readImageFile(equMovieName);
	    osg::ImageStream* m_imagestream = dynamic_cast<osg::ImageStream*>(m_image);
	    //if there are audio contained in the image sequence
	    if(m_imagestream){
			osg::ImageStream::AudioStreams& audioStreams = m_imagestream->getAudioStreams();
			if ( !audioStreams.empty())
			{
				osg::AudioStream* audioStream = audioStreams[0].get();
				osg::notify(osg::NOTICE)<<"AudioStream read ["<<audioStream->getName()<<"]"<<std::endl;
//#if USE_SDL || USE_SDL2
//					if (numAudioStreamsEnabled==0)
//					{
//						audioStream->setAudioSink(new SDLAudioSink(audioStream));
//
//						++numAudioStreamsEnabled;
//					}
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
	    	bool useTextureRectangle = true;
	    	bool xyPlane = false;
	    	bool flip = true;
	    	osg::ref_ptr<osg::Drawable> drawable = myCreateTexturedSphereGeometry(pos, width, height,m_image, useTextureRectangle, xyPlane, flip);
//	    	add SphpereDrawable to geode
	    	geode->addDrawable(drawable.get());
		    //set the bottom right pos
		    bottomright = pos + osg::Vec3(width, height, 0.0f);

		    if (xyPlane) pos.y() += height*1.05f;
		                   else pos.z() += height*1.05f;

			////////////////////////////////////rotate it by 90
				const double angle = 135;
				const osg::Vec3d axis(1, 0, 0);
				transform->setMatrix(osg::Matrix::rotate(angle, axis));
				transform->addChild(geode.get());
	    }
/////////////////////////////////////////////test code start
/*
	        //设置纹理
	        std::string filename("/storage/sdcard0/equ.mp4");
//	        osg::Texture2D* m_texture = new osg::Texture2D;
//	        m_texture->setDataVariance(osg::Object::DYNAMIC);
	        osg::Image* m_image = osgDB::readImageFile(filename);
	        osg::ImageStream*m_imagestream = dynamic_cast<osg::ImageStream*>(m_image);
	        if (m_imagestream)
			{
				osg::ImageStream::AudioStreams& audioStreams = m_imagestream->getAudioStreams();
				if ( !audioStreams.empty())
				{
					osg::AudioStream* audioStream = audioStreams[0].get();
					osg::notify(osg::NOTICE)<<"AudioStream read ["<<audioStream->getName()<<"]"<<std::endl;
//#if USE_SDL || USE_SDL2
//					if (numAudioStreamsEnabled==0)
//					{
//						audioStream->setAudioSink(new SDLAudioSink(audioStream));
//
//						++numAudioStreamsEnabled;
//					}
//#endif
				}
			}
//	        m_texture->setImage(m_image);

//	        osg::StateSet* m_stateset = new osg::StateSet;;
//	        m_stateset->setTextureAttributeAndModes(0, m_texture, osg::StateAttribute::ON);
//	        geode->setStateSet(m_stateset);

		    //创建一个球体
//	        if(m_image){
//	        	osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0,0.0,0.0), osg::WGS_84_RADIUS_POLAR));



				//添加到叶节点
				geode->addDrawable(sd.get());

				////////////////////////////////////rotate it by 90
//				const double angle = 90;
//				const osg::Vec3d axis(1, 0, 0);
//				transform->setMatrix(osg::Matrix::rotate(angle, axis));
//				transform->addChild(geode.get());



//	        }

*/
////////////////////////////////////////////test code end

		osg::Shader * vshader = new osg::Shader(osg::Shader::VERTEX, gVertexShaderfix );
		osg::Shader * fshader = new osg::Shader(osg::Shader::FRAGMENT, gFragmentShaderfix );

		osg::Program * prog = new osg::Program;
		prog->addShader ( vshader );
		prog->addShader ( fshader );


		geode->getOrCreateStateSet()->setAttribute ( prog );
		 //add geode to root
		_root->addChild(transform.get());


	    osgViewer::Viewer::Windows windows;
	    _viewer->getWindows(windows);
	    for(osgViewer::Viewer::Windows::iterator itr = windows.begin();itr != windows.end();++itr)
	    {
	      (*itr)->getState()->setUseModelViewAndProjectionUniforms(true);
	      (*itr)->getState()->setUseVertexAttributeAliasing(true);
	    }

	    _viewer->setSceneData(NULL);
	    _viewer->setSceneData(_root.get());
	    _manipulator->getNode();
	    _viewer->home();

	    _viewer->getDatabasePager()->clear();
	    _viewer->getDatabasePager()->registerPagedLODs(_root.get());
	    _viewer->getDatabasePager()->setUpThreads(3, 1);
	    _viewer->getDatabasePager()->setTargetMaximumNumberOfPageLOD(2);
	    _viewer->getDatabasePager()->setUnrefImageDataAfterApplyPolicy(true, true);

	    //first you need to close the manipulator
//	    _viewer->setCameraManipulator(NULL);
	    //set the camera view matrix
//	    _viewer->getCamera()->setViewMatrixAsLookAt(osg::Vec3(0, -100, 0), osg::Vec3(0, 0, 0), osg::Vec3(0, 1, 0));

	    // pass the model to the MovieEventHandler so it can pick out ImageStream's to manipulate.
	    MovieEventHandler* meh = new MovieEventHandler();
//			    meh->setMouseTracking( mouseTracking );
	    meh->set( _viewer->getSceneData() );
		_viewer->addEventHandler( meh );

	    _vModelsToLoad.clear();
	    equ_display = true;
}
///////////////////////////////////////////////
//equirectangular  code for android start from here
//write by Joseph zhang
//2016.03.16
////////////////////////////////////////////////

osg::ShapeDrawable* OsgMainApp::myCreateTexturedSphereGeometry(const osg::Vec3& pos,float width,float height, osg::Image* image, bool useTextureRectangle, bool xyPlane, bool option_flip)
{
    bool flip = image->getOrigin()==osg::Image::TOP_LEFT;
    if (option_flip) flip = !flip;

    //add a sphere
    osg::TessellationHints* hints = new osg::TessellationHints;
    //set the rotate speed
    hints->setDetailRatio(5.0f);
    //create a new sphere
    osg::ShapeDrawable* sd = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0,0.0,0.0), osg::WGS_84_RADIUS_POLAR), hints);
    //set the texture for the sphere
    osg::Texture2D* texture = new osg::Texture2D(image);
    texture->setResizeNonPowerOfTwoHint(false);
    texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

    sd->getOrCreateStateSet()->setTextureAttributeAndModes(0,
                            texture,
                            osg::StateAttribute::ON);

     return sd;

}
void OsgMainApp::deleteModels(){
    if(_vModelsToDelete.size()==0) return;

    osg::notify(osg::ALWAYS)<<"There are "<<_vModelsToDelete.size()<<" models to delete"<<std::endl;

    Model modelToDelete;
    for(unsigned int i=0; i<_vModelsToDelete.size(); i++){
        modelToDelete = _vModelsToDelete[i];
        osg::notify(osg::ALWAYS)<<"Deleting: "<<modelToDelete.name<<std::endl;

        for(unsigned int j=_root->getNumChildren(); j>0; j--){
            osg::ref_ptr<osg::Node> children = _root->getChild(j-1);
            if(children->getName() == modelToDelete.name){
                _root->removeChild(children);
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

    _viewer = new osgViewer::Viewer();
    _viewer->setUpViewerAsEmbeddedInWindow(x, y, width, height);
    _viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);

    _root = new osg::Group();

    _viewer->realize();

    _viewer->addEventHandler(new osgViewer::StatsHandler);
    _viewer->addEventHandler(new osgGA::StateSetManipulator(_viewer->getCamera()->getOrCreateStateSet()));
    _viewer->addEventHandler(new osgViewer::ThreadingHandler);
    _viewer->addEventHandler(new osgViewer::LODScaleHandler);

    _manipulator = new osgGA::KeySwitchMatrixManipulator;

    _manipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
    _manipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
    _manipulator->addMatrixManipulator( '3', "Drive", new osgGA::DriveManipulator() );
    _manipulator->addMatrixManipulator( '4', "Terrain", new osgGA::TerrainManipulator() );
    _manipulator->addMatrixManipulator( '5', "Orbit", new osgGA::OrbitManipulator() );
    _manipulator->addMatrixManipulator( '6', "FirstPerson", new osgGA::FirstPersonManipulator() );
    _manipulator->addMatrixManipulator( '7', "Spherical", new osgGA::SphericalManipulator() );

    _viewer->setCameraManipulator( _manipulator.get() );

    _viewer->getViewerStats()->collectStats("scene", true);

    _initialized = true;

}
//Draw
void OsgMainApp::draw(){
    //Every load o remove has to be done before any drawing
    //loadModels();
	//I remove the origin function, and add movieSample to display the sphere
	movieSample();
    //deleteModels();

    _viewer->frame();
}
//Events
void OsgMainApp::mouseButtonPressEvent(float x,float y,int button){
    _viewer->getEventQueue()->mouseButtonPress(x, y, button);
}
void OsgMainApp::mouseButtonReleaseEvent(float x,float y,int button){
    _viewer->getEventQueue()->mouseButtonRelease(x, y, button);
}
void OsgMainApp::mouseMoveEvent(float x,float y){
    _viewer->getEventQueue()->mouseMotion(x, y);
}
void OsgMainApp::keyboardDown(int key){
    _viewer->getEventQueue()->keyPress(key);
}
void OsgMainApp::keyboardUp(int key){
    _viewer->getEventQueue()->keyRelease(key);
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
    _viewer->getCamera()->setClearColor(color);
}
osg::Vec4f OsgMainApp::getClearColor(){
    osg::notify(osg::ALWAYS)<<"Getting Clear Color"<<std::endl;
    return _viewer->getCamera()->getClearColor();
}
