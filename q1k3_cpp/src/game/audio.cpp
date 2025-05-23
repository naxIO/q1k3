#include "audio.h"
#include <iostream>
#include <cmath>
#include <cstring>
#include <algorithm>

// Constants
static const int AUDIO_SAMPLERATE = 44100;
static const int AUDIO_TAB_SIZE = 4096;
static const int AUDIO_TAB_MASK = AUDIO_TAB_SIZE - 1;

// Lookup tables for oscillators
static float AUDIO_TAB[AUDIO_TAB_SIZE * 4];

// SDL Audio device
static SDL_AudioDeviceID audio_device = 0;
static SDL_AudioSpec audio_spec;

// Sound buffer storage
static std::vector<std::unique_ptr<audio_buffer_t>> sound_buffers;

// Sound effect pointers (indices into sound_buffers)
void* sfx_shotgun_shoot = nullptr;
void* sfx_shotgun_reload = nullptr;
void* sfx_nailgun_shoot = nullptr;
void* sfx_grenade_shoot = nullptr;
void* sfx_grenade_explode = nullptr;
void* sfx_plasma_shoot = nullptr;
void* sfx_no_ammo = nullptr;
void* sfx_no_ammo_pickup = nullptr;
void* sfx_hurt = nullptr;
void* sfx_pickup = nullptr;
void* sfx_enemy_hit = nullptr;
void* sfx_enemy_gib = nullptr;
void* sfx_zombie_hit = nullptr;
void* sfx_hound_attack = nullptr;

// Active sound instance
struct sound_instance_t {
    audio_buffer_t* buffer;
    int position;
    float volume;
    float pan;
    bool active;
};

// Maximum concurrent sounds
static const int MAX_SOUNDS = 32;
static sound_instance_t active_sounds[MAX_SOUNDS];
static SDL_mutex* audio_mutex = nullptr;

// Audio callback for SDL
static void audio_callback(void* userdata, Uint8* stream, int len) {
    // Clear the stream
    std::memset(stream, 0, len);
    
    // Convert to float buffer
    float* out = reinterpret_cast<float*>(stream);
    int samples = len / sizeof(float);
    
    SDL_LockMutex(audio_mutex);
    
    // Mix all active sounds
    for (int i = 0; i < MAX_SOUNDS; i++) {
        sound_instance_t& sound = active_sounds[i];
        if (!sound.active || !sound.buffer) continue;
        
        // Calculate stereo pan
        float left_vol = sound.volume * (1.0f - std::max(0.0f, sound.pan));
        float right_vol = sound.volume * (1.0f - std::max(0.0f, -sound.pan));
        
        // Mix this sound
        int samples_to_mix = std::min(samples / 2, sound.buffer->length - sound.position);
        
        for (int j = 0; j < samples_to_mix; j++) {
            int src_idx = sound.position * 2;
            int dst_idx = j * 2;
            
            // Mix left and right channels
            out[dst_idx] += sound.buffer->data[src_idx] * left_vol;
            out[dst_idx + 1] += sound.buffer->data[src_idx + 1] * right_vol;
            
            sound.position++;
        }
        
        // Deactivate sound if finished
        if (sound.position >= sound.buffer->length) {
            sound.active = false;
        }
    }
    
    SDL_UnlockMutex(audio_mutex);
    
    // Clamp output to prevent clipping
    for (int i = 0; i < samples; i++) {
        out[i] = std::max(-1.0f, std::min(1.0f, out[i]));
    }
}

