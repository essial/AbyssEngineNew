#ifndef ABYSS_SYSTEMIO_INTERFACE_H
#define ABYSS_SYSTEMIO_INTERFACE_H

#include <string>

namespace AbyssEngine::SystemIO {

    class ISystemIO {
    public:
        virtual ~ISystemIO() = default;
        virtual std::string_view Name() = 0;

        /// Pauses the audio subsystem.
        /// \param pause Pauses the audio if true, resumes if false.
        virtual void PauseAudio(bool pause) = 0;

        /// Sets the window state to full screen.
        /// \param fullscreen Window is set to fullscreen if true, or windowed if false.
        virtual void SetFullscreen(bool fullscreen) = 0;

        /// Starts the main system loop.
        virtual void RunMainLoop() = 0;

        // Stops the main loop
        virtual void Stop() = 0;
    };

}

#endif //ABYSS_SYSTEMIO_INTERFACE_H
