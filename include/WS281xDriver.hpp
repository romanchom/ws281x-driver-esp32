#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

class WS281xDriver
{
public:
    explicit WS281xDriver(int pinNumber, size_t pixelCount);
    void send() const noexcept;
    std::vector<uint8_t> & data() noexcept { return mData; }
private:
    int mChannel;
    std::vector<uint8_t> mData;
};
