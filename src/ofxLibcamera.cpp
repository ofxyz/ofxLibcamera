#include "ofxLibcamera.h"
#include "ofLog.h"

ofxLibcamera::ofxLibcamera() {
    
};

ofxLibcamera::~ofxLibcamera(){
   exit();
};

std::string ofxLibcamera::getCameraName(libcamera::Camera *camera)
{
	const libcamera::ControlList &props = camera->properties();
	std::string name;

	const auto &location = props. get(libcamera::properties::Location);
	if (location) {
		switch (*location) {
		case libcamera::properties::CameraLocationFront:
			name = "Internal front camera";
			break;
		case libcamera::properties::CameraLocationBack:
			name = "Internal back camera";
			break;
		case libcamera::properties::CameraLocationExternal:
			name = "External camera";
			const auto &model = props.get(libcamera::properties::Model);
			if (model)
				name = " '" + *model + "'";
			break;
		}
	}

	name += " (" + camera->id() + ")";

	return name;
}

void ofxLibcamera::setup() {
    
    /*
     * Find connected cameras
     */
    cm = std::make_unique<libcamera::CameraManager>();
    cm->start(); // cm->version()

    cameras = cm->cameras();

    if (cameras.empty()) {
        ofLog() << "No cameras were identified on the system";
        //cm->stop();
        return;
    } 

    /*
     * Select a connected camera
     */
    std::string cameraId;

    // Get first one (for now)
    cameraId = cameras[0]->id();
    ofLog() << "Selected Camera ID: " << cameraId;

    if(!cameraId.empty()) {
        /*
         * Note that `camera` may not compare equal to `cameras[0]`.
         * In fact, it might simply be a `nullptr`, as the particular
         * device might have disappeared (and reappeared) in the meantime.
         */
        camera = cm->get(cameraId);
        camera->acquire(); // exclusive lock no other application can use it
    }

    /*
     * Configure selected camera
     */
    if(camera != nullptr){
        /*
         * Get config for specific 'role'
         */
        config = camera->generateConfiguration( { libcamera::StreamRole::Viewfinder } );

        libcamera::StreamConfiguration &streamConfig = config->at(0);
        ofLog() << "Default viewfinder configuration is: " << streamConfig.toString();
        config->validate();
        camera->configure(config.get());
    }

    /*
     * Allocate a framebuffer
     */
    allocator = new libcamera::FrameBufferAllocator(camera);

    for (libcamera::StreamConfiguration &cfg : *config) 
    {
        int ret = allocator->allocate(cfg.stream());
        if (ret < 0) {
            ofLog() << "Can't allocate buffers";
            return;
        }

        size_t allocated = allocator->buffers(cfg.stream()).size();
        ofLog() << "Allocated " << allocated << " buffers for stream";

    }

    camera->requestCompleted.connect(this, &ofxLibcamera::requestComplete);

    camera->start();

    streamConfig = config->at(0);
    std::cout << "Default viewfinder configuration is: " << streamConfig.toString() << std::endl;

    stream = streamConfig.stream();
    const std::vector<std::unique_ptr<libcamera::FrameBuffer>> &buffers = allocator->buffers(stream);

};

void ofxLibcamera::update()
{
    for (std::unique_ptr<libcamera::Request> &request : requests) {
        camera->queueRequest(request.get());
    }

    /*
    * Frame Capture
    */
    const std::vector<std::unique_ptr<libcamera::FrameBuffer>> &buffers = allocator->buffers(streamConfig.stream());

    for (unsigned int i = 0; i < buffers.size(); ++i) {
        std::unique_ptr<libcamera::Request> request = camera->createRequest();
        if (!request)
        {
            ofLog() << "Can't create request";
            return;
        }

        
        const std::unique_ptr<libcamera::FrameBuffer> &buffer = buffers[i];
        int ret = request->addBuffer(stream, buffer.get());
        if (ret < 0)
        {
            std::cerr << "Can't set buffer for request"
                << std::endl;
            return;
        }

        requests.push_back(std::move(request));
    }
}

void ofxLibcamera::requestComplete(libcamera::Request *request)
{
    if (request->status() == libcamera::Request::RequestCancelled) return;

    const std::map<const libcamera::Stream *, libcamera::FrameBuffer *> &buffers = request->buffers();

    for (auto bufferPair : buffers) {
        libcamera::FrameBuffer *buffer = bufferPair.second;
        const libcamera::FrameMetadata &metadata = buffer->metadata();

        ofLog() << " seq: " << std::setw(6) << std::setfill('0') << metadata.sequence << " bytesused: ";

        unsigned int nplane = 0;
        for (const libcamera::FrameMetadata::Plane &plane : metadata.planes())
        {
            ofLog() << plane.bytesused;
            if (++nplane < metadata.planes().size()) ofLog() << "/";
        }

        /*
		 * Image data can be accessed here, but the FrameBuffer
		 * must be mapped by the application ... (seperate thread?)
		 */
        
    }

    /* Re-queue request to camera. */
    request->reuse(libcamera::Request::ReuseBuffers);
    camera->queueRequest(request);
}

void ofxLibcamera::exit()
{
    if(camera){
        camera->stop();
        allocator->free(stream);
        delete allocator;
        camera->release();
        camera.reset();
    }
    cm->stop();
}
