#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <vector>
#include <memory>

// Audio buffer structure
struct audio_buffer_t {
    float* data;
    int length;
    int channels;
};

// Sound definitions
struct sound_def_t {
    // Instrument parameters
    int osc1_oct, osc1_det, osc1_detune, osc1_xenv, osc1_vol, osc1_waveform;
    int osc2_oct, osc2_det, osc2_detune, osc2_xenv, osc2_vol, osc2_waveform;
    int noise_fader, attack, sustain, release, master;
    int fx_filter, fx_freq, fx_resonance, fx_delay_time, fx_delay_amt, fx_pan_freq, fx_pan_amt;
    int lfo_osc1_freq, lfo_fx_freq, lfo_freq, lfo_amt, lfo_waveform;
    
    // Pattern data
    std::vector<int> pattern;
};

// Initialize audio system
bool audio_init();
void audio_cleanup();

// Play a sound
void audio_play(void* sound, float volume = 1.0f, float pitch = 0.0f, float pan = 0.0f);

// Generate sound effects
void audio_generate_sounds();

// Play music
void audio_play_music();
void audio_stop_music();

// Sound effect handles (defined in audio.cpp)
extern void* sfx_shotgun_shoot;
extern void* sfx_shotgun_reload;
extern void* sfx_nailgun_shoot;
extern void* sfx_grenade_shoot;
extern void* sfx_grenade_explode;
extern void* sfx_plasma_shoot;
extern void* sfx_no_ammo;
extern void* sfx_no_ammo_pickup;
extern void* sfx_hurt;
extern void* sfx_pickup;
extern void* sfx_enemy_hit;
extern void* sfx_enemy_gib;
extern void* sfx_zombie_hit;
extern void* sfx_hound_attack;

#endif // AUDIO_H