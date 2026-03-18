#pragma once
namespace ot
{
    //Quantity logic based on ISO 800000
    // Expression of a Quantity as the product of base quantities
    struct Dimension 
    {
        // Base quantities
        int mass = 0, length = 0, time = 0,
            current = 0, temp = 0, amount = 0, intensity = 0;

        bool operator==(const Dimension& _other) const {
            return mass == _other.mass && length == _other.length && time == _other.time &&
                current == _other.current && temp == _other.temp &&
                amount == _other.amount && intensity == _other.intensity;
        }

        //Dimension scaled(int exp) const {
        //    return { mass * exp, length * exp, time * exp,
        //             current * exp, temp * exp, amount * exp, intensity * exp };
        //}

        //Dimension operator+(const Dimension& o) const {
        //    return { mass + o.mass, length + o.length, time + o.time,
        //             current + o.current, temp + o.temp,
        //             amount + o.amount, intensity + o.intensity };
        //}
    };

}




