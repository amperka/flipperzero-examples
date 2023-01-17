
#include "adc_example_app.h"

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>

#include <stm32wbxx_ll_adc.h>
#include <stm32wbxx_ll_pwr.h>

// ADC

#define TAG "AdcTest"

typedef enum {
    FuriHalVref2048,
    FuriHalVref2500,
} FuriHalVref;

void furi_hal_adc_set_vref(FuriHalVref vref) {
    furi_assert(vref == FuriHalVref2048 || vref == FuriHalVref2500);
    uint32_t trim_value = 0;

    switch(vref) {
    case FuriHalVref2048:
        LL_VREFBUF_SetVoltageScaling(LL_VREFBUF_VOLTAGE_SCALE0);
        trim_value = LL_VREFBUF_SC0_GetCalibration() & 0x3FU;
        break;
    case FuriHalVref2500:
        LL_VREFBUF_SetVoltageScaling(LL_VREFBUF_VOLTAGE_SCALE1);
        trim_value = LL_VREFBUF_SC1_GetCalibration() & 0x3FU;
        break;
    }

    LL_VREFBUF_SetTrimming(trim_value);
}

void furi_hal_adc_init() {
    LL_VREFBUF_Enable();
    LL_VREFBUF_DisableHIZ();

    // FHCLK ≥ FADC / 4 if the resolution of all channels are 12-bit or 10-bit
    // STM32WB55xx ADC maximum frequency is 64MHz (corresponding to 4.27Msmp/s maximum)
    // TODO: calculate correct clock
    LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV4);

    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);
    LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);
    LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_MODE_NONE);

    LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);
    LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);
    LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);
    LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_NONE);
    LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

    LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);

    LL_ADC_DisableDeepPowerDown(ADC1);

    LL_ADC_EnableInternalRegulator(ADC1);
    furi_delay_us(LL_ADC_DELAY_INTERNAL_REGUL_STAB_US);

    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
    while(LL_ADC_IsCalibrationOnGoing(ADC1) != 0) {
        // TODO: add timeout
    }

    // TODO: calculate delay between ADC end of calibration and ADC enable
    furi_delay_us(100);
}

void furi_hal_adc_enable() {
    LL_ADC_Enable(ADC1);
    while(LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0) {
        // TODO: add timeout
    }

    // TODO: find out if the SMPS can be bypassed
    // /* Disable SMPS: SMPS in mode step-down can impact ADC conversion accuracy. */
    // /* It is recommnended to disable SMPS (stop SMPS switching by setting it    */
    // /* in mode bypass) during ADC conversion.                                   */
    // /* Get SMPS effective operating mode */
    // if(LL_PWR_SMPS_GetEffectiveMode() == LL_PWR_SMPS_STEP_DOWN) {
    //     /* Set SMPS operating mode */
    //     LL_PWR_SMPS_SetMode(LL_PWR_SMPS_BYPASS);
    // }
}

/**
 * @brief disable ADC
 * Prerequisites: ADC conversions must be stopped.
 * 
 */
void furi_hal_adc_disable() {
    LL_ADC_Disable(ADC1);
    while(LL_ADC_IsEnabled(ADC1)) {
        // TODO: add timeout
    }
}

void furi_hal_adc_deinit() {
    LL_ADC_DisableInternalRegulator(ADC1);
    LL_ADC_EnableDeepPowerDown(ADC1);
    LL_VREFBUF_EnableHIZ();
    LL_VREFBUF_Disable();
}

