var spawn = require('child_process').spawn;

// nc -6uv fe80::40be:73ff:fe5f:e432%tap0 8808
const proc = spawn('nc', ['-4uv', '10.0.0.111', '8808'])

proc.stdout.on('data', (data) => {
	console.log(`stdout: ${data}`);
});
  
proc.stderr.on('data', (data) => {
	console.log(`stderr: ${data}`);
});

proc.on('close', (code) => {
	console.log(`child process exited with code ${code}`);
});

process.on('SIGINT', function() {
    console.log("Caught interrupt signal");
	proc.kill()
    process.exit();
});