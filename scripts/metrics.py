import subprocess
import re
import difflib
import csv

executable = "../src/stego"
input_text_path = "../tests/TheHobbit.txt"
stego_image_path = "../output/stego.bmp"
output_text_path = "../output/text.txt"

cover_image_names = ["n01", "n02", "t01", "t02", "s01", "s02", "p01"]

metrics_executable = "../src/metrics"
metrics = ["mse", "psnr", "ssim"]

variables_values = [ { "k": k, "t": t } for k in range(1, 9) for t in [100, 370000] ] \
  + [ { "k": k, "t": t } for k in [1, 2] for t in [ 2**i for i in range(4,19)] ]

time_repetitions = 10

metrics_data_points = []
time_data_points = []
efficacy_data_points = []

with open(input_text_path, "r") as input_file:
  original_text = input_file.read()

progress = 0
total_scenarios = len(cover_image_names) * len(variables_values)

for cover_image_name in cover_image_names:
  cover_image_path = "../images/{}.bmp".format(cover_image_name)

  for variables in variables_values:
    k = variables["k"]
    t = variables["t"]
    write_command = [executable, "write", input_text_path, \
      cover_image_path, stego_image_path, str(t), str(k), "-f"]
    read_command = lambda save_output: [executable, "read", stego_image_path, \
      output_text_path if save_output else "/dev/null", str(k)]

    # Metrics of image simmilarity
    subprocess.check_output(write_command)

    for metric in metrics:
      metric_command = [metrics_executable, metric, cover_image_path, stego_image_path]
      metric_result = subprocess.check_output(metric_command)
      data_point = {
        "cover_image_name": cover_image_name,
        "k": k,
        "t": t,
        "metric": metric,
        "value": float(metric_result),
      }
      metrics_data_points.append(data_point)

    # Efficacy
    subprocess.check_output(read_command(True))
    with open(output_text_path, "r") as output_file:
      recovered_text = output_file.read()
      ratio = difflib.SequenceMatcher(None, original_text[0:t], recovered_text).ratio()
      data_point = {
        "cover_image_name": cover_image_name,
        "k": k,
        "t": t,
        "value": ratio,
      }
      efficacy_data_points.append(data_point)

    # Time measurements for writing and reading
    for i in range(0, time_repetitions):
      write_output = bytes.decode(subprocess.check_output(write_command))
      read_output = bytes.decode(subprocess.check_output(read_command(False)))

      data_points = [
        { "cover_image_name": cover_image_name, "k": k, "t": t, "command": "write", \
          "type": "processing", "value": re.findall(r'processing\s+(\d+) µs', write_output)[0] },
        { "cover_image_name": cover_image_name, "k": k, "t": t, "command": "write", \
          "type": "total", "value": re.findall(r'total\s+(\d+) µs', write_output)[0] },
        { "cover_image_name": cover_image_name, "k": k, "t": t, "command": "read", \
          "type": "processing", "value": re.findall(r'processing\s+(\d+) µs', read_output)[0] },
        { "cover_image_name": cover_image_name, "k": k, "t": t, "command": "read", \
          "type": "total", "value": re.findall(r'total\s+(\d+) µs', read_output)[0] },
      ]
      time_data_points.extend(data_points)

    progress += 100 / total_scenarios
    print("Progress: {0:.2f}%".format(progress), end="\r")

data_sets = [
  { "name": "metrics", "data_points": metrics_data_points },
  { "name": "efficacy", "data_points": efficacy_data_points },
  { "name": "time", "data_points": time_data_points },
]

for data_set in data_sets:
  data_points = data_set["data_points"]
  if len(data_points) > 0:
    headers = data_points[0].keys()
    with open("../output/{}.csv".format(data_set["name"]), "w",) as csv_file:
      writer = csv.writer(csv_file)
      writer.writerow(headers)
      for data_point in data_points:
        writer.writerow(data_point.values())
