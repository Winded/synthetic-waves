#include <lua_audio.h>
#include <lua_asset.h>
#include <AL/alc.h>
#include <AL/al.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>

static struct {
    char initialized;
    ALCdevice *device;
    ALCcontext *context;
} lua_audio_config = {0};

int lua_audio_initialize(lua_State *L)
{
    if(lua_audio_config.initialized) {
        luaL_error(L, "Audio library already initialized");
        return 0;
    }

    ALCdevice *device = alcOpenDevice(0);
    if(!device) {
        luaL_error(L, "Failed to create openAL device");
        return 0;
    }

    ALCcontext *ctx = alcCreateContext(device, 0);
    if(!ctx) {
        luaL_error(L, "Failed to create openAL context");
        return 0;
    }
    alcMakeContextCurrent(ctx);

    lua_audio_config.device = device;
    lua_audio_config.context = ctx;
    lua_audio_config.initialized = 1;
    return 0;
}

void *load_wav(const void *buffer, int buffer_size, int *out_size, int *out_num_channels, int *out_bits_per_sample, int *out_sample_rate)
{
    const char *buf = (const char*)buffer;
    int adv = 0;

    char signature[4];
    memcpy(signature, buf + adv, 4);
    if(strncmp(signature, "RIFF", 4) != 0) {
        fprintf(stderr, "WAV error: Specified asset is not a wave file");
        return 0;
    }
    adv += sizeof(signature);

    int riff_chunk_size;
    memcpy(&riff_chunk_size, buf + adv, sizeof(riff_chunk_size));
    adv += sizeof(riff_chunk_size);

    char format[4];
    memcpy(format, buf + adv, 4);
    if(strncmp(format, "WAVE", 4) != 0) {
        fprintf(stderr, "WAV error: Specified asset is not a wave file");
        return 0;
    }
    adv += sizeof(format);

    char format_signature[4];
    memcpy(format_signature, buf + adv, 4);
    if(strncmp(format_signature, "fmt ", 4) != 0) {
        fprintf(stderr, "WAV error: Specified asset is not a supported wave file");
        return 0;
    }
    adv += sizeof(format_signature);

    int format_chunk_start = adv;
    int format_chunk_size, sample_rate, byte_rate;
    short audio_format, num_channels, block_align, bits_per_sample;

    memcpy(&format_chunk_size, buf + adv, sizeof(format_chunk_size));
    adv += sizeof(format_chunk_size);
    memcpy(&audio_format, buf + adv, sizeof(audio_format));
    adv += sizeof(audio_format);
    memcpy(&num_channels, buf + adv, sizeof(num_channels));
    adv += sizeof(num_channels);
    memcpy(&sample_rate, buf + adv, sizeof(sample_rate));
    adv += sizeof(sample_rate);
    memcpy(&byte_rate, buf + adv, sizeof(byte_rate));
    adv += sizeof(byte_rate);
    memcpy(&block_align, buf + adv, sizeof(block_align));
    adv += sizeof(block_align);
    memcpy(&bits_per_sample, buf + adv, sizeof(bits_per_sample));
    adv += sizeof(bits_per_sample);

    //adv += (format_chunk_start + format_chunk_size) - adv;

    char data_signature[4];
    memcpy(data_signature, buf + adv, 4);
    if(strncmp(data_signature, "data", 4) != 0) {
        fprintf(stderr, "WAV error: Specified asset is not a supported wave file");
        return 0;
    }
    adv += sizeof(data_signature);

    int data_chunk_size;
    memcpy(&data_chunk_size, buf + adv, sizeof(data_chunk_size));
    adv += sizeof(data_chunk_size);

    *out_size = data_chunk_size;
    *out_num_channels = num_channels;
    *out_bits_per_sample = bits_per_sample;
    *out_sample_rate = sample_rate;

    return (void*)(buf + adv);
}

int lua_audio_create_buffer(lua_State *L)
{
    const lua_asset *asset = lua_asset_check(L, 1);

    ALuint id;
    alGenBuffers(1, &id);

    int size, num_channels, bits_per_sample, sample_rate;
    void *data = load_wav(asset->buffer, asset->buffer_size, &size, &num_channels, &bits_per_sample, &sample_rate);
    if(!data) {
        luaL_error(L, "Failed to load audio asset (Is it a WAV file?)");
        return 0;
    }

    ALenum format =
                    num_channels == 1 && bits_per_sample == 8 ? AL_FORMAT_MONO8 :
                    num_channels == 1 && bits_per_sample == 16 ? AL_FORMAT_MONO16 :
                    num_channels == 2 && bits_per_sample == 8 ? AL_FORMAT_STEREO8 :
                    num_channels == 2 && bits_per_sample == 16 ? AL_FORMAT_STEREO16 :
                    0;

    alBufferData(id, format, data, size, sample_rate);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to create audio buffer: %s", alGetString(err));
    }

    lua_pushinteger(L, id);
    return 1;
}

int lua_audio_update_buffer(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);
    const lua_asset *asset = lua_asset_check(L, 2);

    int size, num_channels, bits_per_sample, sample_rate;
    void *data = load_wav(asset->buffer, asset->buffer_size, &size, &num_channels, &bits_per_sample, &sample_rate);
    if(!data) {
        luaL_error(L, "Failed to load audio asset (Is it a WAV file?)");
        return 0;
    }

    ALenum format =
                    num_channels == 1 && bits_per_sample == 8 ? AL_FORMAT_MONO8 :
                    num_channels == 1 && bits_per_sample == 16 ? AL_FORMAT_MONO16 :
                    num_channels == 2 && bits_per_sample == 8 ? AL_FORMAT_STEREO8 :
                    num_channels == 2 && bits_per_sample == 16 ? AL_FORMAT_STEREO16 :
                    0;

    alBufferData(id, format, data, size, sample_rate);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to update audio buffer: %s", alGetString(err));
    }

    return 0;
}

