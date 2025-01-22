#include "ofxLibcamera.h"
#include "ofLog.h"

ofxLibcamera::ofxLibcamera() {
    
};

ofxLibcamera::~ofxLibcamera(){
   
};

std::string ofxLibcamera::cameraName(libcamera::Camera *camera)
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

    std::vector< std::shared_ptr< libcamera::Camera > > cameras = cm->cameras();

    if (cameras.empty()) {
        ofLog() << "No cameras were identified on the system";
        cm->stop();
        return;
    } 

    /*
     * Select a connected camera
     */
    std::string cameraId;

    // Get first one (for now)
    cameraId = cameras[0]->id();
    ofLog() << "Selected Camera ID: " << cameraId;

    std::shared_ptr<libcamera::Camera> camera = nullptr;

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
    std::unique_ptr<libcamera::CameraConfiguration> config;
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
    libcamera::FrameBufferAllocator *allocator = new libcamera::FrameBufferAllocator(camera);

    for (libcamera::StreamConfiguration &cfg : *config) 
    {
        int ret = allocator->allocate(cfg.stream());
        if (ret < 0) {
            ofLog() << "Can't allocate buffers";
            return;
        }

        size_t allocated = allocator->buffers(cfg.stream()).size();
        ofLog() << "Allocated " << allocated << " buffers for stream";

        /*
        * Frame Capture
        */
        libcamera::Stream *stream = cfg.stream();
        const std::vector<std::unique_ptr<libcamera::FrameBuffer>> &buffers = allocator->buffers(stream);
        std::vector<std::unique_ptr<libcamera::Request>> requests;

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

};
