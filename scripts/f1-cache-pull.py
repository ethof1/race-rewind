import fastf1
import os

fastf1.Cache.enable_cache('f1cache')
session = fastf1.get_session(2022, 'Monza', 'R')
session.load(telemetry=True, laps=True, weather=False)

try:
    os.remove('f1.csv')
except OSError:
    pass

# driver_list = ["HAM", "RUS", "VER", "PER", "LEC", "SAI", "NOR", "RIC", "ALO", "OCO", "STR", "VET", "GAS", "TSU", "BOT", "ZHO", "MAG", "MSC", "DEV", "LAT"]
driver_list = ["VER", "PER"]
for x in range(len(driver_list)):
    driver = driver_list[x]
    per_laps = session.laps.pick_driver(driver)
    laps_data = per_laps.get_pos_data()
    laps_data['DriverId'] = driver
    laps_data.to_csv('f1.csv', mode='a', header=False)
