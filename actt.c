#include <stdio.h>

//function untuk ketentuan DISTANCE
char* handle_gap(float distance) {
    if (distance < 3.5)
        return "Gogogo";
    else if (distance >= 3.5 && distance <= 10)
        return "Push";
    else
        return "Stay out of trouble";
}


//function untuk ketentuan FUEL
char* handle_fuel(int fuel_percent) {
    if (fuel_percent > 80)
        return "Push Push Push";
    else if (fuel_percent >= 50 && fuel_percent <= 80)
        return "You can go";
    else
        return "Conserve Fuel";
}

//function untuk ketentuan TIRE
char* handle_tire(int tire_wear) {
    if (tire_wear > 80)
        return "Go Push Go Push";
    else if (tire_wear >= 50 && tire_wear <= 80)
        return "Good Tire Wear";
    else if (tire_wear > 30 && tire_wear < 50)
        return "Conserve Your Tire";
    else
        return "Box Box Box";
}

//function untuk TIRE CHANGE
char* handle_tire_change(char* tire_type) {
    if (strcmp(tire_type, "Soft") == 0)
        return "Mediums Ready";
    else if (strcmp(tire_type, "Medium") == 0)
        return "Box for Softs";
    else
        return "Invalid tire type";
}
