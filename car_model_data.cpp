#pragma once
#include "car.h"

#include <map>
#include <string>
#include <vector>
std::map<std::string, car_model> model_data;

void populate_car_model_data()
{
  std::vector<car_model> model_vec;
  model_vec.push_back({ "chiron", "Bugatti", "Chiron", 1479, 2000, 420, 1996, 6700, 7, 3000000 });
  model_vec.push_back({ "laferrari", "Ferrari", "LaFerrari", 950, 6750, 420, 1585, 9250, 7, 1400000 });
  model_vec.push_back({ "918_spyder", "Porsche", "918 Spyder", 874, 1000, 344, 1685, 9150, 7, 845000 });
  model_vec.push_back({ "lfa", "Lexus", "LFA", 552, 6800, 338, 1480, 9000, 6, 700000 });
  model_vec.push_back({ "ford_gt", "Ford", "GT", 660, 5900, 347, 1521, 9000, 7, 500000 });
  model_vec.push_back({ "svj", "Lamborghini", "Aventador SVJ", 760, 6750, 350, 1525, 9000, 7, 500000 });
  model_vec.push_back({ "murcielago", "Lamborghini", "Murcielago", 640, 6000, 340, 1665, 7600, 6, 350000 });
  model_vec.push_back({ "huracan", "Lamborghini", "Huracan Evo", 602, 6500, 325, 1653, 8500, 7, 260000 });
  model_vec.push_back({ "570s", "McLaren", "570S Coupe", 562, 5000, 328, 1456, 9000, 7, 192000 });
  model_vec.push_back({ "gallardo", "Lamborghini", "Gallardo", 543, 6500, 320, 1379, 9000, 6, 180000 });
  model_vec.push_back({ "911t", "Porsche", "911 Turbo", 572, 2250, 320, 1648, 9000, 8, 174000 });
  model_vec.push_back({ "gt_r", "Mercedes", "AMG GT-R", 577, 2100, 318, 1671, 9000, 7, 165000 });
  model_vec.push_back({ "r8", "Audi", "R8 Spyder", 562, 6300, 326, 1744, 8700, 7, 160000 });
  model_vec.push_back({ "model_s", "Tesla", "Model S Plaid", 1020, 4500, 321, 2161, 18000, 1, 130000 });
  model_vec.push_back({ "rs5", "Audi", "RS5 Sportback", 444, 4700, 280, 1840, 7000, 8, 76000 });
  model_vec.push_back({ "m3", "BMW", "M3 G80", 473, 3200, 316, 1705, 7200, 6, 71000 });
  model_vec.push_back({ "rs3", "Audi", "RS3", 394, 2000, 279, 1629, 8000, 7, 56000 });
  model_vec.push_back({ "genesis", "Hyundai", "Genesis 5", 420, 5000, 297, 2081, 7000, 8, 55000 });
  model_vec.push_back({ "golf_r", "Volkswagen", "Golf R", 315, 2100, 315, 1549, 7000, 7, 45000 });
  model_vec.push_back({ "cupra", "Seat", "Cupra Leon", 286, 1950, 247, 1431, 6700, 7, 43000 });
  model_vec.push_back({ "focus_rs", "Ford", "Focus RS", 350, 3200, 266, 1557, 6800, 6, 37000 });
  model_vec.push_back({ "a3", "Audi", "A3", 201, 1600, 209, 1584, 6000, 7, 28000 });
  model_vec.push_back({ "golf_gti", "Volkswagen", "Golf GTI", 241, 1750, 249, 1312, 5800, 7, 35000 });
  model_vec.push_back({ "370z", "Nissan", "370Z", 323, 5200, 249, 1496, 7500, 6, 30000 });
  model_vec.push_back({ "ceed", "Kia", "Ceed", 201, 4000, 230, 1513, 6200, 6, 30000 });
  model_vec.push_back({ "mazda_3", "Mazda", "3 2.5 S", 186, 4000, 212, 1406, 6500, 6, 22000 });
  model_vec.push_back({ "fiat_500", "Fiat", "500 Abarth", 160, 2500, 211, 1110, 6500, 5, 20000 });
  model_vec.push_back({ "model_3", "Tesla", "Model 3", 283, 4500, 209, 1611, 16000, 1, 35000 });
  model_vec.push_back({ "brz", "Subaru", "BRZ", 205, 6400, 230, 1269, 7200, 6, 29000 });
  model_vec.push_back({ "huayra", "Pagani", "Huayra R", 838, 5500, 383, 1224, 9000, 7, 3100000 });
  model_vec.push_back({ "m5", "BMW", "M5 Competition", 617, 1700, 304, 1970, 6400, 8, 105000 });
  model_vec.push_back({ "supra", "Toyota", "GR Supra", 340, 1700, 250, 1652, 6500, 8, 54000 });

  for (auto m : model_vec)
  {
    model_data.insert({ m.shortname, m });
  }
}

