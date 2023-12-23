
#ifndef TEMPERATURE_DATA_H
#define TEMPERATURE_DATA_H

#define MAX_TEMP_SAMPLES 54

class TemperatureData {
  float data[MAX_TEMP_SAMPLES];

  int sample_count = 0;

public:
  void init() {
    // Serial.println("Creating Temperature Data object");
    sample_count = 0;
    for( int i = 0; i < MAX_TEMP_SAMPLES; i++ ) {
      data[i] = 0.0;
    }
  }

  void shift_samples() {
    for( int i = MAX_TEMP_SAMPLES - 1; i > 0; i-- ) {
      data[i] = data[i-1];
    }
  }

  void add_sample(float new_sample) {
    shift_samples();
    data[0] = new_sample;

    if(sample_count < MAX_TEMP_SAMPLES) {
      sample_count++;
    }
  }

  float get_max_temp() {
    float max_temp = -100.0;
    for(int i = 0; i < sample_count; i++) {
      if(data[i] > max_temp) {
        max_temp = data[i];
      }
    }
    return max_temp;
  }

  float get_min_temp() {
    float min_temp = 100.0;
    for(int i = 0; i < sample_count; i++) {
      if(data[i] < min_temp) {
        min_temp = data[i];
      }
    }
    return min_temp;
  }

  float get_sample_at(int index) {
    return data[index];
  }

  int get_sample_count() {
    return sample_count;
  }


  void dump() {
    //erial.println(MAX_TEMP_SAMPLES);
    Serial.print("Temps (");
    Serial.print(sample_count);
    Serial.print("/");
    Serial.print(MAX_TEMP_SAMPLES);
    Serial.print(") : ");
    for(int i = 0; i < sample_count; i++) {
      Serial.print(data[i], 2);
      Serial.print(" ");
    }
    Serial.println();
  }
};



#endif