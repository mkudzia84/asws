import subprocess
import time
from datetime import datetime
from multiprocessing.dummy import Pool as ThreadPool

# Number of runs
NUM_RUNS = 300
# Wait between the runs in seconds
WAIT = 600

# Commands - size of it used to number of processes to spawn
soam_client_commands = [
	{ 'cmd' : "aswsclient --test-script stress-test1.xml", 'output' : "awsclient-1.out" },
	{ 'cmd' : "aswsclient --test-script stress-test1.xml", 'output' : "awsclient-2.out" },
	{ 'cmd' : "aswsclient --test-script stress-test1.xml", 'output' : "awsclient-3.out "},
	{ 'cmd' : "aswsclient --test-script stress-test1.xml", 'output' : "awsclient-4.out "}]

def soam_client_run(param):
	print("Starting : {0}".format(param['cmd']))
	with open(param['output'], 'w') as log:
		soam_client = subprocess.Popen(param['cmd'], stdout=log, shell=True)
		(output, err) = soam_client.communicate()
		# Wait
		p_status = soam_client.wait()
	
for run in range(1, NUM_RUNS):
	print("--------------------------------------------------------")
	print("{1} - Starting Run #{0}".format(run, str(datetime.now())))
	
	# Make the pool of workers
	pool = ThreadPool(len(soam_client_commands))
	
	# Start the pool
	print("Starting the clients")
	pool.map(soam_client_run, soam_client_commands)
		
	# Wait
	print ("Waiting for clients to finish")
	pool.close()
	pool.join()
	
	print("{1} - Run #{0} Finished".format(run, str(datetime.now())))
	print("Waiting for {0} seconds".format(WAIT))
	time.sleep(WAIT)

	
	
