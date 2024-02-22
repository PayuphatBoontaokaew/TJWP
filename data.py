import openpyxl

# ข้อมูล
data = [
    {'GyroX': -0.08, 'GyroY': 0.1, 'GyroZ': 0.0, 'Dist': 8, 'Flow Rate': 0},
    {'GyroX': -0.12, 'GyroY': 0.15, 'GyroZ': 0.02, 'Dist': 6, 'Flow Rate': 1},
    {'GyroX': -0.17, 'GyroY': 0.2, 'GyroZ': 0.03, 'Dist': 4, 'Flow Rate': 2},
    {'GyroX': -0.21, 'GyroY': 0.25, 'GyroZ': 0.05, 'Dist': 5, 'Flow Rate': 3},
    {'GyroX': -0.09, 'GyroY': 0.11, 'GyroZ': 0.01, 'Dist': 10, 'Flow Rate': 0},
    {'GyroX': -0.13, 'GyroY': 0.16, 'GyroZ': 0.04, 'Dist': 7, 'Flow Rate': 1},
    {'GyroX': -0.16, 'GyroY': 0.19, 'GyroZ': 0.02, 'Dist': 5, 'Flow Rate': 2},
    {'GyroX': -0.22, 'GyroY': 0.24, 'GyroZ': 0.06, 'Dist': 6, 'Flow Rate': 3},
    {'GyroX': -1.09, 'GyroY': 1.11, 'GyroZ': 1.01, 'Dist': 8, 'Flow Rate': 12},
    {'GyroX': -1.14, 'GyroY': 1.18, 'GyroZ': 1.05, 'Dist': 7, 'Flow Rate': 12},
    {'GyroX': -1.16, 'GyroY': 1.2, 'GyroZ': 1.03, 'Dist': 6, 'Flow Rate': 12},
    {'GyroX': -1.23, 'GyroY': 1.26, 'GyroZ': 1.07, 'Dist': 5, 'Flow Rate': 12},
    {'GyroX': -1.27, 'GyroY': 1.3, 'GyroZ': 1.09, 'Dist': 4, 'Flow Rate': 12},
    {'GyroX': -1.32, 'GyroY': 1.35, 'GyroZ': 1.12, 'Dist': 3, 'Flow Rate': 12}
]

# เปิดไฟล์ Excel
wb = openpyxl.Workbook()
ws = wb.active

# ตั้งชื่อหัวคอลัมน์
ws.append(['GyroX', 'GyroY', 'GyroZ', 'Dist', 'Flow Rate'])

# เขียนข้อมูลลงใน Excel worksheet
for row in data:
    ws.append([row['GyroX'], row['GyroY'], row['GyroZ'], row['Dist'], row['Flow Rate']])

# บันทึกไฟล์ Excel
wb.save("data.xlsx")
