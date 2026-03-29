/* switchfl1p 2025-2026 */
//see https://github.com/switchfl1p/tween

#pragma once

#include <vector>

/* 
To use TimedLinearInterpolator with your type T, you must provide:
    YourValueType getValue(const T &data);
    float getTime(const T &data);

To use ConstVelLinearInterpolator with your type T, you must provide:
    float distance(const YourValueType &a, const YourValueType &b); 

see https://github.com/switchfl1p/tween for examples
*/

template<typename ValueType>
class WeightedLinearInterpolator {
public:
    size_t numSegments() const {
        return values.empty() ? 0 : values.size() - 1;
    }

    ValueType interpolate(float alpha) const {
        if (values.empty())
            return ValueType();
        if (values.size() == 1)
            return values[0].data;

        //Find which segment we are within
        size_t segment = 1;
        for (; segment < values.size(); segment++) {
            if (alpha < values[segment].weight)
                break;
        }
        
        //if alpha >= than 1
        if (segment == values.size())
            return values.back().data;

        float section_alpha = alpha - values[segment - 1].weight; //how far into the specific segment
        section_alpha /= values[segment].weight - values[segment - 1].weight; //normalize it to [0,1]

        float inv_sec_alpha = 1.0f - section_alpha;

        return values[segment - 1].data * inv_sec_alpha + values[segment].data * section_alpha;
    }

protected:
    WeightedLinearInterpolator() {}

    struct Data {
        ValueType data;
        float weight;
    };

    std::vector<Data> values;
};

/* 
Loads a set of (value, time) pairs into the interpolator
Times must be in [0, 1] and in ascending order
If looping, the first value is appended at the end to allow smooth wraparound 
*/
template<typename ValueType>
class TimedLinearInterpolator : public WeightedLinearInterpolator<ValueType> {
public:

    template<typename BidirectionalRange>
    void setValues(const BidirectionalRange &data, bool is_looping = true) {
        this->values.clear();
        typename BidirectionalRange::const_iterator curr = data.begin();
        typename BidirectionalRange::const_iterator final = data.end();

        for (; curr != final; curr++) {
            typename WeightedLinearInterpolator<ValueType>::Data curr_data;
            curr_data.data = getValue(*curr);
            curr_data.weight = getTime(*curr);

            assert(0.0f <= curr_data.weight && curr_data.weight <= 1.0f);
            this->values.push_back(curr_data);
        }

        if (is_looping && !this->values.empty())
            this->values.push_back(this->values[0]);

        
        //Ensure first is weight 0, and last is weight 1
        if (!this->values.empty()) {
            this->values.front().weight = 0.0f;
            this->values.back().weight = 1.0f;
        }
    }
protected:
};

/*
Loads a set of values and automatically distributes them evenly across [0, 1]
Unlike TimedLinearInterpolator, no timestamps are required in the input
If looping, the last value is duplicated at the end to pad the cycle boundary 
*/
template<typename ValueType>
class LinearInterpolator : public WeightedLinearInterpolator<ValueType> {
public:
    //typedef ValueType value_type;

    template<typename BidirectionalRange>
    void setValues(const BidirectionalRange &data, bool is_looping = true) {
        this->values.clear();
        int i_num_values = 0;
        typename BidirectionalRange::const_iterator curr = data.begin();
        typename BidirectionalRange::const_iterator final = data.end();

        for (; curr != final; curr++) {
            typename WeightedLinearInterpolator<ValueType>::Data curr_data;
            curr_data.data = *curr;
            curr_data.weight = 0.0f;
            this->values.push_back(curr_data);

            i_num_values++;
        }
        
        //provides a pause between loops
        if (is_looping && !this->values.empty()) {
            this->values.push_back(this->values.back());
            i_num_values++;
        }

        //Compute weights
        for (size_t val_ix = 0; val_ix < this->values.size(); val_ix++) {
            this->values[val_ix].weight = val_ix / (float)(i_num_values - 1);
        }
    }
private:
};

/*
Interpolates with a constant velocity between positions.

This interpolator maps a range of [0, 1) onto a set of values. However, it takes the distance
between these values. There must be a free function called "distance" which takes two ValueType's and
returns a float distance between them.

The idea is that, if you add 0.1 to your alpha value, you will always get a movement of the same distance.
Not necessarily between the initial and final points, but the object will have moved at the same
speed along the path.
*/
template<typename ValueType>
class ConstVelLinearInterpolator : public WeightedLinearInterpolator<ValueType> {
public:
    //typedef ValueType value_type;

    ConstVelLinearInterpolator()
    : total_dist(0.0f) {}
    
    template<typename BidirectionalRange>
    void setValues(const BidirectionalRange &data, bool is_looping = true){
        this->values.clear();

        typename BidirectionalRange::const_iterator curr = data.begin();
        typename BidirectionalRange::const_iterator last = data.end();

        for (; curr!= last; curr++) {
            typename WeightedLinearInterpolator<ValueType>::Data curr_data;
            curr_data.data = *curr;
            curr_data.weight = 0.0f;
            this->values.push_back(curr_data);
        }

        if (is_looping) {
            typename WeightedLinearInterpolator<ValueType>::Data curr_data;
            curr_data.data = *data.begin();
            curr_data.weight = 0.0f;
            this->values.push_back(curr_data);
        }

        //Compute the distances of each segment
        total_dist = 0.0f;
        for (size_t i_loop = 1; i_loop < this->values.size(); i_loop++) {
            total_dist += distance(this->values[i_loop - 1].data, this->values[i_loop].data);
            this->values[i_loop].weight = total_dist;
        }

        //Compute the alpha value that represents when to use this segment
        for (size_t i_loop = 1; i_loop < this->values.size(); i_loop++) {
            this->values[i_loop].weight /= total_dist;
        }
    }

    float getDistance() const {return total_dist;}

private:
    float total_dist;
};