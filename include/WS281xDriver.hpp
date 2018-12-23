#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

class WS281xDriver
{
public:
    explicit WS281xDriver(int pinNumber, size_t pixelCount);
    void send() const noexcept;
    uint8_t * data() noexcept { return mData.data(); }
    size_t size() const noexcept { return mData.size(); }
private:
    int mChannel;
    std::vector<uint8_t> mData;
};