int lua_audio_delete_buffer(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);

    alDeleteBuffers(1, &id);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to delete audio buffer: %s", alGetString(err));
    }

    return 0;
}

int lua_audio_create_source(lua_State *L)
{
    ALuint id;
    alGenSources(1, &id);

    alSourcef(id, AL_GAIN, 1.f);
    alSourcef(id, AL_PITCH, 1.f);
    alSourcei(id, AL_LOOPING, 0);

    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to create audio source: %s", alGetString(err));
    }

    lua_pushinteger(L, id);
    return 1;
}

int lua_audio_delete_source(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);

    alDeleteSources(1, &id);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to delete audio source: %s", alGetString(err));
    }

    return 0;
}

int lua_audio_source_buffer(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);

    int buf_id;
    alGetSourcei(id, AL_BUFFER, &buf_id);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to get source buffer: %s", alGetString(err));
    }

    lua_pushinteger(L, buf_id);
    return 1;
}

int lua_audio_set_source_buffer(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);
    int buf_id = luaL_checkinteger(L, 2);

    alSourcei(id, AL_BUFFER, buf_id);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to set source buffer: %s", alGetString(err));
    }

    return 0;
}

int lua_audio_source_volume(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);

    float volume;
    alGetSourcef(id, AL_GAIN, &volume);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to get source volume: %s", alGetString(err));
    }

    lua_pushnumber(L, volume);
    return 1;
}

int lua_audio_set_source_volume(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);
    float volume = luaL_checknumber(L, 2);

    alSourcef(id, AL_GAIN, volume);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to set source volume: %s", alGetString(err));
    }

    return 0;
}

int lua_audio_source_pitch(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);

    float pitch;
    alGetSourcef(id, AL_PITCH, &pitch);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to get source pitch: %s", alGetString(err));
    }

    lua_pushnumber(L, pitch);
    return 1;
}

int lua_audio_set_source_pitch(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);
    float pitch = luaL_checknumber(L, 2);

    alSourcef(id, AL_PITCH, pitch);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to set source pitch: %s", alGetString(err));
    }

    return 0;
}

int lua_audio_source_loop(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);

    int loop;
    alGetSourcei(id, AL_LOOPING, &loop);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to get source loop: %s", alGetString(err));
    }

    lua_pushboolean(L, loop);
    return 1;
}

int lua_audio_set_source_loop(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);
    int loop = lua_toboolean(L, 2);

    alSourcei(id, AL_LOOPING, loop);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to set source loop: %s", alGetString(err));
    }

    return 0;
}

int lua_audio_source_playing(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);

    ALenum state;
    alGetSourcei(id, AL_SOURCE_STATE, &state);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to get source state: %s", alGetString(err));
    }

    lua_pushboolean(L, state == AL_PLAYING);
    return 1;
}

int lua_audio_play_source(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);

    alSourcePlay(id);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to play source: %s", alGetString(err));
    }

    return 0;
}

int lua_audio_pause_source(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);

    alSourcePause(id);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to play source: %s", alGetString(err));
    }

    return 0;
}

int lua_audio_stop_source(lua_State *L)
{
    int id = luaL_checkinteger(L, 1);

    alSourceStop(id);
    ALenum err = alGetError();
    if(err != AL_FALSE) {
        luaL_error(L, "Failed to play source: %s", alGetString(err));
    }

    return 0;
}

int lua_audio_quit(lua_State *L)
{
    if(!lua_audio_config.initialized) {
        return 0;
    }

    alcDestroyContext(lua_audio_config.context);
    alcCloseDevice(lua_audio_config.device);
    memset(&lua_audio_config, 0, sizeof(lua_audio_config));
    return 0;
}

static const luaL_reg lua_audio_lib[] = {
    {"initialize", lua_audio_initialize},

    {"createBuffer", lua_audio_create_buffer},
    {"updateBuffer", lua_audio_update_buffer},
    {"deleteBuffer", lua_audio_delete_buffer},

    {"createSource", lua_audio_create_source},
    {"deleteSource", lua_audio_delete_source},

    {"sourceBuffer", lua_audio_source_buffer},
    {"setSourceBuffer", lua_audio_set_source_buffer},
    {"sourceVolume", lua_audio_source_volume},
    {"setSourceVolume", lua_audio_set_source_volume},
    {"sourcePitch", lua_audio_source_pitch},
    {"setSourcePitch", lua_audio_set_source_pitch},
    {"sourceLoop", lua_audio_source_loop},
    {"setSourceLoop", lua_audio_set_source_loop},

    {"sourcePlaying", lua_audio_source_playing},

    {"playSource", lua_audio_play_source},
    {"pauseSource", lua_audio_pause_source},
    {"stopSource", lua_audio_stop_source},

    {"quit", lua_audio_quit},
    {0, 0}
};

void lua_audio_load(lua_State *L)
{
    lua_newtable(L);
    luaL_openlib(L, 0, lua_audio_lib, 0);
    lua_setfield(L, -2, "audio");
}
