/*
 * Purpose: public DECtalk API smoke test for installed Linux headers and
 * libraries.
 * Scope: baseline verification only; this program is compiled by
 * check_api_smoke.sh against the staged dist/include and dist/lib outputs.
 * Behavior preservation: exercises stable scalar public APIs plus the existing
 * exact speaker-0 WAV path without changing DECtalk sources or installed
 * headers.
 * Limitations: keeps runtime output no-audio except for WAV-file generation;
 * it does not test live audio devices, callbacks, in-memory output, phoneme
 * capture, timing-sensitive metadata, or non-US speech output.
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
        printf("api_smoke: %s=ok\n", api_name);
        return 0;
    }

    fprintf(stderr, "api_smoke: %s failed: %u\n", api_name, status);
    return 1;
}

static int check_expected_mmresult(
    const char *api_name,
    MMRESULT status,
    MMRESULT expected)
{
    if (status == expected) {
        printf("api_smoke: %s=%u\n", api_name, status);
        return 0;
    }

    fprintf(
        stderr,
        "api_smoke: %s returned %u, expected %u\n",
        api_name,
        status,
        expected);
    return 1;
}

static int check_bool_true(const char *api_name, BOOL value)
{
    if (value) {
        printf("api_smoke: %s=true\n", api_name);
        return 0;
    }

    fprintf(stderr, "api_smoke: %s returned false\n", api_name);
    return 1;
}

static int check_dword_equal(const char *name, DWORD actual, DWORD expected)
{
    if (actual == expected) {
        printf("api_smoke: %s=%lu\n", name, (unsigned long)actual);
        return 0;
    }

    fprintf(
        stderr,
        "api_smoke: %s was %lu, expected %lu\n",
        name,
        (unsigned long)actual,
        (unsigned long)expected);
    return 1;
}

static int check_dword_range(
    const char *name,
    DWORD actual,
    DWORD minimum,
    DWORD maximum)
{
    if (actual >= minimum && actual <= maximum) {
        printf("api_smoke: %s=%lu\n", name, (unsigned long)actual);
        return 0;
    }

    fprintf(
        stderr,
        "api_smoke: %s was %lu, expected range %lu..%lu\n",
        name,
        (unsigned long)actual,
        (unsigned long)minimum,
        (unsigned long)maximum);
    return 1;
}

static int check_string_equal(const char *name, const char *actual, const char *expected)
{
    if (actual != NULL && strcmp(actual, expected) == 0) {
        printf("api_smoke: %s=%s\n", name, actual);
        return 0;
    }

    fprintf(
        stderr,
        "api_smoke: %s was %s, expected %s\n",
        name,
        actual == NULL ? "(null)" : actual,
        expected);
    return 1;
}

static int check_string_contains(const char *name, const char *actual, const char *needle)
{
    if (actual != NULL && strstr(actual, needle) != NULL) {
        printf("api_smoke: %s=%s\n", name, actual);
        return 0;
    }

    fprintf(
        stderr,
        "api_smoke: %s was %s, expected to contain %s\n",
        name,
        actual == NULL ? "(null)" : actual,
        needle);
    return 1;
}

static int check_language_entry(
    const LANG_ENUM *langs,
    DWORD index,
    const char *code,
    const char *name)
{
    char field_name[64];

    if (index >= langs->Languages) {
        fprintf(
            stderr,
            "api_smoke: missing language entry %lu\n",
            (unsigned long)index);
        return 1;
    }

    snprintf(field_name, sizeof(field_name), "lang[%lu].code", (unsigned long)index);
    if (check_string_equal(field_name, langs->Entries[index].lang_code, code) != 0) {
        return 1;
    }

    snprintf(field_name, sizeof(field_name), "lang[%lu].name", (unsigned long)index);
    return check_string_equal(field_name, langs->Entries[index].lang_name, name);
}

static int check_language_enumeration(void)
{
    static const char *expected_codes[] = {"us", "uk", "sp", "gr", "la", "fr"};
    static const char *expected_names[] = {
        "US English",
        "UK English",
        "Spanish",
        "German",
        "Latin American",
        "French"};
    LPLANG_ENUM langs = NULL;
    DWORD enum_size;
    DWORD index;
    int failed = 0;

    enum_size = TextToSpeechEnumLangs(&langs);
    if (enum_size != sizeof(LANG_ENUM) || langs == NULL) {
        fprintf(
            stderr,
            "api_smoke: TextToSpeechEnumLangs size=%lu langs=%p\n",
            (unsigned long)enum_size,
            (void *)langs);
        return 1;
    }
    printf("api_smoke: TextToSpeechEnumLangs=%lu\n", (unsigned long)enum_size);

    failed |= check_dword_equal("language_count", langs->Languages, 6);
    if (!langs->MultiLang) {
        fprintf(stderr, "api_smoke: MultiLang=false\n");
        failed = 1;
    } else {
        printf("api_smoke: MultiLang=true\n");
    }

    for (index = 0; index < 6 && failed == 0; ++index) {
        failed |= check_language_entry(langs, index, expected_codes[index], expected_names[index]);
    }

    if (langs != NULL) {
        free(langs->Entries);
        free(langs);
    }
    return failed;
}

static int check_version_and_features(void)
{
    LPSTR version = NULL;
    LPVERSION_INFO version_info = NULL;
    DWORD features;
    DWORD version_value;
    DWORD version_ex_size;
    int failed = 0;

    version_value = TextToSpeechVersion(&version);
    if (version_value == 0U) {
        fprintf(stderr, "api_smoke: TextToSpeechVersion returned 0\n");
        return 1;
    }
    printf("api_smoke: TextToSpeechVersion=%lu\n", (unsigned long)version_value);
    failed |= check_string_contains("version_string", version, "Multi-Language Controller");

    features = TextToSpeechGetFeatures();
    if (features == 0U) {
        fprintf(stderr, "api_smoke: TextToSpeechGetFeatures returned 0\n");
        failed = 1;
    } else {
        printf("api_smoke: TextToSpeechGetFeatures=%lu\n", (unsigned long)features);
    }

    version_ex_size = TextToSpeechVersionEx(&version_info);
    if (version_ex_size != sizeof(VERSION_INFO) || version_info == NULL) {
        fprintf(
            stderr,
            "api_smoke: TextToSpeechVersionEx size=%lu info=%p\n",
            (unsigned long)version_ex_size,
            (void *)version_info);
        failed = 1;
    } else {
        printf("api_smoke: TextToSpeechVersionEx=%lu\n", (unsigned long)version_ex_size);
        failed |= check_dword_equal("version_info.StructSize", version_info->StructSize, sizeof(VERSION_INFO));
        failed |= check_dword_equal("version_info.StructVersion", version_info->StructVersion, VERSION_STRUCT_VER);
        failed |= check_dword_equal("version_info.Features", version_info->Features, features);
        failed |= check_string_equal("version_info.VerString", version_info->VerString, "Multi-Language Controller");
        failed |= check_string_equal("version_info.Language", version_info->Language, "ML: Multi-Language Base");
    }

    free(version_info);
    return failed;
}

static int check_caps(void)
{
    TTS_CAPS_T caps;
    int failed = 0;

    memset(&caps, 0, sizeof(caps));
    if (check_mmresult("TextToSpeechGetCaps", TextToSpeechGetCaps(&caps)) != 0) {
        return 1;
    }

    failed |= check_dword_equal("caps.languages", caps.dwNumberOfLanguages, 1);
    failed |= check_dword_equal("caps.sample_rate", caps.dwSampleRate, 11025);
    failed |= check_dword_equal("caps.min_rate", caps.dwMinimumSpeakingRate, 50);
    failed |= check_dword_equal("caps.max_rate", caps.dwMaximumSpeakingRate, 600);
    failed |= check_dword_equal("caps.speakers", caps.dwNumberOfPredefinedSpeakers, 9);
    failed |= check_dword_equal("caps.character_set", caps.dwCharacterSet, TTS_ASCII);
    if (caps.lpLanguageParamsArray == NULL) {
        fprintf(stderr, "api_smoke: caps language params are null\n");
        failed = 1;
    } else {
        failed |= check_dword_equal(
            "caps.language[0]",
            caps.lpLanguageParamsArray[0].dwLanguage,
            TTS_AMERICAN_ENGLISH);
        failed |= check_dword_equal(
            "caps.language[0].attributes",
            caps.lpLanguageParamsArray[0].dwLanguageAttributes,
            PROPER_NAME_PRONUNCIATION);
    }

    return failed;
}

static int check_handle_scalar_apis(LPTTS_HANDLE_T tts_handle)
{
    DWORD rate;
    DWORD original_rate;
    DWORD status_ids[1];
    DWORD status_values[1];
    SPEAKER_T speaker;
    LANGUAGE_T language;
    int volume;
    int original_volume;
    int failed = 0;

    failed |= check_mmresult("TextToSpeechGetRate", TextToSpeechGetRate(tts_handle, &rate));
    if (failed == 0) {
        original_rate = rate;
        failed |= check_dword_range("initial_rate", rate, 50, 600);
    } else {
        original_rate = 180;
    }

    failed |= check_expected_mmresult(
        "TextToSpeechSetRate(invalid)",
        TextToSpeechSetRate(tts_handle, 49),
        MMSYSERR_INVALPARAM);
    failed |= check_mmresult("TextToSpeechSetRate(200)", TextToSpeechSetRate(tts_handle, 200));
    failed |= check_mmresult("TextToSpeechGetRate(after-set)", TextToSpeechGetRate(tts_handle, &rate));
    failed |= check_dword_equal("rate_after_set", rate, 200);
    failed |= check_mmresult(
        "TextToSpeechSetRate(restore)",
        TextToSpeechSetRate(tts_handle, original_rate));
    failed |= check_mmresult(
        "TextToSpeechGetRate(after-restore)",
        TextToSpeechGetRate(tts_handle, &rate));
    failed |= check_dword_equal("rate_after_restore", rate, original_rate);

    failed |= check_expected_mmresult(
        "TextToSpeechSetSpeaker(invalid)",
        TextToSpeechSetSpeaker(tts_handle, 9),
        MMSYSERR_INVALPARAM);
    failed |= check_mmresult("TextToSpeechSetSpeaker(WENDY)", TextToSpeechSetSpeaker(tts_handle, WENDY));
    failed |= check_mmresult("TextToSpeechGetSpeaker", TextToSpeechGetSpeaker(tts_handle, &speaker));
    failed |= check_dword_equal("speaker_after_set", speaker, WENDY);
    failed |= check_mmresult("TextToSpeechSetSpeaker(PAUL)", TextToSpeechSetSpeaker(tts_handle, PAUL));

    failed |= check_mmresult("TextToSpeechGetLanguage", TextToSpeechGetLanguage(tts_handle, &language));
    failed |= check_dword_equal("language", language, TTS_AMERICAN_ENGLISH);
    failed |= check_expected_mmresult(
        "TextToSpeechSetLanguage(invalid)",
        TextToSpeechSetLanguage(tts_handle, TTS_AMERICAN_ENGLISH + 1U),
        MMSYSERR_INVALPARAM);
    failed |= check_mmresult(
        "TextToSpeechSetLanguage(us)",
        TextToSpeechSetLanguage(tts_handle, TTS_AMERICAN_ENGLISH));

    failed |= check_mmresult(
        "TextToSpeechGetVolume(main-initial)",
        TextToSpeechGetVolume(tts_handle, VOLUME_MAIN, &original_volume));
    printf("api_smoke: main_volume_initial=%d\n", original_volume);
    failed |= check_mmresult(
        "TextToSpeechSetVolume(main)",
        TextToSpeechSetVolume(tts_handle, VOLUME_MAIN, 80));
    failed |= check_mmresult(
        "TextToSpeechGetVolume(main)",
        TextToSpeechGetVolume(tts_handle, VOLUME_MAIN, &volume));
    if (volume != 80) {
        fprintf(stderr, "api_smoke: main volume was %d, expected 80\n", volume);
        failed = 1;
    } else {
        printf("api_smoke: main_volume=%d\n", volume);
    }
    failed |= check_mmresult(
        "TextToSpeechSetVolume(main-restore)",
        TextToSpeechSetVolume(tts_handle, VOLUME_MAIN, original_volume));

    status_ids[0] = INPUT_CHARACTER_COUNT;
    status_values[0] = 0xFFFFFFFFU;
    failed |= check_mmresult(
        "TextToSpeechGetStatus(input-count)",
        TextToSpeechGetStatus(tts_handle, status_ids, status_values, 1));
    failed |= check_dword_equal("status.input_count", status_values[0], 0);

    return failed;
}

static int check_handle_error_paths(LPTTS_HANDLE_T tts_handle)
{
    DWORD status_ids[2];
    DWORD status_values[2];
    int failed = 0;

    failed |= check_expected_mmresult(
        "TextToSpeechGetRate(null-rate)",
        TextToSpeechGetRate(tts_handle, NULL),
        MMSYSERR_INVALPARAM);
    failed |= check_expected_mmresult(
        "TextToSpeechGetSpeaker(null-speaker)",
        TextToSpeechGetSpeaker(tts_handle, NULL),
        MMSYSERR_INVALPARAM);

    status_ids[0] = INPUT_CHARACTER_COUNT;
    status_values[0] = 0U;
    failed |= check_expected_mmresult(
        "TextToSpeechGetStatus(zero-count)",
        TextToSpeechGetStatus(tts_handle, status_ids, status_values, 0),
        MMSYSERR_INVALPARAM);

    status_ids[0] = STATUS_SPEAKING;
    status_values[0] = 0U;
    failed |= check_expected_mmresult(
        "TextToSpeechGetStatus(no-audio-speaking)",
        TextToSpeechGetStatus(tts_handle, status_ids, status_values, 1),
        MMSYSERR_ERROR);
    failed |= check_dword_equal("status.no_audio_speaking", status_values[0], 0xFFFFFFFFU);

    status_ids[0] = WAVE_OUT_DEVICE_ID;
    status_values[0] = 0U;
    failed |= check_expected_mmresult(
        "TextToSpeechGetStatus(no-audio-device)",
        TextToSpeechGetStatus(tts_handle, status_ids, status_values, 1),
        MMSYSERR_ERROR);
    failed |= check_dword_equal("status.no_audio_device", status_values[0], 0xFFFFFFFFU);

    status_ids[0] = INPUT_CHARACTER_COUNT;
    status_ids[1] = STATUS_SPEAKING;
    status_values[0] = 0U;
    status_values[1] = 0U;
    failed |= check_expected_mmresult(
        "TextToSpeechGetStatus(mixed-no-audio)",
        TextToSpeechGetStatus(tts_handle, status_ids, status_values, 2),
        MMSYSERR_ERROR);
    failed |= check_dword_equal("status.mixed_input_count", status_values[0], 0);
    failed |= check_dword_equal("status.mixed_no_audio_speaking", status_values[1], 0xFFFFFFFFU);

    failed |= check_expected_mmresult(
        "TextToSpeechCloseInMemory(not-open)",
        TextToSpeechCloseInMemory(tts_handle),
        MMSYSERR_ERROR);
    failed |= check_expected_mmresult(
        "TextToSpeechOpenInMemory(invalid-format)",
        TextToSpeechOpenInMemory(tts_handle, 0xFFFFFFFFU),
        MMSYSERR_INVALPARAM);

    return failed;
}

static int check_repeated_startup_shutdown(void)
{
    LPTTS_HANDLE_T second_handle = NULL;
    MMRESULT status;
    int failed = 0;

    status = TextToSpeechStartup(
        &second_handle,
        WAVE_MAPPER,
        DO_NOT_USE_AUDIO_DEVICE,
        NULL,
        0);
    if (check_mmresult("TextToSpeechStartup(second)", status) != 0) {
        return 1;
    }

    failed |= check_mmresult("TextToSpeechShutdown(second)", TextToSpeechShutdown(second_handle));
    return failed;
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

    failed |= check_expected_mmresult(
        "TextToSpeechShutdown(NULL)",
        TextToSpeechShutdown(NULL),
        MMSYSERR_ERROR);
    failed |= check_language_enumeration();
    failed |= check_version_and_features();
    failed |= check_repeated_startup_shutdown();

    language_id = TextToSpeechStartLang(language);
    if ((language_id & TTS_LANG_ERROR) != 0U) {
        fprintf(stderr, "api_smoke: TextToSpeechStartLang failed: %u\n", language_id);
        free(input_text);
        return 1;
    }
    printf("api_smoke: TextToSpeechStartLang(us)=%u\n", language_id);

    if (check_bool_true("TextToSpeechSelectLang(default)", TextToSpeechSelectLang(NULL, language_id)) != 0) {
        free(input_text);
        return 1;
    }

    failed |= check_caps();

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

    failed |= check_handle_scalar_apis(tts_handle);
    failed |= check_handle_error_paths(tts_handle);
    if (failed != 0) {
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
