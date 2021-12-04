#include <WS281xDriver.hpp>

#include <driver/rmt.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace
{
    constexpr auto DIVIDER = 1;
    constexpr auto DURATION = 12.5; /* minimum time of a single RMT duration
                    in nanoseconds based on clock */

    constexpr auto PULSE_T0H = static_cast<uint32_t>(300 / (DURATION * DIVIDER));
    constexpr auto PULSE_T0L = static_cast<uint32_t>(1300 / (DURATION * DIVIDER));
    constexpr auto PULSE_T1H = static_cast<uint32_t>(800 / (DURATION * DIVIDER));
    constexpr auto PULSE_T1L = static_cast<uint32_t>(1300 / (DURATION * DIVIDER));
    constexpr auto PULSE_TRS = static_cast<uint32_t>(50000 / (DURATION * DIVIDER));

    constexpr int channelCount = 8;
    SemaphoreHandle_t channelSemaphores[channelCount];

    int allocateChannel()
    {
        static bool initialized = false;
        if (!initialized) {
            initialized = true;
    
            rmt_register_tx_end_callback(
                [](rmt_channel_t channel, void *arg) {
                    xSemaphoreGiveFromISR(channelSemaphores[int(channel)], nullptr);
                },
                nullptr
            );
        }

        static int currentChannel = 0;

        int ret = currentChannel++;

        channelSemaphores[ret] = xSemaphoreCreateBinary();
        xSemaphoreGive(channelSemaphores[ret]);

        return ret;
    }

    void lockChannel(int channel)
    {
        xSemaphoreTake(channelSemaphores[channel], portMAX_DELAY);
    }

    void IRAM_ATTR bytesToPulses(
        void const * source, rmt_item32_t * destination, size_t sourceSize,
        size_t wantedCount, size_t * translatedSize, size_t * itemsGenerated)
    {
        if (nullptr == source || nullptr == destination) {
            *translatedSize = 0;
            *itemsGenerated = 0;
            return;
        }

        static rmt_item32_t const bits[2] = {
            {{{PULSE_T0H, 1, PULSE_T0L, 0}}},
            {{{PULSE_T1H, 1, PULSE_T1L, 0}}},
        };

        size_t const wantedBytes = wantedCount / 8;
        size_t const bytesToProcess = std::min(sourceSize, wantedBytes);
        size_t const itemsToGenerate = bytesToProcess * 8;

        *translatedSize = bytesToProcess;
        *itemsGenerated = itemsToGenerate;

        auto pixels = reinterpret_cast<uint8_t const *>(source);
        auto const pixelsEnd = pixels + bytesToProcess;
        while (pixels != pixelsEnd) {
            uint8_t const byte = *pixels;
            for(int i = 7; i >= 0; --i) {
                uint8_t bitValue = (byte >> i) & 1;
                *destination = bits[bitValue];
                ++destination;
            }

            ++pixels;
        }
    }
}

WS281xDriver::WS281xDriver(int pinNumber, size_t pixelCount) :
    mData(pixelCount * 3)
{
    mChannel = allocateChannel();

    rmt_config_t config;
    config.rmt_mode = RMT_MODE_TX;
    config.channel = static_cast<rmt_channel_t>(mChannel);
    config.gpio_num = static_cast<gpio_num_t>(pinNumber);
    config.mem_block_num = 1;
    config.clk_div = DIVIDER;
    config.flags = 0;

    config.tx_config.loop_en = 0;
    config.tx_config.carrier_en = 0;
    config.tx_config.idle_output_en = 1;
    config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;

    rmt_config(&config);
    rmt_driver_install(static_cast<rmt_channel_t>(mChannel), 0, 0);
    rmt_translator_init(static_cast<rmt_channel_t>(mChannel), bytesToPulses);
}

void WS281xDriver::send() const noexcept
{
    lockChannel(mChannel);
    rmt_write_sample(static_cast<rmt_channel_t>(mChannel), (uint8_t const *) mData.data(), mData.size(), false);
}
