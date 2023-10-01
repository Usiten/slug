import os
import sys
import subprocess
import filecmp

def run_tests(file):
	if sys.platform.lower() in ['win32']:
		command = f"..\slug.exe -R {file}"
	else:
		command = f"../slug -R {file}"

	try:
		output = subprocess.check_output(command, shell=True, universal_newlines=True)
	except subprocess.CalledProcessError as e:
		print(f"[ERROR] Test {file} failed with error: {e}", file=sys.stderr)
		return False

	expected_output_file = './expected/' + file.replace(".sl", ".expected.txt")

	if not os.path.exists(expected_output_file):
		print(f"[ERROR] Expected output file {expected_output_file} not found for {file}")
		return False

	with open(expected_output_file) as f:
		expected_output = f.read()

	if expected_output == output:
		print(f".", end='')
		return 0
	else:
		print(f".", end='')
		return file

if __name__ == "__main__":
	test_folder = '.'
	test_files = [f for f in os.listdir(test_folder) if f.endswith(".sl")]

	err = []

	for test_file in test_files:
		v = run_tests(os.path.join(test_folder, test_file))
		if v != 0:
			err.append(v)
	print("\n")

	if (len(err)):
		print(f"Test failed on: {','.join(err)}")
		exit(1)
	else:
		print('All tests passed!~')
		exit(0)
