import os
import subprocess

from SUETA import CURRENT_DIR, EXEC_NAME

times = ["00 00", "00 01", "00 69", "01 23", "02 28", "03 00", "04 20", "05 05", 
        "06 66", "08 30", "09 41", "10 59", "12 00", "12 34", "13 12", "15 00", 
        "20 48", "21 00", "22 80", "23 59", "24 00", "40 96"]
#times = ["{} {}".format(x // 60, x % 60) for x in range(24*60 + 1)]

for i in range(len(times)):
    sp = subprocess.run(os.path.join(CURRENT_DIR, EXEC_NAME), input=times[i], stdout=subprocess.PIPE, text=True)
    print('\033[32m', "ввод:", 
            '\033[36m', times[i], 
            '\033[32m', "вывод:" , 
            '\033[36m', sp.stdout.rstrip(), '\033[39;49m')



#     1 час
#     2 3 4 часА
#     0 5 - 20 часОВ

#     1 21 31 41 51 минутА
#     2 3 4 22-24 32-34 42-44 52-54 минутЫ
#     5 - 20 25-30 35-40 45-50 55-59 минут
#     0 ровно

#     h час(а/ов) m минут(а/ы) ночи/утра/дня/вечера
#     h часов ночи/утра/дня/вечера ровно
#     полночь/полдень
