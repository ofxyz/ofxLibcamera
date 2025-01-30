#include "ofxLibcamera.h"
#include "ofLog.h"
#include "ofBufferObject.h"
#include <sys/mman.h>

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

void ofxLibcamera::setup()
{
    cm = std::make_unique<libcamera::CameraManager>();
    cm->start(); // cm->version()

    /*
     * Find connected cameras
     */    
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
        config->at(0).pixelFormat = libcamera::formats::BGR888;
        libcamera::Size size(1024, 768);
        config->at(0).size = size;
        config->at(0).bufferCount = 1;
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
            ofLog(OF_LOG_ERROR) << "Can't set buffer for request";
            return;
        }

        for (const libcamera::FrameBuffer::Plane &plane : buffer->planes())
        {
            void *memory = mmap(NULL, plane.length, PROT_READ, MAP_SHARED, plane.fd.get(), 0);
            mappedBuffers_[plane.fd.get()] = std::make_pair(memory, plane.length);
        }

        requests.push_back(std::move(request));
    }

    for (std::unique_ptr<libcamera::Request> &request : requests) {
        camera->queueRequest(request.get());
    }
};

void ofxLibcamera::update()
{
    std::lock_guard<std::mutex> lock(free_requests_mutex_);

    if (!requestQueue.empty()) {

        libcamera::StreamConfiguration const &cfg = stream->configuration();

        int w = cfg.size.width;
        int h = cfg.size.height;
        int stride = cfg.stride;

        libcamera::Request *request = requestQueue.front();
    
        const std::map<const libcamera::Stream *, libcamera::FrameBuffer *> &buffers = request->buffers();

        for (auto bufferPair : buffers)
        {
            libcamera::FrameBuffer *buffer = bufferPair.second; 

            for (const libcamera::FrameBuffer::Plane &plane : buffer->planes())
            {
                // Use ofBuffer?
                void *data = mappedBuffers_[plane.fd.get()].first;
                //glPixelStorei(GL_UNPACK_ROW_LENGTH, 32);
                tex.loadData((uint8_t*)data, (unsigned int)w, (unsigned int)h, GL_RGB); //TOFIX
                //glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            } 
        }

        /* Re-queue request to camera. */
        request->reuse(libcamera::Request::ReuseBuffers);
        camera->queueRequest(request);
        requestQueue.pop();
    } 
}

// TODO: ofxLibcamera should not have a draw we need a camera class
void ofxLibcamera::draw()
{
    if(tex.isAllocated()) tex.draw(0,0);
}

void ofxLibcamera::requestComplete(libcamera::Request *request)
{
    if (request->status() == libcamera::Request::RequestCancelled) return;

    requestQueue.push(request);
}

void ofxLibcamera::exit()
{
    while(!requestQueue.empty()) requestQueue.pop();

    if(camera) {
        camera->stop();
        camera->requestCompleted.disconnect(this, &ofxLibcamera::requestComplete);
        allocator->free(stream);
        delete allocator;
        camera->release();
        camera.reset();
    }

    cameras.clear();
    
    config.release();
    config.reset();

    cm->stop();
}
