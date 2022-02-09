#pragma once
#pragma once
#include <cmath>
#include <string>
#include <atomic>
#include <vector>
#include <algorithm>

#include "Ratings.h"

struct car_model
{
  // model-specific properties
  std::string shortname = "f150";
  std::string manufacturer = "Ford";
  std::string model = "F-150";
  int engine_bhp = 600;
  double max_torque_rpm = 6400.;
  int top_speed = 325;
  double weight = 2500;
  int rpm_max = 9000;
  int max_gear = 7;
  int price = 100000;

  //int redline = 8000;
  int rpm_autoshift_up = static_cast<int>(rpm_max * 0.9);
  int rpm_autoshift_down = 800;
  int rpm_min = 1500;

  mutable std::vector<double> ratios;

  std::string name() const // common name for the car
  {
    return manufacturer + " " + model;
  }

  double gear_ratio(int index) const
  {
    if (ratios.empty()) // if ratios are not specified, they can be calculated based on top speed (using some assumptions)
    {
      ratios = calculate_gear_ratios(top_speed);
    }
    return ratios[index];
  }
  std::vector<double> calculate_gear_ratios(int top_speed) const
  {
    std::vector<double> ratios;
    ratios.push_back(0.0);

    for (int i = 1; i < max_gear; ++i)
    {
      double desired_speed = top_speed * (i + 1) * 0.12;
      double ratio = rpm_max / ((axle_ratio * desired_speed) * magic_constant / tire_diam);
      ratios.push_back(ratio);
    }

    double ratio = rpm_max / ((axle_ratio * top_speed) * magic_constant / tire_diam);
    ratios.push_back(ratio);

    return ratios;
  }

  // Immutable simulation properties and constants
  static constexpr double tire_diam = 33.;
  static constexpr double axle_ratio = 3.73;
  //static constexpr double magic_constant = 336.13;
  static constexpr double magic_constant = 267.;

  static constexpr double diff_ratio = axle_ratio;
  static constexpr double wheel_radius = 0.30;
  static constexpr int base_acceleration = 300;
  static constexpr int base_deceleration = 600;
  static constexpr double rpm_loss_per_second = 5000;
  static constexpr double clutched_rev_per_second = 5000;
};

struct car_condition
{
  double engine_condition = 1.0;
  double transmission_condition = 1.0;
  double tire_condition = 1.0;

  bool engine_failure = false;
  bool transmission_failure = false;
  bool tire_failure = false;

  bool can_run() const
  {
    return !(engine_failure || transmission_failure || tire_failure);
  }
};

struct car
{
  const car_model& model;
  car_condition condition;

  bool in_running_condition() const
  {
    return condition.can_run();
  }

  void engine_breakdown(double severity)
  {
    condition.engine_failure = true;
    condition.engine_condition -= severity;
    if (condition.engine_condition < 0.0)
      condition.engine_condition = 0.0;
    car_running = false;
  }

  void transmission_breakdown(double severity)
  {
    condition.transmission_failure = true;
    condition.transmission_condition -= severity;
    if (condition.transmission_condition < 0.0)
      condition.transmission_condition = 0.0;
    car_running = false;

  }

  car(const car_model& model) : model(model)
  {
    custom_ratios.clear();
    if (model.ratios.empty())
    {
      model.gear_ratio(0);
    }
    custom_ratios.insert(custom_ratios.end(), model.ratios.begin(), model.ratios.end());
  }

  // prepare for a new sim
  void reset()
  {
    accel = 0.;
    speed = 5;
    gear = 1;
    travel_distance = 0.;
    rpm = model.rpm_min;
  }

  // dynamic car properties
  int rpm = 1500;
  int gear = 1;
  double accel = 0.; // in m/s
  double speed = 0.; // in km/h
  int throttle = 0; // [0-100]
  bool clutch_engaged = false;
  double travel_distance = 0.;

  std::vector<double> custom_ratios; // use custom ratios if the car has been tuned
  double speed_tuning = 1.0; // current top speed after tuning

