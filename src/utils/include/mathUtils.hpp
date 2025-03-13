#ifndef MATH_UTILS_H
#define MATH_UTILS_H

template <typename T>
T convert_to_range(T value, T old_min, T old_max, T new_min, T new_max) {
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

template <typename T>
T clamp(T value, T min_value, T max_value) {
    return (value < min_value) ? min_value : (value > max_value ? max_value : value);
}

template <typename T, typename J>
T clamp(T value, J min_value, J max_value) {
    return (value < min_value) ? min_value : (value > max_value ? max_value : value);
}

template <typename T>
T clamp_angle(T angle, T min_val, T max_val) {
	angle = std::fmod(angle, 360.0f);
	return clamp(angle, min_val, max_val);
}

template <typename T, typename J>
T clamp_angle(T angle, J min_val, J max_val) {
    angle = std::fmod(angle, static_cast<T>(360)); // Ensure proper type casting
    return clamp(angle, static_cast<T>(min_val), static_cast<T>(max_val));
}

// Linear interpolation function
template <typename T>
T lerp(T a, T b, float t) {
    return a + t * (b - a);
}

// Spherical interpolation function
template <typename A, typename B>
LQuaternionf slerp(const A& a, const A& b, float t) {
    // Compute the dot product (cosine of the angle between the quaternions)
    float dot = a.dot(b);

    // If the dot product is negative, use the opposite quaternion for shorter path
    if (dot < 0.0f) {
        dot = -dot;
    }

    const float THRESHOLD = 0.9995f; // If quaternions are very close, use linear interpolation
    if (dot > THRESHOLD) {
        return a * (1.0f - t) + b * t;
    }

    // Compute the angle between the quaternions
    float theta = std::acos(dot);
    float sinTheta = std::sin(theta);

    // Compute interpolation coefficients
    float w1 = std::sin((1.0f - t) * theta) / sinTheta;
    float w2 = std::sin(t * theta) / sinTheta;

    return a * w1 + b * w2;
}

#endif // MATH_UTILS_H