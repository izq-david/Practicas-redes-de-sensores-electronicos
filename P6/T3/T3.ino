#include <Izquierdo-project-1_inferencing.h>
#include <Arduino_LSM9DS1.h>

#define CONVERT_G_TO_MS2    9.80665f
#define MAX_ACCEPTED_RANGE  2.0f

static bool debug_nn = false;
static uint32_t run_inference_every_ms = 200;
static rtos::Thread inference_thread(osPriorityLow);
static float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };
static float inference_buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

void run_inference_background();

void setColor(int r, int g, int b) {
    analogWrite(LEDR, 255 - r);
    analogWrite(LEDG, 255 - g);
    analogWrite(LEDB, 255 - b);
}

void setup()
{
    Serial.begin(115200);
    while (!Serial);
    Serial.println("Edge Impulse Inferencing - 6 Ejes + LEDs (Índices)");

    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT);
    setColor(0, 0, 0);

    if (!IMU.begin()) {
        ei_printf("Failed to initialize IMU!\r\n");
    }

    if (EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME != 6) {
        ei_printf("ERR: Samples per frame should be 6\n");
        return;
    }

    inference_thread.start(mbed::callback(&run_inference_background));
}

float ei_get_sign(float number) {
    return (number >= 0.0) ? 1.0 : -1.0;
}

void run_inference_background()
{
    delay((EI_CLASSIFIER_INTERVAL_MS * EI_CLASSIFIER_RAW_SAMPLE_COUNT) + 100);

    while (1) {
        memcpy(inference_buffer, buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE * sizeof(float));

        signal_t signal;
        int err = numpy::signal_from_buffer(inference_buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
        if (err != 0) return;

        ei_impulse_result_t result = { 0 };
        err = run_classifier(&signal, &result, debug_nn);
        if (err != EI_IMPULSE_OK) return;

        ei_printf("\nProbabilidades:\n");
        int best_index = -1;
        float max_prob = 0.0;

        for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
            ei_printf("  %s: %.5f\n", result.classification[i].label, result.classification[i].value);
            if (result.classification[i].value > max_prob) {
                max_prob = result.classification[i].value;
                best_index = i;
            }
        }

        if (max_prob > 0.60) {
            ei_printf("Movimiento elegido: %s\n", result.classification[best_index].label);
            
            switch (best_index) {
                case 0: setColor(255, 255, 0); break;   // Pitch_neg: Amarillo
                case 1: setColor(0, 255, 0); break;     // Pitch_pos: Verde
                case 2: setColor(255, 105, 180); break; // Roll_neg:  Rosa
                case 3: setColor(255, 0, 0); break;     // Roll_pos:  Rojo
                case 4: setColor(128, 0, 128); break;   // Yaw_neg:   Morado
                case 5: setColor(0, 0, 255); break;     // Yaw_pos:   Azul
            }
        } else {
            ei_printf("Movimiento elegido: Incertidumbre (apagado)\n");
            setColor(0, 0, 0); // Apagado
        }

        delay(run_inference_every_ms);
    }
}

void loop()
{
    while (1) {
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);

        numpy::roll(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, -6);

        IMU.readAcceleration(
            buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 6],
            buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 5],
            buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 4]
        );
        
        IMU.readGyroscope(
            buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 3],
            buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 2],
            buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 1]
        );

        for (int i = 0; i < 3; i++) {
            if (fabs(buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 6 + i]) > MAX_ACCEPTED_RANGE) {
                buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 6 + i] = ei_get_sign(buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 6 + i]) * MAX_ACCEPTED_RANGE;
            }
            buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - 6 + i] *= CONVERT_G_TO_MS2;
        }

        uint64_t time_to_wait = next_tick - micros();
        delay((int)floor((float)time_to_wait / 1000.0f));
        delayMicroseconds(time_to_wait % 1000);
    }
}