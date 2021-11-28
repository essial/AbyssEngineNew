#include "video.h"
#include "../engine/engine.h"

namespace {
const int EncodeBufferSize = 1024 * 32;
const int DecodeBufferSize = 1024 * 32;
} // namespace

AbyssEngine::Video::Video(LibAbyss::InputStream stream)
    : _stream(std::move(stream)), _avBuffer(), _videoBufferData(), _videoStreamIdx(-1), _audioStreamIdx(-1), _videoCodecContext(), _audioCodecContext(),
      _yPlane(), _uPlane(), _vPlane(), _avFrame(), _videoTexture() {

    _avBuffer.resize(DecodeBufferSize);
    _videoBufferData.resize(EncodeBufferSize);

    _avioContext = avio_alloc_context(_avBuffer.data(), DecodeBufferSize, 0, this, &Video::VideoStreamReadCallback, 0, &Video::VideoStreamSeekCallback);

    _avFormatContext = avformat_alloc_context();
    _avFormatContext->pb = _avioContext;
    _avFormatContext->flags |= AVFMT_FLAG_CUSTOM_IO;

    int avError = 0;

    if ((avError = avformat_open_input(&_avFormatContext, "", nullptr, nullptr)) < 0)
        throw std::runtime_error("Failed to open AV format context: " + std::string(av_err2str(avError)));

    if ((avError = avformat_find_stream_info(_avFormatContext, NULL)) < 0)
        throw std::runtime_error("Failed to find stream info: " + std::string(av_err2str(avError)));

    for (auto i = 0; i < _avFormatContext->nb_streams; i++) {
        if (_avFormatContext->streams[i]->codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
            continue;

        _videoStreamIdx = i;
        break;
    }

    if (_videoStreamIdx == -1)
        throw std::runtime_error("Could not locate video stream.");

    for (auto i = 0; i < _avFormatContext->nb_streams; i++) {
        if (_avFormatContext->streams[i]->codecpar->codec_type != AVMEDIA_TYPE_AUDIO)
            continue;

        _audioStreamIdx = i;
        break;
    }

    _microsPerFrame = (uint64_t)(1000000 / ((float)_avFormatContext->streams[_videoStreamIdx]->r_frame_rate.num) /
                                 (float)_avFormatContext->streams[_videoStreamIdx]->r_frame_rate.den);

    const auto videoCodecPar = _avFormatContext->streams[_videoStreamIdx]->codecpar;
    auto videoDecoder = avcodec_find_decoder(videoCodecPar->codec_id);

    if (videoDecoder == nullptr)
        throw std::runtime_error("Missing video codec.");

    _videoCodecContext = avcodec_alloc_context3(videoDecoder);
    if ((avError = avcodec_parameters_to_context(_videoCodecContext, videoCodecPar)) < 0)
        throw std::runtime_error("Failed to apply parameters to video context: " + std::string(av_err2str(avError)));

    if ((avError = avcodec_open2(_videoCodecContext, videoDecoder, nullptr)) < 0)
        throw std::runtime_error("Failed to open video context: " + std::string(av_err2str(avError)));

    if (_audioStreamIdx >= 0) {
        const auto audioCodecPar = _avFormatContext->streams[_audioStreamIdx]->codecpar;
        auto audioDecoder = avcodec_find_decoder(audioCodecPar->codec_id);

        if (audioDecoder == nullptr)
            throw std::runtime_error("Missing audio codec.");

        _audioCodecContext = avcodec_alloc_context3(audioDecoder);
        if ((avError = avcodec_parameters_to_context(_audioCodecContext, audioCodecPar)) < 0)
            throw std::runtime_error("Failed to apply parameters to audio context: " + std::string(av_err2str(avError)));

        if ((avError = avcodec_open2(_audioCodecContext, audioDecoder, nullptr)) < 0)
            throw std::runtime_error("Failed to open audio context: " + std::string(av_err2str(avError)));

        _resampleContext = swr_alloc();
        av_opt_set_channel_layout(_resampleContext, "in_channel_layout", _audioCodecContext->channel_layout, 0);
        av_opt_set_channel_layout(_resampleContext, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
        av_opt_set_int(_resampleContext, "in_sample_rate", _audioCodecContext->sample_rate, 0);
        av_opt_set_int(_resampleContext, "out_sample_rate", 44100, 0);
        av_opt_set_sample_fmt(_resampleContext, "in_sample_fmt", _audioCodecContext->sample_fmt, 0);
        av_opt_set_sample_fmt(_resampleContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

        if ((avError = swr_init(_resampleContext)) < 0)
            throw std::runtime_error("Failed to initialize sound re-sampler: " + std::string(av_err2str(avError)));
    }

    _videoTexture = Engine::Get()->GetSystemIO().CreateTexture(ITexture::Format::YUV, _videoCodecContext->width, _videoCodecContext->height);

    _swsContext = sws_getContext(_videoCodecContext->width, _videoCodecContext->height, _videoCodecContext->pix_fmt, _videoCodecContext->width,
                                 _videoCodecContext->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, nullptr, nullptr, nullptr);

    _yPlaneSize = _videoCodecContext->width * _videoCodecContext->height;
    _uvPlaneSize = _videoCodecContext->width * _videoCodecContext->height / 4;
    _yPlane.resize(_yPlaneSize);
    _uPlane.resize(_uvPlaneSize);
    _vPlane.resize(_uvPlaneSize);
    _uvPitch = _videoCodecContext->width / 2;

    _avFrame = av_frame_alloc();
    _videoTimestamp = av_gettime();
}

AbyssEngine::Video::~Video() {
    av_free(_avioContext->buffer);
    avio_context_free(&_avioContext);
    avcodec_free_context(&_videoCodecContext);
    avcodec_free_context(&_audioCodecContext);
    sws_freeContext(_swsContext);
    swr_free(&_resampleContext);
    av_frame_free(&_avFrame);
    avformat_free_context(_avFormatContext);
}

void AbyssEngine::Video::UpdateCallback(uint32_t ticks) {
    //
    Node::UpdateCallback(ticks);
}

void AbyssEngine::Video::RenderCallback(int offsetX, int offsetY) {
    //
    Node::RenderCallback(offsetX, offsetY);
}

void AbyssEngine::Video::MouseEventCallback(const AbyssEngine::MouseEvent &event) {
    //
    Node::MouseEventCallback(event);
}
int AbyssEngine::Video::VideoStreamRead(uint8_t *buffer, int size) {
    int read = 0;

    while (read < size && !_stream.eof())
        buffer[read++] = _stream.get();

    return read;
}
int64_t AbyssEngine::Video::VideoStreamSeek(int64_t offset, int whence) {
    std::ios_base::seekdir dir;

    switch(whence) {
    case SEEK_SET:
        dir = std::ios_base::seekdir::beg;
        break;
    case SEEK_CUR:
        dir = std::ios_base::seekdir::cur;
        break;
    case SEEK_END:
        dir = std::ios_base::seekdir::end;
        break;
    case AVSEEK_SIZE:
    {
        const auto curPos = _stream.tellg();
        _stream.seekg(0, std::ios_base::seekdir::end);
        const auto endPos = _stream.tellg();
        _stream.seekg(curPos, std::ios_base::seekdir::cur);
        return endPos;
    } break;
    default:
        return -1;
    }

    _stream.seekg(offset, dir);

    return 0;
}
