import os
import sys
import subprocess
import filecmp

def run_tests(file):
	if sys.platform.lower() in ['win32']:
		command = f"slug.exe -R {file}"
	else:
		command = f"./slug -R {file}"

	try:
		output = subprocess.check_output(command, shell=True, universal_newlines=True)
	except subprocess.CalledProcessError as e:
		return file

	expected_output_file = file.replace(".sl", ".expected.txt")
	expected_output_file = expected_output_file.replace("tests", "tests/expected")

	if not os.path.exists(expected_output_file):
		print(f"[ERROR] Expected output file {expected_output_file} not found for {file}")
		return file

	with open(expected_output_file) as f:
		expected_output = f.read()

	if expected_output == output:
		return 0
	else:
		return file

if __name__ == "__main__":
	test_folder = 'tests'
	test_files = [f for f in os.listdir(test_folder) if f.endswith(".sl")]

	err = []

	for test_file in test_files:
		v = run_tests(os.path.join(test_folder, test_file))
		if v != 0:
			err.append(v)

	if (len(err)):
		print(f"[INFO] {len(test_files) - len(err)}/{len(test_files)} tests passed")
		print(f"[ERROR] Failed on file{'s' if len(err) > 1 else ''} {', '.join(err)}")
		exit(1)
	else:
		print(f'[INFO] All tests passed!~ ({len(test_files) - len(err)}/{len(test_files)})')
		exit(0)