typedef enum {
    FuriHalAdcChannel0 = LL_ADC_CHANNEL_0,
    FuriHalAdcChannel1 = LL_ADC_CHANNEL_1,
    FuriHalAdcChannel2 = LL_ADC_CHANNEL_2,
    FuriHalAdcChannel3 = LL_ADC_CHANNEL_3,
    FuriHalAdcChannel4 = LL_ADC_CHANNEL_4,
    FuriHalAdcChannel5 = LL_ADC_CHANNEL_5,
    FuriHalAdcChannel6 = LL_ADC_CHANNEL_6,
    FuriHalAdcChannel7 = LL_ADC_CHANNEL_7,
    FuriHalAdcChannel8 = LL_ADC_CHANNEL_8,
    FuriHalAdcChannel9 = LL_ADC_CHANNEL_9,
    FuriHalAdcChannel10 = LL_ADC_CHANNEL_10,
    FuriHalAdcChannel11 = LL_ADC_CHANNEL_11,
    FuriHalAdcChannel12 = LL_ADC_CHANNEL_12,
    FuriHalAdcChannel13 = LL_ADC_CHANNEL_13,
    FuriHalAdcChannel14 = LL_ADC_CHANNEL_14,
    FuriHalAdcChannel15 = LL_ADC_CHANNEL_15,
    FuriHalAdcChannel16 = LL_ADC_CHANNEL_16,
    FuriHalAdcChannel17 = LL_ADC_CHANNEL_17,
    FuriHalAdcChannel18 = LL_ADC_CHANNEL_18,
    FuriHalAdcChannelVREFINT = LL_ADC_CHANNEL_VREFINT,
    FuriHalAdcChannelTEMPSENSOR = LL_ADC_CHANNEL_TEMPSENSOR,
    FuriHalAdcChannelVBAT = LL_ADC_CHANNEL_VBAT,
} FuriHalAdcChannel;

/**
 * @brief Set single channel for ADC
 * ADC has a 18 channels, and channels 0-5 are "fast" channels.
 * Fast channels have ADC conversion rate up to 5.33 Ms/s (0.188 us for 12-bit resolution).
 * Slow channels have ADC conversion rate up to 4.21 Ms/s (0.238 us for 12-bit resolution).
 * @param channel 
 */
void furi_hal_adc_set_single_channel(FuriHalAdcChannel channel) {
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, channel);
    // TODO: calculate sampling time
    LL_ADC_SetChannelSamplingTime(ADC1, channel, LL_ADC_SAMPLINGTIME_2CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC1, channel, LL_ADC_SINGLE_ENDED);
}

/**
 * @brief Read ADC value by software trigger
 * 
 * @return uint32_t ADC value
 */
uint32_t furi_hal_adc_read_sw() {
    LL_ADC_REG_StartConversion(ADC1);
    while(LL_ADC_IsActiveFlag_EOC(ADC1) == 0) {
    }
    return LL_ADC_REG_ReadConversionData12(ADC1);
}

static void adc_example_app_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    ADCExample* app = ctx;

    canvas_clear(canvas);

    FuriString* adc_string = furi_string_alloc();

    float adc_voltage = 2.5f * (float)app->adc_value / 4096.0f;

    furi_string_printf(adc_string, "ADC: %ld\n\n%f V", app->adc_value, (double)adc_voltage);

    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(
        canvas, 40, 20, AlignLeft, AlignTop, furi_string_get_cstr(adc_string));
}

static void adc_example_app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

ADCExample* adc_example_app_alloc() {
    ADCExample* app = malloc(sizeof(ADCExample));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    view_port_draw_callback_set(app->view_port, adc_example_app_draw_callback, app);
    view_port_input_callback_set(app->view_port, adc_example_app_input_callback, app->event_queue);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    // ADC

    app->adc_pin = &gpio_ext_pc3; // PC3 is ADC1_IN4
    furi_hal_gpio_init(app->adc_pin, GpioModeAnalog, GpioPullUp, GpioSpeedLow);
    FURI_LOG_I(TAG, "Gpio Set OK");

    furi_hal_adc_init();
    FURI_LOG_I(TAG, "ADC Init OK");

    furi_hal_adc_set_vref(FuriHalVref2500);
    FURI_LOG_I(TAG, "Vref Set OK");

    furi_hal_adc_set_single_channel(FuriHalAdcChannel4);
    FURI_LOG_I(TAG, "ADC Set Channel OK");

    furi_hal_adc_enable();
    FURI_LOG_I(TAG, "ADC Enable OK");

    return app;
}

void adc_example_app_free(ADCExample* app) {
    furi_assert(app);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_message_queue_free(app->event_queue);

    furi_record_close(RECORD_GUI);

    // ADC

    furi_hal_adc_disable();
    FURI_LOG_I(TAG, "ADC Disable OK");

    furi_hal_adc_deinit();
    FURI_LOG_I(TAG, "ADC Deinit OK");
}

int32_t adc_example_app(void* p) {
    UNUSED(p);
    ADCExample* app = adc_example_app_alloc();

    InputEvent event;

    while(1) {
        app->adc_value = furi_hal_adc_read_sw();
        furi_delay_ms(10);

        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.key == InputKeyBack) {
                if(event.type == InputTypePress) break;
            }
        }
    }

    adc_example_app_free(app);
    return 0;
}