  std::atomic<bool> car_running = false;
  void update_speed(double seconds_per_tick)
  {
    if (rpm <= model.rpm_max) // can't accelerate further if the rpm is too high
      speed += accel * seconds_per_tick * 3.6;

  }
  int price() const
  {
    return model.price;
  }
  std::string name() const
  {
    return model.name();
  }

  void brake(double seconds_per_tick)
  {
    rpm -= static_cast<int>(model.rpm_loss_per_second * seconds_per_tick);
    accel -= 10; // simple brake
    if (speed < 10)
      accel = 0;
  }

  void update_acceleration()
  {
    if (clutch_engaged)
      accel = calculate_acceleration(0); // counts as zero throttle
    else
      accel = calculate_acceleration(throttle);

  }
  void update_rpm(double seconds_per_tick)
  {
    if (throttle == 100)
    {
      if (clutch_engaged) // no power from engine, just "empty" rpm
        rpm += static_cast<int>(model.clutched_rev_per_second * seconds_per_tick);
      else
        rpm = calculate_rpm(speed, gear);
    }
    else // throttle depressed, reduce rpm rapidly
    {
      rpm -= static_cast<int>(model.rpm_loss_per_second*seconds_per_tick);
    }
  }


  double gear_shift_up()
  {
    if (gear == model.max_gear)
      return 0.;
    double gear_accuracy = (double)rpm / model.rpm_max;

    ++gear;
    //rpm = calculate_rpm(speed, gear);
    return gear_accuracy;

  }
  void gear_shift_down()
  {
    if (gear == 1)
      return;

    --gear;
    rpm = calculate_rpm(speed, gear);
  }
  // if top speed is known, we can deduce the ratio of the highest gear
  std::vector<double> calculate_gear_ratios(int top_speed) const
  {
    return model.calculate_gear_ratios(top_speed);
  }


private:
  int calculate_rpm(double speed, const int& gear) const
  {

    double rpm = model.axle_ratio * speed * custom_ratios[gear] * model.magic_constant / model.tire_diam;
    return static_cast<int>(rpm);
  }

  int calculate_top_speed() const
  {
    //double top = 0;
    //for (int i = 100; i < 400; ++i)
    //{
    //    int temprpm = calculate_rpm(i, model.max_gear);
    //    if (temprpm >= model.rpm_max)
    //    {
    //        top = i; break;
    //      }
    //        
    //}

    double top_speed = model.rpm_max / (model.axle_ratio * custom_ratios[model.max_gear] * model.magic_constant / model.tire_diam);
    return static_cast<int>(top_speed);
  }

  double calculate_torque(int rpm) const
  {
    // distance from peak torque
    double from_peak = 1 - std::fabs(rpm - model.max_torque_rpm) / model.max_torque_rpm;
    double lower_dropoff = 0.25;
    double upper_dropoff = 0.65;

    double relative_torque = 0.;

    if (rpm < model.max_torque_rpm)
      relative_torque = (from_peak) * (1 - lower_dropoff) + lower_dropoff;
    else if (rpm > model.max_torque_rpm)
      relative_torque = (from_peak) * (1 - upper_dropoff) + upper_dropoff;
    else
      relative_torque = 1;

    double torque_reduction_factor = 1.0 - pow(1 - condition.engine_condition, 2);
    relative_torque *= torque_reduction_factor;
    return relative_torque * (model.engine_bhp * 7127 / model.max_torque_rpm);
  }
  double calculate_current_force() const
  {
    // power is a combination of assumed linear decrease in torque from zero to some high RPM
    double f = calculate_torque(rpm) * custom_ratios[gear] * model.diff_ratio / model.wheel_radius;
    //std::cout << "f: " << f << "\n";
    return f;
  }

  double calculate_acceleration(int throttle) const // 0-100 throttle
  {
    double engine_force = calculate_current_force() * (throttle / 100.0);
    double drag = 0.02 * speed * speed;
    double a = (engine_force - drag) / model.weight;
    //std::cout << "a: " << a << "\n";
    return a;
  }
};
