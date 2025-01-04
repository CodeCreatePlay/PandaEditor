
template <typename T>
T convert_to_range(const T value, T old_min, T old_max, T new_min, T new_max) {
    T old_range = old_max - old_min;
    T new_value;

    if (old_range == 0) {
        new_value = new_min;
    } else {
        T new_range = new_max - new_min;
        new_value = (((value - old_min) * new_range) / old_range) + new_min;
    }

    return new_value;
}
