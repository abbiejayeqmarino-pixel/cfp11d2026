/**
 * P9 — Motor Selection & Efficiency Calculator
 */

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Motor {
    std::string model;
    std::string frame;
    double rated_power_kw;
    double eff_100;
    double eff_75;
    double eff_50;
    double cost_usd;
};

const std::vector<Motor> MOTOR_CATALOG = {
    {"Standard-M075",  "80M",   0.75,  0.825, 0.815, 0.790, 150.00},
    {"Standard-M150",  "90L",   1.50,  0.840, 0.835, 0.810, 220.00},
    {"Standard-M220",  "100L",  2.20,  0.855, 0.850, 0.830, 310.00},
    {"Standard-M370",  "112M",  3.70,  0.875, 0.870, 0.855, 450.00},
    {"Standard-M550",  "132S",  5.50,  0.885, 0.880, 0.865, 600.00},
    {"Standard-M750",  "132M",  7.50,  0.895, 0.895, 0.880, 850.00},
    {"Standard-M1100", "160M", 11.00,  0.910, 0.910, 0.895, 1200.00},
    {"Standard-M1500", "160L", 15.00,  0.920, 0.920, 0.905, 1600.00}
};

const std::vector<Motor> PREMIUM_CATALOG = {
    {"Premium-P075",  "80M",   0.75,  0.855, 0.845, 0.825, 210.00},
    {"Premium-P150",  "90L",   1.50,  0.865, 0.860, 0.840, 290.00},
    {"Premium-P220",  "100L",  2.20,  0.885, 0.880, 0.860, 395.00},
    {"Premium-P370",  "112M",  3.70,  0.895, 0.895, 0.880, 560.00},
    {"Premium-P550",  "132S",  5.50,  0.910, 0.905, 0.890, 760.00},
    {"Premium-P750",  "132M",  7.50,  0.917, 0.915, 0.900, 1050.00},
    {"Premium-P1100", "160M", 11.00,  0.924, 0.922, 0.910, 1480.00},
    {"Premium-P1500", "160L", 15.00,  0.930, 0.930, 0.918, 1950.00}
};

