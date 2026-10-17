
#pragma once

#if (OG_TARGET_PLATFORM == OG_PLATFORM_ANDROID || OG_TARGET_PLATFORM == OG_PLATFORM_IOS || OG_TARGET_PLATFORM == OG_PLATFORM_TIZEN) && !defined(OG_PLATFORM_OS_TVOS)

#include "ui/UIWidget.h"

/**
 * @addtogroup ui
 * @{
 */

OG_BEGIN
namespace ui{

    /**
     * @class VideoPlayer
     * @brief Displays a video file.
     * 
     * @note VideoPlayer displays a video file base on system widget.
     * It's mean VideoPlayer displays a video file above all graphical elements of orange.
     * @js NA
     */
    class VideoPlayer : public orange::ui::Widget
    {
    public:
        /**
         * Videoplayer play event type.
         */
        enum class EventType
        {
            PLAYING = 0,
            PAUSED,
            STOPPED,
            COMPLETED,
            ERROR
        };
        
        /**
         * Styles of how the the video player is presented
         * For now only used on iOS to use either MPMovieControlStyleEmbedded (DEFAULT) or 
         * MPMovieControlStyleNone (NONE)
         */
        enum class StyleType
        {
            DEFAULT = 0,
            NONE
        };

        /**
         * A callback which will be called after specific VideoPlayer event happens.
         */
        typedef std::function<void(Ref*,VideoPlayer::EventType)> ogVideoPlayerCallback;

        /**
         *Static create method for instancing a VideoPlayer.
         */
        CREATE_FUNC(VideoPlayer);

        /**
         * Sets a file path as a video source for VideoPlayer.
         */
        virtual void setFileName(const std::string& videoPath);
        
        /**
         * @brief Get the local video file name.
         *
         * @return The video file name.
         */
        virtual const std::string& getFileName() const { return _videoURL;}

        /**
         * Sets a URL as a video source for VideoPlayer.
         */
        virtual void setURL(const std::string& _videoURL);

        
        /**
         * @brief Get the URL of remoting video source.
         *
         * @return A remoting URL address.
         */
        virtual const std::string& getURL() const { return _videoURL;}
        
        /**
         * @brief Set if playback is done in loop mode
         *
         * @param looping the video will or not automatically restart at the end
         */
        virtual void setLooping(bool looping);
        
        /**
         * Set if the player will enable user input for basic pause and resume of video
         *
         * @param enableInput If true, input will be handled for basic functionality (pause/resume)
         */
        virtual void setUserInputEnabled(bool enableInput);
        
        /**
         * Set the style of the player
         *
         * @param style The corresponding style
         */
        virtual void setStyle(StyleType style);

        /**
         * Starts playback.
         */
        virtual void play();

        /**
         * Pauses playback.
         */
        virtual void pause()override;

        /**
         * Resumes playback.
         */
        virtual void resume()override;

        /**
         * Stops playback.
         */
        virtual void stop();

        /**
         * Seeks to specified time position.
         *
         * @param sec   The offset in seconds from the start to seek to.
         */
        virtual void seekTo(float sec);

        /**
         * Checks whether the VideoPlayer is playing.
         *
         * @return True if currently playing, false otherwise.
         */
        virtual bool isPlaying() const;
        
        /**
         * Checks whether the VideoPlayer is set with looping mode.
         *
         * @return true if the videoplayer is set to loop, false otherwise.
         */
        virtual bool isLooping() const;


        /**
         * Checks whether the VideoPlayer is set to listen user input to resume and pause the video
         *
         * @return true if the videoplayer user input is set, false otherwise.
         */            
        virtual bool isUserInputEnabled() const;
        

        /**
         * Causes the video player to keep aspect ratio or no when displaying the video.
         *
         * @param enable    Specify true to keep aspect ratio or false to scale the video until 
         * both dimensions fit the visible bounds of the view exactly.
         */
        virtual void setKeepAspectRatioEnabled(bool enable);

        /**
         * Indicates whether the video player keep aspect ratio when displaying the video.
         */
        virtual bool isKeepAspectRatioEnabled()const { return _keepAspectRatioEnabled;}

        /**
         * Causes the video player to enter or exit full-screen mode.
         * 
         * @param fullscreen    Specify true to enter full-screen mode or false to exit full-screen mode.
         */
        virtual void setFullScreenEnabled(bool fullscreen);

        /**
         * Indicates whether the video player is in full-screen mode.
         *
         * @return True if the video player is in full-screen mode, false otherwise.
         */
        virtual bool isFullScreenEnabled()const;

        /**
         * Register a callback to be invoked when the video state is updated.
         *
         * @param callback  The callback that will be run.
         */
        virtual void addEventListener(const VideoPlayer::ogVideoPlayerCallback& callback);
        
        /**
         * @brief A function which will be called when video is playing.
         *
         * @param event @see VideoPlayer::EventType.
         
         */
        virtual void onPlayEvent(int event);
        virtual void setVisible(bool visible) override;
        virtual void draw(Renderer *renderer, const Mat3& transform, uint32_t flags) override;
        virtual void onEnter() override;
        virtual void onExit() override;

    protected:
        virtual orange::ui::Widget* createCloneInstance() override;
        virtual void copySpecialProperties(Widget* model) override;
        
public:
        VideoPlayer();
        virtual ~VideoPlayer();

    protected:
#if OG_VIDEOPLAYER_DEBUG_DRAW
        DrawNode *_debugDrawNode;
#endif

        enum class Source
        {
            FILENAME = 0,
            URL
        };

        bool _isPlaying = false;
        bool _isLooping = false;
        bool _isUserInputEnabled = true;
        bool _fullScreenDirty = false;
        bool _fullScreenEnabled = false;
        bool _keepAspectRatioEnabled = false;

        StyleType _styleType = StyleType::DEFAULT;

        std::string _videoURL;
        Source _videoSource;

        int _videoPlayerIndex = -1;
        ogVideoPlayerCallback _eventCallback = nullptr;

        void* _videoView = nullptr;
    };
}

OG_END
// end group
/// @}
#endif
