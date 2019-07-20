#pragma once

#include <vector>
#include <cstddef>

struct WS281xDriver
{
    explicit WS281xDriver(int pinNumber, size_t pixelCount);
    void send() const noexcept;
    std::byte * data() noexcept { return mData.data(); }
    size_t size() const noexcept { return mData.size(); }
private:
    int mChannel;
    std::vector<std::byte> mData;
};
