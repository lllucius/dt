/*
 * Purpose: public DECtalk API callback smoke test for installed Linux headers
 * and libraries.
 * Scope: baseline verification only; this program is compiled by
 * check_api_callback_smoke.sh against the staged dist/include and dist/lib
 * outputs.
 * Behavior preservation: exercises a stable callback path with WAV-file output
 * and sanitized scalar callback fields without changing DECtalk sources,
 * installed headers, or live-audio behavior.
 * Limitations: does not test live audio devices, scheduler timing, queue
 * timing, pipe behavior, in-memory buffers, or callback pointer payloads.
 */

#include <stdio.h>

#include <dtk/ttsapi.h>

#define MAX_EVENTS 64

struct callback_event {
    LONG param1;
    LONG param2_kind;
    DWORD instance;
    UINT message;
};

static struct callback_event events[MAX_EVENTS];
static unsigned int event_count;
static unsigned int dropped_count;

static void callback_smoke(LONG param1, LONG param2, DWORD instance, UINT message)
{
    LONG param2_kind;

    if (message == TTS_MSG_BUFFER) {
        param2_kind = (param2 == 0) ? 0 : 1;
    } else {
        param2_kind = param2;
    }

    if (event_count < MAX_EVENTS) {
        events[event_count].param1 = param1;
        events[event_count].param2_kind = param2_kind;
        events[event_count].instance = instance;
        events[event_count].message = message;
        ++event_count;
    } else {
        ++dropped_count;
    }
}

static int check_mmresult(const char *api_name, MMRESULT status)
{
    if (status == MMSYSERR_NOERROR) {
        printf("api_callback_smoke: %s=ok\n", api_name);
        return 0;
    }

    fprintf(stderr, "api_callback_smoke: %s failed: %u\n", api_name, status);
    return 1;
}

static int check_bool_true(const char *api_name, BOOL value)
{
    if (value) {
        printf("api_callback_smoke: %s=true\n", api_name);
        return 0;
    }

    fprintf(stderr, "api_callback_smoke: %s returned false\n", api_name);
    return 1;
}

static void print_events(void)
{
    unsigned int i;

    printf("api_callback_smoke: callback.count=%u\n", event_count);
    printf("api_callback_smoke: callback.dropped=%u\n", dropped_count);
    for (i = 0; i < event_count; ++i) {
        printf(
            "api_callback_smoke: callback[%u].message=%u\n",
            i,
            events[i].message);
        printf(
            "api_callback_smoke: callback[%u].param1=%ld\n",
            i,
            events[i].param1);
        printf(
            "api_callback_smoke: callback[%u].param2_kind=%ld\n",
            i,
            events[i].param2_kind);
        printf(
            "api_callback_smoke: callback[%u].instance=%lu\n",
            i,
            (unsigned long)events[i].instance);
    }
}

int main(int argc, char **argv)
{
    static char input_text[] = "Hello. [:index mark 42] World.";
    static char flush_text[] = "        ";
    LPTTS_HANDLE_T tts_handle = NULL;
    unsigned int language_id;
    MMRESULT status;
    int failed = 0;

    if (argc != 2) {
        fprintf(stderr, "usage: api_callback_smoke OUT_WAV\n");
        return 2;
    }

    language_id = TextToSpeechStartLang("us");
    if ((language_id & TTS_LANG_ERROR) != 0U) {
        fprintf(stderr, "api_callback_smoke: TextToSpeechStartLang failed: %u\n", language_id);
        return 1;
    }
    printf("api_callback_smoke: TextToSpeechStartLang(us)=%u\n", language_id);

    if (check_bool_true(
            "TextToSpeechSelectLang(default)",
            TextToSpeechSelectLang(NULL, language_id)) != 0) {
        return 1;
    }

    status = TextToSpeechStartup(
        &tts_handle,
        WAVE_MAPPER,
        DO_NOT_USE_AUDIO_DEVICE,
        callback_smoke,
        1234);
    if (check_mmresult("TextToSpeechStartup", status) != 0) {
        return 1;
    }

    status = TextToSpeechSetSpeaker(tts_handle, PAUL);
    if (check_mmresult("TextToSpeechSetSpeaker(PAUL)", status) != 0) {
        failed = 1;
        goto shutdown;
    }

    status = TextToSpeechOpenWaveOutFile(tts_handle, argv[1], WAVE_FORMAT_1M16);
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
        goto close_wave;
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

    if (failed == 0) {
        print_events();
    }

    return failed;
}
