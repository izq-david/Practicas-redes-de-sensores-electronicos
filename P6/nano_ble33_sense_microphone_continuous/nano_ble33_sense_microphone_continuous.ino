#define EIDSP_QUANTIZE_FILTERBANK    0
#define EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW 4

/* Includes ---------------------------------------------------------------- */
#include <PDM.h>
#include <reconocer_voz_inferencing.h>

/** Audio buffers, pointers and selectors */
typedef struct {
    signed short *buffers[2];
    unsigned char buf_select;
    unsigned char buf_ready;
    unsigned int buf_count;
    unsigned int n_samples;
} inference_t;

static inference_t inference;
static bool record_ready = false;
static signed short *sampleBuffer;
static bool debug_nn = false; 
static int print_results = -(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW);

// --- VARIABLES PARA EL LED ---
static unsigned long led_david_timer = 0; // Temporizador para el LED
const long led_duration = 1000;           // Duración: 1 segundo

void setup()
{
    Serial.begin(115200);
    while (!Serial);

    // Configuración del LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.println("Edge Impulse Inferencing Demo");

    run_classifier_init();
    if (microphone_inference_start(EI_CLASSIFIER_SLICE_SIZE) == false) {
        ei_printf("ERR: Could not allocate audio buffer\r\n");
        return;
    }
}

void loop()
{
    bool m = microphone_inference_record();
    if (!m) {
        ei_printf("ERR: Failed to record audio...\n");
        return;
    }

    signal_t signal;
    signal.total_length = EI_CLASSIFIER_SLICE_SIZE;
    signal.get_data = &microphone_audio_signal_get_data;
    ei_impulse_result_t result = {0};

    EI_IMPULSE_ERROR r = run_classifier_continuous(&signal, &result, debug_nn);
    if (r != EI_IMPULSE_OK) {
        ei_printf("ERR: Failed to run classifier (%d)\n", r);
        return;
    }

    // BLOQUE DE PREDICCIONES Y LÓGICA
    if (++print_results >= (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW)) {
        float val_david = 0, val_unknown = 0, val_ruido = 0;

        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            String label = String(result.classification[ix].label);
            float value = result.classification[ix].value;

            // Guardamos valores para el Plotter
            if (label == "David") val_david = value;
            if (label == "Unknown") val_unknown = value;
            if (label == "Ruido") val_ruido = value; // Asegúrate de que en EI se llame 'noise' o 'Ruido'

            // Lógica del LED
            if (label == "David" && value > 0.80) {
                digitalWrite(LED_BUILTIN, HIGH);
                led_david_timer = millis();
            }
        }

        // --- FORMATO PARA SERIAL PLOTTER ---
        // El formato es Etiqueta:Valor, separado por comas o espacios
        Serial.print("David:");   Serial.print(val_david);   Serial.print(",");
        Serial.print("Unknown:"); Serial.print(val_unknown); Serial.print(",");
        Serial.print("Ruido:");   Serial.println(val_ruido); 
        // -----------------------------------

        print_results = 0;
    }

    // LÓGICA DE APAGADO (No bloqueante)
    // Si el LED está encendido y ha pasado 1 segundo, se apaga
    if (led_david_timer != 0 && (millis() - led_david_timer > led_duration)) {
        digitalWrite(LED_BUILTIN, LOW);
        led_david_timer = 0; // Resetear temporizador
    }
}

/**
 * PDM buffer full callback
 */
static void pdm_data_ready_inference_callback(void)
{
    int bytesAvailable = PDM.available();
    int bytesRead = PDM.read((char *)&sampleBuffer[0], bytesAvailable);

    if (record_ready == true) {
        for (int i = 0; i < bytesRead >> 1; i++) {
            inference.buffers[inference.buf_select][inference.buf_count++] = sampleBuffer[i];
            if (inference.buf_count >= inference.n_samples) {
                inference.buf_select ^= 1;
                inference.buf_count = 0;
                inference.buf_ready = 1;
            }
        }
    }
}

static bool microphone_inference_start(uint32_t n_samples)
{
    inference.buffers[0] = (signed short *)malloc(n_samples * sizeof(signed short));
    inference.buffers[1] = (signed short *)malloc(n_samples * sizeof(signed short));
    sampleBuffer = (signed short *)malloc((n_samples >> 1) * sizeof(signed short));

    if (inference.buffers[0] == NULL || inference.buffers[1] == NULL || sampleBuffer == NULL) return false;

    inference.buf_select = 0;
    inference.buf_count = 0;
    inference.n_samples = n_samples;
    inference.buf_ready = 0;

    PDM.onReceive(&pdm_data_ready_inference_callback);
    PDM.setBufferSize((n_samples >> 1) * sizeof(int16_t));

    if (!PDM.begin(1, EI_CLASSIFIER_FREQUENCY)) {
        ei_printf("Failed to start PDM!");
    }

    PDM.setGain(127);
    record_ready = true;
    return true;
}

static bool microphone_inference_record(void)
{
    if (inference.buf_ready == 1) {
        // ei_printf("Error sample buffer overrun\n"); // Opcional: silenciar para limpiar monitor serie
    }
    while (inference.buf_ready == 0) {
        delay(1);
    }
    inference.buf_ready = 0;
    return true;
}

static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
    numpy::int16_to_float(&inference.buffers[inference.buf_select ^ 1][offset], out_ptr, length);
    return 0;
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_MICROPHONE
#error "Invalid model for current sensor."
#endif