bool audio_init() {
    // Initialize lookup tables
    for (int i = 0; i < AUDIO_TAB_SIZE; i++) {
        AUDIO_TAB[i] = std::sin(i * 6.283184f / AUDIO_TAB_SIZE); // sin
        AUDIO_TAB[i + AUDIO_TAB_SIZE] = AUDIO_TAB[i] < 0 ? -1.0f : 1.0f; // square
        AUDIO_TAB[i + AUDIO_TAB_SIZE * 2] = i / float(AUDIO_TAB_SIZE) - 0.5f; // saw
        AUDIO_TAB[i + AUDIO_TAB_SIZE * 3] = i < AUDIO_TAB_SIZE/2 
            ? (i / float(AUDIO_TAB_SIZE/4)) - 1.0f 
            : 3.0f - (i / float(AUDIO_TAB_SIZE/4)); // tri
    }
    
    // Create audio mutex
    audio_mutex = SDL_CreateMutex();
    if (!audio_mutex) {
        std::cerr << "Failed to create audio mutex" << std::endl;
        return false;
    }
    
    // Initialize active sounds
    for (int i = 0; i < MAX_SOUNDS; i++) {
        active_sounds[i].active = false;
    }
    
    // Setup SDL audio
    SDL_AudioSpec desired;
    std::memset(&desired, 0, sizeof(desired));
    desired.freq = AUDIO_SAMPLERATE;
    desired.format = AUDIO_F32SYS;
    desired.channels = 2;
    desired.samples = 2048;
    desired.callback = audio_callback;
    
    audio_device = SDL_OpenAudioDevice(nullptr, 0, &desired, &audio_spec, 0);
    if (audio_device == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Start audio playback
    SDL_PauseAudioDevice(audio_device, 0);
    
    // Generate all sound effects
    audio_generate_sounds();
    
    return true;
}

void audio_cleanup() {
    if (audio_device != 0) {
        SDL_CloseAudioDevice(audio_device);
        audio_device = 0;
    }
    
    if (audio_mutex) {
        SDL_DestroyMutex(audio_mutex);
        audio_mutex = nullptr;
    }
    
    // Clean up sound buffers
    for (auto& buffer : sound_buffers) {
        if (buffer && buffer->data) {
            delete[] buffer->data;
        }
    }
    sound_buffers.clear();
}

void audio_play(void* sound, float volume, float pitch, float pan) {
    if (!sound) return;
    
    // Convert sound pointer to buffer index
    int buffer_idx = reinterpret_cast<intptr_t>(sound);
    if (buffer_idx < 0 || buffer_idx >= sound_buffers.size()) return;
    
    audio_buffer_t* buffer = sound_buffers[buffer_idx].get();
    if (!buffer) return;
    
    SDL_LockMutex(audio_mutex);
    
    // Find a free sound slot
    for (int i = 0; i < MAX_SOUNDS; i++) {
        if (!active_sounds[i].active) {
            active_sounds[i].buffer = buffer;
            active_sounds[i].position = 0;
            active_sounds[i].volume = volume;
            active_sounds[i].pan = pan;
            active_sounds[i].active = true;
            break;
        }
    }
    
    SDL_UnlockMutex(audio_mutex);
}

// Generate a single sound effect
static audio_buffer_t* audio_generate_sound(const sound_def_t& def) {
    // Simplified sound generation - would need full Sonant-X implementation
    auto buffer = std::make_unique<audio_buffer_t>();
    
    // Calculate buffer size based on pattern length
    int samples = AUDIO_SAMPLERATE; // 1 second for now
    buffer->length = samples;
    buffer->channels = 2;
    buffer->data = new float[samples * 2];
    
    // Generate simple sine wave for now
    for (int i = 0; i < samples; i++) {
        float value = std::sin(i * 440.0f * 2.0f * M_PI / AUDIO_SAMPLERATE) * 0.3f;
        buffer->data[i * 2] = value;     // Left
        buffer->data[i * 2 + 1] = value; // Right
    }
    
    return buffer.release();
}

void audio_generate_sounds() {
    // Sound definitions would go here
    // For now, create dummy sounds
    
    sound_def_t dummy_sound = {};
    
    // Generate all sound effects
    sound_buffers.emplace_back(audio_generate_sound(dummy_sound));
    sfx_shotgun_shoot = reinterpret_cast<void*>(0);
    
    sound_buffers.emplace_back(audio_generate_sound(dummy_sound));
    sfx_shotgun_reload = reinterpret_cast<void*>(1);
    
    // ... and so on for all sound effects
    
    std::cout << "Generated " << sound_buffers.size() << " sound effects" << std::endl;
}

void audio_play_music() {
    // TODO: Implement music playback
}

void audio_stop_music() {
    // TODO: Stop music
}