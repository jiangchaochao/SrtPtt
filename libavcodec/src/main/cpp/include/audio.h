
#ifndef SRTPTT_AUDIO_H
#define SRTPTT_AUDIO_H
#ifdef __cplusplus
extern "C"{
#endif

int create_engine();
int create_recorder(int micId, int sample, int channel, int frameLen);
int start_recording();
int destroy_recorder();
int pause_recorder();
int continue_recorder();
void set_debug_file_path(const char *path, int length);



#ifdef __cplusplus
}
#endif
#endif //SRTPTT_AUDIO_H