double get_positive_float(const std::string& prompt, const std::string& unit) {
    double val;
    while (true) {
        std::cout << prompt << " [" << unit << "]: ";
        if (std::cin >> val && val > 0) return val;
        std::cout << "❌ Error: Must be greater than zero.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

double get_bounded_float(const std::string& prompt, const std::string& unit, double min_v, double max_v) {
    double val;
    while (true) {
        std::cout << prompt << " [" << unit << "]: ";
        if (std::cin >> val && val >= min_v && val <= max_v) return val;
        std::cout << "❌ Error: Out of bounds (" << min_v << " to " << max_v << ").\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

double calculate_mechanical_power(double torque_nm, double speed_rpm) {
    return (torque_nm * speed_rpm * 2.0 * M_PI) / (60.0 * 1000.0);
}

double estimate_part_load_efficiency(const Motor& motor, double load_factor) {
    if (load_factor >= 1.0) return motor.eff_100;
    if (load_factor >= 0.75) return motor.eff_75 + ((load_factor - 0.75) / 0.25) * (motor.eff_100 - motor.eff_75);
    if (load_factor >= 0.50) return motor.eff_50 + ((load_factor - 0.50) / 0.25) * (motor.eff_75 - motor.eff_50);
    return motor.eff_50 * (0.5 + 0.5 * (load_factor / 0.50));
}

double calculate_annual_energy_cost(double operating_power_kw, double efficiency, double hours, double rate) {
    return (operating_power_kw / efficiency) * hours * rate;
}

Motor find_suitable_motor(const std::vector<Motor>& catalog, double required_power_kw) {
    for (const auto& motor : catalog) {
        if (motor.rated_power_kw >= required_power_kw) return motor;
    }
    return Motor{"", "", 0.0, 0.0, 0.0, 0.0, 0.0};
}

void display_report(double load_p_kw, double t_nm, double rpm, const Motor& std_m, const Motor& prem_m, 
                    double std_c, double prem_c, double pur_diff, double savings) {
    std::cout << "\n============================================================\n";
    std::cout << "           MOTOR SELECTION & EFFICIENCY REPORT          \n";
    std::cout << "============================================================\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << " DESIGN LOAD PROFILE:\n";
    std::cout << "  - Target Speed             : " << rpm << " RPM\n";
    std::cout << "  - Required Shaft Torque    : " << t_nm << " N-m\n";
    std::cout << "  - Calculated Load Power   : " << load_p_kw << " kW\n-----\n";
    
    std::cout << " Selected Model Standard   : " << std_m.model << " (" << std_m.rated_power_kw << " kW)\n";
    std::cout << " Selected Model Premium    : " << prem_m.model << " (" << prem_m.rated_power_kw << " kW)\n";
    
    double std_lf = load_p_kw / std_m.rated_power_kw;
    double prem_lf = load_p_kw / prem_m.rated_power_kw;
    std::cout << " Standard Load Factor      : " << (std_lf * 100.0) << "%\n";
    std::cout << " Premium Load Factor       : " << (prem_lf * 100.0) << "%\n";
    
    std::cout << " Standard Operating Eff.   : " << (estimate_part_load_efficiency(std_m, std_lf) * 100.0) << "%\n";
    std::cout << " Premium Operating Eff.    : " << (estimate_part_load_efficiency(prem_m, prem_lf) * 100.0) << "%\n-----\n";
    
    std::cout << " Standard Annual Energy Cost: $" << std_c << "\n";
    std::cout << " Premium Annual Energy Cost : $" << prem_c << "\n";
    std::cout << "============================================================\n";
    std::cout << " FINANCIAL SUMMARY:\n";
    std::cout << "  - Annual Savings          : $" << savings << " / year\n";
    std::cout << "  - Premium Upfront Cost    : $" << pur_diff << "\n";
    if (savings > 0) std::cout << "  - Simple Payback Period   : " << (pur_diff / savings) << " years\n";
    else std::cout << "  - Simple Payback Period   : N/A\n";
    std::cout << "============================================================\n";
}

int main() {
    std::cout << "=========================================================\n";
    std::cout << "  Welcome to the Motor Selection & Efficiency Calculator\n";
    std::cout << "=========================================================\n";
    
    double req_p = get_positive_float("Enter required operational power", "kW");
    double req_t = get_positive_float("Enter maximum operational shaft torque", "N-m");
    double speed = get_bounded_float("Enter operating shaft speed", "RPM", 100.0, 5000.0);
    double hours = get_bounded_float("Enter annual runtime duration", "hours/year", 1.0, 8760.0);
    double rate  = get_positive_float("Enter commercial energy tariff", "USD/kWh");
    
    double calc_p = calculate_mechanical_power(req_t, speed);
    double design_p = std::max(req_p, calc_p);
    
    Motor std_m = find_suitable_motor(MOTOR_CATALOG, design_p);
    Motor prem_m = find_suitable_motor(PREMIUM_CATALOG, design_p);
    
    if (std_m.model.empty() || prem_m.model.empty()) {
        std::cout << "\n❌ Error: Load exceeds catalog capacity (15.0 kW).\n";
        return 1;
    }

    double std_lf = design_p / std_m.rated_power_kw;
    double prem_lf = design_p / prem_m.rated_power_kw;
    
    double std_c = calculate_annual_energy_cost(design_p, estimate_part_load_efficiency(std_m, std_lf), hours, rate);
    double prem_c = calculate_annual_energy_cost(design_p, estimate_part_load_efficiency(prem_m, prem_lf), hours, rate);
    
    display_report(design_p, req_t, speed, std_m, prem_m, std_c, prem_c, (prem_m.cost_usd - std_m.cost_usd), (std_c - prem_c));
    return 0;
}
