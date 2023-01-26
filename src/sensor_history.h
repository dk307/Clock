#pragma once

#include <CircularBuffer.h>

class sensor_history
{
public:
    void add_value(uint16_t value)
    {
        last_x_values.push(value);
    }

    std::optional<uint16_t> get_average() const
    {
        const auto size = last_x_values.size();
        if (size)
        {
            double sum = 0;
            for (auto i = 0; i < size; i++)
            {
                const auto value = last_x_values[i];
                sum += value;
            }
            return static_cast<uint16_t>(sum / size);
        }
        else
        {
            return std::nullopt;
        }
    }

private:
    CircularBuffer<uint16_t, 128> last_x_values;
};
