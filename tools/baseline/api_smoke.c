/*
 * Purpose: public DECtalk API smoke test for installed Linux headers and
 * libraries.
 * Scope: baseline verification only; this program is compiled by
 * check_api_smoke.sh against the staged dist/include and dist/lib outputs.
 * Behavior preservation: exercises language selection, startup, speaker
 * selection, speak-to-WAV, sync, wave close, and shutdown through public APIs
 * without changing DECtalk sources or installed headers.
 * Limitations: covers US English speaker 0 and file output only; it does not
 * test live audio devices, callbacks, in-memory output, or other languages.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dtk/ttsapi.h>

static char *read_file(const char *path)
{
    FILE *file;
    long length;
    size_t read_length;
    char *buffer;

    file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "api_smoke: cannot open %s: %s\n", path, strerror(errno));
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "api_smoke: cannot seek %s\n", path);
        fclose(file);
        return NULL;
    }

    length = ftell(file);
    if (length < 0) {
        fprintf(stderr, "api_smoke: cannot size %s\n", path);
        fclose(file);
        return NULL;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        fprintf(stderr, "api_smoke: cannot rewind %s\n", path);
        fclose(file);
        return NULL;
    }

    buffer = (char *)malloc((size_t)length + 1U);
    if (buffer == NULL) {
        fprintf(stderr, "api_smoke: cannot allocate %ld bytes\n", length + 1L);
        fclose(file);
        return NULL;
    }

    read_length = fread(buffer, 1U, (size_t)length, file);
    if (read_length != (size_t)length) {
        fprintf(stderr, "api_smoke: short read from %s\n", path);
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[read_length] = '\0';
    fclose(file);
    return buffer;
}

static int check_mmresult(const char *api_name, MMRESULT status)
{
    if (status == MMSYSERR_NOERROR) {
        return 0;
    }

    fprintf(stderr, "api_smoke: %s failed: %u\n", api_name, status);
    return 1;
}

int main(int argc, char **argv)
{
    const char *input_path;
    const char *output_path;
    char *input_text;
    char language[] = "us";
    char flush_text[] = "        ";
    unsigned int language_id;
    LPTTS_HANDLE_T tts_handle = NULL;
    MMRESULT status;
    int failed = 0;

    if (argc != 3) {
        fprintf(stderr, "usage: api_smoke INPUT_TEXT OUTPUT_WAV\n");
        return 2;
    }

    input_path = argv[1];
    output_path = argv[2];
    input_text = read_file(input_path);
    if (input_text == NULL) {
        return 1;
    }

    language_id = TextToSpeechStartLang(language);
    if ((language_id & TTS_LANG_ERROR) != 0U) {
        fprintf(stderr, "api_smoke: TextToSpeechStartLang failed: %u\n", language_id);
        free(input_text);
        return 1;
    }

    if (!TextToSpeechSelectLang(NULL, language_id)) {
        fprintf(stderr, "api_smoke: TextToSpeechSelectLang failed\n");
        free(input_text);
        return 1;
    }

    status = TextToSpeechStartup(
        &tts_handle,
        WAVE_MAPPER,
        DO_NOT_USE_AUDIO_DEVICE,
        NULL,
        0);
    if (check_mmresult("TextToSpeechStartup", status) != 0) {
        free(input_text);
        return 1;
    }

    status = TextToSpeechSetSpeaker(tts_handle, 0);
    if (check_mmresult("TextToSpeechSetSpeaker", status) != 0) {
        failed = 1;
        goto shutdown;
    }

    status = TextToSpeechOpenWaveOutFile(tts_handle, (char *)output_path, WAVE_FORMAT_1M16);
    if (check_mmresult("TextToSpeechOpenWaveOutFile", status) != 0) {
        failed = 1;
        goto shutdown;
    }

    status = TextToSpeechSpeak(tts_handle, input_text, TTS_FORCE);
    if (check_mmresult("TextToSpeechSpeak(input)", status) != 0) {
        failed = 1;
        goto close_wave;
    }

    status = TextToSpeechSync(tts_handle);
    if (check_mmresult("TextToSpeechSync(input)", status) != 0) {
        failed = 1;
        goto close_wave;
    }

    status = TextToSpeechSpeak(tts_handle, flush_text, TTS_FORCE);
    if (check_mmresult("TextToSpeechSpeak(flush)", status) != 0) {
        failed = 1;
        goto close_wave;
    }

    status = TextToSpeechSync(tts_handle);
    if (check_mmresult("TextToSpeechSync(flush)", status) != 0) {
        failed = 1;
    }

close_wave:
    status = TextToSpeechCloseWaveOutFile(tts_handle);
    if (check_mmresult("TextToSpeechCloseWaveOutFile", status) != 0) {
        failed = 1;
    }

shutdown:
    status = TextToSpeechShutdown(tts_handle);
    if (check_mmresult("TextToSpeechShutdown", status) != 0) {
        failed = 1;
    }

    free(input_text);
    return failed ? 1 : 0;
}
